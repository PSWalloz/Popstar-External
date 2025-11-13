#pragma once

static NTSTATUS( __stdcall* NtDelayExecution )( BOOL Alertable, PLARGE_INTEGER DelayInterval ) = ( NTSTATUS( __stdcall* )( BOOL, PLARGE_INTEGER ) ) GetProcAddress( GetModuleHandleA( ( "ntdll.dll" ) ), ( "NtDelayExecution" ) );
static NTSTATUS( __stdcall* ZwSetTimerResolution )( IN ULONG RequestedResolution, IN BOOLEAN Set, OUT PULONG ActualResolution ) = ( NTSTATUS( __stdcall* )( ULONG, BOOLEAN, PULONG ) ) GetProcAddress( GetModuleHandleA( ( "ntdll.dll" ) ), ( "ZwSetTimerResolution" ) );

namespace utility {
    void sleep_short( float milliseconds ) {
        static bool once = true;
        if ( once ) {
            ULONG actualResolution;
            ZwSetTimerResolution( 1, true, &actualResolution );
            once = false;
        }

        LARGE_INTEGER interval;
        interval.QuadPart = -1 * ( int )( milliseconds * 10000.0f );
        NtDelayExecution( false, &interval );
    }

    struct memory_info_t {
        size_t total_physical_mb = 0;
        size_t available_physical_mb = 0;
        size_t total_virtual_mb = 0;
        size_t available_virtual_mb = 0;
        size_t process_working_set_mb = 0;
        size_t process_private_mb = 0;
        double memory_load_percent = 0.0;
    };

    memory_info_t get_system_memory_info( ) {
        memory_info_t info{};

        try {
            MEMORYSTATUSEX mem_status{};
            mem_status.dwLength = sizeof( mem_status );

            if ( GlobalMemoryStatusEx( &mem_status ) ) {
                info.total_physical_mb = static_cast< size_t >( mem_status.ullTotalPhys / ( 1024 * 1024 ) );
                info.available_physical_mb = static_cast< size_t >( mem_status.ullAvailPhys / ( 1024 * 1024 ) );
                info.total_virtual_mb = static_cast< size_t >( mem_status.ullTotalVirtual / ( 1024 * 1024 ) );
                info.available_virtual_mb = static_cast< size_t >( mem_status.ullAvailVirtual / ( 1024 * 1024 ) );
                info.memory_load_percent = static_cast< double >( mem_status.dwMemoryLoad );
            }

            PROCESS_MEMORY_COUNTERS_EX pmc{};
            if ( GetProcessMemoryInfo( GetCurrentProcess( ), reinterpret_cast< PROCESS_MEMORY_COUNTERS* >( &pmc ), sizeof( pmc ) ) ) {
                info.process_working_set_mb = pmc.WorkingSetSize / ( 1024 * 1024 );
                info.process_private_mb = pmc.PrivateUsage / ( 1024 * 1024 );
            }
        }
        catch ( ... ) {
        }

        return info;
    }

    bool is_system_memory_low( double threshold_percent = 85.0 ) {
        try {
            auto mem_info = get_system_memory_info( );
            return mem_info.memory_load_percent > threshold_percent;
        }
        catch ( ... ) {
            return false;
        }
    }

    bool is_process_memory_high( size_t threshold_mb = 1024 ) {
        try {
            auto mem_info = get_system_memory_info( );
            return mem_info.process_working_set_mb > threshold_mb;
        }
        catch ( ... ) {
            return false;
        }
    }

    //std::uint64_t get_module( const std::wstring& module_name ) {
    //    const auto* peb = reinterpret_cast< peb_t* >( __readgsqword( 0x60 ) );
    //    if ( peb == nullptr ) {
    //        return 0;
    //    }

    //    auto first_module =
    //        reinterpret_cast< kldr_data_table_entry_t* >( peb->m_ldr->m_module_list_memory_order.m_flink );
    //    auto current_module = first_module;

    //    do {
    //        auto entry = reinterpret_cast< kldr_data_table_entry_t* >(
    //            reinterpret_cast< std::uint64_t >( current_module ) - sizeof( list_entry_t ) );

    //        if ( ( ( entry->m_base_dll_name.m_buffer != nullptr ) && !_wcsicmp( entry->m_base_dll_name.m_buffer, module_name.c_str( ) ) ) ) {
    //            const auto module_base = reinterpret_cast< std::uintptr_t >( entry->m_dll_base );

    //            return module_base;
    //        }

    //        current_module =
    //            reinterpret_cast< kldr_data_table_entry_t* >( reinterpret_cast< list_entry_t* >( current_module )->m_flink );
    //    } while ( current_module != first_module );

    //    return 0;
    //}
}