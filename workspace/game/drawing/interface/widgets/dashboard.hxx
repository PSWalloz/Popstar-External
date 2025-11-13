
namespace dashboard {
	inline std::time_t g_last_leaderboard_refresh = 0;
	inline std::string currentTime12h()
	{
		std::time_t now = std::time(nullptr);
		std::tm* local_tm{};

		local_tm =  localtime(&now);

		int hour24 = local_tm->tm_hour;
		int minute = local_tm->tm_min;
		bool is_pm = hour24 >= 12;

		int hour12 = hour24 % 12;
		if (hour12 == 0) hour12 = 12;

		std::string hh = ( hour12 < 10 ) ? ( std::string( "0" ) + std::to_string( hour12 ) ) : std::to_string( hour12 );
		std::string mm = ( minute < 10 ) ? ( std::string( "0" ) + std::to_string( minute ) ) : std::to_string( minute );
		return hh + ":" + mm + ( is_pm ? " PM" : " AM" );
	}
	
	class c_window {
	private:
		std::string m_title;
		const char* m_icon;
		bool m_is_enabled;
		bool m_should_close;
		bool m_once_set_pos;
		float m_corner_radius;
		ImVec2 m_window_padding;
		ImVec2 m_target_size;

		// Animation state structure - based on custom_popup pattern
		struct window_animation_state {
			float target_height;
			float current_height;
			float target_alpha;
			float current_alpha;
			float target_width;
			float current_width;
			bool is_closing;
			bool is_opening;
			ImVec2 initial_position;
		};

		// Button state for interactive elements
		struct window_button_state {
			ImVec4 bg_color;
			ImVec4 text_color;
			ImVec2 text_offset;
			float press_anim;
			float hover_anim;
			float gradient_alpha;
		};

		window_animation_state m_anim_state;
		std::map<ImGuiID, window_button_state> m_button_states;

	public:
		c_window( const std::string& title, const char* icon = ICON_STRIPE_FILL )
			: m_title( title ), m_icon( icon ), m_is_enabled( false ), m_should_close( false ),
			m_once_set_pos( false ), m_corner_radius( 20.0f ), m_window_padding( ImVec2( 12, 12 ) ),
			m_target_size( ImVec2( 670, 550 ) ) {

			// Initialize animation state
			m_anim_state.target_height = 550.0f;
			m_anim_state.current_height = 10.0f;
			m_anim_state.target_width = 600.0f;
			m_anim_state.current_width = 10.0f;
			m_anim_state.target_alpha = 1.0f;
			m_anim_state.current_alpha = 0.0f;
			m_anim_state.is_closing = false;
			m_anim_state.is_opening = false;
			m_anim_state.initial_position = ImVec2( 0, 0 );
		}

		void set_size( const ImVec2& size ) {
			m_target_size = size;
			m_anim_state.target_height = size.y;
			m_anim_state.target_width = size.x;
		}
		void set_corner_radius( float radius ) { m_corner_radius = radius; }
		void set_padding( const ImVec2& padding ) { m_window_padding = padding; }

		void open( ) {
			if ( !m_is_enabled ) {
				m_is_enabled = true;
				m_should_close = false;
				m_once_set_pos = false;
				m_anim_state.is_closing = false;
				m_anim_state.is_opening = false;
				// Snap to target instantly (no animation)
				m_anim_state.target_height = m_target_size.y;
				m_anim_state.target_width = m_target_size.x;
				m_anim_state.target_alpha = 1.0f;
				m_anim_state.current_alpha = 1.0f;
				m_anim_state.current_height = m_target_size.y;
				m_anim_state.current_width = m_target_size.x;
				// Calculate safe position within viewport bounds
				ImVec2 mouse_pos = ImGui::GetMousePos( );
				ImVec2 viewport_size = ImGui::GetIO( ).DisplaySize;
				float edge_padding = 50.0f;
				ImVec2 desired_center = mouse_pos;
				ImVec2 window_half_size = m_target_size * 0.5f;
				ImVec2 top_left = ImVec2(
					desired_center.x - window_half_size.x,
					desired_center.y - window_half_size.y
				);
				float safe_x = ImClamp( top_left.x, edge_padding, viewport_size.x - m_target_size.x - edge_padding );
				float safe_y = ImClamp( top_left.y, edge_padding, viewport_size.y - m_target_size.y - edge_padding );
				m_anim_state.initial_position = ImVec2( safe_x, safe_y );
			}
		}

		void close( ) {
			// Close instantly (no animation)
			m_is_enabled = false;
			m_should_close = false;
			m_anim_state.is_closing = false;
			m_anim_state.is_opening = false;
		}

		bool is_open( ) const {
			return m_is_enabled;
		}

		bool is_closing( ) const {
			return m_anim_state.is_closing;
		}

		bool begin( const char* custom_title = nullptr ) {
			if ( !m_is_enabled ) {
				return false;
			}

			const char* window_title = custom_title ? custom_title : m_title.c_str( );

			// No animations: force static styles and sizes
			ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, m_corner_radius );
			ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, m_window_padding );
			ImGui::PushStyleVar( ImGuiStyleVar_Alpha, 1.0f );
			ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0 );
			ImGui::SetNextWindowSize( ImVec2( m_target_size.x, m_target_size.y ), ImGuiCond_Always );

			if ( !m_once_set_pos ) {
				// Double-check bounds on first positioning
				ImVec2 viewport_size = ImGui::GetIO( ).DisplaySize;
				float edge_padding = 50.0f;

				ImVec2 corrected_pos = ImVec2(
					ImClamp( m_anim_state.initial_position.x, edge_padding, viewport_size.x - m_anim_state.current_width - edge_padding ),
					ImClamp( m_anim_state.initial_position.y, edge_padding, viewport_size.y - m_anim_state.current_height - edge_padding )
				);

				ImGui::SetNextWindowPos( corrected_pos );
				m_once_set_pos = true;
			}

			bool is_window_open = ImGui::Begin( window_title, &m_is_enabled,
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoBackground |
				ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_AlwaysUseWindowPadding );

			return is_window_open;
		}

		void end( ) {
			ImGui::End( );
			ImGui::PopStyleVar( 4 );
		}

	public:
		void render_settings( ) {
			ImVec2 window_pos = ImGui::GetWindowPos( );
			ImVec2 window_size = ImGui::GetWindowSize( );

			// Add small padding to ensure rounded corners are fully visible
			float corner_padding = m_corner_radius * 0.1f; // Small padding relative to corner radius
			ImVec2 bg_pos = ImVec2( window_pos.x + corner_padding, window_pos.y + corner_padding );
			ImVec2 bg_size = ImVec2( window_size.x - corner_padding * 2.0f, window_size.y - corner_padding * 2.0f );

			ImGui::GetForegroundDrawList( )->AddShadowRect(
				bg_pos,
				bg_pos + bg_size,
				ImColor( 135, 122, 233, 100 ),
				25.0f,
				ImVec2( 0.0f, 0.0f ),
				c::bg::rounding
			);

			// Draw main window background FIRST
			ImGui::GetForegroundDrawList( )->AddRectFilled( bg_pos, bg_pos + bg_size,
				c::window_bg_color, m_corner_radius );

			ImGui::GetForegroundDrawList( )->AddRectFilled( bg_pos, bg_pos + bg_size,
				c::widget_bg_color, m_corner_radius );

			ImGui::GetForegroundDrawList( )->PushClipRect( bg_pos + ImVec2( 5, 5 ), bg_pos + bg_size - ImVec2( 5, 5 ), true );

			{
				const float line_width = 1.0f;
				const float line_spacing = 40.0f;  // Increased spacing
				const float dash_length = 4.0f;
				const float gap_length = 3.0f;
				const ImU32 line_color = ImColor( 40, 45, 65, 200 );

				auto box_max = bg_pos + bg_size;

				float box_width = box_max.x - bg_pos.x;
				float box_height = box_max.y - bg_pos.y;

				// Draw dashed horizontal lines
				for ( float y = bg_pos.y; y <= box_max.y; y += line_spacing ) {
					for ( float x = bg_pos.x; x <= box_max.x; x += dash_length + gap_length ) {
						float dash_end = ImMin( x + dash_length, box_max.x );
						if ( dash_end > x ) {
							ImVec2 line_start = ImVec2( x, y );
							ImVec2 line_end = ImVec2( dash_end, y );
							ImGui::GetForegroundDrawList( )->AddLine( line_start, line_end, line_color, line_width );
						}
					}
				}

				// Draw dashed vertical lines
				for ( float x = bg_pos.x; x <= box_max.x; x += line_spacing ) {
					for ( float y = bg_pos.y; y <= box_max.y; y += dash_length + gap_length ) {
						float dash_end = ImMin( y + dash_length, box_max.y );
						if ( dash_end > y ) {
							ImVec2 line_start = ImVec2( x, y );
							ImVec2 line_end = ImVec2( x, dash_end );
							ImGui::GetForegroundDrawList( )->AddLine( line_start, line_end, line_color, line_width );
						}
					}
				}
			}

			ImGui::GetForegroundDrawList( )->PopClipRect( );

			// Window outline last
			ImGui::GetForegroundDrawList( )->AddRect( bg_pos, bg_pos + bg_size,
				ImColor( 60, 54, 82, 255 ), m_corner_radius, 0, 2.0f );

			// Add top-left positioned Settings text
			ImGui::PushFont( awesome_font::bold_font );
			const char* leaderboard_text = "Settings";
			ImVec2 leaderboard_size = ImGui::CalcTextSize( leaderboard_text );
			ImVec2 hello_text_pos = ImVec2(
				bg_pos.x + 25.0f,  // 25px from left edge
				bg_pos.y + 20.0f  // 20px from top
			);
			custom::fade_text( ImGui::GetForegroundDrawList( ), leaderboard_text, hello_text_pos, c::text::description::default_color, c::anim::active, 0 );

			// Add underline below settings text using description color
			float settings_underline_y = hello_text_pos.y + leaderboard_size.y + 5.0f;  // 5px padding from text
			float settings_underline_thickness = 2.0f;
			ImVec2 settings_underline_start = ImVec2( hello_text_pos.x, settings_underline_y );
			ImVec2 settings_underline_end = ImVec2( hello_text_pos.x + leaderboard_size.x, settings_underline_y );
			
			// Use the text description color instead of main color
			custom::fade_line( ImGui::GetForegroundDrawList( ), settings_underline_start, settings_underline_end, c::anim::active, c::text::description::default_color, settings_underline_thickness );

			ImGui::PopFont( );

			// Add two rounded rectangles under the leaderboard text
			float rect_start_x = bg_pos.x + 25.0f;  // 20px spacing from start
			float rect_end_x = bg_pos.x + bg_size.x - 20.0f;  // End at max size minus 20
			float rect_y = hello_text_pos.y + ImGui::CalcTextSize( "Settings" ).y + 25.0f;  // 15px below text
			float rect_end_y = bg_pos.y + bg_size.y - 23.0f;
			float spacing_between = 20.0f;

			float available_width = rect_end_x - rect_start_x;
			float rect_width = ( available_width - spacing_between ) * 0.5f;

			ImVec2 rect1_min = ImVec2( rect_start_x, rect_y );
			ImVec2 rect1_max = ImVec2( rect_start_x + rect_width, rect_end_y );

			ImGui::GetForegroundDrawList( )->AddRectFilled(
				rect1_min,
				rect1_max,
				ImColor( 19, 20, 34, 200 ),
				c::child::rounding + 10
			);

			ImGui::GetForegroundDrawList( )->AddRect(
				rect1_min,
				rect1_max,
				c::outline_color,
				c::child::rounding + 10
			);

			// ImColor( 15, 11, 35, 150 )


		}

		void render_giveaway( ) {
			ImVec2 window_pos = ImGui::GetWindowPos( );
			ImVec2 window_size = ImGui::GetWindowSize( );

			// Add small padding to ensure rounded corners are fully visible
			float corner_padding = m_corner_radius * 0.1f; // Small padding relative to corner radius
			ImVec2 bg_pos = ImVec2( window_pos.x + corner_padding, window_pos.y + corner_padding );
			ImVec2 bg_size = ImVec2( window_size.x - corner_padding * 2.0f, window_size.y - corner_padding * 2.0f );

			ImGui::GetForegroundDrawList( )->AddShadowRect(
				bg_pos,
				bg_pos + bg_size,
				ImColor( 135, 122, 233, 100 ),
				25.0f,
				ImVec2( 0.0f, 0.0f ),
				c::bg::rounding
			);

			// Draw main window background FIRST
			ImGui::GetForegroundDrawList( )->AddRectFilled( bg_pos, bg_pos + bg_size,
				c::window_bg_color, m_corner_radius );

			ImGui::GetForegroundDrawList( )->AddRectFilled( bg_pos, bg_pos + bg_size,
				c::widget_bg_color, m_corner_radius );

			ImGui::GetForegroundDrawList( )->PushClipRect( bg_pos + ImVec2( 5, 5 ), bg_pos + bg_size - ImVec2( 5, 5 ), true );

			{
				const float line_width = 1.0f;
				const float line_spacing = 40.0f;  // Increased spacing
				const float dash_length = 4.0f;
				const float gap_length = 3.0f;
				const ImU32 line_color = ImColor( 40, 45, 65, 200 );

				auto box_max = bg_pos + bg_size;

				float box_width = box_max.x - bg_pos.x;
				float box_height = box_max.y - bg_pos.y;

				// Draw dashed horizontal lines
				for ( float y = bg_pos.y; y <= box_max.y; y += line_spacing ) {
					for ( float x = bg_pos.x; x <= box_max.x; x += dash_length + gap_length ) {
						float dash_end = ImMin( x + dash_length, box_max.x );
						if ( dash_end > x ) {
							ImVec2 line_start = ImVec2( x, y );
							ImVec2 line_end = ImVec2( dash_end, y );
							ImGui::GetForegroundDrawList( )->AddLine( line_start, line_end, line_color, line_width );
						}
					}
				}

				// Draw dashed vertical lines
				for ( float x = bg_pos.x; x <= box_max.x; x += line_spacing ) {
					for ( float y = bg_pos.y; y <= box_max.y; y += dash_length + gap_length ) {
						float dash_end = ImMin( y + dash_length, box_max.y );
						if ( dash_end > y ) {
							ImVec2 line_start = ImVec2( x, y );
							ImVec2 line_end = ImVec2( x, dash_end );
							ImGui::GetForegroundDrawList( )->AddLine( line_start, line_end, line_color, line_width );
						}
					}
				}
			}

			ImGui::GetForegroundDrawList( )->PopClipRect( );

			// Window outline last
			ImGui::GetForegroundDrawList( )->AddRect( bg_pos, bg_pos + bg_size,
				ImColor( 60, 54, 82, 255 ), m_corner_radius, 0, 2.0f );

			// Create a custom clipped glow effect using multiple circles
			ImDrawList* draw_list = ImGui::GetForegroundDrawList( );

			// Push clip rect to strictly contain the glow
			draw_list->PushClipRect( bg_pos, bg_pos + bg_size, true );

			ImVec2 circle_center = ImVec2(
				bg_pos.x + 20.0f,  // Position inside the window bounds
				bg_pos.y + 10.0f
			);

			//ImGui::GetForegroundDrawList( )->AddShadowCircle(
			//	circle_center,
			//	85,
			//	ImColor( 135, 122, 233, 150 ),  // Purple glow
			//	60.0f,                        // Moderate blur that won't exceed bounds
			//	ImVec2( 0.0f, 0.0f ),          // No offset
			//	false,                        // Not filled
			//	32                           // Segments
			//);

			draw_list->PopClipRect( );

			// Add centered leaderboard text
			ImGui::PushFont( awesome_font::bold_font );
			const char* leaderboard_text = "Leaderboard";
			ImVec2 leaderboard_size = ImGui::CalcTextSize( leaderboard_text );
			ImVec2 hello_text_pos = ImVec2(
				bg_pos.x + ( bg_size.x * 0.5f ) - ( leaderboard_size.x * 0.5f ),  // Center horizontally
				bg_pos.y + 20.0f  // 20px from top
			);
			custom::fade_text( ImGui::GetForegroundDrawList( ), leaderboard_text, hello_text_pos, c::main_color, c::anim::active, 0 );

			// Add underline below leaderboard text
			float underline_y = hello_text_pos.y + leaderboard_size.y + 5.0f;  // 5px padding from text
			float underline_thickness = 2.0f;
			ImVec2 underline_start = ImVec2( hello_text_pos.x, underline_y );
			ImVec2 underline_end = ImVec2( hello_text_pos.x + leaderboard_size.x, underline_y );
			
			// Use the same gradient method as custom::fade_text for the underline
			custom::fade_line( ImGui::GetForegroundDrawList( ), underline_start, underline_end, c::anim::active, c::main_color, underline_thickness );

			ImGui::PopFont( );

			// Add two rounded rectangles under the leaderboard text
			float rect_start_x = bg_pos.x + 23.0f;  // 23px spacing from start (reduced by 2px)
			float rect_end_x = bg_pos.x + bg_size.x - 20.0f;  // End at max size minus 20
			float rect_y = hello_text_pos.y + ImGui::CalcTextSize( "Leaderboard" ).y + 25.0f;  // 15px below text
			float rect_end_y = bg_pos.y + bg_size.y - 23.0f;
			float spacing_between = 20.0f;

			float available_width = rect_end_x - rect_start_x;
			float rect_width = ( available_width - spacing_between ) * 0.5f;

			ImVec2 rect1_min = ImVec2( rect_start_x, rect_y );
			ImVec2 rect1_max = ImVec2( rect_start_x + rect_width, rect_end_y );

			ImGui::GetForegroundDrawList( )->AddRectFilled(
				rect1_min,
				rect1_max,
				ImColor( 15, 11, 35, 150 ),
				c::child::rounding + 10
			);

			ImGui::GetForegroundDrawList( )->AddRect(
				rect1_min,
				rect1_max,
				c::outline_color,
				c::child::rounding + 10,
				2
			);

			float entry_spacing = 52.0f;  // Entry height + spacing (50 + 2px between entries)

			if ( !m_leaderboard_loaded ) {
				m_leaderboard_loaded = true;

				std::thread( [ this ]( ) {
					auto leaderboard = g_auth->fetch_leaderboard( );

					std::sort( leaderboard.begin( ), leaderboard.end( ),
						[ ]( const std::pair<std::string, int>& a, const std::pair<std::string, int>& b ) {
							return a.second > b.second;
						} );

					{
						m_cached_leaderboard = std::move( leaderboard );
						g_last_leaderboard_refresh = std::time( nullptr );
					}
					} ).detach( );
			}

			int max_entries = min( 8, ( int )m_cached_leaderboard.size( ) );

			for ( int i = 0; i < max_entries; i++ ) {
				const auto& entry = m_cached_leaderboard[ i ];

				const char* icon = ( i == 0 ) ? "\uf521" : "\uf007";

				std::string score_text = "Score: " + std::to_string( entry.second );

				render_leaderboard_entry(
					rect1_min,
					rect1_max,
					entry_spacing * i,
					entry.first.c_str( ),
					score_text.c_str( ),
					icon
				);
			}

			// Add "x+ more entries" box if there are more entries than displayed
			if ( ( int )m_cached_leaderboard.size( ) > max_entries ) {
				int remaining_entries = ( int )m_cached_leaderboard.size( ) - max_entries;
				float more_box_y = rect1_min.y + 20.0f + ( entry_spacing * max_entries );
				float more_box_height = 15.0f;

				ImVec2 more_box_min = ImVec2( rect1_min.x + 15.0f, more_box_y );
				ImVec2 more_box_max = ImVec2( rect1_max.x - 15.0f, more_box_y + more_box_height );

				// Draw box background (transparent like arraylist entries)
				ImGui::GetForegroundDrawList( )->AddRectFilled(
					more_box_min,
					more_box_max,
					ImColor( 14, 16, 30, 120 ),  // Same semi-transparent background as arraylist entries
					6.0f  // Same rounding as arraylist entries
				);

				// Draw box border (same as arraylist entries)
				ImGui::GetForegroundDrawList( )->AddRect(
					more_box_min,
					more_box_max,
					c::outline_color,
					6.0f,  // Same rounding as arraylist entries
					0,
					1.0f
				);

				// Add centered text
				ImGui::PushFont( font::small_font );
				std::string more_text = std::format( "{}+ more players", remaining_entries );
				ImVec2 more_text_size = ImGui::CalcTextSize( more_text.c_str( ) );
				ImVec2 more_text_pos = ImVec2(
					more_box_min.x + ( more_box_max.x - more_box_min.x ) * 0.5f - more_text_size.x * 0.5f,  // Center horizontally
					more_box_y + ( more_box_height * 0.5f ) - more_text_size.y * 0.5f  // Center vertically
				);

				custom::fade_text( ImGui::GetForegroundDrawList( ), more_text.c_str( ), more_text_pos, c::anim::active, c::text::description::default_color, 0 );
				ImGui::PopFont( );
			}

			ImVec2 rect2_min = ImVec2( rect_start_x + rect_width + spacing_between, rect_y );
			ImVec2 rect2_max = ImVec2( rect_end_x, rect_end_y );

			ImGui::GetForegroundDrawList( )->AddRectFilled(
				rect2_min,
				rect2_max,
				ImColor( 15, 11, 35, 150 ),
				c::child::rounding + 10
			);

			ImGui::GetForegroundDrawList( )->AddRect(
				rect2_min,
				rect2_max,
				c::outline_color,
				c::child::rounding + 10,
				2
			);

			// Add circle outline with user icon at the top of the second child
			float user_circle_radius = 25.0f;
			ImVec2 user_circle_center = ImVec2(
				rect2_min.x + ( rect2_max.x - rect2_min.x ) * 0.5f,  // Center horizontally
				rect2_min.y + 35.0f  // 35px from top
			);

			// Draw circle outline
			ImGui::GetForegroundDrawList( )->AddCircle(
				user_circle_center,
				user_circle_radius,
				c::outline_color,
				32,
				2.0f
			);

			// Add user icon inside the circle
			ImGui::PushFont( awesome_font::bold_font );
			const char* user_unicode = "\uf007";  // User icon
			ImVec2 user_icon_size = ImGui::CalcTextSize( user_unicode );
			ImVec2 user_icon_pos = ImVec2(
				user_circle_center.x - user_icon_size.x * 0.5f,
				user_circle_center.y - user_icon_size.y * 0.5f
			);

			custom::fade_text( ImGui::GetForegroundDrawList( ), user_unicode, user_icon_pos, c::anim::active, c::main_color, 0 );
			ImGui::PopFont( );

			// Add username below the circle with gradient color scheme
			ImGui::PushFont( font::s_inter_semibold );
			std::string username = g_profile->get_cache( ).m_player_name;
			ImVec2 username_size = ImGui::CalcTextSize( username.c_str( ) );
			ImVec2 username_pos = ImVec2(
				user_circle_center.x - username_size.x * 0.5f,  // Center horizontally
				user_circle_center.y + user_circle_radius + 10.0f  // 15px below circle
			);

			// Use the same gradient color scheme as the rest of the dashboard
			ImColor gradient_color = ImColor( 135, 122, 233, 255 );  // Purple gradient
			custom::fade_text( ImGui::GetForegroundDrawList( ), username.c_str( ), username_pos, c::anim::active, gradient_color, 0 );
			ImGui::PopFont( );

			// Add score text below the username in smaller font (use leaderboard vector when available)
			ImGui::PushFont( font::esp_font );
			int current_score_value = g_profile->get_cache( ).m_kill_score;
			for ( const auto& pair : m_cached_leaderboard ) {
				if ( pair.first == username ) { current_score_value = pair.second; break; }
			}
			std::string score_text = "Score: " + std::to_string( current_score_value );
			ImVec2 score_size = ImGui::CalcTextSize( score_text.c_str( ) );
			ImVec2 score_pos = ImVec2(
				user_circle_center.x - score_size.x * 0.5f,  // Center horizontally
				username_pos.y + username_size.y  // 8px below username
			);

			// Use the same gradient color scheme for consistency
			custom::fade_text( ImGui::GetForegroundDrawList( ), score_text.c_str( ), score_pos, c::anim::active, gradient_color, 0 );
			ImGui::PopFont( );

			// Add warning text above the box
			ImGui::PushFont( font::esp_font );
			const char* warning_icon = "\uf071";  // Warning triangle icon
			ImVec2 warning_icon_size = ImGui::CalcTextSize( warning_icon );
			std::string warning_text = "AI/Bot Eliminations don't count towards score";
			ImVec2 warning_text_size = ImGui::CalcTextSize( warning_text.c_str( ) );

			// Position warning above the box
			float warning_y = score_pos.y + score_size.y + 15.0f;  // 15px below score, 15px above box
			ImVec2 warning_icon_pos = ImVec2(
				rect2_min.x + 20.0f,  // Left side with 20px padding
				warning_y - warning_icon_size.y * 0.5f
			);
			ImVec2 warning_text_pos = ImVec2(
				warning_icon_pos.x + warning_icon_size.x + 8.0f,  // 8px spacing from icon
				warning_y - warning_text_size.y * 0.5f
			);

			// Add header above the Earned rectangle
			ImGui::PushFont( font::bold_font );
			std::string header_text = "Scoring";
			ImVec2 header_size = ImGui::CalcTextSize( header_text.c_str( ) );

			// Calculate header position (centered on x axis)
			float header_x = rect2_min.x + ( rect2_max.x - rect2_min.x ) * 0.5f - header_size.x * 0.5f;
			float header_y = warning_y + 10.0f;  // 15px below warning text
			ImVec2 header_pos = ImVec2( header_x, header_y );

			custom::fade_text( ImGui::GetForegroundDrawList( ), header_text.c_str( ), header_pos, c::anim::active, c::main_color, 0 );
			ImGui::PopFont( );

			// Add gradient underline below header text using same method as custom::fade_text
			// Add gradient underline below header text using same method as custom::fade_text
			float header_underline_y = header_y + header_size.y + 2.0f;  // 2px padding from text
			float header_underline_thickness = 1.5f;
			float header_underline_padding = 22.f;
			ImVec2 header_underline_start = ImVec2( header_x - header_underline_padding, header_underline_y );
			ImVec2 header_underline_end = ImVec2( header_x + header_size.x + header_underline_padding, header_underline_y );

			// Use the same gradient method as custom::fade_text
			custom::fade_line( ImGui::GetForegroundDrawList( ), header_underline_start, header_underline_end, c::anim::active, c::main_color, header_underline_thickness );

			// Common box settings
			float box_start_x = rect2_min.x + 30.0f;  // 30px offset from start (increased for smaller width)
			float box_end_x = rect2_max.x - 30.0f;    // 30px offset from end (increased for smaller width)
			float box_height = 46.0f;  // Height as specified
			ImGui::PopFont( );

			// Add first box (Eliminations) under the header
			float first_box_y = header_underline_y + 15.0f;  // 10px below underline (increased padding)
			ImVec2 first_box_min = ImVec2( box_start_x, first_box_y );
			ImVec2 first_box_max = ImVec2( box_end_x, first_box_y + box_height );

			render_stats_box( first_box_min, first_box_max, "\ue54e", "Elimination", "1 Point" );

			// Add second box (Wins) under the eliminations box
			float second_box_y = first_box_y + box_height + 10.0f;  // 10px padding below first box (reduced from 15px)
			ImVec2 second_box_min = ImVec2( box_start_x, second_box_y );
			ImVec2 second_box_max = ImVec2( box_end_x, second_box_y + box_height );

			render_stats_box( second_box_min, second_box_max, "\uf071", "AI Eliminations", "No Reward" );

			// Add "Rank" header above the Earned box
			ImGui::PushFont( font::bold_font );
			std::string rank_header_text = "Session";
			ImVec2 rank_header_size = ImGui::CalcTextSize( rank_header_text.c_str( ) );

			// Calculate rank header position (centered on x axis)
			float rank_header_x = rect2_min.x + ( rect2_max.x - rect2_min.x ) * 0.5f - rank_header_size.x * 0.5f;
			float rank_header_y = second_box_y + box_height + 15.0f;  // 25px below wins box for header spacing
			ImVec2 rank_header_pos = ImVec2( rank_header_x, rank_header_y );

			custom::fade_text( ImGui::GetForegroundDrawList( ), rank_header_text.c_str( ), rank_header_pos, c::anim::active, c::main_color, 0 );
			ImGui::PopFont( );

			// Add gradient underline below rank header text
			float rank_underline_y = rank_header_y + rank_header_size.y + 2.0f;  // 2px padding from text
			float rank_underline_thickness = 1.5f;
			float rank_underline_padding = 15.f;
			ImVec2 rank_underline_start = ImVec2( rank_header_x - rank_underline_padding, rank_underline_y );
			ImVec2 rank_underline_end = ImVec2( rank_header_x + rank_header_size.x + rank_underline_padding, rank_underline_y );

			// Use the same gradient method as custom::fade_text
			custom::fade_line( ImGui::GetForegroundDrawList( ), rank_underline_start, rank_underline_end, c::anim::active, c::main_color, rank_underline_thickness );

			// Add third box (Earned) under the rank header
			float third_box_y = rank_underline_y + 10.0f;  // 5px below rank underline (reduced from 10px)
			ImVec2 third_box_min = ImVec2( box_start_x, third_box_y );
			ImVec2 third_box_max = ImVec2( box_end_x, third_box_y + box_height );

			std::string points_text = std::format( "{} Points", g_profile->get_cache( ).m_total_points_earned );
			render_stats_box( third_box_min, third_box_max, "\uf091", "Earned", points_text.c_str( ) );

			// Add refresh button positioned 10px from bottom of main window
			float button_width = 120.0f;
			float button_height = 40.0f;
			// Position 10px from bottom of the main window
			float button_y = rect2_max.y - button_height - 10.0f;  // 10px from bottom of main window
			float button_x = rect2_min.x + ( rect2_max.x - rect2_min.x ) * 0.5f - button_width * 0.5f;  // Center horizontally

			ImVec2 button_min = ImVec2( button_x, button_y );
			ImVec2 button_max = ImVec2( button_x + button_width, button_y + button_height );

			// Draw button background
			ImGui::GetForegroundDrawList( )->AddRectFilled(
				button_min,
				button_max,
				c::widget_bg_color,  // Same color as the box above
				9.0f
			);

			// Draw button outline
			ImGui::GetForegroundDrawList( )->AddRect(
				button_min,
				button_max,
				c::outline_color,
				9.0f,
				0,
				1.5f
			);

			// Calculate total content width for centering
			std::string refresh_text = "Refresh";
			ImGui::PushFont( font::s_inter_semibold );
			const char* refresh_icon = "\uf021";  // Refresh icon
			ImVec2 refresh_text_size = ImGui::CalcTextSize( refresh_text.c_str( ) );

			float icon_text_spacing = 12.0f;  // Increased spacing between icon and text
			float text_left_padding = 15.0f;  // Padding from left side for text
			float icon_right_padding = 10.0f;  // Padding from right side for icon

			// Position text on the left side
			ImVec2 refresh_text_pos = ImVec2(
				button_x + text_left_padding,  // Left side with padding
				button_y + ( button_height * 0.5f ) - refresh_text_size.y * 0.5f  // Center vertically in button
			);

			custom::fade_text( ImGui::GetForegroundDrawList( ), refresh_text.c_str( ), refresh_text_pos, c::anim::active, c::main_color, 0 );
			ImGui::PopFont( );

			// Position icon on the right side, centered on Y axis
			ImGui::PushFont( awesome_font::regular );
			ImVec2 refresh_icon_size = ImGui::CalcTextSize( refresh_icon );

			ImVec2 refresh_icon_pos = ImVec2(
				button_x + button_width - icon_right_padding - refresh_icon_size.x,  // Right side with padding
				button_y + ( button_height * 0.5f ) - refresh_icon_size.y * 0.5f  // Center vertically in button
			);

			custom::fade_text( ImGui::GetForegroundDrawList( ), refresh_icon, refresh_icon_pos, c::anim::active, c::main_color, 0 );
			ImGui::PopFont( );

			// Check if refresh button was clicked (simple mouse position check)
			ImVec2 mouse_pos = ImGui::GetMousePos( );
			if ( ImGui::IsMouseClicked( 0 ) &&  // Left mouse click
				mouse_pos.x >= button_min.x && mouse_pos.x <= button_max.x &&
				mouse_pos.y >= button_min.y && mouse_pos.y <= button_max.y ) {
				// Reset leaderboard cache to force refresh
				m_leaderboard_loaded = false;
				m_cached_leaderboard.clear( );
				g_last_leaderboard_refresh = std::time( nullptr );
			}

			// Show last updated (relative) at top-right inside this giveaway area
			if ( g_last_leaderboard_refresh != 0 ) {
				auto now = std::time( nullptr );
				long long seconds = now - g_last_leaderboard_refresh;
				const char* unit = "s"; long long value = seconds;
				if ( seconds >= 3600 ) { value = seconds / 3600; unit = "h"; }
				else if ( seconds >= 60 ) { value = seconds / 60; unit = "m"; }
				char rel_buf[ 32 ];
#ifdef _MSC_VER
				sprintf_s( rel_buf, sizeof( rel_buf ), "%lld%s ago", value, unit );
#else
				snprintf( rel_buf, sizeof( rel_buf ), "%lld%s ago", value, unit );
#endif
				PushFont( font::small_font );
				const char* updated_lbl = "updated";
				ImVec2 upd_sz = ImGui::CalcTextSize( updated_lbl );
				ImVec2 rel_sz = ImGui::CalcTextSize( rel_buf );
				ImVec2 upd_pos = ImVec2( rect2_max.x - upd_sz.x - 12.0f, rect2_min.y + 8.0f );
				ImVec2 rel_pos = ImVec2( rect2_max.x - rel_sz.x - 12.0f, upd_pos.y + upd_sz.y + 1.0f );
				custom::fade_text( ImGui::GetForegroundDrawList( ), updated_lbl, upd_pos, c::text::description::default_color, c::main_color, 0 );
				PopFont( );

				PushFont( font::s_inter_semibold );
				custom::fade_text( ImGui::GetForegroundDrawList( ), rel_buf, rel_pos, c::text::description::default_color, c::main_color, 0 );
				PopFont( );
			}


		}

		// Function to render a statistics box
		void render_stats_box( ImVec2 box_min, ImVec2 box_max, const char* icon, const char* title, const char* value, ImU32 accent_color = c::main_color, bool use_accent = false ) {
			// Style like arraylist entries
			ImGui::GetForegroundDrawList( )->AddRectFilled(
				box_min,
				box_max,
				ImColor( 14, 16, 30, 120 ),  // Semi-transparent like arraylist entries
				6.0f  // Same rounding as arraylist entries
			);

			ImGui::GetForegroundDrawList( )->AddRect(
				box_min,
				box_max,
				c::outline_color,
				6.0f,  // Same rounding as arraylist entries
				0,
				1.0f  // Same border width as arraylist entries
			);

			// Calculate positioning
			float box_height = box_max.y - box_min.y;
			float icon_padding = 15.0f;  // Padding from left side for icon
			float text_padding = -10.0f;  // Padding between icon and text
			float icon_scale = 1.8f;  // Scale factor for bigger icon
			float icon_y = box_min.y + ( box_height * 0.5f );  // Center vertically in box

			// Add icon on the LEFT SIDE
			ImGui::PushFont( awesome_font::semibold_font );
			ImVec2 icon_size = ImGui::CalcTextSize( icon );
			// Make icon bigger by scaling
			ImVec2 scaled_icon_size = ImVec2( icon_size.x * icon_scale, icon_size.y * icon_scale );
			ImVec2 icon_pos = ImVec2(
				box_min.x + icon_padding,  // Left side with padding
				icon_y - scaled_icon_size.y * 0.5f + 10
			);

			custom::fade_text( ImGui::GetForegroundDrawList( ), icon, icon_pos, c::anim::active, accent_color, 0 );
			ImGui::PopFont( );

			// Calculate text start position (after icon + padding)
			float text_start_x = icon_pos.x + scaled_icon_size.x + text_padding;

			// Add title text on top (small font) - RIGHT OF ICON
			ImGui::PushFont( font::esp_font );
			ImVec2 title_size = ImGui::CalcTextSize( title );
			ImVec2 title_pos = ImVec2(
				text_start_x,  // After icon with padding
				box_min.y + 5.0f  // 5px from top of box
			);

			custom::fade_text( ImGui::GetForegroundDrawList( ), title, title_pos, c::anim::active, c::text::description::default_color, 0 );
			ImGui::PopFont( );

			// Add value text below in big font - RIGHT OF ICON
			ImGui::PushFont( font::s_inter_semibold );
			ImVec2 value_pos = ImVec2(
				text_start_x,  // After icon with padding
				title_pos.y + title_size.y - 1.0f  // Below title text with reduced padding
			);

			custom::fade_text( ImGui::GetForegroundDrawList( ), value, value_pos, c::anim::active, c::main_color, 0 );
			ImGui::PopFont( );
		}

		// Function to render a leaderboard entry
		void render_leaderboard_entry( ImVec2 rect_min, ImVec2 rect_max, float y_offset, const char* player_name, const char* score, const char* user_icon = "\uf007" ) {
			float entry_height = 70.0f;  // Height per entry
			float current_y = rect_min.y + 5.0f + y_offset;  // Start 5px from top + offset (reduced from 10px)

			// User circle
			float user_circle_radius = 22.0f;
			ImVec2 user_circle_center = ImVec2(
				rect_min.x + 30.0f,
				current_y + 35.0f  // Center vertically in entry area
			);

			// Draw outline circle
			ImGui::GetForegroundDrawList( )->AddCircle(
				user_circle_center,
				user_circle_radius,
				c::outline_color,
				32,
				2.0f
			);

			// Add icon in the middle of circle
			ImGui::PushFont( awesome_font::regular );
			ImVec2 user_icon_size = ImGui::CalcTextSize( user_icon );
			ImVec2 user_icon_pos = ImVec2(
				user_circle_center.x - user_icon_size.x * 0.5f,
				user_circle_center.y - user_icon_size.y * 0.5f
			);

			custom::fade_text( ImGui::GetForegroundDrawList( ), user_icon, user_icon_pos, c::anim::active, c::main_color, 0 );
			ImGui::PopFont( );

			// Draw line from circle to right edge
			float line_start_x = user_circle_center.x + user_circle_radius + 3.0f;
			float line_end_x = rect_max.x - 5.0f;
			float line_y = user_circle_center.y - user_circle_radius;

			ImVec2 line_start = ImVec2( line_start_x, line_y );
			ImVec2 line_end = ImVec2( line_end_x, line_y );

			ImGui::GetForegroundDrawList( )->AddLine(
				line_start,
				line_end,
				c::outline_color,
				1.5f
			);

			// Add player name text
			ImGui::PushFont( font::s_inter_semibold );
			ImVec2 player_name_pos = ImVec2(
				user_circle_center.x + user_circle_radius + 5.0f,
				line_y + 5.0f
			);

			custom::fade_text( ImGui::GetForegroundDrawList( ), player_name, player_name_pos, c::anim::active, c::main_color, 0 );
			ImGui::PopFont( );

			// Add score text under the player name
			ImGui::PushFont( font::esp_font );
			ImVec2 score_text_pos = ImVec2(
				player_name_pos.x,
				player_name_pos.y + ImGui::CalcTextSize( player_name ).y + 3.0f
			);

			custom::fade_text( ImGui::GetForegroundDrawList( ), score, score_text_pos, c::anim::active, c::text::description::default_color, 0 );
			ImGui::PopFont( );

			// Add up-right arrow icon on the right side
			ImGui::PushFont( awesome_font::bold_font );
			const char* up_right_arrow = "\uf35b";
			ImVec2 arrow_icon_pos = ImVec2(
				line_end_x - 30.0f,
				player_name_pos.y + 7.0f
			);

			ImColor gradient_start = ImColor( 135, 122, 233, 255 );  // Purple
			custom::fade_text( ImGui::GetForegroundDrawList( ), up_right_arrow, arrow_icon_pos, c::anim::active, gradient_start, 0 );
			ImGui::PopFont( );
		}

	private:
		// Cached leaderboard data
		std::vector<std::pair<std::string, int>> m_cached_leaderboard;
		bool m_leaderboard_loaded = false;
		// Removed: using global g_last_leaderboard_refresh for UI display
	};

	class c_dashboard {
	public:
		c_dashboard() {
			m_profile_window = std::make_shared<c_window>("Profile");
			m_settings_window = std::make_shared<c_window>("Settings");
		}

		bool m_profile_active = false;
		bool m_home_active = true;
		bool m_leaderboard_active = false;
		bool m_settings_active = false;

		void render( ) {
			auto bar_width = 850.0f;
			auto bar_height = 60.0f;
			auto y_offset = 20.0f;
			auto corner_radius = 25.0f;

			auto bar_pos = ImVec2(
				( g_overlay->m_width - bar_width ) / 2.0f,
				y_offset
			);
			auto bar_size = ImVec2( bar_width, bar_height );

			auto theme_color = ImColor( 135, 122, 233, 255 );
			auto bg_color = ImColor( 14, 13, 29, 255 );
			auto border_color = c::outline_color;

			g_background->AddRectFilled(
				bar_pos,
				bar_pos + bar_size,
				bg_color,
				corner_radius
			);

			g_background->AddRect(
				bar_pos,
				bar_pos + bar_size,
				border_color,
				corner_radius,
				0,
				2.f
			);

			auto pos = bar_pos;

			PushFont( awesome_font::bold_font );
			auto person_icon_pos = ImVec2( pos.x + 15, pos.y + 18 );
			custom::fade_text( g_background, "\uf007", person_icon_pos, c::anim::active, c::main_color, 0 );
			PopFont( );

			// (moved) updated/relative time from widget bar to giveaway popup

			auto user_icon = CalcTextSize( "\uf007" );
			PushFont( font::s_inter_semibold );
			auto hello_text_size = CalcTextSize( "Welcome back," );
			auto hello_text_pos = ImVec2( person_icon_pos.x + user_icon.x + 18, person_icon_pos.y - 8 );
			custom::fade_text( g_background, "Welcome back,", hello_text_pos, c::anim::active, c::main_color, 0 );
			PopFont( );

			PushFont( font::esp_font );
			auto display_name_pos = ImVec2( hello_text_pos.x, hello_text_pos.y + hello_text_size.y + 2 );

			std::string display_name = g_profile->get_cache( ).m_player_name;
			custom::fade_text( g_background, display_name.c_str( ), display_name_pos, c::anim::active, c::main_color, 0 );
			PopFont( );

			PushFont( awesome_font::bold_font );
			auto popstar_text_width_right = CalcTextSize( g_auth_config->get_brand( ).c_str( ) ).x;
			auto fire_icon_width = CalcTextSize( "\uf005" ).x;
			auto total_right_content_width = fire_icon_width + 10 + popstar_text_width_right;

			auto right_content_start_x = bar_pos.x + bar_width - total_right_content_width - 19;

			// Update FPS tracking
			int current_fps = (int)ImGui::GetIO().Framerate;
			if (!m_fps_initialized) {
				m_avg_fps = (float)current_fps;
				m_highest_fps = current_fps;
				m_fps_initialized = true;
			} else {
				// Exponential moving average for stability
				const float alpha = 0.10f; // smoothing factor
				m_avg_fps = alpha * (float)current_fps + (1.0f - alpha) * m_avg_fps;
				if (current_fps > m_highest_fps) m_highest_fps = current_fps;
			}

			// Right-side time widget
			{
				const float widget_width = 130.0f;
				const float widget_bottom_pad = 10.0f;
				const float right_pad = 110.0f;
				const float widget_x = bar_pos.x + bar_width - right_pad - widget_width; // right end
				ImVec2 small_rect_min = ImVec2( widget_x, bar_pos.y + 7 ); // align Y with "Welcome back,"
				ImVec2 small_rect_max = ImVec2( widget_x + widget_width, bar_pos.y + bar_height - widget_bottom_pad );
				//g_background->AddRectFilled( small_rect_min, small_rect_max, ImColor( 19, 20, 34, 255 ), corner_radius );

				// Label on top: "Time"
				float padding = 12.0f;
				float text_x = small_rect_min.x + padding;
				PushFont( font::small_font );
				const char* time_label = "Time";
				ImVec2 time_label_size = CalcTextSize( time_label );
				float label_y = small_rect_min.y + 2.0f;
				custom::fade_text( g_background, time_label, ImVec2( text_x, label_y ), c::text::description::default_color, c::main_color, 0 );
				PopFont();

				// Time value below the label
				std::string time_text = currentTime12h();
				PushFont( font::bold_font );
				ImVec2 time_size = CalcTextSize( time_text.c_str() );
				float time_x = text_x;
				float time_y = ImMin( small_rect_max.y - time_size.y - 2.0f, label_y + time_label_size.y + 3.0f );
				custom::fade_text( g_background, time_text.c_str(), ImVec2( time_x, time_y ), c::main_color, c::anim::active, 0 );
				PopFont();
			}

			// Render an additional fixed-position widget on the bar (hardcoded from bar_pos)
			{
				const float widget_width = 130.0f;
				const float widget_bottom_pad = 10.0f;
				const float left_pad = 170.0f; // fixed offset from left side of bar
				const float widget_x = bar_pos.x + left_pad;
				ImVec2 rect_min = ImVec2( widget_x, bar_pos.y + 7 ); // align Y with "Welcome back,"
				ImVec2 rect_max = ImVec2( widget_x + widget_width, bar_pos.y + bar_height - widget_bottom_pad );
				//g_background->AddRectFilled( rect_min, rect_max, ImColor( 19, 20, 34, 255 ), corner_radius );

				// Label on top: "Framerate"
				const float inner_pad_x = 12.0f;
				float text_x2 = rect_min.x + inner_pad_x;
				PushFont( font::small_font );
				const char* time_lbl2 = "Framerate";
				ImVec2 label_sz = CalcTextSize( time_lbl2 );
				float label_y2 = rect_min.y + 2.0f;
				custom::fade_text( g_background, time_lbl2, ImVec2( text_x2, label_y2 ), c::text::description::default_color, c::main_color, 0 );
				PopFont();

				// FPS value below the label
				std::string time_text2 = std::to_string( ( int )ImGui::GetIO().Framerate );
				PushFont( font::bold_font );
				ImVec2 time_sz = CalcTextSize( time_text2.c_str() );
				float time_y2 = ImMin( rect_max.y - time_sz.y - 2.0f, label_y2 + label_sz.y + 3.0f );
				custom::fade_text( g_background, time_text2.c_str(), ImVec2( text_x2, time_y2 ), c::main_color, c::anim::active, 0 );
				PopFont();
			}

			auto fire_icon_pos = ImVec2( right_content_start_x, person_icon_pos.y   );
			custom::fade_text( g_background, "\uf005", fire_icon_pos, c::anim::active, c::main_color, 0 );
			PopFont( );

			PushFont( font::bold_font );
			auto popstar_right_pos = ImVec2( right_content_start_x + fire_icon_width + 11, hello_text_pos.y + 5 );
			custom::fade_text( g_background, g_auth_config->get_brand( ).c_str( ), popstar_right_pos, c::anim::active, c::main_color, 0 );
			PopFont( );

			ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 8, 8 ) );
			ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 10, 10 ) );

			ImGui::SetNextWindowPos( bar_pos + ImVec2( 300, -4 ) );
			ImGui::SetNextWindowSize( bar_size );
			ImGui::Begin( "Dashboard", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground );
			{
				if ( custom::SquareButton( "Home", ICON_HOME_2_FILL, ImVec2( 60, 42 ) ) ) {
					m_home_active = !m_home_active;
				}

				ImGui::SameLine( );

				if ( custom::SquareButton( "Leaderboard", "", ImVec2( 60, 42 ), true ) ) {
					m_leaderboard_active = !m_leaderboard_active;
				}

				ImGui::SameLine( );

				if ( custom::SquareButton( "Settings", ICON_SETTINGS_4_FILL, ImVec2( 60, 42 ) ) ) {
					m_settings_active = !m_settings_active;
				}

				ImGui::SameLine( );

				if ( custom::SquareButton( "Close", "\uf057", ImVec2( 50, 42 ), true ) ) {
					g_driver->unload( );
					g_overlay->destroy( );
					exit( 0 );
				}
			}

			ImGui::End( );
			ImGui::PopStyleVar( 2 );
		}

		// avalei makes me angy rn- leproxy note 10:05pm 8/4/2025
		void draw_leaderboard( ) {
			if ( m_leaderboard_active ) {
				m_profile_window->open( );
			}
			else {
				m_profile_window->close( );
			}

			if ( m_profile_window->begin( ) ) { 
				m_profile_window->render_giveaway( );
			}

			m_profile_window->end( );
		}

		void draw_settings( ) {
			if ( m_settings_active ) {
				m_settings_window->open( );
			}
			else {
				m_settings_window->close( );
			}

			if ( m_settings_window->begin( "Settings" ) ) {
				m_settings_window->render_settings( );
			}

			m_settings_window->end( );
		}

	private:
		std::shared_ptr<c_window> m_profile_window;
		std::shared_ptr<c_window> m_settings_window;

		float m_avg_fps = 0.0f;
		int m_highest_fps = 0;
		bool m_fps_initialized = false;
	};
}