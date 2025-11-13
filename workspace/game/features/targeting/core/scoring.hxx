#pragma once

namespace scoring {
	float get_distance_priority( float distance ) {
		if ( distance < 20.0f ) {
			return ( 5.0f * 20.0f ) / distance;
		}
		else if ( distance < 1.0f ) {
			float t = ( distance - 20.0f ) / ( 1.0f - 20.0f );
			return 5.0f + t * ( 1.0f - 5.0f );
		}
		else if ( distance < 200.0f ) {
			float t = ( distance - 1.0f ) / ( 200.0f - 1.0f );
			return 1.0f + t * ( 0.1f - 1.0f );
		}
		return 0.1f;
	}

	float get_crosshair_priority( float crosshair_distance ) {
		auto normalized_distance = crosshair_distance / ( g_overlay->m_width_center * 0.3f );
		auto exponential_factor = exp( -normalized_distance * 4.0f );

		auto close_bonus = 0.0f;
		if ( crosshair_distance < 75.0f ) {
			close_bonus = ( 75.0f - crosshair_distance ) / 75.0f * 8.0f;
		}

		return ( 20.0f * exponential_factor ) + close_bonus;
	}

	float get_visibility_bonus( const lists::c_player& player ) {
		auto bonus = 0.0f;

		if ( player.m_is_visible ) {
			bonus += 20.0f;
		}

		return bonus;
	}

	float get_weakspot_score( lists::c_weakspot weakspot, float crosshair_distance ) {
		auto distance = weakspot.m_distance;
		auto distance_score = get_distance_priority( distance );
		auto crosshair_score = crosshair_distance * 1.0f;

		auto final_score = crosshair_score + ( distance_score * -0.4f );
		return final_score;
	}

	float get_improved_weakspot_score( lists::c_weakspot weakspot, float crosshair_distance ) {
		float max_crosshair_dist = g_target.m_aimbot_fov;
		float normalized_crosshair = crosshair_distance / max( max_crosshair_dist, 1.0f );

		float max_weakspot_dist = static_cast< float >( config::weakspots::max_distance );
		float normalized_distance = weakspot.m_distance / max( max_weakspot_dist, 1.0f );

		float crosshair_weight = 0.90f;
		float distance_weight = 0.10f;

		float enhanced_crosshair_score = normalized_crosshair + ( normalized_distance * 0.5f );
		float crosshair_score = enhanced_crosshair_score * crosshair_weight;
		float distance_score = normalized_distance * distance_weight;

		return 1.0f - ( crosshair_score + distance_score );
	}

	float get_score( const lists::c_player& player, float crosshair_distance ) {
		auto player_id = player.m_player_id;

		auto distance_score = get_distance_priority( player.m_distance );
		auto crosshair_score = get_crosshair_priority( crosshair_distance );
		auto visibility_bonus = get_visibility_bonus( player );

		auto final_score = crosshair_score * 0.75f +
			distance_score * 0.15f +
			visibility_bonus * 0.03f;
		return final_score;
	}

	float get_improved_score( lists::c_player player, float crosshair_distance ) {
		float max_crosshair_dist = g_target.m_aimbot_fov;
		float normalized_crosshair = crosshair_distance / max( max_crosshair_dist, 1.0f );

		float max_player_dist = static_cast< float >( g_weapon->m_max_distance );
		float normalized_distance = player.m_distance / max( max_player_dist, 1.0f );

		float crosshair_weight = 0.80f;
		float distance_weight = 0.15f;
		float visibility_weight = 0.05f;

		float enhanced_crosshair_score = normalized_crosshair + ( normalized_distance * 0.3f );
		float crosshair_score = enhanced_crosshair_score * crosshair_weight;
		float distance_score = normalized_distance * distance_weight;

		float visibility_score = player.m_is_visible ? 0.0f : visibility_weight;
		
		return 1.0f - ( crosshair_score + distance_score + visibility_score );
	}

	std::vector<std::pair<float, size_t>> get_player_scores( const std::vector<lists::c_player>& players, const std::vector<float>& crosshair_distances ) {
		std::vector<std::pair<float, size_t>> scores;
		scores.reserve( players.size( ) );

		for ( size_t i = 0; i < players.size( ) && i < crosshair_distances.size( ); ++i ) {
			auto score = get_improved_score( players[ i ], crosshair_distances[ i ] );
			scores.emplace_back( score, i );
		}

		std::sort( scores.begin( ), scores.end( ), std::greater<std::pair<float, size_t>>( ) );
		return scores;
	}

	lists::c_player get_best_target_from_list( const std::vector<lists::c_player>& players ) {
		if ( players.empty( ) ) {
			return lists::c_player{};
		}

		std::vector<float> crosshair_distances;
		crosshair_distances.reserve( players.size( ) );

		for ( const auto& player : players ) {
			auto head_location = player.m_head_location;
			auto screen_position = engine::project( head_location );
			auto dx = screen_position.x - g_overlay->m_width_center;
			auto dy = screen_position.y - g_overlay->m_height_center;
			auto distance = sqrt( dx * dx + dy * dy );
			crosshair_distances.push_back( distance );
		}

		auto sorted_scores = get_player_scores( players, crosshair_distances );
		if ( !sorted_scores.empty( ) ) {
			return players[ sorted_scores[ 0 ].second ];
		}

		return lists::c_player{};
	}
}