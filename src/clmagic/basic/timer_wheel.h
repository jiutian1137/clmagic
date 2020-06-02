#pragma once
#ifndef clmagic_basic_TIMER_WHEEL_h_
#define clmagic_basic_TIMER_WHEEL_h_
#include <vector>

namespace clmagic {
	template<typename _Ty, typename _Cont = std::vector<_Ty>>
	struct timer_wheel {
		timer_wheel() = default;

		size_t cycle() const {
			return _Mydata.size();
		}

		void expend_cycle(const _Ty& _Clock) {
			_Mydata.push_back(_Clock);
		}

		void expend_cycle(_Ty&& _Clock) {
			_Mydata.push_back(std::move(_Clock));
		}

		void shrink_cycle() {
			_Mydata.pop_back();
		}

		size_t timer_pointer = 0;

		void turn() {
			timer_pointer = (timer_pointer + 1) %  cycle();
		}

		void clear() {
			_Mydata.clear();
		}

		_Ty& clock() {
			return _Mydata[timer_pointer];
		}
		_Ty& clock(size_t _Pos) {
			return _Mydata[_Pos];
		}
		const _Ty& clock() const {
			return _Mydata[timer_pointer];
		}
		const _Ty& clock(size_t _Pos) const {
			return _Mydata[_Pos];
		}

		_Ty& operator*() {
			return _Mydata[timer_pointer];
		}
		_Ty* operator->() {
			return &_Mydata[timer_pointer];
		}
		const _Ty& operator*() const {
			return _Mydata[timer_pointer];
		}
		const _Ty* operator->() const {
			return &_Mydata[timer_pointer];
		}

	private:
		_Cont _Mydata;
	};
}// namespace clmagic

#endif