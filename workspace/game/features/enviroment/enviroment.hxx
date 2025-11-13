#pragma once 

namespace enviroment {
    void render_pickup( lists::c_pickup const pickup ) {
        auto text_offset = 1.0f;

        auto rarity = pickup.m_rarity;
        auto tier_color = esp::get_tier_color( rarity );
        if ( !config::pickups::loot_tier[ 0 ] && rarity == engine::e_fort_rarity::common ) return;
        if ( !config::pickups::loot_tier[ 1 ] && rarity == engine::e_fort_rarity::uncommon ) return;
        if ( !config::pickups::loot_tier[ 2 ] && rarity == engine::e_fort_rarity::rare ) return;
        if ( !config::pickups::loot_tier[ 3 ] && rarity == engine::e_fort_rarity::epic ) return;
        if ( !config::pickups::loot_tier[ 4 ] && rarity == engine::e_fort_rarity::legendary ) return;

        auto world_location = pickup.m_world_location;
        auto world_screen = engine::project( world_location );
        if ( world_screen.in_screen( ) ) {
            if ( config::pickups::weapon_icon ) {
                auto weapon_texture = pickup.get_texture( );
                if ( weapon_texture ) {
                    const ImVec2 texture_size( 32.0f, 32.0f );
                    auto texture_pos = ImVec2(
                        world_screen.x - ( texture_size.x / 2.0f ),
                        world_screen.y - texture_size.y - 5.0f
                    );

                    auto border_color = esp::get_tier_color( rarity );
                    g_background->AddRect(
                        texture_pos - ImVec2( 2.0f, 2.0f ),
                        texture_pos + texture_size + ImVec2( 2.0f, 2.0f ),
                        border_color,
                        2.0f,
                        0,
                        2.0f
                    );

                    g_background->AddImage(
                        weapon_texture,
                        texture_pos,
                        texture_pos + texture_size
                    );
                }
            }

            auto& pickup_name = pickup.m_pickup_name;
            auto text_format = config::pickups::show_distance ? std::format( "{} ({:.1f}m)", pickup_name, pickup.m_distance ) :
                std::format( "{}", pickup_name );
            auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

            auto text_pos = ImVec2(
                world_screen.x - ( text_size.x / 2.0f ),
                world_screen.y + text_offset
            );

            auto outline_color = ImColor( 0, 0, 0, 255 );
            for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                    if ( dx == 0.0f && dy == 0.0f ) continue;
                    g_background->AddText(
                        ImVec2( text_pos.x + dx, text_pos.y + dy ),
                        outline_color,
                        text_format.c_str( )
                    );
                }
            }

            g_background->AddText( text_pos, tier_color, text_format.c_str( ) );
            text_offset += text_size.y;

            if ( config::pickups::magazine_size ) {
                auto magazine_size = pickup.get_magazine_size( );
                if ( magazine_size > 0 ) {
                    auto magazine_text = std::format( "Magazine ({})", magazine_size );
                    auto text_size = ImGui::CalcTextSize( magazine_text.c_str( ) );

                    auto text_color = ImColor( 142, 188, 142, 255 );
                    auto text_pos = ImVec2(
                        world_screen.x - ( text_size.x / 2.0f ),
                        world_screen.y + text_offset
                    );

                    for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                        for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                            if ( dx == 0.0f && dy == 0.0f ) continue;
                            g_background->AddText(
                                ImVec2( text_pos.x + dx, text_pos.y + dy ),
                                outline_color,
                                magazine_text.c_str( )
                            );
                        }
                    }

                    g_background->AddText( text_pos, text_color, magazine_text.c_str( ) );
                    text_offset += text_size.y;
                }
            }

            if ( config::pickups::damage ) {
                auto damage = pickup.get_damage( );
                if ( damage > 0 ) {
                    auto damage_text = std::format( "Damage ({})", damage );
                    auto text_size = ImGui::CalcTextSize( damage_text.c_str( ) );

                    auto text_color = ImColor( 255, 116, 108, 255 );
                    auto text_pos = ImVec2(
                        world_screen.x - ( text_size.x / 2.0f ),
                        world_screen.y + text_offset
                    );

                    for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                        for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                            if ( dx == 0.0f && dy == 0.0f ) continue;
                            g_background->AddText(
                                ImVec2( text_pos.x + dx, text_pos.y + dy ),
                                outline_color,
                                damage_text.c_str( )
                            );
                        }
                    }

                    g_background->AddText( text_pos, text_color, damage_text.c_str( ) );
                    text_offset += text_size.y;
                }
            }
        }
    }

    void render_container( lists::c_container container ) {
        auto text_color = ImColor( 106, 137, 167, 255 );
        auto text_format = std::format( "Container" );

        if ( container.m_spawn_source == engine::e_fort_pickup_spawn_source::chest ) {
            text_format = std::format( "Chest" );
            text_color = ImColor( 239, 191, 4, 255 );
        }
        else if ( container.m_spawn_source == engine::e_fort_pickup_spawn_source::ammo_box ) {
            text_format = std::format( "Ammobox" );
            text_color = ImColor( 177, 210, 123, 255 );
        }

        text_format = config::containers::show_distance ?
            std::format( "{} ({:.1f}m)", text_format.c_str( ), container.m_distance ) : text_format;

        auto world_location = container.m_world_location;
        auto world_screen = engine::project( world_location );
        if ( world_screen.in_screen( ) ) {
            auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );
            auto text_pos = ImVec2(
                world_screen.x - ( text_size.x / 2.0f ),
                world_screen.y
            );

            auto outline_color = ImColor( 0, 0, 0, 255 );
            for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                    if ( dx == 0.0f && dy == 0.0f ) continue;
                    g_background->AddText(
                        ImVec2( text_pos.x + dx, text_pos.y + dy ),
                        outline_color,
                        text_format.c_str( )
                    );
                }
            }

            g_background->AddText( text_pos, text_color, text_format.c_str( ) );
        }
    }

    void render_weakspot( lists::c_weakspot weakspot ) {
        auto text_color = ImColor( 204, 102, 102, 255 );
        auto text_format = config::weakspots::show_distance ? std::format( "Weakspot ({:.1f}m)", weakspot.m_distance ) :
            std::format( "Weakspot" );
        auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

        auto world_location = weakspot.m_world_location;
        auto world_screen = engine::project( world_location );
        if ( world_screen.in_screen( ) ) {
            auto text_pos = ImVec2(
                world_screen.x - ( text_size.x / 2.0f ),
                world_screen.y
            );

            auto outline_color = ImColor( 0, 0, 0, 255 );
            for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                    if ( dx == 0.0f && dy == 0.0f ) continue;
                    g_background->AddText(
                        ImVec2( text_pos.x + dx, text_pos.y + dy ),
                        outline_color,
                        text_format.c_str( )
                    );
                }
            }

            g_background->AddText( text_pos, text_color, text_format.c_str( ) );
        }
    }

    void render_projectile( lists::c_projectile projectile ) {
        if ( config::projectiles::show_tracers ) {
            auto& traces = g_projectile_traces[ projectile.m_actor ].get_traces( );
            for ( auto i = 1; i < traces.size( ); i++ ) {
                auto prev_position = traces[ i - 1 ].m_position;
                auto curr_position = traces[ i ].m_position;

                auto prev_screen = engine::project( prev_position );
                auto curr_screen = engine::project( curr_position );

                if ( prev_screen.is_valid( ) && curr_screen.is_valid( ) ) {
                    g_background->AddLine(
                        prev_screen.vec( ),
                        curr_screen.vec( ),
                        ImColor( 247, 108, 108, 255 ),
                        1.5f
                    );

                    if ( config::projectiles::hit_marker ) {
                        const float marker_size = 8.0f;
                        const float marker_thickness = 2.0f;
                        const ImColor marker_color = ImColor( 247, 108, 108, 200 );

                        g_background->AddLine(
                            ImVec2( curr_screen.x - marker_size, curr_screen.y ),
                            ImVec2( curr_screen.x + marker_size, curr_screen.y ),
                            marker_color,
                            marker_thickness
                        );
                        g_background->AddLine(
                            ImVec2( curr_screen.x, curr_screen.y - marker_size ),
                            ImVec2( curr_screen.x, curr_screen.y + marker_size ),
                            marker_color,
                            marker_thickness
                        );

                        g_background->AddLine(
                            ImVec2( curr_screen.x - marker_size, curr_screen.y - marker_size ),
                            ImVec2( curr_screen.x + marker_size, curr_screen.y + marker_size ),
                            marker_color,
                            marker_thickness
                        );
                        g_background->AddLine(
                            ImVec2( curr_screen.x - marker_size, curr_screen.y + marker_size ),
                            ImVec2( curr_screen.x + marker_size, curr_screen.y - marker_size ),
                            marker_color,
                            marker_thickness
                        );
                    }
                }
            }
        }

        auto text_color = ImColor( 247, 108, 108, 255 );
        auto text_format = config::projectiles::show_distance ? std::format( "Projectile ({:.1f}m)", projectile.m_distance ) :
            std::format( "Projectile" );
        auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

        auto world_location = projectile.m_base_location;
        auto world_screen = engine::project( world_location );
        if ( world_screen.in_screen( ) ) {
            auto text_pos = ImVec2(
                world_screen.x - ( text_size.x / 2.0f ),
                world_screen.y
            );

            auto outline_color = ImColor( 0, 0, 0, 255 );
            for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                    if ( dx == 0.0f && dy == 0.0f ) continue;
                    g_background->AddText(
                        ImVec2( text_pos.x + dx, text_pos.y + dy ),
                        outline_color,
                        text_format.c_str( )
                    );
                }
            }

            g_background->AddText( text_pos, text_color, text_format.c_str( ) );
        }
    }

    void render_vehicle( lists::c_vehicle const vehicle ) {
        auto text_offset = 1.0;

        auto text_color = ImColor( 255, 105, 98, 255 );
        auto text_format = config::vehicles::show_distance ? std::format( "Vehicle ({:.1f}m)", vehicle.m_distance ) :
            std::format( "Vehicle" );
        auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

        auto world_location = vehicle.m_world_location;
        auto world_screen = engine::project( world_location );
        if ( world_screen.in_screen( ) ) {
            auto text_pos = ImVec2(
                world_screen.x - ( text_size.x / 2.0f ),
                world_screen.y + text_offset
            );

            auto outline_color = ImColor( 0, 0, 0, 255 );
            for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                    if ( dx == 0.0f && dy == 0.0f ) continue;
                    g_background->AddText(
                        ImVec2( text_pos.x + dx, text_pos.y + dy ),
                        outline_color,
                        text_format.c_str( )
                    );
                }
            }

            g_background->AddText( text_pos, text_color, text_format.c_str( ) );
            text_offset += text_size.y;

            if ( config::vehicles::terrain ) {
                auto movement_text = engine::get_movement_name( vehicle );
                if ( !movement_text.empty( ) ) {
                    auto text_size = ImGui::CalcTextSize( movement_text.c_str( ) );
                    auto text_pos = ImVec2(
                        world_screen.x - ( text_size.x / 2.0f ),
                        world_screen.y + text_offset
                    );

                    for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                        for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                            if ( dx == 0.0f && dy == 0.0f ) continue;
                            g_background->AddText(
                                ImVec2( text_pos.x + dx, text_pos.y + dy ),
                                outline_color,
                                movement_text.c_str( )
                            );
                        }
                    }

                    auto text_color = engine::get_movement_color( vehicle );
                    g_background->AddText( text_pos, text_color, movement_text.c_str( ) );
                    text_offset += text_size.y;
                }
            }

            if ( config::vehicles::health ) {
                auto text_format = std::format( "Health ({})", vehicle.m_critical_health );

                auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );
                auto text_pos = ImVec2(
                    world_screen.x - ( text_size.x / 2.0f ),
                    world_screen.y + text_offset
                );

                for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                    for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                        if ( dx == 0.0f && dy == 0.0f ) continue;
                        g_background->AddText(
                            ImVec2( text_pos.x + dx, text_pos.y + dy ),
                            outline_color,
                            text_format.c_str( )
                        );
                    }
                }

                auto text_color = ImColor( 177, 225, 123, 255 );
                g_background->AddText( text_pos, text_color, text_format.c_str( ) );
                text_offset += text_size.y;
            }
        }
    }

    void render_building( lists::c_building const building ) {
        auto origin = building.m_bounds.orgin;
        auto extent = building.m_bounds.box_extent;

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
        engine::f_vector2d box_middle( min_pos.x + ( size.x / 2.0 ), max_pos.y );
        auto text_offset = 1.0;

        if ( config::buildings::build_type ) {
            auto text_color = ImColor( 162, 207, 254, 255 );
            auto text_format = config::buildings::show_distance ? std::format( "{} ({:.1f}m)", esp::get_building_name( building.m_building_type ), building.m_distance ) :
                std::format( "{}", esp::get_building_name( building.m_building_type ) );

            auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );
            auto text_pos = ImVec2(
                box_middle.x - ( text_size.x / 2.0f ),
                box_middle.y + text_offset
            );

            auto outline_color = ImColor( 0, 0, 0, 255 );
            for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                    if ( dx == 0.0f && dy == 0.0f ) continue;
                    g_background->AddText(
                        ImVec2( text_pos.x + dx, text_pos.y + dy ),
                        outline_color,
                        text_format.c_str( )
                    );
                }
            }

            g_background->AddText( text_pos, text_color, text_format.c_str( ) );
            text_offset += text_size.y;
        }

        if ( config::buildings::collision_box ) {
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
                const ImColor box_color( 119, 158, 203, 255 );
                const ImColor outline_color( 0, 0, 0, 255 );
                const float thickness = 1.5f;

                g_background->AddLine( screen_corners[ 0 ].vec( ), screen_corners[ 1 ].vec( ), box_color, thickness );
                g_background->AddLine( screen_corners[ 1 ].vec( ), screen_corners[ 3 ].vec( ), box_color, thickness );
                g_background->AddLine( screen_corners[ 3 ].vec( ), screen_corners[ 2 ].vec( ), box_color, thickness );
                g_background->AddLine( screen_corners[ 2 ].vec( ), screen_corners[ 0 ].vec( ), box_color, thickness );

                g_background->AddLine( screen_corners[ 4 ].vec( ), screen_corners[ 5 ].vec( ), box_color, thickness );
                g_background->AddLine( screen_corners[ 5 ].vec( ), screen_corners[ 7 ].vec( ), box_color, thickness );
                g_background->AddLine( screen_corners[ 7 ].vec( ), screen_corners[ 6 ].vec( ), box_color, thickness );
                g_background->AddLine( screen_corners[ 6 ].vec( ), screen_corners[ 4 ].vec( ), box_color, thickness );

                g_background->AddLine( screen_corners[ 0 ].vec( ), screen_corners[ 4 ].vec( ), box_color, thickness );
                g_background->AddLine( screen_corners[ 1 ].vec( ), screen_corners[ 5 ].vec( ), box_color, thickness );
                g_background->AddLine( screen_corners[ 2 ].vec( ), screen_corners[ 6 ].vec( ), box_color, thickness );
                g_background->AddLine( screen_corners[ 3 ].vec( ), screen_corners[ 7 ].vec( ), box_color, thickness );
            }
        }
    }

    void render_supply_drop( lists::c_supply_drop const supply_drop ) {
        auto text_offset = 1.0;

        auto text_color = ImColor( 88, 160, 236, 255 );
        auto text_format = config::supply_drops::show_distance ? std::format( "Supply Drop ({:.1f}m)", supply_drop.m_distance ) :
            std::format( "Supply Drop" );
        auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

        auto world_location = supply_drop.m_world_location;
        auto world_screen = engine::project( world_location );
        if ( world_screen.in_screen( ) ) {
            auto text_pos = ImVec2(
                world_screen.x - ( text_size.x / 2.0f ),
                world_screen.y + text_offset
            );

            auto outline_color = ImColor( 0, 0, 0, 255 );
            for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                    if ( dx == 0.0f && dy == 0.0f ) continue;
                    g_background->AddText(
                        ImVec2( text_pos.x + dx, text_pos.y + dy ),
                        outline_color,
                        text_format.c_str( )
                    );
                }
            }

            g_background->AddText( text_pos, text_color, text_format.c_str( ) );
            text_offset += text_size.y;

            if ( config::supply_drops::supply_loot ) {
                auto text_format = supply_drop.m_loot_name;

                auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );
                auto text_pos = ImVec2(
                    world_screen.x - ( text_size.x / 2.0f ),
                    world_screen.y + text_offset
                );

                for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                    for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                        if ( dx == 0.0f && dy == 0.0f ) continue;
                        g_background->AddText(
                            ImVec2( text_pos.x + dx, text_pos.y + dy ),
                            outline_color,
                            text_format.c_str( )
                        );
                    }
                }

                auto text_color = ImColor( 255, 83, 73, 255 );
                g_background->AddText( text_pos, text_color, text_format.c_str( ) );
                text_offset += text_size.y;
            }

            if ( config::supply_drops::players_searching ) {
                auto text_format = std::format( "Players Searching ({})", supply_drop.m_players_interacting );

                auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );
                auto text_pos = ImVec2(
                    world_screen.x - ( text_size.x / 2.0f ),
                    world_screen.y + text_offset
                );

                for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                    for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                        if ( dx == 0.0f && dy == 0.0f ) continue;
                        g_background->AddText(
                            ImVec2( text_pos.x + dx, text_pos.y + dy ),
                            outline_color,
                            text_format.c_str( )
                        );
                    }
                }

                auto text_color = ImColor( 255, 165, 0, 255 );
                g_background->AddText( text_pos, text_color, text_format.c_str( ) );
                text_offset += text_size.y;
            }
        }
    }

    void render_loot_llama( lists::c_loot_llama const loot_llama ) {
        auto text_offset = 1.0;

        auto text_color = ImColor( 91, 190, 221, 255 );
        auto text_format = config::llamas::show_distance ? std::format( "Loot Llama ({:.1f}m)", loot_llama.m_distance ) :
            loot_llama.m_class_name;
        auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

        auto world_location = loot_llama.m_world_location;
        auto world_screen = engine::project( world_location );
        if ( world_screen.in_screen( ) ) {
            auto text_pos = ImVec2(
                world_screen.x - ( text_size.x / 2.0f ),
                world_screen.y + text_offset
            );

            auto outline_color = ImColor( 0, 0, 0, 255 );
            for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
                for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
                    if ( dx == 0.0f && dy == 0.0f ) continue;
                    g_background->AddText(
                        ImVec2( text_pos.x + dx, text_pos.y + dy ),
                        outline_color,
                        text_format.c_str( )
                    );
                }
            }

            g_background->AddText( text_pos, text_color, text_format.c_str( ) );
            text_offset += text_size.y;
        }
    }
}