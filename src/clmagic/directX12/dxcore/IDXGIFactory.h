#pragma once
#include <dxgi1_4.h>
#include <assert.h>
#include "packaged_comptr.h"

namespace d3d12 {
	struct factory : public packaged_comptr<IDXGIFactory4>, public uncopyable {
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
	
		factory(int) {
			assert(SUCCEEDED(
				CreateDXGIFactory1(IID_PPV_ARGS(&this->_Impl))
			));
		}
	};
}// namespace d3d12