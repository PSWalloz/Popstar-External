#pragma once

namespace zone {
    class c_zone {
    public:
        void generate_sphere_points( engine::f_vector storm_location, float radius, std::vector<engine::f_vector>& points ) {
            int segments = 40;
            int height_segments = 20;

            for ( int i = 0; i <= segments; ++i ) {
                float angle = 2.0f * std::numbers::pi * i / segments;
                for ( int j = 0; j <= height_segments; ++j ) {
                    float phi = std::numbers::pi * j / height_segments;
                    float x = storm_location.x + radius * sinf( phi ) * cosf( angle );
                    float y = storm_location.y + radius * sinf( phi ) * sinf( angle );
                    float z = storm_location.z + radius * cosf( phi );
                    points.push_back( engine::f_vector( x, y, z ) );
                }
            }
        }

        void generate_dome_points( engine::f_vector storm_location, float radius, float height, std::vector<engine::f_vector>& points ) {
            int segments = 40;

            for ( int i = 0; i <= segments; ++i ) {
                float angle = 2.0f * std::numbers::pi * i / segments;
                for ( int j = 0; j <= 20; ++j ) {
                    float z = height * ( 1.0f - powf( j / 20.0f, 2 ) );
                    float scale = 1.0f - ( z / height );
                    float x = storm_location.x + radius * scale * sinf( angle ) * cosf( acosf( 1.0f - z / height ) );
                    float y = storm_location.y + radius * scale * cosf( angle ) * cosf( acosf( 1.0f - z / height ) );
                    points.push_back( engine::f_vector( x, y, storm_location.z + z ) );
                }
            }
        }

        void render_wireframe_shape( const std::vector<engine::f_vector>& points ) {
            if ( points.empty( ) )
                return;

            int segments = 40;
            int height_segments = 20;

            for ( int slice = 0; slice <= segments; ++slice ) {
                for ( int height = 0; height < height_segments; ++height ) {
                    int current_index = slice * ( height_segments + 1 ) + height;
                    int next_index = slice * ( height_segments + 1 ) + height + 1;

                    if ( current_index < points.size( ) && next_index < points.size( ) ) {
                        auto start_location = points[ current_index ];
                        auto end_location = points[ next_index ];

                        auto start_screen = engine::project( start_location );
                        auto end_screen = engine::project( end_location );

                        if ( start_screen.is_valid( ) && end_screen.is_valid( ) &&
                            start_screen.in_screen( ) && end_screen.in_screen( ) ) {
                            g_background->AddLine( start_screen.vec( ), end_screen.vec( ), ImColor( 255, 100, 100, 150 ), 1.f );
                        }
                    }
                }
            }

            for ( int height = 0; height <= height_segments; ++height ) {
                for ( int slice = 0; slice < segments; ++slice ) {
                    int current_index = slice * ( height_segments + 1 ) + height;
                    int next_index = ( slice + 1 ) * ( height_segments + 1 ) + height;

                    if ( current_index < points.size( ) && next_index < points.size( ) ) {
                        auto start_location = points[ current_index ];
                        auto end_location = points[ next_index ];

                        auto start_screen = engine::project( start_location );
                        auto end_screen = engine::project( end_location );

                        if ( start_screen.is_valid( ) && end_screen.is_valid( ) &&
                            start_screen.in_screen( ) && end_screen.in_screen( ) ) {
                            g_background->AddLine( start_screen.vec( ), end_screen.vec( ), ImColor( 255, 100, 100, 150 ), 1.f );
                        }
                    }
                }
            }
        }

        void draw_zone( engine::f_vector location, float radius, float height ) {
            std::vector<engine::f_vector> points;
            switch ( config::self::zone_shape ) {
            case 1: {
                generate_dome_points( location, radius, height, points );
            } break;
            case 2: {
                generate_sphere_points( location, radius, points );
            } break;
            }

            render_wireframe_shape( points );
        }

        void draw_zone_center( engine::f_vector location, std::string zone_text ) {
            auto screen_location = engine::project( location );
            if ( !screen_location.is_valid( ) || !screen_location.in_screen( ) ) return;

            float size = 8.0f;
            auto center = screen_location.vec( );

            ImVec2 points[ 4 ] = {
                ImVec2( center.x, center.y - size ),
                ImVec2( center.x + size, center.y ),
                ImVec2( center.x, center.y + size ),
                ImVec2( center.x - size, center.y )
            };

            g_background->AddConvexPolyFilled( points, 4, ImColor( 100, 255, 100, 100 ) );

            for ( int i = 0; i < 4; i++ ) {
                g_background->AddLine(
                    points[ i ],
                    points[ ( i + 1 ) % 4 ],
                    ImColor( 100, 255, 100, 200 ), 2.0f
                );
            }

            auto text_size = ImGui::CalcTextSize( zone_text.c_str( ) );
            auto text_pos = ImVec2( center.x - text_size.x / 2, center.y + size + 5 );
            
            // Draw text outline
            auto outline_color = ImColor( 0, 0, 0, 255 );
            for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                    if ( dx == 0.0f && dy == 0.0f ) continue;
                    g_background->AddText(
                        ImVec2( text_pos.x + dx, text_pos.y + dy ),
                        outline_color,
                        zone_text.c_str( )
                    );
                }
            }
            
            // Draw main text
            g_background->AddText(
                text_pos,
                ImColor( 100, 255, 100, 255 ),
                zone_text.c_str( )
            );

            if ( config::self::center_line ) {
                auto screen_center = ImVec2(
                    g_overlay->m_width_center,
                    g_overlay->m_height_center
                );
                
                g_background->AddLine(
                    center,
                    screen_center,
                    ImColor( 100, 255, 100, 120 ),
                    1.5f
                );
            }
        }

        void draw_next_zone( engine::f_vector location, float radius, float height ) {
            std::vector<engine::f_vector> points;
            switch ( config::self::zone_shape ) {
            case 1: {
                generate_dome_points( location, radius, height, points );
            } break;
            case 2: {
                generate_sphere_points( location, radius, points );
            } break;
            }

            if ( !points.empty( ) ) {
                for ( int i = 0; i < points.size( ); i += 21 ) {
                    for ( int j = 0; j < 20; ++j ) {
                        auto start_location = points[ i + j ];
                        auto end_location = points[ i + j + 1 ];

                        auto start_screen = engine::project( start_location );
                        auto end_screen = engine::project( end_location );

                        if ( start_screen.is_valid( ) && end_screen.is_valid( ) &&
                            start_screen.in_screen( ) && end_screen.in_screen( ) ) {
                            g_background->AddLine( start_screen.vec( ), end_screen.vec( ), ImColor( 100, 255, 100, 120 ), 1.f );
                        }
                    }
                }
            }
        }
    };
}