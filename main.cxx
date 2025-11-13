#include <impl/includes.h>

int main( ) {
	//SetUnhandledExceptionFilter( exception::exception_filter );
//	g_auth_config->load_value( );
//	SetConsoleTitleA( g_auth_config->get_brand( ).c_str( ) );

	if (!g_driver->setup())
		return std::getchar();
	//MessageBoxA( 0, encrypt( "Press OK when you are in lobby." ),
//		std::format( "{} External", g_auth_config->get_brand( ) ).c_str( ), MB_ICONWARNING | MB_OK );
	Beep( 100, 200 );
	if (!g_driver->attach(encrypt(L"FortniteClient-Win64-Shipping.exe"))) {
		logging::print(encrypt("Failed to attach process."));
		g_driver->unload();
		return std::getchar();
	}


	if ( !g_overlay->setup( ) ) {
		logging::print( encrypt( "Failed to setup overlay." ) );
		g_driver->unload( );
		return std::getchar( );
	}

	if ( !g_overlay->setup_directx( ) ) {
		logging::print( encrypt( "Failed to setup directx." ) );
		g_driver->unload( );
		return std::getchar( );
	}

	if ( !g_weapon_parser->setup( ) ) {
		logging::print( encrypt( "Failed to setup weapons" ) );
		g_driver->unload( );
		return std::getchar( );
	}

	g_menu->setup( );
	g_config->get_available_configs( );

	CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE )start_engine, NULL, NULL, NULL );
	CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE )start_actors, NULL, NULL, NULL );
	CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE )start_network, NULL, NULL, NULL );
	CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE )start_world, NULL, NULL, NULL );
	CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE )start_exploits, NULL, NULL, NULL );
	CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE )start_targeting, NULL, NULL, NULL );

	g_loop->render( );

	return std::getchar( );
}