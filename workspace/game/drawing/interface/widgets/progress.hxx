#pragma once

namespace progress {
	struct progress_info_t {
		std::string operation_name;
		std::string current_item;
		int current_progress;
		int total_progress;
		float percentage;
		bool is_active;
		std::chrono::steady_clock::time_point start_time;
		std::chrono::steady_clock::time_point last_update;
	};

	// Loading wheel animation state
	struct loading_wheel_state {
		float rotation_angle = 0.0f;
	};

	class c_progress {
	private:
		progress_info_t m_progress;
		bool m_should_render;
		ImVec2 m_window_size;
		ImVec2 m_window_pos;
		float m_progress_bar_width;
		float m_progress_bar_height;
		loading_wheel_state m_wheel_state;

		void calculate_window_position( ) {
			auto content_height = 30.0f +
				135.0f +
				12.0f +
				15.0f +
				16.0f +
				50.0f;

			m_window_size = ImVec2( 600.0f, content_height );
			m_window_pos = ImVec2(
				g_overlay->m_width_center - ( m_window_size.x * 0.5f ),
				g_overlay->m_height_center - ( m_window_size.y * 0.5f )
			);

			m_progress_bar_width = 450.0f;
			m_progress_bar_height = 12.0f;
		}

		void draw_weed_image( ImDrawList* draw_list, ImVec2 center_pos ) {
			float image_width = 100.0f;
			float image_height = 110.0f;

			ImVec2 image_min = ImVec2( center_pos.x - image_width * 0.5f, center_pos.y - image_height * 0.5f );
			ImVec2 image_max = ImVec2( center_pos.x + image_width * 0.5f, center_pos.y + image_height * 0.5f );

			draw_list->AddImage( texture::weed_image, image_min, image_max );
		}

		void draw_loading_wheel( ImDrawList* draw_list, ImVec2 center_pos ) {
			float dt = ImGui::GetIO( ).DeltaTime;

			m_wheel_state.rotation_angle += dt * 4.0f;
			if ( m_wheel_state.rotation_angle > 2.0f * IM_PI ) {
				m_wheel_state.rotation_angle -= 2.0f * IM_PI;
			}

			const float wheel_radius = 50.0f;
			const float stroke_width = 8.0f;

			g_foreground->AddShadowCircle(
				center_pos,
				wheel_radius,
				ImColor( 135, 122, 233, 255 ),
				20.0f,
				ImVec2( 0.0f, 0.0f ),
				3.0f,
				64
			);

			ImColor bg_color = utils::GetColorWithAlpha( c::outline_color, 0.6f );
			draw_list->AddCircle(
				center_pos,
				wheel_radius,
				bg_color,
				64,
				stroke_width
			);

			const int segments = 16;
			const float start_angle = m_wheel_state.rotation_angle;


			ImColor accent_start = utils::GetColorWithAlpha( c::main_color, 1.0f );
			ImColor accent_end = utils::GetColorWithAlpha( ImColor( 230, 131, 90, 255 ), 0.9f );

			for ( int i = 0; i < segments; i++ ) {
				float angle1 = start_angle + ( float )i / segments * IM_PI * 0.5f;
				float angle2 = start_angle + ( float )( i + 1 ) / segments * IM_PI * 0.5f;

				float t = ( float )i / segments;

				ImColor segment_color = ImColor(
					accent_start.Value.x * ( 1.0f - t ) + accent_end.Value.x * t,
					accent_start.Value.y * ( 1.0f - t ) + accent_end.Value.y * t,
					accent_start.Value.z * ( 1.0f - t ) + accent_end.Value.z * t,
					1.0f
				);

				ImVec2 p1_inner = ImVec2(
					center_pos.x + cosf( angle1 ) * ( wheel_radius - stroke_width * 0.5f ),
					center_pos.y + sinf( angle1 ) * ( wheel_radius - stroke_width * 0.5f )
				);
				ImVec2 p1_outer = ImVec2(
					center_pos.x + cosf( angle1 ) * ( wheel_radius + stroke_width * 0.5f ),
					center_pos.y + sinf( angle1 ) * ( wheel_radius + stroke_width * 0.5f )
				);
				ImVec2 p2_inner = ImVec2(
					center_pos.x + cosf( angle2 ) * ( wheel_radius - stroke_width * 0.5f ),
					center_pos.y + sinf( angle2 ) * ( wheel_radius - stroke_width * 0.5f )
				);
				ImVec2 p2_outer = ImVec2(
					center_pos.x + cosf( angle2 ) * ( wheel_radius + stroke_width * 0.5f ),
					center_pos.y + sinf( angle2 ) * ( wheel_radius + stroke_width * 0.5f )
				);

				// Create shadow using exact quad shape
				ImVec2 quad_points[4] = { p1_inner, p1_outer, p2_outer, p2_inner };
				draw_list->AddShadowConvexPoly( quad_points, 4, segment_color, 10.0f, ImVec2( 0.0f, 0.0f ) );
				draw_list->AddQuadFilled( p1_inner, p1_outer, p2_outer, p2_inner, segment_color );
			}
		}

		std::string format_elapsed_time( ) const {
			auto now = std::chrono::steady_clock::now( );
			auto elapsed = std::chrono::duration_cast< std::chrono::seconds >( now - m_progress.start_time );

			int minutes = elapsed.count( ) / 60;
			int seconds = elapsed.count( ) % 60;

			if ( minutes > 0 ) {
				return std::format( "{}m", minutes );
			}
			return std::format( "{}s", seconds );
		}

		std::string get_loading_dots( ) const {
			auto now = std::chrono::steady_clock::now( );
			auto elapsed_ms = std::chrono::duration_cast< std::chrono::milliseconds >( now - m_progress.start_time ).count( );

			int dot_state = ( elapsed_ms / 700 ) % 4;

			switch ( dot_state ) {
			case 0: return "";
			case 1: return ".";
			case 2: return "..";
			case 3: return "...";
			default: return "";
			}
		}

		std::string estimate_remaining_time( ) const {
			if ( m_progress.percentage <= 5.0f ) return "Calculating...";

			auto now = std::chrono::steady_clock::now( );
			auto elapsed = std::chrono::duration_cast< std::chrono::seconds >( now - m_progress.start_time );
			if ( elapsed.count( ) < 3 ) return "Calculating...";

			float items_per_second = static_cast< float >( m_progress.current_progress ) / elapsed.count( );
			if ( items_per_second <= 0.0f ) return "Calculating...";

			int remaining_items = m_progress.total_progress - m_progress.current_progress;
			int estimated_remaining = static_cast< int >( remaining_items / items_per_second );

			if ( estimated_remaining <= 0 ) return "Done";

			int minutes = estimated_remaining / 60;
			int seconds = estimated_remaining % 60;

			if ( minutes > 0 ) {
				return std::format( "{}m", minutes );
			}
			return std::format( "{}s", seconds );
		}

	public:
		c_progress( ) {
			m_progress = {};
			m_should_render = false;
			m_wheel_state = {};
			calculate_window_position( );
		}

		void begin_operation( const std::string& operation_name, int total_items ) {
			m_progress.operation_name = operation_name;
			m_progress.current_item = "";
			m_progress.current_progress = 0;
			m_progress.total_progress = total_items;
			m_progress.percentage = 0.0f;
			m_progress.is_active = true;
			m_progress.start_time = std::chrono::steady_clock::now( );
			m_progress.last_update = m_progress.start_time;
			m_should_render = true;

			// Reset wheel animation
			m_wheel_state = {};

			calculate_window_position( );
		}

		void update_progress( int current, const std::string& current_item = "" ) {
			if ( !m_progress.is_active ) return;

			m_progress.current_progress = current;
			m_progress.current_item = current_item;
			m_progress.percentage = m_progress.total_progress > 0 ?
				( static_cast< float >( current ) / static_cast< float >( m_progress.total_progress ) ) * 100.0f : 0.0f;
			m_progress.last_update = std::chrono::steady_clock::now( );

			if ( m_progress.percentage > 100.0f ) m_progress.percentage = 100.0f;
		}

		void set_current_item( const std::string& item_name ) {
			m_progress.current_item = item_name;
			m_progress.last_update = std::chrono::steady_clock::now( );
		}

		void complete_operation( ) {
			if ( !m_progress.is_active ) return;

			m_progress.current_progress = m_progress.total_progress;
			m_progress.percentage = 100.0f;
			m_progress.current_item = "Complete!";
			m_progress.is_active = false;

			std::this_thread::sleep_for( std::chrono::milliseconds( 5000 ) );
			m_should_render = false;
		}

		void cancel_operation( ) {
			m_progress.is_active = false;
			m_should_render = false;
		}

		bool is_active( ) const {
			return m_progress.is_active;
		}

		bool should_render( ) const {
			return m_should_render;
		}

		void render( ) {
			if ( !m_should_render ) return;

			calculate_window_position( );

			ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
			ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 9.0f );
			ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
			ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f ) );

			ImGui::SetNextWindowPos( m_window_pos, ImGuiCond_Always );
			ImGui::SetNextWindowSize( m_window_size, ImGuiCond_Always );

			ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoFocusOnAppearing;

			if ( ImGui::Begin( "##ProgressWindow", nullptr, flags ) ) {
				ImDrawList* draw_list = g_foreground;

				g_foreground->AddShadowRect(
					m_window_pos,
					m_window_pos + m_window_size,
					ImColor( 135, 122, 233, 255 ),
					20.0f,
					ImVec2( 0.0f, 0.0f ),
					3.0f
				);

				draw_list->AddRectFilled(
					m_window_pos,
					m_window_pos + m_window_size,
					ImColor( 14, 13, 29, 255 ),
					9.0f
				);

				draw_list->AddRect(
					m_window_pos,
					m_window_pos + m_window_size,
					ImColor( 70, 70, 70, 255 ),
					9.0f,
					0,
					1.5f
				);

				ImGui::SetCursorPos( ImVec2( 30.0f, 30.0f ) );
				float start_y = ImGui::GetCursorPosY( );

				// Draw loading wheel instead of weed image
				ImVec2 wheel_center = ImVec2(
					m_window_pos.x + m_window_size.x * 0.5f,
					m_window_pos.y + 75.0f
				);
				draw_loading_wheel( draw_list, wheel_center );

				ImGui::SetCursorPosY( start_y + 135.0f );

				ImVec2 progress_start = ImVec2(
					m_window_pos.x + ( m_window_size.x - m_progress_bar_width ) * 0.5f,
					m_window_pos.y + ImGui::GetCursorPosY( )
				);

				ImVec2 progress_end = ImVec2(
					progress_start.x + m_progress_bar_width,
					progress_start.y + m_progress_bar_height
				);

				draw_list->AddShadowRect(
					progress_start,
					progress_end,
					ImColor( 135, 122, 233, 255 ),
					20.0f,
					ImVec2( 0.0f, 0.0f ),
					3.0f
				);

				draw_list->AddRectFilled( progress_start, progress_end, c::window_bg_color );

				if ( m_progress.percentage > 0.0f ) {
					float fill_width = ( m_progress_bar_width * m_progress.percentage ) / 100.0f;
					ImVec2 fill_end = ImVec2( progress_start.x + fill_width, progress_end.y );

					ImU32 color_start = ImColor( 0.2f, 0.6f, 1.0f, 1.0f );
					ImU32 color_end = ImColor( 0.2f, 0.8f, 0.3f, 1.0f );

					draw_list->AddRectFilled(
						progress_start, fill_end,
						c::main_color
					);
				}

				draw_list->AddRect( progress_start, progress_end, c::outline_color, 0.0f, 0, 2.0f );

				float text_y_position = progress_end.y + 15.0f;

				std::string progress_text = std::format( "[{}/{}] {}{}",
					m_progress.current_progress, m_progress.total_progress, m_progress.operation_name, get_loading_dots( ) );

				std::string percentage_text = std::format( "{:.0f}%", m_progress.percentage );

				auto progress_text_size = ImGui::CalcTextSize( progress_text.c_str( ) );
				auto percentage_text_size = ImGui::CalcTextSize( percentage_text.c_str( ) );

				float progress_text_x = progress_start.x;
				float percentage_text_x = progress_end.x - percentage_text_size.x;

				draw_list->AddText(
					ImVec2( progress_text_x, text_y_position ),
					ImGui::GetColorU32( text::text ),
					progress_text.c_str( )
				);

				draw_list->AddText(
					ImVec2( percentage_text_x, text_y_position ),
					ImGui::GetColorU32( text::text ),
					percentage_text.c_str( )
				);
			}
			ImGui::End( );

			ImGui::PopStyleColor( 1 );
			ImGui::PopStyleVar( 3 );
		}

		void increment_progress( const std::string& current_item = "" ) {
			update_progress( m_progress.current_progress + 1, current_item );
		}

		void add_progress( int amount, const std::string& current_item = "" ) {
			update_progress( m_progress.current_progress + amount, current_item );
		}

		float get_percentage( ) const {
			return m_progress.percentage;
		}

		int get_current_progress( ) const {
			return m_progress.current_progress;
		}

		int get_total_progress( ) const {
			return m_progress.total_progress;
		}
	};
}