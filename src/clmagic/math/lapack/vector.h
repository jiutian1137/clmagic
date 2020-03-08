//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef clmagic_math_lapack_VECTOR_h_
#define clmagic_math_lapack_VECTOR_h_
#include "../general/clmagic.h"// include is_*<>, shuffle<>
#include "../general/general.h"// include abs() minval() maxval()
#include "../general/simd.h"// SIMD
#include "../real/real.h"// include CSTD math
#include <string>
#include <algorithm>

namespace clmagic {

	struct normal_vector_tag {};
	struct simd128_vector_tag {};
	struct simd256_vector_tag {};
	struct simd512_vector_tag {};


#define vecN vec_<_Ty, _Size>
	/*
	@_vec_<N, T>: VectorN for type of T
	@_Example:
		constexpr vec_<3, float> _Position( 1.0f, 2.0f, 10.0f );
		vec_<2, std::string> _Fullname = { std::string(), std::string() };
		vec_<9, int>         _State    = { 0, 0, 0, 0, 0, 1, 0, 0, 0 };
		vec_<5, joint>       _Actorjoint;
		...
	@_Convert: vec_<3, float> _Vec0;
		simd_vec_:   simd_vec_<3, float>(_Vec0.begin(), _Vec0.end());
		std::string: clmagic::to_string(_Vec0);
		pointer:      _Vec0.ptr();
	*/
	template<typename _Ty, size_t _Size>
	struct vec_ {
		static_assert(_Size >= 1, "vec_<> size error");

		using scalar_type          = _Ty;/* !!!important */
		using vector_type          = vec_<_Ty, _Size>;
		using scalar_pointer       = scalar_type*;
		using scalar_const_pointer = scalar_type const*;
		
		using iterator       = typename _STD _Array_iterator<_Ty, _Size>;
		using const_iterator = typename _STD _Array_const_iterator<_Ty, _Size>;

		static constexpr size_t size() {
			return _Size; }
		static constexpr size_t align() {
			return 1; }
	
		void assign(const scalar_type& _Val);
		void assign(scalar_const_pointer _Ptr);
		void assign(_STD initializer_list<scalar_type> _Ilist);
		template<typename _Iter> void assign(_Iter _First, _Iter _Last);
		template<typename _Fn> void assign(const vecN& _Left, _Fn _Func);
		template<typename _Fn> void assign(const vecN& _Left, const vecN& _Right, _Fn _Func);
		template<typename _Fn> void assign(const vecN& _Left, const scalar_type& _Right, _Fn _Func);
		template<typename _Fn> void assign(const scalar_type& _Left, const vecN& _Right, _Fn _Func);

		constexpr vec_() : _Mydata{ 0 } { }
		explicit vec_(const scalar_type& _Val);
		explicit vec_(scalar_const_pointer _Ptr);
		vec_(_STD initializer_list<scalar_type> _Ilist);
		template<typename _Iter> vec_(_Iter _First, _Iter _Last);
		template<typename _Fn> vec_(const vecN& _Left, _Fn _Func);
		template<typename _Fn> vec_(const vecN& _Left, const vecN& _Right, _Fn _Func);
		template<typename _Fn> vec_(const vecN& _Left, const scalar_type& _Right, _Fn _Func);
		template<typename _Fn> vec_(const scalar_type& _Left, const vecN& _Right, _Fn _Func);

		constexpr scalar_pointer ptr(size_t _Pos = 0) {
			return (_Mydata + _Pos); }
		constexpr scalar_const_pointer ptr(size_t _Pos = 0) const {
			return (_Mydata + _Pos); }
		constexpr scalar_type& operator[](size_t _Pos) {
			return _Mydata[_Pos]; }
		constexpr const scalar_type& operator[](size_t _Pos) const {
			return _Mydata[_Pos]; }

		constexpr iterator begin() {
			return iterator(_Mydata, 0); }
		constexpr iterator end() {
			return iterator(_Mydata, this->size()); }
		constexpr const_iterator begin() const {
			return const_iterator(_Mydata, 0); }
		constexpr const_iterator end() const {
			return const_iterator(_Mydata, this->size()); }

		template<size_t _Size2> vec_<_Ty, _Size2>& prefix();
		template<size_t _Size2> vec_<_Ty, _Size2>& posfix();
		template<typename ..._Tys> 
		auto shuffle(_Tys... _Selector) const {
			using _OutTy = vec_<_Ty, types_size_v<_Tys...>>;
			return clmagic::shuffle<_OutTy>(*this, _Selector...);
		}

		vecN operator-() const;
		vecN operator+(const vecN& _Right) const;
		vecN operator-(const vecN& _Right) const;
		vecN operator*(const vecN& _Right) const;
		vecN operator/(const vecN& _Right) const;
		vecN operator+(const scalar_type& _Right) const;
		vecN operator-(const scalar_type& _Right) const;
		vecN operator*(const scalar_type& _Right) const;
		vecN operator/(const scalar_type& _Right) const;
		vecN& operator+=(const vecN& _Right);
		vecN& operator-=(const vecN& _Right);
		vecN& operator*=(const vecN& _Right);
		vecN& operator/=(const vecN& _Right);
		vecN& operator+=(const scalar_type& _Right);
		vecN& operator-=(const scalar_type& _Right);
		vecN& operator*=(const scalar_type& _Right);
		vecN& operator/=(const scalar_type& _Right);
		scalar_type sum() const;
		scalar_type product() const;
		scalar_type normL1() const;
		scalar_type normL2_sq() const;
		scalar_type normL2() const;
		scalar_type norm(size_t p) const;
		vecN& normalize();

	private:
		_Ty _Mydata[_Size];
	};// struct vec_<>

	template<size_t _SIze> using ivec = vec_<int32_t, _SIze>;
	template<size_t _SIze> using uvec = vec_<uint32_t, _SIze>;
	template<size_t _SIze> using bvec = vec_<bool, _SIze>;
	template<size_t _SIze> using fvec = vec_<float, _SIze>;
	template<size_t _SIze> using dvec = vec_<double, _SIze>;

#define SCALAR_TYPE typename vec_<_Ty, _Size>::scalar_type

	template<typename _Ty, size_t _Size>
		_STD string to_string(const vecN& _X);
	template<typename _Ty, size_t _Size>
		_STD ostream& operator<<(_STD ostream& _Ostr, const vecN& _X);

	template<typename _Ty, size_t _Size>
		vec_<bool, _Size> operator==(const vecN& _Left, const vecN& _Right);
	template<typename _Ty, size_t _Size>
		vec_<bool, _Size> operator!=(const vecN& _Left, const vecN& _Right);
	template<typename _Ty, size_t _Size>
		vec_<bool, _Size> operator<(const vecN& _Left, const vecN& _Right);
	template<typename _Ty, size_t _Size>
		vec_<bool, _Size> operator>(const vecN& _Left, const vecN& _Right);
	template<typename _Ty, size_t _Size>
		vec_<bool, _Size> operator<=(const vecN& _Left, const vecN& _Right);
	template<typename _Ty, size_t _Size>
		vec_<bool, _Size> operator>=(const vecN& _Left, const vecN& _Right);

	template<size_t _Size>
		bool all_of(const vec_<bool, _Size>& _X);
	template<size_t _Size>
		bool any_of(const vec_<bool, _Size>& _X);
	template<size_t _Size>
		bool none_of(const vec_<bool, _Size>& _X);

	// extend operation
	template<typename _Ty, size_t _Size>
		vecN mod(const vecN& _Left, const vecN& _Right);
	template<typename _Ty, size_t _Size>
		vecN pow(const vecN& _Left, const vecN& _Right);
	template<typename _Ty, size_t _Size>
		vecN vmin(const vecN& _Left, const vecN& _Right);
	template<typename _Ty, size_t _Size>
		vecN vmax(const vecN& _Left, const vecN& _Right);
	template<typename _Ty, size_t _Size>
		vecN cross3(const vecN& _Left, const vecN& _Right);
	template<typename _Ty, size_t _Size>
		SCALAR_TYPE dot(const vecN& _Left, const vecN& _Right);

	template<typename _Ty, size_t _Size>
		vecN mod(const vecN& _Left, const SCALAR_TYPE& _Right);
	template<typename _Ty, size_t _Size>
		vecN pow(const vecN& _Left, const SCALAR_TYPE& _Right);// why not have pow(scalar, vector)? the function should is EXP_()

	// scalar ? vector
	template<typename _Ty, size_t _Size>
		vecN operator+(const SCALAR_TYPE& _Left, const vecN& _Right);
	template<typename _Ty, size_t _Size>
		vecN operator-(const SCALAR_TYPE& _Left, const vecN& _Right);
	template<typename _Ty, size_t _Size>
		vecN operator*(const SCALAR_TYPE& _Left, const vecN& _Right);
	template<typename _Ty, size_t _Size>
		vecN operator/(const SCALAR_TYPE& _Left, const vecN& _Right);
	template<typename _Ty, size_t _Size>
		vecN mod(const SCALAR_TYPE& _Left, const vecN& _Right);

	template<typename _Ty, size_t _Size>
		vecN abs(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN floor(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN ceil(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN trunc(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN round(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN sqrt(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN cbrt(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN invsqrt(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN invcbrt(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN log(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN log2(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN log10(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN exp(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN exp2(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN exp10(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN log1p(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN expm1(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN erf(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN erfc(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN erfinv(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN erfcinv(const vecN& _X);

	template<typename _Ty, size_t _Size>
		vecN hypot(const vecN& _A, const vecN& _B);/* sqrt(_A², _B²) */
	template<typename _Ty, size_t _Size>
		vecN sin(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN cos(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN tan(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN sincos(vecN& _Cos, const vecN& _X);// return sin
	template<typename _Ty, size_t _Size>
		vecN asin(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN acos(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN atan(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN atan2(vecN& _Y, const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN sinh(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN cosh(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN tanh(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN asinh(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN acosh(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN atanh(const vecN& _X);

	template<typename _Ty, size_t _Size>
		SCALAR_TYPE length(const vecN& _X);
	template<typename _Ty, size_t _Size>
		vecN normalize(const vecN& _X);

#undef SCALAR_TYPE


	template<typename _Ty>
		constexpr bool is_vector_v = false;
	template<typename _Ty, size_t _Size>
		constexpr bool is_vector_v<vec_<_Ty, _Size>> = true;
#ifdef clmagic_math_SIMD_h_
	template<typename _Ty, size_t _Size>
		constexpr bool is_vector_v<simd_vec_<_Ty, _Size>> = true;
#endif

	template<typename _Ty, size_t _Size>
		constexpr bool is_support_scalar_operator<vec_<_Ty, _Size>> = true;
#ifdef clmagic_math_SIMD_h_
	template<typename _Ty, size_t _Size>
		constexpr bool is_support_scalar_operator<simd_vec_<_Ty, _Size>> = true;
#endif


	/*
	@_Effect: ①Switch vector type, ②enum is simd vector
	*/
	template<typename _Ty, size_t _Size, typename _Vtag>// default normal_vector_tag
	struct _Vector_traits {
		using vector_type = vec_<_Ty, _Size>;
	};

#ifdef clmagic_math_SIMD_h_
#define SIMD_VECTOR_TRAITS(TYPE, SIMD, TAG) \
template<size_t _Size>                       \
struct _Vector_traits<##TYPE##, _Size, ##TAG##> { \
	using vector_type = simd_vec_<##SIMD##, constexpr_align_div(_Size, simd_traits<##SIMD##>::scalar_align)>; \
};

	SIMD_VECTOR_TRAITS(float,   __m128,  simd128_vector_tag)
	SIMD_VECTOR_TRAITS(double,  __m128d, simd128_vector_tag)
	SIMD_VECTOR_TRAITS(int32_t, __m128i, simd128_vector_tag)
	SIMD_VECTOR_TRAITS(float,   __m256,  simd256_vector_tag)
	SIMD_VECTOR_TRAITS(double,  __m256d, simd256_vector_tag)
#undef SIMD_VECTOR_TRAITS
#endif

#define vectorN vector<_Ty, _Size, _Vtag>
	template<typename _Ty, size_t _Size, typename _Vtag = normal_vector_tag>
		using vector = typename _Vector_traits<_Ty, _Size, _Vtag>::vector_type;
	template<typename _Ty, typename _Vtag = normal_vector_tag>
		using vector2 = typename _Vector_traits<_Ty, 2, _Vtag>::vector_type;
	template<typename _Ty, typename _Vtag = normal_vector_tag>
		using vector3 = typename _Vector_traits<_Ty, 3, _Vtag>::vector_type;
	template<typename _Ty, typename _Vtag = normal_vector_tag>
		using vector4 = typename _Vector_traits<_Ty, 4, _Vtag>::vector_type;

	template<typename _Ty, size_t _Size, typename _Vtag = normal_vector_tag>
		using point = vector<_Ty, _Size, _Vtag>;
	template<typename _Ty, typename _Vtag = normal_vector_tag>
		using point2 = vector<_Ty, 2, _Vtag>;
	template<typename _Ty, typename _Vtag = normal_vector_tag>
		using point3 = vector<_Ty, 3, _Vtag>;
	template<typename _Vtag = normal_vector_tag>
		using ipoint2 = vector<int32_t, 2, _Vtag>;
	template<typename _Vtag = normal_vector_tag>
		using upoint2 = vector<uint32_t, 2, _Vtag>;
	using point2_size_t = point2<size_t, normal_vector_tag>;


	/*
	@_unit_vector<T>: as vector3<T>
	@_Traits: length = 1.0
	*/
	template<typename _Ty, size_t _Size, typename _Vtag = normal_vector_tag>
	struct unit_vector : public vector<_Ty, _Size, _Vtag> {
		using _Mybase     = vector<_Ty, _Size, _Vtag>;
		using scalar_type = _Ty;
		using vector_type = unit_vector<_Ty, _Size, _Vtag>;

		unit_vector() = default;

		unit_vector(_STD initializer_list<scalar_type> _Ilist, bool _Unitized = false)
			: _Mybase(_Ilist) {
			if (!_Unitized) this->normalize();
		}

		unit_vector(const _Mybase& _Vector, bool _Unitized = false)
			: _Mybase(_Unitized ? _Vector : normalize(_Vector)) {
			// 
		}

		vector_type operator-() const {
			return vector_type(std::negate<>(*this), true);
		}
	};

#define unit_vectorN unit_vector<_Ty, _Size, _Vtag>
	template<typename _Ty = real_t, typename _Vtag = normal_vector_tag>
		using unit_vector3 = unit_vector<_Ty, 3, _Vtag>;


	/*template<typename _Ty, size_t _Size, typename _Vtag> inline
	auto distance(const vector<_Ty, _Size, _Vtag>& _A, const vector<_Ty, _Size, _Vtag>& _B) 
		-> decltype(length(_B - _A)) {
		return length(_B - _A);
	}*/

	//template<size_t N, typename T> inline
	//	T radians(_in(vector<N, T>) _A, _in(vector<N, T>) _B) 
	//	{	// return dot(A,B)÷|A|÷|B|
	//	return (dot(_A, _B) / _A.length() / _B.length());
	//	}

	template<typename _Ty, size_t _Size, typename _Vtag> inline
	unit_vectorN faceforward(const unit_vectorN& _Normal, const vectorN& _I, const vectorN& _Nref) {// dot(_I,_Nref) < 0 ? N : -N
		return (dot(_I, _Nref) < static_cast<_Ty>(0) ? _Normal : -_Normal);
	}

	template<typename _Ty, size_t _Size, typename _Vtag> inline
	vectorN proj(const vectorN& _Vector, const vectorN& _Proj) {// return dot(V,||_Proj||) * |Proj| 
		return (dot(_Vector, _Proj) / dot(_Proj, _Proj) * _Proj);
		/*
			|          / |
			|_Vector/
			|    /	     |
			| /
			|-> result<--|▜___________
			| N |       N is n|malized
			| ->   _Proj   <- |

			proj_length = (N dot _Lhs)
			proj_length = (normalize(_Proj) dot _Lhs)
			proj = proj_length * normalize(_Proj)

			proj = (_Lhs dot normalize(_Proj)) * normalize(_Proj)
			proj = (_Lhs * _Proj / length(_Proj)) dot (_Proj / length(_Proj) * _Proj / length(_Proj))
			proj = (_Lhs * _Proj / length(_Proj)) dot (_Proj^2 /length(_Proj)^2)
			proj = (_Lhs dot _Proj) / dot(_Proj) * _Proj
		*/
	}

	template<typename _Ty, size_t _Size, typename _Vtag> inline
	vectorN proj(const vectorN& _Vector, const unit_vectorN& _Proj) {// return dot(V,N)*N
		return (dot(_Vector, _Proj) * _Proj);
	}

	template<typename _Ty, typename _Vtag> inline
	vector3<_Ty, _Vtag> proj(const vector3<_Ty, _Vtag>& _Vector, const unit_vector3<_Ty, _Vtag>& _Rt, 
		const unit_vector3<_Ty, _Vtag>& _Up, const unit_vector3<_Ty, _Vtag>& _Fwd) {// return [dot3(V,r), dot(V,u), dot(V,f)]
		return vector3<_Ty, _Vtag>{ dot(_Vector, _Rt), dot(_Vector, _Up), dot(_Vector, _Fwd) };
	}

	template<typename _Ty, size_t _Size, typename _Vtag> inline
	vectorN reflect(const vectorN& _Direction, const unit_vectorN& _Normal) {
		return (static_cast<_Ty>(2) * proj(_Direction, _Normal) - _Direction);
		/*
			/|\
		   / 2.0f * proj(Dir, _Normal)
		  /  |  \
		 /   |
		/____|    \
		\    |    / Result
		Dir  |   /
		  \ Norm/
		   \ | /
		____\|/_________
		*/
	}

	template<typename T>
	vector2<T> perp(_in(vector2<T>) _Lhs) {
		vector2<T>(-_Lhs[1], _Lhs[0]);
	}

	template<typename T>
	void orthogonal(_inout(_STD vector<vector3<T>>) _Vectors) {
		for (size_t i = 1; i != _Vectors.size(); ++i) {
			for (size_t j = 0; j != i; ++j) {
				_Vectors[i] -= proj(_Vectors[i], _Vectors[j]);
			}
		}
	}



	template<typename _Ty, size_t _Size> inline
	void vec_<_Ty, _Size>::assign(const scalar_type& _Val) {
		std::fill(this->begin(), this->end(), _Val);
	}

	template<typename _Ty, size_t _Size> inline
	void vec_<_Ty, _Size>::assign(scalar_const_pointer _Ptr) {
		std::copy(_Ptr, _Ptr + _Size, this->begin());
	}

	template<typename _Ty, size_t _Size> inline
	void vec_<_Ty, _Size>::assign(_STD initializer_list<scalar_type> _Ilist) {
		assert(_Ilist.size() <= this->size());
		std::copy(_Ilist.begin(), _Ilist.end(), this->begin());
	}

	template<typename _Ty, size_t _Size> template<typename _Iter> inline 
	void vec_<_Ty, _Size>::assign(_Iter _First, _Iter _Last) {
		assert( std::distance(_First, _Last) <= static_cast<std::_Iter_diff_t<_Iter>>(this->size()) );
		std::copy(_First, _Last, this->begin());
	}

	template<typename _Ty, size_t _Size> template<typename _Fn> inline 
	void vec_<_Ty, _Size>::assign(const vecN& _Left, _Fn _Func) {
		std::transform(_Left.begin(), _Left.end(), this->begin(), _Func);
	}

	template<typename _Ty, size_t _Size> template<typename _Fn> inline
	void vec_<_Ty, _Size>::assign(const vecN& _Left, const vecN& _Right, _Fn _Func) {
		std::transform(_Left.begin(), _Left.end(), _Right.begin(), this->begin(), _Func);
	}

	template<typename _Ty, size_t _Size> template<typename _Fn> inline
	void vec_<_Ty, _Size>::assign(const vecN& _Left, const scalar_type& _Right, _Fn _Func) {
		auto _First1 = _Left.begin();
		auto _Last1  = _Left.end();
		auto _Dest   = this->begin();
		for (; _First1 != _Last1; ++_First1, ++_Dest) {
			*_Dest = _Func(*_First1, _Right);
		}
	}

	template<typename _Ty, size_t _Size> template<typename _Fn> inline
	void vec_<_Ty, _Size>::assign(const scalar_type& _Left, const vecN& _Right, _Fn _Func) {
		auto _First2 = _Right.begin();
		auto _Last2  = _Right.end();
		auto _Dest   = this->begin();
		for (; _First2 != _Last2; ++_First2, ++_Dest) {
			*_Dest = _Func(_Left , *_First2);
		}
	}

	template<typename _Ty, size_t _Size> inline 
	vec_<_Ty, _Size>::vec_(const scalar_type& _Val) { this->assign(_Val); }

	template<typename _Ty, size_t _Size> inline 
	vec_<_Ty, _Size>::vec_(scalar_const_pointer _Ptr) { this->assign(_Ptr); }

	template<typename _Ty, size_t _Size> inline
	vec_<_Ty, _Size>::vec_(_STD initializer_list<scalar_type> _Ilist) : vec_() { this->assign(_Ilist); }

	template<typename _Ty, size_t _Size> template<typename _Iter> inline 
	vec_<_Ty, _Size>::vec_(_Iter _First, _Iter _Last) : vec_() { this->assign(_First, _Last); }

	template<typename _Ty, size_t _Size> template<typename _Fn> inline 
	vec_<_Ty, _Size>::vec_(const vecN& _Left, _Fn _Func) { this->assign(_Left, _Func); }

	template<typename _Ty, size_t _Size> template<typename _Fn> inline
	vec_<_Ty, _Size>::vec_(const vecN& _Left, const vecN& _Right, _Fn _Func) { this->assign(_Left, _Right, _Func); }

	template<typename _Ty, size_t _Size> template<typename _Fn> inline 
	vec_<_Ty, _Size>::vec_(const vecN& _Left, const scalar_type& _Right, _Fn _Func) { this->assign(_Left, _Right, _Func); }
	
	template<typename _Ty, size_t _Size> template<typename _Fn> inline 
	vec_<_Ty, _Size>::vec_(const scalar_type& _Left, const vecN& _Right, _Fn _Func) { this->assign(_Left, _Right, _Func); }


#define SCALAR_LAMBDA_OP(OP) [](const scalar_type& _A, const scalar_type& _B) { return _A ##OP## _B; }
#define SCALAR_LAMBDA_1ST(FUNC) [](const scalar_type& _A) { return FUNC##(_A); }
#define SCALAR_LAMBDA_2ND(FUNC) [](const scalar_type& _A, const scalar_type& _B) { return FUNC##(_A, _B); }
#define SCALAR_TYPE typename vec_<_Ty, _Size>::scalar_type

	template<typename _Ty, size_t _Size> inline
	vecN vec_<_Ty, _Size>::operator-() const {
		return vecN(*this, std::negate<scalar_type>());
	}

	template<typename _Ty, size_t _Size> inline
	vecN vec_<_Ty, _Size>::operator+(const vecN& _Right) const {
		return vecN(*this, _Right, SCALAR_LAMBDA_OP(+));
	}

	template<typename _Ty, size_t _Size> inline
	vecN vec_<_Ty, _Size>::operator-(const vecN& _Right) const {
		return vecN(*this, _Right, SCALAR_LAMBDA_OP(-));
	}

	template<typename _Ty, size_t _Size> inline
	vecN vec_<_Ty, _Size>::operator*(const vecN& _Right) const {
		return vecN(*this, _Right, SCALAR_LAMBDA_OP(*));
	}

	template<typename _Ty, size_t _Size> inline
	vecN vec_<_Ty, _Size>::operator/(const vecN& _Right) const {
		return vecN(*this, _Right, SCALAR_LAMBDA_OP(/));
	}

	template<typename _Ty, size_t _Size> inline
	vecN vec_<_Ty, _Size>::operator+(const scalar_type& _Right) const {
		return vecN(*this, _Right, SCALAR_LAMBDA_OP(+));
	}

	template<typename _Ty, size_t _Size> inline
	vecN vec_<_Ty, _Size>::operator-(const scalar_type& _Right) const {
		return vecN(*this, _Right, SCALAR_LAMBDA_OP(-));
	}

	template<typename _Ty, size_t _Size> inline
	vecN vec_<_Ty, _Size>::operator*(const scalar_type& _Right) const {
		return vecN(*this, _Right, SCALAR_LAMBDA_OP(*));
	}

	template<typename _Ty, size_t _Size> inline
	vecN vec_<_Ty, _Size>::operator/(const scalar_type& _Right) const {
		return vecN(*this, _Right, SCALAR_LAMBDA_OP(/));
	}

	template<typename _Ty, size_t _Size> inline
	vecN& vec_<_Ty, _Size>::operator+=(const vecN& _Right) {
		this->assign(*this, _Right, SCALAR_LAMBDA_OP(+));
		return (*this);
	}

	template<typename _Ty, size_t _Size> inline
	vecN& vec_<_Ty, _Size>::operator-=(const vecN& _Right) {
		this->assign(*this, _Right, SCALAR_LAMBDA_OP(-));
		return (*this);
	}

	template<typename _Ty, size_t _Size> inline
	vecN& vec_<_Ty, _Size>::operator*=(const vecN& _Right) {
		this->assign(*this, _Right, SCALAR_LAMBDA_OP(*));
		return (*this);
	}

	template<typename _Ty, size_t _Size> inline
	vecN& vec_<_Ty, _Size>::operator/=(const vecN& _Right) {
		this->assign(*this, _Right, SCALAR_LAMBDA_OP(/));
		return (*this);
	}

	template<typename _Ty, size_t _Size> inline
	vecN& vec_<_Ty, _Size>::operator+=(const scalar_type& _Right) {
		this->assign(*this, _Right, SCALAR_LAMBDA_OP(+));
		return (*this);
	}

	template<typename _Ty, size_t _Size> inline
	vecN& vec_<_Ty, _Size>::operator-=(const scalar_type& _Right) {
		this->assign(*this, _Right, SCALAR_LAMBDA_OP(-));
		return (*this);
	}

	template<typename _Ty, size_t _Size> inline
	vecN& vec_<_Ty, _Size>::operator*=(const scalar_type& _Right) {
		this->assign(*this, _Right, SCALAR_LAMBDA_OP(*));
		return (*this);
	}

	template<typename _Ty, size_t _Size> inline
	vecN& vec_<_Ty, _Size>::operator/=(const scalar_type& _Right) {
		this->assign(*this, _Right, SCALAR_LAMBDA_OP(/));
		return (*this);
	}


	template<typename _Ty, size_t _Size> inline
	SCALAR_TYPE vec_<_Ty, _Size>::sum() const {
		auto _First = this->begin();
		auto _Sum   = *_First++;
		for (; _First != this->end(); ++_First) {
			_Sum += (*_First);
		}
		return _Sum;
	}

	template<typename _Ty, size_t _Size> inline
	SCALAR_TYPE vec_<_Ty, _Size>::product() const {
		auto _First = this->begin();
		auto _Product = *_First++;
		for (; _First != this->end(); ++_First) {
			_Product *= (*_First);
		}
		return _Product;
	}

	template<typename _Ty, size_t _Size> inline
	SCALAR_TYPE vec_<_Ty, _Size>::normL1() const {
		auto _First  = this->begin();
		auto _NormL1 = abs(*_First++);
		for (; _First != this->end(); ++_First) {
			_NormL1 += abs((*_First));
		}
		return _NormL1;
	}

	template<typename _Ty, size_t _Size> inline
	SCALAR_TYPE vec_<_Ty, _Size>::normL2_sq() const {
		auto _First    = this->begin();
		auto _NormL2sq = (*_First) * (*_First);
		++_First;
		for (; _First != this->end(); ++_First) {
			_NormL2sq += (*_First) * (*_First);
		}
		return _NormL2sq;
	}

	template<typename _Ty, size_t _Size> inline
	SCALAR_TYPE vec_<_Ty, _Size>::normL2() const {
		return sqrt(this->normL2_sq());
	}

	template<typename _Ty, size_t _Size> inline
	SCALAR_TYPE vec_<_Ty, _Size>::norm(size_t p) const {
		if (p == 1) {
			return this->normL1();
		} else if (p == 2) {
			return this->normL2();
		} else {
			const auto _Exp    = static_cast<scalar_type>(p);
			const auto _Invexp = static_cast<scalar_type>(1) / _Exp;
			auto       _First  = this->begin();
			auto       _Norm   = pow(abs(*_First++), _Exp);
			for (; _First != this->end(); ++_First) {
				_Norm += pow(abs(*_First), _Exp);
			}

			return pow(_Norm, _Invexp);
		}
	}

	template<typename _Ty, size_t _Size> inline
	vecN& vec_<_Ty, _Size>::normalize() {
		const auto _One        = static_cast<scalar_type>(1);
		const auto _Threshould = static_cast<scalar_type>(0.000'0000'002);
		auto       _Length     = this->normL2_sq();// 1² = 1, sqrt(1) = 1
		if (abs(_Length - _One) <= _Threshould) {
			_Length = sqrt(_Length);
			(*this) /= _Length;
		}
		
		return (*this);
	}


	// 
	template<typename _Ty, size_t _Size>
	_STD string to_string(const vecN& _X) {
		using std::to_string;
		auto _Str = to_string(_X[0]) + " ";
		for (size_t i = 1; i != _X.size(); ++i) {
			_Str += to_string(_X[i]) + " ";
		}
		_Str.pop_back();
		return _Str;
	}

	template<typename _Ty, size_t _Size> inline
	_STD ostream& operator<<(_STD ostream& _Ostr, const vecN& _X) {
		return (_Ostr << to_string(_X));
	}

	template<typename _Ty, size_t _Size> inline
	vec_<bool, _Size> operator==(const vecN& _Left, const vecN& _Right) {
		vec_<bool, _Size> _Result;
		for (size_t i = 0; i != _Left.size(); ++i) {
			_Result[i] = (_Left[i] == _Right[i]);
		}
		return _Result;
	}

	template<typename _Ty, size_t _Size> inline
	vec_<bool, _Size> operator!=(const vecN& _Left, const vecN& _Right) {
		vec_<bool, _Size> _Result;
		for (size_t i = 0; i != _Left.size(); ++i) {
			_Result[i] = (_Left[i] != _Right[i]);
		}
		return _Result;
	}

	template<typename _Ty, size_t _Size> inline
	vec_<bool, _Size> operator<(const vecN& _Left, const vecN& _Right) {
		vec_<bool, _Size> _Result;
		for (size_t i = 0; i != _Left.size(); ++i) {
			_Result[i] = (_Left[i] < _Right[i]);
		}
		return _Result;
	}

	template<typename _Ty, size_t _Size> inline
	vec_<bool, _Size> operator>(const vecN& _Left, const vecN& _Right) {
		vec_<bool, _Size> _Result;
		for (size_t i = 0; i != _Left.size(); ++i) {
			_Result[i] = (_Left[i] > _Right[i]);
		}
		return _Result;
	}

	template<typename _Ty, size_t _Size> inline
	vec_<bool, _Size> operator<=(const vecN& _Left, const vecN& _Right) {
		vec_<bool, _Size> _Result;
		for (size_t i = 0; i != _Left.size(); ++i) {
			_Result[i] = (_Left[i] <= _Right[i]);
		}
		return _Result;
	}

	template<typename _Ty, size_t _Size> inline
	vec_<bool, _Size> operator>=(const vecN& _Left, const vecN& _Right) {
		vec_<bool, _Size> _Result;
		for (size_t i = 0; i != _Left.size(); ++i) {
			_Result[i] = (_Left[i] >= _Right[i]);
		}
		return _Result;
	}

	template<size_t _Size> inline
	bool all_of(const vec_<bool, _Size>& _X) {
		for (auto _First = _X.begin(); _First != _X.end(); ++_First) {
			if (!(*_First)) {
				return false;
			}
		}
		return true;
	}
	
	template<size_t _Size> inline
	bool any_of(const vec_<bool, _Size>& _X) {
		for (auto _First = _X.begin(); _First != _X.end(); ++_First) {
			if ( *_First ) {
				return true;
			}
		}
		return false;
	}

	template<size_t _Size> inline
	bool none_of(const vec_<bool, _Size>& _X) {
		for (auto _First = _X.begin(); _First != _X.end(); ++_First) {
			if ((*_First)) {
				return false;
			}
		}
		return true;
	}


	template<typename _Ty, size_t _Size> inline
	vecN mod(const vecN& _Left, const vecN& _Right) {
		return vecN(_Left, _Right, SCALAR_LAMBDA_2ND(mod));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN pow(const vecN& _Left, const vecN& _Right) {
		return vecN(_Left, _Right, SCALAR_LAMBDA_2ND(pow));
	}

	template<typename _Ty, size_t _Size> inline
	vecN vmin(const vecN& _Left, const vecN& _Right) {
		return vecN(_Left, _Right, SCALAR_LAMBDA_2ND(std::min));
	}

	template<typename _Ty, size_t _Size> inline
	vecN vmax(const vecN& _Left, const vecN& _Right) {
		return vecN(_Left, _Right, SCALAR_LAMBDA_2ND(std::max));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN cross3(const vecN& _Left, const vecN& _Right) {
		return vecN{_Left[1] * _Right[2] - _Left[2] * _Right[1],
					_Left[2] * _Right[0] - _Left[0] * _Right[2],
					_Left[0] * _Right[1] - _Left[1] * _Right[0] };
	}

	template<typename _Ty, size_t _Size> inline
	SCALAR_TYPE dot(const vecN& _Left, const vecN& _Right) {
		auto _Dot = _Left[0] * _Right[0];
		for (size_t i = 1; i != _Left.size(); ++i) {
			_Dot += _Left[i] * _Right[i];
		}
		return _Dot;
	}

	template<typename _Ty, size_t _Size> inline
	vecN mod(const vecN& _Left, const SCALAR_TYPE& _Right) {
		return vecN(_Left, _Right, std::modulus<>());
	}

	template<typename _Ty, size_t _Size> inline
	vecN pow(const vecN& _Left, const SCALAR_TYPE& _Right) {
		return vecN(_Left, _Right, SCALAR_LAMBDA_2ND(pow));
	}

	template<typename _Ty, size_t _Size> inline
	vecN operator+(const SCALAR_TYPE& _Left, const vecN& _Right) {
		return vecN(_Left, _Right, std::plus<>());
	}

	template<typename _Ty, size_t _Size> inline
	vecN operator-(const SCALAR_TYPE& _Left, const vecN& _Right) {
		return vecN(_Left, _Right, std::minus<>());
	}

	template<typename _Ty, size_t _Size> inline
	vecN operator*(const SCALAR_TYPE& _Left, const vecN& _Right) {
		return vecN(_Left, _Right, std::multiplies<>());
	}

	template<typename _Ty, size_t _Size> inline
	vecN operator/(const SCALAR_TYPE& _Left, const vecN& _Right) {
		return vecN(_Left, _Right, std::divides<>());
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN mod(const SCALAR_TYPE& _Left, const vecN& _Right) {
		return vecN(_Left, _Right, SCALAR_LAMBDA_2ND(mod) );
	}


	template<typename _Ty, size_t _Size> inline
	vecN abs(const vecN& _X) {
		return vecN(_X, [](const _Ty& _Val) { return abs(_Val); });
	}

	template<typename _Ty, size_t _Size> inline
	vecN floor(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(floor));
	}

	template<typename _Ty, size_t _Size> inline
	vecN ceil(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(ceil));
	}

	template<typename _Ty, size_t _Size> inline
	vecN trunc(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(trunc));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN round(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(round));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN sqrt(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(sqrt));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN cbrt(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(cbrt));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN invsqrt(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(invsqrt));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN invcbrt(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(invcbrt));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN log(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(log));
	}

	template<typename _Ty, size_t _Size> inline
	vecN log2(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(log2));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN log10(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(log10));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN exp(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(exp));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN exp2(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(exp2));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN exp10(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(exp10));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN log1p(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(log1p));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN expm1(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(expm1));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN erf(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(erf));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN erfc(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(erfc));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN erfinv(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(erfinv));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN erfcinv(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(erfcinv));
	}

	
	template<typename _Ty, size_t _Size> inline
	vecN hypot(const vecN& _A, const vecN& _B) {// sqrt(_A², _B²)
		return vecN(_A, _B, SCALAR_LAMBDA_2ND(hypot));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN sin(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(sin));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN cos(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(cos));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN tan(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(tan));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN sincos(vecN& _Cos, const vecN& _X) {
		for (size_t i = 0; i != _X.size(); ++i) {
			_Cos[i] = cos(_X[i]);
		}
		return vecN(_X, SCALAR_LAMBDA_1ST(sin));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN asin(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(asin));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN acos(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(acos));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN atan(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(atan));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN atan2(vecN& _Y, const vecN& _X) {
		return vecN(_Y, _X, SCALAR_LAMBDA_2ND(atan2));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN sinh(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(sinh));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN cosh(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(cosh));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN tanh(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(tanh));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN asinh(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(asinh));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN acosh(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(acosh));
	}
	
	template<typename _Ty, size_t _Size> inline
	vecN atanh(const vecN& _X) {
		return vecN(_X, SCALAR_LAMBDA_1ST(atanh));
	}

	template<typename _Ty, size_t _Size> inline
	SCALAR_TYPE length(const vecN& _X) {
		return _X.normL2();
	}

	template<typename _Ty, size_t _Size> inline
	vecN normalize(const vecN& _X) {
		auto _Normalized = _X;
		return _Normalized.normalize();
	}

#undef SCALAR_LAMBDA_OP
#undef SCALAR_LAMBDA_1ST
#undef SCALAR_LAMBDA_2ND
#undef SCALAR_TYPE

}// namespace clmagic

#endif