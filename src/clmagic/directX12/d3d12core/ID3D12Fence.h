#pragma once
#include "ID3D12Device.h"
#include "ID3D12CommandObjects.h"

namespace d3d12 {

	inline void wait_ID3D12CommandQueue(ID3D12CommandQueue& _Taskqueue, ID3D12Fence& _Fence, UINT64 _Fence_point) {
		// _Fence.GetCompletedValue == _Fence_point if Task completed
		if (_Fence.GetCompletedValue() < _Fence_point) {
			HANDLE hEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
			_Fence.SetEventOnCompletion(_Fence_point, hEvent);
			WaitForSingleObject(hEvent, INFINITE);
			CloseHandle(hEvent);
		}
	}

	inline void flush_ID3D12CommandQueue(ID3D12CommandQueue& _Taskqueue, ID3D12Fence& _Fence, UINT64& _Fence_point) {
		++_Fence_point;

		// Add an instruction to the command queue to set a new fence point.  Because we 
		// are on the GPU timeline, the new fence point won't be set until the GPU finishes
		// processing all the commands prior to this Signal().
		assert(SUCCEEDED( _Taskqueue.Signal(&_Fence, _Fence_point) ));
		wait_ID3D12CommandQueue(_Taskqueue, _Fence, _Fence_point);
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
		
		fence(ID3D12Device& _Device, UINT64 _Initial_value = 0, D3D12_FENCE_FLAGS _Flag = D3D12_FENCE_FLAG_NONE) {
			assert(SUCCEEDED( _Device.CreateFence(_Initial_value, _Flag, IID_PPV_ARGS(&this->_Impl)) ));
			this->_Fence_point = _Initial_value;
		}
	
		void release() {// override
			this->_Impl        = nullptr;
			this->_Fence_point = -1;
		}

		void swap(fence& _Right) {// override
			std::swap(_Right._Impl, this->_Impl);
			std::swap(_Right._Fence_point, this->_Fence_point);
		}

		void flush(ID3D12CommandQueue& _Queue) const {
			flush_ID3D12CommandQueue(_Queue, this->ref(), this->_Fence_point);
		}

		mutable UINT64 _Fence_point;
	};

}// namespace DX12