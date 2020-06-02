#pragma once
#include "../basic/renderable.h"
#include "../math.h"

namespace d3d12 {
	
	template<typename _Ty>
	struct dynamic_mesh : public clmagic::mesh {
		virtual void release() override {
			_My_vertices = nullptr;
			_My_indices  = nullptr;
		}
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList& _Cmdlist) override {
			_Cmdlist.IASetVertexBuffers(0, 1, &this->get_D3D12_VERTEX_BUFFER_VIEW());
			_Cmdlist.IASetIndexBuffer(&this->get_D3D12_INDEX_BUFFER_VIEW());
			_Cmdlist.DrawIndexedInstanced(this->index_count, 1, this->start_index_location, this->base_vertex_location, 0);
			return _Cmdlist;
		}
		virtual std::shared_ptr<mesh> reduce(size_t _Base_vertex_location, size_t _Start_index_location, size_t _Index_count) override {
			std::shared_ptr<dynamic_mesh> _Submesh = std::make_shared<dynamic_mesh>();
			_Submesh->_My_vertices = _My_vertices;
			_Submesh->_My_indices  = _My_indices;
			_Submesh->base_vertex_location = this->base_vertex_location + _Base_vertex_location;
			_Submesh->start_index_location = this->start_index_location + _Start_index_location;
			_Submesh->index_count          = _Index_count;
			return _Submesh;
		}

		D3D12_VERTEX_BUFFER_VIEW get_D3D12_VERTEX_BUFFER_VIEW() const {
			return make_D3D12_VERTEX_BUFFER_VIEW<_Ty>(_My_vertices->ref(), _My_vertices->size());
		}
		D3D12_INDEX_BUFFER_VIEW get_D3D12_INDEX_BUFFER_VIEW() const {
			return make_D3D12_INDEX_BUFFER_VIEW<uint32_t>(_My_indices->ref(), _My_indices->size());
		}

		std::shared_ptr<dynamic_buffer<_Ty>>      _My_vertices;
		std::shared_ptr<dynamic_buffer<uint32_t>> _My_indices;
	};

	template<typename _Ty>
	struct static_mesh : public clmagic::mesh {
		static_mesh() = default;
		static_mesh(const dynamic_mesh<_Ty>& _Right) {
			this->copy_from(_Right);
		}
		
		virtual void release() override {
			_My_vertices = nullptr;
			_My_indices  = nullptr;
		}
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList& _Cmdlist) override {
			_Cmdlist.IASetVertexBuffers(0, 1, &this->get_D3D12_VERTEX_BUFFER_VIEW());
			_Cmdlist.IASetIndexBuffer(&this->get_D3D12_INDEX_BUFFER_VIEW());
			_Cmdlist.DrawIndexedInstanced(this->index_count, 1, this->start_index_location, this->base_vertex_location, 0);
			return _Cmdlist;
		}
		virtual std::shared_ptr<mesh> reduce(size_t _Base_vertex_location, size_t _Start_index_location, size_t _Index_count) override {
			std::shared_ptr<static_mesh> _Submesh = std::make_shared<static_mesh>();
			_Submesh->_My_vertices = _My_vertices;
			_Submesh->_My_indices  = _My_indices;
			_Submesh->base_vertex_location = this->base_vertex_location + _Base_vertex_location;
			_Submesh->start_index_location = this->start_index_location + _Start_index_location;
			_Submesh->index_count          = _Index_count;
			return _Submesh;
		}

	private:
		template<typename _Meshty>
		void _Copy_from(const _Meshty& _Right) {
			_My_vertices = std::make_shared(_Right._My_vertices);
			_My_indices  = std::make_shared(_Right._My_indices);
			this->base_vertex_location = _Right.base_vertex_location;
			this->start_index_location = _Right.start_index_location;
			this->index_count = _Right.index_count;
		}
		template<typename _Meshty>
		void _Merge_from(const _Meshty& _Right) {
			if (_My_vertices == nullptr) {
				this->copy_from(_Right);
			} else {
				/*auto _Device      = get_device(_Right._My_indices->get());
				auto _Add_indices = dynamic_buffer<uint32_t>(_Device, _Right._My_indices->size());
				_Add_indices*/
				_My_vertices->merge_from(_Right._My_vertices);
				_My_indices->merge_from(_Right._My_indices);
			}
		}
		
	public:
		void copy_from(const dynamic_mesh<_Ty>& _Right) {
			this->_Copy_from(_Right);
		}
		void copy_from(const static_mesh<_Ty>& _Right) {
			this->_Copy_from(_Right);
		}

		D3D12_VERTEX_BUFFER_VIEW get_D3D12_VERTEX_BUFFER_VIEW() {
			return make_D3D12_VERTEX_BUFFER_VIEW<_Ty>(_My_vertices->ref(), _My_vertices->size());
		}
		D3D12_INDEX_BUFFER_VIEW get_D3D12_INDEX_BUFFER_VIEW() {
			return make_D3D12_INDEX_BUFFER_VIEW<uint32_t>(_My_indices->ref(), _My_indices->size());
		}

		std::shared_ptr<static_buffer<_Ty>>      _My_vertices;
		std::shared_ptr<static_buffer<uint32_t>> _My_indices;
	};
	
	template<size_t _Prop_param_index, typename _Ts, typename _Tb = _Ts>
	struct surface_substance : public clmagic::substance {
		using surface_type = clmagic::surface<_Ts, _Tb>;
		
		surface_substance() = default;
		surface_substance(surface_type _Val)
			: subsurface_albedo(_Val.subsurface_albedo), roughness_x(_Val.roughness_x), reflect_value(_Val.reflect_value), roughness_y(_Val.roughness_y) {}
		surface_substance(VECTOR3 _Albedo, VECTOR3 _Reflect, SCALAR _Roughness_x, SCALAR _Roughness_y)
			: subsurface_albedo(_Albedo), roughness_x(_Roughness_x), reflect_value(_Reflect), roughness_y(_Roughness_y) {}

		virtual void release() override {}
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList& _Cmdlist) override {
			_Cmdlist.SetGraphicsRoot32BitConstants(_Prop_param_index, 8, &(this->subsurface_albedo), 0);
			return _Cmdlist;
		}

		VECTOR3 subsurface_albedo;
		SCALAR  roughness_x;
		VECTOR3 reflect_value;
		SCALAR  roughness_y;// anisotripic
	};

	template<size_t _Talbedo_param_index, size_t _Prop_param_index, typename _Ts, typename _Tb = _Ts>
	struct subsurface_texture_substance : public clmagic::substance {
		subsurface_texture_substance() = default;
		subsurface_texture_substance(D3D12_GPU_VIRTUAL_ADDRESS _Albedo, VECTOR3 _Reflect, SCALAR _Roughness_x, SCALAR _Roughness_y)
			: subsurface_albedo(_Albedo), reflect_value(_Reflect), roughness_x(_Roughness_x), roughness_y(_Roughness_y) {}

		virtual void release() override {}
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList & _Cmdlist) override {
			_Cmdlist.SetGraphicsRootShaderResourceView(_Talbedo_param_index, subsurface_albedo);
			_Cmdlist.SetGraphicsRoot32BitConstants(_Prop_param_index, 5, &(this->reflect_value), 0);
			return _Cmdlist;
		}

		D3D12_GPU_VIRTUAL_ADDRESS subsurface_albedo;
		VECTOR3 reflect_value;
		SCALAR  roughness_x;
		SCALAR  roughness_y;// anisotripic
	};

	template<size_t _Talbedo_param_index, size_t _Troughness_param_index, size_t _Prop_param_index, typename _Ts, typename _Tb = _Ts>
	struct subsurface_roughness_texture_substance {
		subsurface_roughness_texture_substance() = default;
		subsurface_roughness_texture_substance(D3D12_GPU_VIRTUAL_ADDRESS _Albedo, VECTOR3 _Reflect, D3D12_GPU_VIRTUAL_ADDRESS _Roughness)
			: subsurface_albedo(_Albedo), reflect_value(_Reflect), roughness(_Roughness) {}

		virtual void release() override {}
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList & _Cmdlist) override {
			_Cmdlist.SetGraphicsRootShaderResourceView(_Talbedo_param_index, subsurface_albedo);
			_Cmdlist.SetGraphicsRoot32BitConstants(_Prop_param_index, 3, &(this->reflect_value), 0);
			_Cmdlist.SetGraphicsRootShaderResourceView(_Troughness_param_index, roughness);
			return _Cmdlist;
		}

		D3D12_GPU_VIRTUAL_ADDRESS subsurface_albedo;
		VECTOR3 reflect_value;
		D3D12_GPU_VIRTUAL_ADDRESS roughness;// anisotripic or isotripic
	};

	template<size_t _Param_index, typename _Ts, typename _Tb = _Ts, bool _Major = clmagic::_COL_MAJOR_>
	struct matrix_transform : public clmagic::transform, public MATRIX4x4 {
		using _Mymatrix = MATRIX4x4;

		template<typename ..._Tys>
		matrix_transform(_Tys... _Args) : _Mymatrix(_Args...) {}
		
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList& _Cmdlist) override {
			_Cmdlist.SetGraphicsRoot32BitConstants(_Param_index, 16, transpose(static_cast<MATRIX4x4&>(*this)).begin(), 0);
			return _Cmdlist;
		}
		virtual std::shared_ptr<transform> clone() override {
			std::shared_ptr<matrix_transform> _Transform_ptr = std::make_shared<matrix_transform>();
			static_cast<_Mymatrix&>(*_Transform_ptr) = *this;
			return _Transform_ptr;
		}
		virtual void translate(float x, float y, float z) override {
		
		}
		virtual void rotate(float axis_x, float axis_y, float axis_z, float _Radians) override {
		
		}
		virtual void scale(float scale_x, float scale_y, float scale_z) override {
		
		}
	};
	
}// namespace d3d12