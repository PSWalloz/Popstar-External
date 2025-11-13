#pragma once

namespace rainbow {
	extern std::vector<ImColor> colors;
}

namespace particle {
	struct c_particle {
		float m_pos_x, m_pos_y, m_velocity_x, m_velocity_y, m_alpha;
		int m_lifespan, m_seed, m_flag, m_delay;
		float m_alpha_timer;
		c_particle( ) : m_pos_x( 0 ), m_pos_y( 0 ), m_velocity_x( 0 ), m_velocity_y( 0 ), m_alpha( 0.f ), m_lifespan( 0 ), m_seed( 0 ), m_flag( 0 ), m_delay( 0 ), m_alpha_timer( 0.f ) { }
	};

	class c_particle_system {
	public:
		static constexpr int max_particles = 6048;
		c_particle_system( ) = default;

        void add_particle( ImVec2 origin, ImVec2 size, float flag ) {
            // Use consistent RNG
            static std::mt19937 s_gen{ std::random_device{}() };
            std::uniform_real_distribution<float> velx_dist( -0.75f, 0.75f );
            std::uniform_real_distribution<float> vely_dist( flag ? -2.0f : -0.5f, flag ? -1.0f : 0.25f );
            std::uniform_int_distribution<int> life_dist( 60, 299 );

            c_particle new_particle;
            new_particle.m_pos_x = origin.x + size.x * 0.5f;
            new_particle.m_pos_y = origin.y;
            new_particle.m_velocity_x = velx_dist( s_gen );
            new_particle.m_velocity_y = vely_dist( s_gen );

            new_particle.m_lifespan = life_dist( s_gen );
            if ( new_particle.m_lifespan < 60 ) new_particle.m_lifespan = 60;
            new_particle.m_seed = static_cast<int>( s_gen() );
            new_particle.m_flag = static_cast< int >( flag );
            new_particle.m_alpha = 0.f;
            new_particle.m_alpha_timer = 0.f;

			for ( int i = 0; i < max_particles; i++ ) {
				if ( m_particles[ i ].m_lifespan == 0 ) {
					m_particles[ i ] = new_particle;
					break;
				}
			}
		}

        void render( ImDrawList* draw_list, ImVec2 /*render_size*/, float time_offset ) {
			for ( int i = 0; i < max_particles; i++ ) {
				c_particle& particle = m_particles[ i ];
				if ( particle.m_lifespan ) {
					if ( particle.m_delay ) {
						particle.m_delay--;
					}
					else {
                        const float dt = ImGui::GetIO( ).DeltaTime;
                        // Motion with gravity and damping scaled by dt
                        particle.m_pos_x += particle.m_velocity_x;
                        particle.m_pos_y += particle.m_velocity_y;
                        particle.m_velocity_y += 0.05f * ( dt * 60.0f );
                        const float damping = powf( 0.94525f, dt * 60.0f );
                        particle.m_velocity_x *= damping;
                        particle.m_velocity_y *= damping;
						particle.m_lifespan -= ( particle.m_velocity_y > 0 ) ? 1 : 0;

                        particle.m_alpha = ImLerp( 0.f, 1.f, min( 1.f, particle.m_alpha_timer ) );
                        if ( particle.m_alpha_timer >= 1.f ) {
                            particle.m_alpha = ImLerp( 1.f, 0.f, max( 0.f, particle.m_alpha_timer - 1.f ) );
                        }
                        particle.m_alpha_timer += dt * 1.5f;

						ImVec2 points[ 4 ];
                        float scale = ( 1.5f * 0.5f + 0.1f ) * ( particle.m_flag + 1 );

                        // Deterministic noise based on seed and index
                        float noise = time_offset + ( particle.m_seed % 1000 ) * 0.01f + i * 0.03f;
                        float sin_angle = sinf( noise ) * scale;
                        float cos_angle = cosf( noise ) * scale;

                        for ( int j = 0; j < 4; j++ ) {
                            float angle = 2.0f * IM_PI * j / 4.0f;
                            points[ j ].x = particle.m_pos_x - cos_angle * cosf( angle ) - sin_angle * sinf( angle );
                            points[ j ].y = particle.m_pos_y + sin_angle * cosf( angle ) + cos_angle * sinf( angle );
                        }

						draw_list->AddShadowConvexPoly( points, 4, utils::GetColorWithAlpha( i % 2 == 0 ? ImColor( 75, 87, 219, 255 ) : c::main_color, particle.m_alpha ), 35.f, ImVec2( 0, 0 ) );
						draw_list->AddConvexPolyFilled( points, 4, utils::GetColorWithAlpha( i % 2 == 0 ? ImColor( 75, 87, 219, 255 ) : c::main_color, particle.m_alpha ) );
					}
				}
			}
		}

	private:
		c_particle m_particles[ max_particles ]{};
	};

	// Global instance and pointer to match project conventions and existing usage
	inline c_particle_system g_particle_system;
	inline c_particle_system* g_particle = &g_particle_system;
}