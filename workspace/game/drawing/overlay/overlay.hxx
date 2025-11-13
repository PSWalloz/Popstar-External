#pragma once

namespace texture {
    ID3D11ShaderResourceView* weed_image = nullptr;
    ID3D11ShaderResourceView* anim_logo = nullptr;
}

namespace overlay {
    struct find_window_data {
        unsigned long pid;
        std::string class_name;
        std::string window_name;
        HWND hwnd;
    };

    BOOL __stdcall enum_windows_proc( HWND hwnd, LPARAM l_param ) {
        find_window_data* data = ( find_window_data* )l_param;

        DWORD pid = 0;
        GetWindowThreadProcessId( hwnd, &pid );

        if ( pid == data->pid ) {
            char class_name[ 256 ];
            GetClassNameA( hwnd, class_name, sizeof( class_name ) );
            if ( data->class_name == class_name ) {
                char window_name[ 256 ];
                GetWindowTextA( hwnd, window_name, sizeof( window_name ) );
                if ( data->window_name == window_name ) {
                    data->hwnd = hwnd;
                    return false;
                }
            }
        }
        return true;
    }

    HWND find_child_window_from_parent( HWND parent, const char* class_name, const char* window_name ) {
        DWORD pid = 0;
        GetWindowThreadProcessId( parent, &pid );

        if ( pid == 0 )
            return nullptr;

        find_window_data data = { pid, class_name, window_name, nullptr };
        EnumWindows( enum_windows_proc, reinterpret_cast< LPARAM >( &data ) );
        return data.hwnd;
    }

    RECT get_client_area_and_size( HWND hwnd ) {
        RECT rect;
        if ( GetClientRect( hwnd, &rect ) ) {
            POINT top_left = { rect.left, rect.top };
            POINT bottom_right = { rect.right, rect.bottom };

            ClientToScreen( hwnd, &top_left );
            ClientToScreen( hwnd, &bottom_right );

            rect.left = top_left.x;
            rect.top = top_left.y;
            rect.right = bottom_right.x;
            rect.bottom = bottom_right.y;
        }
        else {
            rect = { 0, 0, 0, 0 };
        }

        return rect;
    }

    class c_overlay {
    public:
        bool setup( ) {
            m_width = GetSystemMetrics( SM_CXSCREEN );
            m_height = GetSystemMetrics( SM_CYSCREEN );

            m_width_center = m_width / 2;
            m_height_center = m_height / 2;

            HWND game_hwnd = FindWindowA( ( "UnrealWindow" ), ( "Fortnite  " ) );

            m_window_handle = find_child_window_from_parent( game_hwnd, "IME", "Default IME" );
            if ( !m_window_handle ) return false;

            RECT rect = get_client_area_and_size( game_hwnd );
            if ( !SetWindowPos( m_window_handle, nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER ) )
                return false;

            GetWindowRect( GetDesktopWindow( ), &rect );
            SetWindowLong( m_window_handle, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW );

            MARGINS window_margin{ -1 };
            DwmExtendFrameIntoClientArea( m_window_handle, &window_margin );
            SetLayeredWindowAttributes( m_window_handle, 0, 255, LWA_ALPHA );

            UpdateWindow( m_window_handle );
            ShowWindow( m_window_handle, SW_SHOW );
            return true;
        }

        bool setup_directx( ) {
            DXGI_SWAP_CHAIN_DESC sd;
            ZeroMemory( &sd, sizeof( sd ) );
            sd.BufferCount = 2;
            sd.BufferDesc.Width = 0;
            sd.BufferDesc.Height = 0;
            sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = m_window_handle;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.Windowed = TRUE;
            sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            D3D_FEATURE_LEVEL feature_level;
            const D3D_FEATURE_LEVEL feature_levels[ 2 ] = {
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_0
            };

            if ( FAILED( D3D11CreateDeviceAndSwapChain(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                0,
                feature_levels,
                2,
                D3D11_SDK_VERSION,
                &sd,
                &m_swap_chain,
                &m_d3d_device,
                &feature_level,
                &m_device_context ) ) ) {
                return false;
            }

            create_render_target( );
            setup_imgui( );
            return true;
        }


        void create_render_target( ) {
            ID3D11Texture2D* back_buffer;
            m_swap_chain->GetBuffer( 0, IID_PPV_ARGS( &back_buffer ) );
            if ( back_buffer ) {
                m_d3d_device->CreateRenderTargetView( back_buffer, nullptr, &m_render_target );
                back_buffer->Release( );
            }
        }

        void setup_imgui( ) {
            IMGUI_CHECKVERSION( );
            ImGui::CreateContext( );
            ImGuiIO& io = ImGui::GetIO( ); ( void )io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
            
            // Explicitly disable debug windows and debug features
            io.ConfigDebugBeginReturnValueOnce = false;
            io.ConfigDebugBeginReturnValueLoop = false;
            
            // Disable any automatic debug window creation
            io.ConfigDebugIniSettings = false;
            
            // Completely disable INI file saving/loading
            io.IniFilename = nullptr;
            io.LogFilename = nullptr;
            io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;

            ImFontConfig cfg;
            cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_NoHinting | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;;

            static ImWchar icomoon_ranges[ ] = { 0x1, 0x10FFFD, 0 };

            // Font Awesome 7.0 comprehensive ranges
            static ImWchar awesomefont_ranges[ ] = {
                // Font Awesome Core (main range with gaps)
                0xe005, 0xf8ff,

                // Material Design Icons (if using FA Pro with MDI)
                // 0xF0001, 0xF1AF0,  // Uncomment if you have FA Pro with MDI

                // Additional icon sets (uncomment as needed)
                // 0xE5FA, 0xE6B7,   // Seti-UI + Custom
                // 0xE700, 0xE8EF,   // Devicons
                // 0xE200, 0xE2A9,   // Font Awesome Extension
                // 0xE300, 0xE3E3,   // Weather Icons
                // 0xF400, 0xF533,   // Octicons
                // 0xE0A0, 0xE0A2,   // Powerline Symbols
                // 0xE0B0, 0xE0B3,   // Powerline Symbols
                // 0x23FB, 0x23FE,   // IEC Power Symbols

                0  // Null terminator - ESSENTIAL!
            };

            static ImFontConfig awesomefont_config;
            awesomefont_config.OversampleH = awesomefont_config.OversampleV = 1;
            awesomefont_config.MergeMode = true;
            awesomefont_config.GlyphOffset.y = 2;

            static ImFontConfig icomoon_config;
            icomoon_config.OversampleH = icomoon_config.OversampleV = 1;
            icomoon_config.MergeMode = true;
            icomoon_config.GlyphOffset.y = 2;

            io.Fonts->AddFontFromMemoryTTF( PoppinsRegular, sizeof( PoppinsRegular ), 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );
            io.Fonts->AddFontFromMemoryCompressedBase85TTF( icomoon_compressed_data_base85, 18.f, &icomoon_config, icomoon_ranges );

            font::small_font = io.Fonts->AddFontFromMemoryTTF( PoppinsRegular, sizeof( PoppinsRegular ), 16.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );
            font::esp_font = io.Fonts->AddFontFromMemoryTTF( PoppinsRegular, sizeof( PoppinsRegular ), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );
            font::regular_m = io.Fonts->AddFontFromMemoryTTF( PoppinsMedium, sizeof( PoppinsMedium ), 21.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );
            font::regular_l = io.Fonts->AddFontFromMemoryTTF( PoppinsMedium, sizeof( PoppinsMedium ), 41.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );

            font::regular_medium = io.Fonts->AddFontFromMemoryTTF( PoppinsRegular, sizeof( PoppinsRegular ), 35.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );
            io.Fonts->AddFontFromMemoryCompressedBase85TTF( icomoon_compressed_data_base85, 35.f, &icomoon_config, icomoon_ranges );
            io.Fonts->AddFontFromMemoryCompressedBase85TTF( af_compressed_data_base85, 27.f, &awesomefont_config, awesomefont_ranges );

            font::s_inter_semibold = io.Fonts->AddFontFromMemoryTTF( PoppinsSemiBold, sizeof( PoppinsSemiBold ), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );
            font::bold_font = io.Fonts->AddFontFromMemoryTTF( PoppinsBold, sizeof( PoppinsBold ), 23.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );
            io.Fonts->AddFontFromMemoryCompressedBase85TTF( icomoon_compressed_data_base85, 27.f, &icomoon_config, icomoon_ranges );
            io.Fonts->AddFontFromMemoryCompressedBase85TTF( af_compressed_data_base85, 27.f, &awesomefont_config, awesomefont_ranges );

            // s_inter_semibold
            font::semibold_font = io.Fonts->AddFontFromMemoryTTF( PoppinsSemiBold, sizeof( PoppinsSemiBold ), 29.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );
            io.Fonts->AddFontFromMemoryCompressedBase85TTF( icomoon_compressed_data_base85, 27.f, &icomoon_config, icomoon_ranges );
            io.Fonts->AddFontFromMemoryCompressedBase85TTF( af_compressed_data_base85, 27.f, &awesomefont_config, awesomefont_ranges );

            //io.Fonts->AddFontFromMemoryCompressedTTF( awesome_font, sizeof( awesome_font ), 27.f, &awesomefont_config, awesomefont_ranges );

            font::inter_bold = io.Fonts->AddFontFromMemoryTTF( PoppinsSemiBold, sizeof( PoppinsSemiBold ), 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );

            font::inter_medium = io.Fonts->AddFontFromMemoryTTF( PoppinsMedium, sizeof( PoppinsMedium ), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );
            font::inter_medium_semibold = io.Fonts->AddFontFromMemoryTTF( PoppinsMedium, sizeof( PoppinsMedium ), 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );

            font::s_inter_semibold = io.Fonts->AddFontFromMemoryTTF( PoppinsSemiBold, sizeof( PoppinsSemiBold ), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );

            awesome_font::regular = io.Fonts->AddFontFromMemoryTTF( PoppinsRegular, sizeof( PoppinsRegular ), 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );
            io.Fonts->AddFontFromMemoryCompressedBase85TTF( af_compressed_data_base85, 22.f, &awesomefont_config, awesomefont_ranges );

            awesome_font::small_font = io.Fonts->AddFontFromMemoryTTF( PoppinsRegular, sizeof( PoppinsRegular ), 18.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );
            io.Fonts->AddFontFromMemoryCompressedBase85TTF( af_compressed_data_base85, 20.f, &awesomefont_config, awesomefont_ranges );

            awesome_font::bold_font = io.Fonts->AddFontFromMemoryTTF( PoppinsBold, sizeof( PoppinsBold ), 23.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );
            io.Fonts->AddFontFromMemoryCompressedBase85TTF( af_compressed_data_base85, 27.f, &awesomefont_config, awesomefont_ranges );

            awesome_font::semibold_font = io.Fonts->AddFontFromMemoryTTF( PoppinsSemiBold, sizeof( PoppinsSemiBold ), 29.f, &cfg, io.Fonts->GetGlyphRangesCyrillic( ) );
            io.Fonts->AddFontFromMemoryCompressedBase85TTF( af_compressed_data_base85, 27.f, &awesomefont_config, awesomefont_ranges );
            io.Fonts->Build( );

            D3DX11_IMAGE_LOAD_INFO info; ID3DX11ThreadPump* pump{ nullptr };
            D3DX11CreateShaderResourceViewFromMemory( m_d3d_device, logo_gif, sizeof( logo_gif ), &info, pump, &texture::anim_logo, 0 );
            D3DX11CreateShaderResourceViewFromMemory( m_d3d_device, weed_bytes, sizeof( weed_bytes ), &info, pump, &texture::weed_image, 0 );

            ImGuiStyle& s = ImGui::GetStyle( );
            s.FramePadding = ImVec2( 18, 7 );
            s.ItemSpacing = ImVec2( 4, 7 );
            s.FrameRounding = 2.f;
            s.WindowRounding = 20.f;
            s.WindowBorderSize = 0.f;
            s.PopupBorderSize = 0.f;
            s.WindowPadding = ImVec2( 20, 20 );
            s.ChildBorderSize = 1.f;
            s.Colors[ ImGuiCol_Border ] = ImVec4( 0.f, 0.f, 0.f, 0.f );
            s.Colors[ ImGuiCol_Separator ] = ImVec4( 1.f, 1.f, 1.f, 0.2f );
            s.Colors[ ImGuiCol_BorderShadow ] = ImVec4( 0.f, 0.f, 0.f, 0.f );
            s.WindowShadowSize = 0;
            s.PopupRounding = 5.f;
            s.ScrollbarSize = 1;
            s.SeparatorTextPadding = ImVec2( 10, 7 );
            s.Alpha = 1;
            ImGui_ImplWin32_Init( m_window_handle );
            ImGui_ImplDX11_Init( m_d3d_device, m_device_context );
            
            // Force disable debug windows and INI saving after backend initialization
            ImGuiIO& io_after_init = ImGui::GetIO();
            io_after_init.ConfigDebugBeginReturnValueOnce = false;
            io_after_init.ConfigDebugBeginReturnValueLoop = false;
            io_after_init.ConfigDebugIniSettings = false;
            io_after_init.IniFilename = nullptr;
            io_after_init.LogFilename = nullptr;
        }

        void new_frame( ) {
            ImGui_ImplDX11_NewFrame( );
            POINT p;
            GetCursorPos( &p );
            ImGuiIO& io = ImGui::GetIO( );
            io.MousePos = ImVec2( p.x, p.y );
            io.MouseDown[ 0 ] = ( GetKeyState( VK_LBUTTON ) & 0x8000 ) != 0;
            io.MouseDown[ 1 ] = ( GetKeyState( VK_RBUTTON ) & 0x8000 ) != 0;

            ImGui_ImplWin32_NewFrame( );
            ImGui::NewFrame( );
        }

        void draw_frame( ) {
            ImGui::Render( );
            const float clear_color[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
            m_device_context->OMSetRenderTargets( 1, &m_render_target, nullptr );
            m_device_context->ClearRenderTargetView( m_render_target, clear_color );
            ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

            m_swap_chain->Present( config::misc::vsync, 0 );
        }

        bool is_window_focused( ) {
            if ( g_driver->m_window_handle == GetForegroundWindow( ) ) {
                return true;
            }

            if ( g_driver->m_window_handle == GetActiveWindow( ) ) {
                return true;
            }

            if ( GetActiveWindow( ) == GetForegroundWindow( ) ) {
                return true;
            }

            return false;
        }

        void destroy( ) {
            ImGui_ImplDX11_Shutdown( );
            ImGui_ImplWin32_Shutdown( );
            ImGui::DestroyContext( );

            if ( m_render_target ) { m_render_target->Release( ); m_render_target = nullptr; }
            if ( m_swap_chain ) { m_swap_chain->Release( ); m_swap_chain = nullptr; }
            if ( m_device_context ) { m_device_context->Release( ); m_device_context = nullptr; }
            if ( m_d3d_device ) { m_d3d_device->Release( ); m_d3d_device = nullptr; }
        }

        void update_affinity( ) const {
            DWORD window_affinity;
            if ( GetWindowDisplayAffinity( m_window_handle, &window_affinity ) ) {
                auto new_affinity = config::misc::stream_proof ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE;
                if ( window_affinity != new_affinity ) {
                    SetWindowDisplayAffinity( m_window_handle, new_affinity );
                }
            }
        }

        ID3D11Device* m_d3d_device = nullptr;
        IDXGISwapChain* m_swap_chain = nullptr;
        ID3D11RenderTargetView* m_render_target = nullptr;
        ID3D11DeviceContext* m_device_context = nullptr;

    public:
        HWND m_window_handle{ nullptr };

        int m_width = 0;
        int m_height = 0;

        int m_width_center = 0;
        int m_height_center = 0;
    };
}