namespace controller {
	class c_controller {
	public:
		c_controller( ) {
			m_device_count = 0;
			m_joyshock_initialized = false;
			memset( m_device_handles, 0, sizeof( m_device_handles ) );
		}
		
		~c_controller( ) {
			if ( m_joyshock_initialized ) {
				try {
					JslDisconnectAndDisposeAll( );
				}
				catch ( ... ) {
				}
			}
		}
		
		bool is_trigger_pressed( float threshold = 0.3f ) const {
			if ( !config::misc::controller_support ) {
				return false;
			}
			
			initialize_joyshock_once( );
			
			if ( m_device_count > 0 ) {
				for ( int i = 0; i < m_device_count; ++i ) {
					int device_id = m_device_handles[ i ];
					
					try {
						auto left_trigger = JslGetLeftTrigger( device_id );
						if ( left_trigger > threshold ) {
							return true;
						}
					}
					catch ( ... ) {
						continue;
					}
				}
			}
			
			return false;
		}
		
		void refresh_controllers( ) {
			if ( !config::misc::controller_support ) {
				return;
			}
			
			try {
				auto total_devices = JslConnectDevices( );
				if ( total_devices <= 0 ) {
					m_device_count = 0;
					return;
				}
				
				auto actual_count = JslGetConnectedDeviceHandles( m_device_handles, MAX_DEVICES );
				m_device_count = ( actual_count > 0 && actual_count <= MAX_DEVICES ) ? actual_count : 0;
				
				if ( m_device_count > 0 ) {
					m_joyshock_initialized = true;
				}
			}
			catch ( ... ) {
				m_device_count = 0;
			}
		}
		
		int get_connected_controller_count( ) const {
			return m_device_count;
		}
		
		bool has_controllers( ) const {
			return m_device_count > 0;
		}

	private:
		static constexpr int MAX_DEVICES = 16;
		mutable int m_device_handles[MAX_DEVICES];
		mutable int m_device_count;
		mutable bool m_joyshock_initialized;
		
		void initialize_joyshock_once( ) const {
			if ( m_joyshock_initialized ) {
				return;
			}
			
			m_joyshock_initialized = true;
			
			try {
				auto total_devices = JslConnectDevices( );
				if ( total_devices <= 0 ) {
					m_device_count = 0;
					return;
				}
				
				auto actual_count = JslGetConnectedDeviceHandles( m_device_handles, MAX_DEVICES );
				m_device_count = ( actual_count > 0 && actual_count <= MAX_DEVICES ) ? actual_count : 0;
			}
			catch ( ... ) {
				m_device_count = 0;
			}
		}
	};
}