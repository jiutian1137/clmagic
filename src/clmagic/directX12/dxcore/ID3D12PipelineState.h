#pragma once
#include <d3d12.h>
#include <wrl.h>// Windows 
#include <assert.h>
#include <array>

#include "d3dx12.h"
#include "uncopyable.h"
#include "enum_string.h"

namespace d3d12 {

	struct _Pipelinestate_impl : public uncopyable {
		_Pipelinestate_impl() = default;
		_Pipelinestate_impl(_Pipelinestate_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		_Pipelinestate_impl& operator=(_Pipelinestate_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}
		
		_Pipelinestate_impl(ID3D12Device& _Device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& _Desc) {
			HRESULT hr = _Device.CreateGraphicsPipelineState(&_Desc, IID_PPV_ARGS(&_Myimpl));
			assert(SUCCEEDED(hr));
		}

		void release() {
			_Myimpl = nullptr;
		}
		void swap(_Pipelinestate_impl& _Right) {
			this->_Myimpl.Swap(_Right._Myimpl);
		}

		ID3D12PipelineState* get() const {
			return _Myimpl.Get();
		}
		ID3D12PipelineState* operator->() const {
			return _Myimpl.Get();
		}
		ID3D12PipelineState& ref() const {
			return *(_Myimpl.Get());
		}
		ID3D12PipelineState& operator*() const {
			return *(_Myimpl.Get());
		}

		bool valid() const {
			return static_cast<bool>(_Myimpl);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> _Myimpl;
	};

}// namespace dx12