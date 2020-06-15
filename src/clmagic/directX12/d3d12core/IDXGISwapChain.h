#pragma once
#include "IDXGISwapChain.h"
#include "ID3D12Resource_t.h"

namespace d3d12 {

	template<DXGI_FORMAT _Fmt/*no auto*/>
	Microsoft::WRL::ComPtr<IDXGISwapChain> make_IDXGISwapChain(HWND _Win, IUnknown& _Queue, UINT _Width, UINT _Height, size_t _Count, DXGI_SAMPLE_DESC _Sdesc) {
		DXGI_SWAP_CHAIN_DESC _Desc;
		_Desc.BufferDesc.Format = _Fmt;
		_Desc.BufferDesc.Width  = _Width;
		_Desc.BufferDesc.Height = _Height;
		_Desc.BufferDesc.RefreshRate.Numerator   = 60;
		_Desc.BufferDesc.RefreshRate.Denominator = 1;
		_Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		_Desc.BufferDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
		_Desc.SampleDesc   = _Sdesc;
		_Desc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		_Desc.BufferCount  = _Count;// vertical-sync... is 3
		_Desc.OutputWindow = _Win;
		_Desc.Windowed     = true;
		_Desc.SwapEffect   = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		_Desc.Flags        = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		Microsoft::WRL::ComPtr<IDXGISwapChain> _Swapchain;
		auto _Factory = make_IDXGIFactory();
		assert(SUCCEEDED( _Factory->CreateSwapChain(&_Queue, &_Desc, _Swapchain.GetAddressOf()) ));
		return _Swapchain;
	}

	template<DXGI_FORMAT _My_fmt>
	struct swap_chain : public packaged_comptr<IDXGISwapChain>, public uncopyable {
		constexpr static DXGI_FORMAT format = _My_fmt;
		using resource_type = static_texture2D<_My_fmt>;

		swap_chain() = default;
		swap_chain(swap_chain&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		swap_chain& operator=(swap_chain&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return *this;
		}

		swap_chain(HWND _Win, ID3D12CommandQueue& _Queue, size_t _Width, size_t _Height, size_t _Count, DXGI_SAMPLE_DESC _Sdesc) {
			this->_Impl = make_IDXGISwapChain<_My_fmt>(_Win, _Queue, _Width, _Height, _Count, _Sdesc);
		
			_Mydata.resize(_Count);
			for (size_t i = 0; i != _Count; ++i) {
				this->_Impl->GetBuffer(i, IID_PPV_ARGS(&_Mydata[i]._Impl));
				_Mydata[i].transition(D3D12_RESOURCE_STATE_PRESENT);
			}
		}

		void release() {
			_Mydata.clear();
			_Impl = nullptr;
		}
		void swap(swap_chain& _Right) {
			std::swap(_Right._Impl, this->_Impl);
			std::swap(_Right.timer_pointer, this->timer_pointer);
			std::swap(_Right._Mydata, this->_Mydata);
		}

		size_t cycle() const {
			return _Mydata.size();
		}

		void turn() {
			this->timer_pointer = (this->timer_pointer+1) % this->cycle();
		}

		resource_type& current() {
			return _Mydata[this->timer_pointer];
		}
		resource_type* current_ptr() {
			return &_Mydata[this->timer_pointer];
		}
		resource_type& operator[](size_t i) {
			return _Mydata[i];
		}

		const resource_type& current() const {
			return _Mydata[this->timer_pointer];
		}
		const resource_type* current_ptr() const {
			return &_Mydata[this->timer_pointer];
		}
		const resource_type& operator[](size_t i) const {
			return _Mydata[i];
		}
	
		IDXGISwapChain& chain() const {
			return *(this->_Impl.Get());
		}

		size_t timer_pointer = 0;
	private:
		std::vector< resource_type > _Mydata;
	};

	using swap_chain_rgba8u = swap_chain<DXGI_FORMAT_R8G8B8A8_UNORM>;
	using swap_chain_d24s8  = swap_chain<DXGI_FORMAT_D24_UNORM_S8_UINT>;

}// namespace d3d12