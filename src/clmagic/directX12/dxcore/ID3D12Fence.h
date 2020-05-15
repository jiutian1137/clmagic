#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include <wrl.h>
#include <assert.h>

#include "uncopyable.h"

namespace d3d12 {

	struct _Fence_impl : uncopyable{
		_Fence_impl() = default;
		_Fence_impl(_Fence_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		_Fence_impl(ID3D12Fence* _Pfence)
			: _Myimpl(_Pfence) {}
		_Fence_impl(ID3D12Device& _Device, UINT64 _Initial_value, D3D12_FENCE_FLAGS _Flag = D3D12_FENCE_FLAG_NONE) {
			HRESULT hr = _Device.CreateFence(_Initial_value, _Flag, IID_PPV_ARGS(_Myimpl.GetAddressOf()));
			assert(SUCCEEDED(hr));
		}
		_Fence_impl& operator=(_Fence_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}

		void release() {
			_Myimpl = nullptr;
		}
		void swap(_Fence_impl& _Right) {
			_Myimpl.Swap(_Right._Myimpl);
		}

		ID3D12Fence* get() const {
			return _Myimpl.Get();
		}
		ID3D12Fence* operator->() const {
			return _Myimpl.Get();
		}
		ID3D12Fence& ref() const {
			return *(_Myimpl.Get());
		}
		ID3D12Fence& operator*() const {
			return *(_Myimpl.Get());
		}

		void flush(ID3D12CommandQueue& _Queue, UINT64& _Fence_val) const {
			_Fence_val++;

			// Add an instruction to the command queue to set a new fence point.  Because we 
			// are on the GPU timeline, the new fence point won't be set until the GPU finishes
			// processing all the commands prior to this Signal().
			HRESULT hr = _Queue.Signal(this->get(), _Fence_val);
			assert(SUCCEEDED(hr));

			// Wait until the GPU has completed commands up to this fence point.
			if (this->get()->GetCompletedValue() < _Fence_val) {
				HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

				// Fire event when GPU hits current fence.  
				this->get()->SetEventOnCompletion(_Fence_val, eventHandle);
				WaitForSingleObject(eventHandle, INFINITE);

				CloseHandle(eventHandle);
			}
		}

		void flush(ID3D12CommandQueue& _Queue, ID3D12CommandList& _List, UINT64& _Fence_val) const {
			ID3D12CommandList* _Tasks[] = { &_List };
			_Queue.ExecuteCommandLists(1, _Tasks);
			this->flush(_Queue, _Fence_val);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Fence> _Myimpl;
	};

}// namespace DX12