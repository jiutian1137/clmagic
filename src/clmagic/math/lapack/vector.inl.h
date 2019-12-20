#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH_LAPACK___VECTOR___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH_LAPACK___VECTOR___H__
#include "../lapack.h"

namespace clmagic 
{
	template<size_t N, typename _Ty> inline
		Vec_<N, _Ty> vector_selector( /*in*/const _Ty* _Src, /*in*/const char* _Selector ) {
			Vec_<N, _Ty> _Result;
			for (size_t i = 0; i != N; ++i) {
				if ( isupper(_Selector[i]) ) {
					_Result[i] = _Selector[i] == 'W' ? _Src[3] : _Src[_Selector[i] - 'X'];
				} else {
					_Result[i] = _Selector[i] == 'w' ? _Src[3] : _Src[_Selector[i] - 'x'];
				}
			}
			return (_Result);
		}

	// - - - - - - - - - - - - - - <vec_>
	// < static >
	template< size_t N, typename T > inline
		Vec_<N, T> Vec_<N, T>::all( _in(T) _Val ) 
		{
		Vec_<N, T> _Result;
		for ( size_t i = 0; i != N; ++i ) _Result.at( i ) = _Val;
		return ( _Result );
		}

	template< size_t N, typename T > inline
		Vec_<N, T> Vec_<N, T>::zero()
		{
		return ( all( static_cast<T>( 0 ) ) );
		}
	// </ static >

	// < construct >
	template< size_t N, typename T > inline
		Vec_< N, T >::Vec_( _in(T) _All)
		{
		for (size_t i = 0; i != N; ++i)
			this->at(i) = static_cast<T>(_All);
		}

	template< size_t N, typename T > inline
		Vec_< N, T >::Vec_( _in(Vec2_<T>) _Right ) : _Mydata{ _Right.x, _Right.y }
		{ /* empty */
		}

	template< size_t N, typename T > inline
		Vec_< N, T >::Vec_( _in(Vec3_<T>) _Right ) : _Mydata{ _Right.x, _Right.y }
		{
		if (N >= 3) { this->at(2) = _Right.z; }
		}

	template< size_t N, typename T > inline
		Vec_< N, T >::Vec_( _in(Vec4_<T>) _Right ) : _Mydata{ _Right.x, _Right.y }
		{
		if (N >= 3) { this->at(2) = _Right.z; }
		if (N >= 4) { this->at(3) = _Right.w; }
		}

	template< size_t N, typename T > inline
		Vec_< N, T >::Vec_( _in(Mat_<1, N, T>) _Right )
		{
		for (size_t i = 0; i != N; ++i)
			this->at(i) = _Right->at(0, i);
		}

	template< size_t N, typename T >
	template<typename _Iter> inline
		Vec_< N, T >::Vec_(_Iter _First, _Iter _Last) 
		{ 
		this->assign( _First, _Last ); 
		}
	// </ construct >

	// <convert>
	template< size_t N, typename T > inline
		Vec_< N, T >::operator std::string( ) const
		{	// convert ot string
		using std::to_string;
		std::string _Str;

		_Str += "vec";
		_Str += std::to_string( N );
		_Str += "[";
		for (int i = 0; i != N; ++i)
			{
			_Str += to_string( this->at(i) );
			_Str += ',';
			}
		_Str.pop_back(); // pop ','
		_Str += "]";
		return ( _Str );
		}

	template< size_t N, typename T >
	template< size_t N2 > inline
		Vec_< N, T >::operator Vec_<N2, T>( ) const
		{
		Vec_< N2, T > _Result;
		std::move( this->ptr(), this->ptr() + minval(N2, N), _Result.ptr() );
		return ( _Result );
		}
	// </convert>

	// < behavior >
	template< size_t N, typename T >
	template< typename _Iter > inline
		void Vec_< N, T >::assign(_Iter _First, _Iter _Last)
		{	// assign by [_First, _Last) iterator
		std::move(_First, _Last, this->begin());
		/*int _Idx = 0;
		while (_First != _Last)
			{
			this->at(_Idx) = *_First;
			++_Idx;
			++_First;
			}*/
		}

	template< size_t N, typename T > inline
		void Vec_< N, T >::assign(std::initializer_list< T > _Ilist)
		{	// call assign(_Iter, _Iter)
		assign(_Ilist.begin(), _Ilist.end());
		}
	// </ behavior >
	// - - - - - - - - - - - - - - </vec_>



	template<typename T> inline
		bool operator< (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{ 
		return ( _Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_less<T>) );
		}

	template<typename T> inline 
		bool operator< (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{ 
		return ( _Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_less<T>) );
		}

	template<typename T> inline
		bool operator< (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{ 
		return ( _Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_less<T>) );
		}

	template<typename T> inline
		bool operator> (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{ 
		return ( _Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_great<T>) );
		}

	template<typename T> inline
		bool operator> (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{ 
		return ( _Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_great<T>) );
		}

	template<typename T> inline
		bool operator> (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{ 
		return ( _Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_great<T>) );
		}

	template<typename T> inline
		bool operator== (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{ 
		return ( _Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_equals<T>) );
		}
	
	template<typename T> inline
		bool operator== (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{ 
		return ( _Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_equals<T>) );
		}

	template<typename T> inline
		bool operator== (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{ 
		return ( _Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_equals<T>) );
		}

	template<typename T> inline
		bool operator<= (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{ 
		return (_Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_lessequal<T>));
		}

	template<typename T> inline
		bool operator<= (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{ 
		return (_Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_lessequal<T>));
		}

	template<typename T> inline
		bool operator<= (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{ 
		return (_Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_lessequal<T>));
		}

	template<typename T> inline
		bool operator>= (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{ 
		return (_Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_greatequal<T>));
		}

	template<typename T> inline
		bool operator>= (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{
		return (_Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_greatequal<T>));
		}

	template<typename T> inline 
		bool operator>= (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{ 
		return ( _Compare_vector_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size(), compare_greatequal<T>) );
		}

	
	// < operator-() >
	template<typename T> inline
		Vec2_<T> operator- (_in(Vec2_<T>) _Lhs)
		{
		return ( Vec2_<T>(-_Lhs[0], -_Lhs[1]) );
		}

	template<typename T> inline
		Vec3_<T> operator- (_in(Vec3_<T>) _Lhs)
		{
		return ( Vec3_<T>(-_Lhs[0], -_Lhs[1], -_Lhs[2]) );
		}

	template<typename T> inline
		Vec4_<T> operator- (_in(Vec4_<T>) _Lhs)
		{
		return ( Vec4_<T>(-_Lhs[0], -_Lhs[1], -_Lhs[2], -_Lhs[3]) );
		}

	template<size_t N, typename T> inline 
		Vec_<N,T> operator- (_in(Vec_<N,T>) _Lhs) 
		{
		Vec_<N, T> _Result;
		for (size_t i = 0; i != N; ++i) { _Result[i] = -_Lhs[i]; }
		return ( _Result );
		}
	// </ operator-() >



	/* < ? vector vector > */
	template<typename T> inline
		Vec2_<T> operator+ (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{
		return ( Vec2_<T>(_Lhs.x + _Rhs.x, _Lhs.y + _Rhs.y) );
		}

	template<typename T> inline
		Vec3_<T> operator+ (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{
		Vec3_<T> _Result; 
		_Add_vector_vector(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size()); 
		return (_Result);
		}

	template<typename T> inline
		Vec4_<T> operator+ (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{ 
		Vec4_<T> _Result; 
		_Add_vector_vector(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size());
		return (_Result); 
		}

	template<size_t N, typename T> inline
		Vec_<N,T> operator+ (_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs)
		{
		Vec_<N,T> _Result;
		_Add_vector_vector(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size());
		return (_Result);
		}


	template<typename T> inline
		Vec2_<T> operator- (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{
		return ( Vec2_<T>(_Lhs.x - _Rhs.x, _Lhs.y - _Rhs.y) );
		}

	template<typename T> inline
		Vec3_<T> operator- (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{
		Vec3_<T> _Result;
		_Sub_vector_vector(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size());
		return ( _Result );
		}

	template<typename T> inline
		Vec4_<T> operator- (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{
		Vec4_<T> _Result;
		_Sub_vector_vector(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size());
		return (_Result);
		}

	template<size_t N, typename T> inline
		Vec_<N,T> operator- (_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs)
		{
		Vec_<N,T> _Result;
		_Sub_vector_vector(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size());
		return (_Result);
		}


	template<typename T> inline
	Vec2_<T> operator* (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs) {
		return ( Vec2_<T>(_Lhs.x * _Rhs.x, _Lhs.y * _Rhs.y) );
	}

	template<typename T> inline
	Vec3_<T> operator* (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs) {
		return (Vec3_<T>(_Lhs.x * _Rhs.x, _Lhs.y * _Rhs.y, _Lhs.z * _Rhs.z));
	}

	template<typename T> inline
	Vec4_<T> operator* (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs) {
		return ( Vec4_<T>(_Lhs.x * _Rhs.x, _Lhs.y * _Rhs.y, _Lhs.z * _Rhs.z, _Lhs.w * _Rhs.w) );
	}

	template<size_t N, typename T> inline
	Vec_<N,T> operator* (_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs) {
		Vec_<N,T> _Result;
		_Mul_vector_vector<T>(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size());
		return (_Result);
	}

	template<> inline 
	Vec3_<float> operator* (_in(Vec3_<float>) _Lhs, _in(Vec3_<float>) _Rhs) {
		return (const_reference_cast<const Vec3_<float>>(
			simd_128f::mul(
				simd_128f::load(_Lhs.ptr()),
				simd_128f::load(_Rhs.ptr()))
		));
	}

	template<> inline
	Vec4_<float> operator* (_in(Vec4_<float>) _Lhs, _in(Vec4_<float>) _Rhs) {
		return (const_reference_cast<const Vec4_<float>>(
			simd_128f::mul(
				simd_128f::load(_Lhs.ptr()),
				simd_128f::load(_Rhs.ptr()))
		));
	}

	template<typename T> inline
		Vec2_<T> operator/ (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{
		return ( Vec2_<T>(_Lhs.x / _Rhs.x, _Lhs.y / _Rhs.y) );
		}

	template<typename T> inline
		Vec3_<T> operator/ (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{
		Vec3_<T> _Result;
		_Div_vector_vector<T>(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size());
		return ( _Result );
		}

	template<typename T> inline
		Vec4_<T> operator/ (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{
		Vec4_<T> _Result;
		_Div_vector_vector<T>(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size());
		return ( _Result );
		}

	template<size_t N, typename T> inline
		Vec_<N,T> operator/ (_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs)
		{
		Vec_<N,T> _Result;
		_Div_vector_vector<T>(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), N);
		return (_Result);
		}


	template<typename T> inline 
		Vec2_<T> operator% (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{
		return ( Vec2_<T>(mod(_Lhs.x, _Rhs.x), mod(_Lhs.y, _Rhs.y)) );
		}

	template<typename T> inline 
		Vec3_<T> operator% (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{
		Vec3_<T> _Result;
		_Mod_vector_vector<T>(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline 
		Vec4_<T> operator% (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{
		Vec4_<T> _Result;
		_Mod_vector_vector<T>(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size());
		return (_Result);
		}

	template<size_t N, typename T> inline
		Vec_<N,T> operator% (_in(Vec_<N,T>) _Lhs, _in(Vec_<N,T>) _Rhs)
		{
		Vec_<N, T> _Result;
		_Mod_vector_vector<T>(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), N);
		return (_Result);
		}
	/* </ ? vector vector > */
	


	/* < ? vector scalar > */
	template<typename T> inline 
		Vec2_<T> operator+ (_in(Vec2_<T>) _Lhs, _in(T) _Rhs) 
		{
		return ( Vec2_<T>(_Lhs.x + _Rhs, _Lhs.y + _Rhs) );
		}

	template<typename T> inline
		Vec3_<T> operator+ (_in(Vec3_<T>) _Lhs, _in(T) _Rhs)
		{
		return ( Vec3_<T>(_Lhs.x + _Rhs, _Lhs.y + _Rhs, _Lhs.z + _Rhs) );
		}

	template<typename T> inline 
		Vec4_<T> operator+ (_in(Vec4_<T>) _Lhs, _in(T) _Rhs)
		{
		Vec4_<T> _Result;
		_Add_vector_scalar( _Result.ptr(), _Lhs.ptr(), _Rhs, _Result.size() );
		return ( _Result );
		}

	template<size_t N, typename T> inline 
		Vec_<N, T> operator+ (_in(Vec_<N, T>) _Lhs, _in(T) _Rhs)
		{
		Vec_<N,T> _Result;
		_Add_vector_scalar( _Result.ptr(), _Lhs.ptr(), _Rhs, _Result.size() );
		return ( _Result );
		}


	template<typename T> inline 
		Vec2_<T> operator- (_in(Vec2_<T>) _Lhs, _in(T) _Rhs)
		{
		return ( Vec2_<T>(_Lhs.x - _Rhs, _Lhs.y - _Rhs) );
		}

	template<typename T> inline 
		Vec3_<T> operator- (_in(Vec3_<T>) _Lhs, _in(T) _Rhs)
		{
		return ( Vec3_<T>(_Lhs.x - _Rhs, _Lhs.y - _Rhs, _Lhs.z - _Rhs) );
		}

	template<typename T> inline 
		Vec4_<T> operator- (_in(Vec4_<T>) _Lhs, _in(T) _Rhs)
		{
		Vec4_<T> _Result;
		_Sub_vector_scalar( _Result.ptr(), _Lhs.ptr(), _Rhs, _Result.size() );
		return ( _Result );
		}

	template<size_t N, typename T> inline
		Vec_<N, T> operator- (_in(Vec_<N, T>) _Lhs, _in(T) _Rhs)
		{
		Vec_<N,T> _Result;
		_Sub_vector_scalar( _Result.ptr(), _Lhs.ptr(), _Rhs, _Result.size() );
		return (_Result);
		}


	template<typename T> inline 
		Vec2_<T> operator* (_in(Vec2_<T>) _Lhs, _in(T) _Rhs)
		{
		return ( Vec2_<T>(_Lhs.x * _Rhs, _Lhs.y * _Rhs) );
		}

	template<typename T> inline 
		Vec3_<T> operator* (_in(Vec3_<T>) _Lhs, _in(T) _Rhs)
		{
		Vec3_<T> _Result; 
		_Mul_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Rhs, _Result.size()); 
		return ( _Result );
		}

	template<typename T> inline 
		Vec4_<T> operator* (_in(Vec4_<T>) _Lhs, _in(T) _Rhs)
		{
		Vec4_<T> _Result; 
		_Mul_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Rhs, _Result.size()); 
		return ( _Result );
		}

	template<size_t N, typename T> inline
		Vec_<N,T> operator* (_in(Vec_<N,T>) _Lhs, _in(T) _Rhs)
		{
		Vec_<N,T> _Result;
		_Mul_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Rhs, _Result.size());
		return ( _Result );
		}


	template<typename T> inline
		Vec2_<T> operator/ (_in(Vec2_<T>) _Lhs, _in(T) _Rhs)
		{ 
		return ( Vec2_<T>(_Lhs.x / _Rhs, _Lhs.y / _Rhs) );
		}

	template<typename T> inline
		Vec3_<T> operator/ (_in(Vec3_<T>) _Lhs, _in(T) _Rhs)
		{
		Vec3_<T> _Result;
		_Div_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Rhs, _Lhs.size());
		return ( _Result );
		}

	template<typename T> inline
		Vec4_<T> operator/ (_in(Vec4_<T>) _Lhs, _in(T) _Rhs)
		{
		Vec4_<T> _Result;
		_Div_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Rhs, _Lhs.size());
		return ( _Result );
		}

	template<size_t N, typename T> inline
		Vec_<N,T> operator/ (_in(Vec_<N,T>) _Lhs, _in(T) _Rhs)
		{
		Vec_<N,T> _Result;
		_Div_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Rhs, N);
		return (_Result);
		}


	template<typename T> inline 
		Vec2_<T> operator% (_in(Vec2_<T>) _Lhs, _in(T) _Rhs)
		{ 
		return ( Vec2_<T>( mod(_Lhs.x, _Rhs), mod(_Lhs.y, _Rhs) ) );
		}

	template<typename T> inline 
		Vec3_<T> operator% (_in(Vec3_<T>) _Lhs, _in(T) _Rhs)
		{
		Vec3_<T> _Result;
		_Mod_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Rhs, _Result.size());
		return (_Result);
		}

	template<typename T> inline 
		Vec4_<T> operator% (_in(Vec4_<T>) _Lhs, _in(T) _Rhs)
		{
		Vec4_<T> _Result;
		_Mod_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Rhs, _Result.size());
		return (_Result);
		}

	template<size_t N, typename T> inline
		Vec_<N,T> operator% (_in(Vec_<N,T>) _Lhs, _in(T) _Rhs)
		{
		Vec_<N, T> _Result;
		_Mod_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Rhs, N);
		return (_Result);
		}


	template<typename T> inline Vec2_<T> operator+ (_in(Vec2_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return ( _Lhs + static_cast<T>(_Rhs) );
		}
	template<typename T> inline Vec3_<T> operator+ (_in(Vec3_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return ( _Lhs + static_cast<T>(_Rhs) );
		}
	template<typename T> inline Vec4_<T> operator+ (_in(Vec4_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs + static_cast<T>(_Rhs));
		}
	template<size_t N, typename T> inline Vec_<N, T> operator+ (_in(Vec_<N, T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs + static_cast<T>(_Rhs));
		}

	template<typename T> inline Vec2_<T> operator- (_in(Vec2_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return ( _Lhs - static_cast<T>(_Rhs) );
		}
	template<typename T> inline Vec3_<T> operator- (_in(Vec3_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs - static_cast<T>(_Rhs));
		}
	template<typename T> inline Vec4_<T> operator- (_in(Vec4_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs - static_cast<T>(_Rhs));
		}
	template<size_t N, typename T> inline Vec_<N, T> operator- (_in(Vec_<N, T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs - static_cast<T>(_Rhs));
		}

	template<typename T> inline Vec2_<T> operator* (_in(Vec2_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs * static_cast<T>(_Rhs));
		}
	template<typename T> inline Vec3_<T> operator* (_in(Vec3_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs * static_cast<T>(_Rhs));
		}
	template<typename T> inline Vec4_<T> operator* (_in(Vec4_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs * static_cast<T>(_Rhs));
		}
	template<size_t N, typename T> inline Vec_<N, T> operator* (_in(Vec_<N, T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs * static_cast<T>(_Rhs));
		}

	template<typename T> inline Vec2_<T> operator/ (_in(Vec2_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs / static_cast<T>(_Rhs));
		}
	template<typename T> inline Vec3_<T> operator/ (_in(Vec3_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs / static_cast<T>(_Rhs));
		}
	template<typename T> inline Vec4_<T> operator/ (_in(Vec4_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs / static_cast<T>(_Rhs));
		}
	template<size_t N, typename T> inline Vec_<N, T> operator/ (_in(Vec_<N, T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs / static_cast<T>(_Rhs));
		}

	template<typename T> inline Vec2_<T> operator% (_in(Vec2_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs % static_cast<T>(_Rhs));
		}
	template<typename T> inline Vec3_<T> operator% (_in(Vec3_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs % static_cast<T>(_Rhs));
		}
	template<typename T> inline Vec4_<T> operator% (_in(Vec4_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs % static_cast<T>(_Rhs));
		}
	template<size_t N, typename T> inline Vec_<N, T> operator% (_in(Vec_<N, T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs % static_cast<T>(_Rhs));
		}
	/* </ ? vector scalar > */



	/* < ? scalar vector > */
	template<typename T> inline
		Vec2_<T> operator* (_in(T) _Lhs, _in(Vec2_<T>) _Rhs)
		{ return (_Rhs * _Lhs); }

	template<typename T> inline
		Vec3_<T> operator* (_in(T) _Lhs, _in(Vec3_<T>) _Rhs)
		{ return (_Rhs * _Lhs); }

	template<typename T> inline
		Vec4_<T> operator* (_in(T) _Lhs, _in(Vec4_<T>) _Rhs)
		{ return (_Rhs * _Lhs); }

	template<size_t N, typename T> inline
		Vec_<N, T> operator* (_in(T) _Lhs, _in(Vec_<N, T>) _Rhs)
		{ return (_Rhs * _Lhs); }


	template<typename T> inline 
		Vec2_<T> operator/ (_in(T) _Lhs, _in(Vec2_<T>) _Rhs)
		{
		return ( Vec2_<T>(_Lhs / _Rhs.x, _Lhs / _Rhs.y) );
		}

	template<typename T> inline 
		Vec3_<T> operator/ (_in(T) _Lhs, _in(Vec3_<T>) _Rhs)
		{
		Vec3_<T> _Result;
		_Div_scalar_vector(_Result.ptr, _Lhs, _Rhs.ptr, _Result.size());
		return ( _Result );
		}

	template<typename T> inline 
		Vec4_<T> operator/ (_in(T) _Lhs, _in(Vec4_<T>) _Rhs)
		{
		Vec4_<T> _Result;
		_Div_scalar_vector(_Result.ptr, _Lhs, _Rhs.ptr, _Result.size());
		return ( _Result );
		}

	template<size_t N, typename T> inline
		Vec_<N,T> operator/ (_in(T) _Lhs, _in(Vec_<N,T>) _Rhs)
		{
		Vec_<N,T> _Result;
		_Div_scalar_vector(_Result._Mydata(), _Lhs, _Rhs._Mydata(), _Result.size());
		return (_Result);
		}
	

	template<typename T> Vec2_<T> inline operator* (_in(Real) _Lhs, _in(Vec2_<T>) _Rhs) 
		{
		return (static_cast<T>(_Lhs) * _Rhs);
		}
	template<typename T> Vec3_<T> inline operator* (_in(Real) _Lhs, _in(Vec3_<T>) _Rhs) 
		{
		return (static_cast<T>(_Lhs) * _Rhs);
		}
	template<typename T> Vec4_<T> inline operator* (_in(Real) _Lhs, _in(Vec4_<T>) _Rhs) 
		{
		return (static_cast<T>(_Lhs) * _Rhs);
		}
	template<size_t N, typename T> inline Vec_<N, T> operator* (_in(Real) _Lhs, _in(Vec_<N, T>) _Rhs) 
		{
		return (static_cast<T>(_Lhs) * _Rhs);
		}

	template<typename T> inline Vec2_<T> operator/ (_in(Real) _Lhs, _in(Vec2_<T>) _Rhs) 
		{
		return (static_cast<T>(_Lhs) / _Rhs);
		}
	template<typename T> inline Vec3_<T> operator/ (_in(Real) _Lhs, _in(Vec3_<T>) _Rhs) 
		{
		return (static_cast<T>(_Lhs) / _Rhs);
		}
	template<typename T> inline Vec4_<T> operator/ (_in(Real) _Lhs, _in(Vec4_<T>) _Rhs) 
		{
		return (static_cast<T>(_Lhs) / _Rhs);
		}
	template<size_t N, typename T> inline Vec_<N, T> operator/ (_in(Real) _Lhs, _in(Vec_<N, T>) _Rhs)
		{
		return (static_cast<T>(_Lhs) / _Rhs);
		}
	/* </ ? scalar vector > */



	/* < ? vector matrix > */
	template<typename T> inline 
		Vec2_<T> operator* (_in(Vec2_<T>) _Lhs, _in(Mat_<2, 2, T>) _Rhs)
		{
		auto _Resutl = reference_cast<const Mat_<1, 2, T>>(_Lhs) * _Rhs;
		return ( reference_cast<Vec2_<T>>(_Resutl) );
		}

	template<typename T> inline 
		Vec3_<T> operator* (_in(Vec2_<T>) _Lhs, _in(Mat_<2, 3, T>) _Rhs)
		{
		auto _Resutl = reference_cast<const Mat_<1, 2, T>>(_Lhs) * _Rhs;
		return (reference_cast<Vec3_<T>>(_Resutl));
		}

	template<typename T> inline 
		Vec4_<T> operator* (_in(Vec2_<T>) _Lhs, _in(Mat_<2, 4, T>) _Rhs)
		{
		auto _Resutl = reference_cast<const Mat_<1, 2, T>>(_Lhs) * _Rhs;
		return ( reference_cast<Vec4_<T>>(_Resutl) );
		}

	template<typename T> inline 
		Vec2_<T> operator* (_in(Vec3_<T>) _Lhs, _in(Mat_<3, 2, T>) _Rhs)
		{
		auto _Resutl = reference_cast<const Mat_<1, 3, T>>(_Lhs) * _Rhs;
		return (reference_cast<Vec2_<T>>(_Resutl));
		}

	template<typename T> inline 
		Vec3_<T> operator* (_in(Vec3_<T>) _Lhs, _in(Mat_<3, 3, T>) _Rhs)
		{
		auto _Resutl = reference_cast<const Mat_<1, 3, T>>(_Lhs) * _Rhs;
		return (reference_cast<Vec3_<T>>(_Resutl));
		}

	template<typename T> inline 
		Vec4_<T> operator* (_in(Vec3_<T>) _Lhs, _in(Mat_<3, 4, T>) _Rhs)
		{
		auto _Resutl = reference_cast<const Mat_<1, 3, T>>(_Lhs) * _Rhs;
		return (reference_cast<Vec4_<T>>(_Resutl));
		}

	template<typename T> inline 
		Vec2_<T> operator* (_in(Vec4_<T>) _Lhs, _in(Mat_<4, 2, T>) _Rhs)
		{
		auto _Resutl = reference_cast<const Mat_<1, 4, T>>(_Lhs) * _Rhs;
		return (reference_cast<Vec2_<T>>(_Resutl));
		}

	template<typename T> inline 
		Vec3_<T> operator* (_in(Vec4_<T>) _Lhs, _in(Mat_<4, 3, T>) _Rhs)
		{
		auto _Resutl = reference_cast<const Mat_<1, 4, T>>(_Lhs) * _Rhs;
		return (reference_cast<Vec3_<T>>(_Resutl));
		}

	template<typename T> inline 
		Vec4_<T> operator* (_in(Vec4_<T>) _Lhs, _in(Mat_<4, 4, T>) _Rhs)
		{
		auto _Resutl = reference_cast<const Mat_<1, 4, T>>(_Lhs) * _Rhs;
		return (reference_cast<Vec4_<T>>(_Resutl));
		}

	template<size_t N, typename T, size_t P> inline
		Vec_<P,T> operator* (_in(Vec_<N, T>) _Lhs, _in(Mat_<N, P, T>) _Rhs)
		{
		auto _Result = reference_cast< const Mat_<1, N, T> >( _Lhs ) * _Rhs;
		return ( reference_cast< Vec_<P, T> >( _Result ) );
		}
	/* </ ? vector matrix > */
	


	/* </ ?= vector vector > */
	template<typename T> inline 
		Vec2_<T>& operator+= (_inout(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{ 
		_Lhs = _Lhs + _Rhs; 
		return(_Lhs);
		}

	template<typename T> inline 
		Vec3_<T>& operator+= (_inout(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{
		_Lhs = _Lhs + _Rhs;
		return(_Lhs);
		}

	template<typename T> inline 
		Vec4_<T>& operator+= (_inout(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{ 
		_Add_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return(_Lhs); 
		}

	template<size_t N, typename T> inline
		Vec_<N,T>& operator+= (_inout(Vec_<N,T>) _Lhs, _in(Vec_<N,T>) _Rhs)
		{
		_Add_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return (_Lhs);
		}


	template<typename T> inline 
		Vec2_<T>& operator-= (_inout(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{ 
		_Lhs = _Lhs - _Rhs;
		return ( _Lhs );
		}

	template<typename T> inline
		Vec3_<T>& operator-= (_inout(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{ 
		_Sub_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return ( _Lhs );
		}

	template<typename T> inline
		Vec4_<T>& operator-= (_inout(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{
		_Sub_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return ( _Lhs );
		}

	template<size_t N, typename T> inline
		Vec_<N,T>& operator-= (_inout(Vec_<N,T>) _Lhs, _in(Vec_<N,T>) _Rhs)
		{
		_Sub_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return ( _Lhs );
		}


	template<typename T> inline
		Vec2_<T>& operator*= (_inout(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{
		_Mul_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return(_Lhs);
		}

	template<typename T> inline
		Vec3_<T>& operator*= (_inout(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{
		_Mul_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return(_Lhs);
		}

	template<typename T> inline
		Vec4_<T>& operator*= (_inout(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{ 
		_Mul_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size()); 
		return(_Lhs); 
		}

	template<size_t N, typename T> inline
		Vec_<N,T>& operator*= (_inout(Vec_<N,T>) _Lhs, _in(Vec_<N, T>) _Rhs)
		{
		_Mul_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return ( _Lhs );
		}


	template<typename T> inline
		Vec2_<T>& operator/= (_inout(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{
		_Div_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return (_Lhs);
		}

	template<typename T> inline
		Vec3_<T>& operator/= (_inout(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{
		_Div_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return (_Lhs);
		}

	template<typename T> inline
		Vec4_<T>& operator/= (_inout(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{ 
		_Div_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return (_Lhs);
		}

	template<size_t N, typename T> inline
		Vec_<N, T>& operator/= (_inout(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs)
		{
		_Div_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return (_Lhs);
		}


	template<typename T> inline 
		Vec2_<T>& operator%= (_inout(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{
		_Mod_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return (_Lhs);
		}

	template<typename T> inline 
		Vec3_<T>& operator%= (_inout(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{
		_Mod_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return (_Lhs);
		}

	template<typename T> inline 
		Vec4_<T>& operator%= (_inout(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{
		_Mod_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return (_Lhs);
		}

	template<size_t N, typename T> inline 
		Vec_<N, T>& operator%= (_inout(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs)
		{
		_Mod_vector_vector(_Lhs.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Lhs.size());
		return (_Lhs);
		}
	/* </ ?= vector vector > */



	/* < ?= vector scalar > */
	template<typename T> inline
		Vec2_<T>& operator*= (_inout(Vec2_<T>) _Lhs, _in(T) _Rhs) 
		{
		_Mul_vector_scalar(_Lhs.ptr(), _Lhs.ptr(), _Rhs, _Lhs.size());
		return(_Lhs);
		}

	template<typename T> inline
		Vec3_<T>& operator*= (_inout(Vec3_<T>) _Lhs, _in(T) _Rhs)
		{
		_Mul_vector_scalar(_Lhs.ptr(), _Lhs.ptr(), _Rhs, _Lhs.size());
		return(_Lhs);
		}

	template<typename T> inline 
		Vec4_<T>& operator*= (_inout(Vec4_<T>) _Lhs, _in(T) _Rhs)
		{ 
		_Mul_vector_scalar(_Lhs.ptr(), _Lhs.ptr(), _Rhs, _Lhs.size());
		return(_Lhs); 
		}

	template<size_t N, typename T> inline
		Vec_<N, T>& operator*= (_inout(Vec_<N, T>) _Lhs, _in(T) _Rhs )
		{
		_Mul_vector_scalar(_Lhs.ptr(), _Lhs.ptr(), _Rhs, _Lhs.size());
		return (_Lhs);
		}


	template<typename T> inline 
		Vec2_<T>& operator/= (_inout(Vec2_<T>) _Lhs, _in(T) _Rhs)
		{
		_Div_vector_scalar(_Lhs.ptr(), _Lhs.ptr(), _Rhs, _Lhs.size());
		return (_Lhs);
		}
	
	template<typename T> inline
		Vec3_<T>& operator/= (_inout(Vec3_<T>) _Lhs, _in(T) _Rhs)
		{
		_Div_vector_scalar(_Lhs.ptr(), _Lhs.ptr(), _Rhs, _Lhs.size());
		return (_Lhs);
		}

	template<typename T> inline 
		Vec4_<T>& operator/= (_inout(Vec4_<T>) _Lhs, _in(T) _Rhs)
		{
		_Div_vector_scalar(_Lhs.ptr(), _Lhs.ptr(), _Rhs, _Lhs.size());
		return (_Lhs);
		}

	template<size_t N, typename T> inline
		Vec_<N, T>& operator/= (_inout(Vec_<N, T>) _Lhs, _in(T) _Rhs)
		{
		_Div_vector_scalar( _Lhs.ptr(), _Lhs.ptr(), _Rhs, _Lhs.size() );
		return (_Lhs);
		}


	template<typename T> inline
		Vec2_<T>& operator%= (_inout(Vec2_<T>) _Lhs, _in(T) _Rhs)
		{
		_Lhs = _Lhs % _Rhs;
		return (_Lhs);
		}

	template<typename T> inline 
		Vec3_<T>& operator%= (_inout(Vec3_<T>) _Lhs, _in(T) _Rhs)
		{
		_Mod_vector_scalar( _Lhs.ptr(), _Lhs.ptr(), _Rhs, _Lhs.size() );
		return (_Lhs);
		}

	template<typename T> inline 
		Vec4_<T>& operator%= (_inout(Vec4_<T>) _Lhs, _in(T) _Rhs) 
		{
		_Mod_vector_scalar( _Lhs.ptr(), _Lhs.ptr(), _Rhs, _Lhs.size() );
		return (_Lhs);
		}

	template<size_t N, typename T> inline 
		Vec_<N,T>& operator%= (_inout(Vec_<N, T>) _Lhs, _in(T) _Rhs) 
		{
		_Mod_vector_scalar(_Lhs.ptr(), _Lhs.ptr(), _Rhs, _Lhs.size());
		return (_Lhs);
		}


	template<typename T> inline 
		Vec2_<T>& operator*= (_inout(Vec2_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs *= static_cast<T>(_Rhs));
		}
	template<typename T> inline 
		Vec3_<T>& operator*= (_inout(Vec3_<T>) _Lhs, _in(Real) _Rhs)
		{
		return (_Lhs *= static_cast<T>(_Rhs));
		}
	template<typename T> inline 
		Vec4_<T>& operator*= (_inout(Vec4_<T>) _Lhs, _in(Real) _Rhs)
		{
		return (_Lhs *= static_cast<T>(_Rhs));
		}
	template<size_t N, typename T> inline 
		Vec_<N, T>& operator*= (_inout(Vec_<N, T>) _Lhs, _in(Real) _Rhs)
		{
		return (_Lhs *= static_cast<T>(_Rhs));
		}

	template<typename T> inline
		Vec2_<T>& operator/= (_inout(Vec2_<T>) _Lhs, _in(Real) _Rhs)
		{
		return (_Lhs /= static_cast<T>(_Rhs));
		}
	template<typename T> inline
		Vec3_<T>& operator/= (_inout(Vec3_<T>) _Lhs, _in(Real) _Rhs)
		{
		return (_Lhs /= static_cast<T>(_Rhs));
		}
	template<typename T> inline
		Vec4_<T>& operator/= (_inout(Vec4_<T>) _Lhs, _in(Real) _Rhs)
		{
		return (_Lhs /= static_cast<T>(_Rhs));
		}
	template<size_t N, typename T> inline 
		Vec_<N, T>& operator/= (_inout(Vec_<N, T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs /= static_cast<T>(_Rhs));
		}

	template<typename T> inline 
		Vec2_<T>& operator%= (_inout(Vec2_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs /= static_cast<T>(_Rhs));
		}
	template<typename T> inline 
		Vec3_<T>& operator%= (_inout(Vec3_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs /= static_cast<T>(_Rhs));
		}
	template<typename T> inline 
		Vec4_<T>& operator%= (_inout(Vec4_<T>) _Lhs, _in(Real) _Rhs) 
		{
		return (_Lhs /= static_cast<T>(_Rhs));
		}
	template<size_t N, typename T> inline 
		Vec_<N, T>& operator%= (_inout(Vec_<N, T>) _Lhs, _in(Real) _Rhs)
		{
		return (_Lhs /= static_cast<T>(_Rhs));
		}
	/* </ ?= vector scalar > */



	/* < cout vector > */
	template<typename T> inline 
		std::ostream& operator<< (_inout(std::ostream) _Ostr, _in(Vec2_<T>) _Lhs)
		{
		_Ostr << static_cast<std::string>(_Lhs);
		return (_Ostr);
		}

	template<typename T> inline
		std::ostream& operator<< (_inout(std::ostream) _Ostr, _in(Vec3_<T>) _Lhs)
		{
		_Ostr << static_cast<std::string>(_Lhs);
		return (_Ostr);
		}

	template<typename T> inline
		std::ostream& operator<< (_inout(std::ostream) _Ostr, _in(Vec4_<T>) _Lhs)
		{
		_Ostr << static_cast<std::string>(_Lhs);
		return (_Ostr);
		}

	template<size_t N, typename T> inline
		std::ostream& operator<< (_inout(std::ostream) _Ostr, _in(Vec_<N, T>) _Lhs)
		{
		_Ostr << std::string(_Lhs);
		return (_Ostr);
		}
	/* </ cout vector > */



	/* < pow > */
	template<typename T> inline
		Vec2_<T> pow(_in(Vec2_<T>) _Lhs, int _Power)
		{
		return ( Vec2_<T>( pow(_Lhs.x, _Power), pow(_Lhs.y, _Power) ) );
		}

	template<typename T> inline
		Vec3_<T> pow(_in(Vec3_<T>) _Lhs, int _Power)
		{
		Vec3_<T> _Result;
		_Pow_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Power, _Result.size());
		return (_Result);
		}

	template<typename T> inline
		Vec4_<T> pow(_in(Vec4_<T>) _Lhs, int _Power)
		{
		Vec4_<T> _Result;
		_Pow_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Power, _Result.size());
		return (_Result);
		}

	template<typename T> inline
		Vec2_<T> pow(_in(Vec2_<T>) _Lhs, double _Power)
		{
		using std::pow;
		return (Vec2_<T>(pow(_Lhs.x, _Power), pow(_Lhs.y, _Power)));
		}

	template<typename T> inline
		Vec3_<T> pow(_in(Vec3_<T>) _Lhs, double _Power)
		{
		Vec3_<T> _Result;
		_Pow_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Power, _Result.size());
		return (_Result);
		}

	template<typename T> inline
		Vec4_<T> pow(_in(Vec4_<T>) _Lhs, double _Power)
		{
		Vec4_<T> _Result;
		_Pow_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Power, _Result.size());
		return (_Result);
		}

	template<typename T> inline 
		Vec2_<T> pow(_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{
		using std::pow;
		return ( Vec2_<T>(pow(_Lhs.x, _Rhs.x), pow(_Lhs.y, _Rhs.y)) );
		}

	template<typename T> inline 
		Vec3_<T> pow(_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{
		Vec3_<T> _Result;
		_Pow_vector_vector(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size());
		return ( _Result );
		}

	template<typename T> inline 
		Vec4_<T> pow(_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{
		Vec4_<T> _Result;
		_Pow_vector_vector(_Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size());
		return ( _Result );
		}

	template<size_t N, typename T> inline 
		Vec_<N, T> pow(_in(Vec_<N, T>) _Lhs, int _Power) 
		{
		Vec_<N, T> _Result;
		_Pow_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Power, _Result.size());
		return ( _Result );
		}
	
	template<size_t N, typename T> inline 
		Vec_<N, T> pow(_in(Vec_<N, T>) _Lhs, double _Power) 
		{
		Vec_<N, T> _Result;
		_Pow_vector_scalar(_Result.ptr(), _Lhs.ptr(), _Power, _Result.size());
		return (_Result);
		}

	template<size_t N, typename T> inline 
		Vec_<N, T> pow(_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs) 
		{
		Vec_<N, T> _Result;
		_Pow_vector_vector( _Result.ptr(), _Lhs.ptr(), _Rhs.ptr(), _Result.size() );
		return (_Result);
		}
	/* </ pow > */

	/* < sqrt > */
	template<typename T> inline
		Vec2_<T> sqrt(_in(Vec2_<T>) _Lhs)
		{
		return ( Vec2_<T>(sqrt(_Lhs.x), sqrt(_Lhs.y)) );
		}

	template<typename T> inline
		Vec3_<T> sqrt(_in(Vec3_<T>) _Lhs)
		{ 
		Vec3_<T> _Result;
		_Sqrt_vector<T>(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline 
		Vec4_<T> sqrt(_in(Vec4_<T>) _Lhs)
		{
		Vec4_<T> _Result;
		_Sqrt_vector<T>(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<size_t N, typename T> inline
		Vec_<N,T> sqrt(_in(Vec_<N,T>) _Lhs)
		{
		Vec_<N, T> _Result;
		_Sqrt_vector<T>(_Result.ptr(), _Lhs.ptr(), N);
		return ( _Result );
		}
	/* </ sqrt > */

	/* < floor > */
	template<typename T> inline 
		Vec2_<T> floor(_in(Vec2_<T>) _Lhs) 
		{
		Vec2_<T> _Result;
		_Floor_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline 
		Vec3_<T> floor(_in(Vec3_<T>) _Lhs)
		{
		Vec3_<T> _Result;
		_Floor_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline
		Vec4_<T> floor(_in(Vec4_<T>) _Lhs)
		{
		Vec4_<T> _Result;
		_Floor_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<size_t N, typename T> inline
		Vec_<N, T> floor(_in(Vec_<N, T>) _Lhs)
		{
		Vec_<N, T> _Result;
		_Floor_vector(_Result.ptr(), _Lhs.ptr(), N);
		return (_Result);
		}
	/* </ floor > */

	/* < ceil > */
	template<typename T> inline
		Vec2_<T> ceil(_in(Vec2_<T>) _Lhs)
		{
		Vec2_<T> _Result;
		_Ceil_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline
		Vec3_<T> ceil(_in(Vec3_<T>) _Lhs)
		{
		Vec3_<T> _Result;
		_Ceil_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline
		Vec4_<T> ceil(_in(Vec4_<T>) _Lhs)
		{
		Vec4_<T> _Result;
		_Ceil_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<size_t N, typename T> inline
		Vec_<N, T> ceil(_in(Vec_<N, T>) _Lhs)
		{
		Vec_<N, T> _Result;
		_Ceil_vector(_Result.ptr(), _Lhs.ptr(), N);
		return (_Result);
		}
	/* </ ceil > */


	/* < mod > */
	template<typename T> inline 
	Vec2_<T> mod(_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs) {
		return (_Lhs % _Rhs);
	}

	template<typename T> inline 
	Vec3_<T> mod(_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs) {
		return (_Lhs % _Rhs);
	}

	template<typename T> inline 
	Vec4_<T> mod(_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs) {
		return (_Lhs % _Rhs);
	}

	template<size_t N, typename T> inline 
	Vec_<N, T> mod(_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs) {
		return (_Lhs % _Rhs);
	}

	template<typename T> inline 
	Vec2_<T> mod(_in(Vec2_<T>) _Lhs, _in(T) _Rhs) {
		return (_Lhs % _Rhs);
	}

	template<typename T> inline 
	Vec3_<T> mod(_in(Vec3_<T>) _Lhs, _in(T) _Rhs) {
		return (_Lhs % _Rhs);
	}

	template<typename T> inline 
	Vec4_<T> mod(_in(Vec4_<T>) _Lhs, _in(T) _Rhs) {
		return (_Lhs % _Rhs);
	}

	template<size_t N, typename T> inline 
	Vec_<N, T> mod(_in(Vec_<N, T>) _Lhs, _in(T) _Rhs) {
		return (_Lhs % _Rhs);
	}
	/* </ mod > */


	/* < trianglar function > */
	template<typename T> inline 
		Vec2_<T> sin(_in(Vec2_<T>) _Lhs) 
		{
		Vec2_<T> _Result;
		_Sin_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return ( _Result );
		}

	template<typename T> inline 
		Vec3_<T> sin(_in(Vec3_<T>) _Lhs) 
		{
		Vec3_<T> _Result;
		_Sin_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline 
		Vec4_<T> sin(_in(Vec4_<T>) _Lhs) 
		{
		Vec4_<T> _Result;
		_Sin_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}


	template<typename T> inline 
		Vec2_<T> cos(_in(Vec2_<T>) _Lhs)
		{
		Vec2_<T> _Result;
		_Cos_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline 
		Vec3_<T> cos(_in(Vec3_<T>) _Lhs)
		{
		Vec3_<T> _Result;
		_Cos_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline 
		Vec4_<T> cos(_in(Vec4_<T>) _Lhs)
		{
		Vec4_<T> _Result;
		_Cos_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}


	template<typename T> inline 
		Vec2_<T> tan(_in(Vec2_<T>) _Lhs)
		{
		Vec2_<T> _Result;
		_Tan_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline 
		Vec3_<T> tan(_in(Vec3_<T>) _Lhs)
		{
		Vec3_<T> _Result;
		_Tan_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline 
		Vec4_<T> tan(_in(Vec4_<T>) _Lhs)
		{
		Vec4_<T> _Result;
		_Tan_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}


	template<typename T> inline
		Vec2_<T> asin(_in(Vec2_<T>) _Lhs)
		{
		Vec2_<T> _Result;
		_Asin_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline
		Vec3_<T> asin(_in(Vec3_<T>) _Lhs)
		{
		Vec3_<T> _Result;
		_Asin_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline
		Vec4_<T> asin(_in(Vec4_<T>) _Lhs)
		{
		Vec4_<T> _Result;
		_Asin_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}


	template<typename T> inline
		Vec2_<T> acos(_in(Vec2_<T>) _Lhs)
		{
		Vec2_<T> _Result;
		_Acos_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline
		Vec3_<T> acos(_in(Vec3_<T>) _Lhs)
		{
		Vec3_<T> _Result;
		_Acos_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline
		Vec4_<T> acos(_in(Vec4_<T>) _Lhs)
		{
		Vec4_<T> _Result;
		_Acos_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}


	template<typename T> inline
		Vec2_<T> atan(_in(Vec2_<T>) _Lhs)
		{
		Vec2_<T> _Result;
		_Atan_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline
		Vec3_<T> atan(_in(Vec3_<T>) _Lhs)
		{
		Vec3_<T> _Result;
		_Atan_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<typename T> inline
		Vec4_<T> atan(_in(Vec4_<T>) _Lhs)
		{
		Vec4_<T> _Result;
		_Atan_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}


	template<size_t N, typename T> inline 
		Vec_<N, T> sin(_in(Vec_<N, T>) _Lhs) 
		{
		Vec_<N, T> _Result;
		_Sin_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<size_t N, typename T> inline 
		Vec_<N, T> cos(_in(Vec_<N, T>) _Lhs) 
		{
		Vec_<N, T> _Result;
		_Cos_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<size_t N, typename T> inline
		Vec_<N, T> tan(_in(Vec_<N, T>) _Lhs)
		{
		Vec_<N, T> _Result;
		_Tan_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<size_t N, typename T> inline
		Vec_<N, T> asin(_in(Vec_<N, T>) _Lhs)
		{
		Vec_<N, T> _Result;
		_Asin_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<size_t N, typename T> inline
		Vec_<N, T> acos(_in(Vec_<N, T>) _Lhs)
		{
		Vec_<N, T> _Result;
		_Acos_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}

	template<size_t N, typename T> inline
		Vec_<N, T> atan(_in(Vec_<N, T>) _Lhs)
		{
		Vec_<N, T> _Result;
		_Atan_vector(_Result.ptr(), _Lhs.ptr(), _Result.size());
		return (_Result);
		}
	/* </ trianglar function > */


	// < dot >
	template<typename T> inline 
		T dot(_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{
		return (_Lhs.x * _Rhs.x + _Lhs.y * _Rhs.y);
		}

	template<typename T> inline
		T dot(_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{
		return (_Lhs.x * _Rhs.x + _Lhs.y * _Rhs.y + _Lhs.z * _Rhs.z);
		}

	template<typename T> inline
		T dot(_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs)
		{
		return (_Lhs.x * _Rhs.x + _Lhs.y * _Rhs.y + _Lhs.z * _Rhs.z + _Lhs.w * _Rhs.w);
		}

	template<size_t N, typename T> inline
		T dot(_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs)
		{
		return ( _Dot_vector<T>(_Lhs.ptr(), _Rhs.ptr(), _Lhs.size()) );
		}
	// </ dot >

	template<typename T> inline
		Vec2_<T> cross(_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs)
		{
		/*
		lhs X   Y
			  *
		rhs X   Y
		*/
		return (Vec2_<T>(_Lhs.y * _Rhs.x - _Lhs.x * _Rhs.y,
			_Lhs.x * _Rhs.y - _Lhs.y * _Rhs.x));
		}

	template<typename T> inline
		Vec3_<T> cross(_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs)
		{
		/*
		lhs X   Y   Z
			  *	  *
		rhs X   Y   Z
		*/
		return (Vec3_<T>(_Lhs.y * _Rhs.z - _Lhs.z * _Rhs.y,
						_Lhs.z * _Rhs.x - _Lhs.x * _Rhs.z,
						_Lhs.x * _Rhs.y - _Lhs.y * _Rhs.x));
		}

	template<typename T> inline
		Vec4_<T> cross(_in(Vec4_<T>) _V1, _in(Vec4_<T>) _V2, _in(Vec4_<T>) _V3)
		{
		// [ ((v2.z*v3.w-v2.w*v3.z)*v1.y)-((v2.y*v3.w-v2.w*v3.y)*v1.z)+((v2.y*v3.z-v2.z*v3.y)*v1.w),
		//   ((v2.w*v3.z-v2.z*v3.w)*v1.x)-((v2.w*v3.x-v2.x*v3.w)*v1.z)+((v2.z*v3.x-v2.x*v3.z)*v1.w),
		//   ((v2.y*v3.w-v2.w*v3.y)*v1.x)-((v2.x*v3.w-v2.w*v3.x)*v1.y)+((v2.x*v3.y-v2.y*v3.x)*v1.w),
		//   ((v2.z*v3.y-v2.y*v3.z)*v1.x)-((v2.z*v3.x-v2.x*v3.z)*v1.y)+((v2.y*v3.x-v2.x*v3.y)*v1.z) ]
		return (Vec4_<T>((_V2.z * _V3.w - _V2.w * _V3.z) * _V1.y - (_V2.y * _V3.w - _V2.w * _V3.y) * _V1.z + (_V2.y * _V3.z - _V2.z * _V3.y) * _V1.w,
						(_V2.w * _V3.z - _V2.z * _V3.w) * _V1.x - (_V2.w * _V3.x - _V2.x * _V3.w) * _V1.z + (_V2.z * _V3.x - _V2.x * _V3.z) * _V1.w,
						(_V2.y * _V3.w - _V2.w * _V3.y) * _V1.x - (_V2.x * _V3.w - _V2.w * _V3.x) * _V1.y + (_V2.x * _V3.y - _V2.y * _V3.x) * _V1.w,
						(_V2.z * _V3.y - _V2.y * _V3.z) * _V1.x - (_V2.z * _V3.x - _V2.x * _V3.z) * _V1.y + (_V2.y * _V3.x - _V2.x * _V3.y) * _V1.z));
		}

	template<typename T> inline
		Vec_<3, T> cross(_in(Vec_<3, T>) _Lhs, _in(Vec_<3, T>) _Rhs)
		{
		/*
			_Lhs [0]   [1]   [2]
					 X     X
			_Rhs [0]   [1]   [2]
		*/
		return (Vec_<3, T>(_Lhs[1] * _Rhs[2] - _Rhs[1] * _Lhs[2],
							_Lhs[2] * _Rhs[0] - _Rhs[2] * _Lhs[0],
							_Lhs[0] * _Rhs[1] - _Rhs[0] * _Lhs[1]));
		}


	// < length >
	template<typename T> inline 
		T length( _in(Vec2_<T>) _Lhs )
		{ return ( sqrt( dot(_Lhs, _Lhs) ) ); }

	template<typename T> inline 
		T length( _in(Vec3_<T>) _Lhs)
		{ return ( sqrt(dot(_Lhs, _Lhs)) ); }

	template<typename T> inline 
		T length( _in(Vec4_<T>) _Lhs)
		{ return ( sqrt(dot(_Lhs, _Lhs)) ); }

	template<typename T> inline 
		T length( _in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs )
		{ return ( length(_Rhs - _Lhs) ); }

	template<typename T> inline 
		T length( _in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs )
		{ return ( length(_Rhs - _Lhs) ); }

	template<typename T> inline 
		T length( _in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs )
		{ return ( length(_Rhs - _Lhs) ); }

	template<size_t N, typename T> inline
		T length(_in(Vec_<N, T>) _Lhs)
		{
		return ( sqrt(dot(_Lhs, _Lhs)) );
		}

	template<size_t N, typename T> inline
		T length(_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs)
		{
		return ( length(_Rhs - _Lhs) );
		}
	// </ length >


	// < normalize >
	template<typename T> inline
		Vec2_<T> normalize( _in(Vec2_<T>) _Lhs )
		{ return (_Lhs / length(_Lhs)); }

	template<typename T> inline
		Vec3_<T> normalize( _in(Vec3_<T>) _Lhs )
		{ return (_Lhs / length(_Lhs)); }

	template<typename T> inline
		Vec4_<T> normalize( _in(Vec4_<T>) _Lhs )
		{ return (_Lhs / length(_Lhs)); }

	template<size_t N, typename T> inline
		Vec_<N, T> normalize( _in(Vec_<N, T>) _Lhs )
		{ return (_Lhs / length(_Lhs)); }
	// </ normalize >

	template<typename T> inline
		T radians(_in(Vec3_<T>) _V0, _in(Vec3_<T>) _V1) 
		{
		/*
		|V0
		|
		|\ radians
		+---------------V1
		*/
		return ( dot(_V0, _V1) / ( length(_V0) * length(_V1) ) );
		}


	template<typename T> inline
		Vec2_<T> faceforward(_in(Vec2_<T>) _Normal, _in(Vec2_<T>) _I, _in(Vec2_<T>) _Nref)
		{
		return (dot(_I, _Nref) < T(0) ? _Normal : (-_Normal));
		}

	template<typename T> inline
		Vec3_<T> faceforward(_in(Vec3_<T>) _Normal, _in(Vec3_<T>) _I, _in(Vec3_<T>) _Nref)
		{
		return (dot(_I, _Nref) < T(0) ? _Normal : (-_Normal));
		}

	template<typename T> inline
		Vec4_<T> faceforward(_in(Vec4_<T>) _Normal, _in(Vec4_<T>) _I, _in(Vec4_<T>) _Nref)
		{
		return (dot(_I, _Nref) < T(0) ? _Normal : (-_Normal));
		}

	template<size_t N, typename T> inline
		Vec_<N, T> faceforward(_in(Vec_<N, T>) _Normal, _in(Vec_<N, T>) _I, _in(Vec_<N, T>) _Nref)
		{
		return (dot(_I, _Nref) < T(0) ? _Normal : (-_Normal));
		}


	template<typename T> inline 
		Vec3_<T> reflect(_in(Vec3_<T>) _Vec, _in(Vec3_<T>) _Normal)
		{
		/*
			/|
		   / 2.0f * proj(_Vec, _Normal)
		  /  |
		 /   |
		/    |
		\----|    / Result
		Vec  |   /
		  \ Norm/
		   \ | /
		____\|/_________
		*/
		return ( Real(2) * dot(_Vec, _Normal) - _Vec );
		}

	template<typename T> inline
		Vec3_<T> proj(_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Proj)
		{	// _Lhs projection to _Proj
		/*
		__________
		|        /|
		|  _Lhs/  |
		|    /	  |
		|  /	  |
		|/________|________
		| N |       N is nomalized
		|  result |
		|	 -> _Proj     |

		proj_length = (N dot _Lhs)
		proj_length = (normalize(_Proj) dot _Lhs)
		proj = proj_length * normalize(_Proj)

		proj = (_Lhs dot normalize(_Proj)) * normalize(_Proj)
		proj = (_Lhs * _Proj / length(_Proj)) dot (_Proj / length(_Proj) * _Proj / length(_Proj))
		proj = (_Lhs dot _Proj) / (length(_Proj) * length(_Proj)) * _Proj
		proj = (_Lhs dot _Proj) / dot(_Proj) * _Proj
		*/
		return ( dot(_Lhs, _Proj) / dot(_Proj, _Proj) * _Proj );
		}

	template<typename T> inline
		Vec2_<T> orthogonal(_in(Vec2_<T>) _Lhs)
		{
		/*
		Lhs dot X = cos(1.57) = 0
		Lhs.x * X.x + Lhs.y * X.y = 0
		Lhs.x * X.x = -Lhs.y * X.y

		X.x = -Lhs.y
		X.y = Lhs.x
		Lhs.x * -Lhs.y = -Lhs.y * Lhs.x
		Lhs.x * -Lhs.y + Lhs.y * Lhs.x = 0
		Lhs dot (-Lhs.y, Lhs.x) = 0
		Lhs กอ (-Lhs.y, Lhs.x)
		*/
		return (Vec2_<T>(-_Lhs.y, _Lhs.x));
		}

	template<typename T> inline
		void orthogonal( _inout( std::vector<Vec3_<T>> ) _Vectors )
		{
		assert(_Vectors.size() >= 2);

		for (size_t i = 1; i != _Vectors.size(); ++i)
			{
			for (size_t j = 0; j != i; ++j)
				{
				_Vectors[i] -= proj(_Vectors[i], _Vectors[i - 1]);
				}
			}
		}

	template<typename T, typename _Fn> inline 
		Vec2_<T> for_each( _in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs, _Fn _Func )
		{ 
		return ( Vec2_<T>( _Func(_Lhs[0], _Rhs[0]), 
						   _Func(_Lhs[1], _Rhs[1]) ) ); 
		}

	template<typename T, typename _Fn> inline 
		Vec3_<T> for_each( _in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs, _Fn _Func)
		{ 
		return ( Vec3_<T>( _Func(_Lhs[0], _Rhs[0]), 
						   _Func(_Lhs[1], _Rhs[1]), 
						   _Func(_Lhs[2], _Rhs[2]) ) ); 
		}

	template<typename T, typename _Fn> inline 
		Vec4_<T> for_each( _in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs, _Fn _Func)
		{
		return ( Vec4_<T>( _Func(_Lhs[0], _Rhs[0]), 
						   _Func(_Lhs[1], _Rhs[1]), 
						   _Func(_Lhs[2], _Rhs[2]), 
						   _Func(_Lhs[3], _Rhs[3]) ) ); 
		}

	template<size_t N, typename T, typename _Fn> inline 
		Vec_<N, T> for_each(_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs, _Fn _Func)
		{
		Vec_<N, T> _Result;
		for (size_t i = 0; i != N; ++i)
			{
			_Result[i] = _Func( _Lhs[i], _Rhs[i] );
			}
		return (_Result);
		}
	

	template<typename T, typename T2> inline 
		Vec2_<T> type_cast(_in(Vec2_<T2>) _Src) 
		{
		return ( Vec2_<T>(static_cast<T>(_Src.x), static_cast<T>(_Src.y)) );
		}

	template<typename T, typename T2> inline 
		Vec3_<T> type_cast(_in(Vec3_<T2>) _Src) 
		{
		return ( Vec3_<T>(static_cast<T>(_Src.x), static_cast<T>(_Src.y), static_cast<T>(_Src.z)) );
		}

	template<typename T, typename T2> inline 
		Vec4_<T> type_cast(_in(Vec4_<T2>) _Src) 
		{
		return ( Vec3_<T>(static_cast<T>(_Src.x), static_cast<T>(_Src.y), static_cast<T>(_Src.z), static_cast<T>(_Src.w)) );
		}

	template<typename T, typename T2, size_t N> inline
		Vec_<N, T> type_cast(_in(Vec_<N, T2>) _Val)
		{
		Vec_<N, T> _Result;

		for (size_t i = 0; i != N; ++i)
			_Result[i] = static_cast<T>(_Val[i]);

		return (_Result);
		}


	template<typename T, size_t N> inline 
		bool operator< (const Vec_<N, T>& _Lhs, const Vec_<N, T>& _Rhs) 
		{
		for (size_t i = 0; i != N; ++i )
			if ( _Lhs[i] >= _Rhs[i] )
				return ( false );
		return ( true );
		}

	template<typename T, size_t N> inline
		bool operator> (const Vec_<N, T>& _Lhs, const Vec_<N, T>& _Rhs) 
		{
		for (size_t i = 0; i != N; ++i)
			if ( _Lhs[i] <= _Rhs[i] )
				return ( false );
		return ( true );
		}

	template<typename T, size_t N> inline 
		bool operator>= (const Vec_<N, T>& _Lhs, const Vec_<N, T>& _Rhs)
		{
		for (size_t i = 0; i != N; ++i)
			if ( _Lhs[i] < _Rhs[i] )
				return ( false );
		return ( true );
		}

	template<typename T, size_t N> inline
		bool operator<= (const Vec_<N, T>& _Lhs, const Vec_<N, T>& _Rhs) 
		{
		for (size_t i = 0; i != N; ++i)
			if (_Lhs[i] > _Rhs[i])
				return ( false );
		return ( true );
		}

	template<typename T, size_t N> inline 
		bool operator== (const Vec_<N, T>& _Lhs, const Vec_<N, T>& _Rhs) 
		{
		for (size_t i = 0; i != N; ++i)
			if ( _Lhs[i] != _Rhs[i] )
				return ( false );
		return ( true );
		}

}// namespace clmagic

#endif