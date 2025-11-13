#pragma once

class c_notifications
{
public:
	void add_message( const std::string& icon, const std::string& msg, const ImVec4& icon_color, ImVec4 icon_colorb = ImVec4( ), bool use_gradient = false ) {
		// Initialize current_y to start off-screen (top-right)
		float initial_y = 600.0f;
		// Initialize current_x to start off-screen (right side)
		float initial_x = g_overlay->m_width_center;
		m_notifications.push_back( { icon, msg, 0.0f, 0.0f, false, 3.0f, initial_y, initial_x, icon_color, icon_colorb, use_gradient } );
	}

	void render( ) {
		ImGui::SetNextWindowSize( ImGui::GetWindowSize( ) );
		ImGui::Begin( "Frame", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus );
		{
			const float padding = 14.0f;
			const float right_padding = 10.0f;
			const ImVec2 viewport_pos = ImGui::GetMainViewport( )->WorkPos;
			const ImVec2 viewport_size = ImGui::GetMainViewport( )->Size;
			float total_offset = 0.0f;

			// Vertical layout constants
			const float notif_height = 30.0f;   // matches rect height
			const float notif_gap = 8.0f;       // spacing between notifications
			const float top_margin = 90.0f;     // distance from top edge

			for ( int i = 0; i < m_notifications.size( ); ++i ) {
				c_notification& note = m_notifications[ i ];
				float delta = ImGui::GetIO( ).DeltaTime;
				note.timer += delta;

				// Fading animation with proper timing
				if ( !note.fading_out ) {
					// Fade in phase
					if (note.timer < 0.3f) { // First 0.3 seconds for fade in
						note.alpha = ImClamp( note.timer / 0.3f, 0.0f, 1.0f );
					} else if (note.timer >= note.duration) {
						// Start fading out after duration
						note.fading_out = true;
						note.timer = 0.0f; // Reset timer for fade out
					} else {
						// Keep full opacity while visible
						note.alpha = 1.0f;
					}
				} else {
					// Fade out phase - like the original
					note.timer += delta;
					note.alpha = ImClamp( note.alpha - delta * 2.0f, 0.0f, 1.0f ); // Original fade out speed
					
					// Remove notification after fade out completes
					if (note.alpha <= 0.0f) {
						m_notifications.erase( m_notifications.begin( ) + i );
						--i;
						continue;
					}
				}

				ImGui::PushFont( awesome_font::regular );
				ImVec2 icon_size = ImGui::CalcTextSize( note.icon.c_str( ) );
				ImGui::PopFont( );

				ImGui::PushFont( font::s_inter_semibold );
				ImVec2 text_size = ImGui::CalcTextSize( note.message.c_str( ) );
				ImGui::PopFont( );

				// Calculate notification width and centered X
				float notification_width = 60 + text_size.x + icon_size.x;
				float target_x = (viewport_size.x - notification_width) / 2.0f; // Center horizontally
				
				// Consistent vertical spacing by index
				float target_y = top_margin + i * (notif_height + notif_gap);
				
				// Update current position with smooth animation
				note.current_y += (target_y - note.current_y) * delta * 10.0f;
				
				// Keep X centered; animate only slight easing to target_x
				note.current_x += (target_x - note.current_x) * delta * 10.0f;
				
				// During fade-out, start moving upward (out of frame) once halfway faded
				if (note.fading_out && note.alpha < 0.5f) {
					float slide_out_y = -60.0f; // move above screen
					note.current_y += (slide_out_y - note.current_y) * delta * 8.0f;
				}

				ImVec2 pos( note.current_x, note.current_y );
				
				// Use the same dimensions and styling as reference code
				ImVec2 rect_start = pos + ImVec2(20, 0);
				ImVec2 rect_end = pos + ImVec2(70, 30) + ImVec2(text_size.x, 0) + ImVec2(icon_size.x, 0);
				
				// Background rectangle
				g_background->AddRectFilled( 
					rect_start, 
					rect_end, 
					ImColor( 16, 16, 19, int( 255 * note.alpha ) ), 
					4.0f
				);
				
				g_background->AddRect( 
					rect_start, 
					rect_end, 
					ImColor( 40, 45, 65, int( 255 * note.alpha)),
					4.0f 
				);

				// Icon text - positioned on the left side like in screenshot
				ImGui::PushFont( awesome_font::small_font );
				if ( note.use_gradient ) {
					custom::fade_text(
						g_background,
						note.icon.c_str( ),
						ImVec2( pos.x + 30, pos.y + 7.5f ),
						ImColor( note.icon_color.x, note.icon_color.y, note.icon_color.z, note.alpha ),
						ImColor( note.icon_color_b.x, note.icon_color_b.y, note.icon_color_b.z, note.alpha ),
						0
					);
				}
				else {
					g_background->AddText(
						ImVec2( pos.x + 30, pos.y + 7.5f ),
						ImColor( note.icon_color.x, note.icon_color.y, note.icon_color.z, note.alpha ),
						note.icon.c_str( )
					);
				}
				ImGui::PopFont( );

				// Vertical separator line - positioned after the icon
				float line_x = pos.x + 30 + icon_size.x + 15.0f;
				float line_y_start = pos.y + 5.0f;
				float line_y_end = pos.y + (notif_height - 5.0f); // match box height
				g_background->AddLine(
					ImVec2( line_x, line_y_start ),
					ImVec2( line_x, line_y_end ),
					ImColor( 40, 45, 65, int( 255 * note.alpha ) ),
					1.0f
				);

				// Main text - positioned to the right of the separator line like in screenshot
				ImGui::PushFont( font::s_inter_semibold );
				g_background->AddText(
					ImVec2( line_x + 14, pos.y + 6.5f ),
					ImColor( 150, 150, 150, int( 255 * note.alpha ) ),
					note.message.c_str( )
				);
				ImGui::PopFont( );

				// No per-item accumulation; vertical order handled by index-based spacing
			}
		}
		ImGui::End( );
	}

private:
	struct c_notification {
		std::string icon;           // Icon comes first
		std::string message;        // Message comes second
		float alpha = 0.0f;
		float timer = 0.0f;
		bool fading_out = false;
		float duration = 3.0f;
		float current_y = 0.0f; // Only keep current_y for smooth animation
		float current_x = 0.0f; // Add current_x for horizontal slide animation
		ImVec4 icon_color = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
		ImVec4 icon_color_b;
		bool use_gradient;
	};

	std::vector<c_notification> m_notifications;
};