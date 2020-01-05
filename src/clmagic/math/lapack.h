#pragma once
#ifndef clmagic_math_LAPACK_h_
#define clmagic_math_LAPACK_h_
#include "general.h"

namespace clmagic 
{
	

	template<typename _Ty> inline
	bool compare_less(_in(_Ty) _Lhs, _in(_Ty) _Rhs) { return (_Lhs < _Rhs); }

	template<typename _Ty> inline
	bool compare_great(_in(_Ty) _Lhs, _in(_Ty) _Rhs) { return (_Lhs > _Rhs); }

	template<typename _Ty> inline
	bool compare_equals(_in(_Ty) _Lhs, _in(_Ty) _Rhs) { return (_Lhs == _Rhs); }

	template<typename _Ty> inline
	bool compare_lessequal(_in(_Ty) _Lhs, _in(_Ty) _Rhs) { return (_Lhs <= _Rhs); }

	template<typename _Ty> inline
	bool compare_greatequal(_in(_Ty) _Lhs, _in(_Ty) _Rhs) { return (_Lhs >= _Rhs); }

	template<typename _Ty, typename _Fn>
	void _Compare_vector_vector(
		/*out*/	   bool* _Pdst, 
		/*in*/const _Ty* _Plhs, 
		/*in*/const _Ty* _Prhs,
		size_t			 _Size, 
		_Fn*			 _Comparetor);

	template<typename _Ty, typename _Fn>
	bool _Compare_vector_vector(
		/*in*/const _Ty* _Plhs,
		/*in*/const _Ty* _Prhs,
		size_t			 _Size,
		_Fn*			 _Comparetor);

	/*
	c: complex (std::complex float[2] double[2])
	s: real	   (int float double)
	v: vector  (int* float* double*)
	m: matrix  (int** float** double**)
	*/
	template<typename _Ty>
	void _Mul_matrix(
		/*out*/	    _Ty* _Pdst, 
		/*in*/const _Ty* _Plhs, 
		size_t			 _M, 
		size_t			 _N, 
		/*in*/const _Ty* _Prhs, 
		size_t			 _P);

	template<typename _Ty>
	int _Lu(
		/*out*/	  _Ty* _Pdst,
		/*inout*/ _Ty* _Plhs,
		size_t		   _M,
		size_t		   _N);
	
	
	
	
	
	
	/*
	@_vec_<_Mysize, _Ty>:
		Example: vec_<3, float> _Position{ 1.0f, 2.0f, 10.0f };
				vec_<2, std::string> _Fullname{ std::string(), std::string() };
				vec_<9, int> _State{ 0, 0, 0, 0, 0, 1, 0, 0, 0 };
				vec_<5, joint> _Actorjoint{ };
				...
	*/
	template<size_t N, typename T = real_t>
	struct Vec_ {
		static_assert(N > 1, "Vec_<> size error");

		using value_type     = T;
		using vector_type    = Vec_<N, T>;
		using pointer        = T*;
		using const_pointer  = const T*;
		using iterator       = typename std::_Array_iterator<T, N>;
		using const_iterator = typename std::_Array_const_iterator<T, N>;

		/* < construct > */
		explicit Vec_(const_pointer _Ptr) { for (size_t i = 0; i != N; ++i, ++_Ptr) { _Mydata[i] = _Ptr; } }

		constexpr Vec_() : _Mydata{ T(0) } { }
		explicit  Vec_(_in(T) _All) { for (size_t i = 0; i != N; ++i) { _Mydata[i] = _All; } }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1) : _Mydata{ _Val0,_Val1 } { static_assert(N >= 2, "size < 2"); }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2) : _Mydata{ _Val0,_Val1,_Val2 } { static_assert(N >= 3, "size < 3"); }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3) : _Mydata{ _Val0,_Val1,_Val2,_Val3 } { static_assert(N >= 4, "size < 4"); }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4 } { static_assert(N >= 5, "size < 5"); }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5 } { static_assert(N >= 6, "size < 6"); }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6 } { static_assert(N >= 7, "size < 7"); }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7 } { static_assert(N >= 8, "size < 8"); }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8 } { static_assert(N >= 9, "size < 9"); }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8, _in(T) _Val9) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8,_Val9 } { static_assert(N >= 10, "size < 10"); }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8, _in(T) _Val9, _in(T) _Val10) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8,_Val9,_Val10 } { static_assert(N >= 11, "size < 11"); }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8, _in(T) _Val9, _in(T) _Val10, _in(T) _Val11) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8,_Val9,_Val10,_Val11 } { static_assert(N >= 12, "size < 12"); }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8, _in(T) _Val9, _in(T) _Val10, _in(T) _Val11, _in(T) _Val12) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8,_Val9,_Val10,_Val11,_Val12 } { static_assert(N >= 13, "size < 13"); }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8, _in(T) _Val9, _in(T) _Val10, _in(T) _Val11, _in(T) _Val12, _in(T) _Val13) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8,_Val9,_Val10,_Val11,_Val12,_Val13 } { static_assert(N >= 14, "size < 14"); }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8, _in(T) _Val9, _in(T) _Val10, _in(T) _Val11, _in(T) _Val12, _in(T) _Val13, _in(T) _Val14) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8,_Val9,_Val10,_Val11,_Val12,_Val13,_Val14 } { static_assert(N >= 15, "size < 15"); }
		constexpr Vec_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8, _in(T) _Val9, _in(T) _Val10, _in(T) _Val11, _in(T) _Val12, _in(T) _Val13, _in(T) _Val14, _in(T) _Val15) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8,_Val9,_Val10,_Val11,_Val12,_Val13,_Val14,_Val15 } { static_assert(N >= 16, "size < 16"); }
		/*
		std::vector<std::string> _Vecstr;
		for (size_t i = 1; i != 16; ++i) {
			std::string _Str = "constexpr vec_(";
			for (size_t j = 0; j <= i; ++j) {
				_Str += " _in(T)";
				_Str += " _Val" + std::to_string(j);
				_Str += ",";
			}
			_Str.back() = ')';

			_Str += " : data{";
			for (size_t j = 0; j <= i; ++j)
				{
				_Str += "_Val" + std::to_string(j);
				_Str += ",";
				}
				_Str.back() = '}';

			_Str += "{ ";
			_Str += "static_assert(N >=" + std::to_string(i + 1) + ", \"size < " + std::to_string(i + 1) + "\");";
			_Str += "}";
			_Vecstr.push_back(_Str);
		}
		*/
		template<typename _Fn>
		Vec_(_in(Vec_) _A, _Fn _Func) : _Mydata() {
			for (size_t i = 0; i != N; ++i) { _Mydata[i] = _Func(_A[i]); }
		}

		template<typename _Fn>
		Vec_(_in(Vec_) _A, _in(Vec_) _B, _Fn _Func) : _Mydata() {
			for (size_t i = 0; i != N; ++i) { _Mydata[i] = _Func(_A[i], _B[i]); }
		}

		template<typename _Fn>
		Vec_(_in(Vec_) _A, _in(T) _B, _Fn _Func) : _Mydata() {
			for (size_t i = 0; i != N; ++i) { _Mydata[i] = _Func(_A[i], _B); }
		}

		template<typename _Fn>
		Vec_(_in(T) _A, _in(Vec_) _B, _Fn _Func) : _Mydata() {
			for (size_t i = 0; i != N; ++i) { _Mydata[i] = _Func(_A, _B[i]); }
		}

		operator SIMDVec_<N,T>() const {
			if (N >= 4) {
				return (SIMDVec_<N, T>(_Mydata[0], _Mydata[1], _Mydata[2], _Mydata[3]));
			} else if (N == 3) {
				return (SIMDVec_<N, T>(_Mydata[0], _Mydata[1], _Mydata[2], T(0)));
			} else if (N == 2) {
				return (SIMDVec_<N, T>(_Mydata[0], _Mydata[1], T(0), T(0)));
			} else {
				SIMDVec_<N, T> _Result;
				std::copy(this->ptr(), this->ptr(N), _Result.ptr());
				return (_Result);
			}
		}
		/* </ construct > */

		constexpr       T* begin() { return (_Mydata); }
		constexpr       T* end()   { return (_Mydata + N); }
		constexpr const T* begin() const { return (_Mydata); }
		constexpr const T* end()   const { return (_Mydata + N); }

		constexpr   size_t size() const { return (N); }
		constexpr       T* ptr()           { return (_Mydata); }
		constexpr const T* ptr() const        { return (_Mydata); }
		constexpr       T* ptr(size_t _Pos)      { return (_Mydata + _Pos); }
		constexpr const T* ptr(size_t _Pos) const   { return (_Mydata + _Pos); }
		constexpr       T& operator[](size_t _Pos)     { return (_Mydata[_Pos]); }
		constexpr const T& operator[](size_t _Pos) const  { return (_Mydata[_Pos]); }

		template<typename _Fn>
		void assign(_in(Vec_) _A, _Fn _Func) {
			for (size_t i = 0; i != N; ++i) { _Mydata[i] = _Func(_A[i]); }
		}

		template<typename _Fn>
		void assign(_in(Vec_) _A, _in(Vec_) _B, _Fn _Func) {
			for (size_t i = 0; i != N; ++i) { _Mydata[i] = _Func(_A[i], _B[i]); }
		}

		template<typename _Fn>
		void assign(_in(Vec_) _A, _in(T) _B, _Fn _Func) {
			for (size_t i = 0; i != N; ++i) { _Mydata[i] = _Func(_A[i], _B); }
		}

		template<typename _Fn>
		void assign(_in(T) _A, _in(Vec_) _B, _Fn _Func) {
			for (size_t i = 0; i != N; ++i) { _Mydata[i] = _Func(_A, _B[i]); }
		}

		template<size_t _Size>
		Vec_<_Size, T>& prefix() { 
			static_assert( _Size <= N, "[prefix grater than Vec_]" );
			return ( reference_cast<Vec_<_Size, T>>(_Mydata) );
		}

		template<size_t _Size>
		Vec_<_Size, T>& posfix() {
			static_assert(_Size <= N, "[prefix grater than Vec_]");
			size_t _Off = N - _Size;
			return ( reference_cast<Vec_<_Size, T>>(_Mydata + _Off) );
		}

		template<typename ..._Tys>
		auto shuffle(_Tys... _Selector) const ->decltype( Vec_<std::tuple_size<std::tuple<_Tys...>>::value, T>(/* empty */) ) {
			return ( clmagic::shuffle< Vec_</*bg*/std::tuple_size<std::tuple<_Tys...>>::value/*ed*/, T> >(*this, _Selector...) );
		}
		
		T sum() const {
			T _Sum = _Mydata[0];
			for (size_t i = 1; i != N; ++i) { _Sum += _Mydata[i]; }
			return (_Sum);
		}

		T length() const {
			T _Length2 = _Mydata[0] * _Mydata[0];
			for (size_t i = 1; i != N; ++i) { _Length2 += _Mydata[i] * _Mydata[i]; }
			return (sqrt(_Length2));
		}

		Vec_& operator+=(_in(Vec_) _B) { this->assign(*this, _B, [](T a, T b) { return (a + b); }); return (*this); }
		Vec_& operator-=(_in(Vec_) _B) { this->assign(*this, _B, [](T a, T b) { return (a - b); }); return (*this); }
		Vec_& operator*=(_in(Vec_) _B) { this->assign(*this, _B, [](T a, T b) { return (a * b); }); return (*this); }
		Vec_& operator*=(_in(T)    _B) { this->assign(*this, _B, [](T a, T b) { return (a * b); }); return (*this); }
		Vec_& operator/=(_in(Vec_) _B) { this->assign(*this, _B, [](T a, T b) { return (a / b); }); return (*this); }
		Vec_& operator/=(_in(T)    _B) { this->assign(*this, _B, [](T a, T b) { return (a / b); }); return (*this); }
		Vec_& incr      (_in(Vec_) _B) { this->assign(*this, _B, [](T a, T b) { return (a + b); }); return (*this); }
		Vec_& incr      (_in(T)    _B) { this->assign(*this, _B, [](T a, T b) { return (a + b); }); return (*this); }
		Vec_& decr      (_in(Vec_) _B) { this->assign(*this, _B, [](T a, T b) { return (a - b); }); return (*this); }
		Vec_& decr      (_in(T)    _B) { this->assign(*this, _B, [](T a, T b) { return (a - b); }); return (*this); }
		Vec_& normalize ( /* void */ ) { auto _Length = this->length(); return (*this /= _Length); }

		friend Vec_ operator-(_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T a     ) { return (-a);    })); }
		friend Vec_ operator+(_in(Vec_) _A, _in(Vec_) _B) { return (Vec_(_A, _B, [](T a, T b) { return (a + b); })); }
		friend Vec_ operator-(_in(Vec_) _A, _in(Vec_) _B) { return (Vec_(_A, _B, [](T a, T b) { return (a - b); })); }
		friend Vec_ operator*(_in(Vec_) _A, _in(Vec_) _B) { return (Vec_(_A, _B, [](T a, T b) { return (a * b); })); }
		friend Vec_ operator*(_in(Vec_) _A, _in(T)    _B) { return (Vec_(_A, _B, [](T a, T b) { return (a * b); })); }
		friend Vec_ operator*(_in(T)    _A, _in(Vec_) _B) { return (Vec_(_A, _B, [](T a, T b) { return (a * b); })); }
		friend Vec_ operator/(_in(Vec_) _A, _in(Vec_) _B) { return (Vec_(_A, _B, [](T a, T b) { return (a / b); })); }
		friend Vec_ operator/(_in(Vec_) _A, _in(T)    _B) { return (Vec_(_A, _B, [](T a, T b) { return (a / b); })); }
		friend Vec_ operator/(_in(T)    _A, _in(Vec_) _B) { return (Vec_(_A, _B, [](T a, T b) { return (a / b); })); }
		friend Vec_ incr     (_in(Vec_) _A, _in(Vec_) _B) { return (Vec_(_A, _B, [](T a, T b) { return (a + b); })); }
		friend Vec_ incr     (_in(Vec_) _A, _in(T)    _B) { return (Vec_(_A, _B, [](T a, T b) { return (a + b); })); }
		friend Vec_ decr     (_in(Vec_) _A, _in(Vec_) _B) { return (Vec_(_A, _B, [](T a, T b) { return (a - b); })); }
		friend Vec_ decr     (_in(Vec_) _A, _in(T)    _B) { return (Vec_(_A, _B, [](T a, T b) { return (a - b); })); }

		friend Vec_ mod      (_in(Vec_) _A, _in(Vec_) _B) { return (Vec_(_A, _B, [](T a, T b) { return mod(a, b); })); }
		friend Vec_ mod      (_in(Vec_) _A, _in(T)    _B) { return (Vec_(_A, _B, [](T a, T b) { return mod(a, b); })); }
		friend Vec_ floor    (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return floor(x);  })); }
		friend Vec_ ceil     (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return ceil(x);   })); }
		friend Vec_ trunc    (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return trunc(x);  })); }

		friend Vec_ sqrt     (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return sqrt(x);   })); }
		friend Vec_ exp      (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return exp(x);    })); }
		friend Vec_ exp2     (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return exp2(x);   })); }

		friend Vec_ pow      (_in(Vec_) _A, _in(Vec_) _B) { return (Vec_(_A, _B, [](T a, T b) { return pow(a, b); })); }
		friend Vec_ pow      (_in(Vec_) _A, _in(T)    _B) { return (Vec_(_A, _B, [](T a, T b) { return pow(a, b); })); }

		friend Vec_ log10    (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return log10(x);  })); }
		friend Vec_ log2     (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return log2(x);   })); }
		friend Vec_ loge     (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return loge(x);   })); }

		friend Vec_ sin      (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return sin(x);    })); }
		friend Vec_ cos      (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return cos(x);    })); }
		friend Vec_ tan      (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return tan(x);    })); }

		friend Vec_ asin     (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return asin(x);   })); }
		friend Vec_ acos     (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return acos(x);   })); }
		friend Vec_ atan     (_in(Vec_) _A    /* void */) { return (Vec_(_A,     [](T x     ) { return atan(x);   })); }
		friend Vec_ atan2    (_in(Vec_) _Y, _in(Vec_) _X) { return (Vec_(_Y, _X, [](T y, T x) { return atan2(y, x); })); }

		friend T    dot      (_in(Vec_) _A, _in(Vec_) _B) { auto _Tmp = _A * _B; return (_Tmp.sum()); }
		friend T    dot2     (_in(Vec_) _A, _in(Vec_) _B) { auto _Tmp = _A * _B; return (_Tmp[0] + _Tmp[1]); }
		friend T    dot3     (_in(Vec_) _A, _in(Vec_) _B) { auto _Tmp = _A * _B; return (_Tmp[0] + _Tmp[1] + _Tmp[2]); }
		friend T    dot4     (_in(Vec_) _A, _in(Vec_) _B) { auto _Tmp = _A * _B; return (_Tmp[0] + _Tmp[1] + _Tmp[2] + _Tmp[3]); }
		friend Vec_ cross3   (_in(Vec_) _A, _in(Vec_) _B) { return (_A.shuffle(1, 2, 0) * _B.shuffle(2, 0, 1) - _A.shuffle(2, 0, 1) * _B.shuffle(1, 2, 0)); }
		friend Vec_ normalize(_in(Vec_) _A    /* void */) { auto _Length = _A.length(); return (_A / _Length); }

		friend std::string to_string(_in(Vec_<N, T>) _A) {
			using std::to_string;
			std::string _Str = "Vec" + to_string(_A.size()) + "[";
			for (size_t i = 0; i != _A.size(); ++i) {
				_Str += to_string(_A[i]) + ",";
			}
			_Str.back() = ']';
			return (_Str);
		}

		friend std::ostream& operator<<(_inout(std::ostream) _Ostr, _in(Vec_<N, T>) _A) {
			return (_Ostr << to_string(_A));
		}

		T _Mydata[N];
	};// struct Vec_<>

	constexpr auto cX = Vec_<3, real_t>(1.f, 0.f, 0.f);
	constexpr auto cY = Vec_<3, real_t>(0.f, 1.f, 0.f);
	constexpr auto cZ = Vec_<3, real_t>(0.f, 0.f, 1.f);

	/* optimization */
	template<> template<typename _Fn> inline
		Vec_<2, float>::Vec_(_in(Vec_) _A, _Fn _Func)
		: _Mydata{ _Func(_A[0]), _Func(_A[1]) } { }

	template<> template<typename _Fn> inline
		Vec_<2, float>::Vec_(_in(Vec_) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B[0]), _Func(_A[1], _B[1]) } { }

	template<> template<typename _Fn> inline
		Vec_<2, float>::Vec_(_in(Vec_) _A, _in(float) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B), _Func(_A[1], _B) } { }

	template<> template<typename _Fn> inline
		Vec_<2, float>::Vec_(_in(float) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A, _B[0]), _Func(_A, _B[1]) } { }

	template<> template<typename _Fn> inline
		Vec_<2, double>::Vec_(_in(Vec_) _A, _Fn _Func)
		: _Mydata{ _Func(_A[0]), _Func(_A[1]) } { }

	template<> template<typename _Fn> inline
		Vec_<2, double>::Vec_(_in(Vec_) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B[0]), _Func(_A[1], _B[1]) } { }

	template<> template<typename _Fn> inline
		Vec_<2, double>::Vec_(_in(Vec_) _A, _in(double) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B), _Func(_A[1], _B) } { }

	template<> template<typename _Fn> inline
		Vec_<2, double>::Vec_(_in(double) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A, _B[0]), _Func(_A, _B[1]) } { }

	template<> template<typename _Fn> inline
		Vec_<2, int>::Vec_(_in(Vec_) _A, _Fn _Func)
		: _Mydata{ _Func(_A[0]), _Func(_A[1]) } { }

	template<> template<typename _Fn> inline
		Vec_<2, int>::Vec_(_in(Vec_) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B[0]), _Func(_A[1], _B[1]) } { }

	template<> template<typename _Fn> inline
		Vec_<2, int>::Vec_(_in(Vec_) _A, _in(int) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B), _Func(_A[1], _B) } { }

	template<> template<typename _Fn> inline
		Vec_<2, int>::Vec_(_in(int) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A, _B[0]), _Func(_A, _B[1]) } { }

	template<> template<typename _Fn> inline
		Vec_<3, float>::Vec_(_in(Vec_) _A, _Fn _Func)
		: _Mydata{ _Func(_A[0]), _Func(_A[1]), _Func(_A[2]) } { }

	template<> template<typename _Fn> inline
		Vec_<3, float>::Vec_(_in(Vec_) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B[0]), _Func(_A[1], _B[1]), _Func(_A[2], _B[2]) } { }

	template<> template<typename _Fn> inline
		Vec_<3, float>::Vec_(_in(Vec_) _A, _in(float) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B), _Func(_A[1], _B), _Func(_A[2], _B) } { }

	template<> template<typename _Fn> inline
		Vec_<3, float>::Vec_(_in(float) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A, _B[0]), _Func(_A, _B[1]), _Func(_A, _B[2]) } { }

	template<> template<typename _Fn> inline
		Vec_<3, double>::Vec_(_in(Vec_) _A, _Fn _Func)
		: _Mydata{ _Func(_A[0]), _Func(_A[1]), _Func(_A[2]) } { }

	template<> template<typename _Fn> inline
		Vec_<3, double>::Vec_(_in(Vec_) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B[0]), _Func(_A[1], _B[1]), _Func(_A[2], _B[2]) } { }

	template<> template<typename _Fn> inline
		Vec_<3, double>::Vec_(_in(Vec_) _A, _in(double) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B), _Func(_A[1], _B), _Func(_A[2], _B) } { }

	template<> template<typename _Fn> inline
		Vec_<3, double>::Vec_(_in(double) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A, _B[0]), _Func(_A, _B[1]), _Func(_A, _B[2]) } { }

	template<> template<typename _Fn> inline
		Vec_<3, int>::Vec_(_in(Vec_) _A, _Fn _Func)
		: _Mydata{ _Func(_A[0]), _Func(_A[1]), _Func(_A[2]) } { }

	template<> template<typename _Fn> inline
		Vec_<3, int>::Vec_(_in(Vec_) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B[0]), _Func(_A[1], _B[1]), _Func(_A[2], _B[2]) } { }

	template<> template<typename _Fn> inline
		Vec_<3, int>::Vec_(_in(Vec_) _A, _in(int) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B), _Func(_A[1], _B), _Func(_A[2], _B) } { }

	template<> template<typename _Fn> inline
		Vec_<3, int>::Vec_(_in(int) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A, _B[0]), _Func(_A, _B[1]), _Func(_A, _B[2]) } { }

	template<> template<typename _Fn> inline
		Vec_<4, float>::Vec_(_in(Vec_) _A, _Fn _Func)
		: _Mydata{ _Func(_A[0]), _Func(_A[1]), _Func(_A[2]), _Func(_A[3]) } { }

	template<> template<typename _Fn> inline
		Vec_<4, float>::Vec_(_in(Vec_) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B[0]), _Func(_A[1], _B[1]), _Func(_A[2], _B[2]), _Func(_A[3], _B[3]) } { }

	template<> template<typename _Fn> inline
		Vec_<4, float>::Vec_(_in(Vec_) _A, _in(float) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B), _Func(_A[1], _B), _Func(_A[2], _B), _Func(_A[3], _B) } { }

	template<> template<typename _Fn> inline
		Vec_<4, float>::Vec_(_in(float) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A, _B[0]), _Func(_A, _B[1]), _Func(_A, _B[2]), _Func(_A, _B[3]) } { }

	template<> template<typename _Fn> inline
		Vec_<4, double>::Vec_(_in(Vec_) _A, _Fn _Func)
		: _Mydata{ _Func(_A[0]), _Func(_A[1]), _Func(_A[2]), _Func(_A[3]) } { }

	template<> template<typename _Fn> inline
		Vec_<4, double>::Vec_(_in(Vec_) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B[0]), _Func(_A[1], _B[1]), _Func(_A[2], _B[2]), _Func(_A[3], _B[3]) } { }

	template<> template<typename _Fn> inline
		Vec_<4, double>::Vec_(_in(Vec_) _A, _in(double) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B), _Func(_A[1], _B), _Func(_A[2], _B), _Func(_A[3], _B) } { }

	template<> template<typename _Fn> inline
		Vec_<4, double>::Vec_(_in(double) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A, _B[0]), _Func(_A, _B[1]), _Func(_A, _B[2]), _Func(_A, _B[3]) } { }

	template<> template<typename _Fn> inline
		Vec_<4, int>::Vec_(_in(Vec_) _A, _Fn _Func)
		: _Mydata{ _Func(_A[0]), _Func(_A[1]), _Func(_A[2]), _Func(_A[3]) } { }

	template<> template<typename _Fn> inline
		Vec_<4, int>::Vec_(_in(Vec_) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B[0]), _Func(_A[1], _B[1]), _Func(_A[2], _B[2]), _Func(_A[3], _B[3]) } { }

	template<> template<typename _Fn> inline
		Vec_<4, int>::Vec_(_in(Vec_) _A, _in(int) _B, _Fn _Func)
		: _Mydata{ _Func(_A[0], _B), _Func(_A[1], _B), _Func(_A[2], _B), _Func(_A[3], _B) } { }

	template<> template<typename _Fn> inline
		Vec_<4, int>::Vec_(_in(int) _A, _in(Vec_) _B, _Fn _Func)
		: _Mydata{ _Func(_A, _B[0]), _Func(_A, _B[1]), _Func(_A, _B[2]), _Func(_A, _B[3]) } { }
	/*
	constexpr static size_t eNone = 0;
	constexpr static size_t eVector = 1;
	constexpr static size_t eScalar = 2;

	auto _Cast_arg = [](size_t _Tyleft, size_t _Tyright, std::string _Ty) -> std::string {
		char   _Str[1024];
		size_t _Endpos = 0;
		if (_Tyleft == eVector && _Tyright == eVector) {
			_Endpos = snprintf(_Str, 1023, "_in(Vec_) _A, _in(Vec_) _B");
		} else if (_Tyleft == eScalar && _Tyright == eVector) {
			_Endpos = snprintf(_Str, 1023, "_in(%s) _A, _in(Vec_) _B", _Ty.c_str());
		} else if (_Tyleft == eVector && _Tyright == eScalar) {
			_Endpos = snprintf(_Str, 1023, "_in(Vec_) _A, _in(%s) _B", _Ty.c_str());
		} else if (_Tyleft == eNone || _Tyright == eNone) {
			return ("_in(Vec_) _A");
		}
		return (std::string(_Str, _Str + _Endpos));
	};

	auto _Cast_call = [](size_t i, size_t _Tyleft, size_t _Tyright) -> std::string {
		char _Str[1024];
		size_t _Endpos = 0;
		using std::to_string;
		if (_Tyleft == eVector && _Tyright == eVector) {
			_Endpos = snprintf(_Str, 1023, "_A[%i], _B[%i]", i, i);
		} else if (_Tyleft == eScalar && _Tyright == eVector) {
			_Endpos = snprintf(_Str, 1023, "_A, _B[%i]", i);
		} else if (_Tyleft == eVector && _Tyright == eScalar) {
			_Endpos = snprintf(_Str, 1023, "_A[%i], _B", i);
		} else if (_Tyleft == eNone || _Tyright == eNone) {
			_Endpos = snprintf(_Str, 1023, "_A[%i]", i);
		}
		return (std::string(_Str, _Str + _Endpos));
	};

	auto _Func_decl = [_Cast_arg, _Cast_call](std::string _Funcname, size_t N, std::string _Ty, size_t _Tyleft, size_t _Tyright) -> std::string {
		char _Str[4096];
		size_t _Endpos = 0;
		if (_Funcname == "Vec_") {
			std::string _Str_call;
			for (size_t i = 0; i != N; ++i) {
				_Str_call += "_Func(" + _Cast_call(i, _Tyleft, _Tyright) + ")";
				if (i != N - 1) _Str_call += ", ";
			}

			_Endpos = snprintf(_Str, 4095,
				"template<> template<typename _Fn> inline\n"
				"   Vec_<%i, %s>::%s(%s, _Fn _Func)\n"
				"      : _Mydata{%s} { }", N, _Ty.c_str(), _Funcname.c_str(), _Cast_arg(_Tyleft, _Tyright, _Ty).c_str(), _Str_call.c_str());
		} else {
			std::string _Str_call;
			for (size_t i = 0; i != N; ++i) {
				_Str_call += "_Mydata[" + std::to_string(i) + "] = _Func(" + _Cast_call(i, _Tyleft, _Tyright) + "); ";
			}

			_Endpos = snprintf(_Str, 4095,
				"template<> template<typename _Fn> inline\n"
				"   void Vec_<%i, %s>::%s(%s, _Fn _Func)\n"
				"   { %s }", N, _Ty.c_str(), _Funcname.c_str(), _Cast_arg(_Tyleft, _Tyright, _Ty).c_str(), _Str_call.c_str());
		}

		return (std::string(_Str, _Str + _Endpos));
	};

	for (size_t N = 2; N <= 4; ++N) {
		std::cout << _Func_decl("Vec_", N, "float", eVector, eNone) << std::endl << std::endl;
		std::cout << _Func_decl("Vec_", N, "float", eVector, eVector) << std::endl << std::endl;
		std::cout << _Func_decl("Vec_", N, "float", eVector, eScalar) << std::endl << std::endl;
		std::cout << _Func_decl("Vec_", N, "float", eScalar, eVector) << std::endl << std::endl;
		std::cout << _Func_decl("Vec_", N, "double", eVector, eNone) << std::endl << std::endl;
		std::cout << _Func_decl("Vec_", N, "double", eVector, eVector) << std::endl << std::endl;
		std::cout << _Func_decl("Vec_", N, "double", eVector, eScalar) << std::endl << std::endl;
		std::cout << _Func_decl("Vec_", N, "double", eScalar, eVector) << std::endl << std::endl;
		std::cout << _Func_decl("Vec_", N, "int", eVector, eNone) << std::endl << std::endl;
		std::cout << _Func_decl("Vec_", N, "int", eVector, eVector) << std::endl << std::endl;
		std::cout << _Func_decl("Vec_", N, "int", eVector, eScalar) << std::endl << std::endl;
		std::cout << _Func_decl("Vec_", N, "int", eScalar, eVector) << std::endl << std::endl;
	}
	*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


/* 
@_Note: 1. Can only use clmagic_using_SIMDvector
		2. Structure cannot be SIMD if not use clmagic_using_SIMDvector
*/
#define clmagic_using_SIMDMath

#ifdef clmagic_using_SIMDMath
	template<typename T = real_t> using Vector2_ = SIMDVec_<2, T>;
	template<typename T = real_t> using Vector3_ = SIMDVec_<3, T>;
	template<typename T = real_t> using Vector4_ = SIMDVec_<4, T>;
	template<size_t N, typename T = real_t> using Vector_ = SIMDVec_<N, T>;
#else
	template<typename T = real_t> using Vector2_ = Vec_<2, T>;
	template<typename T = real_t> using Vector3_ = Vec_<3, T>;
	template<typename T = real_t> using Vector4_ = Vec_<4, T>;
	template<size_t N, typename T = real_t> using Vector_ = Vec_<N, T>;
#endif
	/*
@_Normal_<T>: Internal structure, as Vector3_<T>
@_Traits: length = 1.0
*/
	template<typename T>
	struct Normal_ : public Vector3_<T> {
		using _Mybase = Vector3_<T>;

		Normal_(T _X, T _Y, T _Z)
			: _Mybase(_X, _Y, _Z) { this->normalize(); }

		Normal_(_in(Vector3_<T>) _Vector)
			: _Mybase(normalize(_Vector)) { }

		friend Normal_ operator-(_in(Normal_) _A) {
			return (Normal_(-_A[0], -_A[1], -_A[2]));
		}
	};





/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

	template<size_t M, typename T>
	struct RowVector_ : Vector_<M, T> {
		RowVector_(_in(Vector_<M, T>) _Vec)
			: Vector_<M, T>(_Vec) { }
	};

	template<size_t N, typename T>
	struct ColVector_ : Vector_<N, T> {
		ColVector_(_in(Vector_<N, T>) _Vec)
			: Vector_<N, T>(_Vec) { }
	};

/*
@_Mat_<M,N,T>: 
@_clmagic_using_SIMDvector: 
	Must be align 16, Note:( @Mat_::row(size_t) line:633 )
@_Note: 
	Must closed clmagic_using_SIMDvector if used for Mat_<M,N,String> or Mat_<M,N,Other>, line:539
*/
	template<size_t M, size_t N, typename T = real_t>
#if defined(clmagic_using_SIMDMath)
	struct _declspec(align(16)) Mat_ {
#else
	struct Mat_ {
#endif
		using row_type = typename Mat_<1, N, T>;
		using col_type = typename Mat_<M, 1, T>;
		using iterator = typename std::_Array_iterator<T, M * N>;
		using const_iterator = typename std::_Array_const_iterator<T, M * N>;

		static Mat_<M, N, T> all(_in(T) _Val);
		static Mat_<M, N, T> zero();
		static Mat_<M, N, T> identity();


		constexpr Mat_() : _Mydata() { /* empty */ }
		explicit Mat_(T _Val0) { for (size_t i = 0, j = 0; i < M; ++j, j %= N ? ++i : 1) this->at(i, j) = _Val0; }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1) : _Mydata{ _Val0,_Val1 } { static_assert(M * N >= 2, "size < 2"); }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2) : _Mydata{ _Val0,_Val1,_Val2 } { static_assert(M * N >= 3, "size < 3"); }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3) : _Mydata{ _Val0,_Val1,_Val2,_Val3 } { static_assert(M * N >= 4, "size < 4"); }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4 } { static_assert(M * N >= 5, "size < 5"); }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5 } { static_assert(M * N >= 6, "size < 6"); }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6 } { static_assert(M * N >= 7, "size < 7"); }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7 } { static_assert(M * N >= 8, "size < 8"); }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8 } { static_assert(M * N >= 9, "size < 9"); }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8, _in(T) _Val9) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8,_Val9 } { static_assert(M * N >= 10, "size < 10"); }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8, _in(T) _Val9, _in(T) _Val10) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8,_Val9,_Val10 } { static_assert(M * N >= 11, "size < 11"); }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8, _in(T) _Val9, _in(T) _Val10, _in(T) _Val11) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8,_Val9,_Val10,_Val11 } { static_assert(M * N >= 12, "size < 12"); }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8, _in(T) _Val9, _in(T) _Val10, _in(T) _Val11, _in(T) _Val12) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8,_Val9,_Val10,_Val11,_Val12 } { static_assert(M * N >= 13, "size < 13"); }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8, _in(T) _Val9, _in(T) _Val10, _in(T) _Val11, _in(T) _Val12, _in(T) _Val13) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8,_Val9,_Val10,_Val11,_Val12,_Val13 } { static_assert(M * N >= 14, "size < 14"); }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8, _in(T) _Val9, _in(T) _Val10, _in(T) _Val11, _in(T) _Val12, _in(T) _Val13, _in(T) _Val14) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8,_Val9,_Val10,_Val11,_Val12,_Val13,_Val14 } { static_assert(M * N >= 15, "size < 15"); }
		constexpr Mat_(_in(T) _Val0, _in(T) _Val1, _in(T) _Val2, _in(T) _Val3, _in(T) _Val4, _in(T) _Val5, _in(T) _Val6, _in(T) _Val7, _in(T) _Val8, _in(T) _Val9, _in(T) _Val10, _in(T) _Val11, _in(T) _Val12, _in(T) _Val13, _in(T) _Val14, _in(T) _Val15) : _Mydata{ _Val0,_Val1,_Val2,_Val3,_Val4,_Val5,_Val6,_Val7,_Val8,_Val9,_Val10,_Val11,_Val12,_Val13,_Val14,_Val15 } { static_assert(M * N >= 16, "size < 16"); }
		/*
		std::vector<std::string> _Vecstr;
		for (size_t i = 1; i != 16; ++i) {
			std::string _Str = "constexpr Mat_(";
			for (size_t j = 0; j <= i; ++j) {
				_Str += " _in(T)";
				_Str += " _Val" + std::to_string(j);
				_Str += ",";
			}
			_Str.back() = ')';

			_Str += " : data{";
			for (size_t j = 0; j <= i; ++j) {
				_Str += "_Val" + std::to_string(j);
				_Str += ",";
			}
			_Str.back() = ')';

			_Str += "{ ";
			_Str += "static_assert(M * N >=" + std::to_string(i + 1) + ", \"size < " + std::to_string(i + 1) + "\");";
			_Str += "}";
			_Vecstr.push_back(_Str);
		}
		*/

		explicit Mat_(const T* _Array);
		Mat_(std::initializer_list<T> _Ilist);
		template<size_t _Rm, size_t _Rn> Mat_(_in(Mat_<_Rm, _Rn, T>) _Right);
		
		explicit operator std::string() const;

		constexpr size_t rows() const { return (M); }
		constexpr size_t cols() const { return (N); }
		constexpr size_t size() const { return (rows() * cols()); }
		constexpr       T* ptr()	   { return (_Mydata[0]); }
		constexpr const T* ptr() const { return (_Mydata[0]); }
		constexpr       T* ptr(size_t _Pos) { return (_Mydata[0] + _Pos); }
		constexpr const T* ptr(size_t _Pos) const { return (_Mydata[0] + _Pos); }
		constexpr       T* operator[] (size_t _Pos)	      { return (_Mydata[_Pos]); }
		constexpr const T* operator[] (size_t _Pos) const { return (_Mydata[_Pos]); }
		constexpr       T& at(size_t _Row, size_t _Col)		  { return (_Mydata[_Row][_Col]); }
		constexpr const T& at(size_t _Row, size_t _Col) const { return (_Mydata[_Row][_Col]); }
		Vector_<N, T>& row(size_t _Pos) { return reference_cast<Vector_<N, T>>(_Mydata[_Pos]); }
		const Vector_<N, T>& row(size_t _Pos) const { return const_reference_cast<const Vector_<N, T>>(_Mydata[_Pos]); }

		void assign(std::initializer_list<T> _Ilist);

		Mat_<N, M, T> transpose() const;
		Mat_<M, N, T> inverse() const;
		bool is_identity() const;

		Mat_& operator+=(_in(Mat_) _B) {
			for (size_t i = 0; i != M; ++i) {
				this->row(i) += _B.row(i);
			}
			return (*this);
		}

		Mat_& operator-=(_in(Mat_) _B) {
			for (size_t i = 0; i != M; ++i) {
				this->row(i) -= _B.row(i);
			}
			return (*this);
		}

		Mat_& operator*=(_in(T) _B) {
			for (size_t i = 0; i != M; ++i) {
				this->row(i) *= _B;
			}
			return (*this);
		}

		Mat_& operator/=(_in(T) _B) {
			for (size_t i = 0; i != M; ++i) {
				this->row(i) /= _B;
			}
			return (*this);
		}

		/* Mat_ operator*=(Mat_) at line:745 */

		friend Mat_ operator-(_in(Mat_) _A) {
			return (_A.inverse());
		}

		friend Mat_ operator+(_in(Mat_) _A, _in(Mat_) _B) {
			Mat_ _C;
			for (size_t i = 0; i != M; ++i) {
				_C.row(i) = _A.row(i) + _B.row(i);
			}
			return (_C);
		}

		friend Mat_ operator-(_in(Mat_) _A, _in(Mat_) _B) {
			Mat_ _C;
			for (size_t i = 0; i != M; ++i) {
				_C.row(i) = _A.row(i) - _B.row(i);
			}
			return (_C);
		}

		friend Mat_ operator*(_in(Mat_) _A, _in(T) _B) {
			Mat_ _C;
			for (size_t i = 0; i != M; ++i) {
				_C.row(i) = _A.row(i) * _B;
			}
			return (_C);
		}

		friend Mat_ operator*(_in(T) _A, _in(Mat_) _B) {
			Mat_ _C;
			for (size_t i = 0; i != M; ++i) {
				_C.row(i) = _A * _B.row(i);
			}
			return (_C);
		}

		friend Mat_ operator/(_in(Mat_) _A, _in(T) _B) {
			Mat_ _C;
			for (size_t i = 0; i != M; ++i) {
				_C.row(i) = _A.row(i) / _B;
			}
			return (_C);
		}

		friend Mat_ operator/(_in(T) _A, _in(Mat_) _B) {
			Mat_ _C;
			for (size_t i = 0; i != M; ++i) {
				_C.row(i) = _A / _B.row(i);
			}
			return (_C);
		}

		template<size_t P>
		friend Mat_<M,P,T> operator*(_in(Mat_<M, N, T>) _A, _in(Mat_<N, P, T>) _B) {
			Mat_<M, P, T> _C;
			_Mul_matrix(_C.ptr(), _A.ptr(), M, N, _B.ptr(), P);
			return (_C);
		}

		/* VecN as MatNx1 */
		friend RowVector_<M,T> operator*(_in(Mat_<M,N,T>) _A, _in(RowVector_<N, T>) _B) {
			const auto& _B2 = const_reference_cast< const Mat_<N,1,T> >( _B );
			return ( const_reference_cast<const RowVector_<M,T>>(_A * _B2) );
			/*
			  [ 00 01 02 .. 0N ]     [0]  [A.row(0) dot B]--┱--
			  [ 10 11 12 .. 1N ]     [1]  [A.row(1) dot B]  ↓
			A:[ 20 21 22 .. 2N ] * B:[3]= [A.row(2) dot B] size:M
			  [ ...            ]     [.]  [...           ]  ↑
			  [ M0 M1 M2 .. MN ]     [N]  [A.row(M) dot B]--┻--
			*/
		}

		/* MatNx1*Mat */
		friend ColVector_<N, T> operator*(_in(ColVector_<M, T>) _A, _in(Mat_<M, N, T>) _B) {
			const auto& _A2 = const_reference_cast< const Mat_<1,M,T> >( _A );
			return ( const_reference_cast<const ColVector_<N, T>>(_A2 * _B) );
			/*
			                      [ 00 01 02 .. 0N ]   
			                      [ 10 11 12 .. 1N ]   
			A:[0][1][2][.][M] * B:[ 20 21 22 .. 2N ] = [A dot B.col(0)] [A dot B.col(1)] [..] [A dot B.col(N)]
			                      [ ...            ]   |------------------->  size: N  <---------------------|
			                      [ M0 M1 M2 .. MN ]   
			*/
		}

		friend std::string to_string(_in(Mat_) _A) {	// convert to string
			using std::to_string;
			std::string _Str = "mat" + std::to_string(M) + "x" + std::to_string(N) + "[";
			for (int i = 0; i != M; ++i) {
				for (size_t j = 0; j != N; ++j) {
					_Str += to_string(_A[i][j]);
					_Str += ",";
				}
				_Str.back() = '\n';
			}
			_Str += "]";
			return (_Str);
		}

		friend std::ostream& operator<<(std::ostream& _Ostr, _in(Mat_) _A) {
			return (_Ostr << to_string(_A));
		}

		T _Mydata[M][N];
	};

	/* only square matrix, for example:M = N = P */
	template<size_t M, typename T> inline
	Mat_<M, M, T> operator*=(_inout(Mat_<M, M, T>) _A, _in(Mat_<M, M, T>) _B) 
		{	// A = A*B
		_Mul_matrix(_A.ptr(), _A.ptr(), M, M, _B.ptr(), M);
		}

	using Mat2i = Mat_<2, 2, int>;
	using Mat2f = Mat_<2, 2, float>;
	using Mat2d = Mat_<2, 2, double>;
	using Mat2  = Mat_<2, 2, real_t>;

	using Mat3i = Mat_<3, 3, int>;
	using Mat3f = Mat_<3, 3, float>;
	using Mat3d = Mat_<3, 3, double>;
	using Mat3  = Mat_<3, 3, real_t>;

	using Mat3x4i = Mat_<3, 4, int>;
	using Mat3x4f = Mat_<3, 4, float>;
	using Mat3x4d = Mat_<3, 4, double>;
	using Mat3x4  = Mat_<3, 4, real_t>;;

	using Mat4i = Mat_<4, 4, int>;
	using Mat4f = Mat_<4, 4, float>;
	using Mat4d = Mat_<4, 4, double>;
	using Mat4  = Mat_<4, 4, real_t>;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
	
	template<typename _Vec>
		auto distance(
			_in(_Vec) _A,
			_in(_Vec) _B);

	template<typename _Vec>
		_Vec proj(
			_in(_Vec) _Vector, 
			_in(_Vec) _Proj);

	template<typename _Vec>
		_Vec reflect(
			_in(_Vec) _Direction, 
			_in(_Vec) _Normal);

	/* dot(_I, _Nref) < 0 ? _Normal : -_Normal */
	template<typename _Vec>
		_Vec faceforward(
			_in(_Vec) _Normal, 
			_in(_Vec) _I, 
			_in(_Vec) _Nref);

	template<size_t N, typename T>
		T radians(
			_in(Vector_<N, T>) _A, 
			_in(Vector_<N, T>) _B);

	template<typename T>
		Vector3_<T> proj(
			_in(Vector3_<T>) _Vector, 
			_in(Normal_<T>) _Proj);

	template<typename T>
		Vector3_<T> proj(
			_in(Vector3_<T>) _Vector, 
			_in(Normal_<T>)  _Rt, 
			_in(Normal_<T>)  _Up, 
			_in(Normal_<T>)  _Fwd );

	template<typename T>
		Vector3_<T> reflect(
			_in(Vector3_<T>) _Direction, 
			_in(Normal_<T>)  _Normal);

	template<typename T> 
		Vector2_<T> orthogonal(
			_in(Vector2_<T>) _Lhs);

	template<typename T> 
		void orthogonal(
			_inout(std::vector<Vector3_<T>>) _Vectors);

	
	template<size_t M, size_t N, typename T> Mat_<N, M, T> transpose( _in(Mat_<M, N, T>) _Lhs);
	template<size_t M, size_t N, typename T> Mat_<M, N, T> inverse( _in(Mat_<M, N, T>) _Lhs);

	template<typename T> 
		Vector3_<T> transform_coord(_in(Vector3_<T>) _Lhs, _in(Mat_<4, 4, T>) _Matrix);
	template<typename T> 
		Normal_<T> transform_normal(_in(Normal_<T>) _Lhs, _in(Mat_<4, 4, T>) _Matrix);
	
	Vector3_<float> screen_to_world(_in(Vector3_<float>) _Vec, _in(Mat4) _Viewmatrix, _in(Mat4) _Projmatrix);

	template<typename _Ty>
		_Ty determinant(const Mat_<2, 2, _Ty> _Src);
		
	template<typename T> 
		Mat_<4, 4, T> scaling_matrix(
			_in(T) _Width, 
			_in(T) _Height, 
			_in(T) _Depth );

	template<typename T>
		Mat_<4, 4, T> scaling_matrix( 
			_in(Vector3_<T>) _sXyz );

	template<typename T> 
		Mat_<4, 4, T> rotation_matrix( 
			_in(Normal_<T>) _Axis, 
			_in(T)          _Radians );

	template<typename T>
		Mat_<4, 4, T> translation_matrix(
			_in(T) _X, 
			_in(T) _Y, 
			_in(T) _Z );

	template<typename T>
		Mat_<4, 4, T> translation_matrix(
			_in(Vector3_<T>) _dXyz );

	/*
	@_Eye as eye position
	@_Fwd as forward vector
	@_Up  as up vector, require normalize vector
	*/
	template<typename T> 
		Mat_<4, 4, T> Lookat( 
			_in(Vector3_<T>) _Eye, 
			_in(Normal_<T>)  _Fwd, 
			_in(Normal_<T>)  _Up);

	// @_Target as target position
	template<typename T>
		Mat_<4, 4, T> Lookat2( 
			_in(Vector3_<T>) _Eye, 
			_in(Vector3_<T>) _Target, 
			_in(Normal_<T>)  _Up);

	template<typename T> 
		Mat_<4, 4, T> Ortho(
			_in(T) _Left, 
			_in(T) _Right, 
			_in(T) _Bottom, 
			_in(T) _Top, 
			_in(T) _Nearplane = 0.0f, 
			_in(T) _Farplane = 10000.0f);

	template<typename T> 
		Mat_<4, 4, T> Perspective(
			_in(T) _Fov, 
			_in(T) _Aspect, 
			_in(T) _Znear, 
			_in(T) _Zfar);
		
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 



	template<typename T>
	struct MatrixTransform_ 
	{
		MatrixTransform_();
		MatrixTransform_(_in(Mat_<4, 4, T>) _Matrix);

		Normal_<T>&  transform(_inout(Normal_<T>) _Normal) const;
		Vector3_<T>& transform(_inout(Vector3_<T>) _Vector) const;
		Normal_<T>   transform_copy(_in(Normal_<T>) _Normal) const;
		Vector3_<T>  transform_copy(_in(Vector3_<T>) _Vector) const;

		Normal_<T>&  inv_transform(_inout(Normal_<T>) _Normal) const;
		Vector3_<T>& inv_transform(_inout(Vector3_<T>) _Vector) const;
		Normal_<T>   inv_transform_copy(_in(Normal_<T>) _Normal) const;
		Vector3_<T>  inv_transform_copy(_in(Vector3_<T>) _Vector) const;

		Mat_<4, 4, T> _Mymat;
		Mat_<4, 4, T> _Myinvmat;
	};
	



	/*
	@_Name: Transform
	@_interface: to_matrix(), transform(=), transform_copy(&)
	*/

#if defined(clmagic_using_SIMDMath)
	template<typename T>
		struct _declspec(align(16)) Quaternion_;/* forward definition */
#else
	template<typename T>
		struct Quaternion_;/* forward definition */
#endif


	/* @_S: scale, scaling */
	template<typename T = real_t>
	struct ScaleTransform_ {
		ScaleTransform_( _in(Vector3_<T>) _Vec = Vector3_<T>(T(1)) );
		ScaleTransform_(T _sX, T _sY, T _sZ);
		
		Mat_<4, 4, T> to_matrix() const;
		Vector3_<T>&  transform(_inout(Vector3_<T>) _Input) const;
		Vector3_<T>   transform_copy(_in(Vector3_<T>) _Input) const;

		ScaleTransform_& operator+=(_in(ScaleTransform_) _B) { _Mydata += _B._Mydata; return (*this); }
		ScaleTransform_& operator-=(_in(ScaleTransform_) _B) { _Mydata -= _B._Mydata; return (*this); }
		ScaleTransform_& operator*=(_in(T)				 _B) { _Mydata *= _B; return (*this); }
		ScaleTransform_& operator/=(_in(T)				 _B) { _Mydata /= _B; return (*this); }
		friend ScaleTransform_ operator+(_in(ScaleTransform_) _A, _in(ScaleTransform_) _B) { return (_A._Mydata + _B._Mydata); }
		friend ScaleTransform_ operator-(_in(ScaleTransform_) _A, _in(ScaleTransform_) _B) { return (_A._Mydata - _B._Mydata); }
		friend ScaleTransform_ operator*(_in(ScaleTransform_) _A, _in(T) _B) { return (_A._Mydata * _B); }
		friend ScaleTransform_ operator/(_in(ScaleTransform_) _A, _in(T) _B) { return (_A._Mydata / _B); }
		friend ScaleTransform_ operator*(_in(T) _A, _in(ScaleTransform_) _B) { return (_A * _B._Mydata); }
		friend ScaleTransform_ operator/(_in(T) _A, _in(ScaleTransform_) _B) { return (_A / _B._Mydata); }
			
		Vector3_<T> _Mydata;
	};


	/* @_Q: rotate */
	template<typename T = real_t>
	struct RotateTransform_ {
		RotateTransform_(_in(Quaternion_<T>) _Quat = Quaternion_<T>());
		RotateTransform_(_in(Vector3_<T>) _Axis, T _Radians);
		RotateTransform_(_in(Vector3_<T>) _From, _in(Vector3_<T>) _To);

		Mat_<4, 4, T> to_matrix() const;
		Vector3_<T>&  transform(_inout(Vector3_<T>) _Input) const;
		Vector3_<T>   transform_copy(_in(Vector3_<T>) _Input) const;

		/* unsupport scalar mul div */
		RotateTransform_& operator+=(_in(RotateTransform_) _B) { _Mydata *= _B._Mydata; return (*this); }
		RotateTransform_& operator-=(_in(RotateTransform_) _B) { _Mydata /= _B._Mydata; return (*this); }
		friend RotateTransform_ operator+(_in(RotateTransform_) _A, _in(RotateTransform_) _B) { return (_A._Mydata * _B._Mydata); }
		friend RotateTransform_ operator-(_in(RotateTransform_) _A, _in(RotateTransform_) _B) { return (_A._Mydata / _B._Mydata); }

		Quaternion_<T> _Mydata;
	};


	/* @_T: translate, move */
	template<typename T = real_t>
	struct TranslateTransform_ {
		TranslateTransform_(_in(Vector3_<T>) _Vec = Vector3_<T>());
		TranslateTransform_(T _X, T _Y, T _Z);
			
		Mat_<4, 4, T> to_matrix() const;
		Vector3_<T>&  transform(_inout(Vector3_<T>) _Input) const;
		Vector3_<T>   transform_copy(_in(Vector3_<T>) _Input) const;

		TranslateTransform_& operator+=(_in(TranslateTransform_) _B) { _Mydata += _B._Mydata; return (*this); }
		TranslateTransform_& operator-=(_in(TranslateTransform_) _B) { _Mydata -= _B._Mydata; return (*this); }
		TranslateTransform_& operator*=(_in(T)					 _B) { _Mydata *= _B; return (*this); }
		TranslateTransform_& operator/=(_in(T)					 _B) { _Mydata /= _B; return (*this); }
		friend TranslateTransform_ operator+(_in(TranslateTransform_) _A, _in(TranslateTransform_) _B) { return (_A._Mydata + _B._Mydata); }
		friend TranslateTransform_ operator-(_in(TranslateTransform_) _A, _in(TranslateTransform_) _B) { return (_A._Mydata - _B._Mydata); }
		friend TranslateTransform_ operator*(_in(TranslateTransform_) _A, _in(T) _B) { return (_A._Mydata * _B); }
		friend TranslateTransform_ operator/(_in(TranslateTransform_) _A, _in(T) _B) { return (_A._Mydata / _B); }
		friend TranslateTransform_ operator*(_in(T) _A, _in(TranslateTransform_) _B) { return (_A * _B._Mydata); }
		friend TranslateTransform_ operator/(_in(T) _A, _in(TranslateTransform_) _B) { return (_A / _B._Mydata); }

		Vector3_<T> _Mydata;
	};


	/* @_SQT: scale and rotate and translate */
	template<typename T = real_t>
	struct SQTTransform_ {
		SQTTransform_();
		SQTTransform_(_in(ScaleTransform_<T>) _Scale);
		SQTTransform_(_in(RotateTransform_<T>) _Scale);
		SQTTransform_(_in(TranslateTransform_<T>) _Scale);
		SQTTransform_(_in(ScaleTransform_<T>) _Scale, _in(RotateTransform_<T>) _Rotate, _in(TranslateTransform_<T>) _Translate);
		explicit SQTTransform_(_in(Mat4) _Matrix);

		Mat_<4, 4, T> to_matrix( ) const;
		Mat_<3, 4, T> to_matrix3x4() const;
		Vector3_<T>&  transform(_inout(Vector3_<T>) _Input) const;
		Vector3_<T>   transform_copy(_in(Vector3_<T>) _Input) const;

		SQTTransform_& operator+=(_in(SQTTransform_) _B) { _Myscl += _B._Myscl; _Myrot += _B._Myrot; _Mymov += _B._Mymov; return (*this); }
		SQTTransform_& operator-=(_in(SQTTransform_) _B) { _Myscl -= _B._Myscl; _Myrot -= _B._Myrot; _Mymov -= _B._Mymov; return (*this); }
		SQTTransform_& operator*=(_in(T)             _B) { _Myscl *= _B; _Mymov *= _B; return (*this); }
		SQTTransform_& operator/=(_in(T)             _B) { _Myscl /= _B; _Mymov /= _B; return (*this); }
		friend SQTTransform_ operator+(_in(SQTTransform_) _A, _in(SQTTransform_) _B) { return SQTTransform_(_A._Myscl + _B._Myscl, _A._Myrot + _B._Myrot, _A._Mymov + _B._Mymov); }
		friend SQTTransform_ operator-(_in(SQTTransform_) _A, _in(SQTTransform_) _B) { return SQTTransform_(_A._Myscl - _B._Myscl, _A._Myrot - _B._Myrot, _A._Mymov - _B._Mymov); }
		friend SQTTransform_ operator*(_in(SQTTransform_) _A, _in(T) _B) { return SQTTransform_(_A._Myscl * _B, _A._Myrot, _A._Mymov * _B); }
		friend SQTTransform_ operator/(_in(SQTTransform_) _A, _in(T) _B) { return SQTTransform_(_A._Myscl / _B, _A._Myrot, _A._Mymov / _B); }
		friend SQTTransform_ operator*(_in(T) _A, _in(SQTTransform_) _B) { return SQTTransform_(_A * _B._Myscl, _A._Myrot, _A * _B._Mymov); }
		friend SQTTransform_ operator/(_in(T) _A, _in(SQTTransform_) _B) { return SQTTransform_(_A / _B._Myscl, _A._Myrot, _A * _B._Mymov); }

		ScaleTransform_<T>     _Myscl;/*S*/
		RotateTransform_<T>    _Myrot;/*Q*/
		TranslateTransform_<T> _Mymov;/*T*/
	};
	

	/*
	@_tangent_btangent_normal
	@_tangent_space
	*/
	//struct tbn
	//{
	//	tbn(const Vec3& _P0, const Vec3& _P1, const Vec3& _P2,
	//		const Vec2& _Texcoord0, const Vec2& _Texcoord1, const Vec2& _Texcoord2);

	//	Vec3 tangent;
	//	Vec3 btangent;
	//	Vec3 _Mynormal;
	//};



	/* < following: is object [in] the region [with] crosspoint > */
	template<typename _Ty>
		bool intersect(
			_in(Vector2_<_Ty>) _Pos,
			/*in*/_Ty	    _Left,
			/*in*/_Ty		_Right,
			/*in*/_Ty		_Bottom,
			/*in*/_Ty		_Top);

	/*
	@_Return no hit if( 0 )
	*/
	template<typename T> 
		int intersect(
			_in(Vector3_<T>) _Pos,
			_in(Vector3_<T>) _V0,
			_in(Vector3_<T>) _V1,
			_in(Vector3_<T>) _V2);

	template<typename T> 
		bool intersect(
			_in(Vector3_<T>) _Origin,
			_in(Vector3_<T>) _Dir,
			_in(Vector3_<T>) _V0,
			_in(Vector3_<T>) _V1,
			_in(Vector3_<T>) _V2,
			_out(Vector3_<T>) _Point);

	/* </ following: is object [in] the region [with] crosspoint > */
}// namespace clmagic

#endif