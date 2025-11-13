#pragma once

namespace ring {
	class c_ring {
	private:
		struct ring_history {
			std::deque<engine::f_vector> positions;
			std::deque<engine::f_vector> velocities;
			float animation_progress = 0.0f;
			engine::f_vector last_position = {0, 0, 0};
		};

		std::map<void*, ring_history> m_player_ring_data;

		const float m_animation_speed = 0.0001f;
		const int m_trail_length = 300;
		const int m_segments = 36;
		const float m_line_thickness = 5.0f;
		// Colors will be determined from c namespace gradient

		float m_smoothed_vertical_velocity = 0.0f;
		float m_smoothed_trail_direction = 1.0f;
		float m_trail_anchor_offset = 0.0f;
		float m_last_direction = 1.0f;

	public:
		c_ring( ) = default;
		~c_ring( ) = default;

		void render( const lists::c_player& player ) {
			auto& ring_data = m_player_ring_data[ player.m_actor ];

			ring_data.animation_progress += m_animation_speed;
			if ( ring_data.animation_progress > 1.0f ) {
				ring_data.animation_progress = 0.0f;
			}

			auto head_location = player.m_head_location;
			auto root_location = player.m_base_location;

			float vertical_oscillation = 0.5f * ( 1.0f + sin( ring_data.animation_progress * 2.0f * 3.1415926535f ) );
			engine::f_vector ring_center = root_location.lerp( head_location, vertical_oscillation );

			engine::f_vector velocity = {0, 0, 0};
			if (ring_data.last_position.x != 0 || ring_data.last_position.y != 0 || ring_data.last_position.z != 0) {
				velocity = ring_center - ring_data.last_position;
			}

			ring_data.positions.push_front( ring_center );
			ring_data.velocities.push_front( velocity );
			ring_data.last_position = ring_center;

			while ( ring_data.positions.size( ) > m_trail_length ) {
				ring_data.positions.pop_back( );
				ring_data.velocities.pop_back( );
			}

			draw_ring_at_position( ring_center, player.m_bounds.box_extent.x * 1.5f );
			
			draw_velocity_trail( ring_data.positions, ring_data.velocities, player.m_bounds.box_extent.x * 1.5f );
		}

		void cleanup_player( void* actor ) {
			m_player_ring_data.erase( actor );
		}

		void clear_all( ) {
			m_player_ring_data.clear( );
		}

	private:
		void draw_ring_at_position( engine::f_vector center, float radius ) {
			auto center_2d = engine::project( center );
			if ( !center_2d.is_valid() ) return;

			// Use main color from c namespace for the center ring
			ImColor ring_color = c::main_color;
			
			// Draw shadow circle for optimized performance
			g_foreground->AddShadowCircle(
				center_2d.vec(),
				radius * 0.5f,  // Adjust radius for screen space
				ring_color,
				20.0f,  // Shadow radius
				ImVec2( 0, 0 ),  // No offset
				0.0f  // No rounding
			);
		}

		void draw_velocity_trail( const std::deque<engine::f_vector>& positions, const std::deque<engine::f_vector>& velocities, float radius ) {
			if ( positions.size( ) < 2 ) return;

			engine::f_vector current_pos = positions[0];
			
			float actual_vertical_velocity = 0.0f;
			int velocity_samples = min(8, static_cast<int>(velocities.size()));
			
			for ( int i = 0; i < velocity_samples; ++i ) {
				actual_vertical_velocity += velocities[i].z;
			}
			
			if (velocity_samples > 0) {
				actual_vertical_velocity /= velocity_samples;
			}

			const float velocity_smoothing_factor = 0.08f;
			m_smoothed_vertical_velocity += (actual_vertical_velocity - m_smoothed_vertical_velocity) * velocity_smoothing_factor;

			float current_direction = (actual_vertical_velocity > 0) ? -1.0f : 1.0f;
			
			bool direction_changed = (current_direction != m_last_direction) && (abs(actual_vertical_velocity) > 0.0001f);
			
			if (direction_changed) {
				float smoothed_velocity_magnitude = abs(m_smoothed_vertical_velocity);
				float velocity_factor = smoothed_velocity_magnitude * 10000.0f;
				velocity_factor = max(0.05f, min(3.5f, velocity_factor));
				
				float trail_extent = (60.0f * velocity_factor) * (0.2f + velocity_factor * 0.2f);
				
				m_trail_anchor_offset += m_last_direction * trail_extent;
				
				m_last_direction = current_direction;
			}
			
			const float direction_smoothing_factor = 0.15f;
			m_smoothed_trail_direction += (current_direction - m_smoothed_trail_direction) * direction_smoothing_factor;
			
			float smoothed_velocity_magnitude = abs(m_smoothed_vertical_velocity);
			float velocity_factor = smoothed_velocity_magnitude * 10000.0f;
			velocity_factor = max(0.05f, min(3.5f, velocity_factor));
			
			int base_trail_length = static_cast<int>(m_trail_length * 0.6f * velocity_factor);
			base_trail_length = max(10, min(m_trail_length, base_trail_length));
			
			float trail_spacing = 0.15f + (velocity_factor * 0.15f);
			float velocity_alpha_boost = 0.2f + (velocity_factor * 0.45f);
			
			float min_z = positions[0].z;
			float max_z = positions[0].z;
			for (const auto& pos : positions) {
				if (pos.z < min_z) min_z = pos.z;
				if (pos.z > max_z) max_z = pos.z;
			}
			
			const float anchor_return_speed = 0.02f;
			if (!direction_changed && abs(m_trail_anchor_offset) > 0.1f) {
				m_trail_anchor_offset *= (1.0f - anchor_return_speed);
			}
			
			const int vertical_step = 1;

			// Reduce trail length for better performance with shadow circles
			int optimized_trail_length = min(base_trail_length, 20);
			
			for ( int i = 1; i < optimized_trail_length; i += 2 ) {  // Skip every other for performance
				
				float distance = static_cast<float>(i) * trail_spacing * 2.0f;  // Adjust spacing
				engine::f_vector trail_center = current_pos + engine::f_vector(0, 0, m_trail_anchor_offset + m_smoothed_trail_direction * distance);

				trail_center.z = max(min_z, min(max_z, trail_center.z));

				auto trail_center_2d = engine::project( trail_center );
				if ( !trail_center_2d.is_valid() ) continue;

				float progress = static_cast<float>(i) / optimized_trail_length;
				float alpha_factor = 1.0f - progress;
				alpha_factor = alpha_factor * alpha_factor * velocity_alpha_boost;
				
				// Create gradient between main_color and second_color
				ImVec4 main_color_vec = c::main_color.Value;
				ImVec4 second_color_vec = c::second_color.Value;
				
				// Lerp between main color (start) and second color (end)
				ImVec4 gradient_color = ImVec4(
					main_color_vec.x + (second_color_vec.x - main_color_vec.x) * progress,
					main_color_vec.y + (second_color_vec.y - main_color_vec.y) * progress,
					main_color_vec.z + (second_color_vec.z - main_color_vec.z) * progress,
					alpha_factor * 0.8f  // Apply alpha
				);
				
				ImColor trail_color = ImColor( gradient_color );
				
				// Draw optimized shadow circle instead of many polygons
				float trail_radius = radius * (1.0f - progress * 0.5f);  // Shrink as we go back
				float shadow_strength = 15.0f * alpha_factor;
				
				g_foreground->AddShadowCircle(
					trail_center_2d.vec(),
					trail_radius * 0.3f,  // Smaller radius for trail
					trail_color,
					shadow_strength,
					ImVec2( 0, 0 ),
					0.0f
				);
			}
		}
	};
}