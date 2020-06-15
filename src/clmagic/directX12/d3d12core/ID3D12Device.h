#pragma once
#ifndef clmagic_directX12_ID3D12DEVICE_h_
#define clmagic_directX12_ID3D12DEVICE_h_
#include "d3dx12.h"
#include "IDXGIFactory.h"

namespace d3d12 {

	inline Microsoft::WRL::ComPtr<ID3D12Device> get_ID3D12Device(ID3D12DeviceChild& _Arg) {
		Microsoft::WRL::ComPtr<ID3D12Device> _Device;
		_Arg.GetDevice(IID_PPV_ARGS(&_Device));
		return std::move(_Device);
	}

	inline Microsoft::WRL::ComPtr<ID3D12Device> get_ID3D12Device(ID3D12DeviceChild* _Parg) {
		Microsoft::WRL::ComPtr<ID3D12Device> _Device;
		_Parg->GetDevice(IID_PPV_ARGS(&_Device));
		return std::move(_Device);
	}

	typedef ID3D12Device4 ID3D12NoRaytracingDevice;

	inline Microsoft::WRL::ComPtr<ID3D12NoRaytracingDevice> make_ID3D12NoRaytracingDevice(D3D_FEATURE_LEVEL _Version/* D3D_FEATURE_LEVEL_11_0 */) {
		Microsoft::WRL::ComPtr< ID3D12NoRaytracingDevice > _Device;
		try {
			assert(SUCCEEDED( D3D12CreateDevice(nullptr, _Version, IID_PPV_ARGS(&_Device)) ));
		} catch (const std::exception&) {
			Microsoft::WRL::ComPtr< IDXGIFactory4 > _Factory;
			assert(SUCCEEDED( CreateDXGIFactory1(IID_PPV_ARGS(&_Factory)) ));
			Microsoft::WRL::ComPtr< IDXGIAdapter > _Adaptor;
			assert(SUCCEEDED( _Factory->EnumWarpAdapter(IID_PPV_ARGS(&_Adaptor)) ));
			assert(SUCCEEDED( D3D12CreateDevice(_Adaptor.Get(), _Version, IID_PPV_ARGS(&_Device)) ));
		}
		return std::move(_Device);
	}

	inline Microsoft::WRL::ComPtr<ID3D12NoRaytracingDevice> get_ID3D12NoRaytracingDevice(ID3D12DeviceChild& _Arg) {
		Microsoft::WRL::ComPtr<ID3D12NoRaytracingDevice> _Device;
		_Arg.GetDevice(IID_PPV_ARGS(&_Device));
		return std::move(_Device);
	}
	
	inline Microsoft::WRL::ComPtr<ID3D12NoRaytracingDevice> get_ID3D12NoRaytracingDevice(ID3D12DeviceChild* _Parg) {
		Microsoft::WRL::ComPtr<ID3D12NoRaytracingDevice> _Device;
		_Parg->GetDevice(IID_PPV_ARGS(&_Device));
		return std::move(_Device);
	}
	
	struct device : public packaged_comptr<ID3D12NoRaytracingDevice>, public uncopyable {
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
	
		device(D3D_FEATURE_LEVEL _Version/* D3D_FEATURE_LEVEL_11_0 */) {
			this->_Impl = make_ID3D12NoRaytracingDevice(_Version);
		}
	};

	inline device get_device(ID3D12DeviceChild& _Arg) {
		device _Device;
		_Arg.GetDevice(IID_PPV_ARGS(&_Device._Impl));
		return std::move(_Device);
	}

	inline device get_device(ID3D12DeviceChild* _Parg) {
		device _Device;
		_Parg->GetDevice(IID_PPV_ARGS(&_Device._Impl));
		return std::move(_Device);
	}

}// namespace d3d12

#endif