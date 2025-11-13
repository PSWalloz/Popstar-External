#pragma once

namespace prediction {
	void apply_delta( engine::f_vector& aim_location ) {
		aim_location.x += g_target.m_player.m_velocity.x;
		aim_location.y += g_target.m_player.m_velocity.y;
		aim_location.z += g_target.m_player.m_velocity.z;
	}

	void apply_bullet( engine::f_vector& aim_location ) {
		//auto projectile_speed = g_local_pawn->m_projectile_speed;
		//auto projectile_gravity = g_local_pawn->m_projectile_gravity;
		//if ( !g_local_pawn->m_actor->current_weapon( ) || !projectile_speed || !projectile_gravity ) {
		//	return;
		//}

		//auto target_velocity = g_target.m_player.m_velocity;

		//auto distance = g_camera->m_location.distance( aim_location );
		//auto time_to_target = distance / projectile_speed;

		//auto predicted_position = aim_location + ( target_velocity * time_to_target );

		//auto new_distance = g_camera->m_location.distance( predicted_position );
		//time_to_target = new_distance / projectile_speed;

		//auto gravity = fabs( projectile_gravity * 980.0f );
		//auto gravity_drop = 0.5f * gravity * time_to_target * time_to_target;

		//aim_location = predicted_position;
		/*aim_location.z += gravity_drop;*/
	}
}