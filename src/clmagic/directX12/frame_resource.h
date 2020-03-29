#pragma once
#ifndef clmagic_directX12_FRAME_RESOURCE_h_
#define clmagic_directX12_FRAME_RESOURCE_h_
#include "dxcore.h"

namespace dx12 {
	/*
	@_basic_frame_resource:
	*/
	struct basic_frame_resource {
		basic_frame_resource() = default;

		basic_frame_resource(ID3D12Device& _Device, D3D12_COMMAND_LIST_TYPE _Type) {
			ThrowIfFailed(_Device.CreateCommandAllocator(_Type, IID_PPV_ARGS(&_Myalloc)));
		}

		ID3D12CommandAllocator* command_allocator_ptr() const {
			return (_Myalloc.Get());
		}

		uint64_t fence = 0;

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _Myalloc;
	};
}// namespace dx12

#endif