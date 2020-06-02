#pragma once
#ifndef clmagic_directX12_FRAME_RESOURCE_h_
#define clmagic_directX12_FRAME_RESOURCE_h_
#include <d3d12.h>
#include <wrl.h>
#include <assert.h>

namespace d3d12 {

	struct basic_frame_resource {
		basic_frame_resource() = default;
		basic_frame_resource(ID3D12Device& _Device, D3D12_COMMAND_LIST_TYPE _Type = D3D12_COMMAND_LIST_TYPE_DIRECT) {
			HRESULT hr = _Device.CreateCommandAllocator(_Type, IID_PPV_ARGS(&_Myalloc));
			assert(SUCCEEDED(hr));
		}
		~basic_frame_resource() {
			this->release();
		}

		virtual void release() {
			this->_Myalloc = nullptr;
		}

		ID3D12CommandAllocator* command_allocator_ptr() const {
			return (_Myalloc.Get());
		}

		uint64_t fence = 0;

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _Myalloc;
	};

}// namespace DX12

#endif