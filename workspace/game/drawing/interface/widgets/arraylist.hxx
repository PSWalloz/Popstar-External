#pragma once
#include <set>

namespace arraylist {
	struct list_entry_t {
		std::string name;
		bool enabled;
		float anim_progress = 0.0f;
		float anim_y = 0.0f;
		engine::e_fort_rarity rarity = engine::e_fort_rarity::common;
	};

	class c_arraylist {
	private:
		std::vector<list_entry_t> m_entries;

		static constexpr float m_spacing = 15.0f;
		static constexpr float m_padding = 3.0f;
		static constexpr float m_right_bar_width = 25.0f;
		static constexpr float m_base_y = 10.0f;
		static constexpr float m_screen_offset = 10.0f;
		static constexpr float m_group_padding = 15.0f;  // Padding inside group rectangles
		static constexpr float m_group_spacing = 12.0f; // Spacing between group rectangles
		static constexpr float m_text_left_padding = 10.0f; // Extra left padding before text
		static constexpr float m_group_left_extra = 120.0f;  // Extra left spacing at start of group
		static constexpr float m_group_right_extra = 100.0f; // Extra right spacing at end of group

		ImColor m_box_color = ImColor( 19, 20, 34, 255 );
		ImVec4 m_color_a = ImVec4( 1.0f, 100 / 255.f, 150 / 255.f, 1.0f );
		ImVec4 m_color_b = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );

		float m_gradient_pos = 0.0f;
		bool m_draw_glow = true;

		struct draw_entry_t {
			list_entry_t* entry;
			float text_width;
		};

		void update_gradient_animation( float delta_time ) {
			m_gradient_pos += 0.4f * delta_time;
			if ( m_gradient_pos > 1.0f ) {
				m_gradient_pos -= 1.0f;
			}
		}

		ImVec4 calculate_module_color( float y_relative, float total_height ) const {
			float band_center = m_gradient_pos;
			float distance = fabsf( y_relative - band_center );
			if ( distance > 0.5f ) {
				distance = 1.0f - distance;
			}
			float blend = ImClamp( 1.0f - distance * 4.0f, 0.0f, 1.0f );
			return ImLerp( m_color_b, m_color_a, blend );
		}

		void render_group_box( const ImVec2& box_min, const ImVec2& box_max, const ImColor& group_color ) {
			auto draw_list = ImGui::GetBackgroundDrawList( );

			const float left_rounding = 10.0f;

			//draw_list->AddShadowRect(
			//	box_min,
			//	box_max,
			//	c::main_color,
			//	20.0f,
			//	ImVec2( 0.0f, 0.0f ),
			//	ImDrawFlags_RoundCornersLeft,
			//	3.0f
			//);

			// Draw main group rectangle with left-side rounding only (no shadow)
			draw_list->AddRectFilled( box_min, box_max, ImColor( 19, 20, 34, 220 ) , left_rounding, ImDrawFlags_RoundCornersLeft );
			draw_list->AddRectFilled( box_min, box_max, ImColor( 15, 11, 35, 150 ), left_rounding, ImDrawFlags_RoundCornersLeft );
			draw_list->AddRect( box_min, box_max, c::outline_color, left_rounding, ImDrawFlags_RoundCornersLeft, 3.0f );

			// Draw second smaller transparent rectangle with darker color
			//const float inner_padding = 20.0f;
			//ImVec2 inner_min = ImVec2( box_min.x + inner_padding, box_min.y + inner_padding );
			//ImVec2 inner_max = ImVec2( box_max.x - inner_padding, box_max.y - inner_padding );
			//ImColor inner_color = ImColor( 16, 16, 29, 255 ); // Transparent darker color
			//draw_list->PushClipRect( inner_min, inner_max, true );
			//draw_list->AddShadowRect( inner_min, inner_max, ImColor( 72, 57, 111, 255 ), 120, ImVec2( 0,0 ), left_rounding - 2.0f );
			//draw_list->PopClipRect( );
			//// Draw colored right bar for the entire group
			//draw_list->AddRectFilled(
			//	ImVec2( box_max.x - m_right_bar_width, box_min.y ),
			//	box_max,
			//	group_color,
			//	0.0f
			//);
		}

	public:
		c_arraylist( ) = default;
		~c_arraylist( ) = default;

		void add_entry( const std::string& name, bool enabled = true, engine::e_fort_rarity rarity = engine::e_fort_rarity::common ) {
			list_entry_t entry{};
			entry.name = name;
			entry.enabled = enabled;
			entry.anim_progress = 0.0f;
			entry.anim_y = 0.0f;
			entry.rarity = rarity;
			m_entries.push_back( entry );
		}

		void add_player( const std::string& player_name, engine::e_fort_rarity rarity = engine::e_fort_rarity::common ) {
			add_entry( player_name, true, rarity );
		}

		void clear_entries( ) {
			m_entries.clear( );
		}

		void update_entry( const std::string& name, bool enabled ) {
			for ( auto& entry : m_entries ) {
				if ( entry.name == name ) {
					entry.enabled = enabled;
					return;
				}
			}

			add_entry( name, enabled );
		}

		void update_entry( const std::string& name, bool enabled, engine::e_fort_rarity rarity ) {
			for ( auto& entry : m_entries ) {
				if ( entry.name == name ) {
					entry.enabled = enabled;
					entry.rarity = rarity;
					return;
				}
			}
			add_entry( name, enabled, rarity );
		}

		size_t size( ) const {
			return m_entries.size( );
		}

		bool contains( const std::string& name ) const {
			for ( const auto& entry : m_entries ) {
				if ( entry.name == name ) {
					return true;
				}
			}
			return false;
		}

		std::string get_current_weapon_name( ) const {
			if ( !g_local_pawn || !g_local_pawn->m_weapon ) {
				return "";
			}
			
			auto weapon = g_local_pawn->m_weapon;
			if ( weapon ) {
				auto weapon_data = weapon->weapon_data( );
				if ( weapon_data ) {
					return weapon_data->item_name( )->get( );
				}
			}

			return std::string( "Pickaxe" );
		}

		bool is_current_weapon( const std::string& weapon_name ) const {
			std::string current_weapon = get_current_weapon_name( );
			if ( current_weapon.empty( ) || weapon_name.empty( ) ) {
				return false;
			}
			
			std::string current_lower = current_weapon;
			std::string compare_lower = weapon_name;
			
			std::transform( current_lower.begin( ), current_lower.end( ), current_lower.begin( ), ::tolower );
			std::transform( compare_lower.begin( ), compare_lower.end( ), compare_lower.begin( ), ::tolower );
			
			return current_lower == compare_lower;
		}

		void update_inventory_state( const std::set<std::string>& current_weapons ) {
			// Disable entries that are no longer in the current weapon set
			for ( auto& entry : m_entries ) {
				if ( current_weapons.find( entry.name ) == current_weapons.end( ) ) {
					entry.enabled = false;
				}
			}
		}

		static int rarity_to_index( engine::e_fort_rarity r ) {
			switch ( r ) {
			case engine::e_fort_rarity::common: return 0;
			case engine::e_fort_rarity::uncommon: return 1;
			case engine::e_fort_rarity::rare: return 2;
			case engine::e_fort_rarity::epic: return 3;
			case engine::e_fort_rarity::legendary: return 4;
			case engine::e_fort_rarity::mythic: return 5;
			case engine::e_fort_rarity::transcendent: return 6;
			}
			return 0;
		}

		static const char* rarity_to_name( engine::e_fort_rarity r ) {
			switch ( r ) {
			case engine::e_fort_rarity::common: return "Common";
			case engine::e_fort_rarity::uncommon: return "Uncommon";
			case engine::e_fort_rarity::rare: return "Rare";
			case engine::e_fort_rarity::epic: return "Epic";
			case engine::e_fort_rarity::legendary: return "Legendary";
			case engine::e_fort_rarity::mythic: return "Mythic";
			case engine::e_fort_rarity::transcendent: return "Transcendent";
			}
			return "Loot";
		}

		void render( ) {
			if ( m_entries.empty( ) ) {
				return;
			}

			auto& io = ImGui::GetIO( );
			auto draw_list = ImGui::GetBackgroundDrawList( );

            float baseY = 10.0f;
			float screenRight = io.DisplaySize.x;
			float animSpeed = 8.0f * io.DeltaTime;
			float yLerpSpeed = 20.0f * io.DeltaTime;

			// Build a single list of draw entries
			std::vector<draw_entry_t> draw_entries;
			for ( auto& entry : m_entries ) {
				// Calculate text size using the smaller font
				ImGui::PushFont( font::esp_font );
				ImVec2 text_size = ImGui::CalcTextSize( entry.name.c_str( ) );
				ImGui::PopFont( );
				draw_entries.push_back( { &entry, text_size.x } );
			}

            // Measure title block first
            const char* title = "Quickbar";
            ImGui::PushFont( font::bold_font );
            ImVec2 title_size = ImGui::CalcTextSize( title );
			ImGui::PopFont( );
            const float title_pad = 8.0f;
            const float bar_thickness = 2.0f;
            float title_block_h = title_size.y + title_pad + bar_thickness + title_pad;

            // First pass: positions/animation (start after title block and padding)
            float currentY = baseY + title_block_h + m_padding + m_group_padding;
			for ( auto& draw_entry : draw_entries ) {
				auto* entry = draw_entry.entry;
				entry->anim_progress = ImClamp( entry->anim_progress + ( entry->enabled ? animSpeed : -animSpeed ), 0.0f, 1.0f );
				if ( entry->anim_progress > 0.0f ) {
					if ( entry->anim_y == 0.0f ) entry->anim_y = currentY;
					entry->anim_y = ImLerp( entry->anim_y, currentY, yLerpSpeed );
					currentY += ImGui::GetFontSize( ) + m_padding * 2 + m_spacing;
				}
			}

			// Collect visible, measure bounds
			std::vector<draw_entry_t*> visible_entries;
			float group_min_y = FLT_MAX;
			float group_max_y = -FLT_MAX;
			float max_width = 0.0f;
			for ( auto& draw_entry : draw_entries ) {
				auto* entry = draw_entry.entry;
				if ( entry->anim_progress > 0.0f ) {
					visible_entries.push_back( &draw_entry );
					group_min_y = ImMin( group_min_y, entry->anim_y );
					group_max_y = ImMax( group_max_y, entry->anim_y + ImGui::GetFontSize( ) + m_padding * 2 );
					// Add space for index number (estimate 2 digits max + spacing)
					float index_width = 20.0f; // Estimated width for index + spacing
					float entry_width = draw_entry.text_width + m_padding * 2 + m_text_left_padding + m_group_left_extra + m_group_right_extra + m_right_bar_width + index_width;
					max_width = ImMax( max_width, entry_width );
				}
			}
            if ( visible_entries.empty( ) ) return;

            // Build final group bounds: start at baseY to include title block, extend to last entry bottom + padding
            ImVec2 group_draw_min = ImVec2( screenRight - max_width, baseY );
            ImVec2 group_draw_max = ImVec2( screenRight, group_max_y + m_group_padding );

			ImColor group_color = c::main_color;

            // Title inside the rectangle: Quickbar + blue bar
			render_group_box( group_draw_min, group_draw_max, group_color );
			

			
			ImVec2 title_pos = ImVec2( group_draw_min.x + m_padding + m_text_left_padding, group_draw_min.y + m_padding + 10.0f );
			ImGui::PushFont( font::bold_font );
			custom::fade_text( draw_list, title, title_pos, c::main_color, c::second_color, 0 );
			ImGui::PopFont( );
			ImVec2 bar_min = ImVec2( title_pos.x, title_pos.y + title_size.y + title_pad );
			ImVec2 bar_max = ImVec2( title_pos.x + title_size.x, bar_min.y + bar_thickness );
			draw_list->AddRectFilled( bar_min, bar_max, c::second_color );

			// Entries
			for ( auto* draw_entry_ptr : visible_entries ) {
				auto& draw_entry = *draw_entry_ptr;
				auto& entry = *draw_entry.entry;
				
				// Calculate entry box dimensions
				const float entry_padding = 6.0f;
				const float entry_rounding = 6.0f;
				const float font_size = ImGui::GetFontSize();
				
				ImVec2 entry_box_min = ImVec2(
					group_draw_min.x + m_padding + 5.0f,
					entry.anim_y - entry_padding
				);
				ImVec2 entry_box_max = ImVec2(
					group_draw_max.x - m_padding - 5.0f,
					entry.anim_y + font_size + entry_padding * 2
				);

				// Get rarity color for text
				ImColor rarity_color = esp::get_tier_color( entry.rarity );
				bool is_current = is_current_weapon( entry.name );
				
				// Get entry background color (semi-transparent)
				ImColor entry_bg_color = ImColor( 14, 16, 30, 120 ); // Semi-transparent background
				
				// Draw entry background box with rounded corners (transparent)
				draw_list->AddRectFilled( entry_box_min, entry_box_max, entry_bg_color, entry_rounding );

				
				// Keep outline color consistent
				ImColor border_color = c::outline_color;
				float border_width = 1.0f;
				draw_list->AddRect( entry_box_min, entry_box_max, border_color, entry_rounding, 0, border_width );
				
				// Add small right-side rectangle if this is the current weapon
				if ( is_current ) {
					const float indicator_width = 4.0f;
					const float indicator_padding = 2.0f;
					
					ImVec2 indicator_min = ImVec2(
						entry_box_max.x - indicator_width - indicator_padding,
						entry_box_min.y + indicator_padding
					);
					ImVec2 indicator_max = ImVec2(
						entry_box_max.x - indicator_padding,
						entry_box_max.y - indicator_padding
					);
					
					// Add shadow rect highlight around the indicator
					draw_list->AddShadowRect( 
						indicator_min, 
						indicator_max, 
						rarity_color, 
						15.0f,        // Increased blur strength for brighter glow
						ImVec2( 0, 0 ), // No offset
						2.0f          // Match indicator rounding
					);
					
					draw_list->AddRectFilled( indicator_min, indicator_max, rarity_color, 2.0f );
				}

				// Calculate entry index (1-based)
				int entry_index = 0;
				for ( int i = 0; i < (int)visible_entries.size(); i++ ) {
					if ( visible_entries[i] == draw_entry_ptr ) {
						entry_index = i + 1;
						break;
					}
				}
				
				// Draw index number on the left
				ImGui::PushFont( font::bold_font );
				std::string index_text = std::to_string( entry_index );
				ImVec2 index_size = ImGui::CalcTextSize( index_text.c_str( ) );
				ImVec2 index_pos = ImVec2(
					entry_box_min.x + entry_padding + 4.0f,
					entry.anim_y + m_padding + (font_size * 0.5f - index_size.y * 0.5f)
				);
				
				// Draw index with outline
				const ImU32 outline_col = IM_COL32( 0, 0, 0, 255 );
				for ( int ox = -1; ox <= 1; ++ox ) {
					for ( int oy = -1; oy <= 1; ++oy ) {
						if ( ox == 0 && oy == 0 ) continue;
						draw_list->AddText( ImVec2( index_pos.x + ( float )ox, index_pos.y + ( float )oy ), outline_col, index_text.c_str( ) );
					}
				}
				draw_list->AddText( index_pos, rarity_color, index_text.c_str( ) );
				ImGui::PopFont( );
				
				// Calculate text position with padding from index
				float index_width = index_size.x + 8.0f; // Index width + spacing
				float thisWidth = draw_entry.text_width + m_padding * 2 + m_text_left_padding + index_width;
				float animOffset = ( 1.0f - entry.anim_progress ) * thisWidth;
				ImVec2 text_pos = ImVec2(
					entry_box_min.x + entry_padding + 8.0f + index_width + animOffset,
					entry.anim_y + m_padding
				);
				
				// Draw weapon name in smaller font
				ImGui::PushFont( font::esp_font );
				for ( int ox = -1; ox <= 1; ++ox ) {
					for ( int oy = -1; oy <= 1; ++oy ) {
						if ( ox == 0 && oy == 0 ) continue;
						draw_list->AddText( ImVec2( text_pos.x + ( float )ox, text_pos.y + ( float )oy ), outline_col, entry.name.c_str( ) );
					}
				}
				draw_list->AddText( text_pos, rarity_color, entry.name.c_str( ) );
				ImGui::PopFont( );

				//ImGui::PushFont( awesome_font::regular );
				//const float icon_padding = 8.0f;
				//const float right_padding = 15.0f;
				//const char* icon1 = "\uf005"; // Star icon
				//const char* icon2 = "\uf007";
				//
				//ImVec2 icon1_size = ImGui::CalcTextSize( icon1 );
				//ImVec2 icon2_size = ImGui::CalcTextSize( icon2 );
				//
				//// Position icons aligned to the right side of the box with 10px padding
				//float box_right_x = group_draw_max.x - right_padding;
				//ImVec2 icon2_pos = ImVec2( box_right_x - icon2_size.x, text_pos.y );
				//ImVec2 icon1_pos = ImVec2( icon2_pos.x - icon_padding - icon1_size.x, text_pos.y );
				//
				//custom::fade_text( draw_list, icon1, icon1_pos, c::anim::active, c::main_color, 0 );
				//custom::fade_text( draw_list, icon2, icon2_pos, c::anim::active, c::main_color, 0 );
				//ImGui::PopFont( );
			}
		}

		void set_glow_enabled( bool enabled ) {
			m_draw_glow = enabled;
		}

		void set_colors( const ImVec4& color_a, const ImVec4& color_b ) {
			m_color_a = color_a;
			m_color_b = color_b;
		}

		void set_box_color( const ImColor& color ) {
			m_box_color = color;
		}
	};
}