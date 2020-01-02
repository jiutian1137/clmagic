#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH___LAPACK___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH___LAPACK___H__
#include "general.h"

namespace clmagic 
{
	template<size_t M, size_t N, typename T> struct Mat_;
	template<typename T> struct Vec2_;
	template<typename T> struct Vec3_;
	template<typename T> struct Vec4_;
	template<size_t _Mysize, typename _Ty> struct Vec_;


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
		void _Add_vector_scalar(
			/*out*/		_Ty* _Pdst, 
			/*in*/const _Ty* _Psrc, 
			_in(_Ty)		 _Scalar, 
			size_t			 _Size);

	template<typename _Ty>
		void _Add_vector_vector(
			/*out*/		_Ty* _Pdst, 
			/*in*/const _Ty* _Plhs, 
			/*in*/const _Ty* _Prhs, 
			size_t		     _Size);

	template<typename _Ty>
		void _Sub_vector_scalar(
			/*out*/		_Ty* _Pdst, 
			/*in*/const _Ty* _Psrc,
			_in(_Ty)		 _Scalar, 
			size_t			 _Size);

	template<typename _Ty>
		void _Sub_vector_vector(
			/*out*/	    _Ty* _Pdst,
			/*in*/const _Ty* _Plhs, 
			/*in*/const _Ty* _Prhs,
			size_t			 _Size);

	template<typename _Ty>
		void _Mul_vector_scalar(
			/*out*/		_Ty* _Pdst,
			/*in*/const _Ty* _Psrc, 
			_in(_Ty)		 _Scalar, 
			size_t			 _Size);

	template<typename _Ty>
		void _Mul_vector_vector(
			/*out*/		_Ty* _Pdst, 
			/*in*/const _Ty* _Plhs, 
			/*in*/const _Ty* _Prhs, 
			size_t			 _Size);

	template<typename _Ty>
		void _Div_vector_scalar(
			/*out*/		_Ty* _Pdst, 
			/*in*/const _Ty* _Psrc, 
			_in(_Ty)		 _Scalar, 
			size_t			 _Size);

	template<typename _Ty>
		void _Div_scalar_vector(
			/*out*/		_Ty* _Pdst, 
			_in(_Ty)		 _Scalar, 
			/*in*/const _Ty* _Prhs,
			size_t			 _Size);

	template<typename _Ty>
		void _Div_vector_vector(
			/*out*/		_Ty* _Pdst, 
			/*in*/const _Ty* _Plhs, 
			/*in*/const _Ty* _Prhs, 
			size_t			 _Size);

	template<typename _Ty>
		void _Mod_vector_scalar(
			/*out*/		_Ty* _Pdst, 
			/*in*/const _Ty* _Plhs, 
			_in(_Ty)		 _Scalar, 
			size_t			 _Size);

	template<typename _Ty>
		void _Mod_vector_vector(
			/*out*/		_Ty* _Pdst, 
			/*in*/const _Ty* _Plhs, 
			/*in*/const _Ty* _Prhs, 
			size_t			 _Size);

	template<typename _Ty>
		void _Pow_vector_scalar(
			/*out*/	    _Ty* _Pdst, 
			/*in*/const _Ty* _Psrc, 
			/*in*/		int  _Power, 
			size_t			 _Size);

	template<typename _Ty>
		void _Pow_vector_scalar(
			/*out*/		_Ty* _Pdst, 
			/*in*/const _Ty* _Psrc, 
			/*in*/	  float  _Power, 
			size_t			 _Size);

	template<typename _Ty>
		void _Pow_vector_scalar(
			/*out*/		_Ty* _Pdst, 
			/*in*/const _Ty* _Psrc, 
			/*in*/	 double  _Power,
			size_t			 _Size);

	template<typename _Ty>
		void _Pow_vector_vector(
			/*out*/		_Ty* _Pdst, 
			/*in*/const _Ty* _Plhs, 
			/*in*/const _Ty* _Prhs, 
			size_t			 _Size);

	template<typename _Ty>
		void _Sqrt_vector(
			/*out*/		_Ty* _Pdst, 
			/*in*/const _Ty* _Psrc, 
			size_t			 _Size);

	template<typename _Ty>
		void _Floor_vector(
			/*out*/	    _Ty* _Pdst,
			/*in*/const _Ty* _Plhs,
			size_t           _Size);

	template<typename _Ty>
		void _Ceil_vector(
			/*out*/	    _Ty* _Pdst,
			/*in*/const _Ty* _Plhs,
			size_t           _Size);

	/* < triangle function > */
	template<typename _Ty>
		void _Sin_vector(
			/*out*/	    _Ty* _Pdst,
			/*in*/const _Ty* _Plhs,
			size_t           _Size);

	template<typename _Ty>
		void _Cos_vector(
			/*out*/	    _Ty* _Pdst,
			/*in*/const _Ty* _Plhs,
			size_t           _Size);

	template<typename _Ty>
		void _Tan_vector(
			/*out*/	    _Ty* _Pdst,
			/*in*/const _Ty* _Plhs,
			size_t           _Size);

	template<typename _Ty>
		void _Asin_vector(
			/*out*/	    _Ty* _Pdst,
			/*in*/const _Ty* _Plhs,
			size_t           _Size);

	template<typename _Ty>
		void _Acos_vector(
			/*out*/	    _Ty* _Pdst,
			/*in*/const _Ty* _Plhs,
			size_t           _Size);

	template<typename _Ty>
		void _Atan_vector(
			/*out*/	    _Ty* _Pdst,
			/*in*/const _Ty* _Plhs,
			size_t           _Size);
	/* </ triangle function > */

	template<typename _Ty>
		_Ty _Sum_vector(
			/*in*/const _Ty* _Plhs,
			size_t           _Size);

	template<typename _Ty>
		_Ty _Dot_vector(
			/*in*/const _Ty* _Plhs,
			/*in*/const _Ty* _Prhs,
			size_t           _Size);

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
	@_matrix:
	*/
	template<size_t M, size_t N, typename T>
	struct Mat_
	{
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
		for (size_t i = 1; i != 16; ++i)
			{
			std::string _Str = "constexpr Mat_(";
			for (size_t j = 0; j <= i; ++j)
				{
				_Str += " _in(T)";
				_Str += " _Val" + std::to_string(j);
				_Str += ",";
				}
			_Str.pop_back();
			_Str += ")";

			_Str += " : data{";
			for (size_t j = 0; j <= i; ++j)
				{
				_Str += "_Val" + std::to_string(j);
				_Str += ",";
				}
			_Str.pop_back();
			_Str += "}";

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
		constexpr       T* ptr()	   { return (_Mydata); }
		constexpr const T* ptr() const { return (_Mydata); }
		constexpr       T* ptr(size_t _Pos) { return (_Mydata + _Pos); }
		constexpr const T* ptr(size_t _Pos) const { return (_Mydata + _Pos); }
		constexpr T*	   operator[] (size_t _Pos)	      { return (_Mydata[_Pos]); }
		constexpr const T* operator[] (size_t _Pos) const { return (_Mydata[_Pos]); }
		constexpr T&	   at(size_t _Row, size_t _Col)		  { return (_Mydata[_Row][_Col]); }
		constexpr const T& at(size_t _Row, size_t _Col) const { return (_Mydata[_Row][_Col]); }
			   Vec_<N, T>& row(size_t _Pos);
		 const Vec_<N, T>& row(size_t _Pos) const;

		void assign(std::initializer_list<T> _Ilist);

		
		Mat_<N, M, T> transpose() const;
		Mat_<M, N, T> inverse() const;
		bool is_identity() const;

		T _Mydata[M * N];
	};


	// Mat_<size_t, size_t, _Ty>::operator std::string() dependent on the this function
	template< size_t M, size_t N, typename _Ty > inline
		std::string to_string( const Mat_< M, N, _Ty >& _Val )
		{
		return ( static_cast<std::string>( _Val ) );
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

	
	template<size_t M, size_t N, typename T> Mat_<M, N, T> operator- ( _in(Mat_<M, N, T>) _Lhs );
	template<size_t M, size_t N, typename T> Mat_<M, N, T> operator+ ( _in(Mat_<M, N, T>) _Lhs, _in(Mat_<M, N, T>) _Rhs);
	template<size_t M, size_t N, typename T> Mat_<M, N, T> operator- ( _in(Mat_<M, N, T>) _Lhs, _in(Mat_<M, N, T>) _Rhs);

	/* < ? matrix, scalar > */
	template<size_t M, size_t N, typename T> Mat_<M, N, T> operator* ( _in(Mat_<M, N, T>) _Lhs, _in(T) _Rhs );
	template<size_t M, size_t N, typename T> Mat_<M, N, T> operator* ( _in(T) _Lhs, _in(Mat_<M, N, T>) _Rhs );
	/* </ ? matrix, scalar > */

	/* < ? matrix matrix > */
	template<size_t M, size_t N, size_t P, typename T> Mat_<M, P, T> operator* (_in(Mat_<M, N, T>) _Lhs, _in(Mat_<N, P, T>) _Rhs);
	/* </ ? matrix matrix > */

	template<size_t M, size_t N, typename T> Mat_<M, N, T>& operator+= ( _inout(Mat_<M, N, T>) _Lhs, _in(Mat_<M, N, T>) _Rhs);
	template<size_t M, size_t N, typename T> Mat_<M, N, T>& operator-= ( _inout(Mat_<M, N, T>) _Lhs, _in(Mat_<M, N, T>) _Rhs);
	template<size_t M, size_t N, typename T> Mat_<M, N, T>& operator*= ( _inout(Mat_<M, N, T>) _Lhs, _in(T) _Rhs);

	/* < ?= square_matrix > */
	template<size_t M, typename T> Mat_<M, M, T>& operator*= ( _inout(Mat_<M, M, T>) _Lhs, _in(Mat_<M, M, T>)    _Rhs);
	/* </ ?= square_matrix > */

	template<size_t M, size_t N, typename T> Mat_<N, M, T> transpose( _in(Mat_<M, N, T>) _Lhs);
	template<size_t M, size_t N, typename T> Mat_<M, N, T> inverse( _in(Mat_<M, N, T>) _Lhs);

	template<typename T> 
		Vec3_<T> transform_coord(_in(Vec3_<T>) _Lhs, _in(Mat_<4, 4, T>) _Matrix);
	template<typename T> 
		Vec3_<T> transform_normal(_in(Vec3_<T>) _Lhs, _in(Mat_<4, 4, T>) _Matrix);
	
	Vec3_<float> screen_to_world(_in(Vec3_<float>) _Vec, _in(Mat4) _Viewmatrix, _in(Mat4) _Projmatrix);

	template<typename _Ty>
		_Ty determinant(const Mat_<2, 2, _Ty> _Src);
		
	template<typename T> 
		Mat_<4, 4, T> scaling_matrix(
			_in(T) _Width,
			_in(T) _Height,
			_in(T) _Depth);

	template<typename T>
		Mat_<4, 4, T> scaling_matrix(
			_in(Vec3_<T>) _sXyz);

	template<typename T> 
		Mat_<4, 4, T> rotation_matrix(
			_in(Vec3_<T>) _Axis, 
			_in(T)	      _Radians);

	template<typename T>
		Mat_<4, 4, T> translation_matrix(
			_in(T) _X,
			_in(T) _Y,
			_in(T) _Z);

	template<typename T>
		Mat_<4, 4, T> translation_matrix(
			_in(Vec3_<T>) _dXyz);

	/*
	@_Eye as eye position
	@_Forward as forward vector
	@_Up as up vector, require normalize vector
	*/
	template<typename T> 
		Mat_<4, 4, T> Lookat(
			_in(Vec3_<T>) _Eye, 
			_in(Vec3_<T>) _Forward, 
			_in(Vec3_<T>) _Up);

	// @_Target as target position
	template<typename T>
		Mat_<4, 4, T> Lookat2(
			_in(Vec3_<T>) _Eye,
			_in(Vec3_<T>) _Target,
			_in(Vec3_<T>) _Up);

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
	
#undef X
#undef Y
#undef Z
#undef W
	template<size_t N, typename _Ty>
		Vec_<N, _Ty> vector_selector(
			/*in*/const _Ty*  _Src,
			/*in*/const char* _Selector);

	//- - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - - -



	

	

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

			using value_type = T;
			using vector_type = Vec_<N, T>;
			using iterator = typename std::_Array_iterator<T, N>;
			using const_iterator = typename std::_Array_const_iterator<T, N>;

			/* < construct > */
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

			operator SIMDVec4_<T>() const {
				if (N >= 4) {
					return (SIMDVec4_<T>(_Mydata[0], _Mydata[1], _Mydata[2], _Mydata[3]));
				} else if (N == 3) {
					return (SIMDVec4_<T>(_Mydata[0], _Mydata[1], _Mydata[2], T(0)));
				} else {
					return (SIMDVec4_<T>(_Mydata[0], _Mydata[1], T(0), T(0)));
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

			template<typename ..._Tys>
				auto shuffle(_Tys... _Selector) const ->decltype( Vec_<std::tuple_size<std::tuple<_Tys...>>::value, T>(/* empty */) ) {
					return ( clmagic::shuffle< Vec_</*bg*/std::tuple_size<std::tuple<_Tys...>>::value/*ed*/, T> >(*this, _Selector...) );
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
			friend Vec_ atan     (_in(Vec_) _Y, _in(Vec_) _X) { return (Vec_(_A, _B, [](T y, T x) { return atan(y, x); })); }

			friend T    dot2     (_in(Vec_) _A, _in(Vec_) _B) { auto _Tmp = _A * _B; return (_Tmp[0] + _Tmp[1]); }
			friend T    dot3     (_in(Vec_) _A, _in(Vec_) _B) { auto _Tmp = _A * _B; return (_Tmp[0] + _Tmp[1] + _Tmp[2]); }
			friend T    dot4     (_in(Vec_) _A, _in(Vec_) _B) { auto _Tmp = _A * _B; return (_Tmp[0] + _Tmp[1] + _Tmp[2] + _Tmp[3]); }
			friend Vec_ cross3   (_in(Vec_) _A, _in(Vec_) _B) { return (_A.shuffle(1, 2, 0) * _B.shuffle(2, 0, 1) - _A.shuffle(2, 0, 1) * _B.shuffle(1, 2, 0)); }

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


		using Vec2b = Vec_<2, char>;
		using Vec2i = Vec_<2, int>;
		using Vec2ui = Vec_<2, unsigned int>;
		using Vec2f = Vec_<2, float>;
		using Vec2d = Vec_<2, double>;
		using Vec2 = Vec2f;

		using Vec3b = Vec_<3, char>;
		using Vec3i = Vec_<3, int>;
		using Vec3ui = Vec_<3, unsigned int>;
		using Vec3f = Vec_<3, float>;
		using Vec3d = Vec_<3, double>;
		using Vec3 = Vec3f;

		using Vec4b = Vec_<4, char>;
		using Vec4i = Vec_<4, int>;
		using Vec4ui = Vec_<4, unsigned int>;
		using Vec4f = Vec_<4, float>;
		using Vec4d = Vec_<4, double>;
		using Vec4 = Vec4f;


	// vec_<size_t, _Ty>::operator std::string() dependent on the this function
	template<size_t N, typename T>
		std::string to_string(_in(Vec_<N, T>) _Src) {
			return ( static_cast<std::string>(_Src) );
		}

#ifndef  cX
	constexpr Vec3 cX = Vec3(1.f, 0.f, 0.f);
#endif
#ifndef  cY
	constexpr Vec3 cY = Vec3(0.f, 1.f, 0.f);
#endif
#ifndef  cZ
	constexpr Vec3 cZ = Vec3(0.f, 0.f, 1.f);
#endif


	template<typename T> T length(_in(Vec2_<T>) _Lhs);
	template<typename T> T length(_in(Vec3_<T>) _Lhs);
	template<typename T> T length(_in(Vec4_<T>) _Lhs);
	template<typename T> T length(_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> T length(_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> T length(_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> T length(_in(Vec_<N, T>) _Lhs);
	template<size_t N, typename T> T length(_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs);

	template<typename T> Vec2_<T> normalize(_in(Vec2_<T>) _Lhs);
	template<typename T> Vec3_<T> normalize(_in(Vec3_<T>) _Lhs);
	template<typename T> Vec4_<T> normalize(_in(Vec4_<T>) _Lhs);
	template<size_t N, typename T> Vec_<N, T> normalize(_in(Vec_<N, T>) _Lhs);

	template<typename T> T radians(_in(Vec3_<T>) _V0, _in(Vec3_<T>) _V1);

	// dot(_I, _Nref) < 0 ? _Normal : -_Normal
	template<typename T> Vec2_<T> faceforward(_in(Vec2_<T>) _Normal, _in(Vec2_<T>) _I, _in(Vec2_<T>) _Nref);
	template<typename T> Vec3_<T> faceforward(_in(Vec3_<T>) _Normal, _in(Vec3_<T>) _I, _in(Vec3_<T>) _Nref);
	template<typename T> Vec4_<T> faceforward(_in(Vec4_<T>) _Normal, _in(Vec4_<T>) _I, _in(Vec4_<T>) _Nref);
	template<size_t N, typename T> Vec_<N, T> faceforward(_in(Vec_<N, T>) _Normal, _in(Vec_<N, T>) _I, _in(Vec_<N, T>) _Nref);

	template<typename T> Vec3_<T> reflect(_in(Vec3_<T>) _Vec, _in(Vec3_<T>) _Normal);
	template<typename T> Vec3_<T> proj( _in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Proj );
	template<typename T> Vec2_<T> orthogonal( _in(Vec2_<T>) _Lhs );
	template<typename T> void     orthogonal( _inout(std::vector<Vec3_<T>>) _Vectors );

	template<typename T, typename _Fn> Vec2_<T> for_each(_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs, _Fn _Func);
	template<typename T, typename _Fn> Vec3_<T> for_each(_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs, _Fn _Func);
	template<typename T, typename _Fn> Vec4_<T> for_each(_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs, _Fn _Func);
	template<size_t N, typename T, typename _Fn> Vec_<N, T> for_each(_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs, _Fn _Func);

	template<typename T, typename T2> Vec2_<T> type_cast(_in(Vec2_<T2>) _Src);
	template<typename T, typename T2> Vec3_<T> type_cast(_in(Vec3_<T2>) _Src);
	template<typename T, typename T2> Vec4_<T> type_cast(_in(Vec4_<T2>) _Src);
	template<typename T, typename T2, size_t N> Vec_<N, T> type_cast(_in(Vec_<N, T2>) _Src);

	template<typename T, size_t N> bool operator< (const Vec_<N, T>& _Lhs, const Vec_<N, T>& _Rhs);
	template<typename T, size_t N> bool operator> (const Vec_<N, T>& _Lhs, const Vec_<N, T>& _Rhs);
	template<typename T, size_t N> bool operator>= (const Vec_<N, T>& _Lhs, const Vec_<N, T>& _Rhs);
	template<typename T, size_t N> bool operator<= (const Vec_<N, T>& _Lhs, const Vec_<N, T>& _Rhs);
	template<typename T, size_t N> bool operator== (const Vec_<N, T>& _Lhs, const Vec_<N, T>& _Rhs);


	using Vec_2i = Vec_<2, int>;
	using Vec_3i = Vec_<3, int>;
	using Vec_4i = Vec_<4, int>;
	using Vec_2f = Vec_<2, float>;
	using Vec_3f = Vec_<3, float>;
	using Vec_4f = Vec_<4, float>;
	using Vec_2d = Vec_<2, double>;
	using Vec_3d = Vec_<3, double>;
	using Vec_4d = Vec_<4, double>;

	
	/* this tag is normal */
	struct _Tag_normal { /* empty */ };

	template<typename T>
	struct MatrixTransform_ 
	{
		MatrixTransform_();
		MatrixTransform_(_in(Mat_<4, 4, T>) _Matrix);

		Vec3_<T> transform(_in(Vec3_<T>) _Vector) const;
		Vec3_<T> transform(_in(Vec3_<T>) _Normal, _Tag_normal) const;
		Vec3_<T> inv_transform(_in(Vec3_<T>) _Vector) const;
		Vec3_<T> inv_transform(_in(Vec3_<T>) _Normal, _Tag_normal) const;

		Mat_<4, 4, T> _Mymat;
		Mat_<4, 4, T> _Myinvmat;
	};
	


	template<typename T>
	struct Quaternion_;

	/* SQT */
	template<typename T>
	struct SQTTransform_
	{
		SQTTransform_();
		explicit SQTTransform_(_in(Mat4) _Matrix);

		explicit operator Mat_<4, 4, T>() const;
		explicit operator std::string() const;

		Vec3_<T> transform(_in(Vec3_<T>) _Vector) const;
		Vec3_<T> transform(_in(Vec3_<T>) _Normal, _Tag_normal) const;

		Vec3_<T>  _Myscl;	  // S
		T _Pand1;
		Quaternion_<T> _Myrot;// Q
		Vec3_<T>  _Mymov;     // T
		T _Pand2;
	};
	
	template<typename T> SQTTransform_<T> operator+ (_in(SQTTransform_<T>) _Lhs, _in(SQTTransform_<T>) _Rhs);
	template<typename T> SQTTransform_<T> operator- (_in(SQTTransform_<T>) _Lhs, _in(SQTTransform_<T>) _Rhs);
	template<typename T> SQTTransform_<T>& operator+= (_inout(SQTTransform_<T>) _Lhs, _in(SQTTransform_<T>) _Rhs);
	template<typename T> SQTTransform_<T>& operator-= (_inout(SQTTransform_<T>) _Lhs, _in(SQTTransform_<T>) _Rhs);

	


	/*
	@_tangent_btangent_normal
	@_tangent_space
	*/
	struct tbn
	{
		tbn(const Vec3& _P0, const Vec3& _P1, const Vec3& _P2,
			const Vec2& _Texcoord0, const Vec2& _Texcoord1, const Vec2& _Texcoord2);

		Vec3 tangent;
		Vec3 btangent;
		Vec3 _Mynormal;
	};



	/* < following: is object [in] the region [with] crosspoint > */
	template<typename _Ty>
		bool intersect(
			_in(Vec2_<_Ty>) _Pos,
			/*in*/_Ty	    _Left,
			/*in*/_Ty		_Right,
			/*in*/_Ty		_Bottom,
			/*in*/_Ty		_Top);

	/*
	@_Return no hit if( 0 )
	*/
	template<typename T> 
		int intersect(
			_in(Vec3_<T>) _Pos,
			_in(Vec3_<T>) _V0, 
			_in(Vec3_<T>) _V1, 
			_in(Vec3_<T>) _V2);

	template<typename T> 
		bool intersect(
			_in(Vec3_<T>) _Origin, 
			_in(Vec3_<T>) _Dir, 
			_in(Vec3_<T>) _V0, 
			_in(Vec3_<T>) _V1, 
			_in(Vec3_<T>) _V2, 
			_out(Vec3_<T>) _Point);

	/* </ following: is object [in] the region [with] crosspoint > */


	using MatrixTransform = MatrixTransform_<float>;
	using sqt_transform = SQTTransform_<float>;
}// namespace clmagic

#endif