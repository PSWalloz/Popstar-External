#include <ntstatus.h>

namespace driver {
    class c_driver {
    public:
        bool setup( ) {
            logging::print( encrypt( "Starting driver installation" ) );

            m_driver_handle = CreateFileW(
                L"\\\\.\\this-isnt-a-real-device-hi-from-leproxy", // hello ioctl.
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                nullptr,
                OPEN_EXISTING,
                FILE_FLAG_OVERLAPPED,
                nullptr
            );

            if ( m_driver_handle == INVALID_HANDLE_VALUE ) {
                logging::print( encrypt( "Failed to open driver handle. Error: %x\n" ) , GetLastError( ) );
                return false;
            }

            logging::print( encrypt( "Successfully opened driver handle\n" ) );
            return true;
        }

        bool attach( const wchar_t* target_name ) {
            logging::print( encrypt( "Searching for process" ) );

            while ( true ) {
                m_process_id = get_process_id( target_name );
                if ( m_process_id ) break;
                Sleep( 250 );
            }

            logging::print( encrypt( "Process PID: %i" ), m_process_id );

            m_window_handle = get_window_handle( m_process_id );
            if ( !m_window_handle )
                return false; 

            logging::print( encrypt( "Window Handle: %llx" ), m_window_handle );

            m_process_handle = open_process( m_process_id );
            if ( !m_process_handle )
                return false;

            logging::print( encrypt( "Process Handle: %llx" ), m_process_handle );

            m_eprocess = get_eprocess( m_process_id );
            if ( !m_eprocess )
                return false;

            logging::print( encrypt( "EProcess: %llx" ), m_eprocess );

            m_base_address = get_base_address( m_eprocess );
            if ( !m_base_address )
                return false;

            logging::print( encrypt( "Base Address: %llx" ), m_base_address );

            auto time_now = std::chrono::high_resolution_clock::now( );

            m_dtb = get_dtb( m_base_address );
            if ( !m_dtb )
                return false;

            auto time_stop = std::chrono::duration_cast< std::chrono::duration< float > >(
                std::chrono::high_resolution_clock::now( ) - time_now
            );

            logging::print( encrypt( "Found DTB: %llx, MS: %f" ), m_dtb, time_stop.count( ) );
            logging::print( encrypt( "Successfully found process\n" ) );

            return true;
        }

        bool test_read( ) {
            if ( !m_base_address )
                return false;

            logging::print( encrypt( "Testing read in range: 0x10,000" ) );
            for ( auto i = 0ull; i < 0x10000; i++ ) {
                auto time_now = std::chrono::high_resolution_clock::now( );
                auto address = this->read< std::uintptr_t >( m_base_address + i );
                auto time_stop = std::chrono::duration_cast< std::chrono::duration< float > >(
                    std::chrono::high_resolution_clock::now( ) - time_now
                );
                if ( !address )
                    continue;

                logging::print( encrypt( "Successfully read address at 0x%llx in %f MS" ), address, time_stop.count( ) );
                return true;
            }

            return false;
        }

        bool read_memory( uintptr_t address, void* buffer, size_t size ) {
            s_command_data data{};
            data.m_process = this->m_eprocess;
            data.m_pid = this->m_process_id;
            data.m_address = address;
            data.m_buffer = buffer;
            data.m_size = size;

            auto result = send_command( e_command_type::read_memory, data );
            return !result.get_status( );
        }

        bool write_memory( uintptr_t address, void* buffer, size_t size ) {
            s_command_data data{};
            data.m_pid = this->m_process_id;
            data.m_process = this->m_eprocess;
            data.m_address = address;
            data.m_buffer = buffer;
            data.m_size = size;

            auto result = send_command( e_command_type::write_memory, data );
            buffer = result.get_data( ).m_buffer;
            return !result.get_status( );
        }

        std::uintptr_t get_dtb( std::uintptr_t base_address ) {
            s_command_data data{};
            data.m_process = this->m_eprocess;
            data.m_address = base_address;

            auto result = send_command( e_command_type::get_dtb, data );
            return result.get_data( ).m_address2;
        }

        std::uintptr_t get_base_address( eprocess_t* process ) {
            s_command_data data{};
            data.m_process = process;

            auto result = send_command( e_command_type::get_base_address, data );
            return result.get_data( ).m_address;
        }

        std::uintptr_t map_physical( std::uintptr_t address, std::uint32_t size ) {
            s_command_data data{};
            data.m_address = address;
            data.m_size = size;

            auto result = send_command( e_command_type::map_physical, data );
            return result.get_data( ).m_address2;
        }

        std::pair<std::uintptr_t, std::size_t> translate_linear( std::uintptr_t address ) {
            s_command_data data{};
            data.m_address = address;

            auto result = send_command( e_command_type::translate_linear, data );
            return { result.get_data( ).m_address2, result.get_data( ).m_size };
        }

       

        peb_t* get_peb( eprocess_t* process ) {
            s_command_data data{};
            data.m_process = process;

            auto result = send_command( e_command_type::get_process_peb, data );
            return result.get_data( ).m_peb;
        }

        peb_t get_process_peb( eprocess_t* process ) {
            peb_t process_peb;
            auto peb = get_peb( process );
            read_memory( reinterpret_cast< std::uintptr_t >( peb ), &process_peb, sizeof( peb_t ) );
            return process_peb;
        }

        eprocess_t* get_eprocess( std::uint32_t process_id ) {
            s_command_data data{};
            data.m_pid = process_id;

            auto result = send_command( e_command_type::get_eprocess, data );
            return result.get_data( ).m_process;
        }


        void* open_process( std::uint32_t process_id ) {
            s_command_data data{};
            data.m_pid = process_id;

            auto result = send_command( e_command_type::open_process, data );
            return result.get_data( ).m_buffer;
        }

        template <typename T>
        std::vector<T> read_array( uintptr_t address, size_t count ) {
            std::vector<T> buffer( count );
            const size_t size = count * sizeof( T );

            if ( !read_memory( address, buffer.data( ), size ) ) {
                logging::print( encrypt( "Failed to read array of %zu elements from 0x%llX" ), count, address );
                return {};
            }

            return buffer;
        }

        template<typename ret_t = std::uintptr_t, typename addr_t>
        ret_t read( addr_t address ) {
            ret_t data{};
            if ( !read_memory(
                address,
                &data,
                sizeof( ret_t )
                ) ) return ret_t{};
            return data;
        }

        template<typename data_t, typename addr_t>
        bool write( addr_t address, data_t data ) {
            return this->write_memory(
                address,
                &data,
                sizeof( data_t )
            );
        }

        void unload( ) {
            if ( m_driver_handle != INVALID_HANDLE_VALUE ) {
                CloseHandle( m_driver_handle );
                m_driver_handle = INVALID_HANDLE_VALUE;
            }
        }

        std::uintptr_t find_min( std::uint32_t g, std::size_t f ) {
            auto h = ( std::uint32_t )f;

            return ( ( ( g ) < ( h ) ) ? ( g ) : ( h ) );
        }

        std::uint32_t get_process_id( std::wstring module_name ) {
            auto snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
            if ( snapshot == INVALID_HANDLE_VALUE )
                return false;

            PROCESSENTRY32W process_entry{ };
            process_entry.dwSize = sizeof( process_entry );
            Process32FirstW( snapshot, &process_entry );
            do {
                if ( !module_name.compare( process_entry.szExeFile ) )
                    return process_entry.th32ProcessID;
            } while ( Process32NextW( snapshot, &process_entry ) );

            return 0;
        }

        std::uintptr_t get_process_module( const wchar_t* module_name ) {
            if ( !m_process_handle || !m_eprocess )
                return 0;

            auto process_peb = get_peb( m_eprocess );
            if ( !process_peb )
                return 0;

            peb_t peb_data;
            if ( !read_memory( reinterpret_cast< std::uintptr_t >( process_peb ), &peb_data, sizeof( peb_t ) ) )
                return 0;

            peb_ldr_data_t ldr_data;
            if ( !read_memory( reinterpret_cast< std::uintptr_t >( peb_data.m_ldr ), &ldr_data, sizeof( peb_ldr_data_t ) ) )
                return 0;

            auto current_entry = ldr_data.m_module_list_load_order.m_flink;
            auto first_entry = current_entry;

            do {
                ldr_data_table_entry_t entry;
                if ( !read_memory( reinterpret_cast< std::uintptr_t >( current_entry ), &entry, sizeof( ldr_data_table_entry_t ) ) )
                    break;

                wchar_t module_name_buffer[ MAX_PATH ];
                if ( entry.m_base_dll_name.m_length > 0 && entry.m_base_dll_name.m_length < MAX_PATH * 2 ) {
                    if ( read_memory( reinterpret_cast< std::uintptr_t >( entry.m_base_dll_name.m_buffer ),
                        module_name_buffer,
                        entry.m_base_dll_name.m_length ) ) {

                        module_name_buffer[ entry.m_base_dll_name.m_length / 2 ] = L'\0';
                        if ( _wcsicmp( module_name_buffer, module_name ) == 0 ) {
                            return reinterpret_cast< std::uintptr_t >( entry.m_dll_base );
                        }
                    }
                }

                current_entry = entry.m_in_load_order_module_list.m_flink;
            } while ( current_entry && current_entry != first_entry );

            return 0;
        }

        std::vector<void*> get_process_threads( ) {
            std::vector<void*> threads;

            if ( !m_process_id || !m_eprocess )
                return threads;

            list_entry_t list_head;
            if ( !read_memory( reinterpret_cast< std::uintptr_t >( m_eprocess ) + 0x030, &list_head, sizeof( list_entry_t ) ) )
                return threads;

            list_entry_t current_entry;
            if ( !read_memory( reinterpret_cast< std::uintptr_t >( list_head.m_flink ), &current_entry, sizeof( list_entry_t ) ) )
                return threads;

            auto first_entry = current_entry;

            do {
                ethread_t entry;
                if ( !read_memory( reinterpret_cast< std::uintptr_t >( current_entry.m_flink ) - 0x4e8,
                    &entry, sizeof( ethread_t ) ) )
                    break;

                logging::print( encrypt( "Thread handle: %llx" ), entry.m_client_id.m_unique_thread );
                threads.push_back( entry.m_client_id.m_unique_thread );

                read_memory(
                    reinterpret_cast< std::uintptr_t >( current_entry.m_flink ),
                    &current_entry,
                    sizeof( current_entry )
                );

                if ( current_entry.m_flink == first_entry.m_flink )
                    break;

            } while ( current_entry.m_flink != first_entry.m_flink );
            return threads;
        }

        std::map<std::uintptr_t, std::wstring> get_process_modules( ) {
            std::map<std::uintptr_t, std::wstring> modules;

            if ( !m_process_id || !m_eprocess )
                return modules;

            auto process_peb = get_process_peb( m_eprocess );
            if ( !process_peb.m_ldr )
                return modules;

            peb_ldr_data_t ldr_data;
            if ( !read_memory( reinterpret_cast< std::uintptr_t >( process_peb.m_ldr ), &ldr_data, sizeof( peb_ldr_data_t ) ) )
                return modules;

            auto current_entry = ldr_data.m_module_list_in_it_order;
            auto first_entry = current_entry;

            do {
                ldr_data_table_entry_t entry;
                if ( !read_memory( reinterpret_cast< std::uintptr_t >( current_entry.m_flink ) - offsetof( ldr_data_table_entry_t, m_in_initialization_order_module_list ),
                    &entry, sizeof( ldr_data_table_entry_t ) ) )
                    break;

                wchar_t module_name[ MAX_PATH ] = { 0 };
                if ( entry.m_base_dll_name.m_length > 0 && entry.m_base_dll_name.m_buffer ) {
                    if ( !read_memory( reinterpret_cast< std::uintptr_t >( entry.m_base_dll_name.m_buffer ),
                        module_name,
                        entry.m_base_dll_name.m_length ) )
                        break;
                }

                if ( entry.m_dll_base && module_name[ 0 ] != L'\0' ) {
                    modules[ reinterpret_cast< std::uintptr_t >( entry.m_dll_base ) ] = module_name;
                    logging::print( encrypt( "Found module: %S at 0x%llX" ), module_name, entry.m_dll_base );
                }

                read_memory( 
                    reinterpret_cast<std::uintptr_t>( current_entry.m_flink ), 
                    &current_entry,
                    sizeof( current_entry )
                );

                if ( current_entry.m_flink == first_entry.m_flink )
                    break;

            } while ( current_entry.m_flink != nullptr && current_entry.m_flink != first_entry.m_flink );
            return modules;
        }

        std::uintptr_t get_module_export( std::uintptr_t module_base, const char* export_name ) {
            if ( !module_base || !export_name )
                return 0;

            IMAGE_DOS_HEADER dos_header;
            if ( !read_memory( module_base, &dos_header, sizeof( IMAGE_DOS_HEADER ) ) )
                return 0;

            if ( dos_header.e_magic != IMAGE_DOS_SIGNATURE )
                return 0;

            IMAGE_NT_HEADERS nt_headers;
            if ( !read_memory( module_base + dos_header.e_lfanew, &nt_headers, sizeof( IMAGE_NT_HEADERS ) ) )
                return 0;

            if ( nt_headers.Signature != IMAGE_NT_SIGNATURE )
                return 0;

            auto export_dir = &nt_headers.OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ];
            if ( !export_dir->VirtualAddress || !export_dir->Size )
                return 0;

            IMAGE_EXPORT_DIRECTORY export_directory;
            if ( !read_memory( module_base + export_dir->VirtualAddress, &export_directory, sizeof( IMAGE_EXPORT_DIRECTORY ) ) )
                return 0;

            auto functions = read_array<DWORD>( module_base + export_directory.AddressOfFunctions,
                export_directory.NumberOfFunctions );
            if ( functions.empty( ) )
                return 0;

            auto names = read_array<DWORD>( module_base + export_directory.AddressOfNames,
                export_directory.NumberOfNames );
            if ( names.empty( ) )
                return 0;

            auto ordinals = read_array<WORD>( module_base + export_directory.AddressOfNameOrdinals,
                export_directory.NumberOfNames );
            if ( ordinals.empty( ) )
                return 0;

            for ( DWORD i = 0; i < export_directory.NumberOfNames; i++ ) {
                char name_buffer[ 256 ] = { 0 };
                if ( !read_memory( module_base + names[ i ], name_buffer, sizeof( name_buffer ) - 1 ) )
                    continue;

                if ( strcmp( name_buffer, export_name ) == 0 ) {
                    WORD ordinal = ordinals[ i ];
                    return module_base + functions[ ordinal ];
                }
            }

            return 0;
        }

        HWND get_window_handle( std::uint32_t pid ) {
            std::pair<HWND, DWORD> params = { 0, pid };
            auto result = EnumWindows( [ ]( HWND hwnd, LPARAM lParam ) -> int {
                auto params = ( std::pair<HWND, DWORD>* )( lParam );

                DWORD process_id;
                if ( GetWindowThreadProcessId( hwnd, &process_id ) && process_id == params->second ) {

                    SetLastError( -1 );
                    params->first = hwnd;
                    return false;
                }

                return true;

                }, reinterpret_cast< LPARAM >( &params ) );

            if ( !result && GetLastError( ) == -1 && params.first ) {
                return params.first;
            }

            return 0;
        }

    public:
        std::uint32_t m_process_id;
        eprocess_t* m_eprocess;
        peb_t* m_process_peb;
        void* m_process_handle;
        std::uintptr_t m_base_address;
        std::uintptr_t m_game_assembly;
        std::uintptr_t m_dtb;
        HWND m_window_handle;

    private:
        HANDLE m_driver_handle = INVALID_HANDLE_VALUE;

        c_command send_command( e_command_type type, const s_command_data& data, std::uint32_t timeout_ms = 2000 ) const {
            c_command command( type, data );

            try {
                if ( this->m_driver_handle == INVALID_HANDLE_VALUE ) {
                    logging::print( encrypt( "Cannot send command: driver handle not valid" ) );
                    command.set_status( STATUS_UNSUCCESSFUL );
                    exit( 0xb00b );
                    return command;
                }

                IO_STATUS_BLOCK block;
                auto result =
                    direct_device_control(
                        this->m_driver_handle,
                        nullptr,
                        nullptr,
                        nullptr,
                        &block,
                        0,
                        &command,
                        sizeof( command ),
                        &command,
                        sizeof( command ) );
            }
            catch ( const std::exception& ) {
                logging::print( encrypt( "Exception in send_command" ) );
                command.set_status( STATUS_UNSUCCESSFUL );
            }
            catch ( ... ) {
                logging::print( encrypt( "Unknown exception in send_command" ) );
                command.set_status( STATUS_UNSUCCESSFUL );
            }

            return command;
        }
    };
}