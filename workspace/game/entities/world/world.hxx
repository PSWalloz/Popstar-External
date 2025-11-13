
#pragma once

namespace world {
    class c_world {
    public:
        void loop_actors( const std::function<void( lists::c_player const )>& render ) {
            auto list_copy = m_player_list.get_front_buffer_copy( );
            for ( const auto& player : list_copy ) {
                if ( player.is_valid( ) && player.m_actor ) {
                    render( player );
                }
            }
        }

        void loop_pickups( const std::function<void( lists::c_pickup const )>& render ) {
            if ( !config::pickups::enabled ) return;

            auto list_copy = m_pickup_list.get_front_buffer_copy( );
            for ( const auto& pickup : list_copy ) {
                if ( pickup.is_valid( ) ) {
                    render( pickup );
                }
            }
        }

        void loop_containers( const std::function<void( lists::c_container const )>& render ) {
            if ( !config::containers::enabled ) return;

            auto list_copy = m_container_list.get_front_buffer_copy( );
            for ( const auto& container : list_copy ) {
                if ( container.is_valid( ) ) {
                    render( container );
                }
            }
        }

        void loop_weakspots( const std::function<void( lists::c_weakspot const )>& render ) {
            if ( !config::weakspots::enabled ) return;

            auto list_copy = m_weakspot_list.get_front_buffer_copy( );
            for ( const auto& weakspot : list_copy ) {
                if ( weakspot.is_valid( ) ) {
                    render( weakspot );
                }
            }
        }

        void loop_projectiles( const std::function<void( lists::c_projectile const )>& render ) {
            if ( !config::projectiles::enabled ) return;

            auto list_copy = m_projectile_list.get_front_buffer_copy( );
            for ( const auto& projectile : list_copy ) {
                if ( projectile.is_valid( ) ) {
                    render( projectile );
                }
            }
        }

        void loop_vehicles( const std::function<void( lists::c_vehicle const )>& render ) {
            if ( !config::vehicles::enabled ) return;

            auto list_copy = m_vehicle_list.get_front_buffer_copy( );
            for ( const auto& vehicle : list_copy ) {
                if ( vehicle.is_valid( ) ) {
                    render( vehicle );
                }
            }
        }

        void loop_buildings( const std::function<void( lists::c_building const )>& render ) {
            if ( !config::buildings::enabled ) return;

            auto list_copy = m_building_list.get_front_buffer_copy( );
            for ( const auto& building : list_copy ) {
                if ( building.is_valid( ) ) {
                    render( building );
                }
            }
        }

        void loop_supply_drops( const std::function<void( lists::c_supply_drop const )>& render ) {
            if ( !config::supply_drops::enabled ) return;

            auto list_copy = m_supply_drop_list.get_front_buffer_copy( );
            for ( const auto& supply_drop : list_copy ) {
                if ( supply_drop.is_valid( ) ) {
                    render( supply_drop );
                }
            }
        }

        void loop_loot_llamas( const std::function<void( lists::c_loot_llama const )>& render ) {
            if ( !config::llamas::enabled ) return;

            auto list_copy = m_loot_llama_list.get_front_buffer_copy( );
            for ( const auto& loot_llama : list_copy ) {
                if ( loot_llama.is_valid( ) ) {
                    render( loot_llama );
                }
            }
        }

        std::vector<lists::c_bullet_tracer> get_tracer_list( ) {
            return m_tracer_list.get_front_buffer_copy( );
        }

        void update_tracer_list( std::vector<lists::c_bullet_tracer>&& updated_tracers ) {
            m_tracer_list.update_buffer( std::move( updated_tracers ) );
        }

        void update_tracer( ) {
            if ( !g_local_pawn->m_weapon || !g_local_pawn->m_weapon->is_firing( ) ||
                !g_local_pawn->m_fire_location.is_valid( ) ||
                !g_local_pawn->m_fire_direction.is_valid( ) ||
                !g_camera->m_rotation.is_valid( ) ) {
                return;
            }

            auto direction = g_camera->m_rotation.get_forward_vector( );
            if ( !direction.is_valid( ) ) return;

            auto dot_product = direction.vector_scalar( g_local_pawn->m_fire_direction );
            dot_product = ( dot_product < -1.0 ) ? -1.0 : ( dot_product > 1.0 ) ? 1.0 : dot_product;

            if ( g_local_pawn->m_projectile_distance <= 0.0 ||
                g_local_pawn->m_projectile_distance > 10000.0 ) return;

            auto start_location = g_local_pawn->m_fire_location;
            auto end_location = start_location + ( g_local_pawn->m_fire_direction * g_local_pawn->m_projectile_distance );

            if ( !start_location.is_valid( ) || !end_location.is_valid( ) ||
                is_duplicate_tracer( start_location, end_location ) ) return;

            lists::c_bullet_tracer bullet_tracer;
            bullet_tracer.m_start_location = start_location;
            bullet_tracer.m_end_location = end_location;
            bullet_tracer.m_impact_angle = acos( dot_product );
            bullet_tracer.m_lifetime = 0.0f;
            bullet_tracer.m_alpha = 1.0f;

            bullet_tracer.m_trail_points.emplace_back( end_location );

            auto current_tracers = get_tracer_list();
            current_tracers.emplace_back( std::move( bullet_tracer ) );
            update_tracer_list( std::move( current_tracers ) );
        }

        std::vector<lists::c_spectator> get_spectator_list( ) {
            return m_spectator_list;  // Direct return of vector
        }

        std::vector<lists::c_inventory> get_inventory_list( ) {
            return m_inventory_list.get_front_buffer_copy( );
        }

        void update_actors( ) {
            while ( true ) {
                try {
                    auto world = g_engine->m_world;
                    if ( !world ) {
                        utility::sleep_short( 120 );
                        continue;
                    }

                    if ( world && engine::is_valid( reinterpret_cast< std::uint64_t > ( world ) ) ) {
                        std::vector<engine::a_fort_player_pawn_athena*> player_cache;
                        std::vector<engine::a_fort_pickup*> pickup_cache;
                        std::vector<engine::a_building_container*> container_cache;
                        std::vector<engine::a_fort_projectile_athena*> projectile_cache;
                        std::vector<engine::a_fort_athena_vehicle*> vehicle_cache;
                        std::vector<engine::a_building_weakspot*> weakspot_cache;
                        std::vector<engine::a_building_actor*> building_cache;
                        std::vector<engine::a_fort_athena_supply_drop*> supply_drop_cache;
                        std::vector<engine::a_athena_creative_spawn_drop_c*> loot_llama_cache;

                        auto level_array = world->levels( );
                        if ( level_array.is_valid( ) && engine::is_valid( level_array.get_addr( ) ) ) {
                            for ( int idx = 0, level_size = level_array.size( ); idx < level_size; ++idx ) {
                                auto level = level_array.get( idx );
                                if ( !level || !engine::is_valid( reinterpret_cast< std::uint64_t > ( level ) ) ) continue;

                                auto actor_array = level->actors( );
                                if ( !actor_array.is_valid( ) || !engine::is_valid( actor_array.get_addr( ) ) ) continue;
                                if ( actor_array.size( ) > 10000 ) continue;

                                auto actor_size = actor_array.size( );
                                for ( int actor_idx = 0; actor_idx < actor_size; ++actor_idx ) {
                                    auto actor = actor_array.get( actor_idx );
                                    if ( !actor || !engine::is_valid( reinterpret_cast< std::uint64_t > ( actor ) ) ) continue;
                                    if ( engine::f_name::to_string( actor ).empty( ) ) continue;

                                    if ( auto player = reinterpret_cast< engine::a_fort_player_pawn_athena* >( actor ) ) {
                                        if ( player->is_player( ) && !player->b_is_dying( ) ) {
                                           player_cache.emplace_back( player );
                                        }
                                    }

                                    if ( config::pickups::enabled ) {
                                        if ( auto pickup = reinterpret_cast< engine::a_fort_pickup* >( actor ) ) {
                                            if ( pickup->is_pickup( ) ) {
                                                pickup_cache.emplace_back( pickup );
                                            }
                                        }
                                    }

                                    if ( config::containers::enabled ) {
                                        if ( auto container = reinterpret_cast< engine::a_building_container* >( actor ) ) {
                                            if ( container->is_container( ) && !container->b_already_searched( ) ) {
                                                container_cache.emplace_back( container );
                                            }
                                        }
                                    }

                                    if ( config::projectiles::enabled ) {
                                        if ( auto projectile = reinterpret_cast< engine::a_fort_projectile_athena* >( actor ) ) {
                                            if ( projectile->is_projectile( ) ) {
                                                projectile_cache.emplace_back( projectile );
                                            }
                                        }
                                    }

                                    if ( config::vehicles::enabled ) {
                                        if ( auto vehicle = reinterpret_cast< engine::a_fort_athena_vehicle* >( actor ) ) {
                                            if ( vehicle->is_vehicle( ) ) {
                                                vehicle_cache.emplace_back( vehicle );
                                            }
                                        }
                                    }

                                    if ( config::weakspots::enabled ) {
                                        if ( auto weakspot = reinterpret_cast< engine::a_building_weakspot* >( actor ) ) {
                                            if ( weakspot->is_weakspot( ) && !weakspot->b_hit( ) && weakspot->b_active( ) ) {
                                                weakspot_cache.emplace_back( weakspot );
                                            }
                                        }
                                    }

                                    if ( config::supply_drops::enabled ) {
                                        if ( auto supply_drop = reinterpret_cast< engine::a_fort_athena_supply_drop* >( actor ) ) {
                                            if ( supply_drop->is_supply_drop( ) ) {
                                                supply_drop_cache.emplace_back( supply_drop );
                                            }
                                        }
                                    }

                                    if ( config::buildings::enabled ) {
                                        if ( auto building = reinterpret_cast< engine::a_building_actor* >( actor ) ) {
                                            if ( building->is_building( ) &&
                                                ( !config::buildings::engine_placed || building->b_editor_placed( ) ) &&
                                                ( !config::buildings::player_placed || building->b_player_placed( ) ) ) {
                                                building_cache.emplace_back( building );
                                            }
                                        }
                                    }

                                    if ( config::llamas::enabled ) {
                                        if ( auto loot_llama = reinterpret_cast< engine::a_athena_creative_spawn_drop_c* >( actor ) ) {
                                            if ( true /*loot_llama->is_loot_llama( )*/ ) {
                                                loot_llama_cache.emplace_back( loot_llama );
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        m_player_cache.update_buffer( std::move( player_cache ) );
                        m_pickup_cache.update_buffer( std::move( pickup_cache ) );
                        m_container_cache.update_buffer( std::move( container_cache ) );
                        m_projectile_cache.update_buffer( std::move( projectile_cache ) );
                        m_vehicle_cache.update_buffer( std::move( vehicle_cache ) );
                        m_weakspot_cache.update_buffer( std::move( weakspot_cache ) );
                        m_building_cache.update_buffer( std::move( building_cache ) );
                        m_supply_drop_cache.update_buffer( std::move( supply_drop_cache ) );
                        m_loot_llama_cache.update_buffer( std::move( loot_llama_cache ) );
                    }

                    utility::sleep_short( 100 );
                }
                catch ( const std::exception& e ) {
                    logging::print( "thread crash cought: update_actors -> std::exception: %s", e.what( ) );
                }
                catch ( ... ) {
                    logging::print( "thread crash cought: update_actors -> Unknown exception" );
                }
            }
        }

    public:
        void update_engine( ) {
            while ( true ) {
                try {
                    auto world = engine::u_world::get( );
                    if ( !world ) {
                        if ( g_engine->m_world ) {
                            g_engine->m_world = nullptr;
                        }
                        utility::sleep_short( 5 );
                        continue;
                    }

                    auto game_state = world->game_state( );
                    if ( !game_state ) {
                        utility::sleep_short( 5 );
                        continue;
                    }

                    g_engine->m_world = world;
                    this->m_world_time_seconds = world->world_time_seconds( );

                    g_engine->m_game_state = game_state;
                    g_engine->m_in_lobby = game_state->server_world_time_seconds_delta( ) ? false : true;

                    if ( config::self::zone_prediction ) {
                        this->update_safe_zone( game_state );
                    }

                    auto game_instance = world->owning_game_instance( );
                    if ( game_instance ) {
                        g_engine->m_game_instance = game_instance;

                        auto local_player = game_instance->get_localplayer( );
                        if ( local_player ) {
                            g_engine->m_local_player = local_player;

                            auto viewport_client = local_player->viewport_client( );
                            if ( viewport_client ) {
                                g_engine->m_viewport_client = viewport_client;

                                auto view_state = local_player->get_view_state( );
                                if ( view_state ) {
                                    g_engine->m_view_state = view_state;

                                    auto projection = view_state->projection( );

                                    g_camera->m_rotation.pitch = engine::math::to_deg( std::asin( projection.z_plane.w ) );
                                    g_camera->m_rotation.yaw = engine::math::to_deg( std::atan2( projection.y_plane.w, projection.x_plane.w ) );
                                    g_camera->m_rotation.roll = 0.0;

                                    g_camera->m_location.x = projection.m[ 3 ][ 0 ];
                                    g_camera->m_location.y = projection.m[ 3 ][ 1 ];
                                    g_camera->m_location.z = projection.m[ 3 ][ 2 ];

                                    auto fov = atanf( 1 / view_state->field_of_view( ) ) * 2;
                                    g_camera->m_fov = engine::math::to_deg( fov );
                                }

                                auto player_controller = local_player->player_controller( );
                                if ( player_controller ) {
                                    g_engine->m_player_controller = player_controller;

                                    if ( config::self::inventory ) {
                                        this->update_inventory( player_controller );
                                    }

                                    auto player_camera_manager = player_controller->player_camera_manager( );
                                    if ( player_camera_manager ) {
                                        g_engine->m_player_camera_manager = player_camera_manager;
                                    }
                                }
                            }
                        }
                    }
                }
                catch ( const std::exception& e ) {
                    logging::print( "thread crash cought: update_engine -> std::exception: %s", e.what( ) );
                }
                catch ( ... ) {
                    logging::print( "thread crash cought: update_engine -> Unknown exception" );
                }
                utility::sleep_short( 1 );
            }
        }

        void update_exploits( ) {
            while ( true ) {
                try {
                    auto viewport_client = g_engine->m_viewport_client;
                    if ( viewport_client ) {
                        if ( config::exploits::dark_sky ) {
                            g_driver->write( ( uintptr_t )viewport_client + 0xB0, true );
                        }
                    }

                    if ( config::exploits::fov_changer ) {
                        auto player_camera_manager = g_engine->m_player_camera_manager;
                        if ( player_camera_manager ) {
                            player_camera_manager->locked_fov( config::exploits::min_fov );
                        }
                    }

                    auto player_controller = g_engine->m_player_controller;
                    if ( !player_controller ) 
                        continue;

                    auto local_pawn = player_controller->acknowledged_pawn( );
                    if ( local_pawn ) {
						auto local_vehicle = local_pawn->current_vehicle( );
                        if ( local_vehicle ) {
                            if ( config::exploits::infinite_fuel ) {
                                auto cached_fuel_component = local_vehicle->cached_fuel_component( );
                                if ( cached_fuel_component ) {
                                    cached_fuel_component->server_fuel( FLT_MAX );
                                    cached_fuel_component->infinite_fuel( FLT_MAX );
                                }
                            }

                            if ( config::exploits::bike_jump ) {
                                auto motorcycle_vehicle = local_vehicle->motorcycle_vehicle_config( );
                                if ( motorcycle_vehicle ) {
                                    motorcycle_vehicle->max_charge_jump_force( config::exploits::max_jump_force );
                                    motorcycle_vehicle->max_jump_forward_angle( config::exploits::max_jump_forward );

                                    motorcycle_vehicle->max_jump_charge_time( 0.0f );
                                    motorcycle_vehicle->jump_charge_grace_time_seconds( 0.0f );
                                    motorcycle_vehicle->jump_grace_time_seconds( 0.0f );
                                }
                            }

                            if ( config::exploits::vehicle_fly ) {
                                if ( GetAsyncKeyState( config::exploits::key_up ) ) {
                                    local_vehicle->vehicle_gravity_scale( 10 * -1 );
                                }
                                else if ( GetAsyncKeyState( config::exploits::key_down ) ) {
                                    local_vehicle->vehicle_gravity_scale( 10 );
                                }
                                else {
                                    local_vehicle->vehicle_gravity_scale( 1 );
                                }
                            }
                        }

                        auto local_weapon = local_pawn->current_weapon( );
                        if ( local_weapon ) {
							auto weapon_animation = local_weapon->weapon_core_animation( );
                            if ( weapon_animation != engine::e_weapon_core_animation::melee &&
                                weapon_animation != engine::e_weapon_core_animation::melee_one_hand &&
                                weapon_animation  != engine::e_weapon_core_animation::unarmed ) {

                                //if ( config::exploits::firerate_modifier ) {
                                //    // Weapon.UsePerBulletRecoil
                                //    g_driver->write( g_driver->target::m_base_address + 0x17DEEA5A, true );

                                //    auto a1 = ( std::uint64_t )local_weapon;

                                //    auto control_flow_0 = g_driver->read<float>( a1 + 0x2448 );

                                //    auto a2 = g_driver->read( a1 + 0x2C8 );
                                //    if ( a2 ) {
                                //        auto weapon_child = local_weapon->get_child( );
                                //        auto firerate_modifier = 1;

                                //        auto control_flow_2 = g_driver->read<std::uint32_t>( a2 + 0x400 );
                                //        logging::print( encrypt( "control_flow_2: 0x%i" ), control_flow_2 );

                                //        if ( weapon_child ) {

                                //            g_driver->write<float>( weapon_child + 0x840, 7 );
                                //            g_driver->write<float>( a1 + 0x122C, firerate_modifier );
                                //            g_driver->write<float>( a1 + 0x11AC, 7 );

                                //            g_driver->write<float>( a1 + 0x68, firerate_modifier );
                                //            //g_driver->write<float>( a1 + 0x2448, firerate_modifier );
                                //            //g_driver->write<float>( a1 + 0x1E00, firerate_modifier );

                                //            auto world_time = g_driver->read< double >( weapon_child + 0x840 );
                                //            auto weapon_time = g_driver->read< float >( a1 + 0x122C );
                                //            auto weapon_factor = g_driver->read< float >( a1 + 0x68 );

                                //            if ( world_time > 0.0 && weapon_factor > 0.0f ) {
                                //                firerate_modifier = ( float )( ( world_time - weapon_time ) * weapon_factor );
                                //            }
                                //        }

                                //        // return (*(_BYTE *)(v1 + 0x2B2) & 8) == 0;
                                //        auto control_flow_1 = ( g_driver->read<std::uint8_t>( a2 + 0x2B2 ) & 8 ) == 0;

                                //        auto v8 = g_driver->read<std::uint32_t>( a2 + 16LL * ( unsigned __int8 )control_flow_2 + 0x3F0 );
                                //        if ( v8 ) {
                                //            auto v3 = g_driver->read<float*>( a2 + 16LL * ( unsigned __int8 )control_flow_2 + 0x3E8 );
                                //            if ( v3 ) {
                                //                auto fifth_element = g_driver->read<float>( ( uintptr_t )v3 + ( 5 * sizeof( float ) ) );
                                //                if ( fifth_element > 0.0f ) {
                                //                    logging::print( encrypt( "fifth_element: 0x%f" ), fifth_element );

                                //                    auto current_weapon_time = g_driver->read<float>( a1 + 0x2448 );
                                //                    if ( current_weapon_time <= 0.0f ) {
                                //                        continue;
                                //                    }
                                //                    
                                //                    if ( a2 ) {
                                //                        auto weapon_type = ( unsigned __int8 )control_flow_2;
                                //                        auto v7 = 2LL * weapon_type;
                                //                        auto v8 = g_driver->read<int>( a2 + 16LL * weapon_type + 0x3F0 );

                                //                        if ( v8 > 0 ) {
                                //                            auto v3_ptr = g_driver->read<uintptr_t>( a2 + 16LL * weapon_type + 0x3E8 );
                                //                            
                                //                            auto v3_element_5 = g_driver->read<float>( v3_ptr + ( 5 * sizeof( float ) ) );

                                //                            if ( v3_element_5 > 0.0f ) {
                                //                                
                                //                                int v9 = 0;
                                //                                float v10 = 0.0f, v11 = 0.0f, v12 = 0.0f, v13 = 0.0f;
                                //                                int v14 = 0;
                                //                                uintptr_t v15 = 0;
                                //                                
                                //                                while ( true ) {
                                //                                    if ( v14 >= 0 && v9 < g_driver->read<int>( a2 + 8 * v7 + 0x3F0 ) ) {
                                //                                        auto v16 = g_driver->read<uintptr_t>( a2 + 8 * v7 + 0x3E8 );
                                //                                        auto v17 = v15 + v16;
                                //                                        
                                //                                        // if ( (float)*(int *)(v15 + v16) >= *(float *)(a1 + 0x2448) )
                                //                                        if ( ( float )g_driver->read<int>( v15 + v16 ) >= current_weapon_time ) {
                                //                                            auto v18 = g_driver->read<float>( v17 + 0x14 );
                                //                                            if ( v18 > 0.0f ) {
                                //                                                v12 = v18;
                                //                                                v13 = ( float )g_driver->read<int>( v16 + 24LL * v9 );
                                //                                                goto LABEL_22;
                                //                                            }
                                //                                        }
                                //                                        
                                //                                        auto v19 = g_driver->read<float>( v17 + 20 );
                                //                                        if ( v19 > 0.0f ) {
                                //                                            v11 = ( float )g_driver->read<int>( v15 + v16 );
                                //                                            v10 = v19;
                                //                                        }
                                //                                        
                                //                                        if ( v9 == v8 - 1 ) {
                                //                                            if ( v19 <= 0.0f ) {
                                //                                                v13 = v11;
                                //                                                v12 = v10;
                                //                                            } else {
                                //                                                v13 = ( float )g_driver->read<int>( v15 + v16 );
                                //                                                v12 = v19;
                                //                                            }
                                //                                        }
                                //                                    }
                                //                                    
                                //                                    ++v9;
                                //                                    ++v14;
                                //                                    v15 += 24;
                                //                                    if ( v14 >= v8 ) {
                                //                                        goto LABEL_22;
                                //                                    }
                                //                                }
                                //                                
                                //                                LABEL_22:
                                //                                float v20;
                                //                                if ( v13 <= v11 ) {
                                //                                    v20 = v12;
                                //                                } else {
                                //                                    v20 = ( ( g_driver->read<float>( a1 + 9288 ) - v11 ) / ( v13 - v11 ) ) * ( v12 - v10 ) + v10;
                                //                                }

                                //                                float v28 = fmaxf( v20 - firerate_modifier, 0.0f );
                                //                                
                                //                                int v21 = 0, v22 = 0;
                                //                                uintptr_t v27 = 0;
                                //                                float v23 = 0.0f, v24 = 0.0f, v25 = 0.0f, v26 = 0.0f;

                                //                                while ( true ) {
                                //                                    if ( v22 >= 0 && v21 < g_driver->read<int>( a2 + 8 * v7 + 0x3F0 ) ) {
                                //                                        auto v29 = g_driver->read<uintptr_t>( a2 + 8 * v7 + 0x3E8 );
                                //                                        auto v30 = g_driver->read<float>( v27 + v29 + 0x14 );
                                //                                        
                                //                                        if ( v28 <= v30 ) {
                                //                                            auto v32 = 3LL * v21;
                                //                                            if ( g_driver->read<float>( v29 + 8 * v32 + 0x14 ) > 0.0f ) {
                                //                                                v25 = g_driver->read<float>( v29 + 8 * v32 + 0x14 );
                                //                                                v26 = ( float )g_driver->read<int>( v29 + 8 * v32 );
                                //                                            }
                                //                                            goto LABEL_40;
                                //                                        }
                                //                                        
                                //                                        if ( v30 > 0.0f ) {
                                //                                            v23 = g_driver->read<float>( v27 + v29 + 0x14 );
                                //                                            v24 = ( float )g_driver->read<int>( v27 + v29 );
                                //                                        }
                                //                                        
                                //                                        if ( v21 == v8 - 1 ) {
                                //                                            auto v31 = 3LL * v21;
                                //                                            v25 = g_driver->read<float>( v29 + 8 * v31 + 20 );
                                //                                            if ( v25 <= 0.0f ) {
                                //                                                v26 = v24;
                                //                                                v25 = v23;
                                //                                            } else {
                                //                                                v26 = ( float )g_driver->read<int>( v29 + 8 * v31 );
                                //                                            }
                                //                                            v28 = v25;
                                //                                            goto LABEL_40;
                                //                                        }
                                //                                    }
                                //                                    
                                //                                    ++v21;
                                //                                    ++v22;
                                //                                    v27 += 24;
                                //                                    if ( v22 >= v8 ) {
                                //                                        goto LABEL_40;
                                //                                    }
                                //                                }
                                //                                
                                //                                LABEL_40:
                                //                                float v33 = v25 - v23;
                                //                                float final_result;
                                //                                if ( v33 <= 0.0f ) {
                                //                                    final_result = v24;
                                //                                } else {
                                //                                    final_result = ( ( v28 - v23 ) / v33 ) * ( v26 - v24 ) + v24;
                                //                                }
                                //                                
                                //                                g_driver->write<float>( a1 + 0x2448, final_result );
                                //                                logging::print( encrypt( "[FIRERATE] Original: %f -> Modified: %f" ), v20, final_result );
                                //                            }
                                //                        }
                                //                    }
                                //                }
                                //            }
                                //        }
                                //    }
                                //}

                                if ( config::exploits::no_recoil ) {
                                    g_driver->write<double>( ( uintptr_t )local_weapon + 0x2318, config::exploits::recoil_x );
                                    g_driver->write<double>( ( uintptr_t )local_weapon + 0x2320, config::exploits::recoil_y );
                                    g_driver->write<double>( ( uintptr_t )local_weapon + 0x2328, config::exploits::recoil_z );
                                }

                                // no update because I don't want u to have
                                if ( config::exploits::no_spread ) {
                                    uint32_t desired_spread_x = config::exploits::spread_x;
                                    uint32_t desired_spread_y = config::exploits::spread_y;

                                    auto pad_0x52 = g_driver->read<unsigned __int8>( ( std::uintptr_t )local_weapon + 0x23BA );
                                    auto spread_field_one_ptr = ( std::uintptr_t )( local_weapon + ( 4 * pad_0x52 ) ) + 0x23AA;
                                    if ( engine::is_valid( spread_field_one_ptr ) ) {
                                        auto pad_0x53 = g_driver->read<unsigned __int8>( ( std::uintptr_t )local_weapon + 0x23BB );
                                        auto spread_field_one_xor = ( ( pad_0x53 - 0xE886 ) & 0x82FFFFFE ) | 0x4C000001;

                                        auto encrypted_value = g_driver->read<uint32_t>( ( std::uintptr_t )spread_field_one_ptr );
                                        auto decrypted_spread = encrypted_value ^ spread_field_one_xor;

                                        auto current_spread = static_cast< float >( static_cast< unsigned __int8 >( decrypted_spread ) );
                                        if ( current_spread >= 0.0f && current_spread <= 255.0f ) {
                                            auto spread_invalidator = desired_spread_x ^ spread_field_one_xor;
                                            g_driver->write<uint32_t>( spread_field_one_ptr, spread_invalidator );
                                        }
                                    }

                                    auto pad_0x28 = g_driver->read<unsigned __int8>( ( std::uintptr_t )local_weapon + 0x23A8 );
                                    auto spread_field_two_ptr = ( std::uintptr_t )( local_weapon + ( 4 * pad_0x28 ) ) + 0x2398;
                                    if ( engine::is_valid( spread_field_two_ptr ) ) {
                                        auto pad_0x29 = g_driver->read<unsigned __int8>( ( std::uintptr_t )local_weapon + 0x23A9 );
                                        auto spread_field_two_xor = ( ( pad_0x29 - 0xE886 ) & 0x82FFFFFE ) | 0x4C000001;

                                        auto encrypted_value = g_driver->read<uint32_t>( ( std::uintptr_t )spread_field_two_ptr );
                                        auto decrypted_spread = encrypted_value ^ spread_field_two_xor;

                                        auto current_spread = static_cast< float >( static_cast< unsigned __int8 >( decrypted_spread ) );
                                        if ( current_spread >= 0.0f && current_spread <= 255.0f ) {
                                            auto spread_invalidator = desired_spread_y ^ spread_field_two_xor;
                                            g_driver->write<uint32_t>( spread_field_two_ptr, spread_invalidator );
                                        }
                                    }
                                }

                                // no update because I don't want u to have
                                if ( config::exploits::shoot_through_walls ) {
                                    g_driver->write<engine::f_rotator>( g_driver->m_base_address + 0x186B7460, engine::f_rotator( 0, 16, 16 ) );
                                    g_driver->write<engine::f_rotator>( g_driver->m_base_address + 0x186B7460, engine::f_rotator( 0, 0, 0 ) );
                                }

                                // no update because I don't want u to have
                                if ( config::exploits::hit_sound ) {
                                    auto new_sound = engine::u_object::get_object<engine::u_sound_base>( 34622 );
                                    if ( new_sound ) {
                                        local_weapon->primary_fire_sound_1p( new_sound );
                                        local_weapon->primary_fire_stop_sound_1p( new_sound );

                                        for ( auto idx = 0ul; idx < 0x3; idx++ ) {
                                            local_weapon->primary_fire_sound( idx, new_sound );
                                            local_weapon->primary_fire_stop_sound( idx, new_sound );
                                            local_weapon->secondary_fire_sound( idx, new_sound );
                                            local_weapon->secondary_fire_stop_sound( idx, new_sound );
                                        }

                                        for ( auto idx = 0ul; idx < 0x23; idx++ ) {
                                            local_weapon->impact_physical_surface_sounds( idx, new_sound );
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                catch ( const std::exception& e ) {
                    logging::print( "thread crash cought: update_exploits -> std::exception: %s", e.what( ) );
                }
                catch ( ... ) {
                    logging::print( "thread crash cought: update_exploits -> Unknown exception" );
                }
                utility::sleep_short( 1 );
            }
        }

        void update_world( ) {
            while ( true ) {
                try {
                    std::vector< lists::c_pickup     > pickups_list;
                    std::vector< lists::c_container  > containers_list;
                    std::vector< lists::c_projectile > projectiles_list;
                    std::vector< lists::c_vehicle    > vehicles_list;
                    std::vector< lists::c_weakspot   > weakspots_list;
                    std::vector< lists::c_building   > buildings_list;
                    std::vector< lists::c_supply_drop> supply_drop_list;
                    std::vector< lists::c_loot_llama > loot_llama_list;
                    std::vector<lists::c_player> players_list;

                    auto pickup_cache    = m_pickup_cache.get_front_buffer_copy( );
                    auto container_cache = m_container_cache.get_front_buffer_copy( );
                    auto projectile_cache= m_projectile_cache.get_front_buffer_copy( );
                    auto vehicle_cache   = m_vehicle_cache.get_front_buffer_copy( );
                    auto weakspot_cache  = m_weakspot_cache.get_front_buffer_copy( );
                    auto building_cache  = m_building_cache.get_front_buffer_copy( );
                    auto supply_cache    = m_supply_drop_cache.get_front_buffer_copy( );
                    auto llama_cache     = m_loot_llama_cache.get_front_buffer_copy( );
                    auto player_cache = m_player_cache.get_front_buffer_copy( );

                    pickups_list.reserve( pickup_cache.size( ) );
                    containers_list.reserve( container_cache.size( ) );
                    projectiles_list.reserve( projectile_cache.size( ) );
                    vehicles_list.reserve( vehicle_cache.size( ) );
                    weakspots_list.reserve( weakspot_cache.size( ) );
                    buildings_list.reserve( building_cache.size( ) );
                    supply_drop_list.reserve( supply_cache.size( ) );
                    loot_llama_list.reserve( llama_cache.size( ) );
                    players_list.reserve( player_cache.size( ) );

                    for ( const auto& actor : pickup_cache ) {
                        if ( !actor ) continue;
                        if ( auto p = update_pickup( actor ); p.is_valid( ) ) pickups_list.emplace_back( std::move( p ) );
                    }
                    for ( const auto& actor : container_cache ) {
                        if ( !actor ) continue;
                        if ( auto c = update_container( actor ); c.is_valid( ) ) containers_list.emplace_back( std::move( c ) );
                    }
                    for ( const auto& actor : projectile_cache ) {
                        if ( !actor ) continue;
                        if ( auto pr = update_projectiles( actor ); pr.is_valid( ) ) projectiles_list.emplace_back( std::move( pr ) );
                    }
                    for ( const auto& actor : vehicle_cache ) {
                        if ( !actor ) continue;
                        if ( auto v = update_vehicle( actor ); v.is_valid( ) ) vehicles_list.emplace_back( std::move( v ) );
                    }
                    for ( const auto& actor : weakspot_cache ) {
                        if ( !actor ) continue;
                        if ( auto w = update_weakspot( actor ); w.is_valid( ) ) weakspots_list.emplace_back( std::move( w ) );
                    }
                    for ( const auto& actor : building_cache ) {
                        if ( !actor ) continue;
                        if ( auto b = update_building( actor ); b.is_valid( ) ) buildings_list.emplace_back( std::move( b ) );
                    }
                    for ( const auto& actor : supply_cache ) {
                        if ( !actor ) continue;
                        if ( auto sd = update_supply_drop( actor ); sd.is_valid( ) ) supply_drop_list.emplace_back( std::move( sd ) );
                    }
                    for ( const auto& actor : llama_cache ) {
                        if ( !actor ) continue;
                        if ( auto ll = update_loot_llama( actor ); ll.is_valid( ) ) loot_llama_list.emplace_back( std::move( ll ) );
                    }
                    for ( auto idx = 0; idx < player_cache.size( ); ++idx ) {
                        auto actor = player_cache[ idx ];
                        if ( !actor ) continue;

                        auto player_controller = actor->controller( );
                        if ( player_controller && player_controller->b_is_local_player_controller( ) ) {
                            g_local_pawn->m_actor = actor;

                            auto player_state = actor->player_state( );
                            if ( player_state ) {
                                g_local_pawn->m_state = player_state;
                                //update_spectators( player_state );
                            }

                            auto root_component = actor->root_component( );
                            if ( root_component ) {
                                g_local_pawn->m_rotation = root_component->relative_rotation( );
                            }

                            auto mesh = actor->mesh( );
                            if ( mesh ) {
                                auto bone_array = mesh->bone_array( );
                                if ( bone_array.is_valid( ) ) {
                                    auto head_location = mesh->get_bone_location( bone_array, engine::e_bone::vb_head_fx );
                                    if ( head_location.is_valid( ) ) {
                                        g_local_pawn->m_location = head_location;
                                    }
                                }
                            }

                            auto weapon = actor->current_weapon( );
                            if ( weapon ) {
                                g_local_pawn->m_weapon = weapon;
                                g_local_pawn->m_fire_location = actor->last_fired_location( );
                                g_local_pawn->m_fire_direction = actor->last_fired_direction( );
                                g_local_pawn->m_weapon_animation = weapon->weapon_core_animation( );
                                g_local_pawn->m_projectile_speed = weapon->projectile_speed( );
                                g_local_pawn->m_projectile_gravity = weapon->projectile_gravity( );
                                g_local_pawn->m_projectile_distance = weapon->current_projected_impact_distance( );
                            }

                            auto vehicle = actor->current_vehicle( );
                            if ( vehicle ) {
                                g_local_pawn->m_vehicle = vehicle;
                            }

                            continue;
                        }

                        if ( auto player = update_player( actor ); player.is_valid( ) ) {
                            players_list.emplace_back( std::move( player ) );
                        }
                    }

                    m_pickup_list.update_buffer( std::move( pickups_list ) );
                    m_container_list.update_buffer( std::move( containers_list ) );
                    m_projectile_list.update_buffer( std::move( projectiles_list ) );
                    m_vehicle_list.update_buffer( std::move( vehicles_list ) );
                    m_weakspot_list.update_buffer( std::move( weakspots_list ) );
                    m_building_list.update_buffer( std::move( buildings_list ) );
                    m_supply_drop_list.update_buffer( std::move( supply_drop_list ) );
                    m_loot_llama_list.update_buffer( std::move( loot_llama_list ) );
                    m_player_list.update_buffer( std::move( players_list ) );

                    utility::sleep_short( 1 );
                }
                catch ( const std::exception& e ) {
                    logging::print( "thread crash cought: update_world -> std::exception: %s", e.what( ) );
                }
                catch ( ... ) {
                    logging::print( "thread crash cought: update_world -> Unknown exception" );
                }
            }
        }

        void update_network( ) {
            while ( true ) {
                try {
                    auto player_list_copy = m_player_list.get_front_buffer_copy( );
                    if ( player_list_copy.empty( ) ) {
                        m_player_stats_cache.clear( );
                        utility::sleep_short( 1 );
                        continue;
                    }

                    for ( const auto& player : player_list_copy ) {
                        if ( player.m_player_name.empty( ) || !player.m_actor ) continue;
                        auto it = m_player_stats_cache.find( player.m_actor );
                        if ( it == m_player_stats_cache.end( ) ) {
                            auto stats = g_player_parser->get_player_stats( player.m_player_name );
                            m_player_stats_cache[ player.m_actor ] = stats;
                        }
                    }

                    utility::sleep_short( 1 );
                }
                catch ( const std::exception& e ) {
                    logging::print( "thread crash cought: update_network -> std::exception: %s", e.what( ) );
                }
                catch ( ... ) {
                    logging::print( "thread crash cought: update_network -> Unknown exception" );
                }
            }
        }

        double m_world_time_seconds = 0.0;
        std::unordered_map<engine::a_fort_player_pawn_athena*, parser::player_stats_t*> m_player_stats_cache;
        
        static inline std::atomic<uint64_t> s_last_update_actors_heartbeat = 0;
        static inline std::atomic<uint64_t> s_update_actors_iteration_count = 0;
        static inline std::atomic<bool> s_update_actors_crashed = false;

    private:
        double_buffered_cache<engine::a_fort_player_pawn_athena*> m_player_cache;
        double_buffered_cache<engine::a_fort_pickup*> m_pickup_cache;
        double_buffered_cache<engine::a_building_container*> m_container_cache;
        double_buffered_cache<engine::a_fort_projectile_athena*> m_projectile_cache;
        double_buffered_cache<engine::a_fort_athena_vehicle*> m_vehicle_cache;
        double_buffered_cache<engine::a_building_weakspot*> m_weakspot_cache;
        double_buffered_cache<engine::a_building_actor*> m_building_cache;
        double_buffered_cache<engine::a_fort_athena_supply_drop*> m_supply_drop_cache;
        double_buffered_cache<engine::a_athena_creative_spawn_drop_c*> m_loot_llama_cache;

        // Lists
        double_buffered_cache<lists::c_player> m_player_list;
        double_buffered_cache<lists::c_pickup> m_pickup_list;
        double_buffered_cache<lists::c_container> m_container_list;
        double_buffered_cache<lists::c_projectile> m_projectile_list;
        double_buffered_cache<lists::c_vehicle> m_vehicle_list;
        double_buffered_cache<lists::c_weakspot> m_weakspot_list;
        double_buffered_cache<lists::c_building> m_building_list;
        double_buffered_cache<lists::c_supply_drop> m_supply_drop_list;
        double_buffered_cache<lists::c_loot_llama> m_loot_llama_list;
        double_buffered_cache<lists::c_bullet_tracer> m_tracer_list;
        std::vector<lists::c_spectator> m_spectator_list;  // Changed to basic vector
        double_buffered_cache<lists::c_inventory> m_inventory_list;

        std::uint64_t m_update_damage_start_location = 0;
        std::uint64_t m_weapon_vtable = 0;
        bool m_initialized_hook = false;
        bool m_thread_debugging = false;

        bool is_duplicate_tracer( engine::f_vector& start, engine::f_vector& end, double threshold = 0.30 ) {
            auto tracer_copy = m_tracer_list.get_front_buffer_copy( );
            for ( const auto& tracer : tracer_copy ) {
                auto tracer_start = tracer.m_start_location;
                auto tracer_end = tracer.m_end_location;
                if ( tracer_start.distance_to( start ) < threshold &&
                    tracer_end.distance_to( end ) < threshold )
                    return true;
            }
            return false;
        }

        bool is_duplicate_item( std::vector<lists::c_inventory>& inventory_list, lists::c_inventory inventory_item ) {
            for ( const auto& inventory : inventory_list ) {
                auto& weapon_name = inventory.m_weapon_name;
                if ( weapon_name == inventory_item.m_weapon_name )
                    return true;
            }
            return false;
        }

        void update_safe_zone( engine::a_game_state_base* game_state ) {
            lists::c_safe_zone safe_zone;

            if ( !g_engine->m_in_lobby ) {
                auto game_phase = game_state->get_game_phase( );
                if ( game_phase ) {
                    auto safe_zone_indicator = game_phase->safe_zone_indicator( );
                    if ( safe_zone_indicator ) {
                        auto finish_shrink_time = safe_zone_indicator->safe_zone_finish_shrink_time( );
                        auto time_remaining = finish_shrink_time - m_world_time_seconds;
                        if ( time_remaining > 0.0 ) {
                            safe_zone.m_indicator = safe_zone_indicator;
                            safe_zone.m_radius = safe_zone_indicator->radius( );
                            safe_zone.m_center = safe_zone_indicator->next_center( );
                            safe_zone.m_next_radius = safe_zone_indicator->next_radius( );
                            safe_zone.m_next_center = safe_zone_indicator->next_next_center( );
                            safe_zone.m_phase = safe_zone_indicator->current_phase( );
                            safe_zone.m_phase_count = safe_zone_indicator->phase_count( );
                            safe_zone.m_height = safe_zone_indicator->safe_zone_height( );
                            safe_zone.m_time_remaining = time_remaining;

                            auto current_damage = safe_zone_indicator->current_damage_info( );
                            if ( current_damage.b_percentage_based_damage ) {
                                safe_zone.m_damage = current_damage.damage;
                            }
                        }
                    }
                }
            }

            std::swap( g_engine->m_zone, safe_zone );
        }

        void update_inventory( engine::a_player_controller* player_controller ) {
            std::vector<lists::c_inventory> inventory_list;

            auto client_quick_bars = player_controller->client_quick_bars( );
            if ( client_quick_bars ) {
                auto add_inventory_items = [ & ]( engine::f_quick_bar quick_bar ) {
                    auto item_definitions = quick_bar.equipped_item_definitions.sparse_array;
                    if ( item_definitions.is_valid( ) ) {
                        for ( auto i = 0; i < item_definitions.size( ) * 2; i++ ) {
                            auto item_definition = item_definitions.get( i );
                            if ( !item_definition ) continue;

                            lists::c_inventory inventory_item;
                            inventory_item.m_item_definition = item_definition;
                            inventory_item.m_weapon_name = inventory_item.m_item_definition->item_name( )->get( );
                            inventory_item.m_weapon_rarity = inventory_item.m_item_definition->rarity( );

                            if ( !is_duplicate_item( inventory_list, inventory_item ) ) {
                                inventory_list.emplace_back( inventory_item );
                            }
                        }
                    }
                    };

                add_inventory_items( client_quick_bars->primary_quick_bar( ) );
                add_inventory_items( client_quick_bars->secondary_quick_bar( ) );
                add_inventory_items( client_quick_bars->creative_quick_bar( ) );
            }

            m_inventory_list.update_buffer( std::move( inventory_list ) );
        }

        void update_spectators( engine::a_player_state* player_state ) {
            try {
                std::vector <lists::c_spectator> spectator_list;

                auto spectator_array = player_state->spectators( );
                if ( spectator_array.is_valid( ) ) {
                    for ( auto idx = 0ul; idx < spectator_array.size( ); idx++ ) {
                        auto spectator_state = g_driver->read<engine::a_player_state*>( spectator_array.get_addr( ) + ( idx * 0x18 ) + 0x10 );
                        if ( !spectator_state ) continue;

                        lists::c_spectator spectator;
                        spectator.m_state = spectator_state;
                        spectator.m_index = spectator_state->player_id( );
                        spectator.m_platform_name = spectator_state->get_platform_name( );
                        spectator.m_spectator_name = spectator_state->get_player_name( g_engine->m_in_lobby );

                        spectator_list.emplace_back( std::move( spectator ) );
                    }
                }

                //m_spectator_list.swap( spectator_list );
            }
            catch ( const std::exception& e ) {
                logging::print( "thread crash cought: update_spectators -> std::exception: %s", e.what( ) );
            }
            catch ( ... ) {
                logging::print( "thread crash cought: update_spectators -> Unknown exception" );
            }
        }

        lists::c_loot_llama update_loot_llama( engine::a_athena_creative_spawn_drop_c* actor ) const {
            auto root_component = actor->root_component( );
            if ( !root_component ) return {};

            auto world_location = root_component->relative_location( );
            if ( !world_location.is_valid( ) ) return {};

            auto world_screen = engine::project( world_location );
            if ( !world_screen.in_screen( ) || !world_location.is_valid( ) ) return {};

            auto distance = g_camera->m_location.distance_to( world_location );
            if ( distance >= config::buildings::max_distance ) return {};

            lists::c_loot_llama loot_llama;
            loot_llama.m_actor = actor;
            loot_llama.m_root_component = root_component;
            loot_llama.m_world_location = world_location;
            loot_llama.m_class_name = engine::f_name( actor->class_private( ) ).to_string( );
            loot_llama.m_distance = distance;

            return loot_llama;
        }

        lists::c_supply_drop update_supply_drop( engine::a_fort_athena_supply_drop* actor ) const {
            auto root_component = actor->root_component( );
            if ( !root_component ) return {};

            auto world_location = root_component->relative_location( );
            if ( !world_location.is_valid( ) ) return {};

            auto world_screen = engine::project( world_location );
            if ( !world_screen.in_screen( ) || !world_location.is_valid( ) ) return {};

            auto distance = g_camera->m_location.distance_to( world_location );
            if ( distance >= config::buildings::max_distance ) return {};

            lists::c_supply_drop supply_drop;
            supply_drop.m_actor = actor;
            supply_drop.m_root_component = root_component;
            supply_drop.m_world_location = world_location;
            supply_drop.m_players_interacting = actor->num_players_interacting( );
            supply_drop.m_distance = distance;

            return supply_drop;
        }

        lists::c_building update_building( engine::a_building_actor* actor ) const {
            auto root_component = actor->root_component( );
            if ( !root_component ) return {};

            auto world_location = root_component->relative_location( );
            if ( !world_location.is_valid( ) ) return {};

            auto world_screen = engine::project( world_location );
            if ( !world_screen.in_screen( ) || !world_location.is_valid( ) ) return {};

            auto distance = g_camera->m_location.distance_to( world_location );
            if ( distance >= config::buildings::max_distance ) return {};

            lists::c_building building;
            building.m_actor = actor;
            building.m_root_component = root_component;
            building.m_world_location = world_location;
            building.m_mesh_component = actor->static_mesh_component( );
            building.m_building_type = actor->building_type( );
            building.m_distance = distance;

            if ( building.m_mesh_component ) {
                building.m_bounds = building.m_mesh_component->get_bounds( );
            }

            return building;
        }

        lists::c_weakspot update_weakspot( engine::a_building_weakspot* actor ) const {
            auto root_component = actor->root_component( );
            if ( !root_component ) return {};

            auto world_location = root_component->relative_location( );
            if ( !world_location.is_valid( ) ) return {};

            auto world_screen = engine::project( world_location );
            if ( !world_screen.in_screen( ) || !world_screen.is_valid( ) ) return {};

            auto distance = g_camera->m_location.distance_to( world_location );
            if ( distance >= config::weakspots::max_distance ) return {};

            lists::c_weakspot weakspot;
            weakspot.m_actor = actor;
            weakspot.m_root_component = root_component;
            weakspot.m_world_location = world_location;
            weakspot.m_world_screen = world_screen;
            weakspot.m_distance = distance;

            return weakspot;
        }

        lists::c_vehicle update_vehicle( engine::a_fort_athena_vehicle* actor ) const {
            auto root_component = actor->root_component( );
            if ( !root_component ) return {};

            auto world_location = root_component->relative_location( );
            if ( !world_location.is_valid( ) ) return {};

            auto world_screen = engine::project( world_location );
            if ( !world_screen.in_screen( ) || !world_screen.is_valid( ) ) return {};

            auto distance = g_camera->m_location.distance_to( world_location );
            if ( distance >= config::vehicles::max_distance ) return {};

            lists::c_vehicle vehicle;
            vehicle.m_actor = actor;
            vehicle.m_root_component = root_component;
            vehicle.m_world_location = world_location;
            vehicle.m_on_road = actor->b_on_road( );
            vehicle.m_on_landscape = actor->b_on_landscape( );
            vehicle.m_on_dirt = actor->b_on_dirt( );
            vehicle.m_on_grass = actor->b_on_grass( );
            vehicle.m_on_ice = actor->b_on_ice( );
            vehicle.m_on_snow = actor->b_on_snow( );
            vehicle.m_on_mud = actor->b_on_mud( );
            vehicle.m_exploded = actor->b_exploded( );
            vehicle.m_critical_health = actor->server_critical_health( );
            vehicle.m_distance = distance;
            return vehicle;
        }

        lists::c_projectile update_projectiles( engine::a_fort_projectile_athena* actor ) const {
            auto root_component = actor->root_component( );
            if ( !root_component ) return {};

            auto world_location = root_component->relative_location( );
            if ( !world_location.is_valid( ) ) return {};

            auto world_screen = engine::project( world_location );
            if ( !world_screen.in_screen( ) || !world_screen.is_valid( ) ) return {};

            auto distance = g_camera->m_location.distance_to( world_location );
            if ( distance >= config::projectiles::max_distance ) return {};

            lists::c_projectile projectile;
            projectile.m_actor = actor;
            projectile.m_base_location = world_location;
            projectile.m_start_location = actor->fire_start_loc( );
            projectile.m_cached_impact = actor->cached_impact_result( );
            projectile.m_pawn_hit = actor->pawn_hit_result( );
            projectile.m_penetrating_pawn = projectile.m_pawn_hit.b_start_penetrating || projectile.m_pawn_hit.b_blocking_hit;
            projectile.m_distance = distance;

            if ( projectile.m_pawn_hit.time ) {
                projectile.m_impact_location = projectile.m_pawn_hit.impact_point;
                projectile.m_impact_distance = projectile.m_pawn_hit.distance;
            }
            else if ( projectile.m_cached_impact.time ) {
                projectile.m_impact_location = projectile.m_cached_impact.impact_point;
                projectile.m_impact_distance = projectile.m_cached_impact.distance;
            }

            if ( config::projectiles::show_tracers ) {
                g_projectile_traces[ actor ].add_trace(
                    projectile.m_base_location,
                    config::enemies::duration,
                    config::enemies::update
                );
            }

            return projectile;
        }

        lists::c_container update_container( engine::a_building_container* actor ) const {
            auto root_component = actor->root_component( );
            if ( !root_component ) return {};

            auto world_location = root_component->relative_location( );
            if ( !world_location.is_valid( ) ) return {};

            auto world_screen = engine::project( world_location );
            if ( !world_screen.in_screen( ) || !world_screen.is_valid( ) ) return {};

            auto distance = g_camera->m_location.distance_to( world_location );
            if ( distance >= config::containers::max_distance ) return {};

            lists::c_container container;
            container.m_actor = actor;
            container.m_spawn_source = actor->spawn_source_override( );
            container.m_mesh_component = actor->static_mesh_component( );
            container.m_root_component = root_component;
            container.m_world_location = world_location;
            container.m_distance = distance;

            if ( config::containers::container_chams ) {
                if ( container.m_mesh_component ) {
                    //auto current_context = container.m_mesh_component->custom_depth_stencil_value( );
                    //auto selected_context = esp::get_stencil_color( config::enemies::glow_color );
                    //if ( selected_context != current_context ) {
                    //    container.m_mesh_component->render_state_dirty( 1 );
                    //    container.m_mesh_component->custom_depth_stencil_value( 11 );
                    //    container.m_mesh_component->b_render_in_depth_pass( 1 );
                    //    container.m_mesh_component->replicates( 1 );
                    //}
                }
            }

            return container;
        }

        lists::c_pickup update_pickup( engine::a_fort_pickup* actor ) const {
            auto item_entry = actor->primary_pickup_item_entry( );
            auto item_definition = item_entry.item_definition;
            if ( !item_definition ) return {};

            auto root_component = actor->root_component( );
            if ( !root_component ) return {};

            auto world_location = root_component->relative_location( );
            if ( !world_location.is_valid( ) ) return {};

            auto world_screen = engine::project( world_location );
            if ( !world_screen.in_screen( ) || !world_screen.is_valid( ) ) return {};

            auto distance = g_camera->m_location.distance_to( world_location );
            if ( distance >= config::pickups::max_distance ) return {};

            lists::c_pickup pickup;
            pickup.m_actor = actor;
            pickup.m_item_definition = item_definition;
            pickup.m_rarity = item_definition->rarity( );
            pickup.m_pickup_name = item_definition->item_name( )->get( );
            pickup.m_root_component = root_component;
            pickup.m_world_location = world_location;
            pickup.m_distance = distance;
            return pickup;
        }

        lists::c_player update_player( engine::a_fort_player_pawn_athena* actor ) {
            if ( !actor ) return {};

            auto mesh = actor->mesh( );
            if ( !mesh ) return {};

            auto bone_array = mesh->bone_array( );
            if ( !bone_array.is_valid( ) ) return {};

            auto base_location = mesh->get_bone_location( bone_array, engine::e_bone::root );
            auto head_location = mesh->get_bone_location( bone_array, engine::e_bone::vb_head_fx );
          //  if ( !base_location.is_valid( ) || !head_location.is_valid( ) ) return {};

            auto distance = g_camera->m_location.distance_to( base_location );
          //  if ( distance >= config::enemies::max_distance ) return {};

            lists::c_player player;
            player.m_actor = actor;
            player.m_mesh = mesh;
            player.m_bone_array = bone_array;
            player.m_base_location = base_location;
            player.m_head_location = head_location;
            player.m_distance = distance;
            player.m_state = actor->player_state( );
            player.m_root_component = actor->root_component( );
            player.m_movement = actor->character_movement( );
            player.m_mesh_components = actor->skeletal_meshes( );
            player.m_pawn_materials = actor->pawn_materials_all( );
            player.m_weapon = actor->current_weapon( );
            player.m_vehicle = actor->current_vehicle( );
            player.m_is_downed = actor->b_is_dbno( );
            player.m_is_crouched = actor->b_is_crouched( );
            player.m_is_visible = mesh->is_visible( g_engine->m_world );
            player.m_is_sliding = actor->b_is_sliding( ) || actor->b_is_slope_sliding( );
            player.m_is_skydiving = actor->b_is_skydiving( ) || actor->b_is_parachute_open( );
            player.m_camera_location = mesh->get_bone_location( bone_array, engine::e_bone::camera );
            player.m_camera_rotation = mesh->get_bone_rotation( bone_array, engine::e_bone::camera );
            player.m_bounds = mesh->get_bounds( );

            if ( player.m_state ) {
                player.m_player_name = player.m_state->get_player_name( g_engine->m_in_lobby );
                player.m_is_teammate = player.m_state->is_teammate( g_local_pawn->m_state );
                player.m_player_id = player.m_state->player_id( );
                player.m_team_size = player.m_state->initial_squad_size( );
                player.m_player_kills = player.m_state->kill_score( );
                player.m_player_level = player.m_state->season_level_ui_display( );
                player.m_habanero_component = player.m_state->habanero_component( );
                player.m_platform_name = player.m_state->get_platform_name( );
                player.m_is_bot = player.m_state->b_is_a_bot( );
            }

            if ( player.m_weapon ) {
                player.m_weapon_ammo = player.m_weapon->ammo_count( );
                player.m_is_reloading = player.m_weapon->b_is_reloading_weapon( );
                player.m_is_targeting = player.m_weapon->b_is_targeting( );
                player.m_weapon_data = player.m_weapon->weapon_data( );
                player.m_weapon_name = player.m_weapon_data->item_name( )->get( );
                player.m_weapon_rarity = player.m_weapon_data->rarity( );
            }

            if ( player.m_vehicle ) {
                player.m_root_component = player.m_vehicle->root_component( );
            }

            if ( player.m_root_component ) {
                player.m_player_rotation = player.m_root_component->relative_rotation( );
                player.m_player_location = player.m_root_component->relative_location( );
                player.m_velocity = player.m_root_component->component_velocity( );
            }

            if ( player.m_movement ) {
                player.m_movement_mode = player.m_movement->movement_mode( );
            }

            if ( player.m_habanero_component ) {
                player.m_player_rank = player.m_habanero_component->rank_progress( );
            }

            if ( config::enemies::enabled ) {
                if ( config::enemies::stencil ) {
                    player.m_actor->clear_cham( );
                    g_driver->write<std::uint8_t>( ( std::uint64_t )player.m_actor + 0xB9, 11 );
                    g_driver->write<std::uint8_t>( ( std::uint64_t )player.m_actor + 0xB8, 11 );

                    auto mesh_components = player.m_mesh_components.get_itter( );
                    std::for_each( mesh_components.begin( ), mesh_components.end( ),
                        [ & ]( engine::u_skeletal_mesh_component* component ) {
                            if ( !component || g_chams->is_modified( component ) ) {
                                return;
                            }

                            //auto current_context = component->custom_depth_stencil_value( );
                            //auto selected_context = esp::get_stencil_color( config::enemies::glow_color );
                            //if ( selected_context != current_context ) {
                            //component->never_needs_render_update( 1 );

                            //component->custom_depth_stencil_value( 11 );
                            //

                            //component->render_state_dirty( 1 );
                            //component->b_render_in_depth_pass( 1 );


                            //component->replicates( 1 );

                            g_chams->add( component );
                        } );
                }

                if ( config::enemies::skeleton ) {
                    if ( player.m_skeleton_pairs.size( ) != engine::skeleton_pairs.size( ) ) {
                        player.m_skeleton_pairs.resize( engine::skeleton_pairs.size( ) );
                    }

                    std::transform( engine::skeleton_pairs.begin( ),
                        engine::skeleton_pairs.end( ),
                        player.m_skeleton_pairs.begin( ),
                        [ &mesh, &bone_array ]( const auto& src_pair ) {
                            return std::make_pair(
                                mesh->get_bone_location( bone_array, src_pair.first ),
                                mesh->get_bone_location( bone_array, src_pair.second )
                            );
                        } );
                }

                if ( config::enemies::movement_tracers ) {
                    g_player_traces[ actor ].add_trace(
                        player.m_base_location,
                        config::enemies::duration,
                        config::enemies::update
                    );
                }
            }

            return player;
        }
    };
}