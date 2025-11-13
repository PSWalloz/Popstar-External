#pragma once

namespace message {
	enum e_message_type {
		MESSAGE_TYPE_GENERAL,
		MESSAGE_TYPE_SUCCESS,
		MESSAGE_TYPE_WARNING,
		MESSAGE_TYPE_ERROR
	};

	struct message_info_t {
		std::string title;
		std::vector<std::tuple<std::string, std::string, ImColor>> entries;
		const char* icon_text = ICON_USER_3_FILL;
		e_message_type type;
		bool is_active;
	};

	class c_message {
	private:
		std::vector<message_info_t> m_messages;
		bool m_should_render;
		float m_message_spacing;

		void calculate_message_position( const message_info_t& message, int index, ImVec2& window_pos, ImVec2& window_size ) {
			float entry_height = 19.0f;
			float base_height = 42.0f;
			float bottom_padding = 15.0f;
			float content_height = base_height + ( message.entries.size( ) * entry_height ) + bottom_padding;

			window_size = ImVec2( 350.f, content_height );

			float cumulative_offset = 0.0f;
			for ( int i = 0; i < index; ++i ) {
				if ( i < m_messages.size() && m_messages[i].is_active ) {
					float prev_entry_height = 19.0f;
					float prev_base_height = 42.0f;
					float prev_bottom_padding = 15.0f;
					float prev_content_height = prev_base_height + ( m_messages[i].entries.size( ) * prev_entry_height ) + prev_bottom_padding;
					cumulative_offset += prev_content_height + m_message_spacing;
				}
			}

			window_pos = ImVec2( 20.0f, 20.0f + cumulative_offset );
		}

	public:
		c_message( ) {
			m_should_render = false;
			m_message_spacing = 9.0f;
		}

		void add_message( const message_info_t& message ) {
			message_info_t new_message = message;
			new_message.is_active = true;

			m_messages.push_back( new_message );
			m_should_render = true;
		}

		void remove_message( int index ) {
			if ( index >= 0 && index < m_messages.size( ) ) {
				m_messages.erase( m_messages.begin( ) + index );
				if ( m_messages.empty( ) ) {
					m_should_render = false;
				}
			}
		}

		void clear_all_messages( ) {
			m_messages.clear( );
			m_should_render = false;
		}

		void set_message_spacing( float spacing ) {
			m_message_spacing = spacing;
		}

		void show( ) {
			m_should_render = true;
		}

		void hide( ) {
			m_should_render = false;
		}

		bool is_active( ) const {
			return !m_messages.empty( ) && m_should_render;
		}

		size_t get_message_count( ) const {
			return m_messages.size( );
		}

		ImVec2 get_last_debug_box_position( ) const {
			if ( m_messages.empty( ) ) {
				float message_width = 350.0f;
				float centered_x = 20.0f + ( message_width * 0.5f );
				return ImVec2( centered_x, 20.0f );
			}

			float cumulative_offset = m_message_spacing + 2;
			int active_messages = 0;

			for ( int i = 0; i < m_messages.size( ); ++i ) {
				const auto& message = m_messages[ i ];
				if ( !message.is_active ) continue;

				float entry_height = 19.0f;
				float base_height = 42.0f;
				float bottom_padding = 15.0f;
				float content_height = base_height + ( message.entries.size( ) * entry_height ) + bottom_padding;

				cumulative_offset += content_height;
				if ( active_messages > 0 ) {
					cumulative_offset += m_message_spacing;
				}
				active_messages++;
			}

			float message_width = 350.0f;
			float centered_x = 20.0f + ( message_width * 0.5f );

			return ImVec2( centered_x, 20.0f + cumulative_offset );
		}

		void render( ) {
			for ( int i = 0; i < m_messages.size( ); ++i ) {
				const auto& message = m_messages[ i ];
				if ( !message.is_active ) continue;

				ImVec2 window_pos, window_size;
				calculate_message_position( message, i, window_pos, window_size );

				ImColor theme_color;
				const char* icon_text = message.icon_text;
				int index = 0;

				ImColor title_color = theme_color;
				
				switch ( message.type ) {
				case MESSAGE_TYPE_SUCCESS:
					theme_color = ImColor( 0, 255, 100, 255 );
					title_color = ImColor( 100, 200, 120, 255 );
					icon_text = ICON_CHECK_FILL;
					index = 2;
					break;
				case MESSAGE_TYPE_WARNING:
					theme_color = ImColor( 245, 158, 11, 255 );
					icon_text = ICON_ALERT_FILL;
					title_color = theme_color;
					break;
				case MESSAGE_TYPE_ERROR:
					theme_color = ImColor( 239, 68, 68, 255 );
					icon_text = ICON_CLOSE_CIRCLE_FILL;
					title_color = theme_color;
					index = 2;
					break;
				default:
					theme_color = ImColor( 135, 122, 233, 255 );
					title_color = theme_color;
					break;
				}

				g_foreground->AddShadowRect(
					window_pos,
					window_pos + window_size,
					theme_color,
					20.0f,
					ImVec2( 0.0f, 0.0f ),
					3.0f
				);

				g_foreground->AddRectFilled(
					window_pos,
					window_pos + window_size,
					ImColor( 14, 13, 29, 255 ),
					3.0f
				);

				g_foreground->AddRect(
					window_pos,
					window_pos + window_size,
					ImColor( 70, 70, 70, 255 ),
					3.0f,
					0,
					1.5f
				);

				ImVec2 icon_area_pos = ImVec2(
					window_pos.x + 13.0f,
					window_pos.y + 9.0f
				);

				float icon_size = 35.0f;
				ImVec2 icon_area_size = ImVec2( icon_size, icon_size );

				ImVec2 circle_center = ImVec2(
					icon_area_pos.x + icon_size * 0.5f,
					icon_area_pos.y + icon_size * 0.6f - index
				);
				float circle_radius = icon_size * 0.5f;

				g_foreground->AddCircleFilled(
					circle_center,
					circle_radius,
					theme_color,
					32
				);

				ImGui::PushFont( font::semibold_font );
				ImVec2 icon_pos = utils::center_text( icon_area_pos, icon_area_pos + icon_area_size, icon_text ) + ImVec2( 0, 4.5f );

				g_foreground->AddText(
					icon_pos,
					ImColor( 14, 13, 29, 255 ),
					icon_text
				);

				g_foreground->AddCircle(
					circle_center,
					circle_radius,
					ImColor( 0, 0, 0, 255 ),
					32,
					1.5f
				);

				ImGui::PopFont( );
				ImGui::PushFont( font::inter_medium_semibold );

				if ( !message.title.empty( ) ) {
					ImVec2 title_pos = ImVec2(
						icon_area_pos.x + icon_size + 16.0f,
						window_pos.y + 14.0f
					);

					auto outline_color = ImColor( 0, 0, 0, 255 );
					for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
						for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
							if ( dx == 0.0f && dy == 0.0f ) continue;
							g_foreground->AddText(
								ImVec2( title_pos.x + dx, title_pos.y + dy ),
								outline_color,
								message.title.c_str( )
							);
						}
					}

					g_foreground->AddText(
						title_pos,
						title_color,
						message.title.c_str( )
					);
				}

				ImGui::PopFont( );

				if ( !message.entries.empty( ) ) {
					ImGui::PushFont( font::inter_medium );

					float current_y = window_pos.y + 42.0f;
					float entry_height = 19.0f;

					for ( const auto& [entry, value, color] : message.entries ) {
						auto outline_color = ImColor( 0, 0, 0, 255 );
						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								g_foreground->AddText(
									ImVec2( icon_area_pos.x + icon_size + 16.0f + dx, current_y - dy ),
									outline_color,
									entry.c_str( )
								);
							}
						}

						g_foreground->AddText(
							ImVec2( icon_area_pos.x + icon_size + 16.0f, current_y ),
							ImColor( 160, 160, 160, 255 ),
							entry.c_str( )
						);

						auto value_size = ImGui::CalcTextSize( value.c_str( ) );
						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								g_foreground->AddText(
									ImVec2( window_pos.x + window_size.x - value_size.x - 15.0f + dx, current_y - dx ),
									outline_color,
									value.c_str( )
								);
							}
						}

						g_foreground->AddText(
							ImVec2( window_pos.x + window_size.x - value_size.x - 15.0f, current_y ),
							color,
							value.c_str( )
						);

						current_y += entry_height;
					}

					ImGui::PopFont( );
				}
			}

			m_messages.clear( );
		}
	};
}