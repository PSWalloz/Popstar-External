namespace lists {
	class c_inventory {
	private:
	public:
		c_inventory( ) = default;

		engine::u_fort_item_definition* m_item_definition;
		engine::e_fort_rarity m_weapon_rarity;
		std::string m_weapon_name;

		int get_magazine_size( ) const {
			return g_weapon_parser->get_weapon_magazine_size( m_weapon_name, m_weapon_rarity );
		}

		int get_damage( ) const {
			return g_weapon_parser->get_weapon_damage( m_weapon_name, m_weapon_rarity );
		}
	};

	class c_safe_zone {
	private:
	public:
		c_safe_zone( ) = default;

		engine::a_fort_safe_zone_indicator* m_indicator;
		float m_radius;
		float m_next_radius;
		engine::f_vector m_center;
		engine::f_vector m_next_center;
		float m_damage;
		float m_time_remaining;
		std::uint32_t m_phase;
		std::uint32_t m_phase_count;
		float m_height;
	};

	class c_spectator {
	private:
	public:
		c_spectator( ) = default;

		engine::a_player_state* m_state;
		std::uint32_t m_index;
		std::string m_platform_name;
		std::string m_spectator_name;
	};

	class c_bullet_tracer {
	private:
	public:
		c_bullet_tracer( ) = default;

		engine::f_vector m_start_location;
		engine::f_vector m_end_location;
		double m_impact_angle;
		float m_lifetime;
		float m_alpha;
		bool m_impacting;
		std::vector<engine::f_vector> m_trail_points;
	};

	class c_loot_llama {
	private:
	public:
		c_loot_llama( ) = default;

		engine::a_athena_creative_spawn_drop_c* m_actor;
		engine::u_scene_component* m_root_component;
		engine::f_vector m_world_location;
		std::string m_class_name;
		float m_distance;

		bool is_valid( ) const {
			return this->m_actor != nullptr;
		}
	};

	class c_supply_drop {
	private:
	public:
		c_supply_drop( ) = default;

		engine::a_fort_athena_supply_drop* m_actor;
		engine::u_scene_component* m_root_component;
		engine::f_vector m_world_location;
		std::string m_loot_name;
		int m_players_interacting;
		float m_distance;

		bool is_valid( ) const {
			return this->m_actor != nullptr;
		}
	};

	class c_building {
	private:
	public:
		c_building( ) = default;

		engine::a_building_actor* m_actor;
		engine::u_scene_component* m_root_component;
		engine::f_vector m_world_location;
		engine::u_mesh_component* m_mesh_component;
		engine::e_fort_building_type m_building_type;
		engine::fbox_sphere_bounds m_bounds;
		float m_distance;

		bool is_valid( ) const {
			return this->m_actor != nullptr;
		}
	};

	class c_vehicle {
	private:
	public:
		c_vehicle( ) = default;

		engine::a_fort_athena_vehicle* m_actor;
		engine::u_scene_component* m_root_component;
		engine::f_vector m_world_location;
		bool m_on_road;
		bool m_on_landscape;
		bool m_on_dirt;
		bool m_on_grass;
		bool m_on_ice;
		bool m_on_snow;
		bool m_on_mud;
		bool m_exploded;
		float m_critical_health;
		float m_distance;

		bool is_valid( ) const {
			return this->m_actor != nullptr;
		}
	};

	class c_projectile {
	private:
	public:
		c_projectile( ) = default;

		engine::a_fort_projectile_athena* m_actor;
		engine::u_scene_component* m_root_component;
		engine::f_hit_result m_pawn_hit;
		engine::f_hit_result m_cached_impact;
		engine::f_vector m_base_location;
		engine::f_vector m_start_location;
		engine::f_vector m_impact_location;
		bool m_penetrating_pawn;
		float m_impact_distance;
		float m_distance;

		bool is_valid( ) const {
			return this->m_actor != nullptr;
		}
	};

	class c_weakspot {
	private:
	public:
		c_weakspot( ) = default;

		engine::a_building_weakspot* m_actor;
		engine::u_scene_component* m_root_component;
		engine::u_fort_item_definition* m_item_definition;
		engine::f_vector m_world_location;
		engine::f_vector2d m_world_screen;
		float m_distance;

		bool is_valid( ) const {
			return this->m_actor != nullptr;
		}
	};

	class c_container {
	private:
	public:
		c_container( ) = default;

		engine::a_building_container* m_actor;
		engine::u_mesh_component* m_mesh_component;
		engine::e_fort_pickup_spawn_source m_spawn_source;
		engine::u_scene_component* m_root_component;
		engine::f_vector m_world_location;
		float m_distance;

		bool is_valid( ) const {
			return this->m_actor != nullptr;
		}
	};

	class c_pickup {
	private:
	public:
		c_pickup( ) = default;

		engine::a_fort_pickup* m_actor;
		engine::u_scene_component* m_root_component;
		engine::u_fort_item_definition* m_item_definition;
		engine::f_vector m_world_location;
		engine::e_fort_rarity m_rarity;
		std::string m_pickup_name;
		float m_distance;

		int get_magazine_size( ) const {
			return g_weapon_parser->get_weapon_magazine_size( m_pickup_name, m_rarity );
		}

		int get_damage( ) const {
			return g_weapon_parser->get_weapon_damage( m_pickup_name, m_rarity );
		}

		void* get_texture( ) const {
			return g_weapon_parser->get_weapon_texture( m_pickup_name, m_rarity );
		}

		bool is_valid( ) const {
			return this->m_actor != nullptr;
		}
	};

	class c_player {
	private:
	public:
		c_player( ) = default;

		std::uint32_t m_player_id;
		engine::a_fort_player_pawn_athena* m_actor;
		engine::a_fort_player_state_athena* m_state;
		engine::a_fort_weapon* m_weapon;
		engine::a_actor* m_vehicle;
		engine::u_skeletal_mesh_component* m_mesh;
		engine::f_fort_base_weapon_stats* m_weapon_stats;
		engine::u_fort_weapon_item_definition* m_weapon_data;
		engine::u_fort_player_state_component_habanero* m_habanero_component;
		engine::tarray< engine::u_skeletal_mesh_component* > m_mesh_components;
		engine::tarray< engine::u_material_instance_dynamic* > m_pawn_materials;
		engine::u_character_movement_component* m_movement;
		engine::u_scene_component* m_root_component;
		engine::a_player_controller* m_controller;
		engine::e_movement_mode m_movement_mode;
		engine::f_rotator m_player_rotation;
		engine::f_vector m_player_location;
		engine::f_vector m_velocity;
		std::string m_platform_name;
		std::string m_player_name;
		std::uint32_t m_weapon_ammo;
		std::uint32_t m_reserve_ammo;
		bool m_is_reloading;
		std::uint32_t m_player_level;
		std::uint32_t m_player_kills;
		std::uint32_t m_player_rank;
		engine::e_fort_rarity m_weapon_rarity;
		std::string m_weapon_name;
		engine::tarray<engine::f_transform> m_bone_array;
		engine::f_vector m_base_location;
		engine::f_vector m_head_location;
		engine::f_vector m_camera_location;
		engine::f_rotator m_camera_rotation;
		int m_wins;
		int m_score;
		int m_matches;
		int m_team_size;
		double m_winrate;
		double m_kd;
		bool m_is_downed;
		bool m_is_bot;
		bool m_is_teammate;
		bool m_is_visible;
		bool m_is_crouched;
		bool m_is_sliding;
		bool m_is_targeting;
		bool m_is_skydiving;
		float m_distance;
		engine::fbox_sphere_bounds m_bounds;
		std::vector<std::pair<engine::f_vector, engine::f_vector>> m_skeleton_pairs;

		int get_magazine_size( ) const {
			return g_weapon_parser->get_weapon_magazine_size( m_weapon_name, m_weapon_rarity );
		}

		int get_damage( ) const {
			return g_weapon_parser->get_weapon_damage( m_weapon_name, m_weapon_rarity );
		}

		void* get_texture( ) const {
			return g_weapon_parser->get_weapon_texture( m_weapon_name, m_weapon_rarity );
		}

		bool is_valid( ) const {
			return this->m_actor != nullptr;
		}
	};
}