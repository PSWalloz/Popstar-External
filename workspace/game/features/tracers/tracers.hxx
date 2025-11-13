namespace tracers {
    enum class tracer_style_t {
        SOLID = 0,
        DASHED = 1,
        GLOWING = 2,
        RAINBOW = 3
    };

    enum class tracer_fade_t {
        LINEAR = 0,
        EXPONENTIAL = 1,
        EASE_OUT = 2,
        PULSE = 3
    };

    struct tracer_t {
        engine::f_vector m_position;
        std::chrono::steady_clock::time_point m_timestamp;
        float m_duration;
        float m_update_rate;
        std::chrono::steady_clock::time_point m_last_update;
        float m_velocity = 0.0f;
    };

    class c_tracers {
    private:
        std::vector<tracer_t> m_traces;

        float calculate_velocity( const engine::f_vector& curr, const engine::f_vector& prev, float delta_time ) {
            if ( delta_time <= 0.0f ) return 0.0f;
            float distance = sqrtf(
                powf( curr.x - prev.x, 2 ) +
                powf( curr.y - prev.y, 2 ) +
                powf( curr.z - prev.z, 2 )
            );
            return distance / delta_time;
        }

    public:
        void add_trace( const engine::f_vector& position, float duration, float update_rate ) {
            auto now = std::chrono::steady_clock::now( );

            if ( !m_traces.empty( ) ) {
                const auto& last_trace = m_traces.back( );
                auto time_since_last = std::chrono::duration_cast< std::chrono::duration<float> >(
                    now - m_traces.back( ).m_timestamp ).count( );
                if ( time_since_last < update_rate ) return;
            }

            float velocity = 0.0f;
            if ( !m_traces.empty( ) ) {
                const auto& prev_trace = m_traces.back( );
                auto delta_time = std::chrono::duration_cast< std::chrono::duration<float> >(
                    now - prev_trace.m_timestamp ).count( );
                velocity = calculate_velocity( position, prev_trace.m_position, delta_time );
            }

            m_traces.emplace_back( std::move( tracer_t{
                position,
                now,
                duration,
                update_rate,
                now,
                velocity
                } ) );

            cleanup( );
        }

        void cleanup( ) {
            auto now = std::chrono::steady_clock::now( );
            m_traces.erase(
                std::remove_if( m_traces.begin( ), m_traces.end( ),
                    [ & ]( const tracer_t& point ) {
                        auto age = std::chrono::duration_cast< std::chrono::duration<float> >(
                            now - point.m_timestamp ).count( );
                        return age > point.m_duration;
                    }
                ),
                m_traces.end( )
            );
        }

        bool has_valid_traces( ) const {
            return m_traces.size( ) >= 2;
        }

        float get_fade_alpha( float age_ratio, tracer_fade_t fade_type ) {
            switch ( fade_type ) {
            case tracer_fade_t::LINEAR:
                return 1.0f - age_ratio;
            case tracer_fade_t::EXPONENTIAL:
                return powf( 1.0f - age_ratio, 2.0f );
            case tracer_fade_t::EASE_OUT:
                return 1.0f - powf( age_ratio, 3.0f );
            case tracer_fade_t::PULSE:
                return 0.5f + 0.5f * sinf( ( 1.0f - age_ratio ) * 3.14159f * 4.0f );
            default:
                return 1.0f - age_ratio;
            }
        }

        ImColor get_segment_color( const tracer_t& point, float age_ratio, tracer_style_t style,
            ImColor base_color = ImColor( 142, 130, 209, 255 ) ) {
            ImColor color = base_color;

            switch ( style ) {
            case tracer_style_t::RAINBOW: {
                float hue = fmodf( age_ratio * 360.0f + std::chrono::duration_cast< std::chrono::milliseconds >(
                    std::chrono::steady_clock::now( ).time_since_epoch( ) ).count( ) * 0.1f, 360.0f );

                float c = 1.0f;
                float x = 1.0f - fabsf( fmodf( hue / 60.0f, 2.0f ) - 1.0f );
                float r = 0, g = 0, b = 0;
                if ( hue < 60 ) { r = c; g = x; }
                else if ( hue < 120 ) { r = x; g = c; }
                else if ( hue < 180 ) { g = c; b = x; }
                else if ( hue < 240 ) { g = x; b = c; }
                else if ( hue < 300 ) { r = x; b = c; }
                else { r = c; b = x; }
                color = ImColor( ( int )( r * 255 ), ( int )( g * 255 ), ( int )( b * 255 ), 255 );
                break;
            }
            default:
                break;
            }

            return color;
        }

        void render( ImDrawList* draw_list, const ImVec2& start, const ImVec2& end,
            ImColor color, float thickness, tracer_style_t style ) {
            switch ( style ) {
            case tracer_style_t::DASHED: {
                ImVec2 dir = ImVec2( end.x - start.x, end.y - start.y );
                float length = sqrtf( dir.x * dir.x + dir.y * dir.y );
                if ( length < 1.0f ) return;

                dir.x /= length;
                dir.y /= length;

                float dash_length = 8.0f;
                float gap_length = 4.0f;
                float pattern_length = dash_length + gap_length;

                for ( float t = 0; t < length; t += pattern_length ) {
                    float dash_start = t;
                    float dash_end = min( t + dash_length, length );

                    ImVec2 seg_start = ImVec2( start.x + dir.x * dash_start, start.y + dir.y * dash_start );
                    ImVec2 seg_end = ImVec2( start.x + dir.x * dash_end, start.y + dir.y * dash_end );

                    draw_list->AddLine( seg_start, seg_end, color, thickness );
                }
                break;
            }
            case tracer_style_t::GLOWING: {
                for ( int glow_step = 3; glow_step >= 0; --glow_step ) {
                    float glow_thickness = thickness * ( 1.0f + glow_step * 0.5f );
                    ImColor glow_color = color;
                    glow_color.Value.w *= ( 1.0f / ( glow_step + 1 ) ) * 0.4f;
                    draw_list->AddLine( start, end, glow_color, glow_thickness );
                }
                break;
            }
            case tracer_style_t::SOLID:
            case tracer_style_t::RAINBOW:
            default:
                draw_list->AddLine( start, end, color, thickness );
                break;
            }
        }

        const std::vector<tracer_t>& get_traces( ) const {
            return m_traces;
        }
    };
}