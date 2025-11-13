#pragma once
#include "../parser.hxx"
#include <filesystem>
#include <fstream>

namespace parser {
	const std::string CACHE_DIR = "cache/weapons/";
	const std::string WEAPON_DB_FILE = CACHE_DIR + "weapon_db.json";
	const std::string TEXTURE_CACHE_DIR = CACHE_DIR + "textures/";

	struct weapon_info_t {
		std::string name;
		engine::e_fort_rarity rarity;
		int magazine_size;
		int damage;
		std::string icon_url;

		nlohmann::json to_json() const {
			return {
				{"name", name},
				{"rarity", static_cast<int>(rarity)},
				{"magazine_size", magazine_size},
				{"damage", damage},
				{"icon_url", icon_url}
			};
		}

		static weapon_info_t from_json(const nlohmann::json& j) {
			weapon_info_t info;
			info.name = j["name"].get<std::string>();
			info.rarity = static_cast<engine::e_fort_rarity>(j["rarity"].get<int>());
			info.magazine_size = j["magazine_size"].get<int>();
			info.damage = j["damage"].get<int>();
			info.icon_url = j["icon_url"].get<std::string>();
			return info;
		}
	};

	struct download_performance_t {
		int total_downloads = 0;
		int successful_downloads = 0;
		int failed_downloads = 0;
		float total_download_time = 0.0f;
		float total_bytes_downloaded = 0.0f;
		float average_download_speed = 0.0f;
		float current_download_progress = 0.0f;
		std::string current_downloading_item = "";
		std::chrono::steady_clock::time_point session_start_time;
		std::chrono::steady_clock::time_point last_download_time;

		void reset( ) {
			total_downloads = 0;
			successful_downloads = 0;
			failed_downloads = 0;
			total_download_time = 0.0f;
			total_bytes_downloaded = 0.0f;
			average_download_speed = 0.0f;
			current_download_progress = 0.0f;
			current_downloading_item = "";
			session_start_time = std::chrono::steady_clock::now( );
			last_download_time = session_start_time;
		}

		void start_download( const std::string& item_name ) {
			current_downloading_item = item_name;
			last_download_time = std::chrono::steady_clock::now( );
			total_downloads++;
		}

		void finish_download( bool success, size_t bytes_downloaded ) {
			auto now = std::chrono::steady_clock::now( );
			auto download_duration = std::chrono::duration_cast< std::chrono::milliseconds >( now - last_download_time ).count( ) / 1000.0f;

			total_download_time += download_duration;
			total_bytes_downloaded += bytes_downloaded / 1024.0f;

			if ( success ) {
				successful_downloads++;
			}
			else {
				failed_downloads++;
			}

			if ( total_download_time > 0.0f ) {
				average_download_speed = total_bytes_downloaded / total_download_time;
			}

			current_downloading_item = "";
			current_download_progress = 0.0f;
		}

		float get_success_rate( ) const {
			return total_downloads > 0 ? ( float )successful_downloads / total_downloads * 100.0f : 0.0f;
		}

		float get_session_duration( ) const {
			auto now = std::chrono::steady_clock::now( );
			return std::chrono::duration_cast< std::chrono::seconds >( now - session_start_time ).count( );
		}
	};

	const std::map<std::string, engine::e_fort_rarity> m_rarity_map = {
		{"common", engine::e_fort_rarity::common},
		{"uncommon", engine::e_fort_rarity::uncommon},
		{"rare", engine::e_fort_rarity::rare},
		{"epic", engine::e_fort_rarity::epic},
		{"legendary", engine::e_fort_rarity::legendary},
		{"mythic", engine::e_fort_rarity::mythic},
		{"exotic", engine::e_fort_rarity::transcendent}
	};

	class c_weapon_parser : public c_base_parser {
	public:
		c_weapon_parser( ) {
			m_performance.reset( );
			ensure_cache_directories();
		}

		bool setup( ) override {
			m_performance.reset( );

			if (load_from_cache()) {
				logging::print(encrypt("Loaded weapon data from cache"));
				load_all_weapon_textures();
				return !m_weapons.empty();
			}

			const std::string weapons_url = "https://fortnite.gg/weapons";

			m_performance.start_download( "Weapon Database HTML" );
			std::string html_content = download_html_to_memory( weapons_url );
			m_performance.finish_download( !html_content.empty( ), html_content.size( ) );

			if ( html_content.empty( ) ) {
				logging::print( encrypt( "Failed to download weapon data from fortnite.gg" ) );
				return false;
			}

			m_weapons = parse_weapons_from_html( html_content );
			build_weapon_database( );

			save_to_cache();

			load_all_weapon_textures( );
			return !m_weapons.empty( );
		}

		//void get_performance_debug_info( debug::debug_info_t& perf_info ) {
			//perf_info.type = debug::DEBUG_TYPE_PERFORMANCE;
			//perf_info.header = "Parsing Performance";
			//perf_info.metrics.clear( );

			//perf_info.metrics[ "Avg KBs" ] = m_performance.average_download_speed;
			//perf_info.metrics[ "Total KBs" ] = m_performance.total_bytes_downloaded;
			//perf_info.metrics[ "Session Time" ] = m_performance.get_session_duration( );

			//if ( !m_performance.current_downloading_item.empty( ) ) {
			//	perf_info.metrics[ "Current Progress %" ] = m_performance.current_download_progress;
			//}
		//}

		bool is_downloading( ) const {
			return !m_performance.current_downloading_item.empty( );
		}

		const std::string& get_current_download_item( ) const {
			return m_performance.current_downloading_item;
		}

		float get_current_download_progress( ) const {
			return m_performance.current_download_progress;
		}

		const download_performance_t& get_performance_stats( ) const {
			return m_performance;
		}

		void build_weapon_database( ) {
			m_weapon_database.clear( );
			std::set<std::tuple<std::string, engine::e_fort_rarity>> unique_entries;

			for ( const auto& weapon : m_weapons ) {
				auto key = std::make_tuple( weapon.name, weapon.rarity );
				if ( unique_entries.find( key ) == unique_entries.end( ) ) {
					unique_entries.insert( key );
					m_weapon_database[ weapon.name ][ weapon.rarity ] = weapon;
				}
			}
		}

		weapon_info_t* get_weapon_info( const std::string& weapon_name, engine::e_fort_rarity rarity ) {
			auto weapon_it = m_weapon_database.find( weapon_name );
			if ( weapon_it != m_weapon_database.end( ) ) {
				auto rarity_it = weapon_it->second.find( rarity );
				if ( rarity_it != weapon_it->second.end( ) ) {
					return &rarity_it->second;
				}
			}
			return nullptr;
		}

		int get_weapon_damage( const std::string& weapon_name, engine::e_fort_rarity rarity ) {
			auto weapon_info = get_weapon_info( weapon_name, rarity );
			return weapon_info ? weapon_info->damage : 0;
		}

		int get_weapon_magazine_size( const std::string& weapon_name, engine::e_fort_rarity rarity ) {
			auto weapon_info = get_weapon_info( weapon_name, rarity );
			return weapon_info ? weapon_info->magazine_size : 0;
		}

		std::string get_weapon_icon_url( const std::string& weapon_name, engine::e_fort_rarity rarity ) {
			auto weapon_info = get_weapon_info( weapon_name, rarity );
			if ( weapon_info && !weapon_info->icon_url.empty( ) ) {
				if ( weapon_info->icon_url.substr( 0, 1 ) == "/" ) {
					return "https://fortnite.gg" + weapon_info->icon_url;
				}
				return weapon_info->icon_url;
			}
			return "";
		}

		void* get_weapon_texture( const std::string& weapon_name, engine::e_fort_rarity rarity ) {
			auto weapon_it = m_weapon_textures.find( weapon_name );
			if ( weapon_it != m_weapon_textures.end( ) ) {
				auto rarity_it = weapon_it->second.find( rarity );
				if ( rarity_it != weapon_it->second.end( ) ) {
					return rarity_it->second;
				}
			}
			return nullptr;
		}

		void set_weapon_texture( const std::string& weapon_name, engine::e_fort_rarity rarity, void* texture ) {
			m_weapon_textures[ weapon_name ][ rarity ] = texture;
		}

		const std::vector<weapon_info_t>& get_all_weapons( ) const {
			return m_weapons;
		}

		const std::map<std::string, std::map<engine::e_fort_rarity, weapon_info_t>>& get_weapon_database( ) const {
			return m_weapon_database;
		}

		int get_loaded_texture_count( ) const {
			int count = 0;
			for ( const auto& weapon_entry : m_weapon_textures ) {
				count += weapon_entry.second.size( );
			}
			return count;
		}

	private:
		std::vector<weapon_info_t> m_weapons;
		std::map<std::string, std::map<engine::e_fort_rarity, weapon_info_t>> m_weapon_database;
		std::map<std::string, std::map<engine::e_fort_rarity, void*>> m_weapon_textures;
		download_performance_t m_performance;

		engine::e_fort_rarity get_rarity_value( const std::string& rarity_str ) {
			std::string norm = normalize_string( rarity_str );
			auto it = m_rarity_map.find( norm );
			if ( it != m_rarity_map.end( ) ) {
				return it->second;
			}
			return engine::e_fort_rarity::num_rarity_values;
		}

		std::string extract_icon_url( const std::string& html_content ) {
			return extract_attribute_value( html_content, "img", "src" );
		}

		std::vector<std::string> extract_table_cells( const std::string& row_html ) {
			std::vector<std::string> cells;
			size_t pos = 0;
			while ( true ) {
				size_t td_start = row_html.find( "<td", pos );
				if ( td_start == std::string::npos ) break;
				size_t td_end = row_html.find( "</td>", td_start );
				if ( td_end == std::string::npos ) break;
				size_t content_start = row_html.find( ">", td_start );
				if ( content_start == std::string::npos || content_start > td_end ) break;
				content_start += 1;
				std::string cell_content = row_html.substr( content_start, td_end - content_start );
				cells.push_back( cell_content );
				pos = td_end + 5;
			}
			return cells;
		}

		void ensure_cache_directories() {
			std::filesystem::create_directories(CACHE_DIR);
			std::filesystem::create_directories(TEXTURE_CACHE_DIR);
		}

		bool load_from_cache() {
			try {
				if (!std::filesystem::exists(WEAPON_DB_FILE)) {
					return false;
				}

				std::ifstream file(WEAPON_DB_FILE);
				if (!file.is_open()) {
					return false;
				}

				nlohmann::json j;
				file >> j;

				m_weapons.clear();
				for (const auto& weapon_json : j) {
					m_weapons.push_back(weapon_info_t::from_json(weapon_json));
				}

				build_weapon_database();
				return true;
			}
			catch (const std::exception& e) {
				logging::print(encrypt("Failed to load weapon cache: %s"), e.what());
				return false;
			}
		}

		void save_to_cache() {
			try {
				nlohmann::json j = nlohmann::json::array();
				for (const auto& weapon : m_weapons) {
					j.push_back(weapon.to_json());
				}

				std::ofstream file(WEAPON_DB_FILE);
				file << j.dump(4);
			}
			catch (const std::exception& e) {
				logging::print(encrypt("Failed to save weapon cache: %s"), e.what());
			}
		}

		std::string get_texture_cache_path(const std::string& weapon_name, engine::e_fort_rarity rarity) {
			std::string sanitized_name = weapon_name;
			std::replace(sanitized_name.begin(), sanitized_name.end(), ' ', '_');
			std::replace(sanitized_name.begin(), sanitized_name.end(), '/', '_');
			return TEXTURE_CACHE_DIR + sanitized_name + "_" + std::to_string(static_cast<int>(rarity)) + ".dds";
		}

		void load_all_weapon_textures( ) {
			std::vector<std::pair<std::string, engine::e_fort_rarity>> texture_jobs;

			for ( const auto& weapon_entry : m_weapon_database ) {
				for ( const auto& rarity_entry : weapon_entry.second ) {
					const auto& weapon_info = rarity_entry.second;

					if ( !weapon_info.icon_url.empty( ) ) {
						texture_jobs.emplace_back( weapon_info.name, weapon_info.rarity );
					}
				}
			}

			if ( texture_jobs.empty( ) ) {
				logging::print( encrypt( "No weapon icons to load" ) );
				return;
			}

			g_transition->begin_operation( "Parsing Weapons", texture_jobs.size( ) );

			int loaded_count = 0;
			int failed_count = 0;

			for ( size_t i = 0; i < texture_jobs.size( ); ++i ) {
				const auto& job = texture_jobs[ i ];
				const auto& weapon_name = job.first;
				const auto& rarity = job.second;

				auto weapon_info = get_weapon_info( weapon_name, rarity );
				if ( !weapon_info ) {
					g_transition->increment_progress( "Skipped: " + weapon_name );
					continue;
				}

				std::string cache_path = get_texture_cache_path(weapon_name, rarity);
				void* texture = nullptr;
				bool loaded_from_cache = false;

				if ( std::filesystem::exists( cache_path ) ) {
					std::vector<uint8_t> cached_data;
					std::ifstream cache_file( cache_path, std::ios::binary );
					if ( cache_file.is_open( ) ) {
						cache_file.seekg( 0, std::ios::end );
						cached_data.resize( cache_file.tellg( ) );
						cache_file.seekg( 0, std::ios::beg );
						cache_file.read( reinterpret_cast< char* >( cached_data.data( ) ), cached_data.size( ) );

						texture = create_d3d11_texture_from_memory( cached_data );
						if ( texture ) {
							loaded_from_cache = true;
						}
					}
				}

				if ( !texture ) {
					std::string full_url = weapon_info->icon_url;
					if ( weapon_info->icon_url.substr( 0, 1 ) == "/" ) {
						full_url = "https://fortnite.gg" + weapon_info->icon_url;
					}

					std::string download_item = std::format( "{} ({})", weapon_name,
						rarity == engine::e_fort_rarity::common ? "Common" :
						rarity == engine::e_fort_rarity::uncommon ? "Uncommon" :
						rarity == engine::e_fort_rarity::rare ? "Rare" :
						rarity == engine::e_fort_rarity::epic ? "Epic" :
						rarity == engine::e_fort_rarity::legendary ? "Legendary" :
						rarity == engine::e_fort_rarity::mythic ? "Mythic" : "Exotic" );

					m_performance.start_download( download_item );
					m_performance.current_download_progress = ( ( float )i / texture_jobs.size( ) ) * 100.0f;


					auto image_bytes = download_image_to_memory( full_url );
					auto download_success = !image_bytes.empty( );
					if ( download_success ) {
						texture = create_d3d11_texture_from_memory( image_bytes );
						if ( texture ) {
							std::ofstream cache_file( cache_path, std::ios::binary );
							if ( cache_file.is_open( ) ) {
								cache_file.write( reinterpret_cast< const char* >( image_bytes.data( ) ), image_bytes.size( ) );
							}
						}
					}

					m_performance.finish_download( download_success, image_bytes.size( ) );
				}

				if (texture) {
					m_weapon_textures[weapon_name][rarity] = texture;
					loaded_count++;
				}
				else {
					failed_count++;
				}

				g_transition->increment_progress(weapon_name + (loaded_from_cache ? " (cached)" : ""));

				g_overlay->new_frame( );
				//debug::debug_info_t download_info;
				//get_performance_debug_info( download_info );

				g_foreground = ImGui::GetForegroundDrawList( );
				g_background = ImGui::GetBackgroundDrawList( );
				c::anim::speed = ImGui::GetIO( ).DeltaTime * 14.f;

				//g_debug->draw( download_info );

				g_transition->render( );
				g_overlay->draw_frame( );
			}

			//g_debug->reset_all( );

			logging::print( encrypt( "Weapon texture loading complete: %d loaded, %d failed" ),
				loaded_count, failed_count );
			logging::print( encrypt( "Download Performance: %.2f KB/s average, %.2f%% success rate" ),
				m_performance.average_download_speed, m_performance.get_success_rate( ) );
		}

		std::vector<weapon_info_t> parse_weapons_from_html( const std::string& html_content ) {
			std::vector<weapon_info_t> weapons;
			size_t pos = 0;

			while ( true ) {
				size_t row_start = html_content.find( "<tr", pos );
				if ( row_start == std::string::npos ) break;
				size_t row_end = html_content.find( "</tr>", row_start );
				if ( row_end == std::string::npos ) break;

				size_t td_start = html_content.find( "<td", pos );
				if ( td_start == std::string::npos || td_start > row_end ) {
					pos = row_end + 5;
					continue;
				}

				size_t rowspan_pos = html_content.find( "rowspan", td_start );
				if ( rowspan_pos == std::string::npos || rowspan_pos > row_end ) {
					pos = row_end + 5;
					continue;
				}

				size_t content_start = html_content.find( ">", td_start ) + 1;
				size_t content_end = html_content.find( "</td>", content_start );
				if ( content_end == std::string::npos ) break;

				std::string name_raw = html_content.substr( content_start, content_end - content_start );
				std::string weapon_name = strip_tags_and_whitespace( name_raw );
				std::string weapon_icon = extract_icon_url( name_raw );

				pos = row_end + 5;

				while ( true ) {
					size_t variant_row_start = html_content.find( "<tr", pos );
					if ( variant_row_start == std::string::npos ) break;
					size_t variant_row_end = html_content.find( "</tr>", variant_row_start );
					if ( variant_row_end == std::string::npos ) break;

					size_t next_td_start = html_content.find( "<td", variant_row_start );
					bool next_weapon_found = false;
					if ( next_td_start != std::string::npos && next_td_start < variant_row_end ) {
						size_t next_rowspan_pos = html_content.find( "rowspan", next_td_start );
						if ( next_rowspan_pos != std::string::npos && next_rowspan_pos < variant_row_end ) {
							next_weapon_found = true;
						}
					}

					if ( next_weapon_found ) break;

					std::string variant_row_html = html_content.substr( variant_row_start, variant_row_end - variant_row_start + 5 );
					auto cells = extract_table_cells( variant_row_html );

					if ( cells.size( ) < 7 ) {
						pos = variant_row_end + 5;
						continue;
					}

					std::string rarity_str = strip_tags_and_whitespace( cells[ 1 ] );
					auto rarity_val = get_rarity_value( rarity_str );

					std::string variant_icon = weapon_icon;
					if ( !cells.empty( ) ) {
						std::string icon_from_variant = extract_icon_url( cells[ 0 ] );
						if ( !icon_from_variant.empty( ) ) {
							variant_icon = icon_from_variant;
						}
					}

					int magazine_size = 0;
					std::string mag_str = strip_tags_and_whitespace( cells[ 6 ] );
					try {
						if ( mag_str == "1000000" ) magazine_size = -1;
						else magazine_size = std::stoi( mag_str );
					}
					catch ( ... ) {
						magazine_size = 0;
					}

					int damage = 0;
					if ( cells.size( ) > 3 ) {
						std::string damage_str = strip_tags_and_whitespace( cells[ 3 ] );
						try {
							damage = std::stoi( damage_str );
						}
						catch ( ... ) {
							damage = 0;
						}
					}

					weapon_info_t weapon_info;
					weapon_info.name = weapon_name;
					weapon_info.rarity = rarity_val;
					weapon_info.magazine_size = magazine_size;
					weapon_info.damage = damage;
					weapon_info.icon_url = variant_icon;

					weapons.push_back( weapon_info );
					pos = variant_row_end + 5;
				}
			}

			return weapons;
		}
	};
}