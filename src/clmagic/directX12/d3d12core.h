#pragma once
#include "d3d12core/d3dx12.h"
#include "d3d12core/packaged_comptr.h"
#include "d3d12core/IDXGIFactory.h"
#include "d3d12core/ID3D12Device.h"
#include "d3d12core/ID3D12Fence.h"
#include "d3d12core/ID3D12CommandObjects.h"
#include "d3d12core/ID3D12Heap.h"
#include "d3d12core/ID3D12Resource.h"
#include "d3d12core/ID3D12Resource_b.h"
#include "d3d12core/ID3D12Resource_t.h"
#include "d3d12core/IDXGISwapChain.h"
#include "d3d12core/ID3D12DescriptorHeap.h"
#include "d3d12core/ID3D12PipelineState.h"
#include "d3d12core/ID3D12RootSignature.h"

namespace d3d12 {
	
	using command_allocator = _Command_allocator_impl;
	using graphics_command_list = _Graphics_command_list_impl;
	using command_queue    = _Command_queue_impl;

	using shader_input     = _Vector_input_element_desc;
	using shader_signature = _Rootsignature_impl;
	
	// ID3D12DescriptorHeap
	template<size_t _Size> using CBV_SRV_UAV_array = descriptor_array<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, _Size>;
	template<size_t _Size> using SAMPLER_array     = descriptor_array<D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, _Size>;
	template<size_t _Size> using RTV_array         = descriptor_array<D3D12_DESCRIPTOR_HEAP_TYPE_RTV, _Size>;
	template<size_t _Size> using DSV_array         = descriptor_array<D3D12_DESCRIPTOR_HEAP_TYPE_DSV, _Size>;
	template<typename _Kty, size_t _Size> using CBV_SRV_UAV_array_map = descriptor_array_map<_Kty, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, _Size>;
	template<typename _Kty, size_t _Size> using SAMPLER_array_map     = descriptor_array_map<_Kty, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, _Size>;
	template<typename _Kty, size_t _Size> using RTV_array_map         = descriptor_array_map<_Kty, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, _Size>;
	template<typename _Kty, size_t _Size> using DSV_array_map         = descriptor_array_map<_Kty, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, _Size>;

	


	// ID3D12Resource
	using static_texture2D_rgba8u  = static_texture2D<DXGI_FORMAT_R8G8B8A8_UNORM>;
	using static_texture2D_d24s8   = static_texture2D<DXGI_FORMAT_D24_UNORM_S8_UINT>;
	
	using static_texture2D_rgb32f  = static_texture2D<DXGI_FORMAT_R32G32B32_FLOAT>;
	using dynamic_texture2D_rgb32f = dynamic_texture2D<DXGI_FORMAT_R32G32B32_FLOAT>;

	using static_texture2D_rgba32f  = static_texture2D<DXGI_FORMAT_R32G32B32A32_FLOAT>;
	using dynamic_texture2D_rgba32f = dynamic_texture2D<DXGI_FORMAT_R32G32B32A32_FLOAT>;

}// namespace d3d12
