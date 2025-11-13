#pragma once

namespace weapon {
	class c_weapon {
	public:
		void apply_config( ) {
			if ( config::aimbot::override_general[ config::aimbot::weapon_configs ] ) {
				if ( !g_local_pawn->m_actor ) {
					this->apply_config( 0 );
					return;
				}

				auto current_weapon = g_local_pawn->m_actor->current_weapon( );
				if ( !current_weapon ) {
					this->apply_config( 0 );
					return;
				}

				auto weapon_animation = current_weapon->weapon_core_animation( );
				switch ( weapon_animation ) {
				case engine::e_weapon_core_animation::rifle: {
					this->apply_config( 1 );
				} break;
				case engine::e_weapon_core_animation::assault_rifle: {
					this->apply_config( 1 );
				} break;
				case engine::e_weapon_core_animation::shotgun: {
					this->apply_config( 2 );
				} break;
				case engine::e_weapon_core_animation::tactical_shotgun: {
					this->apply_config( 2 );
				} break;
				case engine::e_weapon_core_animation::machine_pistol: {
					this->apply_config( 3 );
				} break;
				case engine::e_weapon_core_animation::smg_p90: {
					this->apply_config( 3 );
				} break;
				case engine::e_weapon_core_animation::pistol: {
					this->apply_config( 4 );
				} break;
				case engine::e_weapon_core_animation::sniper_rifle: {
					this->apply_config( 5 );
				} break;
				default: {
					this->apply_config( 0 );
				} break;
				}
			}
			else
				this->apply_config( 0 );
		}

		void apply_close_config( ) {
			if ( config::aimbot::close_aim ) {
				if ( !g_local_pawn->m_actor ) {
					return;
				}

				auto current_weapon = g_local_pawn->m_actor->current_weapon( );
				if ( !current_weapon ) {
					return;
				}

				auto weapon_animation = current_weapon->weapon_core_animation( );
				if ( config::aimbot::shotgun_only && weapon_animation != engine::e_weapon_core_animation::shotgun )
					return;

				auto distance = g_target.m_player.m_distance;
				if ( distance > config::aimbot::max_distance[ 6 ] )
					return;

				this->apply_config( 6 );
			}
		}

		bool m_silent_aim = false;
		bool m_linear_aim = false;
		bool m_deadzone_dot = false;
		bool m_prediction_x = false;
		bool m_no_target_switch = false;
		bool m_inverted_vertical_axis = false;
		bool m_fov_circle = false;
		bool m_target_line = false;
		bool m_target_dot = false;
		bool m_aim_curve = false;
		bool m_target_lock = false;
		float m_fov_radius = 0.f;
		bool m_deadzone = false;
		bool m_aim_prediction[ 5 ];
		int m_smoothing = 0;
		bool m_aim_bone[ 5 ]{ };
		int m_aim_key = 0;
		float m_restriction = 0.f;
		bool m_smart_targeting = false;
		bool m_ignore_downed = false;
		bool m_ignore_bots = false;
		bool m_only_visible = false;
		bool m_aim_acceleration = false;
		float m_max_velocity = 0.f;
		float m_acceleration = 0.f;
		float m_deceleration = 0.f;
		float m_velocity_threshold = 0.f;
		int m_max_distance = 0;
		int m_hit_chance = 0;

		bool m_triggerbot_deadzone = false;
		float m_triggerbot_deadzone_radius = 0.f;
		bool m_triggerbot_deadzone_circular = true;
		float m_triggerbot_deadzone_width = 0.f;
		float m_triggerbot_deadzone_height = 0.f;

	private:
		void apply_config( int weapon_config ) {
			m_triggerbot_deadzone = config::triggerbot::deadzone;
			m_triggerbot_deadzone_radius = config::triggerbot::deadzone_radius;
			m_triggerbot_deadzone_circular = config::triggerbot::deadzone_circular;
			m_triggerbot_deadzone_width = config::triggerbot::deadzone_width;
			m_triggerbot_deadzone_height = config::triggerbot::deadzone_height;

			if ( weapon_config > 5 ) {
				m_silent_aim = config::aimbot::silent_aim[ weapon_config ];
				m_linear_aim = config::aimbot::linear_aim[ weapon_config ];
				m_no_target_switch = config::aimbot::no_target_switch[ weapon_config ];
				m_inverted_vertical_axis = config::aimbot::inverted_vertical_axis[ weapon_config ];
				m_fov_circle = config::aimbot::fov_circle[ weapon_config ];
				m_fov_radius = config::aimbot::fov_radius[ weapon_config ];
				m_deadzone = config::aimbot::deadzone[ weapon_config ];
				m_smoothing = config::aimbot::smoothing[ weapon_config ];
				memcpy( m_aim_bone, config::aimbot::aim_bone[ weapon_config ], sizeof( m_aim_bone ) );
				m_aim_key = config::aimbot::aim_key[ weapon_config ];
				m_restriction = config::aimbot::restriction[ weapon_config ];
				m_max_distance = config::aimbot::max_distance[ weapon_config ];
			}
			else {
				m_prediction_x = config::aimbot::prediction_x[ weapon_config ];
				m_silent_aim = config::aimbot::silent_aim[ weapon_config ];
				m_hit_chance = config::aimbot::hit_chance[ weapon_config ];
				m_deadzone_dot = config::aimbot::deadzone_dot[ weapon_config ];
				m_linear_aim = config::aimbot::linear_aim[ weapon_config ];
				m_no_target_switch = config::aimbot::no_target_switch[ weapon_config ];
				m_inverted_vertical_axis = config::aimbot::inverted_vertical_axis[ weapon_config ];
				m_fov_circle = config::aimbot::fov_circle[ weapon_config ];
				m_target_line = config::aimbot::target_line[ weapon_config ];
				m_target_dot = config::aimbot::target_dot[ weapon_config ];
				m_aim_curve = config::aimbot::aim_curve[ weapon_config ];
				m_target_lock = config::aimbot::no_target_switch[ weapon_config ];
				m_fov_radius = config::aimbot::fov_radius[ weapon_config ];
				m_deadzone = config::aimbot::deadzone[ weapon_config ];
				memcpy( m_aim_prediction, config::aimbot::aim_prediction[ weapon_config ], sizeof( m_aim_prediction ) );
				m_smoothing = config::aimbot::smoothing[ weapon_config ];
				memcpy( m_aim_bone, config::aimbot::aim_bone[ weapon_config ], sizeof( m_aim_bone ) );
				m_aim_key = config::aimbot::aim_key[ weapon_config ];
				m_restriction = config::aimbot::restriction[ weapon_config ];
				m_smart_targeting = config::aimbot::smart_targeting[ weapon_config ];
				m_ignore_downed = config::aimbot::ignore_downed[ weapon_config ];
				m_ignore_bots = config::aimbot::ignore_bots[ weapon_config ];
				m_only_visible = config::aimbot::only_visible[ weapon_config ];
				m_aim_acceleration = config::aimbot::aim_acceleration[ weapon_config ];
				m_max_velocity = config::aimbot::max_velocity[ weapon_config ];
				m_acceleration = config::aimbot::acceleration[ weapon_config ];
				m_deceleration = config::aimbot::deceleration[ weapon_config ];
				m_velocity_threshold = config::aimbot::velocity_threshold[ weapon_config ];
				m_max_distance = config::aimbot::max_distance[ weapon_config ];
			}

			g_target.m_aimbot_fov = ( m_fov_radius * g_overlay->m_width_center / g_camera->m_fov ) / 2.0;
			
			// Apply target lock configuration
			g_target.m_target_lock_enabled = m_target_lock;
		}
	};
}