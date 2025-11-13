
namespace server {
    class c_config {
    public:
        void load_value( ) {
            HKEY h_key;
            LONG result = RegOpenKeyExA(
                HKEY_CURRENT_USER,
                "SOFTWARE\\popstar",
                0,
                KEY_READ,
                &h_key
            );

            if ( result != ERROR_SUCCESS )
                throw std::runtime_error( "Failed to open registry key." );

            m_brand = read_string( h_key, "brand" );
            m_key = read_string( h_key, "key" );
            m_level = read_dword( h_key, "level" );

            RegCloseKey( h_key );
        }

        const std::string& get_brand( ) const { return m_brand; }
        const std::string& get_key( ) const { return m_key; }
        DWORD get_level( ) const { return m_level; }

    private:
        std::string m_brand;
        std::string m_key;
        DWORD m_level = 0;

        std::string read_string( HKEY h_key, const char* value_name ) {
            char buffer[ 256 ];
            DWORD buffer_size = sizeof( buffer );
            DWORD type = 0;

            LONG result = RegQueryValueExA(
                h_key,
                value_name,
                nullptr,
                &type,
                reinterpret_cast< LPBYTE >( buffer ),
                &buffer_size
            );

            if ( result != ERROR_SUCCESS || type != REG_SZ )
                return "";

            return std::string( buffer, buffer_size - 1 );
        }

        DWORD read_dword( HKEY h_key, const char* value_name ) {
            DWORD data = 0;
            DWORD data_size = sizeof( data );
            DWORD type = 0;

            LONG result = RegQueryValueExA(
                h_key,
                value_name,
                nullptr,
                &type,
                reinterpret_cast< LPBYTE >( &data ),
                &data_size
            );

            if ( result != ERROR_SUCCESS || type != REG_DWORD )
                return 0;

            return data;
        }
    };
}