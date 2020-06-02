#pragma once
#include <dxgi.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <vector>
#include "ID3D12Resource_t.h"

namespace d3d12 {
	template<DXGI_FORMAT _My_fmt>
	struct _Swap_chain_impl : public uncopyable {
		constexpr static DXGI_FORMAT format = _My_fmt;
		using resource_type = static_texture2D<_My_fmt>;

		_Swap_chain_impl() = default;
		_Swap_chain_impl(_Swap_chain_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		_Swap_chain_impl& operator=(_Swap_chain_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return *this;
		}

		_Swap_chain_impl(HWND _Win, IDXGIFactory4& _Factory, ID3D12CommandQueue& _Queue, int _Width, int _Height, DXGI_SAMPLE_DESC _Sdesc, size_t _Count = 2) {
			DXGI_SWAP_CHAIN_DESC sd;
			sd.BufferDesc.Width  = _Width;
			sd.BufferDesc.Height = _Height;
			sd.BufferDesc.RefreshRate.Numerator   = 60;
			sd.BufferDesc.RefreshRate.Denominator = 1;
			sd.BufferDesc.Format = _My_fmt;
			sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			sd.SampleDesc   = _Sdesc;
			sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.BufferCount  = 2;// vertical-sync... is 3
			sd.OutputWindow = _Win;
			sd.Windowed   = true;
			sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			sd.Flags      = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			assert(SUCCEEDED(
				_Factory.CreateSwapChain(&_Queue, &sd, &this->_Impl)
			));
		
			_Mydata.resize(2);
			for (size_t i = 0; i != _Count; ++i) {
				this->_Impl->GetBuffer(i, IID_PPV_ARGS(&_Mydata[i]._Impl));
				_Mydata[i].transition(D3D12_RESOURCE_STATE_PRESENT);
			}
		}

		void release() {
			_Mydata.clear();
			_Impl = nullptr;
		}
		void swap(_Swap_chain_impl& _Right) {
			std::swap(_Right._Impl, this->_Impl);
			std::swap(_Right.timer_pointer, this->timer_pointer);
			std::swap(_Right._Mydata, this->_Mydata);
		}

		size_t cycle() const {
			return _Mydata.size();
		}

		void turn() {
			this->timer_pointer =  (this->timer_pointer+1) % this->cycle();
		}

		resource_type& clock() {
			return _Mydata[this->timer_pointer];
		}
		resource_type& clock(size_t i) {
			return _Mydata[i];
		}
		resource_type& operator*() {
			return _Mydata[this->timer_pointer];
		}
		resource_type* operator->() {
			return &_Mydata[this->timer_pointer];
		}
		resource_type* clock_ptr() {
			return &_Mydata[this->timer_pointer];
		}

		const resource_type& clock() const {
			return _Mydata[this->timer_pointer];
		}
		const resource_type& clock(size_t i) const {
			return _Mydata[i];
		}
		IDXGISwapChain& chain() const {
			return *(_Impl.Get());
		}

		Microsoft::WRL::ComPtr<IDXGISwapChain> _Impl;
		size_t timer_pointer = 0;
	private:
		std::vector< resource_type > _Mydata;
	};

}// namespace d3d12