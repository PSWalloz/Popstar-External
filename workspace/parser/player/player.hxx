#pragma once

namespace parser {
	struct game_mode_stats_t {
		int m_wins = 0;
		int m_kills = 0;
		int m_matches = 0;
		int m_minutes = 0;
		int m_score = 0;
		int m_players_outlived = 0;
		double m_kd = 0.0;
		double m_winrate = 0.0;
	};

	struct player_stats_t {
		std::string m_player_name;
		std::string m_account_id;
		int m_current_season = 0;
		int m_current_level = 0;
		double m_season_progress = 0.0;
		
		std::map<std::string, game_mode_stats_t> m_game_modes;
		
		int m_total_wins = 0;
		int m_total_kills = 0;
		int m_total_matches = 0;
		int m_total_minutes = 0;
		int m_total_score = 0;
		double m_overall_kd = 0.0;
		double m_overall_winrate = 0.0;

		std::string m_best_winrate_mode;
		std::string m_best_kd_mode;
		std::string m_most_wins_mode;
		double m_best_winrate_value = 0.0;
		double m_best_kd_value = 0.0;
		int m_most_wins_value = 0;
	};

	class c_player_parser : public c_base_parser {
	public:
		bool setup( ) override { return true; }

		player_stats_t* get_player_stats( const std::string& username ) {
			if ( username.empty( ) ) {
				return {};
			}

			std::string account_id = lookup_account_id( username );
			if ( account_id.empty( ) ) {
				return {};
			}

			auto stats = fetch_player_stats( account_id );
			if ( !stats || stats->m_player_name.empty( ) ) {
				return {};
			}

			return stats.get( );
		}

	private:
		std::string url_encode( const std::string& str ) {
			std::string encoded;
			for ( char c : str ) {
				if ( std::isalnum( c ) || c == '-' || c == '_' || c == '.' || c == '~' ) {
					encoded += c;
				} else if ( c == ' ' ) {
					encoded += "%20";
				} else {
					encoded += '%';
					encoded += "0123456789ABCDEF"[ ( ( unsigned char )c ) >> 4 ];
					encoded += "0123456789ABCDEF"[ ( ( unsigned char )c ) & 15 ];
				}
			}
			return encoded;
		}

		std::string make_api_request( const std::string& url ) {
			std::vector<std::string> headers = {
				"accept: application/json, text/plain, */*",
				"accept-language: en-US,en;q=0.8",
				"authorization: 5b2ab460-0b34b085-7c8068a4-39411b8d",
				"cache-control: no-cache",
				"origin: https://www.fortniteplayerstats.com",
				"pragma: no-cache",
				"priority: u=1, i",
				"referer: https://www.fortniteplayerstats.com/",
				"sec-ch-ua: \"Brave\";v=\"137\", \"Chromium\";v=\"137\", \"Not/A)Brand\";v=\"24\"",
				"sec-ch-ua-mobile: ?0",
				"sec-ch-ua-platform: \"Windows\"",
				"sec-fetch-dest: empty",
				"sec-fetch-mode: cors",
				"sec-fetch-site: cross-site",
				"sec-gpc: 1"
			};

			return make_winhttp_request( url, headers );
		}

		std::string lookup_account_id( const std::string& username ) {
			std::string encoded_username = url_encode( username );
			std::string url = "https://fortniteapi.io/v1/lookup?username=" + encoded_username;
			
			std::string response = make_api_request( url );
			if ( response.empty( ) ) {
				return "";
			}
			
			c_simple_json json( response );
			
			std::string error = json.get_string( "error" );
			if ( !error.empty( ) || response.find( "INVALID_API_KEY" ) != std::string::npos ) {
				return "";
			}
			
			bool result = json.get_bool( "result" );
			if ( !result ) {
				return "";
			}
			
			return json.get_string( "account_id" );
		}

		std::unique_ptr<player_stats_t> fetch_player_stats( const std::string& account_id ) {
			auto stats = std::make_unique<player_stats_t>( );
			
			std::string url = "https://fortniteapi.io/v1/stats?account=" + account_id;
			std::string response = make_api_request( url );
			if ( response.empty( ) ) {
				return nullptr;
			}
			
			c_simple_json json( response );
			
			std::string error = json.get_string( "error" );
			if ( !error.empty( ) ) {
				return nullptr;
			}
			
			stats->m_player_name = json.get_string( "name" );
			stats->m_account_id = account_id;
			
			if ( stats->m_player_name.empty( ) ) {
				return nullptr;
			}
			
			c_simple_json account = json.get_object( "account" );
			stats->m_current_season = account.get_int( "season" );
			stats->m_current_level = account.get_int( "level" );
			stats->m_season_progress = account.get_double( "process_pct" );
			
			c_simple_json global_stats = json.get_object( "global_stats" );
			std::vector<std::string> modes = { "solo", "duo", "trio", "squad" };
			
			for ( const std::string& mode : modes ) {
				c_simple_json mode_stats = global_stats.get_object( mode );
				if ( !mode_stats.get_raw_data( ).empty( ) && mode_stats.get_raw_data( ) != "{}" ) {
					game_mode_stats_t game_mode;
					game_mode.m_wins = mode_stats.get_int( "placetop1" );
					game_mode.m_kills = mode_stats.get_int( "kills" );
					game_mode.m_matches = mode_stats.get_int( "matchesplayed" );
					game_mode.m_minutes = mode_stats.get_int( "minutesplayed" );
					game_mode.m_score = mode_stats.get_int( "score" );
					game_mode.m_players_outlived = mode_stats.get_int( "playersoutlived" );
					game_mode.m_kd = mode_stats.get_double( "kd" );
					game_mode.m_winrate = mode_stats.get_double( "winrate" );
					
					if ( game_mode.m_matches > 0 || game_mode.m_kills > 0 ) {
						stats->m_game_modes[ mode ] = game_mode;
						
						stats->m_total_wins += game_mode.m_wins;
						stats->m_total_kills += game_mode.m_kills;
						stats->m_total_matches += game_mode.m_matches;
						stats->m_total_minutes += game_mode.m_minutes;
						stats->m_total_score += game_mode.m_score;
					}
				}
			}
			
			if ( stats->m_total_matches > 0 ) {
				stats->m_overall_winrate = ( double )stats->m_total_wins / stats->m_total_matches;
				int total_deaths = stats->m_total_matches - stats->m_total_wins;
				if ( total_deaths > 0 ) {
					stats->m_overall_kd = ( double )stats->m_total_kills / total_deaths;
				}
			}

			for ( const auto& mode : stats->m_game_modes ) {
				if ( mode.second.m_winrate > stats->m_best_winrate_value ) {
					stats->m_best_winrate_value = mode.second.m_winrate;
					stats->m_best_winrate_mode = mode.first;
				}

				if ( mode.second.m_kd > stats->m_best_kd_value ) {
					stats->m_best_kd_value = mode.second.m_kd;
					stats->m_best_kd_mode = mode.first;
				}

				if ( mode.second.m_wins > stats->m_most_wins_value ) {
					stats->m_most_wins_value = mode.second.m_wins;
					stats->m_most_wins_mode = mode.first;
				}
			}

			return stats;
		}
	};
}
