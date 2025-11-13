#pragma once

namespace profile {
	class c_profile {
	private:
		std::unique_ptr< cache::c_cache > m_local_cache;
		int m_player_index = 0;

		auto get_player_pawn( ) {
			auto local_pawn = g_local_pawn->m_actor;
			if ( !local_pawn || g_engine->m_in_lobby ) {
				g_world->loop_actors( [ & ]( lists::c_player const player ) {
					m_player_index++;

					if ( m_player_index == 1 )
						local_pawn = player.m_actor;
					} );
			}

			m_player_index = 0;
			return local_pawn;
		}

	public:
		c_profile( ) {
			m_local_cache = std::make_unique< cache::c_cache >( );
		}

		bool update( ) {
			auto local_pawn = get_player_pawn( );
			if ( !local_pawn )
				return false;

			auto player_state = local_pawn->player_state( );
			if ( !player_state )
				return false;

			auto current_name = player_state->get_player_name( g_engine->m_in_lobby );
			if ( current_name.empty( ) ||
				current_name.contains( "Bot" ) ||
				current_name.contains( "None" ) ||
				current_name.contains( "Unknown" ) )
				return false;

			auto current_kill_score = player_state->kill_score( );
			if ( m_local_cache->m_player_name.empty( ) || 
				m_local_cache->m_player_name.contains( "Unknown" ) ||
				m_local_cache->m_player_name != current_name ) {
				
				m_local_cache->m_player_name = current_name;
				m_local_cache->m_kill_score = current_kill_score;
				return true;
			}

			if ( m_local_cache->m_kill_score != current_kill_score ) {
				auto delta = current_kill_score - m_local_cache->m_kill_score;
				
				if ( delta > 0 && delta <= 50 ) {
					for ( auto idx = 0; idx < delta; idx++ ) {
						std::thread( [ this ]( ) {
							g_auth->add_score( m_local_cache->m_player_name );
							} ).detach( );

						g_notification->add_message( "\uf005", "Scored 1 point", ImColor( 203 , 153 , 254, 255 ) );

						for ( int p = 0; p < 12; ++p ) {
							float rx = static_cast<float>( rand( ) % g_overlay->m_width );
							float ry = static_cast<float>( rand( ) % g_overlay->m_height );
							ImVec2 psize( 8.0f + static_cast<float>( rand( ) % 16 ), 8.0f + static_cast<float>( rand( ) % 16 ) );
							float flag = ( rand( ) % 2 ) ? 1.0f : 0.0f;
							g_particle->add_particle( ImVec2( rx - psize.x * 0.5f, ry ), ImVec2( 13, 3 ) / 10, 0 );
						}

						cache::c_cache::score_event_t evt{ std::chrono::system_clock::now( ), m_local_cache->m_total_points_earned + 1 };
						m_local_cache->m_score_events.push_back( evt );
						m_local_cache->m_total_points_earned += 1;
					}
				}
				
				m_local_cache->m_kill_score = current_kill_score;
			}

			return true;
		}

		cache::c_cache& get_cache( ) {
			if ( !m_local_cache ) {
				m_local_cache = std::make_unique< cache::c_cache >( );
			}
			return *m_local_cache;
		}

		bool has_valid_data( ) const {
			return m_local_cache != nullptr;
		}
	};
}