#pragma once
#ifndef clmagic_directX12_STATIC_MESH_h_
#define clmagic_directX12_STATIC_MESH_h_
#include "dxcore.h"
#include <memory>
#include <unordered_map>

namespace dx12 {
	// subclass-memory will not change
	template<typename _Tvt, typename _Tit = uint32_t>
	struct basic_mesh {
		using vertex_resource_type = packaged_resource;
		using index_resource_type  = packaged_resource;
		using vertex_shared_ptr    = std::shared_ptr<packaged_resource>;
		using index_shared_ptr     = std::shared_ptr<packaged_resource>;

		basic_mesh() = default;

		basic_mesh(basic_mesh&& _Right) noexcept
			: base_vertex_location(_Right.base_vertex_location),
			  start_index_location(_Right.start_index_location),
			  index_count(_Right.index_count),
			_Shared_vertex_resource(std::move(_Right._Shared_vertex_resource)),
			_Shared_index_resource(std::move(_Right._Shared_index_resource)) {
			_Right.release();
		}

		basic_mesh& operator=(basic_mesh&& _Right) {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}

		~basic_mesh() {
			release();
		}

		void release() {
			base_vertex_location = 0;
			start_index_location = 0;
			index_count = 0;
			if (_Shared_vertex_resource != nullptr) {
				_Shared_vertex_resource = vertex_shared_ptr(nullptr);
			}
			if (_Shared_index_resource != nullptr) {
				_Shared_index_resource = index_shared_ptr(nullptr);
			}
		}

		size_t vertices() const {
			return (_Shared_vertex_resource->capacity() / sizeof(_Tvt));
		}

		size_t indices() const {
			//return (_Shared_index_data->capacity() / sizeof(_Tit));
			return index_count;
		}

		D3D12_VERTEX_BUFFER_VIEW vertex_view() const {
			D3D12_VERTEX_BUFFER_VIEW _Desc;
			_Desc.BufferLocation = _Shared_vertex_resource->get()->GetGPUVirtualAddress();
			_Desc.SizeInBytes    = _Shared_vertex_resource->capacity();
			_Desc.StrideInBytes  = sizeof(_Tvt);
			return _Desc;
		}

		D3D12_INDEX_BUFFER_VIEW index_view() const {
			D3D12_INDEX_BUFFER_VIEW _Desc;
			_Desc.BufferLocation = _Shared_index_resource->get()->GetGPUVirtualAddress();
			_Desc.SizeInBytes    = _Shared_index_resource->capacity();
			_Desc.Format         = sizeof(_Tit) == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
			return _Desc;
		}

		bool _Is_valid_vertex_resource() const {
			return (_Shared_vertex_resource != nullptr && _Shared_vertex_resource->valid());
		}
		
		bool _Is_valid_index_resource() const {
			return (_Shared_index_resource != nullptr && _Shared_index_resource->valid());
		}
		
		std::unordered_map<std::string, basic_mesh> submesh;

		basic_mesh(basic_mesh& _Mesh, size_t _Xv, size_t _Xi, size_t _Ni)
			: _Shared_vertex_resource(_Mesh._Shared_vertex_resource),
			  _Shared_index_resource(_Mesh._Shared_index_resource),
			base_vertex_location(_Mesh.base_vertex_location + _Xv),
			start_index_location(_Mesh.start_index_location + _Xi),
			index_count(_Ni) {}

	protected:
		// use std::swap(...)
		void swap(basic_mesh& _Right) {
			std::swap(this->base_vertex_location, _Right.base_vertex_location);
			std::swap(this->start_index_location, _Right.start_index_location);
			std::swap(this->index_count, _Right.index_count);
			this->_Shared_vertex_resource.swap(_Right._Shared_vertex_resource);
			this->_Shared_index_resource.swap(_Right._Shared_index_resource);
		}
		
		std::shared_ptr<packaged_resource> _Shared_vertex_resource;
		std::shared_ptr<packaged_resource> _Shared_index_resource;
	public:
		size_t base_vertex_location = 0;
		size_t start_index_location = 0;
		size_t index_count = 0;
	};


	template<typename _Tvt, typename _Tit = uint32_t>
	struct static_mesh : public basic_mesh<_Tvt, _Tit> {
		using _Mybase = basic_mesh<_Tvt, _Tit>;
		using vertex_resource_type = packaged_resource_default_heap;
		using index_resource_type  = packaged_resource_default_heap;
		using vertex_shared_ptr    = std::shared_ptr<packaged_resource_default_heap>;
		using index_shared_ptr     = std::shared_ptr<packaged_resource_default_heap>;

		static_mesh() = default;

		static_mesh(_Mybase&& _Right) noexcept
			: _Mybase(std::move(_Right)) {}

		static_mesh& operator=(_Mybase&& _Right) {
			return _Mybase::operator=(std::move(_Right));
		}

		static_mesh(ID3D12Device& _Device, size_t _Vertices, size_t _Indices) {
			_Mybase::_Shared_vertex_resource = std::make_shared<vertex_resource_type>(_Device, 
				CD3DX12_RESOURCE_DESC::Buffer(_Vertices * sizeof(_Tvt)), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			
			_Mybase::_Shared_index_resource  = std::make_shared<index_resource_type>(_Device, 
				CD3DX12_RESOURCE_DESC::Buffer(_Indices * sizeof(_Tit)), D3D12_RESOURCE_STATE_INDEX_BUFFER);
			
			_Mybase::index_count         = _Indices;
		}

		static_mesh(ID3D12Device& _Device, ID3D12GraphicsCommandList& _Cmdlist, const _Tvt* _Pvertex, size_t _Vertices, const _Tit* _Pindex, size_t _Indices)
			: static_mesh(_Device, _Vertices, _Indices) {
			assign(_Device, _Cmdlist, _Pvertex, _Pindex);
		}

		void assign(ID3D12Device& _Device, ID3D12GraphicsCommandList& _Cmdlist, const _Tvt* _Pvertex, const _Tit* _Pindex) {
			auto& _Vresource = vertex_resource();
			auto& _Iresource = index_resource();
			_Vresource.upload(_Device, _Cmdlist, _Pvertex, _Vresource.capacity());
			_Iresource.upload(_Device, _Cmdlist, _Pindex, _Iresource.capacity());
		}

		vertex_resource_type& vertex_resource() {
			ThrowIfFailed(_Mybase::_Is_valid_vertex_resource());
			return *dynamic_cast<vertex_resource_type*>(_Mybase::_Shared_vertex_resource.get());
		}

		index_resource_type& index_resource() {
			ThrowIfFailed(_Mybase::_Is_valid_index_resource());
			return *dynamic_cast<index_resource_type*>(_Mybase::_Shared_index_resource.get());
		}
	};


	template<typename _Tvt, typename _Tit = uint32_t>
	struct dynamic_vertex_mesh : public basic_mesh<_Tvt, _Tit> {
		using _Mybase = basic_mesh<_Tvt, _Tit>;
		using vertex_resource_type = packaged_resource_upload_heap;
		using index_resource_type  = packaged_resource_default_heap;
		using vertex_shared_ptr    = std::shared_ptr<packaged_resource_upload_heap>;
		using index_shared_ptr     = std::shared_ptr<packaged_resource_default_heap>;

		dynamic_vertex_mesh() = default;

		dynamic_vertex_mesh(_Mybase && _Right) noexcept
			: _Mybase(std::move(_Right)) {}

		dynamic_vertex_mesh& operator=(_Mybase && _Right) {
			return _Mybase::operator=(std::move(_Right));
		}

		// 
		dynamic_vertex_mesh(ID3D12Device& _Device, size_t _Vertices, size_t _Indices) {
			_Mybase::_Shared_vertex_resource = std::make_shared<vertex_resource_type>( _Device, 
				CD3DX12_RESOURCE_DESC::Buffer(_Vertices * sizeof(_Tvt))/*, *_GENERAL_READ*/ );
			_Mybase::_Shared_index_resource  = std::make_shared<index_resource_type>( _Device, 
				CD3DX12_RESOURCE_DESC::Buffer(_Indices * sizeof(_Tit)), D3D12_RESOURCE_STATE_INDEX_BUFFER );
			_Mybase::index_count = _Indices;
		}

		void assign(ID3D12Device& _Device, ID3D12GraphicsCommandList& _Cmdlist, const _Tvt* _Pvertex, const _Tit* _Pindex) {
			auto& _Vresource = vertex_resource();
			auto& _Iresource = index_resource();
			_Vresource.copy_data(_Pvertex, _Vresource.capacity());
			_Iresource.upload(_Device, _Cmdlist, _Pindex, _Iresource.capacity());
		}

		// 
		dynamic_vertex_mesh(ID3D12Device& _Device, size_t _Indices) {
			_Mybase::_Shared_index_resource = std::make_shared<index_resource_type>(_Device, 
				CD3DX12_RESOURCE_DESC::Buffer(_Indices * sizeof(_Tit)), D3D12_RESOURCE_STATE_INDEX_BUFFER);
			_Mybase::index_count = _Indices;
		}

		void assign(ID3D12Device& _Device, ID3D12GraphicsCommandList& _Cmdlist, const _Tit* _Pindex) {
			auto& _Iresource = index_resource();
			_Iresource.upload(_Device, _Cmdlist, _Pindex, _Iresource.capacity());
		}

		void bind_vertex(vertex_shared_ptr _Vertex) {
			_Mybase::_Shared_vertex_resource = _Vertex;
		}

		vertex_resource_type& vertex_resource() {
			ThrowIfFailed(_Mybase::_Is_valid_vertex_resource());
			return *dynamic_cast<vertex_resource_type*>(_Mybase::_Shared_vertex_resource.get());
		}

		index_resource_type& index_resource() {
			ThrowIfFailed(_Mybase::_Is_valid_index_resource());
			return *dynamic_cast<index_resource_type*>(_Mybase::_Shared_index_resource.get());
		}
	};


}// namespace dx12

#endif