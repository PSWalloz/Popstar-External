#pragma once 

namespace esp {
    struct colors_t {
        ImColor m_main_color;
        ImColor m_outline_color;
        ImColor m_text_color;
    };

    colors_t get_theme_color( lists::c_player player ) {
        colors_t theme_color;

        if ( player.m_is_downed ) {
            theme_color.m_main_color = ImColor( 75, 87, 219, 255 );
            theme_color.m_outline_color = ImColor( 0, 0, 0, 255 );
            theme_color.m_text_color = ImColor( 75, 87, 219, 255 );
        }
        else if ( player.m_is_teammate && !g_engine->m_in_lobby ) {
            theme_color.m_main_color = ImColor( 255, 126, 38, 255 );
            theme_color.m_outline_color = ImColor( 0, 0, 0, 255 );
            theme_color.m_text_color = ImColor( 255, 126, 38, 255 );
        }
        else if ( player.m_is_visible ) {
            theme_color.m_main_color = ImColor( 203, 153, 254, 255 ); //  142, 130, 254
            theme_color.m_outline_color = ImColor( 0, 0, 0, 255 );
            theme_color.m_text_color = ImColor( 203, 153, 254, 255 );
        }
        else {
            theme_color.m_main_color = ImColor( 6, 154, 243, 255 );
            theme_color.m_outline_color = ImColor( 0, 0, 0, 255 );
            theme_color.m_text_color = ImColor( 6, 154, 243, 255 );
        }

        return theme_color;
    }

    ImColor get_team_color( int team_size ) {
        if ( team_size == 1 ) {
            return ImColor( 110, 190, 201, 255 );
        }
        else if ( team_size == 2 ) {
            return ImColor( 45, 104, 196, 255 );
        }
        else if ( team_size == 3 ) {
            return ImColor( 255, 117, 24, 255 );
        }
        else if ( team_size == 4 ) {
            return ImColor( 224, 176, 255, 255 );
        }

        return ImColor( );
    }

    ImColor get_platform_color( std::string platform ) {
        if ( strstr( platform.c_str( ), "WIN" ) ) {
            return ImColor( 125, 165, 170, 255 );
        }
        else if ( strstr( platform.c_str( ), ( "XBL" ) ) || strstr( platform.c_str( ), ( "XSX" ) ) ) {
            return ImColor( 138, 154, 91, 255 );
        }
        else if ( strstr( platform.c_str( ), ( "PSN" ) ) || strstr( platform.c_str( ), ( "PS5" ) ) ) {
            return ImColor( 50, 74, 178, 255 );
        }
        else if ( strstr( platform.c_str( ), ( "SWT" ) ) ) {
            return ImColor( 127, 15, 5, 255 );
        }
        else if ( strstr( platform.c_str( ), ( "AND" ) ) ) {
            return ImColor( 115, 135, 83, 255 );
        }
        else if ( strstr( platform.c_str( ), ( "IOS" ) ) ) {
            return ImColor( 144, 154, 251, 255 );
        }

        return ImColor( 125, 165, 170, 255 );
    }

    ImColor get_tier_color( engine::e_fort_rarity tier ) {
        switch ( tier ) {
        case engine::e_fort_rarity::common:
            return ImColor( 102, 102, 102, 255 );
        case engine::e_fort_rarity::uncommon:
            return ImColor( 138, 154, 91, 255 );
        case engine::e_fort_rarity::rare:
            return ImColor( 93, 137, 186, 255 );
        case engine::e_fort_rarity::epic:
            return ImColor( 141, 78, 133, 255 );
        case engine::e_fort_rarity::legendary:
            return ImColor( 211, 175, 55, 255 );
        case engine::e_fort_rarity::mythic:
            return ImColor( 255, 207, 64, 255 );
        default:
            return ImColor( 102, 102, 102, 255 );
        }
    }

    int get_stencil_color( int stencil ) {
        switch ( stencil ) {
        case 0:
            return 11;
        case 1:
            return 8;
        case 2:
            return 12;
        default:
            return 0;
        }
    }

    ImColor get_rank_color( int32_t rank ) {
        if ( rank >= 0 && rank <= 2 )
            return ImColor( 0xCD, 0x7F, 0x32, 255 );
        else if ( rank >= 3 && rank <= 5 )
            return ImColor( 93, 136, 187, 255 );
        else if ( rank >= 6 && rank <= 8 )
            return ImColor( 255, 170, 51, 255 );
        else if ( rank >= 9 && rank <= 11 )
            return ImColor( 93, 137, 186, 255 );
        else if ( rank >= 12 && rank <= 14 )
            return ImColor( 48, 92, 222, 255 );
        else if ( rank == 15 )
            return ImColor( 222, 181, 181, 255 );
        else if ( rank == 16 )
            return ImColor( 217, 144, 88, 255 );
        else if ( rank == 17 )
            return ImColor( 116, 66, 227, 255 );
        return ImColor( 0x80, 0x80, 0x80, 255 );
    }


    std::string get_building_name( engine::e_fort_building_type building_type ) {
        switch ( building_type ) {
        case engine::e_fort_building_type::wall: {
            return "Wall";
        } break;
        case engine::e_fort_building_type::floor: {
            return "Floor";
        } break;
        case engine::e_fort_building_type::corner: {
            return "Corner";
        } break;
        case engine::e_fort_building_type::deco: {
            return "Deco";
        } break;
        case engine::e_fort_building_type::prop: {
            return "Prop";
        } break;
        case engine::e_fort_building_type::stairs: {
            return "Stairs";
        } break;
        case engine::e_fort_building_type::roof: {
            return "Roof";
        } break;
        case engine::e_fort_building_type::pillar: {
            return "Pillar";
        } break;
        case engine::e_fort_building_type::container: {
            return "Container";
        } break;
        case engine::e_fort_building_type::trap: {
            return "Trap";
        } break;
        default: {
            return "Building";
        } break;
        }
    }

    std::string get_platform_name( std::string platform ) {
        if ( strstr( platform.c_str( ), "WIN" ) ) {
            return "Windows";
        }
        else if ( strstr( platform.c_str( ), ( "XBL" ) ) || strstr( platform.c_str( ), ( "XSX" ) ) ) {
            return "Xbox";
        }
        else if ( strstr( platform.c_str( ), ( "PSN" ) ) || strstr( platform.c_str( ), ( "PS5" ) ) ) {
            return "Playstation";
        }
        else if ( strstr( platform.c_str( ), ( "SWT" ) ) ) {
            return "Nintendo";
        }
        else if ( strstr( platform.c_str( ), ( "AND" ) ) || strstr( platform.c_str( ), ( "IOS" ) ) ) {
            return "Mobile";
        }

        return platform;
    }

    std::string get_rank_name( int32_t rank ) {
        if ( rank == 0 )
            return ( "Bronze I" );
        else if ( rank == 1 )
            return ( "Bronze II" );
        else if ( rank == 2 )
            return ( "Bronze III" );
        else if ( rank == 3 )
            return ( "Silver I" );
        else if ( rank == 4 )
            return ( "Silver II" );
        else if ( rank == 5 )
            return ( "Silver III" );
        else if ( rank == 6 )
            return ( "Gold I" );
        else if ( rank == 7 )
            return ( "Gold II" );
        else if ( rank == 8 )
            return ( "Gold III" );
        else if ( rank == 9 )
            return ( "Platinum I" );
        else if ( rank == 10 )
            return ( "Platinum II" );
        else if ( rank == 11 )
            return ( "Platinum III" );
        else if ( rank == 12 )
            return ( "Diamond I" );
        else if ( rank == 13 )
            return ( "Diamond II" );
        else if ( rank == 14 )
            return ( "Diamond III" );
        else if ( rank == 15 )
            return ( "Elite" );
        else if ( rank == 16 )
            return ( "Champion" );
        else if ( rank == 17 )
            return ( "Unreal" );
        return ( "" );
    }

    std::string get_team_text( int team_size ) { 
        if ( team_size == 1 ) {
            return std::format( "Solo" );
        }
        else if ( team_size == 2 ) {
            return std::format( "Duo" );
        }
        else if ( team_size == 3 ) {
            return std::format( "Trio" );
        }
        else if ( team_size == 4 ) {
            return std::format( "Squad" );
        }

        return "";
    }
}