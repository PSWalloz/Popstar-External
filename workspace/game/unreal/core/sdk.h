namespace engine {
    struct f_vector;
    struct f_vector2d;
    struct f_rotator;
    class f_string;
    class f_text;
    class a_fort_player_pawn_athena;
    class a_player_state;
    class a_player_controller;
    class f_name;
    class u_object;
    class u_world;
    f_vector2d project( f_vector& world_location );
    f_rotator find_look_at_rotation( f_vector& start, f_vector& target );

    namespace offsets {
        std::uint32_t g_world = 0x173BDD18;
        std::uint32_t g_names = 0x17136200;
        std::uint32_t name_private = 0x18;
        std::uint32_t world_time_seconds = 0x190;
        std::uint32_t levels = 0x1E0;
        std::uint32_t actors = 0x168;
        std::uint32_t player_name = 0xA00;
        std::uint32_t projectile_gravity = 0x2318;
        std::uint32_t projectile_speed = projectile_gravity + 8;
    }

    bool is_valid( const std::uint64_t address ) {
        //auto physical_address = g_driver->paging::translate_linear( address );
        //if ( !physical_address || !g_driver->paging::is_address_valid( physical_address ) )
        //    return false;

        if ( address == 0 || address == 0xCCCCCCCCCCCCCCCC || address == 0xFFFFFFFFFFFFFFFF )
            return false;

        if ( address <= 0x400000 || address > 0x7FFFFFFFFFFFFFFF )
            return false;

        return true;
    }

    enum e_bone {
        root = 0,
        attach,
        pelvis,
        spine_01,
        spine_02,
        spine_03,
        spine_04,
        spine_05,
        clavicle_l,
        upperarm_l,
        lowerarm_l,
        hand_l,
        index_metacarpal_l,
        index_01_l,
        index_02_l,
        index_03_l,
        middle_metacarpal_l,
        middle_01_l,
        middle_02_l,
        middle_03_l,
        pinky_metacarpal_l,
        pinky_01_l,
        pinky_02_l,
        pinky_03_l,
        ring_metacarpal_l,
        ring_01_l,
        ring_02_l,
        ring_03_l,
        thumb_01_l,
        thumb_02_l,
        thumb_03_l,
        weapon_l,
        hand_attach_l,
        lowerarm_twist_01_l,
        lowerarm_twist_02_l,
        upperarm_twist_01_l,
        upperarm_twist_02_l,
        clavicle_r,
        upperarm_r,
        lowerarm_r,
        hand_r,
        index_metacarpal_r,
        index_01_r,
        index_02_r,
        index_03_r,
        middle_metacarpal_r,
        middle_01_r,
        middle_02_r,
        middle_03_r,
        pinky_metacarpal_r,
        pinky_01_r,
        pinky_02_r,
        pinky_03_r,
        ring_metacarpal_r,
        ring_01_r,
        ring_02_r,
        ring_03_r,
        thumb_01_r,
        thumb_02_r,
        thumb_03_r,
        weapon_r,
        hand_attach_r,
        lowerarm_twist_01_r,
        lowerarm_twist_02_r,
        upperarm_twist_01_r,
        upperarm_twist_02_r,
        neck_01,
        neck_02,
        head,
        attach_backpack,
        attach_cape,
        thigh_l,
        calf_l,
        calf_twist_01_l,
        calf_twist_02_l,
        foot_l,
        ball_l,
        thigh_twist_01_l,
        thigh_r,
        calf_r,
        calf_twist_01_r,
        calf_twist_02_r,
        foot_r,
        ball_r,
        thigh_twist_01_r,
        ik_foot_root,
        ik_foot_l,
        ik_foot_r,
        ik_hand_root,
        ik_hand_gun,
        ik_hand_l,
        ik_hand_r,
        attach_emote_01,
        attach_emote_02,
        attach_emote_03,
        warp_point_01,
        warp_point_02,
        warp_point_03,
        root_fp,
        camera_root,
        camera,
        attach_fp,
        vb_spine_05_weapon_r,
        vb_vb_spine_05_weapon_r_ik_hand_gun,
        vb_vb_spine_05_weapon_r_ik_hand_l,
        vb_spine_05_upperarm_r,
        vb_vb_spine_05_upperarm_r_lowerarm_r,
        vb_vb_vb_spine_05_upperarm_r_lowerarm_r_hand_r,
        vb_ik_foot_root_weapon_l,
        vb_root_prop,
        vb_head_fx,
        vb_root_hand_l,
        vb_root_hand_r,
        vb_attach_ik_hand_gun,
        vb_vb_attach_ik_hand_gun_ik_hand_l,
        vb_vb_attach_ik_hand_gun_ik_hand_r,
        vb_spine_05_hand_attach_r,
        vb_vb_spine_05_hand_attach_r_ik_hand_gun,
        vb_vb_spine_05_hand_attach_r_ik_hand_l,
        vb_pelvis_hand_attach_r,
        vb_ik_foot_root_weapon_r,
        vb_ik_hand_root_ik_hand_l,
        vb_attach_ik_foot_l,
        vb_attach_ik_foot_r,
        vb_pelvis_ik_hand_gun,
        vb_pelvis_ik_hand_r,
        vb_pelvis_ik_hand_l,
        vb_pelvis_attach_emote_01,
        vb_pelvis_attach_emote_02,
        vb_pelvis_attach_emote_03,
        vb_ik_foot_root_hand_attach_l,
        vb_ik_foot_root_hand_attach_r
    };

    const std::vector<std::pair<int, int>> skeleton_pairs{
        {e_bone::spine_05, e_bone::neck_01},
        {e_bone::spine_05, e_bone::upperarm_r},
        {e_bone::spine_05, e_bone::upperarm_l},
        {e_bone::upperarm_l, e_bone::lowerarm_l},
        {e_bone::lowerarm_l, e_bone::hand_l},
        {e_bone::upperarm_r, e_bone::lowerarm_r},
        {e_bone::lowerarm_r, e_bone::hand_r},
        {e_bone::spine_05, e_bone::pelvis},
        {e_bone::pelvis, e_bone::thigh_r},
        {e_bone::pelvis, e_bone::thigh_l},
        {e_bone::thigh_r, e_bone::calf_r},
        {e_bone::calf_r, e_bone::ik_foot_r},
        {e_bone::thigh_l, e_bone::calf_l},
        {e_bone::calf_l, e_bone::ik_foot_l}
    };

    enum class e_fort_rarity : std::uint8_t {
        common = 0,
        uncommon = 1,
        rare = 2,
        epic = 3,
        legendary = 4,
        mythic = 5,
        transcendent = 6,
        unattainable = 7,
        num_rarity_values = 8
    };
}