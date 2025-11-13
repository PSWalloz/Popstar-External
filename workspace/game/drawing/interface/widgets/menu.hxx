#pragma once
#include <Windows.h>
#include <commdlg.h>
#include <string>
#include <vector>

int rotation_start_index;
void ImRotateStart( ) {
	rotation_start_index = ImGui::GetWindowDrawList( )->VtxBuffer.Size;
}

ImVec2 ImRotationCenter( ) {
	ImVec2 l( FLT_MAX, FLT_MAX ), u( -FLT_MAX, -FLT_MAX );

	const auto& buf = ImGui::GetWindowDrawList( )->VtxBuffer;
	for ( int i = rotation_start_index; i < buf.Size; i++ )
		l = ImMin( l, buf[ i ].pos ), u = ImMax( u, buf[ i ].pos );

	return ImVec2( ( l.x + u.x ) / 2, ( l.y + u.y ) / 2 );
}

namespace menu {
	custom::custom_popup acceleration_popup( "Aim Acceleration" );
	custom::custom_popup deadzone_popup( "Aim Deadzone" );
	custom::custom_popup visual_popup( "Aim Customization" );
	custom::custom_popup chinese_popup( "Chinese Hat" );
	custom::custom_popup skeleton_popup( "Skeleton" );

	class c_menu {
	public:
		bool setup( ) {
			m_tabs_info.push_back( { "Combat", {"Aim Assistance", "Close Aim", "Trigger Bot"} } );
			m_tabs_info.push_back( { "Visuals", {"Players", "Radar", "World"} } );
			m_tabs_info.push_back( { "Miscellaneous ", {"Misc", "Exploits", "Configuration"} } );

			this->m_tabs = std::make_unique< c_tabs >( m_tabs_info );
			this->m_animated_bg = std::make_unique< c_animated_bg >( );

			std::thread( [ this ]( ) {
				this->m_version = g_auth->get_variable( "version" );
				this->m_detection = g_auth->get_variable( "detection" );
				this->m_status = g_auth->get_variable( "status" );
				} ).detach( );

			return true;
		}

		void render( ) {
			ImGuiStyle& s = ImGui::GetStyle( );

			static bool set_ts = false;
			if ( !set_ts ) {
				ImGui::SetNextWindowPos( ImVec2( g_overlay->m_width_center, g_overlay->m_height_center - c::bg::size.y ) );
				set_ts = true;
			}

			ImGui::SetNextWindowSize( c::bg::size + ImVec2( 280, 0 ) );
			Begin( "MakFN External For Fortnite", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground );
			{
				c::anim::speed = ImGui::GetIO( ).DeltaTime * 14.f;
				c::second_color = ImColor( 155, 150, 219, 255 ) /*utils::GetDarkColor( c::main_color )*/;

				const ImVec2& pos = ImGui::GetWindowPos( );
				const ImVec2& region = ImGui::GetContentRegionMax( );
				const ImVec2& spacing = s.ItemSpacing;

				// Add glow highlight around entire menu
				GetBackgroundDrawList( )->AddShadowRect(
					pos,
					pos + c::bg::size + ImVec2( 280, 0 ),
					ImColor( 135, 122, 233, 100 ),
					25.0f,
					ImVec2( 0.0f, 0.0f ),
					c::bg::rounding
				);


				GetBackgroundDrawList( )->AddRectFilled( pos, pos + ImVec2( 200, c::bg::size.y ),
					utils::GetColorWithAlpha( c::header_bg_color, c::header_bg_color.Value.w * s.Alpha ),
					c::bg::rounding, ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBottomLeft );

				GetBackgroundDrawList( )->AddRectFilled( pos + ImVec2( 200, 0 ), pos + ImVec2( 200, 0 ) + ImVec2( c::bg::size.x + 80, 60 ),
					utils::GetColorWithAlpha( c::window_bg_color, c::window_bg_color.Value.w * s.Alpha ),
					c::bg::rounding, ImDrawFlags_RoundCornersTopRight );

				GetBackgroundDrawList( )->AddRectFilled( pos + ImVec2( 200, 60 ), pos + ImVec2( 200, 60 ) + ImVec2( c::bg::size.x + 80, c::bg::size.y - 60 ),
					utils::GetColorWithAlpha( c::window_bg_color, c::window_bg_color.Value.w * s.Alpha ),
					c::bg::rounding, ImDrawFlags_RoundCornersBottomRight );

				// Add outline around entire menu
				GetBackgroundDrawList( )->AddRect( pos, pos + c::bg::size + ImVec2( 280, 0 ),
					c::outline_color, c::bg::rounding, 0, 2.0f );

				ImVec2 status_pos = pos + ImVec2( 15 + 12, c::bg::size.y - 71 );  // 40px above version
				ImVec2 status_text_size = ImGui::CalcTextSize( ("Status: " + m_status).c_str( ) );
				ImVec2 status_circle_pos = pos + ImVec2( 15, c::bg::size.y - 71 + status_text_size.y * 0.5f );
				
				// Draw status circle
				GetBackgroundDrawList( )->AddCircleFilled( status_circle_pos, 4.0f, ImColor( 246, 92, 85, 255 ), 12 );
				// Draw status text using gradient
				GetBackgroundDrawList( )->AddText( status_pos, c::label::default_color, "Status: " );
				custom::fade_text( GetBackgroundDrawList( ), m_status.c_str( ), 
					status_pos + ImVec2(ImGui::CalcTextSize("Status: ").x, 0), 
					c::anim::active, ImColor(135, 122, 233, 255), 0 );

				// Draw Detection (middle line)
				ImVec2 detection_pos = pos + ImVec2( 15 + 12, c::bg::size.y - 50 );  // 20px above version
				ImVec2 detection_text_size = ImGui::CalcTextSize( ("Detection: " + m_detection).c_str( ) );
				ImVec2 detection_circle_pos = pos + ImVec2( 15, c::bg::size.y - 50 + detection_text_size.y * 0.5f );
				
				// Draw detection circle
				GetBackgroundDrawList( )->AddCircleFilled( detection_circle_pos, 4.0f, ImColor( 254, 191, 33, 255 ), 12 );
				// Draw detection text using gradient
				GetBackgroundDrawList( )->AddText( detection_pos, c::label::default_color, "Detection: " );
				custom::fade_text( GetBackgroundDrawList( ), m_detection.c_str( ), 
					detection_pos + ImVec2(ImGui::CalcTextSize("Detection: ").x, 0), 
					c::anim::active, ImColor(135, 122, 233, 255), 0 );

				// Draw Version (bottom line - original position)
				ImVec2 version_pos = pos + ImVec2( 15 + 12, c::bg::size.y - 30 );  // Original position
				ImVec2 version_size = ImGui::CalcTextSize( m_version.c_str( ) );
				ImVec2 circle_pos = pos + ImVec2( 15, c::bg::size.y - 30 + version_size.y * 0.5f );  // Dot on the left
				float circle_radius = 4.0f;
				GetBackgroundDrawList( )->AddCircleFilled( circle_pos, circle_radius, ImColor( 40, 201, 67, 255 ), 12 );
				
				// Draw version text
				GetBackgroundDrawList( )->AddText( version_pos, c::label::default_color, m_version.c_str( ) );

				//GetBackgroundDrawList( )->AddImage( texture::avatar_image, pos + ImVec2( c::bg::size.x - 55, 15 ), pos + ImVec2( c::bg::size.x - 15, 55 ) );

				//GetBackgroundDrawList( )->AddText( pos + ImVec2( c::bg::size.x - 70 - CalcTextSize( "LeProxy" ).x, 18 ), c::label::active, "LeProxy" );

				//PushFont( font::esp_font );
				//GetBackgroundDrawList( )->AddText( pos + ImVec2( c::bg::size.x - 70 - CalcTextSize( "Administrator" ).x, 37 ), c::label::default_color, "Administrator" );
				//PopFont( );

				m_tabs->DrawTabs( );

				ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 10.0f, 5.0f ) ); // 10px горизонтальный отступ, 5px вертикальный

				ImGui::SetCursorPos( ImVec2( 210.f, 20 ) );
				ImGui::BeginChild( "FRAMECHILD", ImVec2( 1270 * 2 - 40, c::bg::size.y - 35 ), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
				{
					if ( m_tabs->IsTabActive( 0 ) ) {
						ImGuiContext& g = *GImGui;
						ImGuiWindow* window = g.CurrentWindow;

						ImGui::BeginGroup( ); {
							custom::Child( "Aim Config:Aim Features", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_SWORD_FILL );

							const char* weapon_configs[ ]{ "General", "Rifles", "Shotguns" , "SMGs", "Pistols", "Snipers" };
							custom::Combo( "Select Gun", &config::aimbot::weapon_configs, weapon_configs, IM_ARRAYSIZE( weapon_configs ) );
							if ( config::aimbot::weapon_configs )
								custom::Checkbox( "Override General", &config::aimbot::override_general[ config::aimbot::weapon_configs ] );

							custom::Checkbox( "Silent Aim", &config::aimbot::silent_aim[ config::aimbot::weapon_configs ] );
							if ( config::aimbot::silent_aim[ config::aimbot::weapon_configs ] ) {
								custom::SliderInt( "Hit Chance", &config::aimbot::hit_chance[ config::aimbot::weapon_configs ], 1, 100 );
							}

							custom::Checkbox( "Smart Targeting", &config::aimbot::smart_targeting[ config::aimbot::weapon_configs ] );

							custom::Checkbox( "Aim Curve", &config::aimbot::aim_curve[ config::aimbot::weapon_configs ] );
							custom::Checkbox( "Aim Between Bones", &config::aimbot::linear_aim[ config::aimbot::weapon_configs ] );

							custom::Featuresbox( "Aim Acceleration", &config::aimbot::aim_acceleration[ config::aimbot::weapon_configs ], acceleration_popup );
							custom::Featuresbox( "Aim Deadzone", &config::aimbot::deadzone[ config::aimbot::weapon_configs ], deadzone_popup );

							const char* aim_prediction[ ]{ "Delta", "Bullet" };
							custom::MultiCombo( "Aim Prediction", config::aimbot::aim_prediction[ config::aimbot::weapon_configs ], aim_prediction, IM_ARRAYSIZE( aim_prediction ) );
							custom::SliderInt( "Smoothing", &config::aimbot::smoothing[ config::aimbot::weapon_configs ], 0, 50 );

							custom::EndChild( );
						} ImGui::EndGroup( );

						ImGui::SameLine( );

						ImGui::BeginGroup( ); {
							if ( custom::Child( "Customization:Aim Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, true, ICON_TARGET_FILL ) ) {

								custom::Checkbox( "Only Visible", &config::aimbot::only_visible[ config::aimbot::weapon_configs ] );
								custom::Checkbox( "Ignore Invulnerable", &config::aimbot::ignore_invulnerable[ config::aimbot::weapon_configs ] );
								custom::Checkbox( "Ignore Downed", &config::aimbot::ignore_downed[ config::aimbot::weapon_configs ] );
								custom::Checkbox( "Ignore Bots", &config::aimbot::ignore_bots[ config::aimbot::weapon_configs ] );

								const char* aim_bone[ ]{ "Head", "Neck", "Chest", "Middle Chest", "Lower Chest" };
								custom::MultiCombo( "Aim Bone", config::aimbot::aim_bone[ config::aimbot::weapon_configs ], aim_bone, IM_ARRAYSIZE( aim_bone ) );

								static int mode = 0;
								custom::Keybind( "Aim Key", &config::aimbot::aim_key[ config::aimbot::weapon_configs ], &mode );
								custom::SliderInt( "Aim Distance", &config::aimbot::max_distance[ config::aimbot::weapon_configs ], 50.0f, 500.0f );

								//custom::Popupbox( "Aim Visuals", visual_popup );
								custom::Checkbox( "Target Dot", &config::aimbot::target_dot[ config::aimbot::weapon_configs ] );
								custom::Checkbox( "Target Line", &config::aimbot::target_line[ config::aimbot::weapon_configs ] );

								custom::Checkbox( "FOV Circle", &config::aimbot::fov_circle[ config::aimbot::weapon_configs ] );
								custom::SliderFloat( "FOV Radius", &config::aimbot::fov_radius[ config::aimbot::weapon_configs ], 0, 120 );

								custom::EndChild( );
							}
						}
						ImGui::EndGroup( );
					}

					if ( m_tabs->IsTabActive( 1 ) ) {
						const auto max_close_aim = 5;

						ImGui::BeginGroup( ); {
							custom::Child( "Close Aim:Aim Features", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_SWORD_FILL );

							custom::Checkbox( "Close Aim", &config::aimbot::close_aim );
							custom::Checkbox( "Silent Aim", &config::aimbot::silent_aim[ max_close_aim ] );
							custom::Checkbox( "Shotgun Only", &config::aimbot::shotgun_only );
							custom::Checkbox( "No Target Switch", &config::aimbot::no_target_switch[ max_close_aim ] );
							custom::Checkbox( "Aim Between Bones", &config::aimbot::linear_aim[ max_close_aim ] );
							custom::Checkbox( "Inverted Vertical Axis", &config::aimbot::inverted_vertical_axis[ max_close_aim ] );
							custom::Checkbox( "Aim Deadzone", &config::aimbot::deadzone[ max_close_aim ] );
							if ( config::aimbot::deadzone[ max_close_aim ] )
								custom::SliderFloat( "Restriction", &config::aimbot::restriction[ max_close_aim ], 0, 25 );
							custom::SliderInt( "Smoothing", &config::aimbot::smoothing[ max_close_aim ], 0, 50 );

							custom::EndChild( );
						}
						ImGui::EndGroup( );

						ImGui::SameLine( );

						ImGui::BeginGroup( ); {
							if ( custom::Child( "Customization:Aim Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, true, ICON_TARGET_FILL ) ) {

								custom::Checkbox( "Only Visible", &config::aimbot::only_visible[ config::aimbot::weapon_configs ] );
								custom::Checkbox( "Ignore Invulnerable", &config::aimbot::ignore_invulnerable[ config::aimbot::weapon_configs ] );
								custom::Checkbox( "Ignore Downed", &config::aimbot::ignore_downed[ config::aimbot::weapon_configs ] );
								custom::Checkbox( "Ignore Bots", &config::aimbot::ignore_bots[ config::aimbot::weapon_configs ] );

								const char* aim_bone[ ]{ "Head", "Neck", "Chest", "Middle Chest", "Lower Chest" };
								custom::MultiCombo( "Aim Bone", config::aimbot::aim_bone[ config::aimbot::weapon_configs ], aim_bone, IM_ARRAYSIZE( aim_bone ) );

								static int mode = 0;
								custom::Keybind( "Aim Key", &config::aimbot::aim_key[ config::aimbot::weapon_configs ], &mode );
								custom::SliderInt( "Aim Distance", &config::aimbot::max_distance[ config::aimbot::weapon_configs ], 50.0f, 500.0f );

								custom::EndChild( );
							}
						}
						ImGui::EndGroup( );
					}

					if ( m_tabs->IsTabActive( 2 ) ) {
						ImGui::BeginGroup( ); {
							custom::Child( "Trigger Bot:Auto-Shoot Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_EYE_FILL );
							custom::Checkbox( "Trigger bot", &config::triggerbot::enabled );
							custom::Checkbox( "Fire Key", &config::triggerbot::fire_key );
							static int mode = 0;
							custom::Keybind( "Keybind", &config::triggerbot::key_bind, &mode );
							custom::Checkbox( "Ignore Knocked", &config::triggerbot::ignore_knocked );
							custom::SliderInt( "Fire Delay", &config::triggerbot::delay, 1, 5 );
							custom::SliderInt( "Max Distance", &config::triggerbot::max_distance, 0, 50.0 );
							const char* weapon_configs[ ]{ "Rifles", "Shotguns" , "SMGs", "Pistols", "Snipers" };
							custom::MultiCombo( "Fire Weapon", config::triggerbot::weapon_config, weapon_configs, IM_ARRAYSIZE( weapon_configs ) );

							custom::EndChild( );
						}
						ImGui::EndGroup( );
						ImGui::SameLine( );

						ImGui::BeginGroup( ); {
							custom::Child( "Fire Deadzone:Auto-Shoot Deadzone", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, true, ICON_EYE_FILL );
							custom::Checkbox( "Deadzone", &config::triggerbot::deadzone );
							custom::ToggleButton( "Circular", "Rectangular", &config::triggerbot::deadzone_circular, ImVec2( ImGui::GetContentRegionAvail( ).x, 40 ) );

							if ( config::triggerbot::deadzone_circular ) {
								custom::SliderFloat( "Deadzone Radius", &config::triggerbot::deadzone_radius, 0, 25 );
							}
							else {
								custom::SliderInt( "Deadzone Width", &config::triggerbot::deadzone_width, 0, 20 );
								custom::SliderInt( "Deadzone Height", &config::triggerbot::deadzone_height, 0, 20 );
							}

							custom::EndChild( );
						}
						ImGui::EndGroup( );
					}

					if ( m_tabs->IsTabActive( 3 ) ) {
						static int visual_tab = 0;
						custom::SubTab( "General", &visual_tab, 0, c::main_color ); ImGui::SameLine( );
						custom::SubTab( "Safe Zone", &visual_tab, 1, c::main_color ); ImGui::SameLine( );
						custom::SubTab( "Trails", &visual_tab, 2, c::main_color );

						ImGui::SetCursorPosY( 50 + page_offset );

						switch ( visual_tab ) {
						case 0: {
							ImGui::BeginGroup( ); {
								custom::Child( "Player ESP:ESP Features", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_PALETTE_LINE );
								const char* box_esp[ ]{ "None", "Full", "Corner" , "3D" };
								custom::Combo( "Box ESP", &config::enemies::box_esp, box_esp, IM_ARRAYSIZE( box_esp ) );
								const char* box_style[ ]{ "Outlined", "Filled", "Highlight" };
								custom::MultiCombo( "Box Style", config::enemies::box_style, box_style, IM_ARRAYSIZE( box_style ) );
								const char* line_esp[ ]{ "None", "Top", "Middle" , "Bottom" };
								custom::Combo( "Line ESP", &config::enemies::line_esp, line_esp, IM_ARRAYSIZE( line_esp ) );
								//custom::Checkbox( "Weapon Icon", &config::enemies::weapon_icon );
								custom::Checkbox( "Reload Bar", &config::enemies::reload_bar );
								custom::Checkbox( "View Line", &config::enemies::view_line );
								custom::Checkbox( "FOV Arrow", &config::enemies::fov_arrows );
								custom::Featuresbox( "Skeleton", &config::enemies::skeleton, skeleton_popup );
								custom::Featuresbox( "Chinese Hat", &config::enemies::chinse_hat, chinese_popup );

								//custom::Checkbox( "Chams", &config::enemies::stencil );
								//if ( config::enemies::stencil ) {
								//	const char* glow_color[ ]{ "Red Glow", "Blue Glow", "Yellow Glow" };
								//	custom::Combo( "Material", &config::enemies::glow_color, glow_color, IM_ARRAYSIZE( glow_color ) );
								//}

								custom::SliderFloat( "Max Distance", &config::enemies::max_distance, 50.0f, 500.0f );
								custom::EndChild( );
							} ImGui::EndGroup( );
							ImGui::SameLine( );

							ImGui::BeginGroup( ); {
								if ( custom::Child( "Flags:Text Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, true, ICON_EYE_2_FILL, true ) ) {
									custom::Checkbox( "Name", &config::enemies::display_name );
									custom::Checkbox( "Rank", &config::enemies::season_rank );
									custom::Checkbox( "Weapon", &config::enemies::weapon_name );
									custom::Checkbox( "Platform", &config::enemies::platform );
									custom::Checkbox( "Movement", &config::enemies::movement );
									custom::Checkbox( "Distance", &config::enemies::distance );
									custom::Checkbox( "Game Mode", &config::enemies::team_mode );
									custom::Checkbox( "Level", &config::enemies::level );
									custom::Checkbox( "Kills", &config::enemies::kills );
									custom::Checkbox( "Wins", &config::enemies::wins );
									custom::Checkbox( "KDR", &config::enemies::kdr );

									custom::EndChild( );
								}
							} ImGui::EndGroup( );
						} break;
						case 1: {
							ImGui::BeginGroup( ); {
								custom::Child( "Safe Zone:Storm Customization", ImVec2( 705, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.7 ), true, 0, false, ICON_EYE_2_FILL );
								custom::Checkbox( "Storm Prediction", &config::self::zone_prediction );
								custom::Checkbox( "Next Circle", &config::self::show_next_zone );
								custom::Checkbox( "Center", &config::self::zone_center );
								custom::Checkbox( "Line", &config::self::center_line );
								custom::SliderFloat( "Thickness", &config::self::zone_thickness, 0.1f, 6 );
								const char* zone_shape[ ]{ "None", "Dome", "Sphere" };
								custom::Combo( "Shape", &config::self::zone_shape, zone_shape, IM_ARRAYSIZE( zone_shape ) );

								custom::EndChild( );
							} ImGui::EndGroup( );
						} break;
						case 2: {
							ImGui::SetCursorPosY( 50 + page_offset );

							ImGui::BeginGroup( ); {
								custom::Child( "Movement Trails:ESP Features", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_EYE_2_FILL );
								custom::Checkbox( "Movement Trials", &config::enemies::movement_tracers );
								custom::SliderFloat( "Trial Duration", &config::enemies::duration, 1, 3 );
								custom::SliderFloat( "Update Time", &config::enemies::update, 0.1, 1 );
								custom::SliderFloat( "Thickness", &config::enemies::trial_thickness, 0.1f, 2 );
								const char* trial_style[ ]{ "Solid", "Dashed", "Glowing", "Rainbow" };
								custom::Combo( "Trial Style", &config::enemies::trial_style, trial_style, IM_ARRAYSIZE( trial_style ) );
								const char* trial_fade[ ]{ "Linear", "Expontial", "Ease Out", "Pulse" };
								custom::Combo( "Trial Fade", &config::enemies::trial_fade, trial_fade, IM_ARRAYSIZE( trial_fade ) );

								custom::EndChild( );

							} ImGui::EndGroup( );
							ImGui::SameLine( );

							ImGui::BeginGroup( ); {
								if ( custom::Child( "Bullet Tracers:ESP Features", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, true, ICON_EYE_2_FILL, true ) ) {
									custom::Checkbox( "Bullet Tracers", &config::self::bullet_tracers );
									custom::SliderInt( "Max Tracers", &config::self::max_tracers, 1, 10 );
									custom::SliderFloat( "Fadeout Time", &config::self::fadeout_time, 1, 5 );

									custom::EndChild( );
								}
							} ImGui::EndGroup( );

						} break;
						}
					}

					if ( m_tabs->IsTabActive( 4 ) ) {	
						ImGui::BeginGroup( ); {
							custom::Child( "Radar:Radar Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_COMPASS_FILL ); {
								custom::Checkbox( "Radar", &config::radar::enabled );
								custom::Checkbox( "Grid", &config::radar::grid );
								if ( config::radar::grid )
									custom::SliderInt( "Divisions", &config::radar::grid_divisions, 0, 10 );
								const char* position[ ]{ "Draggable", "Centered", "Minimap" };
								custom::Combo( "Position Mode", &config::radar::position, position, IM_ARRAYSIZE( position ) );
								custom::Checkbox( "Rotation Orientated", &config::radar::rotation_mode );
								custom::SliderInt( "Size", &config::radar::size, 0, 800 );
								custom::SliderInt( "Range", &config::radar::range, 0, 1000 );
								custom::SliderInt( "X", &config::radar::position_x, 0, g_overlay->m_width );
								custom::SliderInt( "Y", &config::radar::position_y, 0, g_overlay->m_height );
								custom::EndChild( );
							}
						}
						ImGui::EndGroup( );
						ImGui::SameLine( );

						ImGui::BeginGroup( ); {
							if ( custom::Child( "Radar:Miscellaneous Features", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, true, ICON_SETTINGS_4_FILL, true ) ) {
								custom::Checkbox( "Distance", &config::radar::distance );
								custom::Checkbox( "Line Of Sight", &config::radar::line_of_sight );
								if ( config::radar::line_of_sight )
									custom::Checkbox( "Filled LOS", &config::radar::los_fill );

								//custom::Checkbox( "Storm Overlay", &config::radar::storm_overlay );
								//if ( config::radar::storm_overlay ) {
								//	custom::Checkbox( "Storm Circle", &config::radar::show_zone );
								//	custom::Checkbox( "Next Circle", &config::radar::show_next_zone );
								//	custom::Checkbox( "Indicator", &config::radar::zone_out_of_range_indicator );
								//}

								custom::EndChild( );
							}
						} ImGui::EndGroup( );
					}

					if ( m_tabs->IsTabActive( 5 ) ) {
						static int world_tab = 0;
						custom::SubTab( "General", &world_tab, 0, c::main_color ); ImGui::SameLine( );
						custom::SubTab( "Buildings", &world_tab, 1, c::main_color ); ImGui::SameLine( );
						custom::SubTab( "Crates", &world_tab, 2, c::main_color ); ImGui::SameLine( );
						custom::SubTab( "Loot", &world_tab, 3, c::main_color );

						ImGui::SetCursorPosY( 50 + page_offset );

						switch ( world_tab ) {
						case 0: {
							ImGui::BeginGroup( ); {
								custom::Child( "Vehicles:Vehicle Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_EARTH_2_FILL );
								custom::Checkbox( "Vehicles", &config::vehicles::enabled );
								custom::Checkbox( "Distance", &config::vehicles::show_distance );
								custom::Checkbox( "Terrain", &config::vehicles::terrain );
								custom::Checkbox( "Health", &config::vehicles::health );
								custom::SliderFloat( "Max Distance", &config::vehicles::max_distance, 50, 500 );

								custom::EndChild( );
							} ImGui::EndGroup( );

							ImGui::SameLine( );

							ImGui::BeginGroup( ); {
								if ( custom::Child( "Projectiles:Projectile Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, true, ICON_EARTH_2_FILL, true ) ) {
									custom::Checkbox( "Projectiles", &config::projectiles::enabled );
									custom::Checkbox( "Distance", &config::projectiles::show_distance );
									custom::Checkbox( "Tracers", &config::projectiles::show_tracers );
									custom::Checkbox( "Hit Marker", &config::projectiles::hit_marker );
									custom::SliderFloat( "Max Distance", &config::projectiles::max_distance, 50, 1000 );

									custom::EndChild( );
								}
							} ImGui::EndGroup( );
						} break;
						case 1: {
							ImGui::BeginGroup( ); {
								custom::Child( "Weakspots:Weakspot Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_EARTH_2_FILL );
								custom::Checkbox( "Weakspots", &config::weakspots::enabled );
								custom::Checkbox( "Distance", &config::weakspots::show_distance );
								custom::Checkbox( "Weakspot Aimbot", &config::weakspots::weakspot_aimbot );
								custom::SliderFloat( "Smoothing Modifier", &config::weakspots::smoothing_modifier, 1.f, 10.f );
								custom::SliderFloat( "Max Distance", &config::weakspots::max_distance, 1.f, 50.f );
								custom::EndChild( );
							} ImGui::EndGroup( );
							ImGui::SameLine( );
							ImGui::BeginGroup( ); {
								if ( custom::Child( "Buildings:Building Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, true, ICON_EARTH_2_FILL, true ) ) {
									custom::Checkbox( "Buildings", &config::buildings::enabled );
									custom::Checkbox( "Engine Placed", &config::buildings::engine_placed );
									custom::Checkbox( "Player Placed", &config::buildings::player_placed );
									custom::Checkbox( "Build Type", &config::buildings::build_type );
									custom::Checkbox( "Collision Box", &config::buildings::collision_box );
									custom::Checkbox( "Distance", &config::buildings::show_distance );
									custom::SliderFloat( "Max Distance", &config::buildings::max_distance, 1.f, 50.f );
									custom::EndChild( );
								}
							} ImGui::EndGroup( );
						} break;
						case 2: {
							ImGui::BeginGroup( ); {
								custom::Child( "Supply Drops:Supply Drop Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_EARTH_2_FILL );
								custom::Checkbox( "Supply Drops", &config::supply_drops::enabled );
								custom::Checkbox( "Show Distance", &config::supply_drops::show_distance );
								custom::Checkbox( "Players Searching", &config::supply_drops::players_searching );
								custom::Checkbox( "Loot Tier", &config::supply_drops::supply_loot );
								custom::SliderFloat( "Max Distance", &config::supply_drops::max_distance, 50.0f, 600.0f );
								custom::EndChild( );
							} ImGui::EndGroup( );

							ImGui::SameLine( );
							ImGui::BeginGroup( ); {
								if ( custom::Child( "Llamas:Llama Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, true, ICON_EARTH_2_FILL, true ) ) {
									custom::Checkbox( "Llama", &config::llamas::enabled );
									custom::Checkbox( "Distance", &config::llamas::show_distance );
									custom::SliderFloat( "Max Distance", &config::llamas::max_distance, 50.0f, 500.f );
									custom::EndChild( );
								}
							} ImGui::EndGroup( );
						} break;
						case 3: {
							ImGui::BeginGroup( ); {
								custom::Child( "Pickups:Pickup Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_EARTH_2_FILL );
								custom::Checkbox( "Pickups", &config::pickups::enabled );
								custom::Checkbox( "Distance", &config::pickups::show_distance );
								custom::Checkbox( "Magazine", &config::pickups::magazine_size );
								custom::Checkbox( "Damage", &config::pickups::damage );
								custom::Checkbox( "Icon", &config::pickups::weapon_icon );
								const char* loot_tier[ ]{ "Common", "Uncommon", "Rare", "Epic", "Legendary" };
								custom::MultiCombo( "Loot Tier", config::pickups::loot_tier, loot_tier, IM_ARRAYSIZE( loot_tier ) );
								custom::SliderFloat( "Max Distance", &config::pickups::max_distance, 50.0f, 300.0f );
								custom::EndChild( );
							} ImGui::EndGroup( );

							ImGui::SameLine( );
							ImGui::BeginGroup( ); {
								if ( custom::Child( "Containers:Container Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, true, ICON_EARTH_2_FILL, true ) ) {
									custom::Checkbox( "Containers", &config::containers::enabled );
									custom::Checkbox( "Distance", &config::containers::show_distance );
									custom::Checkbox( "Loot Tier", &config::containers::loot_tier );
									custom::Checkbox( "Container Chams", &config::containers::container_chams );
									custom::SliderFloat( "Max Distance", &config::containers::max_distance, 1.f, 500.f );
									custom::EndChild( );
								}
							} ImGui::EndGroup( );
						} break;
						}
					}

					if ( m_tabs->IsTabActive( 6 ) ) {
						static int exploit_tab = 0;
						custom::SubTab( "General", &exploit_tab, 0, c::main_color ); ImGui::SameLine( );
						custom::SubTab( "Crosshair", &exploit_tab, 1, c::main_color );

						ImGui::SetCursorPosY( 50 + page_offset );

						switch ( exploit_tab ) {
						case 0: {
							ImGui::BeginGroup( ); {
								custom::Child( "Miscellaneous:Miscellaneous Options", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_SETTINGS_4_FILL );
								custom::Checkbox( "VSync", &config::misc::vsync );
								custom::Checkbox( "Stream Proof", &config::misc::stream_proof );
								custom::Checkbox( "Performance Mode", &config::misc::performance_mode );
								custom::Checkbox( "Performance Counter", &config::misc::performance_counter );
								custom::Checkbox( "Controller Support", &config::misc::controller_support );
								if ( config::misc::controller_support ) {
									if ( custom::Button( "Refresh Controllers", ImVec2( ImGui::GetContentRegionAvail( ).x, 30 ) ) ) {
										g_controller->refresh_controllers( );
										auto count = g_controller->get_connected_controller_count( );
										if ( count > 0 ) {
											g_notification->add_message( "\uf11b", std::format( "Found {} controller(s)", count ).c_str( ), c::main_color );
										} else {
											g_notification->add_message( "\uf11b", "No controllers found", c::main_color );
										}
									}
								}

								custom::EndChild( );
							} ImGui::EndGroup( );
							ImGui::SameLine( );

							ImGui::BeginGroup( ); {
								custom::Child( "Debug Boxes:Debug Box Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, true, ICON_SETTINGS_4_FILL );
								custom::Checkbox( "Safe Zone", &config::self::zone_info );
								custom::Checkbox( "Quick Bar", &config::self::inventory );
								custom::Checkbox( "Spectators", &config::self::spectators );

								custom::EndChild( );
							} ImGui::EndGroup( );
						} break;
						case 1: {
							ImGui::BeginGroup( ); {
								custom::Child( "Crosshair:Crosshair Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_CROSS_2_FILL );
								custom::Checkbox( "Crosshair", &config::misc::crosshair::enabled );
								custom::Checkbox( "Outline", &config::misc::crosshair::outline );
								custom::SliderFloat( "Radius", &config::misc::crosshair::radius, 1.f, 20.f );
								custom::SliderInt( "Segments", &config::misc::crosshair::num_segments, 1.f, 50.f );
								custom::SliderFloat( "Thickness", &config::misc::crosshair::thickness, 1.f, 3.f );

								custom::EndChild( );
							} ImGui::EndGroup( );
						} break;
						}
					}

					if ( m_tabs->IsTabActive( 7 ) ) {
						static int exploit_tab = 0;
						custom::SubTab( "Weapon", &exploit_tab, 0, c::main_color ); ImGui::SameLine( );
						custom::SubTab( "Vehicle", &exploit_tab, 1, c::main_color ); ImGui::SameLine( );
						custom::SubTab( "Player", &exploit_tab, 2, c::main_color ); ImGui::SameLine( );
						custom::SubTab( "World", &exploit_tab, 3, c::main_color );

						ImGui::SetCursorPosY( 50 + page_offset );

						switch ( exploit_tab ) {
						case 0: {
							ImGui::BeginGroup( ); {
								custom::Child( "Spread Changer:Spread Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_BOMB_FILL );
								custom::Checkbox( "Spread Changer", &config::exploits::no_spread );
								custom::SliderInt( "Spread X", &config::exploits::spread_x, 1, 100 );
								custom::SliderInt( "Spread Y", &config::exploits::spread_y, 1, 100 );

								custom::EndChild( );
							} ImGui::EndGroup( );
							ImGui::SameLine( );

							ImGui::BeginGroup( ); {
								custom::Child( "Recoil Changer:Recoil Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, true, ICON_BOMB_FILL );
								custom::Checkbox( "Recoil Changer", &config::exploits::no_recoil );
								custom::SliderInt( "Recoil X", &config::exploits::recoil_x, 1, 100 );
								custom::SliderInt( "Recoil Y", &config::exploits::recoil_y, 1, 100 );
								custom::SliderInt( "Recoil Z", &config::exploits::recoil_z, 1, 100 );

								custom::EndChild( );
							} ImGui::EndGroup( );

						} break;
						case 1: {
							ImGui::BeginGroup( ); {
								custom::Child( "Infinite Fuel:Vehicle Customization", ImVec2( 350, 90 ), true, 0, false, ICON_CAR_FILL );
								custom::Checkbox( "Infinite Fuel", &config::exploits::infinite_fuel );
								custom::EndChild( );

								custom::Child( "Bike Jump:Vehicle Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_CAR_FILL );
								custom::Checkbox( "Bike Jump", &config::exploits::bike_jump );
								custom::SliderFloat( "Max Jump Force", &config::exploits::max_jump_force, 1, 200 );
								custom::SliderFloat( "Max Jump Forward", &config::exploits::max_jump_forward, 1, 250 );
								custom::EndChild( );
							} ImGui::EndGroup( );
							ImGui::SameLine( );

							ImGui::BeginGroup( ); {
								custom::Child( "Vehicle Fly:Vehicle Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, true, ICON_CAR_FILL );
								custom::Checkbox( "Vehicle Fly", &config::exploits::vehicle_fly );
								static int mode = 0;
								custom::Keybind( "Key Up", &config::exploits::key_up, &mode );

								static int mode1 = 0;
								custom::Keybind( "Key Down", &config::exploits::key_down, &mode1 );

								custom::EndChild( );
							} ImGui::EndGroup( );
						} break;
						case 2: {
							ImGui::BeginGroup( ); {
								custom::Child( "FOV Changer:Field-Of-View Customization", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_BOMB_FILL );
								custom::Checkbox( "FOV Changer", &config::exploits::fov_changer );
								custom::SliderFloat( "FOV Minimum", &config::exploits::min_fov, 1, 120 );
								custom::EndChild( );
							} ImGui::EndGroup( );
							ImGui::SameLine( );

							ImGui::BeginGroup( ); {
							} ImGui::EndGroup( );
						} break;
						case 3: {
							ImGui::BeginGroup( ); {
								custom::Child( "World:World Customization", ImVec2( 705, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.7 ), true, 0, false, ICON_EYE_2_FILL );
								custom::Checkbox( "Dark Sky", &config::exploits::dark_sky );
								//custom::Checkbox( "Firerate modifier", &config::exploits::firerate_modifier );

								custom::EndChild( );
							} ImGui::EndGroup( );
						} break;
						}
					}

					if ( m_tabs->IsTabActive( 8 ) ) {
						static char config_name[128] = "";
						static int selected_config = -1;
						static std::vector<std::string> config_list;
						static bool refresh_configs = true;
						static config::c_config config_manager;
						
						if (refresh_configs) {
							config_manager.get_available_configs();
							config_list = config_manager.get_configs();
							refresh_configs = false;
						}

						ImGui::BeginGroup( ); {
							custom::Child( "Config List:Available Configurations", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_FOLDER_FILL );
							
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
							if (ImGui::BeginListBox("##ConfigList", ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y - 50))) {
								for (int i = 0; i < config_list.size(); i++) {
									const bool is_selected = (selected_config == i);
									if ( custom::Button(config_list[i].c_str(), ImVec2( ImGui::GetContentRegionAvail( ).x, 35 ) )) {
										selected_config = i;
										strcpy_s(config_name, config_list[i].c_str());
										g_notification->add_message( "\uf085", std::format( "Config {} selected successfully.", config_name ).c_str( ), c::main_color );
									}
								}
								ImGui::EndListBox();
							}
							ImGui::PopStyleVar();
							
							if ( custom::Button("Refresh", ImVec2(ImGui::GetContentRegionAvail().x, 30))) {
								refresh_configs = true;
							}
							
							custom::EndChild( );
						} ImGui::EndGroup( );
						
						ImGui::SameLine( );
						
						ImGui::BeginGroup( ); {
							if ( custom::Child( "Config Actions:Configuration Management", ImVec2( 350, 230 ), true, 0, true, ICON_SETTINGS_4_FILL, true ) ) {
								if ( custom::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f - ImGui::GetStyle().ItemSpacing.x * 0.5f, 35))) {
									std::string new_name = config_manager.generate_new_config_name( );
									if ( config_manager.save_config( new_name ) ) {
										strcpy_s( config_name, new_name.c_str( ) );
										refresh_configs = true;
										g_notification->add_message( "\uf085", "Config saved successfully.", c::main_color );
									}
								}
								
								ImGui::SameLine();
								
								if ( custom::Button("Load", ImVec2(ImGui::GetContentRegionAvail().x, 35))) {
									if (selected_config >= 0 && selected_config < config_list.size()) {
										if (config_manager.load_config(config_list[selected_config])) {
											g_notification->add_message( "\uf085", "Config loaded successfully.", c::main_color );
										}
									}
								}
								
								if ( custom::Button("Delete", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f - ImGui::GetStyle().ItemSpacing.x * 0.5f, 35))) {
									if (selected_config >= 0 && selected_config < config_list.size()) {
										if (config_manager.delete_config(config_list[selected_config])) {
											refresh_configs = true;
											selected_config = -1;
											config_name[0] = '\0';
										}
									}
								}
								
								ImGui::SameLine();
								
								if ( custom::Button( "Open Folder", ImVec2( ImGui::GetContentRegionAvail( ).x, 35 ) ) ) {
									config_manager.open_config_folder( );
								}

								ImGui::Spacing();
								
								if ( custom::Button( "Reset", ImVec2( ImGui::GetContentRegionAvail( ).x, 35 ) ) ) {
									g_notification->add_message( "\uf085", "Config reset successfully.", c::main_color );
									config_manager.reset( );
								}
								
								custom::EndChild( );
							}
							
							if ( custom::Child( "Import/Export:File Operations", ImVec2( 350, ImGui::GetContentRegionAvail( ).y - ImGui::GetStyle( ).FramePadding.y * 2.5 ), true, 0, false, ICON_DOWNLOAD_2_FILL ) ) {
								if ( custom::Button("Import Config", ImVec2(ImGui::GetContentRegionAvail().x, 35))) {
									OPENFILENAMEA ofn;
									char szFile[260] = { 0 };
									ZeroMemory(&ofn, sizeof(ofn));
									ofn.lStructSize = sizeof(ofn);
									ofn.hwndOwner = NULL;
									ofn.lpstrFile = szFile;
									ofn.nMaxFile = sizeof(szFile);
									ofn.lpstrFilter = "JSON Files\0*.json\0All Files\0*.*\0";
									ofn.nFilterIndex = 1;
									ofn.lpstrFileTitle = NULL;
									ofn.nMaxFileTitle = 0;
									ofn.lpstrInitialDir = NULL;
									ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
									
									if (GetOpenFileNameA(&ofn)) {
										if (config_manager.import_config(szFile)) {
											g_notification->add_message( "\uf56f", "Config imported successfully.", c::main_color );
											refresh_configs = true;
										}
									}
								}
								
								if ( custom::Button("Export Config", ImVec2(ImGui::GetContentRegionAvail().x, 35))) {
									if (selected_config >= 0 && selected_config < config_list.size()) {
										OPENFILENAMEA ofn;
										char szFile[260] = { 0 };
										strcpy_s(szFile, (config_list[selected_config] + ".json").c_str());
										ZeroMemory(&ofn, sizeof(ofn));
										ofn.lStructSize = sizeof(ofn);
										ofn.hwndOwner = NULL;
										ofn.lpstrFile = szFile;
										ofn.nMaxFile = sizeof(szFile);
										ofn.lpstrFilter = "JSON Files\0*.json\0All Files\0*.*\0";
										ofn.nFilterIndex = 1;
										ofn.lpstrFileTitle = NULL;
										ofn.nMaxFileTitle = 0;
										ofn.lpstrInitialDir = NULL;
										ofn.lpstrDefExt = "json";
										ofn.Flags = OFN_OVERWRITEPROMPT;
										
										if (GetSaveFileNameA(&ofn)) {
											if (config_manager.export_config(config_list[selected_config], szFile)) {
												g_notification->add_message( "\uf093", "Config exported successfully.", c::main_color );
											}
										}
									}
								}
								
								custom::EndChild( );
							}
						} ImGui::EndGroup( );
					}

					ImGui::EndChild( ); ImGui::PopStyleVar( 1 );
				}

				if ( visual_popup.begin( 350.0f, "Aim Visuals", ICON_PALETTE_LINE ) ) {
					custom::Checkbox( "Target Dot", &config::aimbot::target_dot[ config::aimbot::weapon_configs ] );
					custom::Checkbox( "Target Line", &config::aimbot::target_line[ config::aimbot::weapon_configs ] );

					custom::Checkbox( "FOV Circle", &config::aimbot::fov_circle[ config::aimbot::weapon_configs ] );
					custom::SliderFloat( "FOV Radius", &config::aimbot::fov_radius[ config::aimbot::weapon_configs ], 0, 120 );

					visual_popup.end( );
				}

				if ( acceleration_popup.begin( 350.0f, "Aim Acceleration", ICON_SWORD_FILL ) ) {
					custom::SliderFloat( "Acceleration", &config::aimbot::acceleration[ config::aimbot::weapon_configs ], 20, 150 );
					custom::SliderFloat( "Deceleration", &config::aimbot::deceleration[ config::aimbot::weapon_configs ], 1, 150 );
					custom::SliderFloat( "Max Velocity", &config::aimbot::max_velocity[ config::aimbot::weapon_configs ], 1, 150 );

					acceleration_popup.end( );
				}

				if ( deadzone_popup.begin( 350.0f, "Aim Deadzone", ICON_SWORD_FILL ) ) {
					custom::Checkbox( "Deadzone Dot", &config::aimbot::deadzone_dot[ config::aimbot::weapon_configs ] );
					custom::SliderFloat( "Deadzone Radius", &config::aimbot::restriction[ config::aimbot::weapon_configs ], 0, 7 );

					deadzone_popup.end( );
				}

				if ( chinese_popup.begin( 350.0f, "Chinese Hat", ICON_PALETTE_LINE ) ) {
					custom::SliderFloat( "Base Offset", &config::enemies::hat_base_offset, 1, 15 );
					custom::SliderFloat( "Height", &config::enemies::hat_height, 1, 30 );
					custom::SliderFloat( "Radius", &config::enemies::hat_radius, 1, 25 );

					chinese_popup.end( );
				}
				
				if ( skeleton_popup.begin( 350.0f, "Skeleton", ICON_PALETTE_LINE ) ) {
					custom::Checkbox( "Outline", &config::enemies::skeleton_outline );
					custom::Checkbox( "Head Circle", &config::enemies::head_circle );
					custom::SliderFloat( "Head Size", &config::enemies::head_size, 1.f, 30.0f );
					custom::ToggleButton( "Hexagon", "Circle", &config::enemies::head_hexagon, ImVec2( ImGui::GetContentRegionAvail( ).x, 40 ) );

					custom::SliderFloat( "Thickness", &config::enemies::skeleton_thickness, 0.1f, 3.0f );
					custom::ToggleButton( "Bezier", "Straight", &config::enemies::skeleton_type, ImVec2( ImGui::GetContentRegionAvail( ).x, 40 ) );

					skeleton_popup.end( );
				}
			}
		}
		

	private:

		DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

		bool dark = true;

		char field[ 45 ] = { "" };

		int page = 0;

		float tab_alpha = 0.f; 
		float tab_add;
		int active_tab = 0;

		float knob = 1.f;

		float col[ 4 ] = { 118 / 255.f, 187 / 255.f, 117 / 255.f, 0.5f };

		void ImRotateEnd( float rad, ImVec2 center = ImRotationCenter( ) ) {
			float s = sin( rad ), c = cos( rad );
			center = ImRotate( center, s, c ) - center;

			auto& buf = ImGui::GetWindowDrawList( )->VtxBuffer;
			for ( int i = rotation_start_index; i < buf.Size; i++ )
				buf[ i ].pos = ImRotate( buf[ i ].pos, s, c ) - center;
		}


		struct s_tab {
			const char* header;
			std::vector<const char*> tabs;
		};

		static class c_tabs {

		private:

			int current_idx;
			int check_id;

			std::vector<s_tab> tab_selection;

			void Header( const char* name ) {
				ImGui::PushFont( font::regular_m );
				ImGui::TextColored( ImVec4( 1.f, 1.f, 1.f, 1.f ), name );
				ImGui::PopFont( );
			}



		public:

			c_tabs( std::vector<s_tab> tab_info ) {
				this->current_idx = 0;
				this->check_id = 0;
				this->tab_selection = tab_info;
			}


			int GetCurrentTab( ) {
				return current_idx;
			}


			bool IsTabActive( int id ) {
				return bool( this->c_tabs::current_idx == id );
			}


			void DrawTabs( ) {
				ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 10, 10 ) );
				ImGui::SetCursorPos( ImVec2( 15.f, 25 ) );
				ImGui::BeginChild( "Tabs", ImVec2( 160, ImGui::GetContentRegionAvail( ).y ) );
				{
					auto curr_tab = this->c_tabs::current_idx;

					this->check_id = 0;
					for ( int n = 0; n < tab_selection.size( ); n++ ) {
						this->Header( tab_selection[ n ].header );

						for ( int i = 0; i < tab_selection[ n ].tabs.size( ); i++ ) {
							custom::Tab( tab_selection[ n ].tabs[ i ], &this->c_tabs::current_idx, int( this->check_id ) );

							this->check_id++;
						}
					}

					if ( page_is_changing ) {
						if ( page_offset > 890.f ) {
							page_offset = -900.f;
							page_is_changing = false;
							this->c_tabs::current_idx = wanted_idx;

							if ( curr_tab != wanted_idx && wanted_idx == 7 ) {
								g_notification->add_message( "\uf071", "Warning exploits might be unsafe.", c::main_color );
							}

						}
					}

					page_offset = ImLerp( page_offset, page_is_changing ? 900.f : 0.f, ImGui::GetIO( ).DeltaTime * 12.f );

					//PushFont()
					//    ImGui::Text("Visuals");

					//custom::Tab("Radar", &iTabs, 3);
					//custom::Tab("Exploits", &iTabs, 4);
					//custom::Tab("Settings", &iTabs, 5);


				}
				ImGui::EndChild( );
				ImGui::PopStyleVar( );
			}

		};

		static struct c_animated_bg {

		private:

			const int p_cout = 60;

			ImVec2 GenerateRandomPositionInRect( const ImRect& rect ) {
				std::random_device rd;
				std::mt19937 gen( rd( ) );
				std::uniform_real_distribution<float> x_dist( rect.Min.x, rect.Max.x );
				std::uniform_real_distribution<float> y_dist( rect.Min.y, rect.Max.y );

				float random_x = x_dist( gen );
				float random_y = y_dist( gen );

				return ImVec2( random_x, random_y );
			}

			float GenerateRandomFloat( float min, float max ) {
				std::random_device rd;
				std::mt19937 gen( rd( ) );
				std::uniform_real_distribution<float> dist( min, max );

				return dist( gen );
			}

			struct p_info {
				ImVec2 current_pos;
				ImVec2 target_pos;
				float radius, current_angle, target_angle, speed, alpha, anim_time, timer;
			};

			std::vector<p_info> particle;

			float Distance( const ImVec2& point1, const ImVec2& point2 ) {
				float deltaX = point2.x - point1.x;
				float deltaY = point2.y - point1.y;

				return std::sqrt( deltaX * deltaX + deltaY * deltaY );
			}

			inline ImVec2 vector_angles( const ImVec2& start, const ImVec2& end ) {
				const ImVec2 delta = end - start;

				return ImVec2( std::atan2( delta.y, delta.x ),
					std::atan2( std::sqrt( delta.x * delta.x + delta.y * delta.y ),
						0.0f ) );
			}

			void rotate_to( int idx, ImVec2 vec ) {
				const auto local_pos = this->particle[ idx ].current_pos;

				auto delta_angles = vector_angles( ImGui::GetMousePos( ), local_pos );
				delta_angles.x += 0.1f;

				this->particle[ idx ].current_angle = delta_angles.x;
			}

		public:

			c_animated_bg( ) {
				particle.resize( p_cout );
			}

			void render( ) {
				for ( int i = 0; i < p_cout; i++ ) {
					ImGuiWindow* window = ImGui::GetCurrentWindow( );

					if ( !window->Rect( ).Contains( particle[ i ].target_pos ) ) {
						particle[ i ].target_pos = GenerateRandomPositionInRect( window->Rect( ) );
						particle[ i ].speed = 0.6f + GenerateRandomFloat( 0.5f, 1.0f );
						particle[ i ].current_angle = GenerateRandomFloat( 0.5f, 360.f );
						particle[ i ].target_angle = GenerateRandomFloat( 0.5f, 360.f );
						particle[ i ].radius = GenerateRandomFloat( 1.5f, 3.5f );
						particle[ i ].anim_time = GenerateRandomFloat( 1500, 4000 );
						particle[ i ].timer = 0.f;
						particle[ i ].alpha = -3.f;
					}


					particle[ i ].alpha = ImLerp( particle[ i ].alpha, 1.f, c::anim::speed / 10 );



					particle[ i ].timer += c::anim::speed;

					if ( particle[ i ].timer < particle[ i ].anim_time / 2 )
						particle[ i ].target_angle = 180;

					if ( particle[ i ].timer > particle[ i ].anim_time / 2 )
						particle[ i ].target_angle = 0;


					if ( particle[ i ].timer > particle[ i ].anim_time ) {
						particle[ i ].target_angle = 360;
						particle[ i ].timer = 0;
					}

					particle[ i ].target_pos.x += cosf( particle[ i ].current_angle ) * particle[ i ].speed;
					particle[ i ].target_pos.y += sinf( particle[ i ].current_angle ) * particle[ i ].speed;
					particle[ i ].current_pos = ImLerp( particle[ i ].current_pos, particle[ i ].target_pos, c::anim::speed );


					window->DrawList->AddShadowCircle( particle[ i ].current_pos, particle[ i ].radius, utils::GetColorWithAlpha( c::anim::active, particle[ i ].alpha ), 45.f, ImVec2( 0, 0 ), 0, 36 );
					window->DrawList->AddCircleFilled( particle[ i ].current_pos, particle[ i ].radius + 2.5f, utils::GetColorWithAlpha( c::anim::active, particle[ i ].alpha / 2 ), 36 );
					window->DrawList->AddCircleFilled( particle[ i ].current_pos, particle[ i ].radius, utils::GetColorWithAlpha( c::anim::active, particle[ i ].alpha ), 36 );
				}
			}

		};


		void Triangle_background( ImVec2 p ) {
			ImVec2 screen_size = c::bg::size;

			static ImVec2 particle_pos[ 100 ];
			static ImVec2 particle_speed[ 100 ];
			static float particle_size[ 100 ];
			static float particle_transparency[ 100 ];

			for ( int i = 0; i < 100; ++i ) {
				if ( particle_pos[ i ].x == 0 && particle_pos[ i ].y == 0 ) {
					particle_pos[ i ].x = rand( ) % ( int )screen_size.x;
					particle_pos[ i ].y = rand( ) % 20; // Initial Y position
					particle_speed[ i ] = ImVec2( rand( ) % 205, rand( ) % 205 );
					particle_size[ i ] = rand( ) % 3 + 3; // Random size
					particle_transparency[ i ] = static_cast< float >( rand( ) ) / RAND_MAX; // Random transparency
				}

				particle_pos[ i ] += particle_speed[ i ] * ImVec2( ImGui::GetIO( ).DeltaTime, ImGui::GetIO( ).DeltaTime );

				// ��������� ������������ � ��������� ����
				if ( particle_pos[ i ].x < p.x || particle_pos[ i ].x > p.x + screen_size.x ) {
					// ������ ����������� �� ��� X
					particle_speed[ i ].x = -particle_speed[ i ].x;
				}
				if ( particle_pos[ i ].y < p.y || particle_pos[ i ].y > p.y + screen_size.y + 50 ) {
					// ������ ����������� �� ��� Y
					particle_speed[ i ].y = -particle_speed[ i ].y;
				}

				ImDrawList* draw_list = ImGui::GetWindowDrawList( );
				draw_list->AddCircleFilled( particle_pos[ i ], particle_size[ i ], ImColor( 1.f, 1.f, 1.f, particle_transparency[ i ] ), 16 );
				draw_list->AddShadowCircle( particle_pos[ i ], particle_size[ i ], ImColor( 1.f, 1.f, 1.f, particle_transparency[ i ] ), 30.f, ImVec2( 0, 0 ) );
			}
		}

		static class c_welcome_bg {

		private:

			float welcome_progress;
			float cheatname_progress;
			float logo_progress;
			float background_alpha;

			bool is_finish = false;

			const char* top_text;
			const char* bottom_text;

		public:
			c_welcome_bg( const char* welcome_text, const char* cheat_name ) {
				top_text = welcome_text;
				bottom_text = cheat_name;
				cheatname_progress = 0.f;
				logo_progress = 0.f;
				welcome_progress = 0.f;
				background_alpha = 0.f;
			}

			void update( ) {
				static DWORD dwTickStart = GetTickCount( );
				if ( GetTickCount( ) - dwTickStart > 150 ) {

					if ( !this->is_finish ) {
						this->c_welcome_bg::welcome_progress = ImLerp( this->c_welcome_bg::welcome_progress, this->is_finish ? 0.f : 1.f, c::anim::speed / 3 );

						if ( GetTickCount( ) - dwTickStart > 1000 )
							this->c_welcome_bg::cheatname_progress = ImLerp( this->c_welcome_bg::cheatname_progress, this->is_finish ? 0.f : 1.f, c::anim::speed / 3 );

						if ( GetTickCount( ) - dwTickStart > 1200 )
							this->c_welcome_bg::logo_progress = ImLerp( this->c_welcome_bg::logo_progress, this->is_finish ? 0.f : 1.f, c::anim::speed / 3 );

						this->c_welcome_bg::background_alpha = ImLerp( this->c_welcome_bg::background_alpha, this->is_finish ? 0.f : 1.f, c::anim::speed / 3 );
					}

					else {

						if ( GetTickCount( ) - dwTickStart > 3000 )
							this->c_welcome_bg::welcome_progress = ImLerp( this->c_welcome_bg::welcome_progress, this->is_finish ? 0.f : 1.f, c::anim::speed / 3 );

						if ( GetTickCount( ) - dwTickStart > 3200 )
							this->c_welcome_bg::logo_progress = ImLerp( this->c_welcome_bg::logo_progress, this->is_finish ? 0.f : 1.f, c::anim::speed / 3 );

						if ( GetTickCount( ) - dwTickStart > 2600 )
							this->c_welcome_bg::cheatname_progress = ImLerp( this->c_welcome_bg::cheatname_progress, this->is_finish ? 0.f : 1.f, c::anim::speed / 3 );

						if ( GetTickCount( ) - dwTickStart > 3400 )
							this->c_welcome_bg::background_alpha = ImLerp( this->c_welcome_bg::background_alpha, this->is_finish ? 0.f : 1.f, c::anim::speed / 3 );
					}
					if ( GetTickCount( ) - dwTickStart > 2000 )
						this->is_finish = true;

				}

				ImRect window_bb = ImGui::GetCurrentWindow( )->Rect( );

				ImGui::GetForegroundDrawList( )->AddRectFilled( window_bb.Min, window_bb.Max, utils::GetColorWithAlpha( c::window_bg_color, this->c_welcome_bg::background_alpha ), ImGui::GetStyle( ).FrameRounding );

				ImGui::PushFont( font::regular_m );
				ImGui::GetForegroundDrawList( )->AddText( ImVec2( utils::center_text( window_bb.Min, window_bb.Max, bottom_text ).x, window_bb.GetCenter( ).y -
					( -20 * this->c_welcome_bg::cheatname_progress ) ), utils::GetColorWithAlpha( c::anim::active,
						this->c_welcome_bg::cheatname_progress ), bottom_text );


				ImGui::PopFont( );

				ImGui::PushFont( font::regular_l );
				ImGui::GetForegroundDrawList( )->AddText( ImVec2( utils::center_text( window_bb.Min, window_bb.Max, top_text ).x, window_bb.GetCenter( ).y - ( 50 * this->c_welcome_bg::welcome_progress ) ), utils::GetColorWithAlpha( c::label::active, this->c_welcome_bg::welcome_progress ), top_text );
				ImGui::PopFont( );

				static int current_frame = 0;

				static float frame_offset = 0.f;
				static float static_frame_offset = 0.01111111111f;

				frame_offset = current_frame * static_frame_offset;

				static DWORD dwTickStart2 = GetTickCount( );
				if ( GetTickCount( ) - dwTickStart2 > 15 ) {
					if ( current_frame + 1 <= 90 )
						current_frame++;
					else
						current_frame = 0;

					dwTickStart2 = GetTickCount( );
				}

				ImRect image_bb( window_bb.GetCenter( ) - ImVec2( 30, 30 ), window_bb.GetCenter( ) + ImVec2( 30, 30 ) );

				ImGui::GetForegroundDrawList( )->AddImageRounded( texture::anim_logo, image_bb.Min - ImVec2( 0, 105.f * this->c_welcome_bg::logo_progress ), image_bb.Max - ImVec2( 0, 105.f * this->c_welcome_bg::logo_progress ), ImVec2( 0 + frame_offset, 0 ), ImVec2( frame_offset + static_frame_offset, 1 ), ImColor( 1.f, 1.f, 1.f, this->c_welcome_bg::logo_progress ), c::bg::rounding );


			}

			bool is_finished( ) {
				return is_finish;
			};
		};

		std::vector<s_tab> m_tabs_info;
		std::shared_ptr<c_tabs> m_tabs;
		std::shared_ptr<c_animated_bg> m_animated_bg;
		std::string m_version;
		std::string m_detection;
		std::string m_status;

	};
}