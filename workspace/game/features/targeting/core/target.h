#pragma once

enum class target_type {
	none,
	player,
	weakspot
};

struct target_t {
	lists::c_player m_player{};
	lists::c_weakspot m_weakspot{};
	engine::f_vector m_aim_location{};
	engine::f_vector2d m_aim_screen{};
	float m_aim_distance = 0.f;
	target_type m_type = target_type::none;

	float m_aimbot_fov = 0.f;
	
	// Target lock functionality
	bool m_target_lock_enabled = false;
	std::uint32_t m_locked_player_id = 0;
	bool m_has_locked_target = false;
} g_target;