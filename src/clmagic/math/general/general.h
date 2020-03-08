//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_GENERAL_h_
#define clmagic_math_GENERAL_h_
#include <array>
#include <cmath>
#include <bitset>
#include <string>
#include <vector>
#include <iostream>
#include <valarray>
#include <algorithm>
#include <functional>
#include <initializer_list>

namespace clmagic {
	template <typename... _Types>
	struct _Arg_types : public std::bool_constant<true> /* default true */ {
	};

	template <>
	struct _Arg_types<> : public std::bool_constant<false> {
		/* empty argument list, containers only void argument */
	};

	//template <>
	//struct _Arg_types<void> : public std::bool_constant<false> {
	//	using first_argument_type = void;/* argument type: void */
	//};

	template<typename _Ty1>
	struct _Arg_types<_Ty1> : public std::bool_constant<true> {
		using first_argument_type = _Ty1;
	};

	template<typename _Ty1, typename _Ty2>
	struct _Arg_types<_Ty1, _Ty2> : public std::bool_constant<true> {
		using first_argument_type = _Ty1;
		using second_argument_type = _Ty2;
	};

	template<typename _Ty1, typename _Ty2, typename _Ty3>
	struct _Arg_types<_Ty1, _Ty2, _Ty3> : public std::bool_constant<true> {
		using first_argument_type = _Ty1;
		using second_argument_type = _Ty2;
		using third_argument_type = _Ty3;
	};

	template<typename _Fty>
	struct has_arguments : public _Arg_types<>/* default empty arguments list */ {
	};

#define _GET_FUNCTION_IMPL2(CALL_OPT, X1, X2, X3) \
	template<typename _Ret, typename... _Types>   \
	struct has_arguments<_Ret CALL_OPT(_Types...)> : public _Arg_types<_Types...> { \
	}; 

	_NON_MEMBER_CALL(_GET_FUNCTION_IMPL2, X1, X2, X3)
#undef _GET_FUNCTION_IMPL2

	template <typename _Ret, typename... _Types>
	struct has_arguments<_Ret (__cdecl *)(_Types...)> : public _Arg_types<_Types...> {
	};

	template <typename _Ret, typename _Cla, typename... _Types>
	struct has_arguments<_Ret(_Cla::*)(_Types...)> : public _Arg_types<_Types...> {
	};

	/* don't used to lambda express */
	template<typename _Fty>
		constexpr bool has_arguments_v = has_arguments<_Fty>::value;

	
	template<typename _Ty>
	struct const_value {

		static _Ty One() {
			return static_cast<_Ty>(1);
		}
		
		static _Ty Zero() {
			return static_cast<_Ty>(0);
		}

		template<intmax_t _An = 1, intmax_t _Bn = 0, intmax_t _Ad = 1, intmax_t _Bd = 1>
		static _Ty Pi() {
			return (static_cast<_Ty>(_An) / static_cast<_Ty>(_Ad) * static_cast<_Ty>(3.14159) 
				+ static_cast<_Ty>(_Bn) / static_cast<_Ty>(_Bd));
		}

		//constexpr static auto Pi = static_cast<_Ty>(3.14159);
		//constexpr static auto e = static_cast<_Ty>(2.71828);
	};

	


	template<typename _Ty, bool = std::is_floating_point_v<_Ty>>
	struct _Abs {
		static _Ty process(const _Ty& _Left) {// default false, false
			return (_Left < 0 ? (-_Left) : _Left);
		}
	};

	template<typename _Ty>
	struct _Abs<_Ty, true> {// IEEE754 signed floating point
		static _Ty process(const _Ty& _Left) {
			constexpr size_t _Bits = sizeof(_Ty) * 8;
			using _BitTy = std::bitset<_Bits>;
			auto       _Bitval = *reinterpret_cast<const _BitTy*>(&_Left);
			const auto _Mask   = ~(_BitTy(1) << (_Bits - 1));
			_Bitval &= _Mask;
			return (*reinterpret_cast<_Ty*>(&_Bitval));
		}
	};

	// error C4146: unary minus operator applied to unsigned type...
	template<typename _Ty> inline
	_Ty abs(const _Ty& _Left) { return (_Abs<_Ty>::process(_Left)); }


	/*
	@_Feature: { a, a*q^1, a*q^2, a*q^3, ..., a*q^(n-1) }
	@_Sum: Sn = (a - a*q^n) / (1-q)

	①:   Sn = a + a*q + a*q^2 + a*q^3 + ... + a*q^(n-1)
	②: q*Sn = a*q + a*q^2 + a*q^3 + a*q^4 + ... + a*q^n
	② - ①: (q-1)*Sn = -a + a*q^n
						-a + a*q^n     a(-1 + q^n)
				   Sn = ----------- = -----------
						  q - 1          q - 1

					 a(-1+0)       a
	lim(n->∞) Sn = --------- = -------, (q < 1)
					  q - 1      1 - q

	0.33333333...3
	{ 0.3 + 0.3*0.1^1 + 0.3*0.1^2 + ... + 0.3*0.1^(n-1) }, a = 0.3, q = 0.1
			0.3      0.3      1
	Sn = -------- = ----- = -----
		  1 - 0.1    0.9      3
	*/
	template<typename _Ty>
	struct geometric_sequence {
		_Ty a;
		_Ty q;

		geometric_sequence() = default;
		
		geometric_sequence(const _Ty& _Val0, const _Ty& _Ratio) : a(_Val0), q(_Ratio) { }

		_Ty operator[](size_t i) const {
			return (a * pow(q, (_Ty)i));
		}

		_Ty sum(size_t n) {// Sn = (a - a*q^n) / (1-q)
			return (   (a - (*this)[n]) 
				/*--------------------------*/
				 / (static_cast<_Ty>(1) - q)   );
		}

		_Ty sum() const {// Sn = a(1-0)/(1-q)
			assert( q < static_cast<_Ty>(1) );
			return ( a / (static_cast<_Ty>(1) - q) );
		}
	};

	template<typename _Ty>
	struct arithmetic_sequence {
		_Ty a;
		_Ty d;

		arithmetic_sequence() = default;

		arithmetic_sequence(const _Ty & _Val0, const _Ty & _Difference) : a(_Val0), d(_Difference) { }

		_Ty operator[](size_t i) const {
			return (a + static_cast<_Ty>(i) * d);
		}

		_Ty sum(size_t n) const {
			return (static_cast<_Ty>(n) * a + static_cast<_Ty>(n * (n - 1)) * d 
											/*---------------------------------*/
											               / 2 );
		}
	};


	template<typename _Ty> constexpr
	const _Ty& constexpr_maxval(const _Ty& _Lhs, const _Ty& _Rhs)
		{	// return max value from 2 parameters
		return (_Lhs < _Rhs ? _Rhs : _Lhs);
		}

	template<typename _Ty> constexpr
	const _Ty& constexpr_maxval(const _Ty& _Val1, const _Ty& _Val2, const _Ty& _Val3)
		{	// return max value from 3 parameters
		return ( _Val1 < _Val2 ? constexpr_maxval(_Val2, _Val3) : constexpr_maxval(_Val1, _Val3) );
		}

	template<typename _Ty> constexpr
	const _Ty& constexpr_minval(const _Ty& _Lhs, const _Ty& _Rhs)
		{	// return min value from 2 parameters
		return (_Lhs < _Rhs ? _Lhs : _Rhs);
		}

	template<typename _Ty> constexpr
	const _Ty& constexpr_minval(const _Ty& _Val1, const _Ty& _Val2, const _Ty& _Val3)
		{	// return min value from 3 parameters
		return ( _Val1 < _Val2 ? constexpr_minval(_Val1, _Val3) : constexpr_minval(_Val2, _Val3) );
		}

	template<typename _Ty> inline
	const _Ty& constexpr_clamp(const _Ty& _Lhs, const _Ty& _Lowval, const _Ty& _Upval)
		{	// return _Lhs clamp to [_Lowval, _Upval]
		return (constexpr_minval(constexpr_maxval(_Lhs, _Lowval), _Upval));
		}

	template<typename _Ty> inline
	void sort( _Ty& _Val1, _Ty& _Val2 ) 
		{
		if (_Val2 < _Val1) std::swap( _Val2, _Val1 );
		}

	template<typename _Ty> inline
	void sort(_Ty& _Val1, _Ty& _Val2, _Ty& _Val3) 
		{
		if (_Val3 < _Val2) std::swap(_Val3, _Val2);
		if (_Val2 < _Val1) std::swap(_Val2, _Val1);
		if (_Val3 < _Val2) std::swap(_Val3, _Val2);
		}

	template<typename _Ty1, typename _Pr, typename _Fn, typename ..._Tys>
	auto product(_Ty1 _First, _Pr _Pred, _Ty1 _Last, _Fn _Func, _Tys... _Args) {
		using _Ty2 = decltype(_Func(_First, _Args...));
		if ( _Pred(_First, _Last) ) {
			auto s = _Func(_First, _Args...); 
			++_First;
			for (; _Pred(_First, _Last); ++_First) {
				s += _Func(_First, _Args...);
			}

			return (s);
		}

		return _Ty2();
	}

	template<typename _Ty1, typename _Pr, typename _Fn>
	auto product_narg(_Ty1 _First, _Pr _Pred, _Ty1 _Last, _Fn _Func) {
		using _Ty2 = decltype( _Func() );
		if ( _Pred(_First, _Last) ) {
			auto s = _Func();
			++_First;
			for (; _Pred(_First, _Last); ++_First) {
				s += _Func();
			}

			return (s);
		}

		return _Ty2();
	}



	template<typename _Ty>
	std::vector<_Ty> set_number(_in(_Ty) _First, _in(_Ty) _Last) {	// return [_First, _Last)
		auto _Vec = std::vector<_Ty>(_Last - _First);
		for (_Ty i = _First; i < _Last; ++i) {
			_Vec[i] = i;
		}
		return (_Vec);
	}

	template<typename _Ty = unsigned long long>
	_Ty factorial(_Ty n, _Ty i = _Ty(1))
		{	// ∏[i,n]: i
		return (i < n ? (i * factorial(n, i + 1)) : n);
		}

	template<typename _Ty>
	_Ty Stirling(_Ty n) 
		{	// √(2πn) * (n/e)^n
		return (static_cast<_Ty>(sqrt(6.283 * n) * pow(n / 2.718, n)));
		}

	template<typename _Ty = size_t>
	_Ty binomial_coefficient(size_t n, size_t i) 
		{
		static auto _Lookup_table = std::vector<unsigned long long>{
			1,
			1, 1,
			1, 2, 1,
			1, 3, 3, 1,
			1, 4, 6, 4, 1,
			1, 5, 10, 10, 5, 1
		};

		static auto _Lookup_index = std::vector<size_t>{
			0,  /* 1 elements */
			1,  /* 2 elements */
			3,  /* 3 elements */
			6,  /* 4 elements */
			10, /* 5 elements */
			15  /* n + 1 elements */
		};

		if (n < _Lookup_index.size()) 
			{	// exist lookup table
			return (static_cast<_Ty>(_Lookup_table[_Lookup_index[n] + i]));
			} 
		else 
			{	// compute lookup table
			for (size_t k = _Lookup_index.size(); k <= n; ++k) 
				{
				_Lookup_index.push_back(_Lookup_table.size());      /* insert index */
				_Lookup_table.insert(_Lookup_table.end(), k + 1, 1);/* insert n+1 elemtns into end */

				unsigned long long* _Prev_ptr = &_Lookup_table[_Lookup_index[k - 1]];
				unsigned long long* _Curt_ptr = &_Lookup_table[_Lookup_index[k]];
				_Curt_ptr += 1; /* first element is 1 */
				do {
					*_Curt_ptr = *_Prev_ptr + *(_Prev_ptr + 1);
					++_Prev_ptr; ++_Curt_ptr;
					}
				while (*_Prev_ptr != 1);
				}
			return (static_cast<_Ty>(_Lookup_table[_Lookup_index[n] + i]));
			}
		}

	template<typename _Ty = size_t>
	_Ty C(size_t n, size_t i) 
		{
		return (binomial_coefficient<_Ty>(n, i));
		/*
					n-i+1   n-(i-1)+1		  n-1+1
		C(n, i) = ------ * --------- *.... * -------
						i		  i-1				1
		
		C(n, 0) = C(n, n) = 1
		
		i<n
		ΣC(n, i) = pow(2, n)
		i=0
		*/
		}

	template<typename _Ty>
	_Ty binomial(_in(_Ty) _A, _in(_Ty) _B, size_t n, size_t i) {
		/*	(A + B)^n
			n=2: A² + 2AB + B²
			n=3: A³ + 3A²B + 3AB² + B³
			n=4: A⁴ + 4A³B + 6A²B² + 4AB³ + B⁴         (A⁴ + 3A³B + 3A²B² + AB³ + A³B + 3A²B² + 3AB³ + B⁴)
			n=5: A⁵ + 5A⁴B + 10A³B + 10AB³ + 5AB⁴ + B⁵  (A⁵ + 4A⁴B + 6A³B² + 4A²B³ + AB⁴ + A⁴B + 4A³B² + 6A²B³ + 4AB⁴ + B⁵)
		*/
		if (i == 0) {
			return ((_Ty)pow(_A, n));
		} else if (i != n) {
			return (C<_Ty>(n, i) * (_Ty)pow(_A, n - i) * (_Ty)pow(_B, i));
		} else {
			return ((_Ty)pow(_B, n));
		}
	}


	template<typename _Ty>
	struct range_ {
		_Ty _Mymin, _Mymax;

		range_() { /* empty */ }
		range_(_in(_Ty) _Minval, _in(_Ty) _Maxval) : _Mymin(_Minval), _Mymax(_Maxval) { }

		bool exclude(_in(_Ty) _Val) const {
			return ( _Val < _Mymin || _Val > _Mymax );
		}

		bool include(_in(_Ty) _Val) const {
			return ( !this->exclude(_Val) );
		}

		template<typename _Fn1, typename _Fn2>
		bool exclude(_in(_Ty) _Val, _Fn1 _Func1, _Fn2 _Func2) const {
			return ( _Val < _Func1(_Mymin) || _Val > _Func2(_Mymax) );
		}

		template<typename _Fn1, typename _Fn2>
		bool include(_in(_Ty) _Val, _Fn1 _Func1, _Fn2 _Func2) const {
			return ( !this->exclude(_Val, _Func1, _Func2) );
		}

		_Ty range() const {
			return (_Mymax - _Mymin);
		}

		_Ty remap(_in(_Ty) _Val) const {
			return ( (_Val - _Mymin) / range() );
		}
	};


	template<typename _Ty, typename _Xy>
	void test_function(_Ty _Initial, _Ty _End, _Ty _Increment, _Xy _Func, const std::string& _Funcname) {
		_Ty _Var = _Initial;

		while (_Var <= _End) {
			std::cout << _Funcname << "(" << _Var << ") = " << _Func(_Var) << '\n';
			_Var += _Increment;
		}
		std::cout << _Funcname << " test end" << std::endl;
	}

	//template<typename _Ty>
	//_Ty abs(_in(_Ty) _Src)
	//	{	// @_Require: ( constructor(base type) ) and ( operator<(rhs) ) and ( operator-() )
	//	return (_Src < _Ty(0) ? -_Src : _Src); 
	//	}

	template<typename _Ty>
	const _Ty& maxval(const _Ty& _Lhs, const _Ty& _Rhs)
		{	// @_Require: ( operator<(rhs) )
		return (constexpr_maxval(_Lhs, _Rhs));
		}

	template<typename _Ty>
	const _Ty& maxval(const _Ty& _V0, const _Ty& _V1, const _Ty& _V2)
		{	// @_Require: ( operator<(rhs) )
		return (constexpr_maxval(_V0, _V1, _V2));
		}

	template<typename _Ty>
	const _Ty& minval(const _Ty& _Lhs, const _Ty& _Rhs)
		{	// @_Require: ( operator<(rhs) )
		return ( constexpr_minval(_Lhs, _Rhs) );
		}

	template<typename _Ty>
	const _Ty& minval(const _Ty& _V0, const _Ty& _V1, const _Ty& _V2)
		{	// @_Require: ( operator<(rhs) )
		return ( constexpr_minval(_V0, _V1, _V2) );
		}

	template<typename _Ty, typename _Ty2> 
	bool equal(_in(_Ty) _Lhs, _in(_Ty) _Rhs, _in(_Ty2) _Thresould)
		{	// @_Require: ( @abs ) and ( operator-(rhs) ) and ( operator<=(rhs) )
		return ( abs(_Lhs - _Rhs) <= _Thresould );
		}

	template<typename _Ty>
	const _Ty& clamp(_in(_Ty) _Lhs, _in(_Ty) _Lowval, _in(_Ty) _Upval)
		{	// return _Lhs clamp to [_Lowval, _Upval]
		return ( minval( maxval(_Lhs, _Lowval), _Upval ) );
		}

	template<typename _Ty>
	_Ty saturate(_in(_Ty) _Val) 
		{	// return clamp(_Val, 0, 1)
		return clamp( _Val, static_cast<_Ty>(0), static_cast<_Ty>(1) );
		}

	/*template<typename _Ty>
	_Ty sign(_in(_Ty) _Val) 
		{
		return (_Val > _Ty(0) ? _Ty(1) 
			: _Val == _Ty(0) ? _Ty(0) 
			: _Ty(-1));
		}*/

	template<typename _Ty>
	_Ty fract(_in(_Ty) _Val) 
		{	// @_Require: ( @floor ) and ( operator-(rhs) )
		return ( _Val - floor(_Val) );
		}

	template<typename _Ty, typename _Tyfactor>
	_Ty lerp(_in(_Ty) _A, _in(_Ty) _B, _in(_Tyfactor) t)
		{	// @_Equaltion: (1-t) * A + t * B
		return ( _A + (_B - _A) * t );
		}

	template<typename _Ty>
	const _Ty& lerp(_in(_Ty) _A, _in(_Ty) _B, /*in*/bool _Isright)
		{	// [0, 1] map to [A, B]
		return (_Isright ? _B : _A);
		}

	template<typename _Ty, typename _Tyfactor>
	_Ty mix(_in(_Ty) _A, _in(_Ty) _B, _in(_Tyfactor) t)
		{	// (1-t)*A + t*B
		return ( lerp(_A, _B, t) );
		}

	/* t0 = [-B-√(B^2-4AC)]/2A, 
	   t1 = [-B+√(B^2-4AC)]/2A 
	*/
	template<typename _Ty> 
	bool quadratic(_in(_Ty) A, _in(_Ty) B, _in(_Ty) C, _out(_Ty) t0, _out(_Ty) t1) {// return false if(B^2-4AC < 0)
		/*  A*t^2 + B*t + C = 0

					B*t    C 
			t^2 + --- + --- = 0
					A     A

					B         C
			t^2 + ---*t = - ---
					A         A
		  
					B        B           C      B
			t^2 + ---*t + (----)^2 = - --- + (----)^2
					A        2A          A      2A

					B			  C*4A    B*B     
			(t + ----)^2 = - ------ -------- 
					2A		  A*4A    4A^2

            		B			B^2 - 4AC         
			(t + ----)^2 = -----------
					2A		  4A^2
            
					B		     B^2 - 4AC
			t + ---- = ±√(-----------)
					2A	  	       2A*2A

					B		  √(B^2 - 4AC)
			t + ---- = ±---------------
					2A	  	        2A
					-B ±√(B^2 - 4AC)
			t = --------------------
						2A
		*/
		using T = scalar_type<_Ty>;

		auto discrim = B * B - static_cast<T>(4) * A * C;
		if ( discrim < static_cast<_Ty>(0) ) {
			return ( false );
		} else {
			discrim = sqrt(discrim);
			auto q = ( B < static_cast<_Ty>(0) ) 
				? static_cast<T>(-0.5) * (B - discrim)
				: static_cast<T>(-0.5) * (B + discrim);
			t0 = q / A;
			t1 = C / q;
			
			return ( true );
		}
	}

	/* t0 = 0
	   t1 = -B/A
	*/
	template<typename _Ty>
	bool quadratic(_in(_Ty) A, _in(_Ty) B, _out(_Ty) t0, _out(_Ty) t1) {
		/* A*x^2 + B*x = 0
			x*(A*x + B) = 0
			t0 = 0
			t1 = (A*x+B=0) = -B/A
		*/
		t0 = _Ty(0);
		t1 = -B / A;
		return ( true );
	}

	/* 
	x^2 + y^2 + z^2 - r^2 = 0 
	            *     /
		     ,, | ,, /
	    ,,,o    |   / ,,,
	  ,,    \   y  /     ,,
	 ,       r  | /        ,
	,         \ |/          ,
	,-----------+----x------,--------->
	 ,         /|          ,
	  ,,      z |        ,,
	    ,,,  /  |     ,,,
		    /,, | ,, 
		   /    |
		     ,, , ,,     /
	    ,,,           ,,/t? > 0
	  ,,               o ,,
	 ,               !/    ,
	,                /      ,
	,               o       ,
	 ,             /       ,
	  ,,          /      ,,
	    ,,,      /    ,,,
		     ,, o ,, 
               /t? < 0
			  /
	direction: o->!	
	*/
	template<typename _Ty>
	bool sphere_equation(
		_in(_Ty) ox, _in(_Ty) oy, _in(_Ty) oz, 
		_in(_Ty) dx, _in(_Ty) dy, _in(_Ty) dz, 
		_in(_Ty) r, 
		_out(_Ty) t0, _out(_Ty) t1) 
		{	
		//_Ty A = dx*dx + dy*dy + dz*dz;
		_Ty A = _Ty(1);
		_Ty B = _Ty(2) * (dx*ox + dy*oy + dz*oz);
		_Ty C = ox*ox + oy*oy + oz*oz - r*r;
		return ( quadratic(A, B, C, t0, t1) );
		}

	template<typename _Ty>
	bool disk_equation(
		_in(_Ty) ox, _in(_Ty) oy, _in(_Ty) oz,
		_in(_Ty) dx, _in(_Ty) dy, _in(_Ty) dz, 
		_in(_Ty) inr, _in(_Ty) outr, 
		_out(_Ty) t) 
		{	// o + td = 0
		t = -oz / dz;
		_Ty mu2 = pow(ox + t * dx, 2) + pow(oy + t * dy, 2);
		if (mu2 < pow(inr, 2) || mu2 > pow(outr, 2)) 
			return (false);

		return ( true );
		}


	template<typename _Ty> inline
	_Ty s_curve(_in(_Ty) t) {	// S-Curve
		using T = scalar_type<_Ty>;
		return ( t * t * (static_cast<T>(3) - static_cast<T>(2)* t) );
		/* ①3*t^2 - 2*t^3
		   ②t^2*(3 - 2*t)
		*/
	}

	/* @_Equaltion: 6*t - 6*t^2 */
	template<typename _Ty> inline
	_Ty s_curved(_in(_Ty) t) {	
		using T = scalar_type<_Ty>;
		return ( static_cast<T>(6) * (t - t * t) );
		// ①6t - 6t^2 ②6(t-t^2)
	}

	template<typename _Ty> inline
	_Ty fade(_in(_Ty) t) {	// quintic interpolation curve
		using T = scalar_type<_Ty>;
		return ( t * t * t * ( t * ( t * static_cast<T>(6) - static_cast<T>(15)) + static_cast<T>(10) ) );
		/* ①6*t^5 - 15*t^4 + 10*t^3
		   ②t^3*(6t^2 - 15t + 10)
		   ③t^3*(6t^2 - 15t + 10)
		*/
	}

	template<typename _Ty> inline
	_Ty faded(_in(_Ty) t) {	// d(fade(t))
		using T = scalar_type<_Ty>;
		return ( static_cast<T>(30) * t * t * ( t * ( t - static_cast<T>(2) ) + static_cast<T>(1) ) );
		/* ① d¹(fade)
		   ②5*6*t^4 - 4*15*t^3 + 3*10*t^2
		   ③30t^4 - 60t^3 + 30t^2
		   ④30t^2*(t^2 - 2t + 1)
		   ⑤30t^2*(t*(t-2) + 1)
		*/
	}

	template<typename _Ty> inline
	_Ty taylor_invsqrt(_in(_Ty) r) {	// 1.79284291400159 - 0.85373472095314 * r
		using T = scalar_type<_Ty>;
		return (static_cast<T>(1.79284291400159) - static_cast<T>(0.85373472095314) * r);
	}
	
	template<typename _Ty> inline
	_Ty step(_in(_Ty) _Edge, _in(_Ty) _X)
		{	// 
		return ( lerp(_Ty(1), _Ty(0), _X < _Edge) );
		}
	
	/* x map to [a,b] */
	template<typename _Ty> inline
	_Ty smoothstep(_in(_Ty) a, _in(_Ty) b, _in(_Ty) x)
		{	// @_Equaltion: (x-a)/(b-a)
		return ( s_curve( (x - a) / (b - a) ) );
		}

	template<typename _Ty> inline
	_Ty remap(_in(_Ty) _Oldval, _in(_Ty) _Oldmin, _in(_Ty) _Oldmax, _in(_Ty) _Newmin, _in(_Ty) _Newmax)
		{	// _Oldval map to [_Newmin,_Newmax]
		return (_Newmin + (_Oldval - _Oldmin) / (_Oldmax - _Oldmin) * (_Newmax - _Newmin));
		}

	template<typename _Ty> inline 
	_Ty symmetry(_in(_Ty) _Val, _in(_Ty) _Region0, _in(_Ty) _Region1)
		{
		/*
					|----------|-------|----_Dis--|
		 -----_Return-----------_Region----------_Val------
		*/

		if (_Region0 > _Region1)
			std::swap(_Region0, _Region1);

		if (_Val < _Region0)
			return (_Region1 + (_Region0 - _Val));
		if (_Val > _Region1)
			return (_Region0 - (_Val - _Region1));

		return (_Val);
		}
		
	template<typename _Ty, typename _Tyn, typename _Tyc> inline 
	_Ty Bernstein(/*in*/_Tyn n, /*in*/_Tyn i, _in(_Ty) t, /*in*/_Tyc C)
		{	// @_Equaltion: C * (t^i) * [(1-t)^(n-i)]
		static_assert(std::is_integral<_Tyn>, "invalid template argument, Bernstein(_Tyn, _Tyn, _in(_Ty))");
		return ( _Ty(C) * pow(t, i) * pow(static_cast<_Ty>(1) - t, n - i) );
		}

	template<typename _Ty, typename _Tyn> inline
	_Ty Bernstein(/*in*/_Tyn n, /*in*/_Tyn i, _in(_Ty) t)
		{	// @_Equaltion: C * (t^i) * [(1-t)^(n-i)]
		static_assert(std::is_integral<_Tyn>, "invalid template argument, Bernstein(_Tyn, _Tyn, _in(_Ty))");
		return ( Bernstein( n, i, t, binomial_coefficient<_Tyn>(n, i) ) );
		}

	inline void insert(/*inout*/void* _Dst, size_t _Stride,
		/*in*/const void* _Src, size_t _Cpysize, size_t _Stride2, size_t _Count)
		{
		char* _Pdst = static_cast<char*>(_Dst);
		const char* _Psrc = static_cast<const char*>(_Src);

		_Cpysize = minval(_Cpysize, _Stride, _Stride2);
		for (size_t i = 0; i < _Count; ++i)
			{
			std::move(_Psrc, _Psrc + _Cpysize, _Pdst);
			_Pdst += _Stride;
			_Psrc += _Stride2;
			}
		}

	inline void merge(/*inout*/void* _Dst, size_t _Stride,
		/*in*/const void** _Srcs, const size_t* _Copids, const size_t* _Strides, const size_t* _Counts, size_t _Size)
		{
		char* _Pdst = static_cast<char*>(_Dst);
		for (size_t i = 0; i != _Size; ++i)
			{
			insert(_Pdst, _Stride, _Srcs[i], _Copids[i], _Strides[i], _Counts[i]);
			_Pdst += _Copids[i];
			}
		}

	inline void split(/*inout*/void* _Dst,
		/*in*/const void* _Src, size_t _Off, size_t _Copid, size_t _Stride, size_t _Count)
		{
		insert(_Dst, _Copid, ((const char*)_Src) + _Off, _Copid, _Stride, _Count);
		}

	template<typename _Ty>
	_Ty Gaussian(_in(_Ty) x, _in(_Ty) mu, _in(_Ty) sigma) {
		using T = scalar_type<_Ty>;
		return (
			static_cast<T>(1) 
		 /*-----------------------*/ * exp((x - mu) / ((T)2 * sigma * sigma))
		 / (sigma * ::sqrt(6.283)) );
	}




	template<typename _Ty, size_t _Size>
	struct _Slice_iterator {
		using iterator_category = std::random_access_iterator_tag;
		using value_type        = std::remove_cv_t<_Ty>;
		using difference_type   = ptrdiff_t;
		using pointer           = _Ty*;
		using reference         = _Ty&;

		constexpr _Slice_iterator() : _Ptr(nullptr), _Stride(1) {}

		constexpr _Slice_iterator(pointer _Ptr, size_t _Off, size_t _Inc) : _Ptr(_Ptr + _Off), _Stride(_Inc) {}

		constexpr reference operator*() const {
			assert(_Ptr != nullptr);
			return (*_Ptr);
		}

		constexpr pointer operator->() const {
			assert(_Ptr != nullptr);
			return (_Ptr);
		}

		constexpr _Slice_iterator& operator++() {
			assert(_Ptr != nullptr);
			_Ptr += _Stride;
			return (*this);
		}

		constexpr _Slice_iterator operator++(int) {
			_Slice_iterator _Tmp = *this;
			++(*this);
			return _Tmp;
		}

		constexpr _Slice_iterator& operator--() {
			assert(_Ptr != nullptr);
			_Ptr -= _Stride;
			return (*this);
		}

		constexpr _Slice_iterator operator--(int) {
			_Slice_iterator _Tmp = *this;
			--(*this);
			return _Tmp;
		}

		constexpr _Slice_iterator& operator+=(const ptrdiff_t _Off) {
			assert(_Ptr != nullptr);
			_Ptr += (_Stride * _Off);
			return (*this);
		}

		constexpr _Slice_iterator operator+(const ptrdiff_t _Off) const {
			_Slice_iterator _Tmp = *this;
			return _Tmp += _Off;
		}

		constexpr _Slice_iterator& operator-=(const ptrdiff_t _Off) {
			return *this += -_Off;
		}

		constexpr _Slice_iterator operator-(const ptrdiff_t _Off) const {
			_Slice_iterator _Tmp = *this;
			return _Tmp -= _Off;
		}

		constexpr ptrdiff_t operator-(const _Slice_iterator& _Right) const {
			_Compat(_Right);
			return static_cast<ptrdiff_t>((_Ptr - _Right._Ptr) / _Stride);
		}

		constexpr reference operator[](const ptrdiff_t _Off) const {
			return _Ptr[_Off];
		}

		constexpr bool operator==(const _Slice_iterator& _Right) const {
			return (_Ptr == _Right._Ptr);
		}

		constexpr bool operator!=(const _Slice_iterator& _Right) const {
			return !(*this == _Right);
		}

		constexpr bool operator<(const _Slice_iterator& _Right) const {
			return (_Ptr < _Right._Ptr);
		}

		constexpr bool operator>(const _Slice_iterator& _Right) const {
			return (_Right < *this);
		}

		constexpr bool operator<=(const _Slice_iterator& _Right) const {
			return !(_Right < *this);
		}

		constexpr bool operator>=(const _Slice_iterator& _Right) const {
			return !(*this < _Right);
		}

		constexpr void _Compat(const _Slice_iterator& _Right) const { // test for compatible iterator pair
			_STL_VERIFY(_Stride == _Right._Stride
				&& (ptrdiff_t(_Ptr) - ptrdiff_t(_Right._Ptr)) % ptrdiff_t(_Stride) == 0, "array iterators incompatible");
		}

		//using _Prevent_inheriting_unwrap = _Slice_iterator;

		// this iterator don't Unwrappe
		/*constexpr pointer _Unwrapped() const {
			return _Ptr;
		}*/

		constexpr void _Seek_to(pointer _It) {
			_Ptr = _It;
		}

	private:
		_Ty*   _Ptr;
		size_t _Stride = 1; // the distance between elements
	};

	template<typename _Ty, size_t _Size>
		using _Slice_const_iterator = _Slice_iterator<const _Ty, _Size>;



	template<typename _Ty>
	class slice_array : public std::slice {
	public:
		using scalar_type = _Ty;

		slice_array() = default;

		slice_array(_Ty* _Ptr, size_t _Off, size_t _Count, size_t _Inc)
			: std::slice(_Off, _Count, _Inc), _Myptr(_Ptr) {
			//
		}

		_Ty& at(size_t _Pos) const {
			return (_Myptr[_Start + _Pos * _Stride]);
		}

		_Ty& operator[](size_t _Pos) const {
			return (_Myptr[_Start + _Pos * _Stride]);
		}

	private:
		_Ty* _Myptr;
	};


	template<typename _Ty>
	class vector_slice {
	public:
		vector_slice() = default;

		vector_slice(_Ty* _Ptr, size_t _Size)
			: _Myptr(_Ptr), _Mysize(_Size) {
			//
		}

		size_t size() const {
			return ( _Mysize );
		}

		_Ty& at(size_t _Pos) const {
			return ( _Myptr[_Pos] );
		}

		_Ty& operator[](size_t _Pos) const {
			return ( _Myptr[_Pos] );
		}

	protected:
		_Ty*   _Myptr;
		size_t _Mysize;
	};


	template<typename _Ty>
	class matrix_slice {
	public:
		matrix_slice() = default;

		matrix_slice(_Ty* _Ptr, size_t _Rows, size_t _Cols)
			: _Myptr(_Ptr), _Myrows(_Rows), _Mycols(_Cols) {
			//
		}

		size_t size() const {
			return (_Myrows * _Mycols);
		}

		size_t rows() const {
			return (_Myrows);
		}

		size_t cols() const {
			return (_Mycols);
		}

		_Ty& at(size_t r, size_t c) const {
			return ( _Myptr[r * _Mycols + c] );
		}

		_Ty* operator[](size_t r) const {
			return ( _Myptr + r * _Mycols );
		}

	protected:
		_Ty*   _Myptr;
		size_t _Myrows;
		size_t _Mycols;
	};
	

	struct sign {
		sign() : _Mydata(0) { }
	
		explicit sign(int a) : _Mydata((a > 0) ? 1 : (a < 0) ? -1 : 0) { }

		sign& operator=(const sign&) = default;
		
		sign& operator=(int a) { 
			_Mydata = (a > 0) ? 1 
				    : (a < 0) ? -1 
				    : 0; 
			return (*this); 
		}

		friend sign operator-(sign _Sign) {
			return ( sign(-_Sign._Mydata) );
		}

		template<typename _Ty>
		friend _Ty operator*(sign _Sign, const _Ty& _Number) {
			return (_Sign._Mydata == 0 ? _Ty()
				  : _Sign._Mydata == 1 ? _Number
				  : -_Number);
		}

		template<typename _Ty>
		friend _Ty operator*(const _Ty& _Number, sign _Sign) {
			return (_Sign * _Number);
		}

		char _Mydata = 0;
	};
}// namespace clmagic

#endif