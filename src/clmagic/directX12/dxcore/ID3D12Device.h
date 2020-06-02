#pragma once
#ifndef clmagic_directX12_ID3D12DEVICE_h_
#define clmagic_directX12_ID3D12DEVICE_h_
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_4.h>
#include <assert.h>

#include "packaged_comptr.h"
#include "enum_string.h"

namespace d3d12 {
	struct device : public packaged_comptr<ID3D12Device>, public uncopyable {
		device() = default;
		device(device&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		device& operator=(device&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}
	
		device(IDXGIFactory4& _Factory, D3D_FEATURE_LEVEL _Version = D3D_FEATURE_LEVEL_11_0) {
			//Microsoft::WRL::ComPtr<IDXGIFactory4> _Factory;
			try {
				assert(SUCCEEDED( D3D12CreateDevice(nullptr/*default-adapter*/, _Version, IID_PPV_ARGS(&this->_Impl)) ));
			} catch (const std::exception&) {
				Microsoft::WRL::ComPtr<IDXGIAdapter> _Adaptor;
				assert(SUCCEEDED( _Factory.EnumWarpAdapter(IID_PPV_ARGS(&_Adaptor)) ));
				assert(SUCCEEDED( D3D12CreateDevice( _Adaptor.Get(), _Version, IID_PPV_ARGS(&this->_Impl)) ));
			}
		}
	};
}// namespace d3d12

#endif