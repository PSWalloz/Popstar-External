#pragma once
#include <windows.h>

namespace config {
	namespace aimbot {
		const int max_weapon_configs = 5;
		const int max_close_aim = 6;
		int weapon_configs = 0;

		bool enabled = true;
		bool silent_aim[ max_close_aim ] = { false, false, false, false, false, false };
		int hit_chance[ max_close_aim ] = { 90, 90, 90, 90, 90, 90 };
		bool close_aim = false;
		bool shotgun_only = true;
		bool linear_aim[ max_close_aim ] = { false, false, false, false, false, false };
		bool no_target_switch[ max_close_aim ] = { false, false, false, false, false, false };
		bool inverted_vertical_axis[ max_close_aim ] = { false, false, false, false, false, false };
		bool fov_circle[ max_close_aim ] = { true, true, true, true, true, true };
		bool target_line[ max_weapon_configs ] = { true, true, true, true, true };
		bool target_dot[ max_weapon_configs ] = { true, true, true, true, true };
		bool deadzone_dot[ max_weapon_configs ] = { true, true, true, true, true };
		bool prediction_x[ max_weapon_configs ] = { true, true, true, true, true };
		bool aim_curve[ max_weapon_configs ] = { true, true, true, true, true };
		float fov_radius[ max_close_aim ] = { 40, 40, 40, 40, 40, 120 };
		bool deadzone[ max_close_aim ] = { false, false, false, false, false, false };
		bool aim_prediction[ max_weapon_configs ][ 5 ] = { {false}, {false}, {false}, {false}, {false} };
		int smoothing[ max_close_aim ] = { 25, 25, 25, 25, 25, 17 };
		bool aim_bone[ max_close_aim ][ 5 ] = {
			{true, false, false, false, false},
			{true, false, false, false, false},
			{true, false, false, false, false},
			{true, false, false, false, false},
			{true, false, false, false, false},
			{true, false, false, false, false}
		};
		int aim_key[ max_close_aim ] = { VK_RBUTTON, VK_RBUTTON, VK_RBUTTON, VK_RBUTTON, VK_RBUTTON, VK_RBUTTON };
		float fov_color[ 4 ] = { 1.0f, 0.5f, 0.0f, 1.0f };
		bool aim_toggle[ max_close_aim ] = { false, false, false, false, false };
		float restriction[ max_close_aim ] = { 6, 6, 6, 6, 6, 6 };
		bool smart_targeting[ max_weapon_configs ] = { true, true, true, true, true };
		bool override_general[ max_weapon_configs ] = { false, false, false, false, false };
		bool ignore_downed[ max_weapon_configs ] = { true, true, true, true, true };
		bool ignore_invulnerable[ max_weapon_configs ] = { true, true, true, true, true };
		bool ignore_bots[ max_weapon_configs ] = { false, false, false, false, false };
		bool only_visible[ max_weapon_configs ] = { false, false, false, false, false };
		bool aim_acceleration[ max_weapon_configs ] = { false, false, false, false, false };
		float max_velocity[ max_weapon_configs ] = { 70.0f, 70.0f, 70.0f, 70.0f, 70.0f };
		float acceleration[ max_weapon_configs ] = { 50.0f, 50.0f, 50.0f, 50.0f, 50.0f };
		float deceleration[ max_weapon_configs ] = { 60.0f, 60.0f, 60.0f, 60.0f, 60.0f };
		float velocity_threshold[ max_weapon_configs ] = { 0.2f, 0.2f, 0.2f, 0.2f, 0.2f };
		int max_distance[ max_close_aim ] = { 300, 300, 300, 300, 300, 300 };
	}

	namespace triggerbot {
		bool enabled = false;
		bool deadzone = true;
		float deadzone_radius = 6;
		bool deadzone_circular = true;
		int deadzone_width = 5;
		int deadzone_height = 5;
		bool ignore_knocked = true;
		bool weapon_config[ 6 ] = { true, true, true, true, true, true };
		bool fire_key = true;
		bool aim_toggle = false;
		int key_bind = VK_F7;
		int delay = 1;
		int max_distance = 90;
	}

	namespace enemies {
		bool enabled = true;
		int box_esp = 1;
		bool box_style[ 3 ] = { true, true, true };
		int line_esp = 0;
		bool directional_box = false;
		bool skeleton = false;
		bool snaplines = false;
		bool weapon_name = true;
		bool display_name = true;
		bool season_rank = false;
		bool platform = true;
		bool level = false;
		bool damage = true;
		bool movement = true;
		bool weapon_icon = false;
		bool team_mode = true;
		bool kills = true;
		bool fov_arrows = true;
		bool view_line = false;
		bool distance = true;
		bool stencil = false;
		bool reload_bar = true;
		bool wins = false;
		bool winrate = false;
		bool kdr = true;
		int glow_color = 0;
		bool movement_tracers = true;
		bool chinse_hat = false;
		int hat_segments = 23;
		float hat_base_offset = 9.f;
		float hat_height = 22.f;
		float hat_radius = 17.f;
		bool skeleton_type = 0;
		float outline_thickness = 2.0f;
		float max_distance = 200.f;
		float skeleton_thickness = 1.5f;
		bool skeleton_outline = false;
		bool wireframe = false;
		float duration = 1.5f;
		float update = 0.1f;
		float trial_thickness = 1.5f;
		int trial_style = 2;
		int trial_fade = 2;
		bool head_circle = true;
		bool head_hexagon = false;
		float head_size = 8.0f;
	}

	namespace self {
		bool enabled = true;
		bool spectators = true;
		bool inventory = true;
		bool damage = true;
		bool targeting = true;
		bool movement_tracers = false;
		bool bullet_tracers = true;
		bool zone_prediction = false;
		bool draw_next_zone = true;
		bool zone_center = true;
		bool zone_info = true;
		int max_tracers = 5;
		float fadeout_time = 2.0f;
		int zone_shape = 0;
		bool show_next_zone = true;
		bool show_zone_info = true;
		bool center_line = true;
		float zone_thickness = 5.0f;
		float duration = 2.5f;
		float update = 1.5f;
		float trial_thickness = 1.5f;
		int trial_style = 0;
		int trial_fade = 0;
	}

	namespace pickups {
		bool enabled = false;
		bool radar_distance = true;
		bool show_distance = true;
		bool damage = true;
		bool magazine_size = false;
		bool weapon_icon = true;
		bool loot_tier[ 5 ] = { true, true, true, true, true };
		float max_distance = 100.f;
	}

	namespace containers {
		bool enabled = false;
		bool loot_tier = false;
		bool radar_distance = true;
		bool show_distance = true;
		bool container_chams = false;
		int cham_color = 0;
		float max_distance = 70.f;
	}

	namespace weakspots {
		bool enabled = false;
		bool show_distance = true;
		bool weakspot_aimbot = true;
		float smoothing_modifier = 2.f;
		float max_distance = 10.f;
	}

	namespace projectiles {
		bool enabled = false;
		bool show_distance = true;
		bool show_tracers = true;
		bool hit_marker = true;
		float max_distance = 1000.f;
	}

	namespace vehicles {
		bool enabled = false;
		bool terrain = true;
		bool health = true;
		bool show_distance = true;
		float max_distance = 200.f;
	}

	namespace buildings {
		bool enabled = false;
		bool engine_placed = false;
		bool player_placed = true;
		bool build_type = true;
		bool collision_box = true;
		bool show_distance = true;
		float max_distance = 15.f;
	}

	namespace supply_drops {
		bool enabled = false;
		bool show_distance = true;
		bool players_searching = true;
		bool supply_loot = true;
		float max_distance = 350.f;
	}

	namespace llamas {
		bool enabled = false;
		bool show_distance = false;
		float max_distance = 500.f;
	}

	namespace radar {
		bool enabled = true;
		bool distance = true;
		float opacity = 200;
		int position = 1;
		int position_x = 100;
		int position_y = 500;
		int size = 330;
		int range = 100;
		bool grid = true;
		int grid_divisions = 8;
		bool line_of_sight = false;
		bool los_fill = true;
		bool rotation_mode = true;
		bool show_zone = false;
		bool show_next_zone = false;
		bool storm_overlay = true;
		bool zone_out_of_range_indicator = true;
		float indicator_size = 8.0f;
	}

	namespace misc {
		bool enabled = true;
		bool vsync = false;
		bool performance_mode = false;
		bool performance_counter = true;
		bool controller_support = false;
		bool stream_proof = false;
		int actor_update = 10;
		int collection_update = 35;
		int camera_update = 1;
		int engine_update = 5;

		namespace crosshair {
			bool enabled = false;
			float color[ 4 ] = { 1.f, 1.f, 1.f, 1.f };
			float radius = 12.f;
			float thickness = 1.f;
			int num_segments = 12;
			bool outline = true;
		}
	}

	namespace configs {
		bool enabled = true;
	}

	namespace exploits {
		bool enabled = false;
		bool hit_sound = false;
		int sound = 0;
		bool no_spread = false;
		int spread_x = 0;
		int spread_y = 0;
		bool no_recoil = false;
		bool magic_bullet = false;
		int recoil_x = 0;
		int recoil_y = 0;
		int recoil_z = 0;
		bool no_sway = false;
		int sway = 0;
		bool no_bloom = false;
		int bloom = 0;
		bool no_fall_damage = false;
		int fall_damage = 0;
		bool instant_interact = false;
		bool no_pickup_collision = false;
		bool dark_sky = false;
		bool no_fog = false;
		bool shoot_through_walls = false;
		bool desync = false;
		bool player_fly = false;
		bool fov_changer = false;
		float min_fov = 80.f;
		float max_fov = 110.f;
		bool no_collision = false;
		bool firerate_modifier = false;
		int firerate = 999;
		bool no_reload = false;
		float reload_time = 0.f;
		bool infinite_jump = false;
		int jump_time = 5.0f;
		bool infinite_fuel = false;
		bool bike_jump = false;
		bool vehicle_fly = false;
		int key_up = 0x57;
		int key_down = 0x53;
		float max_jump_force = 100;
		float max_jump_forward = 100;
	}

	namespace debug {
		float box_width = 300.0f;
		float box_spacing = 10.0f;
		float start_y = 50.0f;

		float event_timeout = 5.0f;
	}
}