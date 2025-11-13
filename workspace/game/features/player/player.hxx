#pragma once 

namespace player {
	void render( lists::c_player const player ) {
		if ( config::enemies::enabled ) {
			auto theme_color = esp::get_theme_color( player );

			if ( config::radar::enabled ) {
				g_radar->render( player );
			}

			auto head_location = player.m_head_location;
			auto head_screen = engine::project( head_location );
			if ( head_screen.in_screen( ) ) {
				auto origin = player.m_bounds.orgin;
				auto extent = player.m_bounds.box_extent;

				engine::f_vector2d min_pos( DBL_MAX, DBL_MAX );
				engine::f_vector2d max_pos( DBL_MIN, DBL_MIN );

				for ( int i = 0; i < 8; i++ ) {
					auto corner = engine::f_vector(
						( i & 1 ) ? extent.x : -extent.x,
						( i & 2 ) ? extent.y : -extent.y,
						( i & 4 ) ? extent.z : -extent.z
					);

					auto box_origin = origin + corner;
					auto origin_2d = engine::project( box_origin );
					if ( !origin_2d.is_valid( ) ) continue;

					min_pos.x = min( min_pos.x, origin_2d.x );
					min_pos.y = min( min_pos.y, origin_2d.y );
					max_pos.x = max( max_pos.x, origin_2d.x );
					max_pos.y = max( max_pos.y, origin_2d.y );
				}


				engine::f_vector2d size( max_pos - min_pos );
				engine::f_vector2d bottom_middle( min_pos.x + ( size.x / 2.0 ), max_pos.y );
				engine::f_vector2d top_middle( min_pos.x + ( size.x / 2.0 ), min_pos.y );

				auto corner_width = size.x / 4.0;
				auto corner_height = size.y / 4.5;

				auto font_size = ImGui::GetFontSize( );
				auto top_text_offset = font_size + 2.0;
				auto bottom_text_offset = 1.0;
				auto stat_box_offset = 1.0f;

				auto right_stat_offset = 0.0f;
				const float side_padding = 5.0f;

				if ( config::enemies::box_style[ 0 ] ) {
					auto filled_color = ImColor(
						theme_color.m_main_color.Value.x,
						theme_color.m_main_color.Value.y,
						theme_color.m_main_color.Value.z,
						80.0f / 255.0f
					);

					g_background->AddRectFilled(
						min_pos.vec( ),
						max_pos.vec( ),
						filled_color,
						5.0f
					);
				}

				if ( config::enemies::box_style[ 2 ] ) {
					g_background->AddShadowRect(
						min_pos.vec( ),
						max_pos.vec( ),
						theme_color.m_main_color,
						120.f,
						ImVec2( 0.0f, 0.0f )
					);
				}

				switch ( config::enemies::box_esp ) {
				case 1: {
					ImVec2 tl( min_pos.x, min_pos.y );
					ImVec2 br( max_pos.x, max_pos.y );

					const float offset = 0.5f;

					if ( config::enemies::box_style[ 1 ] ) {
						g_background->AddRect(
							ImVec2( tl.x - offset, tl.y - offset ),
							ImVec2( br.x + offset, br.y + offset ),
							theme_color.m_outline_color,
							0.0f, 0,
							2.0f
						);

						g_background->AddRect(
							ImVec2( tl.x + offset, tl.y + offset ),
							ImVec2( br.x - offset, br.y - offset ),
							theme_color.m_outline_color,
							0.0f, 0,
							2.0f
						);
					}

					g_background->AddRect(
						tl,
						br,
						theme_color.m_main_color,
						0.0f, 0,
						1.5f
					);

				} break;
				case 2: {
					if ( config::enemies::box_style[ 1 ] ) {
						g_background->AddLine( ImVec2( min_pos.x - 0.5f, min_pos.y - 0.5f ), ImVec2( min_pos.x + corner_width + 0.5f, min_pos.y - 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.25f );
						g_background->AddLine( ImVec2( min_pos.x - 0.5f, min_pos.y - 0.5f ), ImVec2( min_pos.x - 0.5f, min_pos.y + corner_height + 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.25f );

						g_background->AddLine( ImVec2( min_pos.x + 0.5f, min_pos.y + 0.5f ), ImVec2( min_pos.x + corner_width - 0.5f, min_pos.y + 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );
						g_background->AddLine( ImVec2( min_pos.x + 0.5f, min_pos.y + 0.5f ), ImVec2( min_pos.x + 0.5f, min_pos.y + corner_height - 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );

						g_background->AddLine( ImVec2( min_pos.x + size.x - corner_width - 0.5f, min_pos.y - 0.5f ), ImVec2( min_pos.x + size.x + 0.5f, min_pos.y - 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.25f );
						g_background->AddLine( ImVec2( min_pos.x + size.x + 0.5f, min_pos.y - 0.5f ), ImVec2( min_pos.x + size.x + 0.5f, min_pos.y + corner_height + 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.25f );

						g_background->AddLine( ImVec2( min_pos.x + size.x - corner_width + 0.5f, min_pos.y + 0.5f ), ImVec2( min_pos.x + size.x - 0.5f, min_pos.y + 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );
						g_background->AddLine( ImVec2( min_pos.x + size.x - 0.5f, min_pos.y + 0.5f ), ImVec2( min_pos.x + size.x - 0.5f, min_pos.y + corner_height - 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );

						g_background->AddLine( ImVec2( min_pos.x - 0.5f, min_pos.y + size.y - corner_height - 0.5f ), ImVec2( min_pos.x - 0.5f, min_pos.y + size.y + 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.25f );
						g_background->AddLine( ImVec2( min_pos.x - 0.5f, min_pos.y + size.y + 0.5f ), ImVec2( min_pos.x + corner_width + 0.5f, min_pos.y + size.y + 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.25f );

						g_background->AddLine( ImVec2( min_pos.x + 0.5f, min_pos.y + size.y - corner_height + 0.5f ), ImVec2( min_pos.x + 0.5f, min_pos.y + size.y - 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );
						g_background->AddLine( ImVec2( min_pos.x + 0.5f, min_pos.y + size.y - 0.5f ), ImVec2( min_pos.x + corner_width - 0.5f, min_pos.y + size.y - 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );

						g_background->AddLine( ImVec2( min_pos.x + size.x - corner_width - 0.5f, min_pos.y + size.y + 0.5f ), ImVec2( min_pos.x + size.x + 0.5f, min_pos.y + size.y + 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.25f );
						g_background->AddLine( ImVec2( min_pos.x + size.x + 0.5f, min_pos.y + size.y - corner_height - 0.5f ), ImVec2( min_pos.x + size.x + 0.5f, min_pos.y + size.y + 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.25f );

						g_background->AddLine( ImVec2( min_pos.x + size.x - corner_width + 0.5f, min_pos.y + size.y - 0.5f ), ImVec2( min_pos.x + size.x - 0.5f, min_pos.y + size.y - 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );
						g_background->AddLine( ImVec2( min_pos.x + size.x - 0.5f, min_pos.y + size.y - corner_height + 0.5f ), ImVec2( min_pos.x + size.x - 0.5f, min_pos.y + size.y - 0.5f ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );
					}

					g_background->AddLine( ImVec2( min_pos.x, min_pos.y ), ImVec2( min_pos.x, min_pos.y + corner_height ), theme_color.m_main_color, 1.5f );
					g_background->AddLine( ImVec2( min_pos.x, min_pos.y ), ImVec2( min_pos.x + corner_width, min_pos.y ), theme_color.m_main_color, 1.5f );

					g_background->AddLine( ImVec2( min_pos.x + size.x - corner_width, min_pos.y ), ImVec2( min_pos.x + size.x, min_pos.y ), theme_color.m_main_color, 1.5f );
					g_background->AddLine( ImVec2( min_pos.x + size.x, min_pos.y ), ImVec2( min_pos.x + size.x, min_pos.y + corner_height ), theme_color.m_main_color, 1.5f );

					g_background->AddLine( ImVec2( min_pos.x, min_pos.y + size.y - corner_height ), ImVec2( min_pos.x, min_pos.y + size.y ), theme_color.m_main_color, 1.5f );
					g_background->AddLine( ImVec2( min_pos.x, min_pos.y + size.y ), ImVec2( min_pos.x + corner_width, min_pos.y + size.y ), theme_color.m_main_color, 1.5f );

					g_background->AddLine( ImVec2( min_pos.x + size.x - corner_width, min_pos.y + size.y ), ImVec2( min_pos.x + size.x, min_pos.y + size.y ), theme_color.m_main_color, 1.5f );
					g_background->AddLine( ImVec2( min_pos.x + size.x, min_pos.y + size.y - corner_height ), ImVec2( min_pos.x + size.x, min_pos.y + size.y ), theme_color.m_main_color, 1.5f );

				} break;
				case 3: {
					engine::f_vector corners[ 8 ] = {
						origin + engine::f_vector( -extent.x, -extent.y, -extent.z ),
						origin + engine::f_vector( extent.x, -extent.y, -extent.z ),
						origin + engine::f_vector( -extent.x, extent.y, -extent.z ),
						origin + engine::f_vector( extent.x, extent.y, -extent.z ),
						origin + engine::f_vector( -extent.x, -extent.y, extent.z ),
						origin + engine::f_vector( extent.x, -extent.y, extent.z ),
						origin + engine::f_vector( -extent.x, extent.y, extent.z ),
						origin + engine::f_vector( extent.x, extent.y, extent.z )
					};

					engine::f_vector2d screen_corners[ 8 ];
					bool valid = true;
					for ( int i = 0; i < 8; i++ ) {
						screen_corners[ i ] = engine::project( corners[ i ] );
						if ( !screen_corners[ i ].is_valid( ) ) valid = false;
					}

					if ( valid ) {
						const float thickness = 1.5f;

						if ( config::enemies::box_style[ 2 ] ) {
							g_background->AddLine( screen_corners[ 0 ].vec( ) - ImVec2( 1, 1 ), screen_corners[ 1 ].vec( ) - ImVec2( -1, 1 ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );
							g_background->AddLine( screen_corners[ 1 ].vec( ) - ImVec2( -1, 1 ), screen_corners[ 3 ].vec( ) - ImVec2( -1, -1 ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );
							g_background->AddLine( screen_corners[ 3 ].vec( ) - ImVec2( -1, -1 ), screen_corners[ 2 ].vec( ) - ImVec2( 1, -1 ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );
							g_background->AddLine( screen_corners[ 2 ].vec( ) - ImVec2( 1, -1 ), screen_corners[ 0 ].vec( ) - ImVec2( 1, 1 ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );

							g_background->AddLine( screen_corners[ 0 ].vec( ) + ImVec2( 1, 1 ), screen_corners[ 1 ].vec( ) + ImVec2( -1, 1 ), theme_color.m_outline_color, config::enemies::outline_thickness );
							g_background->AddLine( screen_corners[ 1 ].vec( ) + ImVec2( -1, 1 ), screen_corners[ 3 ].vec( ) + ImVec2( -1, -1 ), theme_color.m_outline_color, config::enemies::outline_thickness );
							g_background->AddLine( screen_corners[ 3 ].vec( ) + ImVec2( -1, -1 ), screen_corners[ 2 ].vec( ) + ImVec2( 1, -1 ), theme_color.m_outline_color, config::enemies::outline_thickness );
							g_background->AddLine( screen_corners[ 2 ].vec( ) + ImVec2( 1, -1 ), screen_corners[ 0 ].vec( ) + ImVec2( 1, 1 ), theme_color.m_outline_color, config::enemies::outline_thickness );

							g_background->AddLine( screen_corners[ 4 ].vec( ) - ImVec2( 1, 1 ), screen_corners[ 5 ].vec( ) - ImVec2( -1, 1 ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );
							g_background->AddLine( screen_corners[ 5 ].vec( ) - ImVec2( -1, 1 ), screen_corners[ 7 ].vec( ) - ImVec2( -1, -1 ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );
							g_background->AddLine( screen_corners[ 7 ].vec( ) - ImVec2( -1, -1 ), screen_corners[ 6 ].vec( ) - ImVec2( 1, -1 ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );
							g_background->AddLine( screen_corners[ 6 ].vec( ) - ImVec2( 1, -1 ), screen_corners[ 4 ].vec( ) - ImVec2( 1, 1 ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );

							g_background->AddLine( screen_corners[ 4 ].vec( ) + ImVec2( 1, 1 ), screen_corners[ 5 ].vec( ) + ImVec2( -1, 1 ), theme_color.m_outline_color, config::enemies::outline_thickness );
							g_background->AddLine( screen_corners[ 5 ].vec( ) + ImVec2( -1, 1 ), screen_corners[ 7 ].vec( ) + ImVec2( -1, -1 ), theme_color.m_outline_color, config::enemies::outline_thickness );
							g_background->AddLine( screen_corners[ 7 ].vec( ) + ImVec2( -1, -1 ), screen_corners[ 6 ].vec( ) + ImVec2( 1, -1 ), theme_color.m_outline_color, config::enemies::outline_thickness );
							g_background->AddLine( screen_corners[ 6 ].vec( ) + ImVec2( 1, -1 ), screen_corners[ 4 ].vec( ) + ImVec2( 1, 1 ), theme_color.m_outline_color, config::enemies::outline_thickness );

							g_background->AddLine( screen_corners[ 0 ].vec( ) - ImVec2( 1, 1 ), screen_corners[ 4 ].vec( ) - ImVec2( 1, 1 ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );
							g_background->AddLine( screen_corners[ 1 ].vec( ) - ImVec2( -1, 1 ), screen_corners[ 5 ].vec( ) - ImVec2( -1, 1 ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );
							g_background->AddLine( screen_corners[ 2 ].vec( ) - ImVec2( 1, -1 ), screen_corners[ 6 ].vec( ) - ImVec2( 1, -1 ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );
							g_background->AddLine( screen_corners[ 3 ].vec( ) - ImVec2( -1, -1 ), screen_corners[ 7 ].vec( ) - ImVec2( -1, -1 ), theme_color.m_outline_color, config::enemies::outline_thickness * 0.5f );

							g_background->AddLine( screen_corners[ 0 ].vec( ) + ImVec2( 1, 1 ), screen_corners[ 4 ].vec( ) + ImVec2( 1, 1 ), theme_color.m_outline_color, config::enemies::outline_thickness );
							g_background->AddLine( screen_corners[ 1 ].vec( ) + ImVec2( -1, 1 ), screen_corners[ 5 ].vec( ) + ImVec2( -1, 1 ), theme_color.m_outline_color, config::enemies::outline_thickness );
							g_background->AddLine( screen_corners[ 2 ].vec( ) + ImVec2( 1, -1 ), screen_corners[ 6 ].vec( ) + ImVec2( 1, -1 ), theme_color.m_outline_color, config::enemies::outline_thickness );
							g_background->AddLine( screen_corners[ 3 ].vec( ) + ImVec2( -1, -1 ), screen_corners[ 7 ].vec( ) + ImVec2( -1, -1 ), theme_color.m_outline_color, config::enemies::outline_thickness );
						}

						g_background->AddLine( screen_corners[ 0 ].vec( ), screen_corners[ 1 ].vec( ), theme_color.m_main_color, thickness );
						g_background->AddLine( screen_corners[ 1 ].vec( ), screen_corners[ 3 ].vec( ), theme_color.m_main_color, thickness );
						g_background->AddLine( screen_corners[ 3 ].vec( ), screen_corners[ 2 ].vec( ), theme_color.m_main_color, thickness );
						g_background->AddLine( screen_corners[ 2 ].vec( ), screen_corners[ 0 ].vec( ), theme_color.m_main_color, thickness );

						g_background->AddLine( screen_corners[ 4 ].vec( ), screen_corners[ 5 ].vec( ), theme_color.m_main_color, thickness );
						g_background->AddLine( screen_corners[ 5 ].vec( ), screen_corners[ 7 ].vec( ), theme_color.m_main_color, thickness );
						g_background->AddLine( screen_corners[ 7 ].vec( ), screen_corners[ 6 ].vec( ), theme_color.m_main_color, thickness );
						g_background->AddLine( screen_corners[ 6 ].vec( ), screen_corners[ 4 ].vec( ), theme_color.m_main_color, thickness );

						g_background->AddLine( screen_corners[ 0 ].vec( ), screen_corners[ 4 ].vec( ), theme_color.m_main_color, thickness );
						g_background->AddLine( screen_corners[ 1 ].vec( ), screen_corners[ 5 ].vec( ), theme_color.m_main_color, thickness );
						g_background->AddLine( screen_corners[ 2 ].vec( ), screen_corners[ 6 ].vec( ), theme_color.m_main_color, thickness );
						g_background->AddLine( screen_corners[ 3 ].vec( ), screen_corners[ 7 ].vec( ), theme_color.m_main_color, thickness );
					}
				} break;
				}

				if ( config::enemies::movement_tracers ) {
					auto& tracer = g_player_traces[ player.m_actor ];
					if ( tracer.has_valid_traces( ) ) {
						auto& traces = tracer.get_traces( );
						auto now = std::chrono::steady_clock::now( );
						auto style = static_cast< tracers::tracer_style_t >( config::enemies::trial_style );
						auto fade = static_cast< tracers::tracer_fade_t >( config::enemies::trial_fade );

						for ( auto i = 1; i < traces.size( ); ++i ) {
							auto prev_position = traces[ i - 1 ];
							auto curr_position = traces[ i ];

							auto prev_screen = engine::project( prev_position.m_position );
							auto curr_screen = engine::project( curr_position.m_position );
							if ( !prev_screen.is_valid( ) || !curr_screen.is_valid( ) ) continue;

							auto age = std::chrono::duration_cast< std::chrono::duration<float> >(
								now - curr_position.m_timestamp ).count( );
							auto age_ratio = min( age / curr_position.m_duration, 1.0f );
							if ( age_ratio >= 1.0f ) continue;

							auto color = tracer.get_segment_color( curr_position, age_ratio, style,
								theme_color.m_main_color );
							auto alpha = tracer.get_fade_alpha( age_ratio, fade );
							color.Value.w = alpha;
							if ( color.Value.w <= 0.0f ) continue;

							tracer.render(
								g_background,
								prev_screen.vec( ),
								curr_screen.vec( ),
								color,
								config::enemies::trial_thickness,
								style
							);
						}
					}
				}

				ImGui::PushFont( font::esp_font );

				float distance_based_font_size = 15.0f;
				distance_based_font_size += ( 175.0f - player.m_distance ) * 0.1f;
				distance_based_font_size = min( distance_based_font_size, 16.0f );

				ImGui::SetWindowFontScale( distance_based_font_size / 15.0f );

				if ( config::enemies::team_mode ) {
					auto team_text = esp::get_team_text( player.m_team_size );
					auto text_color = esp::get_team_color( player.m_team_size );

					if ( !team_text.empty( ) ) {
						auto text_size = ImGui::CalcTextSize( team_text.c_str( ) );
						auto text_pos = ImVec2(
							max_pos.x + side_padding,
							min_pos.y + right_stat_offset
						);

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								g_background->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									theme_color.m_outline_color,
									team_text.c_str( )
								);
							}
						}

						g_background->AddText( text_pos, text_color, team_text.c_str( ) );
						right_stat_offset += text_size.y + 2.0f;
					}
				}

				if ( config::enemies::wins ) {
					auto player_stats = g_world->m_player_stats_cache[ player.m_actor ];
					if ( player_stats ) {

						auto text_format = std::format( "Wins: {}", player_stats->m_most_wins_value );
						auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

						auto text_color = ImColor( 255, 107, 53, 255 );
						auto text_pos = ImVec2(
							max_pos.x + side_padding,
							min_pos.y + right_stat_offset
						);

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								g_background->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									theme_color.m_outline_color,
									text_format.c_str( )
								);
							}
						}

						g_background->AddText( text_pos, text_color, text_format.c_str( ) );
						right_stat_offset += text_size.y + 2.0f;
					}
				}

				if ( config::enemies::winrate ) {
					auto player_stats = g_world->m_player_stats_cache[ player.m_actor ];
					if ( player_stats ) {
						auto text_format = std::format( "Winrate: {:.1f}%", player_stats->m_best_winrate_value );
						auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

						auto text_color = ImColor( 255, 107, 53, 255 );
						auto text_pos = ImVec2(
							max_pos.x + side_padding,
							min_pos.y + right_stat_offset
						);

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								g_background->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									theme_color.m_outline_color,
									text_format.c_str( )
								);
							}
						}

						g_background->AddText( text_pos, text_color, text_format.c_str( ) );
						right_stat_offset += text_size.y + 2.0f;
					}
				}

				if ( config::enemies::level ) {
					auto player_level = player.m_player_level;
					if ( player_level && !g_engine->m_in_lobby ) {
						auto text_format = std::format( "Level: {}", player_level );
						auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

						auto text_color = ImColor( 209, 135, 57, 255 );
						auto text_pos = ImVec2(
							max_pos.x + side_padding,
							min_pos.y + right_stat_offset
						);

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								g_background->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									theme_color.m_outline_color,
									text_format.c_str( )
								);
							}
						}

						g_background->AddText( text_pos, text_color, text_format.c_str( ) );
						right_stat_offset += text_size.y + 2.0f;
					}
				}

				if ( config::enemies::kills ) {
					auto m_player_kills = player.m_player_kills;
					if ( m_player_kills && !g_engine->m_in_lobby ) {
						auto text_format = std::format( "Kills: {}", m_player_kills );
						auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

						auto text_color = ImColor( 217, 70, 70, 255 );
						auto text_pos = ImVec2(
							max_pos.x + side_padding,
							min_pos.y + right_stat_offset
						);

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								g_background->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									theme_color.m_outline_color,
									text_format.c_str( )
								);
							}
						}

						g_background->AddText( text_pos, text_color, text_format.c_str( ) );
						right_stat_offset += text_size.y + 2.0f;
					}
				}

				if ( config::enemies::kdr ) {
					auto player_stats = g_world->m_player_stats_cache[ player.m_actor ];
					if ( player_stats ) {
						auto text_format = std::format( "KDR: {:.1f}", player_stats->m_best_kd_value );
						auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

						auto text_color = ImColor( 255, 105, 97, 255 );
						auto text_pos = ImVec2(
							max_pos.x + side_padding,
							min_pos.y + right_stat_offset
						);

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								g_background->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									theme_color.m_outline_color,
									text_format.c_str( )
								);
							}
						}

						g_background->AddText( text_pos, text_color, text_format.c_str( ) );
						right_stat_offset += text_size.y + 2.0f;
					}
				}

				ImGui::PopFont( );

				ImGui::SetWindowFontScale( 1.0f );

				if ( config::enemies::season_rank ) {
					auto text_format = esp::get_rank_name( player.m_player_rank );
					auto text_color = esp::get_rank_color( player.m_player_rank );

					if ( !text_format.empty( ) ) {
						auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

						auto text_pos = ImVec2(
							top_middle.x - ( text_size.x / 2.0f ),
							top_middle.y - top_text_offset
						);

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								g_background->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									theme_color.m_outline_color,
									text_format.c_str( )
								);
							}
						}

						g_background->AddText( text_pos, text_color, text_format.c_str( ) );
						top_text_offset += text_size.y;
					}
				}

				if ( config::enemies::platform ) {
					if ( !player.m_is_bot ) {
						auto text_format = std::format( "{}", esp::get_platform_name( player.m_platform_name ) );
						auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

						auto text_color = esp::get_platform_color( player.m_platform_name );
						auto text_pos = ImVec2(
							top_middle.x - ( text_size.x / 2.0f ),
							top_middle.y - top_text_offset
						);

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								g_background->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									theme_color.m_outline_color,
									text_format.c_str( )
								);
							}
						}

						g_background->AddText( text_pos, text_color, text_format.c_str( ) );
						top_text_offset += text_size.y;
					}
				}

				if ( config::enemies::display_name ) {
					if ( !player.m_player_name.empty( ) ) {
						auto text_format = std::format( "{} ({})", player.m_player_name, player.m_player_id );
						auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

						auto text_pos = ImVec2(
							top_middle.x - ( text_size.x / 2.0f ),
							top_middle.y - top_text_offset
						);

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								g_background->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									theme_color.m_outline_color,
									text_format.c_str( )
								);
							}
						}

						g_background->AddText( text_pos, theme_color.m_main_color, text_format.c_str( ) );
						top_text_offset += text_size.y;
					}
				}
				
				if ( config::enemies::reload_bar ) {
					if ( player.m_weapon ) {
						if ( player.get_magazine_size( ) > 0 ) {
							const auto bar_width = 4.7f;
							const auto bar_height = size.y;
							const auto bar_x = min_pos.x - bar_width - 2.0f;

							auto ammo = player.m_weapon_ammo;
							auto max_ammo = player.get_magazine_size( );
							auto ammo_percentage = static_cast< float >( ammo ) / static_cast< float >( max_ammo );

							g_background->AddRectFilled(
								ImVec2( bar_x, min_pos.y ),
								ImVec2( bar_x + bar_width, min_pos.y + bar_height ),
								ImColor( 14, 13, 29, 255 )
							);

							auto fill_height = bar_height * ammo_percentage;
							if ( fill_height > 0 ) {
								ImColor fill_color;
								if ( ammo_percentage > 0.25f ) {
									fill_color = theme_color.m_main_color;
								}
								else {
									fill_color = ImColor( 255, 105, 97, 255 );
								}

								custom::fade_rect_filled(
									g_background,
									ImVec2( bar_x, max_pos.y - fill_height ),
									ImVec2( bar_x + bar_width, max_pos.y ),
									fill_color,
									c::main_color,
									custom::fade_direction::diagonally,
									0.f,
									0,
									true
								);
							}

							g_background->AddRect(
								ImVec2( bar_x, min_pos.y ),
								ImVec2( bar_x + bar_width, min_pos.y + bar_height ),
								ImColor( 70, 70, 70, 255 ),
								0.0f,
								0,
								1.0f
							);
						}
					}
				}

				if ( config::enemies::weapon_name ) {
					if ( player.m_weapon ) {
						if ( !player.m_weapon_name.empty( ) ) {
							auto text_color = esp::get_tier_color( player.m_weapon_rarity );
							auto text_format = player.m_weapon_ammo ? player.get_magazine_size( ) ?
								std::format( "{} ({}/{})", player.m_weapon_name, player.m_weapon_ammo, player.get_magazine_size( ) ) :
								std::format( "{} ({})", player.m_weapon_name, player.m_weapon_ammo ) :
								std::format( "{}", player.m_weapon_name );
							if ( text_format.contains( "Pickaxe" ) || text_format.contains( "Harvesting" ) )
								text_color = ImColor( 102, 102, 102, 255 );

							auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );
							auto text_pos = ImVec2(
								bottom_middle.x - ( text_size.x / 2.0f ),
								bottom_middle.y + bottom_text_offset
							);

							for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
								for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
									if ( dx == 0.0f && dy == 0.0f ) continue;
									g_background->AddText(
										ImVec2( text_pos.x + dx, text_pos.y + dy ),
										theme_color.m_outline_color,
										text_format.c_str( )
									);
								}
							}

							g_background->AddText( text_pos, text_color, text_format.c_str( ) );
							bottom_text_offset += text_size.y;
						}
					}
				}

				if ( config::enemies::movement ) {
					auto movement_name = engine::get_movement_name( player );
					if ( !movement_name.empty( ) ) {
						auto text_size = ImGui::CalcTextSize( movement_name.c_str( ) );

						auto text_color = engine::get_movement_color( player );
						auto text_pos = ImVec2(
							bottom_middle.x - ( text_size.x / 2.0f ),
							bottom_middle.y + bottom_text_offset
						);

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								g_background->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									theme_color.m_outline_color,
									movement_name.c_str( )
								);
							}
						}

						g_background->AddText( text_pos, text_color, movement_name.c_str( ) );
						bottom_text_offset += text_size.y;
					}
				}

				if ( config::enemies::distance ) {
					auto text_format = std::format( "{:.1f}m", player.m_distance );
					auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

					auto text_pos = ImVec2(
						bottom_middle.x - ( text_size.x / 2.0f ),
						bottom_middle.y + bottom_text_offset
					);

					for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
						for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
							if ( dx == 0.0f && dy == 0.0f ) continue;
							g_background->AddText(
								ImVec2( text_pos.x + dx, text_pos.y + dy ),
								theme_color.m_outline_color,
								text_format.c_str( )
							);
						}
					}

					g_background->AddText( text_pos, theme_color.m_main_color, text_format.c_str( ) );
					bottom_text_offset += text_size.y;
				}

				if ( config::enemies::line_esp ) {
					std::vector<engine::f_vector2d> end_positions{
						{ g_overlay->m_width_center , 0 },
						{ g_overlay->m_width_center, g_overlay->m_height_center },
						{ g_overlay->m_width_center , ( g_overlay->m_height_center * 2 ) + 10 },
					};

					std::vector<engine::f_vector2d> start_positions{
						{ top_middle.x, top_middle.y - top_text_offset },
						{ top_middle.x, top_middle.y - top_text_offset },
						{ bottom_middle.x, bottom_middle.y + bottom_text_offset }
					};

					g_background->AddLine(
						start_positions[ config::enemies::line_esp - 1 ].vec( ),
						end_positions[ config::enemies::line_esp - 1 ].vec( ),
						theme_color.m_main_color
					);
				}

				if ( config::enemies::view_line ) {
					auto rotation = player.m_player_rotation;
					auto direction = rotation.get_forward_vector( );

					auto view_direction = player.m_head_location + ( direction * 180.0 );
					auto view_screen = engine::project( view_direction );

					g_background->AddLine(
						head_screen.vec( ),
						view_screen.vec( ),
						theme_color.m_main_color,
						1.5f
					);
				}

				if ( config::enemies::skeleton ) {
					for ( const auto& pair : player.m_skeleton_pairs ) {
						auto world_location_first = pair.first;
						auto world_location_second = pair.second;

						auto screen_position_first = engine::project( world_location_first );
						auto screen_position_second = engine::project( world_location_second );

						if ( screen_position_first.is_valid( ) && screen_position_second.is_valid( ) ) {
							ImVec2 p0 = screen_position_first.vec( );
							ImVec2 p3 = screen_position_second.vec( );
							const float outline_thickness = config::enemies::skeleton_thickness + 2.0f;

							if ( config::enemies::skeleton_type ) {
								auto dir = ImVec2( p3.x - p0.x, p3.y - p0.y );
								auto length = sqrtf( dir.x * dir.x + dir.y * dir.y );

								dir.x /= length;
								dir.y /= length;

								auto box_center = ImVec2( min_pos.x + size.x * 0.5f, min_pos.y + size.y * 0.5f );
								auto mid_point = ImVec2( ( p0.x + p3.x ) * 0.5f, ( p0.y + p3.y ) * 0.5f );

								auto to_center = ImVec2( mid_point.x - box_center.x, mid_point.y - box_center.y );
								auto to_center_length = sqrtf( to_center.x * to_center.x + to_center.y * to_center.y );
								if ( to_center_length > 0.0f ) {
									to_center.x /= to_center_length;
									to_center.y /= to_center_length;
								}

								auto perp = ImVec2( -dir.y, dir.x );
								auto dot_product = perp.x * to_center.x + perp.y * to_center.y;
								if ( dot_product < 0.0f ) {
									perp.x = -perp.x;
									perp.y = -perp.y;
								}

								auto curve_strength = length * 0.15f;
								ImVec2 p1 = ImVec2( p0.x + dir.x * length * 0.25f + perp.x * curve_strength,
									p0.y + dir.y * length * 0.25f + perp.y * curve_strength );
								ImVec2 p2 = ImVec2( p3.x - dir.x * length * 0.25f + perp.x * curve_strength,
									p3.y - dir.y * length * 0.25f + perp.y * curve_strength );

								const auto segments = 20;
								ImVec2 prev_point = p0;

								for ( auto i = 1; i <= segments; i++ ) {
									auto t = i / ( float )segments;

									auto inv_t = 1.0f - t;
									auto inv_t2 = inv_t * inv_t;
									auto inv_t3 = inv_t2 * inv_t;
									auto t2 = t * t;
									auto t3 = t2 * t;

									auto point = ImVec2(
										inv_t3 * p0.x + 3.0f * inv_t2 * t * p1.x + 3.0f * inv_t * t2 * p2.x + t3 * p3.x,
										inv_t3 * p0.y + 3.0f * inv_t2 * t * p1.y + 3.0f * inv_t * t2 * p2.y + t3 * p3.y
									);

									if ( config::enemies::skeleton_outline ) {
										g_background->AddLine(
											prev_point,
											point,
											theme_color.m_outline_color,
											outline_thickness
										);
									}

									g_background->AddLine(
										prev_point,
										point,
										theme_color.m_main_color,
										config::enemies::skeleton_thickness
									);

									prev_point = point;
								}
							}
							else {
								if ( config::enemies::skeleton_outline ) {
									g_background->AddLine(
										p0,
										p3,
										theme_color.m_outline_color,
										outline_thickness
									);
								}

								g_background->AddLine(
									p0,
									p3,
									theme_color.m_main_color,
									config::enemies::skeleton_thickness
								);
							}
						}
					}

					if ( config::enemies::head_circle ) {
						auto head_world = player.m_head_location;
						auto head_screen = engine::project( head_world );
						if ( head_screen.is_valid( ) && head_screen.in_screen( ) ) {
							const float base_radius = config::enemies::head_size;
							const float near_distance = 5.0f;
							const float far_distance = 250.0f;
							float t = ( player.m_distance - near_distance ) / ( far_distance - near_distance );
							t = max( 0.0f, min( t, 1.0f ) );
							const float head_radius = base_radius * ( 1.0f - t * 0.7f );

							if ( config::enemies::head_hexagon ) {
								const float angle_step = 2.0f * std::numbers::pi / 6.0f;

								if ( config::enemies::skeleton_outline ) {
									const float outline_radius = head_radius + 1.5f;
									ImVec2 outline_vertices[ 6 ];
									for ( int i = 0; i < 6; i++ ) {
										const float angle = angle_step * i;
										outline_vertices[ i ] = ImVec2(
											head_screen.x + std::cos( angle ) * outline_radius,
											head_screen.y + std::sin( angle ) * outline_radius
										);
									}

									for ( int i = 0; i < 6; i++ ) {
										ImVec2 start = outline_vertices[ i ];
										ImVec2 end = outline_vertices[ ( i + 1 ) % 6 ];
										g_background->AddLine(
											start,
											end,
											theme_color.m_outline_color,
											config::enemies::skeleton_thickness
										);
									}
								}

								ImVec2 hexagon_vertices[ 6 ];
								for ( int i = 0; i < 6; i++ ) {
									const float angle = angle_step * i;
									hexagon_vertices[ i ] = ImVec2(
										head_screen.x + std::cos( angle ) * head_radius,
										head_screen.y + std::sin( angle ) * head_radius
									);
								}

								for ( int i = 0; i < 6; i++ ) {
									ImVec2 start = hexagon_vertices[ i ];
									ImVec2 end = hexagon_vertices[ ( i + 1 ) % 6 ];
									g_background->AddLine(
										start,
										end,
										theme_color.m_main_color,
										config::enemies::skeleton_thickness
									);
								}
							}
							else {
								if ( config::enemies::skeleton_outline ) {
									g_background->AddCircle(
										head_screen.vec( ),
										head_radius + 1.5f,
										theme_color.m_outline_color,
										60,
										config::enemies::skeleton_thickness
									);
								}

								g_background->AddCircle(
									head_screen.vec( ),
									head_radius,
									theme_color.m_main_color,
									60,
									config::enemies::skeleton_thickness
								);
							}
						}
					}
				}

				if ( config::enemies::chinse_hat ) {
					auto head_location = player.m_head_location;

					engine::f_vector tip_3d = { head_location.x, head_location.y, head_location.z + config::enemies::hat_height };
					auto screen_tip_proj = engine::project( tip_3d );

					std::vector<ImVec2> base_points_2d;
					base_points_2d.reserve( config::enemies::hat_segments );

					const float angle_step = 2.0f * std::numbers::pi / config::enemies::hat_segments;
					for ( int i = 0; i < config::enemies::hat_segments; ++i ) {
						const float angle = angle_step * i;
						engine::f_vector base_point_3d = {
							head_location.x + std::cos( angle ) * config::enemies::hat_radius,
							head_location.y + std::sin( angle ) * config::enemies::hat_radius,
							head_location.z + config::enemies::hat_base_offset
						};

						auto screen_base = engine::project( base_point_3d );
						base_points_2d.push_back( screen_base.is_valid( ) ? screen_base.vec( ) : ImVec2( -FLT_MAX, -FLT_MAX ) );
					}

					for ( size_t i = 0; i < config::enemies::hat_segments; ++i ) {
						const auto& p1 = base_points_2d[ i ];
						const auto& p2 = base_points_2d[ ( i + 1 ) % config::enemies::hat_segments ];
						if ( p1.x != -FLT_MAX && p2.x != -FLT_MAX ) {
							g_background->AddLine( p1, p2, theme_color.m_main_color, 1.5f );
						}
					}

					if ( screen_tip_proj.is_valid( ) ) {
						ImVec2 tip_2d = screen_tip_proj.vec( );
						for ( const auto& base_point : base_points_2d ) {
							if ( base_point.x != -FLT_MAX ) {
								g_background->AddLine( tip_2d, base_point, theme_color.m_main_color, 1.5f );
							}
						}
					}
				}
			}

			if ( config::enemies::fov_arrows && !g_engine->m_in_lobby ) {
				auto aimbot_fov = g_target.m_aimbot_fov;
				if ( !head_screen.in_circle( aimbot_fov ) ) {
					auto delta_seconds = ImGui::GetIO( ).DeltaTime;
					auto angle = atan2(
						head_screen.y - g_overlay->m_height_center,
						head_screen.x - g_overlay->m_width_center
					);

					engine::f_vector2d arrow_position{
						g_overlay->m_width_center + ( aimbot_fov + 12.0f ) * cos( angle ),
						g_overlay->m_height_center + ( aimbot_fov + 12.0f ) * sin( angle )
					};

					auto rotation_angle = angle - delta_seconds;
					engine::f_vector2d arrow_vertex[ 3 ]{
						{
							arrow_position.x + cos( rotation_angle ) * 15.0f,
							arrow_position.y + sin( rotation_angle ) * 15.0f
						},
						{
							arrow_position.x + cos( rotation_angle - 1.5f ) * 12.0f,
							arrow_position.y + sin( rotation_angle - 1.5f ) * 12.0f
						},
						{
							arrow_position.x + cos( rotation_angle + 1.5f ) * 12.0f,
							arrow_position.y + sin( rotation_angle + 1.5f ) * 12.0f
						}
					};

					for ( float t = 0; t <= 1.0f; t += 0.1f ) {
						engine::f_vector2d left = {
							arrow_vertex[ 0 ].x + ( arrow_vertex[ 1 ].x - arrow_vertex[ 0 ].x ) * t,
							arrow_vertex[ 0 ].y + ( arrow_vertex[ 1 ].y - arrow_vertex[ 0 ].y ) * t
						};

						engine::f_vector2d right = {
							arrow_vertex[ 0 ].x + ( arrow_vertex[ 2 ].x - arrow_vertex[ 0 ].x ) * t,
							arrow_vertex[ 0 ].y + ( arrow_vertex[ 2 ].y - arrow_vertex[ 0 ].y ) * t
						};

						g_background->AddLine(
							left.vec( ),
							right.vec( ),
							theme_color.m_main_color,
							1.5f
						);
					}

					g_background->AddLine( arrow_vertex[ 0 ].vec( ), arrow_vertex[ 1 ].vec( ), theme_color.m_outline_color, 2.5f );
					g_background->AddLine( arrow_vertex[ 1 ].vec( ), arrow_vertex[ 2 ].vec( ), theme_color.m_outline_color, 2.5f );
					g_background->AddLine( arrow_vertex[ 2 ].vec( ), arrow_vertex[ 0 ].vec( ), theme_color.m_outline_color, 2.5f );
				}
			}
		}
	}
}