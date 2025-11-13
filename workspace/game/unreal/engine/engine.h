#define current_class reinterpret_cast<uint64_t>( this )

#define declare_member(type, name, offset) type name() { return g_driver->read<type>( current_class + offset ); } 
#define declare_member_bit(Bit, Name, Offset) bool Name( ) { return bool( g_driver->read<char>( current_class + Offset) & (1 << Bit)); }

#define apply_member(type, name, offset) void name( type val ) { g_driver->write<type>( current_class + offset, val); }
#define apply_member_bit(Bit, Name, Offset) void Name( bool Value ) { g_driver->write<char>( g_driver->read<char>( current_class + Offset) | (1 << Bit), Value); }
#define apply_member_array(type, name, offset) void name(int index, type val) { g_driver->write<type>(current_class + offset + (index * sizeof(type)), val); }

namespace engine {
	class f_text {
	public:
		declare_member(std::uint64_t, data, 0x20);
		declare_member(std::int32_t, length, 0x28);

		std::string get() {
			const auto& __ftext = current_class;
			if (!__ftext) return "";

			const auto& src = this->data();
			if (!src) return "";

			const auto& size = this->length();
			if (size <= 0 || size > 100) return "";
			if (size <= 64) {
				wchar_t stack_buffer[65] = { 0 };
				if (!g_driver->read_memory(src, stack_buffer, size * sizeof(wchar_t))) {
					return "";
				}

				try {
					std::wstring wstr(stack_buffer);
					std::string str(wstr.begin(), wstr.end());
					return str;
				}
				catch (...) {
					return "";
				}
			}

			std::unique_ptr<wchar_t[]> dst(new(std::nothrow) wchar_t[size + 1]);
			if (!dst) return "";

			std::memset(dst.get(), 0, (size + 1) * sizeof(wchar_t));

			if (!g_driver->read_memory(src, dst.get(), size * sizeof(wchar_t))) {
				return "";
			}

			try {
				std::wstring wstr(dst.get());
				std::string str(wstr.begin(), wstr.end());

				if (!str.empty()) {
					str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
						return !std::isspace(ch);
						}));

					str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
						return !std::isspace(ch) && ch != '\0';
						}).base(), str.end());
				}

				return str;
			}
			catch (...) {
				return "";
			}
		}
	};

	class f_string {
	public:
		std::string get() {
			const auto& __ftext = current_class;
			if (!__ftext) return "";

			const auto& src = g_driver->read(__ftext);
			if (!src) return "";

			const auto& size = g_driver->read(__ftext + 0x8);
			if (size <= 0 || size > 100) return "";

			if (size <= 64) {
				wchar_t stack_buffer[65] = { 0 };
				if (!g_driver->read_memory(src, stack_buffer, size * sizeof(wchar_t))) {
					return "";
				}

				try {
					std::wstring wstr(stack_buffer);
					std::string str(wstr.begin(), wstr.end());
					return str;
				}
				catch (...) {
					return "";
				}
			}

			std::unique_ptr<wchar_t[]> dst(new(std::nothrow) wchar_t[size + 1]);
			if (!dst) return "";

			std::memset(dst.get(), 0, (size + 1) * sizeof(wchar_t));

			if (!g_driver->read_memory(src, dst.get(), size * sizeof(wchar_t))) {
				return "";
			}

			try {
				std::wstring wstr(dst.get());
				std::string str(wstr.begin(), wstr.end());

				return str;
			}
			catch (...) {
				return "";
			}
		}
	};

	class u_object {
	public:
		declare_member(std::uintptr_t, vtable, 0x0);
		apply_member(std::uintptr_t, apply_vtable, 0x0);
		declare_member(u_object*, class_private, 0x08);
		declare_member(u_object*, outer_private, 0x18);
		declare_member(std::uint32_t, object_flags, 0x18);
		declare_member(std::uint32_t, object_class, 0x30);
		declare_member(std::uint32_t, name_private, offsets::name_private);

		std::uint64_t get_class(std::uint64_t class_ptr) {
			auto outer_private = this->outer_private();

			std::uint64_t result = 0;
			do {
				if (!outer_private)
					break;
				auto class_private = outer_private->class_private();
				auto v6 = g_driver->read < int >(class_ptr + 0x38);
				if ((int)v6 <= g_driver->read< int >((std::uintptr_t)class_private + 0x38) &&
					g_driver->read(g_driver->read((std::uintptr_t)class_private + 0x30) + 8 * v6) == class_ptr + 0x30)
					result = reinterpret_cast<std::uint64_t>(outer_private);
				outer_private = outer_private->outer_private();
			} while (!result);

			if (result) {
				return g_driver->read(result + 0x68);
			}

			return 0;
		}

		static uint64_t get_objects_array() {
			auto encrypted_objects = g_driver->read<uint64_t>(g_driver->m_base_address + 0x17CB3510);
			return (~encrypted_objects) ^ 0xA8F48EB7LL;
		}

		static uint32_t get_objects_count() {
			auto encrypted_count = g_driver->read<uint32_t>(g_driver->m_base_address + 0x17CB3518);
			return encrypted_count ^ 0xEC67DB7F;
		}

		template <typename type = u_object>
		static type* get_object(int32_t index) {
			if (index > -1 && index < get_objects_count()) {
				auto encrypted_array = get_objects_array();
				auto chunk = g_driver->read<uintptr_t>(encrypted_array + 8 * (index >> 16));
				if (chunk) {
					auto object_item = chunk + 24 * (index & 0xFFFF);
					auto decrypted_object = g_driver->read<uint32_t>(object_item + 8) ^ 0x599B3733;
					auto decrypted_object2 = ((g_driver->read(object_item) >> 32) & 0x3FFF) << 32;
					return reinterpret_cast<type*>(8 * (decrypted_object | decrypted_object2));
				}
			}

			return nullptr;
		}

		bool is_class_default_object() {
			return (object_flags() == 0x00000010);
		}

		template <typename T = u_object>
		T* get_default_object() {
			auto obj_class = this->class_private();
			if (!obj_class) return nullptr;

			for (auto idx = 0; idx < get_objects_count(); idx++) {
				auto obj = get_object<T>(idx);
				if (obj && obj->class_private() == obj_class && obj->is_class_default_object()) {
					return obj;
				}
			}
			return nullptr;
		}

		template <typename type = u_object>
		static type* find_object(std::string object_path) {
			for (auto idx = 0; idx < get_objects_count(); idx++) {
				auto object = get_object< type >(idx);
				auto object_name = f_name::to_string(reinterpret_cast<u_object*>(object));

				if (object_name.find(object_path) != std::string::npos) {
					return object;
				}
			}

			return nullptr;
		}

		template <typename type = u_object>
		static std::vector < std::pair< type*, std::uint32_t > > find_object_references(std::string object_type) {
			std::vector< std::pair< type*, std::uint32_t > > object_list;

			for (auto idx = 0; idx < get_objects_count(); idx++) {
				auto object = get_object< type >(idx);
				auto outer = object->outer_private();
				if (!outer) continue;

				auto outer_name = engine::f_name::to_string(outer);
				if (outer_name.find(object_type) == std::string::npos) continue;

				auto found = std::find_if(object_list.begin(), object_list.end(),
					[object](const std::pair<type*, std::uint32_t>& pair) {
						return pair.first == object;
					});

				if (found == object_list.end()) {
					object_list.emplace_back(object, idx);
				}
			}

			return object_list;
		}
	};

	class f_name {
	public:
		int32_t comparison_index;

		f_name() {
			comparison_index = 0;
		}

		f_name(u_object* object) {
			if (object) {
				comparison_index = object->name_private();
			}
		}

		std::string to_string() const {
			return to_string(comparison_index);
		}

		static std::string to_string(u_object* object) {
			return f_name(object).to_string();
		}

		static std::string to_string(int32_t index) {
			int32_t decrypted_index = decrypt_index(index);

			const uint32_t chunk_index = decrypted_index >> 16;
			const uint16_t name_entry_index = static_cast<uint16_t>(decrypted_index);

			const uint64_t chunk_ptr = g_driver->read<uint64_t>(g_driver->m_base_address + offsets::g_names + (chunk_index + 2) * 8);
			const uint64_t name_pool_chunk = chunk_ptr + (2 * name_entry_index);
			if (!name_pool_chunk) {
				return "";
			}

			const auto pool = g_driver->read<uint16_t>(name_pool_chunk);
			if ((pool & 0xFFC0) == 0x9BC0) {
				return to_string(g_driver->read<int32_t>(name_pool_chunk + 6));
			}

			const uint16_t length = (pool >> 6) ^ 0x26F;
			if (length <= 0 || length > 1024) {
				return "";
			}

			std::vector<char> name_buffer(length);
			g_driver->read_memory(name_pool_chunk + 2, name_buffer.data(), length);
			decrypt_fname(name_buffer.data(), length);
			return std::string(name_buffer.data(), length);
		}

		static int32_t decrypt_index(int32_t index) {
			const uint32_t value = static_cast<uint32_t>(index - 1);
			const uint32_t xor_key = 0x6F01622B;
			const uint32_t fallback_value = 0x90FD9D55;
			const uint32_t xor_result = value ^ xor_key;

			uint32_t decrypted_val;
			if (xor_result != 0xFFFFFFFF) {
				decrypted_val = xor_result + 1;
			}
			else {
				decrypted_val = fallback_value;
			}

			return static_cast<int32_t>(decrypted_val);
		}

		static void decrypt_fname(char* buffer, int32_t length) {
			if (length <= 0 || length > 1024) {
				return;
			}

			std::vector< char > tempBuffer(length);

			int32_t key = length;
			for (int32_t i = 0; i < length; ++i) {
				key = (key * -8607) - 23861180;
				tempBuffer[i] = buffer[i] ^ (static_cast<char>(key) + 74);
			}

			memcpy(buffer, tempBuffer.data(), length);
		}
	};

	struct f_gameplay_tag {
		f_name tag_name;
	};

	class u_field : public u_object {
	public:
		declare_member(u_field*, next, 0x28);
	};

	class u_struct : public u_field {
	public:
		declare_member(u_struct*, super_struct, 0x40);
	};

	class u_script_struct : public u_struct {
	public:
	};

	class u_data_table : public u_object {
	public:
		typedef t_map<f_name, uint8_t*> row_map_type;

		declare_member(u_script_struct*, row_struct, 0x28);
		declare_member(row_map_type, row_map, 0x30);

		template <typename type>
		type* find_row(const std::string& row_name) {
			auto row_map = this->row_map();
			auto row_array = row_map.pairs.sparse_array;
			auto data_pairs = row_array.get_itter();

			for (const auto& data_pair : data_pairs) {
				auto curr_row_name = data_pair.first.to_string();
				if (curr_row_name.find(row_name) != std::string::npos) {
					return reinterpret_cast<type*>(data_pair.second);
				}
			}

			return nullptr;
		}
	};

	class u_curve_table : public u_object {
	public:
	};

	struct f_data_registry_type {
		f_name name;
	};

	struct f_data_table_row_handle {
		u_data_table* data_table;
		f_name row_name;
		uint8_t pad_c[0x4];
	};

	struct f_curve_table_row_handle {
		u_curve_table* curve_table;
		f_name row_name;
		uint8_t pad_c[0x4];
	};

	struct f_scalable_float {
		float value;
		uint8_t pad_4[0x4];
		f_curve_table_row_handle curve;
		f_data_registry_type registry_type;
		uint8_t pad_1c[0xc];
	};

	struct f_gameplay_tag_container {
		tarray<f_gameplay_tag> gameplay_tags;
		tarray<f_gameplay_tag> parent_tags;
	};

	struct f_damage_done_source_info {
		f_gameplay_tag_container tracked_source_tags;
		float damage_amount;
		int32_t num_hits;
	};

	struct f_damage_done_target_info {
		f_gameplay_tag_container tracked_target_tags;
		tarray<f_damage_done_source_info> tracked_source_data;
	};

	struct f_damage_info {
		u_object* damage_causer;
		int32_t damage_amount;
		uint8_t Pad_c[0x4];
		f_gameplay_tag_container source_tags;
	};

	struct f_material_parameter_info {
		f_name name;
		e_material_parameter_association association;
		char pad_9[0x3];
		int32_t index;
	};

	struct f_vector_parameter_value {
		f_material_parameter_info parameter_info;
		flinear_color parameter_value;
		fguid expression_guid;
	};

	struct f_hit_result {
		int32_t face_index;
		float time;
		float distance;
		uint8_t pad_c[0x4];
		f_vector location;
		f_vector impact_point;
		f_vector normal;
		f_vector impact_normal;
		f_vector trace_start;
		f_vector trace_end;
		float penetration_depth;
		int32_t my_item;
		int32_t item;
		char element_index;
		uint8_t b_blocking_hit : 1;
		uint8_t b_start_penetrating : 1;
		uint8_t pad_ae[0x2];
		u_object* phys_material;
		f_actor_instance_handle hit_object_handle;
		u_object* component;
		uint8_t pad_e0[0x10];
		f_name bone_name;
		f_name my_bone_name;
	};

	struct f_collection_parameter_base {
		f_name parameter_name;
		fguid id;
	};

	struct f_collection_vector_parameter : public f_collection_parameter_base {
		f_linear_color default_value;
	};

	class f_fort_ranged_weapon_stats {
	public:
		declare_member(float, spread, 0x1f8);
		declare_member(int32_t, bullets_per_cartridge, 0x27c);
		declare_member(int32_t, min_hits_per_cartridge, 0x280);
		declare_member(float, firing_rate, 0x284);
		declare_member(float, recoil_vert, 0x2bc);
		declare_member(float, recoil_horz, 0x2cc);
		declare_member(float, offset_recoil_factor, 0x320);
		declare_member(float, recoil_bounce_factor, 0x324);
		declare_member(float, recoil_bounce_offset_factor, 0x328);
		declare_member(float, projectile_velocity, 0x3c8);
	};

	class f_fort_base_weapon_stats : public f_fort_ranged_weapon_stats {
	public:
		declare_member(float, reload_time, 0x174);
		declare_member(int32_t, num_individual_bullets_to_reload, 0x180);
		declare_member(int32_t, clip_size, 0x184);
		declare_member(int32_t, initial_clips, 0x18c);
		declare_member(int32_t, cartridge_per_fire, 0x194);
		declare_member(int32_t, ammo_cost_per_fire, 0x198);
		declare_member(int32_t, max_ammo_cost_per_fire, 0x19c);
	};

	class u_sound_base : public u_object {
	public:

	};

	class u_actor_component : public u_object {
	public:
	};

	class u_scene_component : public u_actor_component {
	public:
		declare_member(f_vector, component_velocity, 0x188);
		declare_member(f_vector, relative_location, 0x140);
		declare_member(f_rotator, relative_rotation, 0x158);
		declare_member(f_transform, component_to_world, 0x1E0);
	};

	class u_material : public u_object {
	public:
		void wireframe(bool enable) {
			auto current_byte = g_driver->read<BYTE>(current_class + 0x1d8);
			auto new_byte = enable ? (current_byte | (1 << 7))
				: (current_byte & ~(1 << 7));

			if (current_byte == new_byte) return;

			g_driver->write<BYTE>(current_class + 0x1d8, new_byte);
		}

		void b_disable_depth_test(bool enable) {
			auto current_byte = g_driver->read<BYTE>(current_class + 0x1c8);
			auto new_byte = enable ? (current_byte | (1 << 0))
				: (current_byte & ~(1 << 0));

			if (current_byte == new_byte) return;

			g_driver->write<BYTE>(current_class + 0x1c8, new_byte);
		}
	};

	class u_material_instance : public u_material {
	public:
		declare_member(u_material_instance*, parent, 0x128);
		declare_member(tarray<f_vector_parameter_value>, vector_parameter_values, 0x1a0);

		u_material* get_material() {
			auto parent = this->parent();
			if (parent) {
				parent->get_material();
			}
			return nullptr;
		}
	};

	class u_material_interface : public u_material_instance {
	public:
	};

	class u_material_instance_dynamic : public u_material_instance {
	public:
	};

	class u_primitive_component : public u_scene_component {
	public:
		declare_member(int, custom_depth_stencil_value, 0x2A4);
		apply_member(int, custom_depth_stencil_value, 0x2A4);

		void never_needs_render_update(bool enable) {
			auto current_byte = g_driver->read<BYTE>(current_class + 0x96);
			auto new_byte = enable ? (current_byte | (1 << 0))
				: (current_byte & ~(1 << 0));

			if (current_byte == new_byte) return;

			g_driver->write<BYTE>(current_class + 0x96, new_byte);
		}

		void render_state_dirty(bool enable) {
			auto current_byte = g_driver->read<BYTE>(current_class + 0x95);
			auto new_byte = enable ? (current_byte | (1 << 0))
				: (current_byte & ~(1 << 0));

			if (current_byte == new_byte) return;

			g_driver->write<BYTE>(current_class + 0x95, new_byte);
		}

		void b_render_in_depth_pass(bool enable) {
			auto field_depth = g_driver->read<BYTE>(current_class + 0x26C);
			auto new_depth = enable ? (field_depth | (1 << 6)) : (field_depth & ~(1 << 6));

			if (field_depth == new_depth) return;

			g_driver->write<BYTE>(current_class + 0x26C, new_depth);
		}

		void replicates(bool enable) {
			auto replication = g_driver->read<BYTE>(current_class + 0x5B);
			BYTE new_replication = enable ? (replication | (1 << 3)) : (replication & ~(1 << 3));

			if (replication == new_replication) return;

			g_driver->write<BYTE>(current_class + 0x5B, new_replication);
		}

		void clear_render_custom_depth() {
			auto current_byte = g_driver->read<BYTE>(current_class + 0xC0);
			auto new_byte = current_byte & 0xFB;

			if (current_byte == new_byte) return;

			g_driver->write<BYTE>(current_class + 0xC0, new_byte);
		}

		void set_render_custom_depth(bool enable) {
			auto current_byte = g_driver->read<BYTE>(current_class + 0xC0);
			auto new_byte = enable ? (current_byte | 0x04)
				: (current_byte & 0xFB);

			if (current_byte == new_byte) return;

			g_driver->write<BYTE>(current_class + 0xC0, new_byte);
		}
	};

	class u_mesh_component : public u_primitive_component {
	public:
		declare_member(tarray<u_material_interface*>, override_materials, 0x518);
		declare_member_bit(0, b_cached_material_parameter_indices_are_dirty, 0x548);
		apply_member_bit(0, b_cached_material_parameter_indices_are_dirty, 0x548);

		tarray<u_material_interface*> get_materials() {
			return this->override_materials();
		}

		fbox_sphere_bounds get_bounds() {
			fbox_sphere_bounds bounds{ };

			*reinterpret_cast<__m128*>(&bounds.orgin) = g_driver->read<__m128>((std::uintptr_t)this + 0x108);
			*reinterpret_cast<uint64_t*>(reinterpret_cast<uint8_t*>(&bounds.orgin) + 0x10) = g_driver->read<uint64_t>((std::uintptr_t)this + 0x118);

			*reinterpret_cast<__m128*>(&bounds.box_extent) = g_driver->read<__m128>((std::uintptr_t)this + 0x120);
			*reinterpret_cast<uint64_t*>(reinterpret_cast<uint8_t*>(&bounds.box_extent) + 0x10) = g_driver->read<uint64_t>((std::uintptr_t)this + 0x130);

			bounds.sphere_radius = g_driver->read<double>((std::uintptr_t)this + 0x138);

			return bounds;
		}

		bool set_material(int32_t element_index, u_material_interface* material) {
			auto override_materials = this->override_materials();
			if (!override_materials.is_valid()) return false;

			auto current_material = override_materials.get(element_index);
			if (override_materials.size() <= element_index) return false;
			if (override_materials.is_valid_index(element_index) && (current_material == material)) {
				return false;
			}

			if (material) {
				auto dynamic_material = reinterpret_cast<u_material_instance*>(material);
				if (!((dynamic_material != nullptr && dynamic_material->parent() == current_material) || current_material == nullptr)) {
					b_cached_material_parameter_indices_are_dirty(true);
				}

				override_materials.set(element_index, material);

				this->render_state_dirty(true);
				return true;
			}

			return false;
		}
	};

	class u_skeletal_mesh_socket : public u_object {
	public:
		declare_member(f_name, socket_name, 0x28);
		declare_member(f_name, bone_name, 0x2c);
		declare_member(f_vector, relative_location, 0x30);
	};

	class f_reference_skeleton : public u_object {
	public:
		declare_member(__int64, final_name_to_index_map, 0x90);

		std::uint32_t get_bone_index(f_name socket_name) {
			auto name_to_index = this->final_name_to_index_map();
			if (!name_to_index) return 0;

			auto name_elememt = name_to_index + (0x10LL * f_name::decrypt_index(socket_name.comparison_index));
			if (name_elememt) {
				auto v9 = name_elememt + 4;
				auto v10 = -name_elememt;

				if ((v9 & -(__int64)(v10 != 0)) != 0)
					return g_driver->read< unsigned int >(v9 & -(__int64)(v10 != 0));
			}
		}
	};

	class u_skeleton : public u_object {
	public:
		declare_member(f_reference_skeleton*, ref_skeleton, 0x48);
	};

	class u_skinned_assest : public u_object {
	public:
		declare_member(f_reference_skeleton*, ref_skeleton, 0x248);
	};

	struct f_mesh_uv_channel_info {
		bool b_initialized;
		bool b_override_densities;
		uint8_t pad_2[0x2];
		float local_uv_densities[0x4];
	};

	struct f_skeletal_material {
		u_material_interface* material_interface;
		f_name material_slot_name;
		f_mesh_uv_channel_info uv_channel_data;
		u_material_interface* overlay_material_interface;
	};

	class u_skeletal_mesh : public u_skinned_assest {
	public:
		declare_member(u_skeleton*, skeleton, 0xf8);
		declare_member(tarray< f_skeletal_material >, materials, 0x1a0);
		declare_member(tarray< u_skeletal_mesh_socket* >, sockets, 0x518);
	};

	class u_skinned_mesh_component : public u_mesh_component {
	public:
		declare_member(std::int32_t, is_cached, 0x5E8 + 0x48);
		declare_member(tarray<f_transform>, bone_array, 0x5E8 + (this->is_cached() * 0x10));
		declare_member(u_skeletal_mesh*, skeletal_mesh, 0x570);
		declare_member(u_skeletal_mesh*, skinned_asset, 0x578);

		bool is_visible(u_world* g_world) {
			auto delta_time_seconds = g_driver->read<float>((std::uint64_t)g_world + 0x86C);
			auto time_seconds = g_driver->read<double>((std::uint64_t)g_world + 0x860 );

			auto last_render_time = g_driver->read<float>((std::uint64_t)this + 0x328);
			auto time_since = time_seconds - last_render_time;

			auto render_time_threshold = fmaxf(0.06f, delta_time_seconds + 0.000099999997);
			return time_since <= render_time_threshold;
		}

		u_skeletal_mesh* get_skinned_assest() {
			auto skeletal_mesh = this->skeletal_mesh();
			return skeletal_mesh ? skeletal_mesh : this->skinned_asset();
		}

		f_vector get_bone_location(tarray < f_transform > bone_array, std::uint32_t bone_index) {
			auto component_to_world = this->component_to_world();
			component_to_world.fix_scaler();

			auto bone = bone_array.get(bone_index);
			auto matrix = bone.to_matrix().to_multiplication(
				component_to_world.to_matrix()
			);

			return matrix.w_plane;
		}

		f_rotator get_bone_rotation(tarray < f_transform > bone_array, std::uint32_t bone_index) {
			auto component_to_world = this->component_to_world();
			component_to_world.fix_scaler();

			auto bone = bone_array.get(bone_index);
			auto matrix = bone.to_matrix().to_multiplication(
				component_to_world.to_matrix()
			);

			float pitch, yaw, roll;
			pitch = atan2(-matrix.m[2][1], sqrt(matrix.m[0][1] * matrix.m[0][1] + matrix.m[1][1] * matrix.m[1][1]));
			yaw = atan2(matrix.m[0][1], matrix.m[1][1]);
			roll = atan2(matrix.m[2][0], matrix.m[2][2]);

			pitch = pitch * (180.0f / std::numbers::pi);
			yaw = yaw * (180.0f / std::numbers::pi);
			roll = roll * (180.0f / std::numbers::pi);

			return f_rotator(pitch, yaw, roll);
		}
	};

	class u_skeletal_mesh_component : public u_skinned_mesh_component {
	public:
	};

	class a_actor : public u_object {
	public:
		declare_member(u_scene_component*, root_component, 0x1B0);
		declare_member(a_actor*, instigator, 0x198);
		declare_member(a_actor*, owner, 0x158);

		void clear_cham() {
			auto current_byte = g_driver->read<BYTE>(current_class + 0xC0);
			auto new_byte = current_byte & 0xFB;

			if (current_byte == new_byte) return;

			g_driver->write<BYTE>(current_class + 0xC0, new_byte);
		}

		void set_cham(bool enable) {
			auto current_byte = g_driver->read<BYTE>(current_class + 0xC0);
			auto new_byte = enable ? (current_byte | 0x04)
				: (current_byte & 0xFB);

			if (current_byte == new_byte) return;

			g_driver->write<BYTE>(current_class + 0xC0, new_byte);
		}

	};

	class a_controller : public a_actor {
	public:
	};

	class u_fort_player_state_component_habanero : public a_actor {
	public:
		declare_member(int32_t, rank_progress, 0xD8 + 0x10);
	};

	class u_fort_item_definition : public a_actor {
	public:
		declare_member(e_fort_rarity, rarity, 0xAA);
		declare_member(f_text*, item_name, 0x40);
	};

	class u_fort_world_item_definition : public u_fort_item_definition {
	public:
	};

	class u_fort_weapon_item_definition : public u_fort_world_item_definition {
	public:
		declare_member(f_data_table_row_handle, weapon_stat_handle, 0x128);

		f_fort_base_weapon_stats* get_weapon_stats() {
			auto row_handle = this->weapon_stat_handle();
			auto data_table = row_handle.data_table;
			if (!data_table) return nullptr;

			auto row_name = row_handle.row_name.to_string();
			return data_table->find_row<f_fort_base_weapon_stats>(row_name);
		}
	};

	class u_fort_weapon_anim_set {
	public:
	};

	class f_fort_ranged_weapon_recoil {
	public:
	};

	class u_fort_weapon_mod_runtime_data : public u_object {
	public:
	};

	class u_weapon_hit_notify_audio_bank : public u_object {
	public:
		declare_member(u_sound_base*, sound_body, 0x30);
		declare_member(u_sound_base*, sound_crit, 0x38);
		declare_member(u_sound_base*, sound_shield, 0x40);
		declare_member(u_sound_base*, sound_death, 0x48);
		declare_member(u_sound_base*, sound_death_crit, 0x50);
		declare_member(u_sound_base*, sound_body_receive, 0x58);
		declare_member(u_sound_base*, sound_shield_receive, 0x60);
		declare_member(u_sound_base*, sound_crit_receive, 0x68);
		declare_member(u_sound_base*, sound_death_receive, 0x70);
		declare_member(u_sound_base*, sound_death_crit_receive, 0x78);
		declare_member(u_sound_base*, sound_fall_receive, 0x80);
		declare_member(u_sound_base*, sound_fall_death_receive, 0x88);
	};

	class a_fort_weapon : public a_actor {
	public:
		declare_member(u_weapon_hit_notify_audio_bank*, hit_notify_audio_bank, 0x5b0);
		declare_member(float, projectile_speed, offsets::projectile_speed);
		declare_member(float, projectile_gravity, offsets::projectile_gravity);
		declare_member(float, current_projected_impact_distance, 0x1358);
		declare_member(u_fort_weapon_item_definition*, weapon_data, 0x580);
		declare_member(int, ammo_count, 0x14d4);
		declare_member(e_weapon_core_animation, weapon_core_animation, 0x1a28);
		declare_member(u_skeletal_mesh_component*, weapon_mesh, 0x1288);
		declare_member_bit(6, b_is_targeting, 0x1428);
		declare_member_bit(0, b_is_reloading_weapon, 0x3b9);
		apply_member(u_sound_base*, primary_fire_sound_1p, 0xcc8);
		apply_member(u_sound_base*, primary_fire_stop_sound_1p, 0xce8);
		apply_member_array(u_sound_base*, primary_fire_sound, 0xD28);
		apply_member_array(u_sound_base*, primary_fire_stop_sound, 0xD48);
		apply_member_array(u_sound_base*, secondary_fire_sound, 0xD60);
		apply_member_array(u_sound_base*, secondary_fire_stop_sound, 0xD78);
		apply_member_array(u_sound_base*, impact_physical_surface_sounds, 0xDE0);

		bool is_firing() {
			auto u_level = g_driver->read(g_driver->m_base_address + 0x17C37BE8);
			auto v4 = this->get_class(u_level);
			if (v4) {
				auto a = g_driver->read<float>((std::uintptr_t)this + 0x68);
				auto b = g_driver->read<double>(v4 + 0x848);
				auto c = g_driver->read<float>((std::uintptr_t)this + 0x115C);

				return a >= b - c;
			}

			return false;
		}

		u_object* get_lightweight_config() {
			auto fort_weapon_ranged_item_definition = g_driver->read(g_driver->m_base_address + 0x17DE76F8);

			auto lightweight_config = g_driver->read<u_object*>((std::uint64_t)this + 0x570);
			if (!lightweight_config)
				return 0;

			auto class_private = lightweight_config->class_private();
			auto v6 = g_driver->read < int >(fort_weapon_ranged_item_definition + 0x38);
			if ((int)v6 > g_driver->read< int >((std::uintptr_t)class_private + 0x38) ||
				g_driver->read(g_driver->read((std::uintptr_t)class_private + 0x30) + 8 * v6) != fort_weapon_ranged_item_definition + 0x30)
				return 0;

			return lightweight_config;
		}
	};

	struct f_item_component_data_list {
		uint8_t pad[0x108];
	};

	struct f_item_entry : f_fast_array_serializer_item {
		class u_fort_item_definition* item_definition;
		fguid item_entry_id;
		int32_t slot_number;
		f_item_component_data_list item_data;
		tarray<class u_script_struct*> removed_data_types;
		u_object* parent_object;
	};

	struct f_fort_item_entry : f_item_entry {
		int32_t previous_count;
	};

	class a_building_sm_actor : public a_actor {
	public:
		declare_member(u_mesh_component*, static_mesh_component, 0x830);
	};

	class a_building_container : public a_building_sm_actor {
	public:
		declare_member(f_text*, search_text, 0xDA8);
		declare_member(e_fort_pickup_spawn_source, spawn_source_override, 0xBE8);
		declare_member_bit(2, b_already_searched, 0xD52);

		bool is_container() {
			auto search_text = this->search_text()->get();
			if (search_text.contains("Search"))
				return true;
			return false;
		}
	};

	class a_building_weakspot : public a_actor {
	public:
		declare_member(int, max_level, 0x2EC);
		declare_member_bit(0, b_hit, 0x2E0);
		declare_member_bit(2, b_active, 0x2E0);

		bool is_weakspot() {
			auto max_level = this->max_level();
			if (max_level != 7)
				return false;
			return true;
		}
	};

	class a_fort_pickup : public a_actor {
	public:
		declare_member(float, simulating_too_long_length, 0x2D0);
		declare_member(f_fort_item_entry, primary_pickup_item_entry, 0x3A8);

		bool is_pickup() {
			auto simulating_too_long_length = this->simulating_too_long_length();
			if (simulating_too_long_length != 30.f)
				return false;
			return true;
		}
	};

	class a_fort_projectile_base : public a_actor {
	public:
		declare_member(float, max_lifespan_on_stop, 0xB34);
		declare_member(f_hit_result, cached_impact_result, 0xA30);

		bool is_projectile() {
			auto max_lifespan_on_stop = this->max_lifespan_on_stop();
			if (max_lifespan_on_stop != 5.f)
				return false;
			return true;
		}
	};

	class a_fort_motorcycle_vehicle : public a_actor {
	public:
		apply_member(float, max_charge_jump_force, 0xA98);
		apply_member(float, max_jump_forward_angle, 0xAA0);
		apply_member(float, max_jump_charge_time, 0xA90);
		apply_member(float, jump_charge_grace_time_seconds, 0xAD8);
		apply_member(float, jump_grace_time_seconds, 0xADC);
	};

	struct a_fort_vehicle_fuel_component : public a_actor {
	public:
		apply_member(float, server_fuel, 0x130);
		apply_member(float, infinite_fuel, 0x2A0);
	};

	class a_fort_athena_vehicle : public a_actor {
	public:
		declare_member(a_fort_vehicle_fuel_component*, cached_fuel_component, 0x1390);
		declare_member(a_fort_motorcycle_vehicle*, motorcycle_vehicle_config, 0x2378);
		apply_member(float, vehicle_gravity_scale, 0x1368 + 0x18);
		declare_member(float, lifespan_after_death, 0x1140);
		declare_member(float, server_critical_health, 0x1D84);
		declare_member(bool, b_on_road, 0x1DC8);
		declare_member(bool, b_on_landscape, 0x1DC9);
		declare_member(bool, b_on_dirt, 0x1DCA);
		declare_member(bool, b_on_grass, 0x1DCB);
		declare_member(bool, b_on_ice, 0x1DCC);
		declare_member(bool, b_on_snow, 0x1DCD);
		declare_member(bool, b_on_mud, 0x1DCE);
		declare_member_bit(1, b_exploded, 0x1D8C);

		bool is_vehicle() {
			auto lifespan_after_death = this->lifespan_after_death();
			if (lifespan_after_death != 0.5f)
				return false;
			return true;
		}
	};

	class a_building_actor : public a_building_sm_actor {
	public:
		declare_member(float, snap_grid_size, 0x610);
		declare_member(float, vert_snap_grid_size, 0x614);
		declare_member(e_fort_building_type, building_type, 0x4E9);
		declare_member_bit(6, b_player_placed, 0x4E2);
		declare_member_bit(5, b_editor_placed, 0x4E2);
		declare_member_bit(6, b_destroyed, 0x4DE);

		bool is_building() {
			auto vert_snap_grid_size = this->vert_snap_grid_size();
			if (vert_snap_grid_size != 384.f)
				return false;
			return true;
		}
	};

	class a_fort_athena_supply_drop : public a_building_actor {
	public:
		declare_member(int, num_players_interacting, 0xB30);
		declare_member(float, in_storm_despawn_time_in_seconds, 0xB00);

		bool is_supply_drop() {
			auto despawn_time_in_seconds = this->in_storm_despawn_time_in_seconds();
			if (despawn_time_in_seconds != 1.0)
				return false;
			return true;
		}
	};

	class a_athena_creative_spawn_drop_c : public a_fort_athena_supply_drop {
	public:
		declare_member(f_name, loot_table_name, 0xB78);
		declare_member(bool, looted, 0xBC0);

		bool is_loot_llama() {
			auto loot_table_name = this->loot_table_name().to_string();
			if (loot_table_name.contains("Loot_AthenaLlama"))
				return true;
			return false;
		}
	};

	class a_fort_projectile_athena : public a_fort_projectile_base {
	public:
		declare_member(f_hit_result, pawn_hit_result, 0xB78);
		declare_member(f_vector, fire_start_loc, 0xC70);
	};

	class a_player_state : public a_actor {
	public:
		declare_member(a_fort_player_pawn_athena*, pawn_private, 0x328);
		declare_member(tarray<a_player_state*>, spectators, 0xA98 + 0x108);
		declare_member(f_string*, platform, 0x440);
		declare_member(std::uint32_t, player_id, 0x2B4);
		declare_member(std::uint32_t, initial_squad_size, 0x1960);
		declare_member_bit(3, b_is_a_bot, 0x2BA);
		declare_member_bit(5, b_is_inactive, 0x2BA);

		std::string get_platform_name() {
			const auto& state = current_class;
			if (!state) return "";

			int size = g_driver->read<int>(current_class + 0x440 + 0x8);
			if (size <= 0 || size > 50) return "";

			std::uint64_t src = g_driver->read<std::uint64_t>(current_class + 0x440);
			if (!src) return "";

			if (size <= 32) {
				wchar_t stack_buffer[33] = { 0 };
				if (!g_driver->read_memory(src, stack_buffer, size * sizeof(wchar_t))) {
					return "";
				}

				try {
					std::wstring conversion(stack_buffer);
					return std::string(conversion.begin(), conversion.end());
				}
				catch (...) {
					return "";
				}
			}

			std::unique_ptr<wchar_t[]> dst(new (std::nothrow) wchar_t[size + 1]);
			if (!dst) return "";

			if (!g_driver->read_memory(src, dst.get(), size * sizeof(wchar_t))) {
				return "";
			}

			try {
				dst[size] = L'\0';

				std::wstring conversion(dst.get());
				return std::string(conversion.begin(), conversion.end());
			}
			catch (...) {
				return "";
			}
		}

		std::string get_player_name(bool is_lobby) {
			int v25;
			char v21;
			int v22;
			__int64 v6;

			std::uint64_t src;
			int size;

			const auto& state = current_class;
			if (state) {
				if (is_lobby) {
					size = g_driver->read<int>(current_class + 0x348 + 0x8);
					if (!size || size > 100)
						return "Bot";

					src = g_driver->read<std::uint64_t>(current_class + 0x348);
					if (!src)
						return "Bot";
				}
				else {
					const auto& __routine = g_driver->read<std::uint64_t>(current_class + offsets::player_name);
					if (!__routine)
						return "Bot";

					size = g_driver->read<int>(__routine + 0x10);
					if (!size || size > 100)
						return "Bot";

					src = g_driver->read<std::uint64_t>(__routine + 0x8);
					if (!src)
						return "Bot";
				}

				if (size <= 0 || size > 50) return "Bot";

				if (size <= 32) {
					wchar_t stack_buffer[33] = { 0 };
					if (!g_driver->read_memory(src, stack_buffer, size * sizeof(wchar_t))) {
						return "Bot";
					}

					try {
						v6 = size;
						v21 = v6 - 1;
						v22 = 0;

						if (v6 == 0)
							v21 = 0;

						_WORD* v23 = (_WORD*)stack_buffer;

						for (int i = v21 & 3;; *v23++ += i & 7) {
							v25 = v6 - 1;

							if (v6 == 0)
								v25 = 0;

							if (v22 >= v25)
								break;

							i += 3;
							++v22;
						}

						std::wstring conversion(stack_buffer);
						return std::string(conversion.begin(), conversion.end());
					}
					catch (...) {
						return "Bot";
					}
				}

				std::unique_ptr<wchar_t[]> dst(new (std::nothrow) wchar_t[size + 1]);
				if (!dst) return "Bot";

				if (!g_driver->read_memory(src, dst.get(), size * sizeof(wchar_t))) {
					return "Bot";
				}

				try {
					dst[size] = L'\0';

					v6 = size;
					v21 = v6 - 1;
					v22 = 0;

					if (v6 == 0)
						v21 = 0;

					_WORD* v23 = (_WORD*)(dst.get());

					for (int i = v21 & 3;; *v23++ += i & 7) {
						v25 = v6 - 1;

						if (v6 == 0)
							v25 = 0;

						if (v22 >= v25)
							break;

						i += 3;
						++v22;
					}

					std::wstring conversion(dst.get());
					return std::string(conversion.begin(), conversion.end());
				}
				catch (...) {
					return "Bot";
				}
			}

			return "Bot";
		}

		std::string get_spectator_name() {
			auto f_string = g_driver->read<__int64>(current_class + offsets::player_name);
			auto length = g_driver->read<int>(f_string + 16);
			auto f_text = g_driver->read<__int64>(f_string + 8);

			if (length <= 0 || length > 50) {
				return "Bot";
			}

			if (length <= 32) {
				wchar_t stack_buffer[33] = { 0 };
				if (!g_driver->read_memory(f_text, stack_buffer, length * sizeof(wchar_t))) {
					return "Bot";
				}

				try {
					char v21 = length - 1;
					if (!(DWORD)length) v21 = 0;
					int v22 = 0;
					WORD* v23 = (WORD*)stack_buffer;
					for (int i = (v21) & 3; ; *v23++ += i & 7) {
						int v25 = length - 1;
						if (!(DWORD)length) v25 = 0;
						if (v22 >= v25) break;
						i += 3;
						++v22;
					}

					std::wstring username_w(stack_buffer);
					std::string username(username_w.begin(), username_w.end());
					return username;
				}
				catch (...) {
					return "Bot";
				}
			}

			std::unique_ptr<wchar_t[]> buffer(new(std::nothrow) wchar_t[length + 1]);
			if (!buffer) {
				return "Bot";
			}

			if (!g_driver->read_memory(f_text, buffer.get(), length * sizeof(wchar_t))) {
				return "Bot";
			}

			try {
				buffer[length] = L'\0';

				char v21;
				int v22;
				int i;
				int v25;
				WORD* v23;

				v21 = length - 1;
				if (!(DWORD)length)
					v21 = 0;
				v22 = 0;
				v23 = (WORD*)buffer.get();
				for (i = (v21) & 3; ; *v23++ += i & 7) {
					v25 = length - 1;
					if (!(DWORD)length)
						v25 = 0;
					if (v22 >= v25)
						break;
					i += 3;
					++v22;
				}

				std::wstring username_w(buffer.get());
				std::string username(username_w.begin(), username_w.end());
				return username;
			}
			catch (...) {
				return "Bot";
			}
		}
	};

	class a_fort_player_state : public a_player_state {
	public:
		declare_member(u_fort_player_state_component_habanero*, habanero_component, 0x940);
	};

	class a_fort_player_state_zone : public a_fort_player_state {
	public:
	};

	class a_fort_player_state_athena : public a_fort_player_state_zone {
	public:
		declare_member(std::int32_t, kill_score, 0x11C0);
		declare_member(std::int32_t, human_kill_score, 0x11CC);
		declare_member(std::int32_t, season_level_ui_display, 0x11C4);
		declare_member(std::int32_t, team_index, 0x11A9);

		bool is_teammate(a_fort_player_state_athena* player_state) {
			if (player_state) {
				auto team_index = player_state->team_index();
				return this->team_index() == team_index;
			}
			return false;
		}
	};

	class a_pawn : public a_actor {
	public:
		declare_member(a_fort_player_state_athena*, player_state, 0x2d0);
		declare_member(a_player_controller*, controller, 0x2E0);
	};

	class u_character_movement_component : public a_actor {
	public:
		declare_member(e_movement_mode, movement_mode, 0x231);
		declare_member(f_vector, last_update_velocity, 0x378);
	};

	class a_character : public a_pawn {
	public:
		declare_member(tarray<u_skeletal_mesh_component*>, skeletal_meshes, 0x60B0);
		declare_member(tarray<u_material_instance_dynamic*>, pawn_materials_all, 0x60C0);
		declare_member(u_character_movement_component*, character_movement, 0x338);
		declare_member(u_skeletal_mesh_component*, mesh, 0x330);
		declare_member_bit(1, b_is_crouched, 0x440);
		declare_member_bit(4, b_is_sliding, 0x72b);
		declare_member_bit(0, b_is_slope_sliding, 0x88C);
		declare_member_bit(2, b_is_invulnerable, 0x72A);
	};

	class afgf_character : public a_character {
	public:
	};

	class a_fort_pawn : public afgf_character {
	public:
		declare_member(a_fort_weapon*, current_weapon, 0x990);
		declare_member_bit(5, b_is_dying, 0x728);
		declare_member_bit(7, b_is_dbno, 0x841);
	};

	class f_replicated_athena_vehicle_attributes {
	public:
	};

	class a_fort_player_pawn : public a_fort_pawn {
	public:
		declare_member(bool, b_is_skydiving, 0x228F);
		declare_member(bool, b_is_parachute_open, 0x2290);
		declare_member(u_sound_base*, dbno_landing_sound, 0xCF0); 
		declare_member(u_sound_base*, glider_open_sound, 0x4340); 
	};

	class a_fort_player_pawn_athena : public a_fort_player_pawn {
	public:
		declare_member(a_fort_athena_vehicle*, current_vehicle, 0x2C18);
		declare_member(f_vector, last_fired_location, 0x59B0 );
		declare_member(f_vector, last_fired_direction, 0x59C8);
		declare_member(float, revive_from_dbno_time, 0x4DE8);

		bool is_player() {
			auto revive_from_dbno_time = this->revive_from_dbno_time();
			if (revive_from_dbno_time != 10.f)
				return false;
			return true;
		}
	};

	struct f_slotted_item_cache {

	};

	struct f_quick_bar {
		int32_t current_focused_slot;
		int32_t previous_focused_slot;
		int32_t secondary_focused_slot;
		bool has_been_focused_before;
		tarray<f_quick_bar_slot> slots;
		f_quick_bar_data data_definition;
		t_set<u_fort_item_definition*> equipped_item_definitions;
		tarray<int32_t> shared_visible_slot_indices_when_using_gamepad;
		tarray<int32_t> hidden_slot_indices;
		tarray<f_slotted_item_cache> slotted_items_cache;
	};

	class a_fort_quick_bars : public a_actor {
	public:
		declare_member(f_quick_bar, primary_quick_bar, 0x2B8);
		declare_member(f_quick_bar, secondary_quick_bar, 0x368);
		declare_member(f_quick_bar, creative_quick_bar, 0x418);
	};

	class u_material_parameter_collection : public u_object {
	public:
		declare_member(tarray< f_collection_vector_parameter >, vector_parameters, 0x48);
	};

	class a_player_camera_manager : public a_actor {
	public:
		apply_member(float, locked_fov, 0x2c0);
	};

	class a_player_controller : public a_controller {
	public:
		apply_member(f_rotator, player_aim_offset, 0x2BC0 );
		apply_member(f_rotator, additional_aim_offset, 0x2BA8 );
		declare_member(a_fort_player_pawn_athena*, acknowledged_pawn, 0x358);
		declare_member(a_player_camera_manager*, player_camera_manager, 0x368);
		declare_member(a_fort_quick_bars*, client_quick_bars, 0x1AC8 );
		declare_member(bool, b_is_local_player_controller, 0x6CC);
	};

	
	class u_scene_view_state : public u_object {
	public:
		declare_member(f_matrix, projection, 0x940);
		declare_member(double, field_of_view, 0x740);
	};

	class u_game_viewport_client : public u_object {
	public:
	};

	class u_player : public u_object {
	public:
		declare_member(a_player_controller*, player_controller, 0x30);
	};

	class u_localplayer : public u_player {
	public:
		declare_member(tarray<u_scene_view_state*>, view_state, 0xd0);
		declare_member(u_game_viewport_client*, viewport_client, 0x78);

		u_scene_view_state* get_view_state() {
			return view_state().get(1);
		}
	};

	class u_level : public u_object {
	public:
		declare_member(tarray<a_actor*>, actors, offsets::actors);
	};

	class u_game_instance : public u_object {
	public:
		declare_member(tarray<u_localplayer*>, localplayers, 0x38);

		auto get_localplayer() -> u_localplayer* {
			return this->localplayers().get(0);
		}
	};

	
	class a_fort_safe_zone_indicator : public a_actor {
	public:
		declare_member(f_vector, next_center, 0x2d8);
		declare_member(f_vector, next_next_center, 0x2f0);
		declare_member(float, radius, 0x518);
		declare_member(float, next_radius, 0x2B4);
		declare_member(float, next_next_radius, 0x2b8);
		declare_member(float, safe_zone_height, 0x330);
		declare_member(int32_t, current_phase, 0x500);
		declare_member(f_fort_safe_zone_damage_info, current_damage_info, 0x504);
		declare_member(int32_t, phase_count, 0x50c);
		declare_member(float, safe_zone_finish_shrink_time, 0x514);
	};

	
	class u_fort_game_state_component_battle_royale_game_phase_logic : public u_object {
	public:
		declare_member(a_fort_safe_zone_indicator*, safe_zone_indicator, 0x2F0);
	};

	class u_athena_game_phase_logic_c : public u_fort_game_state_component_battle_royale_game_phase_logic {
	public:
	};

	class a_game_state_base : public u_object {
	public:
		declare_member(float, server_world_time_seconds_delta, 0x2E8); 
		declare_member(tarray<u_object*>, components, 0x228); 
		declare_member(tarray<a_player_state*>, player_array, 0x2c8);

		u_athena_game_phase_logic_c* get_game_phase() {
			return reinterpret_cast<u_athena_game_phase_logic_c*>(components().get(2));
		}
	};

	class u_world : public u_object {
	public:
		declare_member(double, world_time_seconds, offsets::world_time_seconds); 
		declare_member(a_game_state_base*, game_state, 0x1D0 );
		declare_member(u_game_instance*, owning_game_instance, 0x248 );
		declare_member(u_level*, persistent_level, 0x40); 
		declare_member(tarray<u_level*>, levels, offsets::levels);

		static u_world* get() {
			return g_driver->read<u_world*>(g_driver->m_base_address + offsets::g_world);
		}
	};
}