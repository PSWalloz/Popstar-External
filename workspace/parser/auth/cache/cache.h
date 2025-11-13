#pragma once

namespace cache {
	class c_cache {
	private:
	public:
		c_cache( ) :
			m_kill_score( 0 ),
			m_last_points_earned( 0 ),
			m_total_points_earned( 0 ),
			m_player_name( "Unknown" ) { }
		
		~c_cache( ) {

		}

		int m_kill_score;

		int m_last_points_earned;
		int m_total_points_earned;

		std::string m_player_name;

		struct score_event_t {
			std::chrono::system_clock::time_point timestamp;
			int cumulative_points;
		};

		std::vector< score_event_t > m_score_events;
	};
}