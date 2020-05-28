#pragma once
#include "dxcore/d3dx12.h"
#include "dxcore/ID3D12Fence.h"
#include "dxcore/ID3D12Heap.h"
#include "dxcore/ID3D12Resource.h"
#include "dxcore/ID3D12DescriptorHeap.h"
#include "dxcore/D3D12_INPUT_LAYOUT_DESC.h"
#include "dxcore/ID3D12PipelineState.h"
#include "dxcore/ID3D12RootSignature.h"

namespace d3d12 {
	using fence            = _Fence_impl;
	using descriptor_heap  = _Descriptorheap_impl;
	using shader_input     = _Vector_input_element_desc;
	using shader_signature = _Rootsignature_impl;
	using shader_program   = _Pipelinestate_impl;
	using resource         = _Resource_impl;

	template<typename _Ty> using static_buffer = _Buffer_resource<_Ty, D3D12_HEAP_TYPE_DEFAULT>;
	template<typename _Ty> using dynamic_buffer = _Buffer_resource<_Ty, D3D12_HEAP_TYPE_UPLOAD>;
	template<DXGI_FORMAT _Fmt> using static_texture1D = _Texture_resource<_Fmt, D3D12_RESOURCE_DIMENSION_TEXTURE1D, D3D12_HEAP_TYPE_DEFAULT>;
	template<DXGI_FORMAT _Fmt> using static_texture2D = _Texture_resource<_Fmt, D3D12_RESOURCE_DIMENSION_TEXTURE2D, D3D12_HEAP_TYPE_DEFAULT>;
	template<DXGI_FORMAT _Fmt> using static_texture3D = _Texture_resource<_Fmt, D3D12_RESOURCE_DIMENSION_TEXTURE3D, D3D12_HEAP_TYPE_DEFAULT>;
	template<DXGI_FORMAT _Fmt> using dynamic_texture1D = _Texture_resource<_Fmt, D3D12_RESOURCE_DIMENSION_TEXTURE1D, D3D12_HEAP_TYPE_UPLOAD>;
	template<DXGI_FORMAT _Fmt> using dynamic_texture2D = _Texture_resource<_Fmt, D3D12_RESOURCE_DIMENSION_TEXTURE2D, D3D12_HEAP_TYPE_UPLOAD>;
	template<DXGI_FORMAT _Fmt> using dynamic_texture3D = _Texture_resource<_Fmt, D3D12_RESOURCE_DIMENSION_TEXTURE3D, D3D12_HEAP_TYPE_UPLOAD>;

	using static_texture1D_rgb32f  = static_texture1D<DXGI_FORMAT_R32G32B32_FLOAT>;
	using static_texture2D_rgb32f  = static_texture2D<DXGI_FORMAT_R32G32B32_FLOAT>;
	using static_texture3D_rgb32f  = static_texture3D<DXGI_FORMAT_R32G32B32_FLOAT>;
	using dynamic_texture1D_rgb32f = dynamic_texture1D<DXGI_FORMAT_R32G32B32_FLOAT>;
	using dynamic_texture2D_rgb32f = dynamic_texture2D<DXGI_FORMAT_R32G32B32_FLOAT>;
	using dynamic_texture3D_rgb32f = dynamic_texture3D<DXGI_FORMAT_R32G32B32_FLOAT>;

	using static_texture1D_rgba32f  = static_texture1D<DXGI_FORMAT_R32G32B32A32_FLOAT>;
	using static_texture2D_rgba32f  = static_texture2D<DXGI_FORMAT_R32G32B32A32_FLOAT>;
	using static_texture3D_rgba32f  = static_texture3D<DXGI_FORMAT_R32G32B32A32_FLOAT>;
	using dynamic_texture1D_rgba32f = dynamic_texture1D<DXGI_FORMAT_R32G32B32A32_FLOAT>;
	using dynamic_texture2D_rgba32f = dynamic_texture2D<DXGI_FORMAT_R32G32B32A32_FLOAT>;
	using dynamic_texture3D_rgba32f = dynamic_texture3D<DXGI_FORMAT_R32G32B32A32_FLOAT>;
	
	template<typename _Ty>
	dynamic_buffer<_Ty> make_dynamic_buffer(ID3D12Device& _Device, const std::vector<_Ty>& _Source) {
		dynamic_buffer<_Ty> _Result = dynamic_buffer<_Ty>(_Device, _Source.size());
		_Ty*                _Ptr    = nullptr;
		_Result->Map(0, nullptr, reinterpret_cast<void**>(&_Ptr));
		std::copy(_Source.begin(), _Source.end(), _Ptr);
		_Result->Unmap(0, nullptr);
		return _Result;
	}

}// namespace d3d12
