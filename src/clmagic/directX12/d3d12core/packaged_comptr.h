#pragma once
#include <wrl.h>
#include <assert.h>

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
		operator _Ty& () const {
			return *(this->_Impl.Get());
		}

		bool valid() const {
			return static_cast<bool>(this->_Impl);
		}

		Microsoft::WRL::ComPtr<_Ty> _Impl;
	};

}// namespace d3d12