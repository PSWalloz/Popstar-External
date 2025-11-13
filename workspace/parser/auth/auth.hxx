#pragma once

namespace server {
    class c_auth {
    public:
        std::string get_variable( const std::string& variable_name ) {
			std::string variable_url = "https://popstar.loaders.cloud/variable/" + variable_name;

            CURL* curl = curl_easy_init( );
            std::string read_buffer;

            if ( curl ) {
                curl_easy_setopt( curl, CURLOPT_URL, variable_url.c_str( ) );
                curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_callback );
                curl_easy_setopt( curl, CURLOPT_WRITEDATA, &read_buffer );
                curl_easy_setopt( curl, CURLOPT_USERAGENT, "popstar/1.0" );
                CURLcode res = curl_easy_perform( curl );
                curl_easy_cleanup( curl );

                if ( res == CURLE_OK ) {
                    try {
                        auto json = nlohmann::json::parse( read_buffer );
                        return json[ "value" ].get<std::string>( );
                    }
                    catch ( ... ) { }
                }
            }

            return "unknown";
        }

        bool modify_variable( const std::string& new_version, const std::string& variable_name ) {
            std::string variable_url = "https://popstar.loaders.cloud/modify_variable/" + variable_name;

            CURLcode ret;
            CURL* hnd;
            struct curl_slist* headers;

            headers = NULL;
            headers = curl_slist_append( headers, "Content-Type: application/json" );

            std::string json_payload = "{\"value\": \"" + new_version + "\"}";

            hnd = curl_easy_init( );
            if ( hnd ) {
                curl_easy_setopt( hnd, CURLOPT_BUFFERSIZE, 102400L );
                curl_easy_setopt( hnd, CURLOPT_URL, variable_url.c_str( ) );
                curl_easy_setopt( hnd, CURLOPT_NOPROGRESS, 1L );
                curl_easy_setopt( hnd, CURLOPT_POSTFIELDS, json_payload.c_str( ) );
                curl_easy_setopt( hnd, CURLOPT_POSTFIELDSIZE_LARGE, ( curl_off_t )json_payload.length( ) );
                curl_easy_setopt( hnd, CURLOPT_HTTPHEADER, headers );
                curl_easy_setopt( hnd, CURLOPT_USERAGENT, "popstar/1.0" );
                curl_easy_setopt( hnd, CURLOPT_MAXREDIRS, 50L );
                curl_easy_setopt( hnd, CURLOPT_HTTP_VERSION, ( long )CURL_HTTP_VERSION_2TLS );
                curl_easy_setopt( hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L );
                curl_easy_setopt( hnd, CURLOPT_TCP_KEEPALIVE, 1L );

                ret = curl_easy_perform( hnd );

                curl_easy_cleanup( hnd );
                hnd = NULL;
                curl_slist_free_all( headers );
                headers = NULL;

                return ( ret == CURLE_OK );
            }

            if ( headers ) {
                curl_slist_free_all( headers );
            }

            return false;
        }

        bool add_score( std::string username ) {
            CURLcode ret;
            CURL* hnd;
            struct curl_slist* headers;

            auto string = "{\"username\":\"" + username + "\"}";

            headers = NULL;
            headers = curl_slist_append( headers, "Content-Type: application/json" );

            hnd = curl_easy_init( );
            curl_easy_setopt( hnd, CURLOPT_BUFFERSIZE, 102400L );
            curl_easy_setopt( hnd, CURLOPT_URL, "https://popstar.loaders.cloud/leaderboard/add_kill" );
            curl_easy_setopt( hnd, CURLOPT_NOPROGRESS, 1L );
            curl_easy_setopt( hnd, CURLOPT_POSTFIELDS, string.c_str( ) );
            curl_easy_setopt( hnd, CURLOPT_POSTFIELDSIZE_LARGE, ( curl_off_t )string.size( ) );
            curl_easy_setopt( hnd, CURLOPT_HTTPHEADER, headers );
            curl_easy_setopt( hnd, CURLOPT_USERAGENT, "popstar/1.0" );
            curl_easy_setopt( hnd, CURLOPT_MAXREDIRS, 50L );
            curl_easy_setopt( hnd, CURLOPT_HTTP_VERSION, ( long )CURL_HTTP_VERSION_2TLS );
            curl_easy_setopt( hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L );
            curl_easy_setopt( hnd, CURLOPT_TCP_KEEPALIVE, 1L );

            ret = curl_easy_perform( hnd );

            curl_easy_cleanup( hnd );
            hnd = NULL;
            curl_slist_free_all( headers );
            headers = NULL;

            return ( int )ret;
        }

        std::vector<std::pair<std::string, int>> fetch_leaderboard( ) {
            CURL* curl = curl_easy_init( );
            if ( !curl ) return {};

            std::string response;
            struct curl_slist* headers = nullptr;

            try {
                headers = curl_slist_append( headers, "accept: application/json" );
                headers = curl_slist_append( headers, "user-agent: popstar/1.0" );

                curl_easy_setopt( curl, CURLOPT_URL, "https://popstar.loaders.cloud/leaderboard/raw" );
                curl_easy_setopt( curl, CURLOPT_HTTPHEADER, headers );
                curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_callback );
                curl_easy_setopt( curl, CURLOPT_WRITEDATA, &response );
                curl_easy_setopt( curl, CURLOPT_TIMEOUT, 0L );
                curl_easy_setopt( curl, CURLOPT_FOLLOWLOCATION, 1L );

                CURLcode res = curl_easy_perform( curl );
                
                if ( curl ) {
                    curl_easy_cleanup( curl );
                    curl = nullptr;
                }
                if ( headers ) {
                    curl_slist_free_all( headers );
                    headers = nullptr;
                }

                if ( res != CURLE_OK || response.empty() ) {
                    return {};
                }

                std::vector<std::pair<std::string, int>> leaderboard;
                
                if ( !response.empty() ) {
                    auto parsed = nlohmann::json::parse( response );
                    if ( parsed.is_array() ) {
                        for ( const auto& entry : parsed ) {
                            if ( entry.is_object() && entry.contains("username") && entry.contains("kills") ) {
                                std::string username = entry["username"].get<std::string>();
                                int kills = entry["kills"].get<int>();
                                if ( !username.empty() ) {
                                    leaderboard.emplace_back( username, kills );
                                }
                            }
                        }
                    }
                }
                
                return leaderboard;
            }
            catch ( ... ) {
                if ( curl ) {
                    curl_easy_cleanup( curl );
                }
                if ( headers ) {
                    curl_slist_free_all( headers );
                }
                return {};
            }
        }

    private:
        static size_t write_callback( void* contents, size_t size, size_t nmemb, void* userp ) {
            ( ( std::string* )userp )->append( ( char* )contents, size * nmemb );
            return size * nmemb;
        }
    };
}