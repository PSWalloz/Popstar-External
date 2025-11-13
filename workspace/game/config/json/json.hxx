#pragma once

namespace config {
	namespace json {
		using json = nlohmann::json;

		json to_json( ) {
			json j;
			j[ "aimbot" ] = {
				{ "enabled", aimbot::enabled },
				{ "weapon_configs", aimbot::weapon_configs },
				{ "close_aim", aimbot::close_aim },
				{ "shotgun_only", aimbot::shotgun_only },
				{ "silent_aim", { aimbot::silent_aim[ 0 ], aimbot::silent_aim[ 1 ], aimbot::silent_aim[ 2 ], aimbot::silent_aim[ 3 ], aimbot::silent_aim[ 4 ], aimbot::silent_aim[ 5 ] } },
				{ "hit_chance", { aimbot::hit_chance[ 0 ], aimbot::hit_chance[ 1 ], aimbot::hit_chance[ 2 ], aimbot::hit_chance[ 3 ], aimbot::hit_chance[ 4 ], aimbot::hit_chance[ 5 ] } },
				{ "linear_aim", { aimbot::linear_aim[ 0 ], aimbot::linear_aim[ 1 ], aimbot::linear_aim[ 2 ], aimbot::linear_aim[ 3 ], aimbot::linear_aim[ 4 ], aimbot::linear_aim[ 5 ] } },
				{ "no_target_switch", { aimbot::no_target_switch[ 0 ], aimbot::no_target_switch[ 1 ], aimbot::no_target_switch[ 2 ], aimbot::no_target_switch[ 3 ], aimbot::no_target_switch[ 4 ], aimbot::no_target_switch[ 5 ] } },
				{ "inverted_vertical_axis", { aimbot::inverted_vertical_axis[ 0 ], aimbot::inverted_vertical_axis[ 1 ], aimbot::inverted_vertical_axis[ 2 ], aimbot::inverted_vertical_axis[ 3 ], aimbot::inverted_vertical_axis[ 4 ], aimbot::inverted_vertical_axis[ 5 ] } },
				{ "fov_circle", { aimbot::fov_circle[ 0 ], aimbot::fov_circle[ 1 ], aimbot::fov_circle[ 2 ], aimbot::fov_circle[ 3 ], aimbot::fov_circle[ 4 ], aimbot::fov_circle[ 5 ] } },
				{ "target_line", { aimbot::target_line[ 0 ], aimbot::target_line[ 1 ], aimbot::target_line[ 2 ], aimbot::target_line[ 3 ], aimbot::target_line[ 4 ] } },
				{ "target_dot", { aimbot::target_dot[ 0 ], aimbot::target_dot[ 1 ], aimbot::target_dot[ 2 ], aimbot::target_dot[ 3 ], aimbot::target_dot[ 4 ] } },
				{ "deadzone_dot", { aimbot::deadzone_dot[ 0 ], aimbot::deadzone_dot[ 1 ], aimbot::deadzone_dot[ 2 ], aimbot::deadzone_dot[ 3 ], aimbot::deadzone_dot[ 4 ] } },
				{ "prediction_x", { aimbot::prediction_x[ 0 ], aimbot::prediction_x[ 1 ], aimbot::prediction_x[ 2 ], aimbot::prediction_x[ 3 ], aimbot::prediction_x[ 4 ] } },
				{ "aim_curve", { aimbot::aim_curve[ 0 ], aimbot::aim_curve[ 1 ], aimbot::aim_curve[ 2 ], aimbot::aim_curve[ 3 ], aimbot::aim_curve[ 4 ] } },
				{ "fov_radius", { aimbot::fov_radius[ 0 ], aimbot::fov_radius[ 1 ], aimbot::fov_radius[ 2 ], aimbot::fov_radius[ 3 ], aimbot::fov_radius[ 4 ], aimbot::fov_radius[ 5 ] } },
				{ "deadzone", { aimbot::deadzone[ 0 ], aimbot::deadzone[ 1 ], aimbot::deadzone[ 2 ], aimbot::deadzone[ 3 ], aimbot::deadzone[ 4 ], aimbot::deadzone[ 5 ] } },
				{ "aim_prediction", {
					{ aimbot::aim_prediction[ 0 ][ 0 ], aimbot::aim_prediction[ 0 ][ 1 ], aimbot::aim_prediction[ 0 ][ 2 ], aimbot::aim_prediction[ 0 ][ 3 ], aimbot::aim_prediction[ 0 ][ 4 ] },
					{ aimbot::aim_prediction[ 1 ][ 0 ], aimbot::aim_prediction[ 1 ][ 1 ], aimbot::aim_prediction[ 1 ][ 2 ], aimbot::aim_prediction[ 1 ][ 3 ], aimbot::aim_prediction[ 1 ][ 4 ] },
					{ aimbot::aim_prediction[ 2 ][ 0 ], aimbot::aim_prediction[ 2 ][ 1 ], aimbot::aim_prediction[ 2 ][ 2 ], aimbot::aim_prediction[ 2 ][ 3 ], aimbot::aim_prediction[ 2 ][ 4 ] },
					{ aimbot::aim_prediction[ 3 ][ 0 ], aimbot::aim_prediction[ 3 ][ 1 ], aimbot::aim_prediction[ 3 ][ 2 ], aimbot::aim_prediction[ 3 ][ 3 ], aimbot::aim_prediction[ 3 ][ 4 ] },
					{ aimbot::aim_prediction[ 4 ][ 0 ], aimbot::aim_prediction[ 4 ][ 1 ], aimbot::aim_prediction[ 4 ][ 2 ], aimbot::aim_prediction[ 4 ][ 3 ], aimbot::aim_prediction[ 4 ][ 4 ] }
				} },
				{ "smoothing", { aimbot::smoothing[ 0 ], aimbot::smoothing[ 1 ], aimbot::smoothing[ 2 ], aimbot::smoothing[ 3 ], aimbot::smoothing[ 4 ], aimbot::smoothing[ 5 ] } },
				{ "aim_bone", {
					{ aimbot::aim_bone[ 0 ][ 0 ], aimbot::aim_bone[ 0 ][ 1 ], aimbot::aim_bone[ 0 ][ 2 ], aimbot::aim_bone[ 0 ][ 3 ], aimbot::aim_bone[ 0 ][ 4 ] },
					{ aimbot::aim_bone[ 1 ][ 0 ], aimbot::aim_bone[ 1 ][ 1 ], aimbot::aim_bone[ 1 ][ 2 ], aimbot::aim_bone[ 1 ][ 3 ], aimbot::aim_bone[ 1 ][ 4 ] },
					{ aimbot::aim_bone[ 2 ][ 0 ], aimbot::aim_bone[ 2 ][ 1 ], aimbot::aim_bone[ 2 ][ 2 ], aimbot::aim_bone[ 2 ][ 3 ], aimbot::aim_bone[ 2 ][ 4 ] },
					{ aimbot::aim_bone[ 3 ][ 0 ], aimbot::aim_bone[ 3 ][ 1 ], aimbot::aim_bone[ 3 ][ 2 ], aimbot::aim_bone[ 3 ][ 3 ], aimbot::aim_bone[ 3 ][ 4 ] },
					{ aimbot::aim_bone[ 4 ][ 0 ], aimbot::aim_bone[ 4 ][ 1 ], aimbot::aim_bone[ 4 ][ 2 ], aimbot::aim_bone[ 4 ][ 3 ], aimbot::aim_bone[ 4 ][ 4 ] },
					{ aimbot::aim_bone[ 5 ][ 0 ], aimbot::aim_bone[ 5 ][ 1 ], aimbot::aim_bone[ 5 ][ 2 ], aimbot::aim_bone[ 5 ][ 3 ], aimbot::aim_bone[ 5 ][ 4 ] }
				} },
				{ "aim_key", { aimbot::aim_key[ 0 ], aimbot::aim_key[ 1 ], aimbot::aim_key[ 2 ], aimbot::aim_key[ 3 ], aimbot::aim_key[ 4 ], aimbot::aim_key[ 5 ] } },
				{ "fov_color", { aimbot::fov_color[ 0 ], aimbot::fov_color[ 1 ], aimbot::fov_color[ 2 ], aimbot::fov_color[ 3 ] } },
				{ "aim_toggle", { aimbot::aim_toggle[ 0 ], aimbot::aim_toggle[ 1 ], aimbot::aim_toggle[ 2 ], aimbot::aim_toggle[ 3 ], aimbot::aim_toggle[ 4 ], aimbot::aim_toggle[ 5 ] } },
				{ "restriction", { aimbot::restriction[ 0 ], aimbot::restriction[ 1 ], aimbot::restriction[ 2 ], aimbot::restriction[ 3 ], aimbot::restriction[ 4 ], aimbot::restriction[ 5 ] } },
				{ "smart_targeting", { aimbot::smart_targeting[ 0 ], aimbot::smart_targeting[ 1 ], aimbot::smart_targeting[ 2 ], aimbot::smart_targeting[ 3 ], aimbot::smart_targeting[ 4 ] } },
				{ "override_general", { aimbot::override_general[ 0 ], aimbot::override_general[ 1 ], aimbot::override_general[ 2 ], aimbot::override_general[ 3 ], aimbot::override_general[ 4 ] } },
				{ "ignore_downed", { aimbot::ignore_downed[ 0 ], aimbot::ignore_downed[ 1 ], aimbot::ignore_downed[ 2 ], aimbot::ignore_downed[ 3 ], aimbot::ignore_downed[ 4 ] } },
				{ "ignore_invulnerable", { aimbot::ignore_invulnerable[ 0 ], aimbot::ignore_invulnerable[ 1 ], aimbot::ignore_invulnerable[ 2 ], aimbot::ignore_invulnerable[ 3 ], aimbot::ignore_invulnerable[ 4 ] } },
				{ "ignore_bots", { aimbot::ignore_bots[ 0 ], aimbot::ignore_bots[ 1 ], aimbot::ignore_bots[ 2 ], aimbot::ignore_bots[ 3 ], aimbot::ignore_bots[ 4 ] } },
				{ "only_visible", { aimbot::only_visible[ 0 ], aimbot::only_visible[ 1 ], aimbot::only_visible[ 2 ], aimbot::only_visible[ 3 ], aimbot::only_visible[ 4 ] } },
				{ "aim_acceleration", { aimbot::aim_acceleration[ 0 ], aimbot::aim_acceleration[ 1 ], aimbot::aim_acceleration[ 2 ], aimbot::aim_acceleration[ 3 ], aimbot::aim_acceleration[ 4 ] } },
				{ "max_velocity", { aimbot::max_velocity[ 0 ], aimbot::max_velocity[ 1 ], aimbot::max_velocity[ 2 ], aimbot::max_velocity[ 3 ], aimbot::max_velocity[ 4 ] } },
				{ "acceleration", { aimbot::acceleration[ 0 ], aimbot::acceleration[ 1 ], aimbot::acceleration[ 2 ], aimbot::acceleration[ 3 ], aimbot::acceleration[ 4 ] } },
				{ "deceleration", { aimbot::deceleration[ 0 ], aimbot::deceleration[ 1 ], aimbot::deceleration[ 2 ], aimbot::deceleration[ 3 ], aimbot::deceleration[ 4 ] } },
				{ "velocity_threshold", { aimbot::velocity_threshold[ 0 ], aimbot::velocity_threshold[ 1 ], aimbot::velocity_threshold[ 2 ], aimbot::velocity_threshold[ 3 ], aimbot::velocity_threshold[ 4 ] } },
				{ "max_distance", { aimbot::max_distance[ 0 ], aimbot::max_distance[ 1 ], aimbot::max_distance[ 2 ], aimbot::max_distance[ 3 ], aimbot::max_distance[ 4 ], aimbot::max_distance[ 5 ] } }
			};

			j[ "triggerbot" ] = {
				{ "enabled", triggerbot::enabled },
				{ "deadzone", triggerbot::deadzone },
				{ "deadzone_radius", triggerbot::deadzone_radius },
				{ "deadzone_circular", triggerbot::deadzone_circular },
				{ "deadzone_width", triggerbot::deadzone_width },
				{ "deadzone_height", triggerbot::deadzone_height },
				{ "ignore_knocked", triggerbot::ignore_knocked },
				{ "weapon_config", { triggerbot::weapon_config[ 0 ], triggerbot::weapon_config[ 1 ], triggerbot::weapon_config[ 2 ], triggerbot::weapon_config[ 3 ], triggerbot::weapon_config[ 4 ], triggerbot::weapon_config[ 5 ] } },
				{ "fire_key", triggerbot::fire_key },
				{ "aim_toggle", triggerbot::aim_toggle },
				{ "key_bind", triggerbot::key_bind },
				{ "delay", triggerbot::delay },
				{ "max_distance", triggerbot::max_distance }
			};

			j[ "enemies" ] = {
				{ "enabled", enemies::enabled },
				{ "box_esp", enemies::box_esp },
				{ "box_style", { enemies::box_style[ 0 ], enemies::box_style[ 1 ], enemies::box_style[ 2 ] } },
				{ "line_esp", enemies::line_esp },
				{ "directional_box", enemies::directional_box },
				{ "skeleton", enemies::skeleton },
				{ "snaplines", enemies::snaplines },
				{ "weapon_name", enemies::weapon_name },
				{ "display_name", enemies::display_name },
				{ "season_rank", enemies::season_rank },
				{ "platform", enemies::platform },
				{ "level", enemies::level },
				{ "damage", enemies::damage },
				{ "movement", enemies::movement },
				{ "weapon_icon", enemies::weapon_icon },
				{ "team_mode", enemies::team_mode },
				{ "kills", enemies::kills },
				{ "fov_arrows", enemies::fov_arrows },
				{ "view_line", enemies::view_line },
				{ "distance", enemies::distance },
				{ "stencil", enemies::stencil },
				{ "reload_bar", enemies::reload_bar },
				{ "wins", enemies::wins },
				{ "winrate", enemies::winrate },
				{ "kdr", enemies::kdr },
				{ "glow_color", enemies::glow_color },
				{ "movement_tracers", enemies::movement_tracers },
				{ "chinse_hat", enemies::chinse_hat },
				{ "hat_segments", enemies::hat_segments },
				{ "hat_base_offset", enemies::hat_base_offset },
				{ "hat_height", enemies::hat_height },
				{ "hat_radius", enemies::hat_radius },
				{ "skeleton_type", enemies::skeleton_type },
				{ "outline_thickness", enemies::outline_thickness },
				{ "max_distance", enemies::max_distance },
				{ "skeleton_thickness", enemies::skeleton_thickness },
				{ "skeleton_outline", enemies::skeleton_outline },
				{ "head_circle", enemies::head_circle },
				{ "head_hexagon", enemies::head_hexagon },
				{ "head_size", enemies::head_size },
				{ "wireframe", enemies::wireframe },
				{ "duration", enemies::duration },
				{ "update", enemies::update },
				{ "trial_thickness", enemies::trial_thickness },
				{ "trial_style", enemies::trial_style },
				{ "trial_fade", enemies::trial_fade }
			};

			j[ "self" ] = {
				{ "enabled", self::enabled },
				{ "spectators", self::spectators },
				{ "inventory", self::inventory },
				{ "damage", self::damage },
				{ "targeting", self::targeting },
				{ "movement_tracers", self::movement_tracers },
				{ "bullet_tracers", self::bullet_tracers },
				{ "zone_prediction", self::zone_prediction },
				{ "draw_next_zone", self::draw_next_zone },
				{ "zone_center", self::zone_center },
				{ "zone_info", self::zone_info },
				{ "max_tracers", self::max_tracers },
				{ "fadeout_time", self::fadeout_time },
				{ "zone_shape", self::zone_shape },
				{ "show_next_zone", self::show_next_zone },
				{ "show_zone_info", self::show_zone_info },
				{ "center_line", self::center_line },
				{ "zone_thickness", self::zone_thickness },
				{ "duration", self::duration },
				{ "update", self::update },
				{ "trial_thickness", self::trial_thickness },
				{ "trial_style", self::trial_style },
				{ "trial_fade", self::trial_fade }
			};

			j[ "pickups" ] = {
				{ "enabled", pickups::enabled },
				{ "radar_distance", pickups::radar_distance },
				{ "show_distance", pickups::show_distance },
				{ "damage", pickups::damage },
				{ "magazine_size", pickups::magazine_size },
				{ "weapon_icon", pickups::weapon_icon },
				{ "loot_tier", { pickups::loot_tier[ 0 ], pickups::loot_tier[ 1 ], pickups::loot_tier[ 2 ], pickups::loot_tier[ 3 ], pickups::loot_tier[ 4 ] } },
				{ "max_distance", pickups::max_distance }
			};

			j[ "containers" ] = {
				{ "enabled", containers::enabled },
				{ "loot_tier", containers::loot_tier },
				{ "radar_distance", containers::radar_distance },
				{ "show_distance", containers::show_distance },
				{ "container_chams", containers::container_chams },
				{ "cham_color", containers::cham_color },
				{ "max_distance", containers::max_distance }
			};

			j[ "weakspots" ] = {
				{ "enabled", weakspots::enabled },
				{ "show_distance", weakspots::show_distance },
				{ "weakspot_aimbot", weakspots::weakspot_aimbot },
				{ "smoothing_modifier", weakspots::smoothing_modifier },
				{ "max_distance", weakspots::max_distance }
			};

			j[ "projectiles" ] = {
				{ "enabled", projectiles::enabled },
				{ "show_distance", projectiles::show_distance },
				{ "show_tracers", projectiles::show_tracers },
				{ "hit_marker", projectiles::hit_marker },
				{ "max_distance", projectiles::max_distance }
			};

			j[ "vehicles" ] = {
				{ "enabled", vehicles::enabled },
				{ "terrain", vehicles::terrain },
				{ "health", vehicles::health },
				{ "show_distance", vehicles::show_distance },
				{ "max_distance", vehicles::max_distance }
			};

			j[ "buildings" ] = {
				{ "enabled", buildings::enabled },
				{ "build_type", buildings::build_type },
				{ "collision_box", buildings::collision_box },
				{ "show_distance", buildings::show_distance },
				{ "max_distance", buildings::max_distance }
			};

			j[ "supply_drops" ] = {
				{ "enabled", supply_drops::enabled },
				{ "show_distance", supply_drops::show_distance },
				{ "players_searching", supply_drops::players_searching },
				{ "supply_loot", supply_drops::supply_loot },
				{ "max_distance", supply_drops::max_distance }
			};

			j[ "llamas" ] = {
				{ "enabled", llamas::enabled },
				{ "show_distance", llamas::show_distance },
				{ "max_distance", llamas::max_distance }
			};

			j[ "radar" ] = {
				{ "enabled", radar::enabled },
				{ "distance", radar::distance },
				{ "opacity", radar::opacity },
				{ "position", radar::position },
				{ "position_x", radar::position_x },
				{ "position_y", radar::position_y },
				{ "size", radar::size },
				{ "range", radar::range },
				{ "grid", radar::grid },
				{ "grid_divisions", radar::grid_divisions },
				{ "line_of_sight", radar::line_of_sight },
				{ "los_fill", radar::los_fill },
				{ "rotation_mode", radar::rotation_mode },
				{ "show_zone", radar::show_zone },
				{ "show_next_zone", radar::show_next_zone },
				{ "storm_overlay", radar::storm_overlay },
				{ "zone_out_of_range_indicator", radar::zone_out_of_range_indicator },
				{ "indicator_size", radar::indicator_size }
			};

			j[ "misc" ] = {
				{ "enabled", misc::enabled },
				{ "vsync", misc::vsync },
				{ "performance_mode", misc::performance_mode },
				{ "performance_counter", misc::performance_counter },
				{ "controller_support", misc::controller_support },
				{ "stream_proof", misc::stream_proof },
				{ "actor_update", misc::actor_update },
				{ "collection_update", misc::collection_update },
				{ "camera_update", misc::camera_update },
				{ "engine_update", misc::engine_update },
				{ "crosshair", {
					{ "enabled", misc::crosshair::enabled },
					{ "color", { misc::crosshair::color[ 0 ], misc::crosshair::color[ 1 ], misc::crosshair::color[ 2 ], misc::crosshair::color[ 3 ] } },
					{ "radius", misc::crosshair::radius },
					{ "thickness", misc::crosshair::thickness },
					{ "num_segments", misc::crosshair::num_segments },
					{ "outline", misc::crosshair::outline }
				} }
			};

			j[ "configs" ] = {
				{ "enabled", configs::enabled }
			};

			j[ "exploits" ] = {
				{ "enabled", exploits::enabled },
				{ "hit_sound", exploits::hit_sound },
				{ "sound", exploits::sound },
				{ "no_spread", exploits::no_spread },
				{ "spread_x", exploits::spread_x },
				{ "spread_y", exploits::spread_y },
				{ "no_recoil", exploits::no_recoil },
				{ "recoil_x", exploits::recoil_x },
				{ "recoil_y", exploits::recoil_y },
				{ "recoil_z", exploits::recoil_z },
				{ "no_sway", exploits::no_sway },
				{ "sway", exploits::sway },
				{ "no_bloom", exploits::no_bloom },
				{ "bloom", exploits::bloom },
				{ "no_fall_damage", exploits::no_fall_damage },
				{ "fall_damage", exploits::fall_damage },
				{ "instant_interact", exploits::instant_interact },
				{ "no_pickup_collision", exploits::no_pickup_collision },
				{ "dark_sky", exploits::dark_sky },
				{ "no_fog", exploits::no_fog },
				{ "shoot_through_walls", exploits::shoot_through_walls },
				{ "desync", exploits::desync },
				{ "player_fly", exploits::player_fly },
				{ "fov_changer", exploits::fov_changer },
				{ "min_fov", exploits::min_fov },
				{ "max_fov", exploits::max_fov },
				{ "no_collision", exploits::no_collision },
				{ "firerate_modifier", exploits::firerate_modifier },
				{ "firerate", exploits::firerate },
				{ "no_reload", exploits::no_reload },
				{ "reload_time", exploits::reload_time },
				{ "infinite_jump", exploits::infinite_jump },
				{ "jump_time", exploits::jump_time },
				{ "infinite_fuel", exploits::infinite_fuel },
				{ "bike_jump", exploits::bike_jump },
				{ "vehicle_fly", exploits::vehicle_fly },
				{ "key_up", exploits::key_up },
				{ "key_down", exploits::key_down },
				{ "max_jump_force", exploits::max_jump_force },
				{ "max_jump_forward", exploits::max_jump_forward }
			};

			j[ "debug" ] = {
				{ "box_width", debug::box_width },
				{ "box_spacing", debug::box_spacing },
				{ "start_y", debug::start_y },
				{ "event_timeout", debug::event_timeout }
			};

			return j;
		}

		bool from_json( const json& j ) {
			try {
				if ( j.contains( "aimbot" ) ) {
					const auto& a = j[ "aimbot" ];
					aimbot::enabled = a.value( "enabled", aimbot::enabled );
					aimbot::weapon_configs = a.value( "weapon_configs", aimbot::weapon_configs );
					aimbot::close_aim = a.value( "close_aim", aimbot::close_aim );
					aimbot::shotgun_only = a.value( "shotgun_only", aimbot::shotgun_only );

					const auto& silent_aim = a[ "silent_aim" ];
					if ( silent_aim.is_array( ) && silent_aim.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::silent_aim[ i ] = silent_aim[ i ].get<bool>();
						}
					}

					const auto& hit_chance = a[ "hit_chance" ];
					if ( hit_chance.is_array( ) && hit_chance.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::hit_chance[ i ] = hit_chance[ i ].get<int>();
						}
					}

					const auto& linear_aim = a[ "linear_aim" ];
					if ( linear_aim.is_array( ) && linear_aim.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::linear_aim[ i ] = linear_aim[ i ].get<bool>();
						}
					}

					const auto& no_target_switch = a[ "no_target_switch" ];
					if ( no_target_switch.is_array( ) && no_target_switch.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::no_target_switch[ i ] = no_target_switch[ i ].get<bool>();
						}
					}

					const auto& inverted_vertical_axis = a[ "inverted_vertical_axis" ];
					if ( inverted_vertical_axis.is_array( ) && inverted_vertical_axis.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::inverted_vertical_axis[ i ] = inverted_vertical_axis[ i ].get<bool>();
						}
					}

					const auto& fov_circle = a[ "fov_circle" ];
					if ( fov_circle.is_array( ) && fov_circle.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::fov_circle[ i ] = fov_circle[ i ].get<bool>();
						}
					}

					const auto& target_line = a[ "target_line" ];
					if ( target_line.is_array( ) && target_line.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::target_line[ i ] = target_line[ i ].get<bool>();
						}
					}

					const auto& target_dot = a[ "target_dot" ];
					if ( target_dot.is_array( ) && target_dot.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::target_dot[ i ] = target_dot[ i ].get<bool>();
						}
					}

					const auto& deadzone_dot = a[ "deadzone_dot" ];
					if ( deadzone_dot.is_array( ) && deadzone_dot.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::deadzone_dot[ i ] = deadzone_dot[ i ].get<bool>();
						}
					}

					const auto& prediction_x = a[ "prediction_x" ];
					if ( prediction_x.is_array( ) && prediction_x.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::prediction_x[ i ] = prediction_x[ i ].get<bool>();
						}
					}

					const auto& aim_curve = a[ "aim_curve" ];
					if ( aim_curve.is_array( ) && aim_curve.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::aim_curve[ i ] = aim_curve[ i ].get<bool>();
						}
					}

					const auto& fov_radius = a[ "fov_radius" ];
					if ( fov_radius.is_array( ) && fov_radius.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::fov_radius[ i ] = fov_radius[ i ].get<float>();
						}
					}

					const auto& deadzone = a[ "deadzone" ];
					if ( deadzone.is_array( ) && deadzone.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::deadzone[ i ] = deadzone[ i ].get<bool>();
						}
					}

					const auto& aim_prediction = a[ "aim_prediction" ];
					if ( aim_prediction.is_array( ) && aim_prediction.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							const auto& pred = aim_prediction[ i ];
							if ( pred.is_array( ) && pred.size( ) >= 5 ) {
								for ( int j = 0; j < 5; j++ ) {
									aimbot::aim_prediction[ i ][ j ] = pred[ j ].get<bool>();
								}
							}
						}
					}

					const auto& smoothing = a[ "smoothing" ];
					if ( smoothing.is_array( ) && smoothing.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::smoothing[ i ] = smoothing[ i ].get<int>();
						}
					}

					const auto& aim_bone = a[ "aim_bone" ];
					if ( aim_bone.is_array( ) && aim_bone.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							const auto& bones = aim_bone[ i ];
							if ( bones.is_array( ) && bones.size( ) >= 5 ) {
								for ( int j = 0; j < 5; j++ ) {
									aimbot::aim_bone[ i ][ j ] = bones[ j ].get<bool>();
								}
							}
						}
					}

					const auto& aim_key = a[ "aim_key" ];
					if ( aim_key.is_array( ) && aim_key.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::aim_key[ i ] = aim_key[ i ].get<int>();
						}
					}

					const auto& fov_color = a[ "fov_color" ];
					if ( fov_color.is_array( ) && fov_color.size( ) >= 4 ) {
						for ( int i = 0; i < 4; i++ ) {
							aimbot::fov_color[ i ] = fov_color[ i ].get<float>();
						}
					}

					const auto& aim_toggle = a[ "aim_toggle" ];
					if ( aim_toggle.is_array( ) && aim_toggle.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::aim_toggle[ i ] = aim_toggle[ i ].get<bool>();
						}
					}

					const auto& restriction = a[ "restriction" ];
					if ( restriction.is_array( ) && restriction.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::restriction[ i ] = restriction[ i ].get<float>();
						}
					}

					const auto& smart_targeting = a[ "smart_targeting" ];
					if ( smart_targeting.is_array( ) && smart_targeting.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::smart_targeting[ i ] = smart_targeting[ i ].get<bool>();
						}
					}

					const auto& override_general = a[ "override_general" ];
					if ( override_general.is_array( ) && override_general.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::override_general[ i ] = override_general[ i ].get<bool>();
						}
					}

					const auto& ignore_downed = a[ "ignore_downed" ];
					if ( ignore_downed.is_array( ) && ignore_downed.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::ignore_downed[ i ] = ignore_downed[ i ].get<bool>();
						}
					}

					const auto& ignore_invulnerable = a[ "ignore_invulnerable" ];
					if ( ignore_invulnerable.is_array( ) && ignore_invulnerable.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::ignore_invulnerable[ i ] = ignore_invulnerable[ i ].get<bool>();
						}
					}

					const auto& ignore_bots = a[ "ignore_bots" ];
					if ( ignore_bots.is_array( ) && ignore_bots.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::ignore_bots[ i ] = ignore_bots[ i ].get<bool>();
						}
					}

					const auto& only_visible = a[ "only_visible" ];
					if ( only_visible.is_array( ) && only_visible.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::only_visible[ i ] = only_visible[ i ].get<bool>();
						}
					}

					const auto& aim_acceleration = a[ "aim_acceleration" ];
					if ( aim_acceleration.is_array( ) && aim_acceleration.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::aim_acceleration[ i ] = aim_acceleration[ i ].get<bool>();
						}
					}

					const auto& max_velocity = a[ "max_velocity" ];
					if ( max_velocity.is_array( ) && max_velocity.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::max_velocity[ i ] = max_velocity[ i ].get<float>();
						}
					}

					const auto& acceleration = a[ "acceleration" ];
					if ( acceleration.is_array( ) && acceleration.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::acceleration[ i ] = acceleration[ i ].get<float>();
						}
					}

					const auto& deceleration = a[ "deceleration" ];
					if ( deceleration.is_array( ) && deceleration.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::deceleration[ i ] = deceleration[ i ].get<float>();
						}
					}

					const auto& velocity_threshold = a[ "velocity_threshold" ];
					if ( velocity_threshold.is_array( ) && velocity_threshold.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							aimbot::velocity_threshold[ i ] = velocity_threshold[ i ].get<float>();
						}
					}

					const auto& max_distance = a[ "max_distance" ];
					if ( max_distance.is_array( ) && max_distance.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::max_distance[ i ] = max_distance[ i ].get<int>();
						}
					}
				}

				if ( j.contains( "triggerbot" ) ) {
					const auto& t = j[ "triggerbot" ];
					triggerbot::enabled = t.value( "enabled", triggerbot::enabled );
					triggerbot::deadzone = t.value( "deadzone", triggerbot::deadzone );
					triggerbot::deadzone_radius = t.value( "deadzone_radius", triggerbot::deadzone_radius );
					triggerbot::deadzone_circular = t.value( "deadzone_circular", triggerbot::deadzone_circular );
					triggerbot::deadzone_width = t.value( "deadzone_width", triggerbot::deadzone_width );
					triggerbot::deadzone_height = t.value( "deadzone_height", triggerbot::deadzone_height );
					triggerbot::ignore_knocked = t.value( "ignore_knocked", triggerbot::ignore_knocked );

					const auto& weapon_config = t[ "weapon_config" ];
					if ( weapon_config.is_array( ) && weapon_config.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							triggerbot::weapon_config[ i ] = weapon_config[ i ].get<bool>();
						}
					}
					triggerbot::fire_key = t.value( "fire_key", triggerbot::fire_key );
					triggerbot::aim_toggle = t.value( "aim_toggle", triggerbot::aim_toggle );
					triggerbot::key_bind = t.value( "key_bind", triggerbot::key_bind );
					triggerbot::delay = t.value( "delay", triggerbot::delay );
					triggerbot::max_distance = t.value( "max_distance", triggerbot::max_distance );
				}

				if ( j.contains( "enemies" ) ) {
					const auto& v = j[ "enemies" ];
					enemies::enabled = v.value( "enabled", enemies::enabled );
					enemies::box_esp = v.value( "box_esp", enemies::box_esp );
					
					const auto& box_style = v[ "box_style" ];
					if ( box_style.is_array( ) && box_style.size( ) >= 3 ) {
						for ( int i = 0; i < 3; i++ ) {
							enemies::box_style[ i ] = box_style[ i ].get<bool>();
						}
					}
					
					enemies::line_esp = v.value( "line_esp", enemies::line_esp );
					enemies::directional_box = v.value( "directional_box", enemies::directional_box );
					enemies::skeleton = v.value( "skeleton", enemies::skeleton );
					enemies::snaplines = v.value( "snaplines", enemies::snaplines );
					enemies::weapon_name = v.value( "weapon_name", enemies::weapon_name );
					enemies::display_name = v.value( "display_name", enemies::display_name );
					enemies::season_rank = v.value( "season_rank", enemies::season_rank );
					enemies::platform = v.value( "platform", enemies::platform );
					enemies::level = v.value( "level", enemies::level );
					enemies::damage = v.value( "damage", enemies::damage );
					enemies::movement = v.value( "movement", enemies::movement );
					enemies::weapon_icon = v.value( "weapon_icon", enemies::weapon_icon );
					enemies::team_mode = v.value( "team_mode", enemies::team_mode );
					enemies::kills = v.value( "kills", enemies::kills );
					enemies::fov_arrows = v.value( "fov_arrows", enemies::fov_arrows );
					enemies::view_line = v.value( "view_line", enemies::view_line );
					enemies::distance = v.value( "distance", enemies::distance );
					enemies::stencil = v.value( "stencil", enemies::stencil );
					enemies::reload_bar = v.value( "reload_bar", enemies::reload_bar );
					enemies::wins = v.value( "wins", enemies::wins );
					enemies::winrate = v.value( "winrate", enemies::winrate );
					enemies::kdr = v.value( "kdr", enemies::kdr );
					enemies::glow_color = v.value( "glow_color", enemies::glow_color );
					enemies::movement_tracers = v.value( "movement_tracers", enemies::movement_tracers );
					enemies::chinse_hat = v.value( "chinse_hat", enemies::chinse_hat );
					enemies::hat_segments = v.value( "hat_segments", enemies::hat_segments );
					enemies::hat_base_offset = v.value( "hat_base_offset", enemies::hat_base_offset );
					enemies::hat_height = v.value( "hat_height", enemies::hat_height );
					enemies::hat_radius = v.value( "hat_radius", enemies::hat_radius );
					enemies::skeleton_type = v.value( "skeleton_type", enemies::skeleton_type );
					enemies::outline_thickness = v.value( "outline_thickness", enemies::outline_thickness );
					enemies::max_distance = v.value( "max_distance", enemies::max_distance );
					enemies::skeleton_thickness = v.value( "skeleton_thickness", enemies::skeleton_thickness );
					enemies::skeleton_outline = v.value( "skeleton_outline", enemies::skeleton_outline );
					enemies::head_circle = v.value( "head_circle", enemies::head_circle );
					enemies::head_hexagon = v.value( "head_hexagon", enemies::head_hexagon );
					enemies::head_size = v.value( "head_size", enemies::head_size );
					enemies::wireframe = v.value( "wireframe", enemies::wireframe );
					enemies::duration = v.value( "duration", enemies::duration );
					enemies::update = v.value( "update", enemies::update );
					enemies::trial_thickness = v.value( "trial_thickness", enemies::trial_thickness );
					enemies::trial_style = v.value( "trial_style", enemies::trial_style );
					enemies::trial_fade = v.value( "trial_fade", enemies::trial_fade );
				}

				if ( j.contains( "self" ) ) {
					const auto& s = j[ "self" ];
					self::enabled = s.value( "enabled", self::enabled );
					self::spectators = s.value( "spectators", self::spectators );
					self::inventory = s.value( "inventory", self::inventory );
					self::damage = s.value( "damage", self::damage );
					self::targeting = s.value( "targeting", self::targeting );
					self::movement_tracers = s.value( "movement_tracers", self::movement_tracers );
					self::bullet_tracers = s.value( "bullet_tracers", self::bullet_tracers );
					self::zone_prediction = s.value( "zone_prediction", self::zone_prediction );
					self::draw_next_zone = s.value( "draw_next_zone", self::draw_next_zone );
					self::zone_center = s.value( "zone_center", self::zone_center );
					self::zone_info = s.value( "zone_info", self::zone_info );
					self::max_tracers = s.value( "max_tracers", self::max_tracers );
					self::fadeout_time = s.value( "fadeout_time", self::fadeout_time );
					self::zone_shape = s.value( "zone_shape", self::zone_shape );
					self::show_next_zone = s.value( "show_next_zone", self::show_next_zone );
					self::show_zone_info = s.value( "show_zone_info", self::show_zone_info );
					self::center_line = s.value( "center_line", self::center_line );
					self::zone_thickness = s.value( "zone_thickness", self::zone_thickness );
					self::duration = s.value( "duration", self::duration );
					self::update = s.value( "update", self::update );
					self::trial_thickness = s.value( "trial_thickness", self::trial_thickness );
					self::trial_style = s.value( "trial_style", self::trial_style );
					self::trial_fade = s.value( "trial_fade", self::trial_fade );
				}

				if ( j.contains( "pickups" ) ) {
					const auto& p = j[ "pickups" ];
					pickups::enabled = p.value( "enabled", pickups::enabled );
					pickups::radar_distance = p.value( "radar_distance", pickups::radar_distance );
					pickups::show_distance = p.value( "show_distance", pickups::show_distance );
					pickups::damage = p.value( "damage", pickups::damage );
					pickups::magazine_size = p.value( "magazine_size", pickups::magazine_size );
					pickups::weapon_icon = p.value( "weapon_icon", pickups::weapon_icon );

					const auto& tiers = p[ "loot_tier" ];
					if ( tiers.is_array( ) && tiers.size( ) >= 5 ) {
						for ( int i = 0; i < 5; i++ ) {
							pickups::loot_tier[ i ] = tiers[ i ].get<bool>();
						}
					}
					pickups::max_distance = p.value( "max_distance", pickups::max_distance );
				}

				if ( j.contains( "containers" ) ) {
					const auto& c = j[ "containers" ];
					containers::enabled = c.value( "enabled", containers::enabled );
					containers::loot_tier = c.value( "loot_tier", containers::loot_tier );
					containers::radar_distance = c.value( "radar_distance", containers::radar_distance );
					containers::show_distance = c.value( "show_distance", containers::show_distance );
					containers::container_chams = c.value( "container_chams", containers::container_chams );
					containers::cham_color = c.value( "cham_color", containers::cham_color );
					containers::max_distance = c.value( "max_distance", containers::max_distance );
				}

				if ( j.contains( "weakspots" ) ) {
					const auto& w = j[ "weakspots" ];
					weakspots::enabled = w.value( "enabled", weakspots::enabled );
					weakspots::show_distance = w.value( "show_distance", weakspots::show_distance );
					weakspots::weakspot_aimbot = w.value( "weakspot_aimbot", weakspots::weakspot_aimbot );
					weakspots::smoothing_modifier = w.value( "smoothing_modifier", weakspots::smoothing_modifier );
					weakspots::max_distance = w.value( "max_distance", weakspots::max_distance );
				}

				if ( j.contains( "projectiles" ) ) {
					const auto& p = j[ "projectiles" ];
					projectiles::enabled = p.value( "enabled", projectiles::enabled );
					projectiles::show_distance = p.value( "show_distance", projectiles::show_distance );
					projectiles::show_tracers = p.value( "show_tracers", projectiles::show_tracers );
					projectiles::hit_marker = p.value( "hit_marker", projectiles::hit_marker );
					projectiles::max_distance = p.value( "max_distance", projectiles::max_distance );
				}

				if ( j.contains( "vehicles" ) ) {
					const auto& v = j[ "vehicles" ];
					vehicles::enabled = v.value( "enabled", vehicles::enabled );
					vehicles::terrain = v.value( "terrain", vehicles::terrain );
					vehicles::health = v.value( "health", vehicles::health );
					vehicles::show_distance = v.value( "show_distance", vehicles::show_distance );
					vehicles::max_distance = v.value( "max_distance", vehicles::max_distance );
				}

				if ( j.contains( "buildings" ) ) {
					const auto& b = j[ "buildings" ];
					buildings::enabled = b.value( "enabled", buildings::enabled );
					buildings::build_type = b.value( "build_type", buildings::build_type );
					buildings::collision_box = b.value( "collision_box", buildings::collision_box );
					buildings::show_distance = b.value( "show_distance", buildings::show_distance );
					buildings::max_distance = b.value( "max_distance", buildings::max_distance );
				}

				if ( j.contains( "supply_drops" ) ) {
					const auto& s = j[ "supply_drops" ];
					supply_drops::enabled = s.value( "enabled", supply_drops::enabled );
					supply_drops::show_distance = s.value( "show_distance", supply_drops::show_distance );
					supply_drops::players_searching = s.value( "players_searching", supply_drops::players_searching );
					supply_drops::supply_loot = s.value( "supply_loot", supply_drops::supply_loot );
					supply_drops::max_distance = s.value( "max_distance", supply_drops::max_distance );
				}

				if ( j.contains( "llamas" ) ) {
					const auto& l = j[ "llamas" ];
					llamas::enabled = l.value( "enabled", llamas::enabled );
					llamas::show_distance = l.value( "show_distance", llamas::show_distance );
					llamas::max_distance = l.value( "max_distance", llamas::max_distance );
				}

				if ( j.contains( "radar" ) ) {
					const auto& r = j[ "radar" ];
					radar::enabled = r.value( "enabled", radar::enabled );
					radar::distance = r.value( "distance", radar::distance );
					radar::opacity = r.value( "opacity", radar::opacity );
					radar::position = r.value( "position", radar::position );
					radar::position_x = r.value( "position_x", radar::position_x );
					radar::position_y = r.value( "position_y", radar::position_y );
					radar::size = r.value( "size", radar::size );
					radar::range = r.value( "range", radar::range );
					radar::grid = r.value( "grid", radar::grid );
					radar::grid_divisions = r.value( "grid_divisions", radar::grid_divisions );
					radar::line_of_sight = r.value( "line_of_sight", radar::line_of_sight );
					radar::los_fill = r.value( "los_fill", radar::los_fill );
					radar::rotation_mode = r.value( "rotation_mode", radar::rotation_mode );
					radar::show_zone = r.value( "show_zone", radar::show_zone );
					radar::show_next_zone = r.value( "show_next_zone", radar::show_next_zone );
					radar::storm_overlay = r.value( "storm_overlay", radar::storm_overlay );
					radar::zone_out_of_range_indicator = r.value( "zone_out_of_range_indicator", radar::zone_out_of_range_indicator );
					radar::indicator_size = r.value( "indicator_size", radar::indicator_size );
				}

				if ( j.contains( "misc" ) ) {
					const auto& m = j[ "misc" ];
					misc::enabled = m.value( "enabled", misc::enabled );
					misc::vsync = m.value( "vsync", misc::vsync );
					misc::performance_mode = m.value( "performance_mode", misc::performance_mode );
					misc::performance_counter = m.value( "performance_counter", misc::performance_counter );
					misc::controller_support = m.value( "controller_support", misc::controller_support );
					misc::stream_proof = m.value( "stream_proof", misc::stream_proof );
					misc::actor_update = m.value( "actor_update", misc::actor_update );
					misc::collection_update = m.value( "collection_update", misc::collection_update );
					misc::camera_update = m.value( "camera_update", misc::camera_update );
					misc::engine_update = m.value( "engine_update", misc::engine_update );

					const auto& crosshair = m[ "crosshair" ];
					if ( crosshair.is_object( ) ) {
						misc::crosshair::enabled = crosshair.value( "enabled", misc::crosshair::enabled );
						const auto& color = crosshair[ "color" ];
						if ( color.is_array( ) && color.size( ) >= 4 ) {
							for ( int i = 0; i < 4; i++ ) {
								misc::crosshair::color[ i ] = color[ i ].get<float>();
							}
						}
						misc::crosshair::radius = crosshair.value( "radius", misc::crosshair::radius );
						misc::crosshair::thickness = crosshair.value( "thickness", misc::crosshair::thickness );
						misc::crosshair::num_segments = crosshair.value( "num_segments", misc::crosshair::num_segments );
						misc::crosshair::outline = crosshair.value( "outline", misc::crosshair::outline );
					}
				}

				if ( j.contains( "configs" ) ) {
					const auto& c = j[ "configs" ];
					configs::enabled = c.value( "enabled", configs::enabled );
				}

				if ( j.contains( "exploits" ) ) {
					const auto& e = j[ "exploits" ];
					exploits::enabled = e.value( "enabled", exploits::enabled );
					exploits::hit_sound = e.value( "hit_sound", exploits::hit_sound );
					exploits::sound = e.value( "sound", exploits::sound );
					exploits::no_spread = e.value( "no_spread", exploits::no_spread );
					exploits::spread_x = e.value( "spread_x", exploits::spread_x );
					exploits::spread_y = e.value( "spread_y", exploits::spread_y );
					exploits::no_recoil = e.value( "no_recoil", exploits::no_recoil );
					exploits::recoil_x = e.value( "recoil_x", exploits::recoil_x );
					exploits::recoil_y = e.value( "recoil_y", exploits::recoil_y );
					exploits::recoil_z = e.value( "recoil_z", exploits::recoil_z );
					exploits::no_sway = e.value( "no_sway", exploits::no_sway );
					exploits::sway = e.value( "sway", exploits::sway );
					exploits::no_bloom = e.value( "no_bloom", exploits::no_bloom );
					exploits::bloom = e.value( "bloom", exploits::bloom );
					exploits::no_fall_damage = e.value( "no_fall_damage", exploits::no_fall_damage );
					exploits::fall_damage = e.value( "fall_damage", exploits::fall_damage );
					exploits::instant_interact = e.value( "instant_interact", exploits::instant_interact );
					exploits::no_pickup_collision = e.value( "no_pickup_collision", exploits::no_pickup_collision );
					exploits::dark_sky = e.value( "dark_sky", exploits::dark_sky );
					exploits::no_fog = e.value( "no_fog", exploits::no_fog );
					exploits::shoot_through_walls = e.value( "shoot_through_walls", exploits::shoot_through_walls );
					exploits::desync = e.value( "desync", exploits::desync );
					exploits::player_fly = e.value( "player_fly", exploits::player_fly );
					exploits::fov_changer = e.value( "fov_changer", exploits::fov_changer );
					exploits::min_fov = e.value( "min_fov", exploits::min_fov );
					exploits::max_fov = e.value( "max_fov", exploits::max_fov );
					exploits::no_collision = e.value( "no_collision", exploits::no_collision );
					exploits::firerate_modifier = e.value( "firerate_modifier", exploits::firerate_modifier );
					exploits::firerate = e.value( "firerate", exploits::firerate );
					exploits::no_reload = e.value( "no_reload", exploits::no_reload );
					exploits::reload_time = e.value( "reload_time", exploits::reload_time );
					exploits::infinite_jump = e.value( "infinite_jump", exploits::infinite_jump );
					exploits::jump_time = e.value( "jump_time", exploits::jump_time );
					exploits::infinite_fuel = e.value( "infinite_fuel", exploits::infinite_fuel );
					exploits::bike_jump = e.value( "bike_jump", exploits::bike_jump );
					exploits::vehicle_fly = e.value( "vehicle_fly", exploits::vehicle_fly );
					exploits::key_up = e.value( "key_up", exploits::key_up );
					exploits::key_down = e.value( "key_down", exploits::key_down );
					exploits::max_jump_force = e.value( "max_jump_force", exploits::max_jump_force );
					exploits::max_jump_forward = e.value( "max_jump_forward", exploits::max_jump_forward );
				}

				if ( j.contains( "debug" ) ) {
					const auto& d = j[ "debug" ];
					debug::box_width = d.value( "box_width", debug::box_width );
					debug::box_spacing = d.value( "box_spacing", debug::box_spacing );
					debug::start_y = d.value( "start_y", debug::start_y );
					debug::event_timeout = d.value( "event_timeout", debug::event_timeout );
				}

				return true;
			}
			catch ( const std::exception& e ) {
				return false;
			}
		}

		bool save( const std::string& filename ) {
			try {
				std::ofstream file( filename, std::ios::out | std::ios::trunc );
				if ( !file.is_open( ) ) {
					return false;
				}

				json j = to_json( );
				file << std::setw(4) << j;
				file.close();
				return true;
			}
			catch ( const std::exception& e ) {
				return false;
			}
		}

		bool load( const std::string& filename ) {
			try {
				std::ifstream file( filename, std::ios::in );
				if ( !file.is_open( ) ) {
					return false;
				}

				json j;
				file >> j;
				file.close();
				
				if (j.is_null() || j.empty()) {
					return false;
				}

				return from_json( j );
			}
			catch ( const std::exception& e ) {
				return false;
			}
		}
	}

	class c_config {
	public:
		c_config( ) { }
		c_config( const c_config& ) = delete;
		c_config& operator=( const c_config& ) = delete;

		std::vector<std::string> get_configs( ) {
			return m_available_configs;
		}

		void get_available_configs( ) {
			m_available_configs.clear( );
			std::filesystem::path config_dir = get_config_directory( );
			if ( !std::filesystem::exists( config_dir ) ) {
				std::filesystem::create_directories( config_dir );
				return;
			}

			for ( const auto& entry : std::filesystem::directory_iterator( config_dir ) ) {
				if ( entry.path( ).extension( ) == ".json" ) {
					m_available_configs.push_back( entry.path( ).stem( ).string( ) );
				}
			}
		}

		bool save_config( const std::string& name ) {
			std::filesystem::path config_dir = get_config_directory( );
			if ( !std::filesystem::exists( config_dir ) ) {
				std::filesystem::create_directories( config_dir );
			}
			std::filesystem::path config_path = config_dir / ( name + ".json" );
			return json::save( config_path.string( ) );
		}

		bool load_config( const std::string& name ) {
			std::filesystem::path config_path = get_config_directory( ) / ( name + ".json" );
			return json::load( config_path.string( ) );
		}

		bool delete_config( const std::string& name ) {
			std::filesystem::path config_path = get_config_directory( ) / ( name + ".json" );
			if ( std::filesystem::exists( config_path ) ) {
				return std::filesystem::remove( config_path );
			}
			return false;
		}

		bool rename_config( const std::string& old_name, const std::string& new_name ) {
			std::filesystem::path old_path = get_config_directory( ) / ( old_name + ".json" );
			std::filesystem::path new_path = get_config_directory( ) / ( new_name + ".json" );

			if ( std::filesystem::exists( old_path ) && !std::filesystem::exists( new_path ) ) {
				std::filesystem::rename( old_path, new_path );
				return true;
			}
			return false;
		}

		bool export_config( const std::string& name, const std::string& path ) {
			std::filesystem::path export_path = path;
			if ( export_path.extension( ) != ".json" ) {
				export_path += ".json";
			}
			return json::save( export_path.string( ) );
		}

		bool import_config( const std::string& path ) {
			std::filesystem::path import_path = path;
			if ( !std::filesystem::exists( import_path ) ) {
				return false;
			}
			return json::load( import_path.string( ) );
		}

		std::string generate_new_config_name( ) {
			get_available_configs( );

			int highest_num = 0;
			for ( const auto& config_name : m_available_configs ) {
				if ( config_name.rfind( "Config ", 0 ) == 0 ) { // Check if it starts with "Config "
					try {
						int num = std::stoi( config_name.substr( 7 ) ); // Extract the number after "Config "
						if ( num > highest_num ) {
							highest_num = num;
						}
					} catch ( const std::invalid_argument& e ) {
						// Not a valid number, ignore
					} catch ( const std::out_of_range& e ) {
						// Number out of range, ignore
					}
				}
			}
			return "Config " + std::to_string( highest_num + 1 );
		}

		void reset( ) {

		}

		void open_config_folder( ) {
			auto config_dir = get_config_directory( );
			ShellExecuteA( NULL, "open", config_dir.string( ).c_str( ), NULL, NULL, SW_SHOWNORMAL );
		}

	private:
		std::vector<std::string> m_available_configs;

		std::filesystem::path get_config_directory( ) {
			char exe_path[ MAX_PATH ];
			GetModuleFileNameA( NULL, exe_path, MAX_PATH );
			std::filesystem::path exe_dir = std::filesystem::path( exe_path ).parent_path( );

			return exe_dir / "configs";
		}
	};
}