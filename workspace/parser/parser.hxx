#pragma once

namespace parser {
	class c_simple_json {
	private:
		std::string m_data;

	public:
		c_simple_json( const std::string& json_data ) : m_data( json_data ) { }

		std::string get_string( const std::string& key ) {
			std::string search_key = "\"" + key + "\":";
			size_t pos = m_data.find( search_key );
			if ( pos == std::string::npos ) return "";

			pos += search_key.length( );
			while ( pos < m_data.length( ) && std::isspace( m_data[ pos ] ) ) pos++;

			if ( pos >= m_data.length( ) || m_data[ pos ] != '"' ) return "";
			pos++;

			size_t end_pos = pos;
			while ( end_pos < m_data.length( ) && m_data[ end_pos ] != '"' ) {
				if ( m_data[ end_pos ] == '\\' ) end_pos++;
				end_pos++;
			}

			return m_data.substr( pos, end_pos - pos );
		}

		int get_int( const std::string& key ) {
			std::string search_key = "\"" + key + "\":";
			size_t pos = m_data.find( search_key );
			if ( pos == std::string::npos ) return 0;

			pos += search_key.length( );
			while ( pos < m_data.length( ) && std::isspace( m_data[ pos ] ) ) pos++;

			std::string num_str;
			while ( pos < m_data.length( ) && ( std::isdigit( m_data[ pos ] ) || m_data[ pos ] == '.' || m_data[ pos ] == '-' ) ) {
				num_str += m_data[ pos++ ];
			}

			if ( num_str.empty( ) ) return 0;
			try {
				if ( num_str == "1000000" ) return -1;
				return std::stoi( num_str );
			}
			catch ( ... ) {
				return 0;
			}
		}

		double get_double( const std::string& key ) {
			std::string search_key = "\"" + key + "\":";
			size_t pos = m_data.find( search_key );
			if ( pos == std::string::npos ) return 0.0;

			pos += search_key.length( );
			while ( pos < m_data.length( ) && std::isspace( m_data[ pos ] ) ) pos++;

			std::string num_str;
			while ( pos < m_data.length( ) && ( std::isdigit( m_data[ pos ] ) || m_data[ pos ] == '.' || m_data[ pos ] == '-' ) ) {
				num_str += m_data[ pos++ ];
			}

			if ( num_str.empty( ) ) return 0.0;
			try {
				return std::stod( num_str );
			}
			catch ( ... ) {
				return 0.0;
			}
		}

		bool get_bool( const std::string& key ) {
			std::string search_key = "\"" + key + "\":";
			size_t pos = m_data.find( search_key );
			if ( pos == std::string::npos ) return false;

			pos += search_key.length( );
			while ( pos < m_data.length( ) && std::isspace( m_data[ pos ] ) ) pos++;

			return m_data.substr( pos, 4 ) == "true";
		}

		c_simple_json get_object( const std::string& key ) {
			std::string search_key = "\"" + key + "\":";
			size_t pos = m_data.find( search_key );
			if ( pos == std::string::npos ) return c_simple_json( "{}" );

			pos += search_key.length( );
			while ( pos < m_data.length( ) && std::isspace( m_data[ pos ] ) ) pos++;

			if ( pos >= m_data.length( ) || m_data[ pos ] != '{' ) return c_simple_json( "{}" );

			int brace_count = 1;
			size_t start_pos = pos;
			pos++;

			while ( pos < m_data.length( ) && brace_count > 0 ) {
				if ( m_data[ pos ] == '{' ) brace_count++;
				else if ( m_data[ pos ] == '}' ) brace_count--;
				pos++;
			}

			return c_simple_json( m_data.substr( start_pos, pos - start_pos ) );
		}

		std::string get_raw_data( ) const { return m_data; }
	};

	class c_base_parser {
	protected:
		std::string strip_tags( const std::string& input ) {
			std::string output;
			bool inside_tag = false;
			for ( char c : input ) {
				if ( c == '<' ) inside_tag = true;
				else if ( c == '>' ) inside_tag = false;
				else if ( !inside_tag ) output += c;
			}
			return output;
		}

		std::string normalize_string( const std::string& s ) {
			std::string result;
			for ( char c : s ) {
				if ( !std::isspace( ( unsigned char )c ) )
					result += std::tolower( ( unsigned char )c );
			}
			return result;
		}

		std::string strip_whitespace( const std::string& s ) {
			size_t start = s.find_first_not_of( " \n\r\t" );
			if ( start == std::string::npos ) return "";
			size_t end = s.find_last_not_of( " \n\r\t" );
			return s.substr( start, end - start + 1 );
		}

		std::string strip_tags_and_whitespace( const std::string& s ) {
			return strip_whitespace( strip_tags( s ) );
		}

		class c_fast_http_client {
		private:
			CURL* m_curl_handle;
			std::string m_user_agent;

		public:
			c_fast_http_client( ) {
				curl_global_init( CURL_GLOBAL_DEFAULT );
				m_curl_handle = curl_easy_init( );
				m_user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/137.0.0.0 Safari/537.36";

				if ( m_curl_handle ) {
					curl_easy_setopt( m_curl_handle, CURLOPT_USERAGENT, m_user_agent.c_str( ) );
					curl_easy_setopt( m_curl_handle, CURLOPT_FOLLOWLOCATION, 1L );
					curl_easy_setopt( m_curl_handle, CURLOPT_MAXREDIRS, 3L );
					curl_easy_setopt( m_curl_handle, CURLOPT_TIMEOUT, 10L );
					curl_easy_setopt( m_curl_handle, CURLOPT_CONNECTTIMEOUT, 5L );
					curl_easy_setopt( m_curl_handle, CURLOPT_SSL_VERIFYPEER, 0L );
					curl_easy_setopt( m_curl_handle, CURLOPT_SSL_VERIFYHOST, 0L );
					
					curl_easy_setopt( m_curl_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0 );
					
					curl_easy_setopt( m_curl_handle, CURLOPT_TCP_KEEPALIVE, 1L );
					curl_easy_setopt( m_curl_handle, CURLOPT_TCP_KEEPIDLE, 60L );
					curl_easy_setopt( m_curl_handle, CURLOPT_TCP_KEEPINTVL, 30L );
					
					curl_easy_setopt( m_curl_handle, CURLOPT_ACCEPT_ENCODING, "gzip, deflate, br" );
					curl_easy_setopt( m_curl_handle, CURLOPT_TCP_NODELAY, 1L );
					
					curl_easy_setopt( m_curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 300L );
					curl_easy_setopt( m_curl_handle, CURLOPT_BUFFERSIZE, 102400L );
				}
			}

			~c_fast_http_client( ) {
				if ( m_curl_handle ) {
					curl_easy_cleanup( m_curl_handle );
				}
				curl_global_cleanup( );
			}

			std::string fast_request( const std::string& url, const std::vector<std::string>& headers = {} ) {				
				if ( !m_curl_handle ) return "";

				std::string response;
				curl_easy_setopt( m_curl_handle, CURLOPT_URL, url.c_str( ) );
				curl_easy_setopt( m_curl_handle, CURLOPT_WRITEFUNCTION, write_callback );
				curl_easy_setopt( m_curl_handle, CURLOPT_WRITEDATA, &response );

				struct curl_slist* header_list = nullptr;
				if ( !headers.empty( ) ) {
					for ( const auto& header : headers ) {
						header_list = curl_slist_append( header_list, header.c_str( ) );
					}
					curl_easy_setopt( m_curl_handle, CURLOPT_HTTPHEADER, header_list );
				}

				CURLcode res = curl_easy_perform( m_curl_handle );

				if ( header_list ) {
					curl_slist_free_all( header_list );
				}

				if ( res != CURLE_OK ) {
					response.clear( );
				}

				return response;
			}
		};

		std::string make_winhttp_request( const std::string& url, const std::vector<std::string>& headers = {} ) {
			std::string host, path;
			bool is_https = false;
			
			if ( url.find( "https://" ) == 0 ) {
				is_https = true;
				size_t start = 8; // "https://"
				size_t slash_pos = url.find( '/', start );
				if ( slash_pos != std::string::npos ) {
					host = url.substr( start, slash_pos - start );
					path = url.substr( slash_pos );
				} else {
					host = url.substr( start );
					path = "/";
				}
			} else if ( url.find( "http://" ) == 0 ) {
				size_t start = 7; // "http://"
				size_t slash_pos = url.find( '/', start );
				if ( slash_pos != std::string::npos ) {
					host = url.substr( start, slash_pos - start );
					path = url.substr( slash_pos );
				} else {
					host = url.substr( start );
					path = "/";
				}
			}

			std::wstring w_host( host.begin( ), host.end( ) );
			std::wstring w_path( path.begin( ), path.end( ) );

			HINTERNET h_session = WinHttpOpen( 
				L"FastFortniteClient/1.0",
				WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
				WINHTTP_NO_PROXY_NAME,
				WINHTTP_NO_PROXY_BYPASS,
				0 );

			if ( !h_session ) return "";

			DWORD timeout = 5000;
			WinHttpSetOption( h_session, WINHTTP_OPTION_CONNECT_TIMEOUT, &timeout, sizeof( timeout ) );
			WinHttpSetOption( h_session, WINHTTP_OPTION_SEND_TIMEOUT, &timeout, sizeof( timeout ) );
			WinHttpSetOption( h_session, WINHTTP_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof( timeout ) );

			HINTERNET h_connect = WinHttpConnect( h_session, w_host.c_str( ), 
				is_https ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, 0 );

			if ( !h_connect ) {
				WinHttpCloseHandle( h_session );
				return "";
			}

			DWORD flags = is_https ? WINHTTP_FLAG_SECURE : 0;
			HINTERNET h_request = WinHttpOpenRequest( h_connect, L"GET", w_path.c_str( ), 
				NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags );

			if ( !h_request ) {
				WinHttpCloseHandle( h_connect );
				WinHttpCloseHandle( h_session );
				return "";
			}

			std::wstring all_headers;
			for ( const auto& header : headers ) {
				std::wstring w_header( header.begin( ), header.end( ) );
				all_headers += w_header + L"\r\n";
			}

			if ( !all_headers.empty( ) ) {
				WinHttpAddRequestHeaders( h_request, all_headers.c_str( ), -1, 
					WINHTTP_ADDREQ_FLAG_ADD );
			}

			BOOL result = WinHttpSendRequest( h_request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
				WINHTTP_NO_REQUEST_DATA, 0, 0, 0 );

			std::string response;
			if ( result ) {
				result = WinHttpReceiveResponse( h_request, NULL );
				if ( result ) {
					DWORD bytes_available = 0;
					char buffer[ 8192 ];
					
					do {
						bytes_available = 0;
						WinHttpQueryDataAvailable( h_request, &bytes_available );
						
						if ( bytes_available > 0 ) {
							DWORD bytes_read = 0;
							DWORD to_read = min( bytes_available, sizeof( buffer ) );
							
							if ( WinHttpReadData( h_request, buffer, to_read, &bytes_read ) ) {
								response.append( buffer, bytes_read );
							}
						}
					} while ( bytes_available > 0 );
				}
			}

			WinHttpCloseHandle( h_request );
			WinHttpCloseHandle( h_connect );
			WinHttpCloseHandle( h_session );

			return response;
		}

		static size_t write_callback( void* contents, size_t size, size_t nmemb, std::string* userp ) {
			userp->append( ( char* )contents, size * nmemb );
			return size * nmemb;
		}

		std::string download_html_to_memory( const std::string& url ) {
			return make_winhttp_request( url );
		}

		std::vector<unsigned char> download_image_to_memory( const std::string& url ) {
			std::string response = make_winhttp_request( url );
			std::vector<unsigned char> image_data( response.begin( ), response.end( ) );
			return image_data;
		}

		void* create_d3d11_texture_from_memory( const std::vector<unsigned char>& image_data ) {
			if ( image_data.empty( ) ) return nullptr;

			auto drawing = g_overlay.get( );
			if ( !drawing || !drawing->m_d3d_device ) return nullptr;

			ID3D11ShaderResourceView* texture_view = nullptr;
			D3DX11_IMAGE_LOAD_INFO load_info;
			ZeroMemory( &load_info, sizeof( load_info ) );
			load_info.Width = D3DX11_DEFAULT;
			load_info.Height = D3DX11_DEFAULT;
			load_info.Depth = D3DX11_DEFAULT;
			load_info.FirstMipLevel = D3DX11_DEFAULT;
			load_info.MipLevels = 1;
			load_info.Usage = D3D11_USAGE_DEFAULT;
			load_info.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			load_info.CpuAccessFlags = 0;
			load_info.MiscFlags = 0;
			load_info.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			load_info.Filter = D3DX11_FILTER_LINEAR;
			load_info.MipFilter = D3DX11_FILTER_LINEAR;

			HRESULT result = D3DX11CreateShaderResourceViewFromMemory(
				drawing->m_d3d_device,
				image_data.data( ),
				image_data.size( ),
				&load_info,
				nullptr,
				&texture_view,
				nullptr
			);

			if ( SUCCEEDED( result ) && texture_view ) {
				return ( void* )texture_view;
			}

			return nullptr;
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

		std::string extract_attribute_value( const std::string& html_content, const std::string& tag, const std::string& attribute ) {
			size_t tag_start = html_content.find( "<" + tag );
			if ( tag_start == std::string::npos ) return "";

			size_t attr_start = html_content.find( attribute + "=", tag_start );
			if ( attr_start == std::string::npos ) return "";

			attr_start += attribute.length( ) + 1;

			char quote = '"';
			if ( attr_start < html_content.length( ) && ( html_content[ attr_start ] == '"' || html_content[ attr_start ] == '\'' ) ) {
				quote = html_content[ attr_start ];
				attr_start++;
			}
			else {
				quote = ' ';
			}

			size_t attr_end = html_content.find( quote, attr_start );
			if ( attr_end == std::string::npos ) {
				attr_end = html_content.find_first_of( " >", attr_start );
				if ( attr_end == std::string::npos ) return "";
			}

			return html_content.substr( attr_start, attr_end - attr_start );
		}

		std::string extract_content_between_tags( const std::string& html_content, const std::string& start_tag, const std::string& end_tag, size_t start_pos = 0 ) {
			size_t start = html_content.find( start_tag, start_pos );
			if ( start == std::string::npos ) return "";
			
			start += start_tag.length( );
			size_t end = html_content.find( end_tag, start );
			if ( end == std::string::npos ) return "";
			
			return html_content.substr( start, end - start );
		}

		int parse_integer( const std::string& str, int default_value = 0 ) {
			try {
				if ( str == "1000000" ) return -1;
				return std::stoi( str );
			}
			catch ( ... ) {
				return default_value;
			}
		}

		double parse_double( const std::string& str, double default_value = 0.0 ) {
			try {
				return std::stod( str );
			}
			catch ( ... ) {
				return default_value;
			}
		}

	public:
		c_base_parser( ) {
		}

		virtual ~c_base_parser( ) = default;
		virtual bool setup( ) = 0;
	};
}
