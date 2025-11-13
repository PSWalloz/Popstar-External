#pragma once 

namespace radar {
    class c_radar {
    private:
        bool m_is_dragging = false;
        ImVec2 m_drag_offset = ImVec2( 0, 0 );
        
    public:
        void draw_background( ) {
            auto radar_pos = ImVec2( config::radar::position_x, config::radar::position_y );
            if ( config::radar::position == 1 ) {
                auto debug_pos = g_message->get_last_debug_box_position( );
                radar_pos = ImVec2( debug_pos.x - ( config::radar::size * 0.5f ), debug_pos.y + 50 );
            }

            auto radar_size = config::radar::size;
            if ( config::radar::position == 0 ) {
                handle_radar_dragging( radar_pos, radar_size );
            }

            float main_radar_size = radar_size - 45.0f;
            g_foreground->AddShadowRect(
                ImVec2( radar_pos.x, radar_pos.y - 45 ),
                ImVec2( radar_pos.x + radar_size, radar_pos.y + main_radar_size ),
                ImColor( 135, 122, 233, 255 ),
                20.0f,
                ImVec2( 0.0f, 0.0f ),
                3.0f
            );

            g_foreground->AddRectFilled(
                ImVec2( radar_pos.x, radar_pos.y ),
                ImVec2( radar_pos.x + radar_size, radar_pos.y + main_radar_size ),
                ImColor( 14, 13, 29, 255 ),
                3.0f
            );

            auto radar_inner_color = ImColor( 10, 14, 24, ( int )( config::radar::opacity * 0.6f ) );
            g_foreground->AddRectFilled(
                ImVec2( radar_pos.x + 5, radar_pos.y + 5 ),
                ImVec2( radar_pos.x + radar_size - 5, radar_pos.y + main_radar_size - 5 ),
                radar_inner_color,
                2.0f
            );

            auto radar_center = ImVec2(
                radar_pos.x + radar_size / 2.0f,
                radar_pos.y + main_radar_size / 2.0f
            );

            if ( config::radar::grid ) {
                draw_grid_overlay( ImVec2( radar_pos.x + 5, radar_pos.y + 5 ), ImVec2( radar_size - 10, main_radar_size - 10 ) );
            }
            else {
                auto line_color = ImColor( 70, 70, 70, 180 );
                g_foreground->AddLine(
                    ImVec2( radar_pos.x + 5, radar_center.y ),
                    ImVec2( radar_pos.x + radar_size - 5, radar_center.y ),
                    line_color,
                    1.0f
                );

                g_foreground->AddLine(
                    ImVec2( radar_center.x, radar_pos.y + 5 ),
                    ImVec2( radar_center.x, radar_pos.y + main_radar_size - 5 ),
                    line_color,
                    1.0f
                );
            }

            float fov = g_camera->m_fov;
            float half_fov_radians = ( fov / 2.0f ) * std::numbers::pi / 180.0f;

            float distance_to_top_edge = radar_center.y - ( radar_pos.y + 5 );

            float x_offset = distance_to_top_edge * tan( half_fov_radians );
            auto left_line_end = ImVec2( radar_center.x - x_offset, radar_pos.y + 5 );
            auto right_line_end = ImVec2( radar_center.x + x_offset, radar_pos.y + 5 );

            left_line_end.x = std::clamp( left_line_end.x, radar_pos.x + 5, radar_pos.x + radar_size - 5 );
            right_line_end.x = std::clamp( right_line_end.x, radar_pos.x + 5, radar_pos.x + radar_size - 5 );

            ImColor fov_fill_color( 135, 122, 233, ( int )( config::radar::opacity * 0.3f ) );
            g_foreground->AddTriangleFilled(
                radar_center,
                left_line_end,
                right_line_end,
                fov_fill_color
            );

            ImColor fov_line_color( 135, 122, 233, 200 );
            g_foreground->AddLine( radar_center, left_line_end, fov_line_color, 1.5f );
            g_foreground->AddLine( radar_center, right_line_end, fov_line_color, 1.5f );

            g_foreground->AddCircleFilled(
                radar_center,
                4.0f,
                ImColor( 0, 0, 0, 255 ),
                15
            );
            g_foreground->AddCircleFilled(
                radar_center,
                3.0f,
                ImColor( 135, 122, 233, 255 ),
                15
            );

            float header_height = 45.0f;
            
            g_foreground->AddRectFilled(
                ImVec2( radar_pos.x, radar_pos.y - 45 ),
                ImVec2( radar_pos.x + radar_size, radar_pos.y ),
                ImColor( 14, 13, 29, 255 ),
                3.0f,
                ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight
            );

            g_foreground->AddRect(
                ImVec2( radar_pos.x, radar_pos.y - 45 ),
                ImVec2( radar_pos.x + radar_size, radar_pos.y + main_radar_size ),
                ImColor( 70, 70, 70, 255 ),
                3.0f,
                0,
                1.5f
            );

            float icon_size = 33.5f;
            ImVec2 icon_area_pos = ImVec2(
                radar_pos.x + 13.0f,
                radar_pos.y - 39
            );

            ImVec2 circle_center = ImVec2(
                icon_area_pos.x + icon_size * 0.5f,
                icon_area_pos.y + icon_size * 0.55f
            );
            float circle_radius = icon_size * 0.5f;

            g_foreground->AddCircleFilled(
                circle_center,
                circle_radius,
                ImColor( 135, 122, 233, 255 ),
                32
            );

            ImGui::PushFont( font::semibold_font );
            ImVec2 icon_pos = utils::center_text( icon_area_pos, ImVec2( icon_area_pos.x + icon_size, icon_area_pos.y + icon_size ), ICON_COMPASS_FILL ) + ImVec2( 0, 4.5f );
            g_foreground->AddText(
                icon_pos,
                ImColor( 14, 13, 29, 255 ),
                ICON_COMPASS_FILL
            );
            ImGui::PopFont( );

            g_foreground->AddCircle(
                circle_center,
                circle_radius,
                ImColor( 0, 0, 0, 255 ),
                32,
                1.5f
            );

            ImGui::PushFont( font::inter_medium_semibold );
            ImVec2 title_pos = ImVec2(
                icon_area_pos.x + icon_size + 16.0f,
                radar_pos.y - 45 + 14.0f
            );

            auto outline_color = ImColor( 0, 0, 0, 255 );
            for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                    if ( dx == 0.0f && dy == 0.0f ) continue;
                    g_foreground->AddText(
                        ImVec2( title_pos.x + dx, title_pos.y + dy ),
                        outline_color,
                        "Radar"
                    );
                }
            }

            g_foreground->AddText(
                title_pos,
                ImColor( 135, 122, 233, 255 ),
                "Radar"
            );
            ImGui::PopFont( );
        }

        void render( lists::c_player player ) {
            auto radar_location = player.m_base_location;
            if ( !radar_location.is_valid( ) ) return;

            engine::f_vector2d radar_screen{};
            if ( config::radar::rotation_mode ) {
                rotate_point_with_player_rotation( &radar_screen, radar_location );
            }
            else {
                rotate_point( &radar_screen, radar_location );
            }

            g_foreground->AddShadowCircle(
                radar_screen.vec( ),
                2.5f,
                c::second_color,
                60.0f,
                ImVec2( 0, 0 ),
                0,
                36
            );

            g_foreground->AddCircle(
                radar_screen.vec( ),
                5.5,
                c::main_color,
                15,
                2.5f
            );

            if ( config::radar::line_of_sight ) {
                draw_line_of_sight( radar_screen, player );
            }

            ImGui::PushFont( font::esp_font );

            if ( config::radar::distance ) {
                auto text_format = std::format( "({:.1f}m)", player.m_distance );
                auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

                auto text_pos = ImVec2(
                    radar_screen.x - ( text_size.x / 2.0f ),
                    radar_screen.y + 6
                );

                auto outline_color = ImColor( 0, 0, 0, 255 );
                for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                    for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                        if ( dx == 0.0f && dy == 0.0f ) continue;
                        g_foreground->AddText(
                            ImVec2( text_pos.x + dx, text_pos.y + dy ),
                            outline_color,
                            text_format.c_str( )
                        );
                    }
                }

                g_foreground->AddText( text_pos, ImColor( 142, 130, 209, 255 ), text_format.c_str( ) );
            }

            ImGui::PopFont( );
        }

    private:
        void rotate_point( engine::f_vector2d* screen, engine::f_vector origin ) {
            auto delta = origin - g_camera->m_location;
            auto angle = atan2( delta.y, delta.x );
            auto distance = sqrt( delta.x * delta.x + delta.y * delta.y ) * 0.01f;

            float main_radar_size = config::radar::size - 45.0f;
            auto scaled_distance = ( distance / config::radar::range ) * ( main_radar_size / 2.0f );
            scaled_distance = min( scaled_distance, main_radar_size / 2.0f );

            auto x = scaled_distance * cos( angle );
            auto y = scaled_distance * sin( angle );

            float radar_pos_x = config::radar::position_x;
            float radar_pos_y = config::radar::position_y;
            
            if ( config::radar::position == 1 ) {
                auto last_debug_pos = g_message->get_last_debug_box_position( );
                radar_pos_x = last_debug_pos.x - (config::radar::size * 0.5f);
                radar_pos_y = last_debug_pos.y + 50;
            }

            auto radar_center = engine::f_vector2d(
                radar_pos_x + ( config::radar::size / 2.0f ),
                radar_pos_y + ( main_radar_size / 2.0f )
            );

            *screen = engine::f_vector2d( radar_center.x + x, radar_center.y + y );
        }

        void draw_grid_overlay( ImVec2 radar_pos, ImVec2 radar_dimensions ) {
            int grid_divisions = config::radar::grid_divisions;
            float grid_spacing_x = radar_dimensions.x / grid_divisions;
            float grid_spacing_y = radar_dimensions.y / grid_divisions;

            ImColor grid_color( 255, 255, 255, 30 );

            // Vertical lines
            for ( int i = 1; i < grid_divisions; i++ ) {
                float x = radar_pos.x + ( i * grid_spacing_x );
                g_foreground->AddLine(
                    ImVec2( x, radar_pos.y ),
                    ImVec2( x, radar_pos.y + radar_dimensions.y ),
                    grid_color,
                    0.5f
                );
            }

            // Horizontal lines
            for ( int i = 1; i < grid_divisions; i++ ) {
                float y = radar_pos.y + ( i * grid_spacing_y );
                g_foreground->AddLine(
                    ImVec2( radar_pos.x, y ),
                    ImVec2( radar_pos.x + radar_dimensions.x, y ),
                    grid_color,
                    0.5f
                );
            }
        }

        void draw_line_of_sight( engine::f_vector2d player_pos, lists::c_player player ) {
            float player_yaw = player.m_player_rotation.yaw;
            float our_yaw = g_local_pawn->m_rotation.yaw;

            float relative_angle;
            if ( config::radar::rotation_mode ) {
                relative_angle = ( player_yaw - our_yaw ) * std::numbers::pi / 180.0f;
            }
            else {
                relative_angle = player_yaw * std::numbers::pi / 180.0f;
            }

            float sight_length = 25.0f;
            float sight_width = 0.3f;

            float center_x = player_pos.x + sight_length * sin( relative_angle );
            float center_y = player_pos.y - sight_length * cos( relative_angle );

            float left_angle = relative_angle - sight_width;
            float right_angle = relative_angle + sight_width;

            float left_x = player_pos.x + sight_length * sin( left_angle );
            float left_y = player_pos.y - sight_length * cos( left_angle );

            float right_x = player_pos.x + sight_length * sin( right_angle );
            float right_y = player_pos.y - sight_length * cos( right_angle );

            ImVec2 player_vec = player_pos.vec( );
            ImVec2 left_end( left_x, left_y );
            ImVec2 right_end( right_x, right_y );

            ImVec2 radar_pos( config::radar::position_x, config::radar::position_y );
            
            if ( config::radar::position == 1 ) {
                auto last_debug_pos = g_message->get_last_debug_box_position( );
                radar_pos.x = last_debug_pos.x - (config::radar::size * 0.5f);
                radar_pos.y = last_debug_pos.y + 50;
            }
            
            float radar_size = config::radar::size;
            float main_radar_size = radar_size - 45.0f;

            left_end = clamp_to_radar( left_end, radar_pos, ImVec2( radar_size, main_radar_size ) );
            right_end = clamp_to_radar( right_end, radar_pos, ImVec2( radar_size, main_radar_size ) );

            ImColor sight_fill_color( 255, 100, 100, 25 );
            ImColor sight_line_color( 255, 150, 150, 100 );

            if ( config::radar::los_fill ) {
                g_foreground->AddTriangleFilled( player_vec, left_end, right_end, sight_fill_color );
            }

            g_foreground->AddLine( player_vec, left_end, sight_line_color, 1.0f );
            g_foreground->AddLine( player_vec, right_end, sight_line_color, 1.0f );
        }

        ImVec2 clamp_to_radar( ImVec2 point, ImVec2 radar_pos, ImVec2 radar_dimensions ) {
            return ImVec2(
                std::clamp( point.x, radar_pos.x, radar_pos.x + radar_dimensions.x ),
                std::clamp( point.y, radar_pos.y, radar_pos.y + radar_dimensions.y )
            );
        }

        void rotate_point_with_player_rotation( engine::f_vector2d* screen, engine::f_vector origin ) {
            auto player_radians = g_local_pawn->m_rotation.yaw * std::numbers::pi / 180.0f;

            auto delta = origin - g_camera->m_location;
            auto angle = atan2( delta.y, delta.x ) - player_radians;
            auto distance = sqrt( delta.x * delta.x + delta.y * delta.y ) * 0.01f;

            float main_radar_size = config::radar::size - 45.0f;
            auto scaled_distance = ( distance / config::radar::range ) * ( main_radar_size / 2.0f );
            scaled_distance = min( scaled_distance, main_radar_size / 2.0f );

            auto x = scaled_distance * sin( angle );
            auto y = -scaled_distance * cos( angle );

            float radar_pos_x = config::radar::position_x;
            float radar_pos_y = config::radar::position_y;
            
            if ( config::radar::position == 1 ) {
                auto last_debug_pos = g_message->get_last_debug_box_position( );
                radar_pos_x = last_debug_pos.x - (config::radar::size * 0.5f);
                radar_pos_y = last_debug_pos.y + 50;
            }

            auto radar_center = engine::f_vector2d(
                radar_pos_x + ( config::radar::size / 2.0f ),
                radar_pos_y + ( main_radar_size / 2.0f )
            );

            *screen = engine::f_vector2d( radar_center.x + x, radar_center.y + y );
        }

        void handle_radar_dragging( ImVec2& radar_pos, float radar_size ) {
            ImVec2 mouse_pos = ImGui::GetMousePos( );
            bool mouse_down = ImGui::IsMouseDown( ImGuiMouseButton_Left );
            
            float header_height = 45.0f;
            float main_radar_size = radar_size - header_height;
            
            auto is_over_radar = ( mouse_pos.x >= radar_pos.x && mouse_pos.x <= radar_pos.x + radar_size &&
                                  mouse_pos.y >= radar_pos.y - header_height && mouse_pos.y <= radar_pos.y + main_radar_size );
            if ( is_over_radar && mouse_down && !m_is_dragging ) {
                m_is_dragging = true;
                m_drag_offset = ImVec2( mouse_pos.x - radar_pos.x, mouse_pos.y - radar_pos.y );
            }
            
            if ( m_is_dragging && mouse_down ) {
                config::radar::position_x = mouse_pos.x - m_drag_offset.x;
                config::radar::position_y = mouse_pos.y - m_drag_offset.y;
                
                auto& io = ImGui::GetIO( );
                config::radar::position_x = std::clamp( ( float )config::radar::position_x, 0.0f, io.DisplaySize.x - radar_size );
                config::radar::position_y = std::clamp( ( float )config::radar::position_y, header_height, io.DisplaySize.y - main_radar_size );
                
                radar_pos.x = config::radar::position_x;
                radar_pos.y = config::radar::position_y;
            }
            
            if ( !mouse_down ) {
                m_is_dragging = false;
            }
            
            if ( m_is_dragging ) {
                g_foreground->AddRect(
                    ImVec2( radar_pos.x, radar_pos.y - header_height ),
                    ImVec2( radar_pos.x + radar_size, radar_pos.y + main_radar_size ),
                    ImColor( 255, 255, 255, 150 ),
                    3.0f,
                    0,
                    2.0f
                );
            }
        }
    };
}