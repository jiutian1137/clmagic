#pragma once
#ifndef clmagic_directX12_STATIC_MESH_h_
#define clmagic_directX12_STATIC_MESH_h_
#include "dxcore.h"
#include <memory>
#include <unordered_map>

namespace dx12 {
	template<typename _Tvt, typename _Tit = uint32_t>
	struct static_mesh : public dx12::_Uncopyable {
		static_mesh() = default;

		static_mesh(static_mesh&& _Right)
			: _Mydata(std::move(_Right._Mydata)), 
			_Myidata(std::move(_Right._Myidata)),
			_Base_vertex_location(0), _Start_index_location(0), _Index_count(0) {}

		static_mesh(ID3D12Device& _Device, size_t _Vertices, size_t _Indices)
			: _Mydata(new dx12::packaged_resource_default_heap(_Device, 
				CD3DX12_RESOURCE_DESC::Buffer(_Vertices * sizeof(_Tvt)), D3D12_RESOURCE_STATE_COMMON)),
			_Myidata(new dx12::packaged_resource_default_heap(_Device,
				CD3DX12_RESOURCE_DESC::Buffer(_Indices * sizeof(_Tit)), D3D12_RESOURCE_STATE_COMMON)),
			_Base_vertex_location(0), _Start_index_location(0), _Index_count(_Indices) {}

		static_mesh(ID3D12Device& _Device, ID3D12GraphicsCommandList& _Cmdlist, 
			const _Tvt* _Pvertex, size_t _Vertices, const _Tit* _Pindex, size_t _Indices)
			: static_mesh(_Device, _Vertices, _Indices) {
			assign(_Device, _Cmdlist, _Pvertex, _Pindex);
		}

		static_mesh& operator=(static_mesh&& _Right) {
			_Mydata  = std::move(_Right._Mydata);
			_Myidata = std::move(_Right._Myidata);
			_Base_vertex_location = _Right._Base_vertex_location;
			_Start_index_location = _Right._Start_index_location;
			_Index_count          = _Right._Index_count;
			return (*this);
		}

		void assign(ID3D12Device& _Device, ID3D12GraphicsCommandList& _Cmdlist, const _Tvt* _Pvertex, const _Tit* _Pindex) {
			ThrowIfFailed(_Mydata->valid());
			ThrowIfFailed(_Myidata->valid());
			_Mydata->upload(_Device, _Cmdlist, _Pvertex, _Mydata->capacity());
			_Myidata->upload(_Device, _Cmdlist, _Pindex, _Myidata->capacity());
		}
	
		size_t vertices() const {
			return (_Mydata->capacity() / sizeof(_Tvt));
		}

		size_t indices() const {
			//return (_Myidata->capacity() / sizeof(_Tit));
			return _Index_count;
		}

		D3D12_VERTEX_BUFFER_VIEW vbv() const {
			D3D12_VERTEX_BUFFER_VIEW _Desc;
			_Desc.BufferLocation = _Mydata->get()->GetGPUVirtualAddress();
			_Desc.SizeInBytes    = _Mydata->capacity();
			_Desc.StrideInBytes  = sizeof(_Tvt);
			return _Desc;
		}

		D3D12_INDEX_BUFFER_VIEW ibv() const {
			D3D12_INDEX_BUFFER_VIEW _Desc;
			_Desc.BufferLocation = _Myidata->get()->GetGPUVirtualAddress();
			_Desc.SizeInBytes    = _Myidata->capacity();
			_Desc.Format         = DXGI_FORMAT_R32_UINT;
			return _Desc;
		}
	
		std::unordered_map<std::string, static_mesh> submesh;

		static_mesh(static_mesh& _Mesh, size_t _Xv, size_t _Xi, size_t _Ni)
			: _Mydata(_Mesh._Mydata), _Myidata(_Mesh._Myidata), 
			_Base_vertex_location(_Xv), _Start_index_location(_Xi), _Index_count(_Ni) {}
	private:

		std::shared_ptr<dx12::packaged_resource_default_heap> _Mydata;
		std::shared_ptr<dx12::packaged_resource_default_heap> _Myidata;
		
	public:
		size_t _Base_vertex_location;
		size_t _Start_index_location;
		size_t _Index_count;
	};

}// namespace dx12

#endif