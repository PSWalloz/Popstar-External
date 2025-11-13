#pragma once

namespace triggerbot {
	class c_triggerbot {
	public:
		void tick( bool is_visible ) {
			if ( g_target.m_aim_distance > config::triggerbot::max_distance || !is_visible )
				return;

			//auto weapon_animation = g_engine->m_weapon.m_animation;
			//if ( !config::triggerbot::weapon_config[ 1 ] && weapon_animation == engine::e_weapon_core_animation::rifle ||
			//	weapon_animation == engine::e_weapon_core_animation::assault_rifle ) return;
			//if ( !config::triggerbot::weapon_config[ 2 ] && weapon_animation == engine::e_weapon_core_animation::shotgun ||
			//	weapon_animation == engine::e_weapon_core_animation::tactical_shotgun ) return;
			//if ( !config::triggerbot::weapon_config[ 3 ] && weapon_animation == engine::e_weapon_core_animation::smg_p90 ) return;
			//if ( !config::triggerbot::weapon_config[ 4 ] && weapon_animation == engine::e_weapon_core_animation::pistol ) return;
			//if ( !config::triggerbot::weapon_config[ 5 ] && weapon_animation == engine::e_weapon_core_animation::sniper_rifle ) return;

			auto is_pressing = config::triggerbot::fire_key ? GetAsyncKeyState( config::triggerbot::key_bind ) & 0x8000 : true;
			if ( !is_pressing )
				return;

			auto current_time = std::chrono::steady_clock::now( );
			auto time_since_last_trigger = std::chrono::duration_cast< std::chrono::milliseconds >( current_time - m_last_trigger_time ).count( );
			if ( time_since_last_trigger >= config::triggerbot::delay )
				m_trigger_ready = true;

			if ( m_trigger_ready ) {
				bool should_fire = ( is_in_triggerbot_deadzone( g_target.m_aim_screen ) );
				if ( should_fire ) {
					keybd_event( VK_LBUTTON, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );
					keybd_event( VK_LBUTTON, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );

					m_last_trigger_time = current_time;
					m_trigger_ready = false;
				}
			}
		}

		void reset( ) {
			m_last_trigger_time = std::chrono::steady_clock::now( );
			m_trigger_ready = true;
		}

	private:
		std::chrono::steady_clock::time_point m_last_trigger_time = std::chrono::steady_clock::now( );
		bool m_trigger_ready = true;

		bool is_in_deadzone( engine::f_vector2d end_position, float restriction ) {
			auto center_x = g_overlay->m_width_center;
			auto center_y = g_overlay->m_height_center;

			auto dx = end_position.x - center_x;
			auto dy = end_position.y - center_y;
			auto distance = sqrt( dx * dx + dy * dy );

			return distance < restriction;
		}

		bool is_in_triggerbot_deadzone( engine::f_vector2d target_position ) {
			if ( !g_weapon->m_triggerbot_deadzone ) {
				return true;
			}

			auto center_x = g_overlay->m_width_center;
			auto center_y = g_overlay->m_height_center;

			auto dx = target_position.x - center_x;
			auto dy = target_position.y - center_y;

			if ( g_weapon->m_triggerbot_deadzone_circular ) {
				auto distance = sqrt( dx * dx + dy * dy );
				return distance <= g_weapon->m_triggerbot_deadzone_radius;
			}
			else {
				auto half_width = g_weapon->m_triggerbot_deadzone_width / 2.0f;
				auto half_height = g_weapon->m_triggerbot_deadzone_height / 2.0f;

				return ( abs( dx ) <= half_width && abs( dy ) <= half_height );
			}
		}
	};
}