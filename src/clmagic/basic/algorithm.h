#pragma once
#ifndef clmagic_basic_ALGORITHM_h_
#define clmagic_basic_ALGORITHM_h_
#include <assert.h>
#include <algorithm>
#include <numeric>

#include <thread>
#include <future>// parallel_algorithm
#include <functional>

#include <vector>

namespace clmagic {
	using std::accumulate;
	
	// Not recommended
	template<size_t _Thread_count, typename _InIt, typename _Ty, typename _Fn>
	_Ty parallel_accumulate(_InIt _First, _InIt _Last, _Ty _Val, _Fn _Reduce_op) {
		constexpr size_t _Min_chunk_size = 3000;
		const auto       _Element_size   = std::distance(_First, _Last);
		//const auto       _Thread_count = std::max<size_t>(1, std::min<size_t>(_Element_size/_Min_chunk_size, std::thread::hardware_concurrency()/2));
		const auto       _Chunk_size   = _Element_size / _Thread_count;

		const auto _Other_thread_count = _Thread_count - 1;
		std::vector<std::future<_Ty>> _Results(_Other_thread_count);
		std::vector<std::thread>      _Threads(_Other_thread_count);
		using task_type  = _Ty(_InIt, _InIt, _Ty, _Fn);

		for (size_t i = 0; i != _Other_thread_count; ++i) {
			auto _Chunk_last = std::next(_First, _Chunk_size);
			auto _Task       = std::packaged_task<task_type>(std::accumulate<_InIt, _Ty, _Fn>);
			     _Results[i] = _Task.get_future();
			     _Threads[i] = std::thread(std::move(_Task), std::next(_First,1), _Chunk_last, *_First, _Reduce_op);
			     _First      = _Chunk_last;
		}
		std::for_each(_Threads.begin(), _Threads.end(), std::mem_fn(&std::thread::join));

		_Val = std::accumulate(_First, _Last, _Val, _Reduce_op);
		auto       _Rfirst = _Results.begin();
		const auto _Rlast  = _Results.end();
		for ( ; _Rfirst != _Rlast; ++_Rfirst) {
			_Val = _Reduce_op(_Val, _Rfirst->get());
		}
		return _Val;
	}

	template<size_t _Thread_count, typename _InIt, typename _Ty>
	_Ty parallel_accumulate(_InIt _First, _InIt _Last, _Ty _Val) {
		return parallel_accumulate<_Thread_count>(_First, _Last, _Val, std::plus<>());
	}



	template<typename _OutIt, typename _InIt>
	_OutIt merge(_OutIt _Dest, std::pair<_InIt, _InIt> _Pairit) {
		std::copy(_Pairit.first, _Pairit.second, _Dest);
		return (_Dest);
	}

	/*
	@_Example:
		std::string _Test_merge  = "abcdefg";
		std::string _Test_merge2 = "hijklmn";
		std::string _Test_merge3 = "opqrst";
		std::string _Test_merge4 = "uvwkyz";
		std::string _Test_merge_dest = std::string(26, ' ');
		merge(_Test_merge_dest.begin(),
			std::pair< std::string::iterator, std::string::iterator>(_Test_merge.begin(), _Test_merge.end()),
			std::pair< std::string::iterator, std::string::iterator>(_Test_merge2.begin(), _Test_merge2.end()),
			std::pair< std::string::iterator, std::string::iterator>(_Test_merge3.begin(), _Test_merge3.end()),
			std::pair< std::string::iterator, std::string::iterator>(_Test_merge4.begin(), _Test_merge4.end()));
		std::cout << _Test_merge_dest << std::endl;
	*/
	template<typename _OutIt, typename _InIt, typename ..._Iters>
	_OutIt merge(_OutIt _Dest, std::pair<_InIt, _InIt> _Pairit, _Iters... _More) {
		std::copy(_Pairit.first, _Pairit.second, _Dest);
		return (merge(_Dest, _More...));
	}

	template<typename _Container, typename _InIt, typename _InIt2, typename _InIt3>
	_Container& _Replace(_Container& _Cont, _InIt _First, size_t _Count, _InIt2 _Old_first, _InIt2 _Old_last, _InIt3 _New_first, _InIt3 _New_last) {
		// replace _Cont [_Old_first, _Old_last) to [_New_first, _New_last) in [_First, _First + _Count)
		const auto _M = static_cast<size_t>(std::distance(_Old_first, _Old_last));
		const auto _P = static_cast<size_t>(std::distance(_New_first, _New_last));
		if (_M > _Count) {	// match cannot exist
			return (_Cont);
		}

		/* We use size_t counter, because last iterator is invalide when insert into _Cont */
		for (size_t i = 0; i < _Count - _M; /* empty */) {
			if (std::equal(_Old_first, _Old_last, _First)) {
				_First = _Cont.erase(_First, std::next(_First, _M));
				_First = _Cont.insert(_First, _New_first, _New_last);
				std::advance(_First, _P);
				i += _M;
			} else {// no modify
				++i;
				++_First;
			}
		}

		return (_Cont);
	}

	/*
	@_Example:
		auto Output = [](const std::vector<int>& _Cont) {
			for (size_t i = 0; i != _Cont.size(); ++i) {
				std::cout << _Cont[i] << " ";
			}
			std::cout << std::endl;
		};

		std::vector<int> _Data = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
		std::vector<int> _Old = { 3, 4 };
		std::vector<int> _New = { 100, 100, 100, 100 };
		Output(_Data);
		replace(_Data, _Data.begin(), _Data.end(), _Old.begin(), _Old.end(), _New.begin(), _New.end());
		Output(_Data);
	*/
	template<typename _Container, typename _Iter, typename _InIt2, typename _InIt3>
	_Container& replace(_Container& _Cont, _Iter _First, _Iter _Last, _InIt2 _Old_first, _InIt2 _Old_last, _InIt3 _New_first, _InIt3 _New_last) {
		// replace _Cont [_Old_first, _Old_last) to [_New_first, _New_last) in [_First, _Last)
		_Replace(_Cont, _First, std::distance(_First, _Last), _Old_first, _Old_last, _New_first, _New_last);
		return (_Cont);
	}

	template<typename _Container, typename _InIt, typename _InIt2, typename _InIt3>
	_Container replace_copy(const _Container& _Cont, _InIt _First, _InIt _Last, _InIt2 _Old_first, _InIt2 _Old_last, _InIt3 _New_first, _InIt3 _New_last) {
		auto _Clone = _Cont;
		auto _Off_first = static_cast<size_t>(std::distance(_Cont.begin(), decltype(_Cont.begin())(_First)));
		auto _Off_last  = static_cast<size_t>(std::distance(decltype(_Cont.begin())(_Last), _Cont.end()));
		replace(_Clone, std::next(_Clone.begin(), _Off_first), std::prev(_Clone.end(), _Off_last), _Old_first, _Old_last, _New_first, _New_last);
		return (_Clone);
	}

	template<typename _Elem, typename _InIt>
	std::basic_string<_Elem>& str_replace(std::basic_string<_Elem>& _Cont, _InIt _First, _InIt _Last, const _Elem* _Old, const _Elem* _New) {
		// replace _Cont _Old to _New in [_First, _Last)
		return (replace(_Cont, _First, _Last, /*first*/_Old, /*last*/_Old + strlen(_Old), /*first*/_New, /*last*/_New + strlen(_New)));
	}

	template<typename _Elem, typename _InIt>
	std::basic_string<_Elem> str_replace_copy(const std::basic_string<_Elem>& _Cont, _InIt _First, _InIt _Last, const _Elem* _Old, const _Elem* _New) {
		// replace _Cont _Old to _New in [_First, _Last)
		return (replace_copy(_Cont, _First, _Last, /*first*/_Old, /*last*/_Old + strlen(_Old), /*first*/_New, /*last*/_New + strlen(_New)));
	}

}// namespace clmagic

#endif