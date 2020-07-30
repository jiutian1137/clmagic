#pragma once
#include "../basic/renderable.h"
#include "../math.h"
#include "d3d12core.h"

namespace d3d12 {
	
	template<typename _Iter>
	void render(_Iter _First, _Iter _Last,
		ID3D12GraphicsCommandList& _Cmdlist, 
		ID3D12PipelineState&       _Pipeline, 
		ID3D12RootSignature&       _Signature, 
		UINT NumDescriptorHeaps, ID3D12DescriptorHeap* const* ppDescriptorHeaps) {
		_Cmdlist.SetPipelineState(&_Pipeline);
		_Cmdlist.SetGraphicsRootSignature(&_Signature);
		_Cmdlist.SetDescriptorHeaps(NumDescriptorHeaps, ppDescriptorHeaps);
		for (; _First != _Last; ++_First) {
			(*_First)->render(_Cmdlist);
		}
	}

	struct shader_program : public shader_pipeline {
		shader_program() = default;
		shader_program(shader_program&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		shader_program& operator=(shader_program&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}

		explicit shader_program(shader_pipeline&& _Pipeline) : shader_pipeline(std::move(_Pipeline)) {}

		void swap(shader_program& _Right) {
			shader_pipeline::swap(_Right);
			std::swap(_Right._My_descriptor_heaps, this->_My_descriptor_heaps);
			std::swap(_Right._My_render_objects, this->_My_render_objects);
		}
		void release() {
			shader_pipeline::release();
			_My_descriptor_heaps.clear();
			_My_render_objects.clear();
		}

		void render(ID3D12GraphicsCommandList& _Cmdlist) {
			d3d12::render(_My_render_objects.begin(), _My_render_objects.end(), 
				_Cmdlist, 
				this->ref(), 
				*this->_My_puniform, 
				_My_descriptor_heaps.size(), _My_descriptor_heaps.data());
		}

		template<typename _Iter>
		void render(_Iter _First, _Iter _Last, ID3D12GraphicsCommandList& _Cmdlist, UINT NumDescriptorHeaps, ID3D12DescriptorHeap* const* ppDescriptorHeaps) {
			d3d12::render(_First, _First,
				_Cmdlist,
				this->ref(),
				*this->_My_puniform,
				NumDescriptorHeaps, ppDescriptorHeaps);
		}

		void push_descriptor_heap(ID3D12DescriptorHeap& _Dheap) {
			_My_descriptor_heaps.push_back(&_Dheap);
		}
		void push_render_object(clmagic::renderable& _Obj) {
			_My_render_objects.push_back(&_Obj);
		}

		std::vector<ID3D12DescriptorHeap*> _My_descriptor_heaps;
		std::vector<clmagic::renderable*> _My_render_objects;
	};

	template<typename _Ty>
	struct dynamic_mesh : public clmagic::geometry {
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
		virtual std::shared_ptr<geometry> reduce(size_t _Base_vertex_location, size_t _Start_index_location, size_t _Index_count) override {
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

		size_t base_vertex_location = 0;
		size_t start_index_location = 0;
		size_t index_count = 0;
		std::shared_ptr<dynamic_buffer<_Ty>>      _My_vertices;
		std::shared_ptr<dynamic_buffer<uint32_t>> _My_indices;
	};

	template<typename _Ty>
	struct static_mesh : public clmagic::geometry {
		static_mesh() = default;
		static_mesh(const static_mesh<_Ty>& _Right) {
			this->copy_from(_Right);
		}
		static_mesh(const dynamic_mesh<_Ty>& _Right) {
			this->copy_from(_Right);
		}
		static_mesh(std::shared_ptr<static_buffer<_Ty>> _Vbuf, std::shared_ptr<static_buffer<uint32_t>> _Ibuf) {
			_My_vertices = _Vbuf;
			_My_indices  = _Ibuf;
			this->base_vertex_location = 0;
			this->start_index_location = 0;
			this->index_count = _Ibuf->size();
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
		virtual std::shared_ptr<geometry> reduce(size_t _Base_vertex_location, size_t _Start_index_location, size_t _Index_count) override {
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

		size_t base_vertex_location = 0;
		size_t start_index_location = 0;
		size_t index_count = 0;
		std::shared_ptr<static_buffer<_Ty>>      _My_vertices;
		std::shared_ptr<static_buffer<uint32_t>> _My_indices;
	};
	
	template<uint32_t _Bx_prop, uint32_t _Tx_albedo, uint32_t _Tx_reflect, uint32_t _Tx_roughness, typename _Ts, typename _Tb = _Ts>
	struct surface_substance : public clmagic::substance {
		enum flags { PROP, TEX };

		surface_substance() = default;
		surface_substance(VECTOR3 _Albedo, VECTOR3 _Reflect, SCALAR _Roughness_x, SCALAR _Roughness_y)
			: _My_prop{ _Albedo, _Reflect, _Roughness_x, _Roughness_y } {}

		virtual void release() override {}
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList& _Cmdlist) override {
			_Cmdlist.SetGraphicsRoot32BitConstants(_Bx_prop, 8, &(_My_prop.subsurface_albedo), 0);
			if (_My_albedo_flag == TEX) {
				_Cmdlist.SetGraphicsRootDescriptorTable(_Tx_albedo, _My_albedo_texture);
			}
			if (_My_reflect_flag == TEX) {
				_Cmdlist.SetGraphicsRootDescriptorTable(_Tx_reflect, _My_reflect_texture);
			}
			if (_My_roughness_flag == TEX) {
				_Cmdlist.SetGraphicsRootDescriptorTable(_Tx_roughness, _My_roughness_texture);
			}
			return _Cmdlist;
		}

		void set_albedo(VECTOR3 _Albedo) {
			_My_prop.subsurface_albedo = _Albedo;
			_My_albedo_flag            = PROP;
		}
		void set_reflect(VECTOR3 _Reflect) {
			_My_prop.reflect_value = _Reflect;
			_My_reflect_flag       = PROP;
		}
		void set_roughness(SCALAR _Roughness_x, SCALAR _Roughness_y = 0) {
			_My_prop.roughness_x = _Roughness_x;
			_My_prop.roughness_y = _Roughness_y;
			_My_roughness_flag = PROP;
		}
		void set_albedo(D3D12_GPU_DESCRIPTOR_HANDLE _Albedo_tex) {
			_My_albedo_texture = _Albedo_tex;
			_My_albedo_flag    = TEX;
		}
		void set_reflect(D3D12_GPU_DESCRIPTOR_HANDLE _Reflect_tex) {
			_My_reflect_texture = _Reflect_tex;
			_My_reflect_flag    = TEX;
		}
		void set_roughness(D3D12_GPU_DESCRIPTOR_HANDLE _Roughness_tex) {
			_My_roughness_texture = _Roughness_tex;
			_My_roughness_flag    = TEX;
		}

	//private:
		SURFACE _My_prop;
		D3D12_GPU_DESCRIPTOR_HANDLE _My_albedo_texture;
		D3D12_GPU_DESCRIPTOR_HANDLE _My_reflect_texture;
		D3D12_GPU_DESCRIPTOR_HANDLE _My_roughness_texture;
		flags _My_albedo_flag = PROP;
		flags _My_reflect_flag = PROP;
		flags _My_roughness_flag = PROP;
	};

	template<size_t _Param_index, typename _Ts, typename _Tb = _Ts, clmagic::matrix_major _Major = clmagic::DEFAULT_MAJOR>
	struct matrix_transform : public clmagic::transform, public clmagic::matrix4x4<_Ts, _Tb, _Major> {
		using _Mybase = clmagic::matrix4x4<_Ts, _Tb, _Major>;

		template<typename ..._Tys>
		matrix_transform(_Tys... _Args) : _Mybase(_Args...) {}
		
		virtual std::shared_ptr<transform> clone() const override {
			std::shared_ptr<matrix_transform> _Mtransform = std::make_shared<matrix_transform>();
			static_cast<_Mybase&>(*_Mtransform) = (*this);
			return _Mtransform;
		}

		virtual void translate(float x, float y, float z) override {
			/*auto M = clmagic::translation<_Ts, _Tb, _Major>(x, y, z);
			static_cast<_Mybase&>(*this) = M(*this);*/
		}
		virtual void scale(float sx, float sy, float sz) override {
	/*		auto M = clmagic::scaling<_Ts, _Tb, _Major>(x, y, z);
			static_cast<_Mybase&>(*this) = M(*this);*/
		}
		virtual void rotate(clmagic::unit_vector3<float> axis, clmagic::angle_t<float> angle) override {
		
		}
		virtual void translate_parent(float x, float y, float z) {
		
		}
		virtual void scale_parent(float sx, float sy, float sz) {
		
		}
		virtual void rotate_parent(clmagic::unit_vector3<float> axis, clmagic::angle_t<float> angle) {
		
		}

		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList& _Cmdlist) override {
			auto M = static_cast<const _Mybase&>(*this);
			if ( M.col_major() ) {
				M = clmagic::transpose(M);
			} 
			_Cmdlist.SetGraphicsRoot32BitConstants(_Param_index, 16, M.begin(), 0);
			return _Cmdlist;
		}
	};
	
}// namespace d3d12