#define COBJMACROS
#define WIN32_LEAN_AND_MEAN
#include <chrono>
#include <ctime>
#include <vector>
#include <Windows.h>
#include <tlhelp32.h>
#include <fstream>
#include <winternl.h>
#include <cstdint>
#include <DbgHelp.h>
#include <thread>
#include <mutex>
#include <map>
#include <algorithm>
#include <d3d11.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <functional>
#include <cassert>
#include <corecrt_math.h>
#include <limits>
#include <numbers>
#include <shared_mutex>
#include <D3DX11.h>
#include <D3DX11core.h>
#include <D3DX11tex.h>
#include <dwmapi.h>
#include <unordered_set>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <shellapi.h>
#include <emmintrin.h>
#include <cassert>
#include <atomic>
#include <intrin.h>
#include <set>
#include <future>
#include <thread>
#include <curl/curl.h>
#include <winhttp.h>
#include <random>
#include <psapi.h> 
#include <deque>
#include <vector>
#include <chrono>
#include <iomanip>
#include <nlohmann/json.hpp>

#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "JoyShockLibrary.lib")

#include <dependencies/imgui/font/font.h>
#include <dependencies/imgui/font/awesome-font.h>
#include <dependencies/imgui/font/icomoon.h>
#include <dependencies/imgui/bytes/image.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <dependencies/imgui/imgui.h>
#include <dependencies/imgui/imgui_internal.h>
#include <dependencies/imgui/imgui_freetype.h>
#include <dependencies/imgui/imgui_impl_dx11.h>
#include <dependencies/imgui/imgui_impl_win32.h>
#include <dependencies/imgui/imgui_settings.h>
#include <dependencies/imgui/bytes/shader.h>

ImDrawList* g_foreground = nullptr;
ImDrawList* g_background = nullptr;

#include <workspace/game/config/config.h>
#include <workspace/game/config/json/json.hxx>       
auto g_config = std::make_shared< config::c_config >( );

#include <dependencies/joyshock/joyshock.h>
#include <dependencies/controller/controller.hxx>
auto g_controller = std::make_shared< controller::c_controller >( );

#include <impl/hexrays/hexrays.h>
#include <dependencies/skcrypt/skcrypter.h>


#include <workspace/utility/logger/logger.hxx>
#include <workspace/utility/utility.hxx>
#include <impl/ia32/ia32.h>
#include <impl/wdk/wdk.h>
#include <workspace/driver/driver.hxx>

auto g_driver = std::make_shared< driver::c_driver >();


#include <workspace/utility/logger/logger.hxx>
#include <workspace/utility/utility.hxx>

#include <workspace/parser/auth/auth.hxx>
auto g_auth = std::make_shared< server::c_auth >( );

#include <impl/config/config.hxx>
auto g_auth_config = std::make_shared< server::c_config >( );

#include <workspace/game/unreal/unreal.h>
#include <workspace/game/features/tracers/tracers.hxx>
std::unordered_map<engine::a_fort_player_pawn_athena*, tracers::c_tracers> g_player_traces;
std::unordered_map<engine::a_fort_projectile_athena*, tracers::c_tracers> g_projectile_traces;

#include <workspace/game/features/chams/chams.hxx>
auto g_chams = std::make_shared< chams::c_chams >( );

#include <workspace/game/drawing/overlay/overlay.hxx>
auto g_overlay = std::make_shared< overlay::c_overlay >( );

#include <workspace/game/drawing/interface/widgets/progress.hxx>
auto g_transition = std::make_shared< progress::c_progress >( );

#include <workspace/parser/parser.hxx>
#include <workspace/parser/player/weapon.hxx>
#include <workspace/parser/player/player.hxx>
auto g_weapon_parser = std::make_shared< parser::c_weapon_parser >( );
auto g_player_parser = std::make_shared< parser::c_player_parser >( );

#include <workspace/game/entities/lists/lists.h>
#include <workspace/game/entities/cache/cache.hxx>
#include <workspace/game/drawing/interface/esp/esp.hxx>

#include <workspace/game/entities/world/world.hxx>
auto g_world = std::make_shared< world::c_world >( );

#include <workspace/game/drawing/interface/widgets/widgets.hxx>
#include <workspace/game/drawing/interface/widgets/notification.h>
auto g_notification = std::make_shared<c_notifications>( );

#include <dependencies/imgui/blur/blur.hxx>
#include <workspace/game/drawing/interface/widgets/particle.hxx>
auto g_particle = std::make_shared< particle::c_particle_system >( );

#include <workspace/game/drawing/interface/widgets/message.hxx>
auto g_message = std::make_shared< message::c_message >( );

#include <workspace/parser/auth/cache/cache.h>
#include <workspace/parser/auth/profile.hxx>
auto g_profile = std::make_shared< profile::c_profile >( );

#include <workspace/game/drawing/interface/widgets/menu.hxx>
auto g_menu = std::make_shared< menu::c_menu >( );

#include <workspace/game/drawing/interface/widgets/dashboard.hxx>
auto g_dashboard = std::make_shared< dashboard::c_dashboard >( );

#include <workspace/game/drawing/interface/widgets/arraylist.hxx>
auto g_arraylist = std::make_shared< arraylist::c_arraylist >( );

#include <workspace/game/features/zone/zone.hxx>
auto g_zone = std::make_shared< zone::c_zone >( );

#include <workspace/game/unreal/engine/functions/functions.hxx>
#include <workspace/game/features/targeting/core/target.h>
#include <workspace/game/features/targeting/weapon/weapon.hxx>
auto g_weapon = std::make_shared< weapon::c_weapon >( );

#include <workspace/game/features/targeting/core/scoring.hxx>
#include <workspace/game/features/targeting/core/selection.hxx>
auto g_selection = std::make_shared< selection::c_selection >( );

#include <workspace/game/features/targeting/smoothing/curves.hxx>
#include <workspace/game/features/targeting/smoothing/smoothing.hxx>
auto g_smoothing = std::make_shared< smoothing::c_smoothing >( );

#include <workspace/game/features/targeting/prediction/prediction.hxx>
#include <workspace/game/features/targeting/triggerbot/triggerbot.hxx>
auto g_triggerbot = std::make_shared< triggerbot::c_triggerbot >( );

#include <workspace/game/features/ring/ring.hxx>
auto g_ring = std::make_shared< ring::c_ring >( );

#include <workspace/game/features/targeting/targeting.hxx>
auto g_targeting = std::make_shared< targeting::c_targeting >( );

#include <workspace/game/features/radar/radar.hxx>
auto g_radar = std::make_shared< radar::c_radar >( );

#include <workspace/game/features/enviroment/enviroment.hxx>
#include <workspace/game/features/player/player.hxx>

void start_engine( ) {
	std::thread entity_thread( [ & ]( ) { g_world->update_engine( ); } );
	entity_thread.detach( );
}

void start_actors( ) {
	std::thread entity_thread( [ & ]( ) { g_world->update_actors( ); } );
	entity_thread.detach( );
}

void start_world( ) {
	std::thread entity_thread( [ & ]( ) { g_world->update_world( ); } );
	entity_thread.detach( );
}

void start_targeting( ) {
	std::thread entity_thread( [ & ]( ) { g_targeting->tick( ); } );
	entity_thread.detach( );
}

void start_exploits( ) {
	std::thread entity_thread( [ & ]( ) { g_world->update_exploits( ); } );
	entity_thread.detach( );
}

void start_network( ) {
	std::thread entity_thread( [ & ]( ) { g_world->update_network( ); } );
	entity_thread.detach( );
}

#include <workspace/game/drawing/loop/loop.hxx>
auto g_loop = std::make_unique< overlay::c_loop >( );

#include <workspace/utility/exception/exception.hxx>