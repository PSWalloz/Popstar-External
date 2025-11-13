#pragma once 

namespace overlay {
	class c_loop {
	public:
		void render( ) {
			MSG msg{ nullptr };
			while ( msg.message != WM_QUIT ) {
				if ( PeekMessageA( &msg, g_overlay->m_window_handle, 0, 0, PM_REMOVE ) ) {
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}

				g_overlay->update_affinity( );
				g_overlay->new_frame( );

				g_foreground = ImGui::GetForegroundDrawList( );
				g_background = ImGui::GetBackgroundDrawList( );

				if ( !m_user_welcome ) {
					auto profile_cache = g_profile->get_cache( );
					if ( !profile_cache.m_player_name.empty( ) && !profile_cache.m_player_name.contains( "Unknown" ) ) {
						g_notification->add_message( "\uf256", std::format( "Welcome back, {}", profile_cache.m_player_name.c_str( ) ), c::main_color, c::second_color, true );
						m_user_welcome = true;
					}
				}

				ImGui::PushFont( font::inter_medium );

				if ( config::misc::crosshair::enabled ) {
					const float
						radius = config::misc::crosshair::radius,
						thickness = config::misc::crosshair::thickness;
					const int num_segments = config::misc::crosshair::num_segments;
					const auto color = ImColor( config::misc::crosshair::color[ 0 ], config::misc::crosshair::color[ 1 ], config::misc::crosshair::color[ 2 ], config::misc::crosshair::color[ 3 ] );
					const auto outline_color = ImColor( 0.f, 0.f, 0.f, config::misc::crosshair::color[ 3 ] );
					const ImVec2 center( g_overlay->m_width_center, g_overlay->m_height_center );

					const float angle_offset = std::numbers::pi / 4.0f;

					if ( config::misc::crosshair::outline ) {
						for ( int i = 0; i < 4; ++i ) {
							const float a_min = ( ( i * 2.0f + 1.0f ) * std::numbers::pi / 4.0f ) - angle_offset / 2.0f;
							const float a_max = ( ( i * 2.0f + 1.0f ) * std::numbers::pi / 4.0f ) + angle_offset / 2.0f;
							g_foreground->PathArcTo( center, radius, a_min, a_max, num_segments );
							g_foreground->PathStroke( outline_color, false, thickness + 2.0f );
						}
					}

					for ( int i = 0; i < 4; ++i ) {
						const float a_min = ( ( i * 2.0f + 1.0f ) * std::numbers::pi / 4.0f ) - angle_offset / 2.0f;
						const float a_max = ( ( i * 2.0f + 1.0f ) * std::numbers::pi / 4.0f ) + angle_offset / 2.0f;
						g_foreground->PathArcTo( center, radius, a_min, a_max, num_segments );
						g_foreground->PathStroke( color, false, thickness );
					}
				}

				if ( !g_engine->m_in_lobby ) {

					/*if ( config::self::bullet_tracers ) {
						g_world->update_tracer( );

						auto tracer_list = g_world->get_tracer_list( );
						std::vector<lists::c_bullet_tracer> updated_tracers;
						updated_tracers.reserve( tracer_list.size( ) );

						for ( auto& tracer : tracer_list ) {
							if ( !tracer.m_start_location.is_valid( ) || !tracer.m_end_location.is_valid( ) ) {
								continue;
							}

							tracer.m_lifetime += ImGui::GetIO( ).DeltaTime;
							tracer.m_alpha = max( 0.0f, 1.0f - ( tracer.m_lifetime / config::self::fadeout_time ) );

							if ( tracer.m_alpha <= 0.0f || !std::isfinite( tracer.m_alpha ) ) continue;

							auto distance = tracer.m_start_location.distance_to( tracer.m_end_location );
							if ( !std::isfinite( distance ) || distance <= 0.0f ) continue;

							auto thickness = min( 2.0, max( 0.5, 2.0 - ( distance / 1000.0 ) ) );
							if ( !std::isfinite( thickness ) ) thickness = 1.0;

							auto start_screen = engine::world_to_screen( tracer.m_start_location );
							auto end_screen = engine::world_to_screen( tracer.m_end_location );
							if ( start_screen.is_valid( ) && end_screen.is_valid( ) ) {
								if ( tracer.m_trail_points.size( ) > 1 ) {
									auto trail_size = static_cast< float >( tracer.m_trail_points.size( ) );
									for ( size_t i = 0; i < tracer.m_trail_points.size( ) - 1; i++ ) {
										if ( !tracer.m_trail_points[ i ].is_valid( ) || !tracer.m_trail_points[ i + 1 ].is_valid( ) ) continue;

										auto trail_start = engine::world_to_screen( tracer.m_trail_points[ i ] );
										auto trail_end = engine::world_to_screen( tracer.m_trail_points[ i + 1 ] );

										if ( trail_start.is_valid( ) && trail_end.is_valid( ) ) {
											auto fade_factor = 1.0f - static_cast< float >( i ) / trail_size;
											auto trail_alpha = min( 1.0f, max( 0.0f, tracer.m_alpha * fade_factor ) );
											auto trail_thickness = thickness * fade_factor;

											if ( std::isfinite( trail_alpha ) && trail_alpha > 0.0f &&
												std::isfinite( trail_thickness ) && trail_thickness > 0.0f ) {

												auto trail_color = ImColor( 195, 177, 225, static_cast< int >( trail_alpha * 255 ) );
												g_foreground->AddLine( trail_start.vec( ), trail_end.vec( ), trail_color, trail_thickness );
											}
										}
									}
								}

								if ( std::isfinite( tracer.m_alpha ) && tracer.m_alpha > 0.0f &&
									std::isfinite( thickness ) && thickness > 0.0f ) {

									auto clamped_alpha = min( 1.0f, max( 0.0f, tracer.m_alpha ) );
									auto tracer_color = ImColor( 144, 154, 251, static_cast< int >( clamped_alpha * 255 ) );

									g_foreground->AddLine( start_screen.vec( ), end_screen.vec( ), tracer_color, thickness );

									auto circle_thickness = thickness * 2.0f;
									if ( std::isfinite( circle_thickness ) && circle_thickness > 0.0f ) {
										g_foreground->AddCircle( end_screen.vec( ), 3.0f, tracer_color, 0, circle_thickness );
									}
								}

							}

							updated_tracers.emplace_back( std::move( tracer ) );
						}

						g_world->update_tracer_list( std::move( updated_tracers ) );
					}*/

					if ( config::self::zone_prediction ) {
						message::message_info_t zone_info;
						zone_info.title = "Safe Zone";
						zone_info.type = message::e_message_type::MESSAGE_TYPE_GENERAL;
						zone_info.icon_text = ICON_THUNDERSTORM_FILL;

						auto safe_zone = g_engine->m_zone;
						if ( safe_zone.m_damage > 0.0 ) {
							zone_info.entries.emplace_back(
								"Current Phase",
								std::format( "{}/{}", safe_zone.m_phase, safe_zone.m_phase_count ),
								ImColor( 220, 220, 220, 255 )
							);

							zone_info.entries.emplace_back(
								"Damage",
								std::format( "{}", safe_zone.m_damage * 100 ),
								ImColor( 220, 220, 220, 255 )
							);
						}
						else {
							zone_info.entries.emplace_back(
								"Zone Status",
								"No active zone",
								ImColor( 220, 220, 220, 255 )
							);
						}

						g_message->add_message( zone_info );

						if ( config::self::zone_prediction ) {
							auto safe_zone = g_engine->m_zone;
							auto center = safe_zone.m_center;
							auto height = safe_zone.m_height;
							auto radius = safe_zone.m_radius;

							g_zone->draw_zone( center, radius, height );

							if ( config::self::zone_center ) {
								auto zone_name = std::format( "Zone {}/{}", safe_zone.m_phase, safe_zone.m_phase_count );
								g_zone->draw_zone_center( center, zone_name );
							}

							if ( config::self::show_next_zone ) {
								auto next_center = safe_zone.m_next_center;
								auto next_radius = safe_zone.m_next_radius;

								g_zone->draw_next_zone( next_center, next_radius, height );

								if ( config::self::zone_center ) {
									auto zone_name = std::format( "Safe Zone" );
									g_zone->draw_zone_center( next_center, zone_name );
								}
							}
						}
					}

					if ( config::self::spectators ) {
						message::message_info_t spectator_info;
						spectator_info.title = "Spectators";
						spectator_info.type = message::e_message_type::MESSAGE_TYPE_GENERAL;
						spectator_info.icon_text = ICON_GROUP_3_FILL;

						auto spectator_list = g_world->get_spectator_list( );
						if ( !spectator_list.empty( ) ) {
							for ( auto& spectator : spectator_list ) {
								if ( !spectator.m_state ) continue;

								spectator_info.entries.emplace_back(
									std::format( "{} ({})", spectator.m_spectator_name, spectator.m_index ),
									esp::get_platform_name( spectator.m_platform_name ),
									esp::get_platform_color( spectator.m_platform_name )
								);
							}

							g_message->add_message( spectator_info );
						}
					}

					if ( config::self::inventory ) {
						message::message_info_t inventory_info;
						inventory_info.title = "Quick Bar";
						inventory_info.type = message::e_message_type::MESSAGE_TYPE_GENERAL;
						inventory_info.icon_text = ICON_SWORD_FILL;

						std::vector<std::tuple<std::string, std::string, ImColor>> entries;

						auto inventory_list = g_world->get_inventory_list( );
						if ( !inventory_list.empty( ) ) {
							for ( auto& inventory : inventory_list ) {
								if ( inventory.m_weapon_name.empty( ) ) continue;

								auto magazine_size = inventory.get_magazine_size( );
								auto magazine_text = magazine_size ? std::format( "{}", magazine_size ) : std::format( "Stack" );

								auto desc_text = std::string( );
								auto text_color = esp::get_tier_color( inventory.m_weapon_rarity );
								if ( inventory.m_weapon_name.contains( "Pickaxe" ) ) {
									desc_text = std::format( "Harvesting Tool" );
									text_color = ImColor( 220, 220, 220, 255 );
								}

								entries.emplace_back(
									inventory.m_weapon_name,
									desc_text.empty( ) ? magazine_text : desc_text,
									text_color
								);
							}
						}

						inventory_info.entries = std::move( entries );
						g_message->add_message( inventory_info );
					}
				}

				if ( config::radar::enabled ) {
					g_radar->draw_background( );
				}

				g_world->loop_buildings( enviroment::render_building );
				g_world->loop_projectiles( enviroment::render_projectile );
				g_world->loop_vehicles( enviroment::render_vehicle );
				g_world->loop_containers( enviroment::render_container );
				g_world->loop_pickups( enviroment::render_pickup );
				g_world->loop_weakspots( enviroment::render_weakspot );
				g_world->loop_supply_drops( enviroment::render_supply_drop );
				g_world->loop_loot_llamas( enviroment::render_loot_llama );
				g_world->loop_actors( player::render );
				ImGui::PopFont( );

				g_arraylist->render( );
				g_targeting->render( );
				g_message->render( );
				g_notification->render( );

				static bool interface_open = true;
				if ( GetAsyncKeyState( VK_INSERT ) & 1 )
					interface_open = !interface_open;
				if ( interface_open ) {
					if ( g_dashboard->m_home_active ) {
						g_menu->render( );
					}

					g_dashboard->render( );
					g_dashboard->draw_leaderboard( );
					g_dashboard->draw_settings( );
				}

				g_particle->render( g_foreground, ImVec2( g_overlay->m_width, g_overlay->m_height ), 24.f );
				g_overlay->draw_frame( );
				g_profile->update( );
			}

			g_overlay->destroy( );
		}

	private:
		bool m_user_welcome = false;
	};
}