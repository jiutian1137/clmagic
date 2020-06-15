#pragma once
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include "packaged_comptr.h"

namespace d3d12 {
	inline Microsoft::WRL::ComPtr<IDXGIFactory7> make_IDXGIFactory() {
		Microsoft::WRL::ComPtr<IDXGIFactory7> _Factory;
		assert(SUCCEEDED( CreateDXGIFactory1(IID_PPV_ARGS(&_Factory)) ));
		return _Factory;
	}

	struct factory : public packaged_comptr<IDXGIFactory7>, public uncopyable {
		factory() = default;
		factory(factory&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		factory& operator=(factory&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}
	
		factory(UINT _Flag) {
			this->_Impl = make_IDXGIFactory();
		}
	};
}// namespace d3d12