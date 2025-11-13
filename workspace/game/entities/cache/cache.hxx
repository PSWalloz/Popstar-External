struct engine_buffer_t {
    struct sound_t {
        engine::u_sound_base* m_battle_bus = nullptr;
    } m_sound;

    lists::c_safe_zone m_zone;

    engine::u_scene_view_state* m_view_state = nullptr;
    engine::u_game_viewport_client* m_viewport_client = nullptr;
    engine::u_athena_game_phase_logic_c* m_game_phase = nullptr;
    engine::a_player_camera_manager* m_player_camera_manager = nullptr;
    engine::a_player_controller* m_player_controller = nullptr;
    engine::u_localplayer* m_local_player = nullptr;
    engine::a_game_state_base* m_game_state = nullptr;
    engine::u_game_instance* m_game_instance = nullptr;
    engine::u_world* m_world = nullptr;

    engine::u_material_interface* m_material = nullptr;
    bool m_applied_fov = false;
    bool m_in_lobby = true;
}; auto g_engine = std::make_unique<engine_buffer_t>( );

struct local_pawn_t {
    engine::a_fort_player_pawn_athena* m_actor = nullptr;
    engine::a_fort_player_state_athena* m_state = nullptr;
    engine::u_skeletal_mesh_component* m_mesh = nullptr;
    engine::a_fort_weapon* m_weapon = nullptr;
	engine::a_fort_athena_vehicle* m_vehicle = nullptr;
    engine::f_vector m_fire_location{};
    engine::f_vector m_fire_direction{};
    float m_projectile_gravity = 0.f;
    float m_projectile_speed = 0.f;
    double m_projectile_distance = 0.f;
    engine::f_vector m_location{};
    engine::f_rotator m_rotation{};
    engine::f_vector m_velocity{};
    engine::e_weapon_core_animation m_weapon_animation{};
}; auto g_local_pawn = std::make_unique<local_pawn_t>( );

struct camera_t {
    engine::f_vector m_location{};
    engine::f_rotator m_rotation{};
    float m_fov = 0.f;
}; auto g_camera = std::make_unique<camera_t>( );

template<typename T>
class double_buffered_cache {
public:
    double_buffered_cache( )
        : m_buffers( std::make_unique<std::array<std::vector<T>, 2>>( ) )
        , m_front_index( 0 ) {
    }

    std::vector<T> get_front_buffer_copy( ) const {
        int front_idx = m_front_index.load( std::memory_order_relaxed );
        return ( *m_buffers )[ front_idx ];
    }

    std::vector<T>& get_back_buffer( ) {
        int front_idx = m_front_index.load( std::memory_order_relaxed );
        return ( *m_buffers )[ 1 - front_idx ];
    }

    void swap_buffers( ) {
        int current_front = m_front_index.load( std::memory_order_relaxed );
        m_front_index.store( 1 - current_front, std::memory_order_release );
    }

    void reset( ) {
        m_buffers = std::make_unique<std::array<std::vector<T>, 2>>( );
        m_front_index = 0;
    }

    void update_buffer( std::vector<T>&& new_data ) {
        auto& back_buffer = get_back_buffer( );
        back_buffer.clear( );
        back_buffer = std::move( new_data );
        swap_buffers( );
    }

    void update_buffer( const std::vector<T>& new_data ) {
        auto& back_buffer = get_back_buffer( );
        back_buffer.clear( );
        back_buffer = new_data;
        swap_buffers( );
    }

    size_t size( ) const {
        int front_idx = m_front_index.load( std::memory_order_relaxed );
        return ( *m_buffers )[ front_idx ].size( );
    }

    bool empty( ) const {
        int front_idx = m_front_index.load( std::memory_order_relaxed );
        return ( *m_buffers )[ front_idx ].empty( );
    }

    bool has_data( ) const {
        return !empty( );
    }

private:
    std::unique_ptr<std::array<std::vector<T>, 2>> m_buffers;
    std::atomic<int> m_front_index;
};
