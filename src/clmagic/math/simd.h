#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH___SIMD___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH___SIMD___H__
#include "clmagic.h"
#include <iostream>

#include <ivec.h>
#include <fvec.h>
#include <dvec.h>

#define _ALIGN16 _declspec(align(16))

inline 
float simd_dot(_in(F32vec4) _A, _in(F32vec4) _B) {
	auto _C = _A * _B;
	return ( _C[0] + _C[1] + _C[2] + _C[3] );
}

class F32mat4 
{
protected:
	F32vec4 _Mydata[4];
public:
	F32mat4() : _Mydata() { }

	F32mat4(_in(F32vec4) _Row0, _in(F32vec4) _Row1, _in(F32vec4) _Row2, _in(F32vec4) _Row3)
		: _Mydata{ _Row0, _Row1, _Row2, _Row3 } 
		{
		}

	F32mat4(float _00, float _01, float _02, float _03, 
		float _10, float _11, float _12, float _13,
		float _20, float _21, float _22, float _23, 
		float _30, float _31, float _32, float _33)
		: _Mydata{ F32vec4(_00, _01, _02, _03),
					F32vec4(_10, _11, _12, _13),
					F32vec4(_20, _21, _22, _23),
					F32vec4(_30, _31, _32, _33) }
		{	// ...
		}

	friend F32mat4 operator+(_in(F32mat4) _A, _in(F32mat4) _B) {
		return ( F32mat4(_A[0] + _B[0], _A[1] + _B[1], _A[2] + _B[2], _A[3] + _B[3]) );
	}

	friend F32mat4 operator-(_in(F32mat4) _A, _in(F32mat4) _B) {
		return (F32mat4(_A[0] - _B[0], _A[1] - _B[1], _A[2] - _B[2], _A[3] - _B[3]));
	}

	friend F32vec4 operator*(_in(F32mat4) _A, _in(F32vec4) _B) {
		return (F32vec4(simd_dot(_A[0], F32vec4(_B[0])),
						simd_dot(_A[1], F32vec4(_B[1])),
						simd_dot(_A[2], F32vec4(_B[2])),
						simd_dot(_A[3], F32vec4(_B[3]))));
	}

	friend F32mat4 operator*(_in(F32mat4) _A, float _B) {
		auto _Tmp = F32vec4(_B);
		return ( F32mat4(_A[0] * _Tmp, _A[1] * _Tmp, _A[2] * _Tmp, _A[3] * _Tmp) );
	}

	friend F32mat4 operator*(_in(F32mat4) _A, _in(F32mat4) _B) {
		/*
			 [ 0 1 2 3 ]  [ 0       ]     [ 0        ]
		dot( [ ...     ], [ 1       ] ) = [ ...      ]
			 [ ...     ]  [ 2       ]     [ ...      ]
			 [ ...     ]  [ 3       ]     [ ...      ]
		*/
		auto _Col0 = F32vec4(_B[0][0], _B[1][0], _B[2][0], _B[3][0]);
		auto _Col1 = F32vec4(_B[0][1], _B[1][1], _B[2][1], _B[3][1]);
		auto _Col2 = F32vec4(_B[0][2], _B[1][2], _B[2][2], _B[3][2]);
		auto _Col3 = F32vec4(_B[0][3], _B[1][3], _B[2][3], _B[3][3]);
		return ( F32mat4( simd_dot(_A[0], _Col0), simd_dot(_A[0], _Col1), simd_dot(_A[0], _Col2), simd_dot(_A[0], _Col3),
					      simd_dot(_A[1], _Col0), simd_dot(_A[1], _Col1), simd_dot(_A[1], _Col2), simd_dot(_A[1], _Col3), 
						  simd_dot(_A[2], _Col0), simd_dot(_A[2], _Col1), simd_dot(_A[2], _Col2), simd_dot(_A[2], _Col3), 
						  simd_dot(_A[3], _Col0), simd_dot(_A[3], _Col1), simd_dot(_A[3], _Col2), simd_dot(_A[3], _Col3)) );
	}

	F32vec4& operator[](size_t _Pos) {
		return (_Mydata[_Pos]);
	}

	const F32vec4& operator[]( size_t _Pos ) const {
		return ( _Mydata[_Pos] );
	}

};// class F32mat4


namespace clmagic {

	template<typename _Ty>
		constexpr bool is_simd_v = std::_Is_any_of_v< 
			std::remove_cv_t<_Ty>, 
			float, 
			double, 
			int, 
			unsigned int,
			short, 
			unsigned short>;

	template<typename _Ty = float>
	struct _SIMD_vec4_data {
		F32vec4 _Mydata;
		using value_type = _Ty;
		using vector_type = F32vec4;

		_SIMD_vec4_data() { }
		_SIMD_vec4_data(value_type _All) : _Mydata(_All, _All, _All, _All) { }
		_SIMD_vec4_data(value_type _Val0, value_type _Val1, value_type _Val2, value_type _Val3) : _Mydata(_Val3, _Val2, _Val1, _Val0) { }
		_SIMD_vec4_data(_in(vector_type) _Vec) : _Mydata(_Vec) { }
	};

	template<>
	struct _SIMD_vec4_data<double> {
		F64vec4 _Mydata;
		using value_type = double;
		using vector_type = F64vec4;

		_SIMD_vec4_data() { }
		_SIMD_vec4_data(value_type _All) : _Mydata(_All, _All, _All, _All) { }
		_SIMD_vec4_data(value_type _Val0, value_type _Val1, value_type _Val2, value_type _Val3) : _Mydata(_Val3, _Val2, _Val1, _Val0) { }
		_SIMD_vec4_data(_in(vector_type) _Vec) : _Mydata(_Vec) { }
	};

	template<>
	struct _SIMD_vec4_data<int> {
		I32vec4 _Mydata;
		using value_type = int;
		using vector_type = I32vec4;

		_SIMD_vec4_data() { }
		_SIMD_vec4_data(value_type _All) : _Mydata(_All, _All, _All, _All) { }
		_SIMD_vec4_data(value_type _Val0, value_type _Val1, value_type _Val2, value_type _Val3) : _Mydata(_Val3, _Val2, _Val1, _Val0) { }
		_SIMD_vec4_data(_in(vector_type) _Vec) : _Mydata(_Vec) { }
	};

	template<>
	struct _SIMD_vec4_data<unsigned int> {
		Iu32vec4 _Mydata;
		using value_type = unsigned int;
		using vector_type = Iu32vec4;

		_SIMD_vec4_data() { }
		_SIMD_vec4_data(value_type _All) : _Mydata(_All, _All, _All, _All) { }
		_SIMD_vec4_data(value_type _Val0, value_type _Val1, value_type _Val2, value_type _Val3) : _Mydata(_Val3, _Val2, _Val1, _Val0) { }
		_SIMD_vec4_data(_in(vector_type) _Vec) : _Mydata(_Vec) { }
	};

	template<>
	struct _SIMD_vec4_data<short> {
		Is16vec4 _Mydata;
		using value_type = short;
		using vector_type = Is16vec4;

		_SIMD_vec4_data() { }
		_SIMD_vec4_data(value_type _All) : _Mydata(_All, _All, _All, _All) { }
		_SIMD_vec4_data(value_type _Val0, value_type _Val1, value_type _Val2, value_type _Val3) : _Mydata(_Val0, _Val1, _Val2, _Val3) { }
		_SIMD_vec4_data(_in(vector_type) _Vec) : _Mydata(_Vec) { }
	};
	
	template<>
	struct _SIMD_vec4_data<unsigned short> {
		Iu16vec4 _Mydata;
		using value_type = unsigned short;
		using vector_type = Iu16vec4;

		_SIMD_vec4_data() { }
		_SIMD_vec4_data(value_type _All) : _Mydata(_All, _All, _All, _All){ }
		_SIMD_vec4_data(value_type _Val0, value_type _Val1, value_type _Val2, value_type _Val3) : _Mydata(_Val0, _Val1, _Val2, _Val3) { }
		_SIMD_vec4_data(_in(vector_type) _Vec) : _Mydata(_Vec) { }
	};


	/*
	@_Note: only contains stl-simd operation
	*/
	template<typename _Ty = float>
	struct SIMDVec4_ : public _SIMD_vec4_data<_Ty> {
		static_assert( is_simd_v<_Ty>, "not support SIMD type" );

		using _Mybase     = _SIMD_vec4_data<_Ty>;
		using value_type  = typename _Mybase::value_type;
		using vector_type = typename _Mybase::vector_type;

		SIMDVec4_() { }
		SIMDVec4_(value_type _All) : _Mybase(_All) { }
		SIMDVec4_(value_type _Val0, value_type _Val1, value_type _Val2, value_type _Val3) : _Mybase(_Val0, _Val1, _Val2, _Val3) { }
		SIMDVec4_(_in(vector_type) _Vec) : _Mybase(_Vec) { }

		explicit operator vector_type() const { return ( _Mybase::_Mydata ); }

		vector_type mmxxx_vector() const { return ( vector_type(*this) ); }

		      _Ty& operator[](size_t _Pos)       { return ( _Mybase::_Mydata[_Pos] ); }
		const _Ty& operator[](size_t _Pos) const { return ( _Mybase::_Mydata[_Pos] ); }

		      _Ty* ptr()       { return ((_Ty*)(_Mybase::_Mydata)); }
		const _Ty* ptr() const { return ((const _Ty*)(_Mybase::_Mydata)); }

		friend SIMDVec4_<_Ty> operator&(_in(SIMDVec4_<_Ty>) _A, _in(SIMDVec4_<_Ty>) _B) { return (SIMDVec4_(_A._Mydata & _B._Mydata) ); }
		friend SIMDVec4_<_Ty> operator|(_in(SIMDVec4_<_Ty>) _A, _in(SIMDVec4_<_Ty>) _B) { return (SIMDVec4_(_A._Mydata | _B._Mydata) ); }
		friend SIMDVec4_<_Ty> operator^(_in(SIMDVec4_<_Ty>) _A, _in(SIMDVec4_<_Ty>) _B) { return (SIMDVec4_(_A._Mydata ^ _B._Mydata) ); }
		friend SIMDVec4_<_Ty> operator+(_in(SIMDVec4_<_Ty>) _A, _in(SIMDVec4_<_Ty>) _B) { return (SIMDVec4_(_A._Mydata + _B._Mydata) ); }
		friend SIMDVec4_<_Ty> operator-(_in(SIMDVec4_<_Ty>) _A, _in(SIMDVec4_<_Ty>) _B) { return (SIMDVec4_(_A._Mydata - _B._Mydata) ); }
		friend SIMDVec4_<_Ty> operator*(_in(SIMDVec4_<_Ty>) _A, _in(SIMDVec4_<_Ty>) _B) { return (SIMDVec4_(_A._Mydata * _B._Mydata) ); }
		friend SIMDVec4_<_Ty> operator/(_in(SIMDVec4_<_Ty>) _A, _in(SIMDVec4_<_Ty>) _B) { return (SIMDVec4_(_A._Mydata / _B._Mydata) ); }

		SIMDVec4_<_Ty>& operator+=(_in(SIMDVec4_<_Ty>) _B) { _Mybase::_Mydata += _B._Mydata; return (*this); }
		SIMDVec4_<_Ty>& operator-=(_in(SIMDVec4_<_Ty>) _B) { _Mybase::_Mydata -= _B._Mydata; return (*this); }
		SIMDVec4_<_Ty>& operator*=(_in(SIMDVec4_<_Ty>) _B) { _Mybase::_Mydata *= _B._Mydata; return (*this); }
		SIMDVec4_<_Ty>& operator/=(_in(SIMDVec4_<_Ty>) _B) { _Mybase::_Mydata /= _B._Mydata; return (*this); }
		SIMDVec4_<_Ty>& operator&=(_in(SIMDVec4_<_Ty>) _B) { _Mybase::_Mydata &= _B._Mydata; return (*this); }
		SIMDVec4_<_Ty>& operator|=(_in(SIMDVec4_<_Ty>) _B) { _Mybase::_Mydata |= _B._Mydata; return (*this); }
		SIMDVec4_<_Ty>& operator^=(_in(SIMDVec4_<_Ty>) _B) { _Mybase::_Mydata ^= _B._Mydata; return (*this); }

		friend std::ostream& operator<<(_inout(std::ostream) _Ostr, _in(SIMDVec4_<_Ty>) _Vec) {
			_Ostr << _Vec._Mydata[0] << "," << _Vec._Mydata[1] << "," << _Vec._Mydata[2] << "," << _Vec._Mydata[3];
			return (_Ostr);
		}
	};	

	/* Many operators are missing */
	using SIMDVec4i = SIMDVec4_<int>;

	using SIMDVec4f = SIMDVec4_<float>;
	using SIMDVec4d = SIMDVec4_<double>;
	using SIMDVec4  = SIMDVec4f;


	template<typename _Ty> inline
	_Ty simd_dot(_in(SIMDVec4_<_Ty>) _A, _in(SIMDVec4_<_Ty>) _B) {
		auto _C = _A * _B;
		return (_C[0] + _C[1] + _C[2] + _C[3]);
	}

	template<typename _Ty> inline
		SIMDVec4_<_Ty> simd_cross3( _in(SIMDVec4_<_Ty>) _A, _in(SIMDVec4_<_Ty>) _B ) {
		/*
		A: [y]   [z],   [z]   [x],   [x]   [y]
		   [*] - [*]    [*] - [*]    [*] - [*]
		B: [z]   [y],   [x]   [z],   [y]   [x]

		1. A:[1][2][0] * B:[2][0][1]
		3.          substract
		2. A:[2][0][1] * B:[1][2][0]
		*/
		std::cout << "undefined" << std::endl;
		abort();
	}

	template<> inline
		SIMDVec4_<float> simd_cross3( _in(SIMDVec4_<float>) _A, _in(SIMDVec4_<float>) _B )
		{
		auto _Lhs = _mm_shuffle_ps(_A._Mydata, _A._Mydata, _MM_SHUFFLE(3, 0, 2, 1));
		auto _Rhs = _mm_shuffle_ps(_B._Mydata, _B._Mydata, _MM_SHUFFLE(3, 1, 0, 2));
		auto _Result = _mm_mul_ps(_Lhs, _Rhs);

		_Lhs = _mm_shuffle_ps(_Lhs, _Lhs, _MM_SHUFFLE(3, 0, 2, 1));
		_Rhs = _mm_shuffle_ps(_Rhs, _Rhs, _MM_SHUFFLE(3, 1, 0, 2));
		_Result = _mm_sub_ps(_Result, _mm_mul_ps(_Lhs, _Rhs));

		return (SIMDVec4_<float>(_Result) );
		}



	template<> inline
		SIMDVec4_<double> simd_cross3(_in(SIMDVec4_<double>) _A, _in(SIMDVec4_<double>) _B)
		{
		auto _Lhs = _mm256_set_pd(_A[0], _A[0], _A[2], _A[1]);
		auto _Rhs = _mm256_set_pd(_B[0], _B[1], _B[0], _B[2]);
		auto _Result = _mm256_mul_pd(_Lhs, _Rhs);
		_Lhs = _mm256_set_pd(_A[0], _A[1], _A[0], _A[2]);
		_Rhs = _mm256_set_pd(_B[0], _B[0], _B[2], _B[1]);
		_Result = _mm256_sub_pd(_Result, _mm256_mul_pd(_Lhs, _Rhs));

		return (SIMDVec4_<double>(_Result) );
		}

}// namespace clmagic

#endif