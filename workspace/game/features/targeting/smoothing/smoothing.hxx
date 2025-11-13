#pragma once

namespace smoothing {
	class c_smoothing {
	public:
		struct curve_buffer {
			std::vector<ImVec2> points;
			float start_time;
			ImVec2 start;
			ImVec2 end;
			float distance;
			bool is_valid;
		};

		struct curve_debug_info_t {
			bool curve_active = false;
			float current_progress = 0.0f;
			int curve_points_count = 0;
		} m_curve_debug_info;

		engine::f_vector apply_hit_chance( engine::f_vector aim_location ) const {
			if ( g_weapon->m_hit_chance >= 100 )
				return aim_location;

			if ( ( rand( ) % 100 ) < g_weapon->m_hit_chance )
				return aim_location;

			const auto cam_fwd = g_camera->m_rotation.get_forward_vector( ).normalized( );
			const engine::f_vector world_up{ 0.f, 0.f, 1.f };

			auto cam_right = cam_fwd.cross( world_up );
			if ( cam_right.is_zero( ) )
				cam_right = engine::f_vector{ 1.f, 0.f, 0.f };
			cam_right = cam_right.normalized( );

			auto cam_up = cam_right.cross( cam_fwd ).normalized( );

			auto rand_unit = static_cast< float >( rand( ) ) / static_cast< float >( RAND_MAX );
			auto base_deg = 0.25f + rand_unit * 0.5f;
			auto strength = max( 0.f, min( ( 100 - g_weapon->m_hit_chance ) / 100.f, 1.f ) );
			auto miss_deg = base_deg * strength;
			if ( miss_deg < 0.01f )
				miss_deg = 0.01f;

			const double miss_rad = engine::math::to_rad( miss_deg );
			const double rand_yaw = ( ( rand( ) & 1 ) ? -1.f : 1.f ) * miss_rad;
			const double rand_pitch = ( ( rand( ) & 1 ) ? -1.f : 1.f ) * miss_rad;

			auto dir = ( aim_location - g_camera->m_location ).normalized( );
			dir += cam_right * rand_yaw + cam_up * rand_pitch;
			dir = dir.normalized( );

			const auto dist = g_camera->m_location.distance_to( aim_location );
			return g_camera->m_location + dir * dist;
		}

		engine::f_vector apply( engine::f_vector aim_location, engine::f_vector2d aim_screen ) {
			if ( !aim_screen.is_valid( ) ) {
				return aim_location;
			}

			double dx = aim_screen.x - g_overlay->m_width_center;
			double dy = aim_screen.y - g_overlay->m_height_center;

			if ( g_weapon->m_inverted_vertical_axis ) {
				dy = -dy;
			}

			auto start_point = ImVec2( g_overlay->m_width_center, g_overlay->m_height_center );
			auto end_point = ImVec2( aim_screen.x, aim_screen.y );

			// Check if we need to reset curve state for new target
			check_and_reset_for_new_target(end_point);

			if ( g_weapon->m_deadzone ) {
				auto target_distance = g_target.m_aim_distance;
				auto restriction = g_weapon->m_restriction * ( 1.0f + ( 50.0f / max( target_distance, 1.0f ) ) );
				auto distance = sqrt( dx * dx + dy * dy );
				// Reduce deadzone restriction when using curves to allow immediate curve start
				if ( g_weapon->m_aim_curve ) {
					restriction *= 0.7f; // 30% smaller deadzone for curves to enable immediate response
				}
				if ( distance < restriction ) {
					return { };
				}
			}

			if ( g_weapon->m_smoothing > 0 ) {
				if ( g_weapon->m_aim_curve ) {
					auto distance = sqrt( dx * dx + dy * dy );
					
					// Reset progress if curve just completed
					if (m_current_progress >= 1.0f) {
						m_current_progress = 0.0f;
					}
					
					// Give immediate initial progress to eliminate startup delay
					if (m_current_progress == 0.0f) {
						m_current_progress = 0.02f; // Start with 2% progress for immediate movement
					}
					
					auto curve_points = get_or_generate_curve_points(start_point, end_point, distance);
					m_render_control_points.store(std::make_shared<const std::vector<ImVec2>>(curve_points));
					
					if (!curve_points.empty()) {
						auto smoothing_modifier = g_target.m_type == target_type::weakspot ? config::weakspots::smoothing_modifier : 1.0f;
						auto effective_smoothing = g_weapon->m_smoothing - smoothing_modifier;
						
						// Faster initial progression to eliminate slow starts
						float base_speed = 1.0f / max(20.0f, effective_smoothing * 5.0f); // Increased base speed significantly
						float distance_factor = min(1.0f, distance / 200.0f); // More responsive to distance
						float progress_step = base_speed * (0.15f + distance_factor * 0.1f); // Much higher base step
						
						// Add minimum progress step to prevent slow starts
						float min_progress_step = 0.008f; // Minimum step to ensure movement isn't too slow
						progress_step = max(progress_step, min_progress_step);
						
						// Add initial acceleration boost for first few frames
						if (m_current_progress < 0.1f) {
							progress_step *= 2.5f; // 2.5x speed boost for initial movement
						} else if (m_current_progress < 0.3f) {
							progress_step *= 1.5f; // 1.5x speed boost for early movement
						}
						
						m_current_progress = min(1.0f, m_current_progress + progress_step);
						
						auto curve_point = calculate_smooth_point(curve_points, m_current_progress, start_point);
						
						// Store curve debug info for potential display
						m_curve_debug_info.current_progress = m_current_progress;
						m_curve_debug_info.curve_points_count = curve_points.size();
						m_curve_debug_info.curve_active = true;
						
						float desired_dx = curve_point.x - g_overlay->m_width_center;
						float desired_dy = curve_point.y - g_overlay->m_height_center;
						
						if (g_weapon->m_aim_acceleration) {
							// Smooth acceleration without bouncing or overshoot
							float smoothing_accel_factor = max(0.02f, min(0.08f / max(1.0f, effective_smoothing * 0.01f), 0.2f)); // More conservative acceleration
							float accel_factor = max(smoothing_accel_factor, min(g_weapon->m_acceleration * 0.003f, 0.2f)); // Reduced acceleration impact
							float momentum = min(0.98f, 0.85f + (effective_smoothing * 0.001f)); // More conservative momentum to prevent overshoot
							
							// Smooth velocity interpolation without bouncing
							m_current_velocity_x = m_current_velocity_x * momentum + 
								(desired_dx - m_current_velocity_x) * accel_factor;
							m_current_velocity_y = m_current_velocity_y * momentum + 
								(desired_dy - m_current_velocity_y) * accel_factor;
							
							float vel_magnitude = sqrt(m_current_velocity_x * m_current_velocity_x + 
								m_current_velocity_y * m_current_velocity_y);
							
							// Gentle velocity capping without sudden transitions that could cause bouncing
							if (vel_magnitude > g_weapon->m_max_velocity && vel_magnitude > 0.001f) {
								float scale = g_weapon->m_max_velocity / vel_magnitude;
								// Smooth scaling without abrupt transitions
								scale = max(0.8f, scale); // Prevent too aggressive velocity reduction
								
								m_current_velocity_x *= scale;
								m_current_velocity_y *= scale;
							}
							
							dx = m_current_velocity_x;
							dy = m_current_velocity_y;
						} else {
							dx = desired_dx;
							dy = desired_dy;
						}
						
						auto max_step = 100.0f / (effective_smoothing * 1.5f + 1.0f);
						auto final_distance = sqrt(dx * dx + dy * dy);
						if (final_distance > max_step) {
							float scale = max_step / final_distance;
							float smooth_scale = 1.0f - pow(1.0f - scale, 2.0f);
							dx *= smooth_scale;
							dy *= smooth_scale;
						}
					}

					dx = max((double)-100.0, min(dx, (double)100.0));
					dy = max((double)-100.0, min(dy, (double)100.0));
				} else {
					reset_curve_state();
					m_render_control_points.store(nullptr);
					m_curve_debug_info.curve_active = false;

					auto smoothing_modifier = g_target.m_type == target_type::weakspot ? config::weakspots::smoothing_modifier : 1.0f;
					auto max_step = 100.0f / ((g_weapon->m_smoothing) + 1.0f) - smoothing_modifier;
					auto distance = sqrt(dx * dx + dy * dy);
					if (distance > max_step) {
						dx = (dx / distance) * max_step;
						dy = (dy / distance) * max_step;
					}

					dx = max((double)-max_step, min(dx, (double)max_step));
					dy = max((double)-max_step, min(dy, (double)max_step));
				}
			} else {
				reset_curve_state();
				m_render_control_points.store(nullptr);
				m_curve_debug_info.curve_active = false;
			}

			auto smoothed_screen = engine::f_vector2d(
				g_overlay->m_width_center + dx,
				g_overlay->m_height_center + dy
			);

			auto aim_direction = engine::deproject(smoothed_screen);
			auto aim_distance = g_camera->m_location.distance_to(aim_location);
			return g_camera->m_location + (aim_direction * aim_distance);
		}

		void reset() {
			reset_curve_state();
			std::lock_guard<std::mutex> lock(m_curve_mutex);
			m_curve_buffers.clear();
			m_render_control_points.store(nullptr);
			m_last_target_position = ImVec2(-1000, -1000); // Reset target tracking
		}

		std::atomic<std::shared_ptr<const std::vector<ImVec2>>> m_render_control_points;

	private:
		void check_and_reset_for_new_target(const ImVec2& current_target) {
			// Calculate distance moved since last target
			float distance_moved = ImLengthSqr(ImVec2(
				current_target.x - m_last_target_position.x,
				current_target.y - m_last_target_position.y
			));
			
			// Reset progress for new targets or when curve is complete
			if (distance_moved > 5000.0f || m_current_progress >= 0.99f) { // Much lower threshold to detect target changes
				m_current_progress = 0.0f; // Reset progress for new curve
				// Very gentle velocity preservation to prevent bouncing on target changes
				m_current_velocity_x *= 0.6f; // More gentle velocity dampening to prevent abrupt changes
				m_current_velocity_y *= 0.6f;
			}
			
			m_last_target_position = current_target;
		}

		void reset_curve_state() {
			// Reset progress to start fresh curve
			m_current_progress = 0.0f; // Always start fresh for new curves
			// Very gentle velocity reset that preserves smoothness and prevents bouncing
			m_current_velocity_x *= 0.5f; // More preservation to prevent abrupt changes
			m_current_velocity_y *= 0.5f;
			
			// Less frequent direction changes for smoother motion without bouncing
			if (m_direction_change_counter++ % 40 == 0) { // Even less frequent changes to prevent erratic movement
				m_positive_curve = !m_positive_curve;
			}
		}

		std::vector<ImVec2> get_or_generate_curve_points(const ImVec2& start, const ImVec2& end, float distance) {
			// Immediate curve generation without delays
			std::vector<ImVec2> new_points;
			
			// Use different curve patterns based on distance and settings
			if (distance < 50.0f) {
				// For very short distances, use simpler curves
				new_points = curves::generate_smooth_curve(m_positive_curve, start, end, distance, g_weapon->m_smoothing);
			} else {
					new_points = curves::generate_smooth_curve(m_positive_curve, start, end, distance, g_weapon->m_smoothing);
					m_pattern_switch_counter++;
			}
			
			return new_points;
		}

		std::vector<ImVec2> generate_advanced_curve(const ImVec2& start, const ImVec2& end, float distance) {
			std::vector<ImVec2> points;
			points.reserve(20); // Reduced points for simpler, smoother curves
			
			ImVec2 direction = ImVec2(end.x - start.x, end.y - start.y);
			ImVec2 perp = ImVec2(-direction.y, direction.x);
			float perp_length = sqrt(perp.x * perp.x + perp.y * perp.y);
			if (perp_length > 1e-6f) {
				perp.x /= perp_length;
				perp.y /= perp_length;
			}
			
			// Simple curve strength without bouncing
			float base_strength = min(distance * 1.5f, 300.0f); // Reduced strength for smoother curves
			float smoothing_factor = max(0.5f, g_weapon->m_smoothing / 20.0f); // Gentler smoothing factor
			float curve_strength = base_strength * smoothing_factor * 1.0f; // Simple, non-bouncing strength
			
			points.push_back(start);
			
			// Fewer control points for smooth, non-bouncing curves
			int num_points = std::clamp(static_cast<int>(distance / 20.0f), 8, 20); // Reduced complexity
			for (int i = 1; i < num_points; i++) {
				float t = static_cast<float>(i) / num_points;
				
				// Single smooth sine wave without any bouncing or oscillations
				float curve_factor = sin(t * 3.14159265358979323846f); // Simple sine curve only
				
				// No damping - maintain consistent curve strength throughout
				
				// No overshoots, no bouncing - just smooth curve
				
				// Simple amplitude without extreme variations
				float amplitude = curve_strength * curve_factor;
				
				// Consistent curve direction without alternating patterns
				if (m_positive_curve) {
					amplitude = abs(amplitude); // Always positive curve
				} else {
					amplitude = -abs(amplitude); // Always negative curve
				}
				
				ImVec2 point;
				point.x = start.x + direction.x * t + perp.x * amplitude;
				point.y = start.y + direction.y * t + perp.y * amplitude;
				points.push_back(point);
			}
			
			points.push_back(end);
			return points;
		}

		ImVec2 calculate_smooth_point(const std::vector<ImVec2>& points, float t, const ImVec2& default_pos) {
			if (points.empty()) return default_pos;
			if (points.size() == 1) return points[0];
			
			// Light easing that preserves curve shape
			float smoothed_t = t;
			
			// Single layer of gentle smoothstep to preserve curve path
			smoothed_t = smoothed_t * smoothed_t * (3.0f - 2.0f * smoothed_t); // Simple smoothstep
			
			// Ensure t stays in valid range
			smoothed_t = max(0.0f, min(1.0f, smoothed_t));
			
			// Get the curve point directly without temporal averaging
			return curves::calculate_bezier_point(points, smoothed_t, default_pos);
		}

		float m_current_velocity_x = 0.0f;
		float m_current_velocity_y = 0.0f;
		float m_current_progress = 0.0f;
		bool m_positive_curve = false;
		int m_pattern_switch_counter = 0;
		int m_direction_change_counter = 0; // New counter for curve direction changes
		ImVec2 m_last_target_position = ImVec2(-1000, -1000); // Track target position changes
		
		std::mutex m_curve_mutex;
		std::deque<curve_buffer> m_curve_buffers;
	};
}