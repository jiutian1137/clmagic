#pragma once
#ifndef clmagic_math_GENERAL_h_
#define clmagic_math_GENERAL_h_
#include "simd.h"
#include "real.h"
#include <array>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <initializer_list>

namespace clmagic
{
	// <general function>
	
	// < constexpr >
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

	constexpr float
		constexpr_trunc(float _Real)
		{
		return (static_cast<float>(static_cast<int>(_Real)));
		}

	constexpr double
		constexpr_trunc(double _Real)
		{
		return (constexpr_trunc(static_cast<float>(_Real)));
		}

	constexpr float
		constexpr_fract(float _Real)
		{
		return (_Real - constexpr_trunc(_Real));
		}

	constexpr double
		constexpr_fract(double _Real)
		{
		return (_Real - constexpr_trunc(_Real));
		}

	constexpr float
		constexpr_floor(float _Real) 
		{
		return ( static_cast<float>(static_cast<int>(_Real)) );
		}

	constexpr double
		constexpr_floor(double _Real)
		{
		return ( static_cast<double>(static_cast<int>(_Real)) );
		}

	constexpr int
		constexpr_mod(int _Real, int _Mod)
		{	// _Real % _Mod
		return (_Real % _Mod);
		}

	constexpr float
		constexpr_mod(float _Real, float _Mod)
		{	// _Real % _Mod
		return (_Real - constexpr_floor(_Real / _Mod) * _Mod);
		}

	constexpr double
		constexpr_mod(double _Real, double _Mod)
		{	// _Real % _Mod
		return (_Real - constexpr_floor(_Real / _Mod) * _Mod);
		}
	// </ constexpr >
	
	inline float trunc(float _Real)
	{ return (std::trunc(_Real)); }

	inline double trunc(double _Real)
	{ return (std::trunc(_Real)); }

	inline int floor(int _Real)
	{ return (_Real); }

	inline float floor(float _Real)
	{ return (std::floor(_Real)); }

	inline double floor(double _Real)
	{ return (std::floor(_Real)); }

	inline int ceil(int _Real) 
	{ return (_Real); }

	inline float ceil(float _Real)
	{ return (std::ceil(_Real)); }

	inline double ceil(double _Real)
	{ return (std::ceil(_Real)); }

	inline int mod(int _Real, int _Mod)
	{ return (_Real % _Mod); }

	inline float mod(float _Real, float _Mod)
	{ return (fmod(_Real, _Mod)); }

	inline double mod(double _Real, double _Mod)
	{ return (fmod(_Real, _Mod)); }

	inline float pow(float _Real, int _Power) 
	{ return (std::pow(_Real, _Power)); }

	inline float pow(float _Real, float _Power)
	{ return (std::powf(_Real, _Power)); }

	inline double pow(double _Real, int _Power)
	{ return (std::pow(_Real, _Power)); }

	inline double pow(double _Real, double _Power) 
	{ return (std::pow(_Real, _Power)); }

	inline int sqrt(int _Real) 
	{ return ( static_cast<int>(std::sqrt(_Real)) ); }

	inline float sqrt(float _Real) 
	{ return ( std::sqrt(_Real) ); }

	inline double sqrt(double _Real)
	{ return ( std::sqrt(_Real) ); }

	inline float randf() // region: [0.0, 1.0]
	{ return ( static_cast<float>(rand()) / static_cast<float>(RAND_MAX) ); }

	inline float randf(float _Min, float _Max) // region: [_Min, _Max]
	{ return (_Min + randf() * (_Max - _Min)); }

	inline float  to_degrees(float _Radians)
	{ return (_Radians * 57.29577791f); }
	
	inline double to_degrees(double _Radians)
	{ return (_Radians * 57.295777918682048834853); }

	inline float  to_radians(float _Degrees)
	{ return (_Degrees * 0.017453293f); }

	inline double to_radians(double _Degrees)
	{ return (_Degrees * 0.017453293005625408057523983); }

	inline float sin(float x) 
	{ return ( std::sin(x) ); }

	inline double sin(double x) 
	{ return ( std::sin(x) ); }

	inline float cos(float x) 
	{ return ( std::cos(x) ); }

	inline double cos(double x)
	{ return ( std::cos(x) ); }

	inline float tan(float x)
	{ return ( std::tan(x) ); }

	inline double tan(double x)
	{ return ( std::tan(x) ); }

	inline float asin(float x)
	{ return ( std::asin(x) ); }

	inline double asin(double x)
	{ return ( std::asin(x) ); }

	inline float acos(float x)
	{ return ( std::acos(x) ); }

	inline double acos(double x)
	{ return ( std::acos(x) ); }

	inline float atan(float x) 
	{ return ( std::atan(x) ); }

	inline double atan(double x) 
	{ return ( std::atan(x) ); }

	inline float atan2(float y, float x) 
	{ return ( std::atan2(y, x) ); }

	inline double atan2(double y, double x) 
	{ return ( std::atan2(y, x) ); }

	inline bool is_powof2(unsigned int x) 
	{
		/*
		pow of 2: only one 1 in bits
		0000100000000 or 000000000010 or 0000000001
		minus 1
		0000011111111 or 000000000001 or 0000000000
		*/
		return (x & (x - 1));
	}

	inline bool is_powof2(unsigned long x) 
	{ return (x & (x - 1)); }

	inline bool is_powof2(unsigned long long x)
	{ return (x & (x - 1)); }

	template<typename _Integ>
		_Integ _Factorial(_Integ n, _Integ _Limit) {
			return (n <= _Limit ? 1 : (n * _Factorial(n - 1, _Limit)));
		}

	template<typename _Integ = unsigned long long, typename _Ty = _Integ>
		_Ty factorial(_Ty n, _Integ _Limit = 1) {
			return ((_Ty)_Factorial((_Integ)n, _Limit));
		}

	template<typename _Ty>
		_Ty Stirling(_Ty n) {
			/*
			√(2πn) * (n/e)^n
			*/
			return (static_cast<_Ty>(sqrt(6.283 * n) * pow(n / 2.718, n)));
		}

	template<typename _Ty = size_t>
		_Ty binomial_coefficient(size_t n, size_t i) {
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

			if (n < _Lookup_index.size()) {
				return (static_cast<_Ty>(_Lookup_table[_Lookup_index[n] + i]));
			} else {
				for (size_t k = _Lookup_index.size(); k <= n; ++k) {
					_Lookup_index.push_back(_Lookup_table.size());      /* insert index */
					_Lookup_table.insert(_Lookup_table.end(), k + 1, 1);/* insert n+1 elemtns into end */

					unsigned long long* _Prev_ptr = &_Lookup_table[_Lookup_index[k - 1]];
					unsigned long long* _Curt_ptr = &_Lookup_table[_Lookup_index[k]];
					_Curt_ptr += 1; /* first element is 1 */
					do {
						*_Curt_ptr = *_Prev_ptr + *(_Prev_ptr + 1);
						++_Prev_ptr; ++_Curt_ptr;
					} while (*_Prev_ptr != 1);
				}
				return (static_cast<_Ty>(_Lookup_table[_Lookup_index[n] + i]));
			}
		}

	template<typename _Ty = size_t>
		_Ty C(size_t n, size_t i) {
			/*
					   n-i+1   n-(i-1)+1		  n-1+1
			C(n, i) = ------ * --------- *.... * -------
						 i		  i-1				1
		
			C(n, 0) = C(n, n) = 1
		
			i<n
			ΣC(n, i) = pow(2, n)
			i=0
			*/
			return (binomial_coefficient<_Ty>(n, i));
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
		struct Range_ {
			_Ty _Min, _Max;

			Range_() { /* empty */ }
			Range_(_in(_Ty) _Minval, _in(_Ty) _Maxval) : _Min(_Minval), _Max(_Maxval) { }

			bool exclude(_in(_Ty) _Val) const {
				return ( _Val < _Min || _Val > _Max );
			}

			bool include(_in(_Ty) _Val) const {
				return ( !this->exclude(_Val) );
			}

			template<typename _Fn1, typename _Fn2>
			bool exclude(_in(_Ty) _Val, _Fn1 _Func1, _Fn2 _Func2) const {
				return ( _Val < _Func1(_Min) || _Val > _Func2(_Max) );
			}

			template<typename _Fn1, typename _Fn2>
			bool include(_in(_Ty) _Val, _Fn1 _Func1, _Fn2 _Func2) const {
				return ( !this->exclude(_Val, _Func1, _Func2) );
			}

			_Ty range() const {
				return (_Max - _Min);
			}

			_Ty remap(_in(_Ty) _Val) const {
				return ( (_Val - _Min) / range() );
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

	template<typename _Ty>
		_Ty abs(_in(_Ty) _Src)
			{	// @_Require: ( constructor(base type) ) and ( operator<(rhs) ) and ( operator-() )
			return (_Src < _Ty(0) ? -_Src : _Src); 
			}

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
		_Ty saturation(_in(_Ty) _Val) 
			{	// return clamp(_Val, 0, 1)
			return ( clamp( _Val, static_cast<_Ty>(0), static_cast<_Ty>(1) ) );
			}

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
		const _Ty& lerp(_in(_Ty) _Lhs, _in(_Ty) _Rhs, /*in*/bool _Isright)
			{
			return (_Isright ? _Rhs : _Lhs);
			}

	template<typename _Ty, typename _Tyfactor>
		_Ty mix(_in(_Ty) _Lhs, _in(_Ty) _Rhs, _in(_Tyfactor) _Factor)
			{
			return ( lerp(_Lhs, _Rhs, _Factor) );
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
			auto discrim = B * B - Real(4.0) * A * C;
			if ( discrim < Real(0) ) {
				return ( false );
			} else {
				discrim = sqrt(discrim);
				auto q = (B < Real(0)) ? Real(-0.5) * (B - discrim) : Real(-0.5) * (B + discrim);
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


	/* @_Equaltion: 3*t^2 - 2*t^3 */
	template<typename _Ty> inline
	_Ty s_curve(_in(_Ty) t) {
		return ( t * t * (t * Real(-2) + Real(3)) );
	}

	/* @_Equaltion: 6*t - 6*t^2 */
	template<typename _Ty> inline
	_Ty s_curved(_in(_Ty) t) {	
		return ( Real(6) * (t - t * t) );
	}

	template<typename _Ty> inline
		_Ty fade(_in(_Ty) t) 
		{	// @_Equaltion: 6*t^5 - 15*t^4 + 10*t^3
		return ( t * t * t * ( t * ( t * Real(6) - Real(15) ) + Real(10) ) );
		}

	template<typename _Ty> inline
		_Ty faded(_in(_Ty) t)
		{	// @_Equaltion: 30 * (t^4 - 2*t^3 + t^2)
		return ( Real(30) * t * t * ( t * ( t - Real(2) ) + Real(1) ) );
		}

	template<typename _Ty> inline
		_Ty taylor_invsqrt(_in(_Ty) r)
		{	// @_Equaltion: 1.79284291400159 - 0.85373472095314 * r
		return (Real(-0.85373472095314) * r + Real(1.79284291400159));
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

	/* _Oldval map to [_Newmin,_Newmax] */
	template<typename _Ty> inline
		_Ty remap(_in(_Ty) _Oldval, _in(_Ty) _Oldmin, _in(_Ty) _Oldmax, _in(_Ty) _Newmin, _in(_Ty) _Newmax)
		{
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
		return (_Ty(C) * pow(t, i) * pow(Real(1) - t, n - i));
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

	// </general function>

}// namespace clmagic

#endif