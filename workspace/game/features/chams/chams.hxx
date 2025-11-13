namespace chams {
	class c_chams {
	public:
		void add( engine::u_skeletal_mesh_component* component ) {
			if ( !component ) return;

			auto current_frame = ImGui::GetFrameCount( );
			m_modified_components[ component ] = current_frame;
		}

		bool is_modified( engine::u_skeletal_mesh_component* component ) {
			if ( !component ) return false;

			auto it = m_modified_components.find( component );
			if ( it == m_modified_components.end( ) ) return false;

			return it->second == ImGui::GetFrameCount( );
		}

		void cleanup( ) {
			auto current_frame = ImGui::GetFrameCount( );

			for ( auto it = m_modified_components.begin( ); it != m_modified_components.end( );) {
				if ( current_frame - it->second > 2 ) {
					it = m_modified_components.erase( it );
				}
				else {
					++it;
				}
			}
		}

	private:
		std::unordered_map<engine::u_skeletal_mesh_component*, uint32_t> m_modified_components;
	};
}