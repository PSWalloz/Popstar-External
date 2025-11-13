#pragma once 

namespace targeting {
	class c_targeting {
	public:
		c_targeting( ) {
			InitializeSRWLock( &m_targeting_lock );
			this->reset( );
		}

		void tick( ) {
			while ( true ) {
				if ( !config::aimbot::enabled || !g_local_pawn->m_actor ) {
					this->reset( );
					utility::sleep_short( 5 );
					continue;
				}

				g_weapon->apply_config( );
				g_selection->update_locked_target( );
				
				g_world->loop_actors( [ this ]( const lists::c_player& player ) {
					if ( !g_selection->is_valid( player ) )
						return;

					if ( g_weapon->m_no_target_switch && g_target.m_player.m_actor && g_target.m_type == target_type::player ) {
						return;
					}

					g_selection->process_player( player );
					} );

				g_world->loop_weakspots( [ this ]( const lists::c_weakspot& weakspot ) {
					if ( !g_selection->is_valid( weakspot ) )
						return;

					if ( g_weapon->m_no_target_switch && g_target.m_weakspot.m_actor && g_target.m_type == target_type::weakspot ) {
						return;
					}

					g_selection->process_weakspot( weakspot );
					} );

				if ( g_target.m_type != target_type::none ) {
					g_weapon->apply_close_config( );

					if ( g_target.m_type == target_type::player && g_target.m_player.m_actor ) {
						if ( !g_selection->is_valid( g_target.m_player ) ) {
							this->reset( );
							continue;
						}

						g_target.m_aim_distance = g_target.m_player.m_distance;
						g_target.m_aim_location = this->get_aim_location( );
						if ( !g_target.m_aim_location.is_valid( ) ) {
							this->reset( );
							continue;
						}

						if ( g_weapon->m_aim_prediction[ 0 ] ) {
							prediction::apply_delta( g_target.m_aim_location );
						}
						if ( g_weapon->m_aim_prediction[ 1 ] ) {
							prediction::apply_bullet( g_target.m_aim_location );
						}

						g_target.m_aim_screen = engine::project( g_target.m_aim_location );
					}
					else if ( g_target.m_type == target_type::weakspot && g_target.m_weakspot.m_actor ) {
						if ( !g_selection->is_valid( g_target.m_weakspot ) ) {
							this->reset( );
							continue;
						}

						g_target.m_aim_distance = g_target.m_weakspot.m_distance;
						g_target.m_aim_location = g_target.m_weakspot.m_world_location;
						if ( !g_target.m_aim_location.is_valid( ) ) return;

						g_target.m_aim_screen = engine::project( g_target.m_aim_location );
					}

					if ( !g_target.m_aim_screen.is_valid( ) ) {
						this->reset( );
						continue;
					}

					if ( g_weapon->m_inverted_vertical_axis ) {
						g_target.m_aim_screen.y = -g_target.m_aim_screen.y;
					}

					if ( config::triggerbot::enabled ) {
						g_triggerbot->tick( ( g_target.m_type == target_type::player ) ? g_target.m_player.m_is_visible : true );
					}

					auto is_pressing = this->is_pressing( );
					if ( is_pressing ) {
						if ( g_weapon->m_silent_aim ) {
							auto randomized_location = g_smoothing->apply_hit_chance( g_target.m_aim_location );
							this->move_crosshair( get_aim_rotation( randomized_location ) );
						}
						else {
							auto smoothed_location = g_smoothing->apply( g_target.m_aim_location, g_target.m_aim_screen );
							if ( !smoothed_location.is_valid( ) ) {
								this->reset( );
								continue;
							}

							this->move_mouse( get_aim_rotation( smoothed_location ) );
						}
					}
					else {
						this->reset( );
					}
				}

				utility::sleep_short( 5 );
			}
		}

		void render( ) {
			if ( g_weapon->m_fov_circle && g_target.m_aimbot_fov > 0.0f ) {
				g_background->AddCircle(
					ImVec2( g_overlay->m_width_center, g_overlay->m_height_center ),
					g_target.m_aimbot_fov,
					ImColor( 255, 255, 255, 100 ),
					0,
					1.5f
				);
			}

			if ( g_target.m_type != target_type::none ) {
				auto distance = g_target.m_aim_distance;
				auto aim_location = g_target.m_aim_location;

				auto aim_screen = engine::project( aim_location );
				if ( aim_screen.is_valid( ) ) {
					if ( g_weapon->m_aim_curve ) {
						auto start_point = ImVec2( g_overlay->m_width_center, g_overlay->m_height_center );
						auto end_point = ImVec2( aim_screen.x, aim_screen.y );
						auto distance = sqrt( pow( end_point.x - start_point.x, 2 ) + pow( end_point.y - start_point.y, 2 ) );
						auto control_points_ptr = g_smoothing->m_render_control_points.load( );

						if ( control_points_ptr && !control_points_ptr->empty( ) ) {
							const auto& control_points = *control_points_ptr;
							if ( g_weapon->m_target_dot ) {
								auto dot_color = g_target.m_type == target_type::player ?
									ImColor( 163, 75, 180, 225 ) : ImColor( 142, 130, 254, 225 );

								for ( const auto& point : control_points ) {
									g_background->AddCircleFilled(
										point,
										3.f,
										dot_color,
										8
									);
								}
							}

							if ( g_weapon->m_target_line ) {
								const int segments = 20;
								auto line_color = g_target.m_type == target_type::player ?
									ImColor( 220, 151, 164, 255 ) : ImColor( 255, 165, 0, 255 );

								if ( !control_points.empty( ) ) {
									auto prev_point = control_points[ 0 ];
									for ( int i = 1; i <= segments; i++ ) {
										auto t = static_cast< float >( i ) / segments;
										auto current_point = curves::calculate_bezier_point( control_points, t, { static_cast< float >( g_overlay->m_width_center ), static_cast< float >( g_overlay->m_height_center ) } );

										g_background->AddLine(
											prev_point,
											current_point,
											line_color,
											1.5f
										);

										prev_point = current_point;
									}
								}
							}
						}
					}
					else {
						if ( g_weapon->m_target_line ) {
							auto line_color = g_target.m_type == target_type::player ?
								ImColor( 220, 151, 164, 255 ) : ImColor( 255, 165, 0, 255 );
							g_background->AddLine(
								ImVec2( g_overlay->m_width_center, g_overlay->m_height_center ),
								aim_screen.vec( ),
								line_color,
								1.5f
							);
						}
					}

					if ( g_weapon->m_deadzone ) {
						if ( g_weapon->m_deadzone_dot ) {
							auto scaled_restriction = g_weapon->m_restriction * ( 1.0f + ( 50.0f / max( distance, 1.0f ) ) );
							auto deadzone_color = g_target.m_type == target_type::player ?
								ImColor( 255, 0, 0, 100 ) : ImColor( 255, 165, 0, 100 );

							auto filled_color = g_target.m_type == target_type::player ?
								ImColor( 255, 0, 0, 30 ) : ImColor( 255, 165, 0, 30 );

							g_background->AddCircle(
								aim_screen.vec( ),
								scaled_restriction,
								deadzone_color,
								0,
								1.0f
							);
							g_background->AddCircleFilled(
								aim_screen.vec( ),
								scaled_restriction,
								filled_color
							);
						}
					}

					if ( g_weapon->m_prediction_x ) {
						auto dot_color = g_target.m_type == target_type::player ?
							ImColor( 176, 49, 44, 150 ) : ImColor( 142, 130, 254, 105 );

						auto predicted_location = g_target.m_aim_location;
						auto predicted_screen = engine::project( predicted_location );
						if ( predicted_screen.is_valid( ) ) {
							const float size = 5.0f;
							auto center = predicted_screen.vec( );

							g_background->AddLine(
								ImVec2( center.x - size, center.y - size ),
								ImVec2( center.x + size, center.y + size ),
								dot_color, 1.5f
							);
							g_background->AddLine(
								ImVec2( center.x + size, center.y - size ),
								ImVec2( center.x - size, center.y + size ),
								dot_color, 1.5f
							);
						}
					}
				}
			}

			if ( config::triggerbot::enabled && config::triggerbot::deadzone ) {
				auto center_x = g_overlay->m_width_center;
				auto center_y = g_overlay->m_height_center;
				auto deadzone_color = ImColor( 255, 165, 0, 120 );
				auto outline_color = ImColor( 255, 165, 0, 200 );

				if ( config::triggerbot::deadzone_circular ) {
					g_background->AddCircleFilled(
						ImVec2( center_x, center_y ),
						config::triggerbot::deadzone_radius,
						deadzone_color
					);
					g_background->AddCircle(
						ImVec2( center_x, center_y ),
						config::triggerbot::deadzone_radius,
						outline_color,
						0,
						1.5f
					);
				}
				else {
					auto half_width = config::triggerbot::deadzone_width / 2.0f;
					auto half_height = config::triggerbot::deadzone_height / 2.0f;

					g_background->AddRectFilled(
						ImVec2( center_x - half_width, center_y - half_height ),
						ImVec2( center_x + half_width, center_y + half_height ),
						deadzone_color
					);
					g_background->AddRect(
						ImVec2( center_x - half_width, center_y - half_height ),
						ImVec2( center_x + half_width, center_y + half_height ),
						outline_color,
						0.0f,
						0,
						1.5f
					);
				}
			}
		}

		SRWLOCK m_targeting_lock;

	private:
		bool is_pressing( ) const {
			bool key_pressed = false;
			if ( g_weapon->m_aim_key != 0 ) {
				key_pressed = ( GetAsyncKeyState( g_weapon->m_aim_key ) & 0x8000 ) != 0;
			}

			bool trigger_pressed = g_controller->is_trigger_pressed( 0.3f );

			return key_pressed || trigger_pressed;
		}

		void reset( ) {
			g_target.m_type = target_type::none;

			g_selection->reset( );
			g_selection->reset_target_lock( );
			g_smoothing->reset( );
			g_triggerbot->reset( );

			if ( m_silent_applied ) {
				move_crosshair( engine::f_rotator( ) );
				m_silent_applied = false;
			}
		}

	private:
		bool m_silent_applied = false;

		void move_mouse( engine::f_rotator aim_rotation ) {
			auto player_controller = g_engine->m_player_controller;
			if ( !player_controller )
				return;

			player_controller->player_aim_offset( aim_rotation );
		}

		void move_crosshair( engine::f_rotator aim_rotation ) {
			auto player_controller = g_engine->m_player_controller;
			if ( !player_controller )
				return;

			player_controller->additional_aim_offset( aim_rotation );
			m_silent_applied = true;
		}

		engine::f_rotator get_aim_rotation( engine::f_vector aim_location ) {
			auto aim_rotation = engine::find_look_at_rotation( g_camera->m_location, aim_location );
			aim_rotation -= g_camera->m_rotation;
			aim_rotation.normalize( );

			if ( g_weapon->m_inverted_vertical_axis ) {
				aim_rotation.pitch = -aim_rotation.pitch;
			}

			return aim_rotation;
		}

		engine::f_vector get_aim_location( ) const {
			std::vector<engine::e_bone> bone_pool;

			auto mesh = g_target.m_player.m_mesh;
			if ( !mesh ) return {};

			auto bone_array = g_target.m_player.m_bone_array;
			if ( !bone_array.is_valid( ) ) return {};

			if ( g_weapon->m_aim_bone[ 0 ] ) {
				bone_pool.push_back( engine::e_bone::vb_head_fx );
			}
			if ( g_weapon->m_aim_bone[ 1 ] ) {
				bone_pool.push_back( engine::e_bone::neck_01 );
			}
			if ( g_weapon->m_aim_bone[ 2 ] ) {
				bone_pool.push_back( engine::e_bone::neck_02 );
			}
			if ( g_weapon->m_aim_bone[ 3 ] ) {
				bone_pool.push_back( engine::e_bone::spine_01 );
			}

			if ( !bone_pool.empty( ) ) {
				if ( g_weapon->m_linear_aim ) {
					auto start_bone = mesh->get_bone_location( bone_array, bone_pool.front( ) );
					auto end_bone = mesh->get_bone_location( bone_array, bone_pool.back( ) );

					static float last_time = 0.0f;
					static float current_t = 0.0f;
					static bool increasing = true;

					auto current_time = static_cast< float >( std::chrono::duration_cast< std::chrono::milliseconds >(
						std::chrono::steady_clock::now( ).time_since_epoch( )
					).count( ) ) / 1000.0f;

					if ( last_time == 0.0f ) {
						last_time = current_time;
					}

					float delta_time = current_time - last_time;
					last_time = current_time;

					float distance = start_bone.distance_to( end_bone );
					float speed = std::clamp( 1.0f / ( distance * 0.1f ), 0.1f, 2.0f );

					if ( increasing ) {
						current_t += delta_time * speed;
						if ( current_t >= 1.0f ) {
							current_t = 1.0f;
							increasing = false;
						}
					}
					else {
						current_t -= delta_time * speed;
						if ( current_t <= 0.0f ) {
							current_t = 0.0f;
							increasing = true;
						}
					}

					float smooth_t = current_t * current_t * ( 3.0f - 2.0f * current_t );

					return engine::f_vector(
						start_bone.x + ( end_bone.x - start_bone.x ) * smooth_t,
						start_bone.y + ( end_bone.y - start_bone.y ) * smooth_t,
						start_bone.z + ( end_bone.z - start_bone.z ) * smooth_t
					);
				}
				else {
					auto index = rand( ) % bone_pool.size( );
					return mesh->get_bone_location( bone_array, bone_pool[ index ] );
				}
			}

			return {};
		}
	};
}