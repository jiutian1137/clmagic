#pragma once
#include <wrl.h>

namespace d3d12 {
	struct uncopyable {
		uncopyable() = default;
		uncopyable(const uncopyable&) = delete;
		uncopyable& operator=(const uncopyable&) = delete;
	};

	template<typename _Ty>
	struct packaged_comptr {
		void release() {
			this->_Impl = nullptr;
		}
		void swap(packaged_comptr<_Ty>& _Right) {
			this->_Impl.Swap(_Right._Impl);
		}

		_Ty* get() const {
			return this->_Impl.Get();
		}
		_Ty* operator->() const {
			return this->_Impl.Get();
		}
		_Ty& ref() const {
			return *(this->_Impl.Get());
		}
		_Ty& operator*() const {
			return *(this->_Impl.Get());
		}

		bool valid() const {
			return static_cast<bool>(this->_Impl);
		}

		Microsoft::WRL::ComPtr<_Ty> _Impl;
	};



	template<typename _Ty>
	Microsoft::WRL::ComPtr<ID3D12Device> get_device(_Ty& _d3d_obj) {
		Microsoft::WRL::ComPtr<ID3D12Device> _Device;
		_d3d_obj.GetDevice(IID_PPV_ARGS(&_Device));
		return std::move(_Device);
	}
	
	template<typename _Ty>
	Microsoft::WRL::ComPtr<ID3D12Device> get_device(_Ty* pd3d_obj) {
		Microsoft::WRL::ComPtr<ID3D12Device> _Device;
		pd3d_obj->GetDevice(IID_PPV_ARGS(&_Device));
		return std::move(_Device);
	}


}// namespace d3d12