#pragma once

namespace selection {
	class c_selection {
	public:
		void process_player( lists::c_player player ) {
			auto screen_location = player.m_head_location;
			auto screen_position = engine::project( screen_location );

			auto dx = screen_position.x - g_overlay->m_width_center;
			auto dy = screen_position.y - g_overlay->m_height_center;
			if ( !screen_position.in_circle( g_target.m_aimbot_fov ) ) return;

			auto crosshair_distance = sqrt( dx * dx + dy * dy );

			if ( g_target.m_target_lock_enabled && g_target.m_has_locked_target ) {
				if ( player.m_player_id == g_target.m_locked_player_id ) {
					g_target.m_player = player;
					g_target.m_type = target_type::player;
					return;
				}
				else {
					return;
				}
			}

			if ( g_weapon->m_smart_targeting ) {
				auto final_score = scoring::get_improved_score( player, crosshair_distance );
				if ( final_score > m_best_target_score ) {
					g_target.m_player = player;
					g_target.m_type = target_type::player;
					m_best_target_score = final_score;
					
					if ( g_target.m_target_lock_enabled ) {
						g_target.m_locked_player_id = player.m_player_id;
						g_target.m_has_locked_target = true;
					}
				}
			}
			else {
				if ( crosshair_distance < m_closest_distance ) {
					g_target.m_player = player;
					g_target.m_type = target_type::player;
					m_closest_distance = crosshair_distance;
					
					if ( g_target.m_target_lock_enabled ) {
						g_target.m_locked_player_id = player.m_player_id;
						g_target.m_has_locked_target = true;
					}
				}
			}
		}

		void process_weakspot( lists::c_weakspot weakspot ) {
			auto screen_position = weakspot.m_world_screen;
			auto dx = screen_position.x - g_overlay->m_width_center;
			auto dy = screen_position.y - g_overlay->m_height_center;
			if ( !screen_position.in_circle( g_target.m_aimbot_fov ) ) return;

			if ( g_target.m_target_lock_enabled && g_target.m_has_locked_target ) {
				return;
			}

			auto crosshair_distance = sqrt( dx * dx + dy * dy );
			if ( g_weapon->m_smart_targeting ) {
				auto final_score = scoring::get_improved_weakspot_score( weakspot, crosshair_distance );
				if ( final_score > m_best_target_score ) {
					g_target.m_weakspot = weakspot;
					g_target.m_type = target_type::weakspot;
					m_best_target_score = final_score;
				}
			}
			else {
				if ( crosshair_distance < m_closest_distance ) {
					g_target.m_weakspot = weakspot;
					g_target.m_type = target_type::weakspot;
					m_closest_distance = crosshair_distance;
				}
			}
		}

		bool is_valid( lists::c_player player ) const {
			return player.m_actor && ( !g_weapon->m_ignore_downed || !player.m_is_downed ) &&
				( !g_weapon->m_only_visible || player.m_is_visible ) &&
				( !g_weapon->m_ignore_bots || !player.m_is_bot ) &&
				!player.m_is_teammate &&
				player.m_distance < g_weapon->m_max_distance;
		}

		bool is_valid( lists::c_weakspot loot ) const {
			return loot.m_actor && !loot.m_actor->b_hit( ) && loot.m_actor->b_active( ) &&
				loot.m_distance < config::weakspots::max_distance;
		}

		void reset( ) {
			m_closest_distance = DBL_MAX;
			m_best_target_score = -DBL_MAX;
		}
		
		void reset_target_lock( ) {
			g_target.m_has_locked_target = false;
			g_target.m_locked_player_id = 0;
		}

		void update_locked_target( ) {
			// If we have a locked target, find and update its actor data
			if ( g_target.m_target_lock_enabled && g_target.m_has_locked_target && g_target.m_locked_player_id != 0 ) {
				bool target_found = false;
				
				// Search through all players to find the one with matching ID
				g_world->loop_actors( [&]( const lists::c_player& player ) {
					if ( player.m_player_id == g_target.m_locked_player_id ) {
						// Update the target's actor data
						g_target.m_player = player;
						g_target.m_type = target_type::player;
						target_found = true;
						return;
					}
				} );
				
				// If the locked target is no longer available, reset the lock
				if ( !target_found ) {
					this->reset_target_lock( );
				}
			}
		}

	private:
		double m_closest_distance = DBL_MAX;
		double m_best_target_score = -DBL_MAX;
	};
}