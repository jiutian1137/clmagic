#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include <assert.h>

#include "packaged_comptr.h"

namespace d3d12 {

	inline void flush_ID3D12CommandQueue(ID3D12CommandQueue& _Tasksqueue, ID3D12Fence& _Fence, UINT64 _Fence_point) {
		++_Fence_point;

		// Add an instruction to the command queue to set a new fence point.  Because we 
		// are on the GPU timeline, the new fence point won't be set until the GPU finishes
		// processing all the commands prior to this Signal().
		assert(SUCCEEDED( _Tasksqueue.Signal(&_Fence, _Fence_point) ));

		// Wait until the GPU has completed commands up to this fence point.
		if (_Fence.GetCompletedValue() < _Fence_point) {
			HANDLE hEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
			_Fence.SetEventOnCompletion(_Fence_point, hEvent);
			WaitForSingleObject(hEvent, INFINITE);
			CloseHandle(hEvent);
		}
	}

	struct fence : public packaged_comptr<ID3D12Fence>, public uncopyable{
		fence() = default;
		fence(fence&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		fence& operator=(fence&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}
		
		fence(ID3D12Device& _Device, UINT64 _Initial_value, D3D12_FENCE_FLAGS _Flag = D3D12_FENCE_FLAG_NONE) {
			HRESULT hr = _Device.CreateFence(_Initial_value, _Flag, IID_PPV_ARGS(&this->_Impl));
			assert(SUCCEEDED(hr));
			this->_Fence_point = _Initial_value;
		}
	
		void flush(ID3D12CommandQueue& _Queue) const {
			flush_ID3D12CommandQueue(_Queue, this->ref(), this->_Fence_point);
		}

		void execute_flush(ID3D12CommandQueue& _Queue, ID3D12CommandList& _List) const {
			ID3D12CommandList* _Tasks[] = { &_List };
			_Queue.ExecuteCommandLists(1, _Tasks);
			this->flush(_Queue);
		}

 		void close_execute_flush(ID3D12CommandQueue& _Queue, ID3D12GraphicsCommandList& _List) const {
			_List.Close();
			this->execute_flush(_Queue, _List);
		}

		mutable UINT64 _Fence_point;
	};

}// namespace DX12