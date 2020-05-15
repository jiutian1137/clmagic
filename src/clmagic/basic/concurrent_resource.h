#pragma once
#ifndef clmagic_basic_CONCURRENT_RESOURCE_h_
#define clmagic_basic_CONCURRENT_RESOURCE_h_
#include <mutex>
#include <assert.h>

namespace clmagic {

	template<typename _Ty>
	struct concurrent_resource {
		concurrent_resource() : _Mydata(), _Mydirty(0) {}
		concurrent_resource(_Ty&& _Source, size_t _Nframe) : _Mydata(std::move(_Source)), _Mydirty(_Nframe) {}
		concurrent_resource(const _Ty& _Source, size_t _Nframe) : _Mydata(_Source), _Mydirty(_Nframe) {}

		struct concurrent_wrapper_reference {
			concurrent_wrapper_reference(_Ty& _Source, int& _Dirty, int _Nframes)
				: source(_Source), _Mydirty(_Dirty), _Nframes(_Nframes){ }

			operator _Ty& () {
				return source;
			}

			void commit() {
				_Mydirty = _Nframes;
			}

			_Ty&        source;
		private:
			int& _Mydirty;
			int  _Nframes;
		};

		void set(const _Ty& _Data, size_t _Nframe) {
			auto _Guard = std::lock_guard<std::mutex>(_Mymutex);
			_Mydata  = _Data;
			_Mydirty = _Nframe;
		}
	
		concurrent_wrapper_reference ref(size_t _Nframe) {
			return concurrent_wrapper_reference(_Mydata, _Mydirty, static_cast<int>(_Nframe));
		}

		const _Ty& get() const {
			assert(_Mydirty != 0);
			--_Mydirty;
			return _Mydata;
		}

		const _Ty& val() const {// also get val
			return _Mydata;
		}

		bool updated() {
			return (_Mydirty != 0);
		}

	private:
		_Ty _Mydata;
		mutable int _Mydirty;

		std::mutex _Mymutex;
	};
}// namespace clmagic

#endif