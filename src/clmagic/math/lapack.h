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
		

		explicit operator	     T*()		{ return (_Mydata[0]); }
		explicit operator  const T*() const { return (_Mydata[0]); }
		explicit operator std::string() const;

		constexpr T*	   operator[] (size_t _Pos)	      { return (_Mydata[_Pos]); }
		constexpr const T* operator[] (size_t _Pos) const { return (_Mydata[_Pos]); }
		constexpr T&	   at(size_t _Row, size_t _Col)		  { return (_Mydata[_Row][_Col]); }
		constexpr const T& at(size_t _Row, size_t _Col) const { return (_Mydata[_Row][_Col]); }
		constexpr T*	   ptr()	   { return (_Mydata[0]); }
		constexpr const T* ptr() const { return (_Mydata[0]); }
			   Vec_<N, T>& row(size_t _Pos);
		 const Vec_<N, T>& row(size_t _Pos) const;

		void assign(std::initializer_list<T> _Ilist);
		constexpr size_t rows() const { return (M); }
		constexpr size_t cols() const { return (N); }
		constexpr size_t size() const { return (rows() * cols()); }

		
		Mat_<N, M, T> transpose() const;
		Mat_<M, N, T> inverse() const;
		bool is_identity() const;

		T _Mydata[M][N];
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

	template<typename T>
	struct Vec2_
	{	// 2 size vector
		constexpr Vec2_() :					    x(T(0)), y(T(0)) { /* empty */ }
		/* X X */ explicit constexpr Vec2_(_in(T) _Val) : x(_Val), y(_Val) { /* empty */ }
		/* X Y */ constexpr Vec2_(_in(T) _X, _in(T) _Y) : x(_X),   y(_Y)	 { /* empty */ }
		/*Mat1x2*/explicit constexpr Vec2_(_in(Mat_<1, 2, T>) _Mat) : x(_Mat[0][0]), y(_Mat[0][1]) { /* empty */ }

		explicit operator std::string() const { using std::to_string; return ("vec2[" + to_string(x) + "," + to_string(y) + "]"); }
		explicit operator		   T*()		  { return ( _Mydata ); }
		explicit operator    const T*() const { return ( _Mydata ); }

		constexpr	    T& operator[](size_t _Pos)		 { return ( _Mydata[_Pos] ); }
		constexpr const T& operator[](size_t _Pos) const { return ( _Mydata[_Pos] ); }
		constexpr	    T* ptr()	   { return (_Mydata); }
		constexpr const T* ptr() const { return (_Mydata); }

		constexpr size_t size() const { return (2); }
		template<size_t N> Vec_<N, T> V(/*in*/const char* _Selector) const { return ( vector_selector(ptr(), _Selector) ); }

		union
		{
			struct { T x, y; };
			struct { T r, g; };
			T _Mydata[2];
		};
	};

	template<typename T> inline 
	std::string to_string( _in(Vec2_<T>) _Src ) {
		return ( static_cast<std::string>(_Src) );
	}

	using Vec2f = Vec2_<float>;
	using Vec2d = Vec2_<double>;
	using Vec2i = Vec2_<int>;
	using Vec2  = Vec2_<real_t>;

	//- - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - - -

	template<typename T>
	struct Vec3_
	{
		constexpr Vec3_() : x(T(0)), y(T(0)), z(T(0)) { }
		/* X Y Z */constexpr Vec3_(_in(T) X, _in(T) Y, _in(T) Z) : x(X), y(Y), z(Z) { }
		/* X X X */explicit constexpr Vec3_(_in(T) _Val) : x(_Val),  y(_Val),  z(_Val)  { }
		/* X-Y Z */constexpr Vec3_(_in(Vec2_<T>) XY,
								   _in(T) Z = T(0)) : x(XY[0]), y(XY[1]), z(Z) { }
		/* X Y-Z */constexpr Vec3_(_in(T) X, _in(Vec2_<T>) YZ) : x(X), y(YZ[0]), z(YZ[1]) { }
		/* Mat1x3*/explicit constexpr Vec3_(_in(Mat_<1, 3, T>) _Mat) : x(_Mat[0][0]), y(_Mat[0][1]), z(_Mat[0][2]) { }

		explicit operator std::string() const { return ( "vec3[" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "]" ); }
		explicit operator		   T*()		  { return (_Mydata); }
		explicit operator    const T*() const { return (_Mydata); }
				 operator    Vec2_<T>() const { return ( Vec2_<T>(x, y) ); }

		constexpr	    T& operator[](size_t _Pos)	     { return ( _Mydata[_Pos] ); }
		constexpr const T& operator[](size_t _Pos) const { return ( _Mydata[_Pos] ); }
		constexpr	    T* ptr()	   { return (_Mydata); }
		constexpr const T* ptr() const { return (_Mydata); }

		constexpr size_t size() const { return (3); }
		template<size_t N> Vec_<N, T> V(const char* _Selector) const { return ( vector_selector(ptr(), _Selector) ); }

		union {
			struct { T x, y, z; };
			struct { T r, g, b; };
			T _Mydata[3];
		};
	};

	template<typename T> inline
	std::string to_string( _in(Vec3_<T>) _Src ) {
		return ( static_cast<std::string>(_Src) );
	}

	using Vec3f = Vec3_<float>;
	using Vec3d = Vec3_<double>;
	using Vec3i = Vec3_<int>;
	using Vec3  = Vec3_<real_t>;

	//- - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - - -

	template< typename T >
	struct Vec4_
	{
		constexpr Vec4_( ) : x(T(0)), y(T(0)), z(T(0)), w(T(0)) { }
		/* X Y Z W */constexpr Vec4_(_in(T) X, _in(T) Y, _in(T) Z, _in(T) W) : x(X), y(Y), z(Z), w(W) { }
		/* X X X X */explicit constexpr Vec4_(_in(T) _Val) : x(_Val), y(_Val), z(_Val), w(_Val) { }

		/* X-Y Z W */constexpr Vec4_(_in(Vec2_<T>) XY,
									_in(T) Z = T(0),
									_in(T) W = T(0)) : x(XY[0]), y(XY[1]), z(Z), w(W) { }

		/* X Y-Z W */constexpr Vec4_(_in(T) X, _in(Vec2_<T>) YZ,
									_in(T) W = T(0)) : x(X), y(YZ[0]), z(YZ[1]), w(W) { }

		/* X Y Z-W */constexpr Vec4_(_in(T) X, _in(T) Y, _in(Vec2_<T>) ZW) : x(X), y(Y), z(ZW[0]), w(ZW[1]) { }

		/* X-Y-Z W */constexpr Vec4_(_in(Vec3_<T>) XYZ,
							        _in(T) W = T(0)) : x(XYZ[0]), y(XYZ[1]), z(XYZ[2]), w(W) { }

		/* X Y-Z-W */constexpr Vec4_(_in(T) X, _in(Vec3_<T>) YZW) : x(X), y(YZW[0]), z(YZW[1]), w(YZW[2]) { }
		
		/*  Mat1x4 */explicit constexpr Vec4_(_in(Mat_<1,4,T>) _Mat) : x(_Mat[0][0]), y(_Mat[0][1]), z(_Mat[0][2]), w(_Mat[0][3]) { }

		explicit operator std::string() const { using std::to_string; return ("Vec4[" + to_string(x) + "," + to_string(y) + "," + to_string(z) + "," + to_string(w) + "]"); }
		explicit operator		   T*()		  { return ( _Mydata ); }
		explicit operator	 const T*() const { return ( _Mydata ); }
		         operator    Vec2_<T>() const { return ( Vec2_<T>(x, y) ); }
		         operator    Vec3_<T>() const { return ( Vec3_<T>(x, y, z) ); }

		constexpr	    T& operator[](size_t _Pos)       { return (_Mydata[_Pos]); }
		constexpr const T& operator[](size_t _Pos) const { return (_Mydata[_Pos]); }
		constexpr	    T* ptr()	   { return (_Mydata); }
		constexpr const T* ptr() const { return (_Mydata); }

		constexpr size_t size() const { return (4); }
		template<size_t N> Vec_<N, T> V(const char* _Selector) const { return ( vector_selector<N>(_Mydata, _Selector) ); }

		union {
			struct { T x, y, z, w; };
			struct { T r, g, b, a; };
			T _Mydata[4];
		};
	};

	template<typename T> inline
	std::string to_string( _in(Vec4_<T>) _Src ) {
		return ( static_cast<std::string>(_Src) );
	}

	using Vec4f = Vec4_<float>;
	using Vec4d = Vec4_<double>;
	using Vec4i = Vec4_<int>;
	using Vec4  = Vec4_<real_t>;

	//- - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef  cX
	constexpr Vec3 cX = Vec3(1.f, 0.f, 0.f);
#endif
#ifndef  cY
	constexpr Vec3 cY = Vec3(0.f, 1.f, 0.f);
#endif
#ifndef  cZ
	constexpr Vec3 cZ = Vec3(0.f, 0.f, 1.f);
#endif

	

	

	/*
	@_vec_<_Mysize, _Ty>:
		Example: vec_<3, float> _Position{ 1.0f, 2.0f, 10.0f };
				vec_<2, std::string> _Fullname{ std::string(), std::string() };
				vec_<9, int> _State{ 0, 0, 0, 0, 0, 1, 0, 0, 0 };
				vec_<5, joint> _Actorjoint{ };
				...
	*/
	template<size_t N, typename T>
	struct Vec_
	{
		using myself_type = Vec_<N, T>;
		using value_type = T;
		using reference = T & ;
		using const_reference = const T&;
		using pointer = T * ;
		using const_pointer = const T*;
		using iterator = typename std::_Array_iterator<T, N>;
		using const_iterator = typename std::_Array_const_iterator<T, N>;

		static_assert(N > 1, "Vec_<> size error");

		// < static >
		static Vec_<N, T> all( _in(T) _Val );
		static Vec_<N, T> zero( );
		// </ static >

		// < construct >
		constexpr Vec_() : _Mydata{ T(0) } { /* empty */ }
		explicit  Vec_(_in(T) _All);
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
		for (size_t i = 1; i != 16; ++i)
			{
			std::string _Str = "constexpr vec_(";
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
			_Str += "static_assert(N >=" + std::to_string(i + 1) + ", \"size < " + std::to_string(i + 1) + "\");";
			_Str += "}";
			_Vecstr.push_back(_Str);
			}
		*/
		explicit Vec_(_in(Vec2_<T>) _Right);
		explicit Vec_(_in(Vec3_<T>) _Right);
		explicit Vec_(_in(Vec4_<T>) _Right);
		explicit Vec_(_in(Mat_<1, N, T>) _Right);
		template<typename _Iter> Vec_(_Iter _First, _Iter _Last);
		// </ construct >

		// < convert >
		explicit operator std::string() const;
		template<size_t N2> 
				 operator Vec_<N2, T>() const;
		explicit operator	    T*()	   { return(_Mydata); }
		explicit operator const T*() const { return(_Mydata); }
		         operator Vec2_<T>() const { return (Vec2_<T>(at(0), at(1))); }
		         operator Vec3_<T>() const { return (Vec3_<T>(at(0), at(1), at(2))); }
		         operator Vec4_<T>() const { return (Vec4_<T>(at(0), at(1), at(2), at(3))); }
		// </ convert >

		// < behavior >
		template<typename _Iter> void assign(_Iter _First, _Iter _Last);
		void assign(std::initializer_list<T> _Ilist);

		constexpr       T& operator[](size_t _Pos)       { return ( _Mydata[_Pos] ); }
		constexpr const T& operator[](size_t _Pos) const { return ( _Mydata[_Pos] ); }
		constexpr	    T& at(size_t _Pos)       { return ( _Mydata[_Pos] ); }
		constexpr const T& at(size_t _Pos) const { return ( _Mydata[_Pos] ); }
		constexpr	    T* ptr()       { return ( _Mydata ); }
		constexpr const T* ptr() const { return ( _Mydata ); }

		constexpr size_t size() const { return ( N ); }

		constexpr       iterator begin()	   { return (iterator(this->ptr(), 0) ); }
		constexpr const_iterator begin() const { return (const_iterator(this->ptr(), 0)); }

		constexpr       iterator end()	     { return ( iterator(this->ptr(), this->size()) ); }
		constexpr const_iterator end() const { return ( const_iterator(this->ptr(), this->size()) ); }

		template<size_t _N2>
			Vec_<_N2, T> V(const char* _Selector) const { return ( vector_selector<_N2, T>(_Mydata, _Selector) ); }
		// </ behavior >

		T _Mydata[N];
	};// end vec_N

	// vec_<size_t, _Ty>::operator std::string() dependent on the this function
	template<size_t N, typename T> inline
		std::string to_string(_in(Vec_<N, T>) _Src)
		{
		return ( static_cast<std::string>(_Src) );
		}



	template<typename T> bool operator< (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> bool operator< (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> bool operator< (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);

	template<typename T> bool operator> (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> bool operator> (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> bool operator> (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);

	template<typename T> bool operator== (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> bool operator== (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> bool operator== (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);

	template<typename T> bool operator<= (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> bool operator<= (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> bool operator<= (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);

	template<typename T> bool operator>= (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> bool operator>= (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> bool operator>= (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);

	template<typename T> Vec2_<T> operator- (_in(Vec2_<T>) _Lhs);
	template<typename T> Vec3_<T> operator- (_in(Vec3_<T>) _Lhs);
	template<typename T> Vec4_<T> operator- (_in(Vec4_<T>) _Lhs);
	template<size_t N, typename T> Vec_<N, T> operator- (_in(Vec_<N, T>) _Lhs);

	/* < ? vector vector > */
	template<typename T> Vec2_<T> operator+ (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T> operator+ (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T> operator+ (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator+ (_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs);

	template<typename T> Vec2_<T> operator- (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T> operator- (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T> operator- (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator- (_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs);
	
	template<typename T> Vec2_<T> operator* (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T> operator* (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T> operator* (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator* (_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs);
	template<> Vec3_<float> operator* (_in(Vec3_<float>) _Lhs, _in(Vec3_<float>) _Rhs);
	template<> Vec4_<float> operator* (_in(Vec4_<float>) _Lhs, _in(Vec4_<float>) _Rhs);

	template<typename T> Vec2_<T> operator/ (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T> operator/ (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T> operator/ (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator/ (_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs);

	template<typename T> Vec2_<T> operator% (_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T> operator% (_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T> operator% (_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator% (_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs);
	/* </ ? vector vector > */

	/* < ? vector scalar > */
	/* this function is bad, but perfermance */
	template<typename T> Vec2_<T> operator+ (_in(Vec2_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec3_<T> operator+ (_in(Vec3_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec4_<T> operator+ (_in(Vec4_<T>) _Lhs, _in(T) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator+ (_in(Vec_<N, T>) _Lhs, _in(T) _Rhs);

	template<typename T> Vec2_<T> operator- (_in(Vec2_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec3_<T> operator- (_in(Vec3_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec4_<T> operator- (_in(Vec4_<T>) _Lhs, _in(T) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator- (_in(Vec_<N, T>) _Lhs, _in(T) _Rhs);

	template<typename T> Vec2_<T> operator* (_in(Vec2_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec3_<T> operator* (_in(Vec3_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec4_<T> operator* (_in(Vec4_<T>) _Lhs, _in(T) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator* (_in(Vec_<N, T>) _Lhs, _in(T) _Rhs);

	template<typename T> Vec2_<T> operator/ (_in(Vec2_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec3_<T> operator/ (_in(Vec3_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec4_<T> operator/ (_in(Vec4_<T>) _Lhs, _in(T) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator/ (_in(Vec_<N, T>) _Lhs, _in(T) _Rhs);

	template<typename T> Vec2_<T> operator% (_in(Vec2_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec3_<T> operator% (_in(Vec3_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec4_<T> operator% (_in(Vec4_<T>) _Lhs, _in(T) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator% (_in(Vec_<N, T>) _Lhs, _in(T) _Rhs);

	template<typename T> Vec2_<T> operator+ (_in(Vec2_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec3_<T> operator+ (_in(Vec3_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec4_<T> operator+ (_in(Vec4_<T>) _Lhs, _in(Real) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator+ (_in(Vec_<N, T>) _Lhs, _in(Real) _Rhs);

	template<typename T> Vec2_<T> operator- (_in(Vec2_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec3_<T> operator- (_in(Vec3_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec4_<T> operator- (_in(Vec4_<T>) _Lhs, _in(Real) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator- (_in(Vec_<N, T>) _Lhs, _in(Real) _Rhs);

	template<typename T> Vec2_<T> operator* (_in(Vec2_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec3_<T> operator* (_in(Vec3_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec4_<T> operator* (_in(Vec4_<T>) _Lhs, _in(Real) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator* (_in(Vec_<N, T>) _Lhs, _in(Real) _Rhs);

	template<typename T> Vec2_<T> operator/ (_in(Vec2_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec3_<T> operator/ (_in(Vec3_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec4_<T> operator/ (_in(Vec4_<T>) _Lhs, _in(Real) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator/ (_in(Vec_<N, T>) _Lhs, _in(Real) _Rhs);

	template<typename T> Vec2_<T> operator% (_in(Vec2_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec3_<T> operator% (_in(Vec3_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec4_<T> operator% (_in(Vec4_<T>) _Lhs, _in(Real) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator% (_in(Vec_<N, T>) _Lhs, _in(Real) _Rhs);
	/* </ ? vector scalar > */

	/* < ? scalar vector > */
	template<typename T> Vec2_<T> operator* (_in(T) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T> operator* (_in(T) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T> operator* (_in(T) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator* (_in(T) _Lhs, _in(Vec_<N, T>) _Rhs);

	template<typename T> Vec2_<T> operator/ (_in(T) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T> operator/ (_in(T) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T> operator/ (_in(T) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator/ (_in(T) _Lhs, _in(Vec_<N, T>) _Rhs);

	template<typename T> Vec2_<T> operator* (_in(Real) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T> operator* (_in(Real) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T> operator* (_in(Real) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator* (_in(Real) _Lhs, _in(Vec_<N, T>) _Rhs);

	template<typename T> Vec2_<T> operator/ (_in(Real) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T> operator/ (_in(Real) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T> operator/ (_in(Real) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T> operator/ (_in(Real) _Lhs, _in(Vec_<N, T>) _Rhs);
	/* </ ? scalar vector > */

	/* < ? vector matrix > */
	template<typename T> Vec2_<T> operator* (_in(Vec2_<T>) _Lhs, _in(Mat_<2, 2, T>) _Rhs);
	template<typename T> Vec3_<T> operator* (_in(Vec2_<T>) _Lhs, _in(Mat_<2, 3, T>) _Rhs);
	template<typename T> Vec4_<T> operator* (_in(Vec2_<T>) _Lhs, _in(Mat_<2, 4, T>) _Rhs);
	template<typename T> Vec2_<T> operator* (_in(Vec3_<T>) _Lhs, _in(Mat_<3, 2, T>) _Rhs);
	template<typename T> Vec3_<T> operator* (_in(Vec3_<T>) _Lhs, _in(Mat_<3, 3, T>) _Rhs);
	template<typename T> Vec4_<T> operator* (_in(Vec3_<T>) _Lhs, _in(Mat_<3, 4, T>) _Rhs);
	template<typename T> Vec2_<T> operator* (_in(Vec4_<T>) _Lhs, _in(Mat_<4, 2, T>) _Rhs);
	template<typename T> Vec3_<T> operator* (_in(Vec4_<T>) _Lhs, _in(Mat_<4, 3, T>) _Rhs);
	template<typename T> Vec4_<T> operator* (_in(Vec4_<T>) _Lhs, _in(Mat_<4, 4, T>) _Rhs);
	template<size_t N, typename T, size_t P> Vec_<P, T> operator* (_in(Vec_<N, T>) _Lhs, _in(Mat_<N, P, T>) _Rhs);
	/* </ ? vector matrix > */

	/* < ?= vector vector > */
	template<typename T> Vec2_<T>& operator+= (_inout(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T>& operator+= (_inout(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T>& operator+= (_inout(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T>& operator+= (_inout(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs);

	template<typename T> Vec2_<T>& operator-= (_inout(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T>& operator-= (_inout(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T>& operator-= (_inout(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T>& operator-= (_inout(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs);

	template<typename T> Vec2_<T>& operator*= (_inout(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T>& operator*= (_inout(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T>& operator*= (_inout(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T>& operator*= (_inout(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs);

	template<typename T> Vec2_<T>& operator/= (_inout(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T>& operator/= (_inout(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T>& operator/= (_inout(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T>& operator/= (_inout(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs);

	template<typename T> Vec2_<T>& operator%= (_inout(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T>& operator%= (_inout(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T>& operator%= (_inout(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T>& operator%= (_inout(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs);
	/* </ ?= vector vector > */

	/* < ?= vector scalar > */
	template<typename T> Vec2_<T>& operator*= (_inout(Vec2_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec3_<T>& operator*= (_inout(Vec3_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec4_<T>& operator*= (_inout(Vec4_<T>) _Lhs, _in(T) _Rhs);
	template<size_t N, typename T> Vec_<N, T>& operator*= (_inout(Vec_<N, T>) _Lhs, _in(T) _Rhs);

	template<typename T> Vec2_<T>& operator/= (_inout(Vec2_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec3_<T>& operator/= (_inout(Vec3_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec4_<T>& operator/= (_inout(Vec4_<T>) _Lhs, _in(T) _Rhs);
	template<size_t N, typename T> Vec_<N, T>& operator/= (_inout(Vec_<N, T>) _Lhs, _in(T) _Rhs);

	template<typename T> Vec2_<T>& operator%= (_inout(Vec2_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec3_<T>& operator%= (_inout(Vec3_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec4_<T>& operator%= (_inout(Vec4_<T>) _Lhs, _in(T) _Rhs);
	template<size_t N, typename T> Vec_<N, T>& operator%= (_inout(Vec_<N, T>) _Lhs, _in(T) _Rhs);

	template<typename T> Vec2_<T>& operator*= (_inout(Vec2_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec3_<T>& operator*= (_inout(Vec3_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec4_<T>& operator*= (_inout(Vec4_<T>) _Lhs, _in(Real) _Rhs);
	template<size_t N, typename T> Vec_<N, T>& operator*= (_inout(Vec_<N, T>) _Lhs, _in(Real) _Rhs);

	template<typename T> Vec2_<T>& operator/= (_inout(Vec2_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec3_<T>& operator/= (_inout(Vec3_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec4_<T>& operator/= (_inout(Vec4_<T>) _Lhs, _in(Real) _Rhs);
	template<size_t N, typename T> Vec_<N, T>& operator/= (_inout(Vec_<N, T>) _Lhs, _in(Real) _Rhs);

	template<typename T> Vec2_<T>& operator%= (_inout(Vec2_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec3_<T>& operator%= (_inout(Vec3_<T>) _Lhs, _in(Real) _Rhs);
	template<typename T> Vec4_<T>& operator%= (_inout(Vec4_<T>) _Lhs, _in(Real) _Rhs);
	template<size_t N, typename T> Vec_<N, T>& operator%= (_inout(Vec_<N, T>) _Lhs, _in(Real) _Rhs);
	/* </ ?= vector scalar > */

	/* < cout vector > */
	template<typename T> std::ostream& operator<< (_inout(std::ostream) _Ostr, _in(Vec2_<T>) _Lhs);
	template<typename T> std::ostream& operator<< (_inout(std::ostream) _Ostr, _in(Vec3_<T>) _Lhs);
	template<typename T> std::ostream& operator<< (_inout(std::ostream) _Ostr, _in(Vec4_<T>) _Lhs);
	template<size_t N, typename T> std::ostream& operator<< (_inout(std::ostream) _Ostr, _in(Vec_<N, T>) _Lhs);
	/* </ cout vector > */

	// elementary function
	/* < pow > */
	template<typename T> Vec2_<T> pow(_in(Vec2_<T>) _Lhs, int _Power);
	template<typename T> Vec3_<T> pow(_in(Vec3_<T>) _Lhs, int _Power);
	template<typename T> Vec4_<T> pow(_in(Vec4_<T>) _Lhs, int _Power);
	template<typename T> Vec2_<T> pow(_in(Vec2_<T>) _Lhs, double _Power);
	template<typename T> Vec3_<T> pow(_in(Vec3_<T>) _Lhs, double _Power);
	template<typename T> Vec4_<T> pow(_in(Vec4_<T>) _Lhs, double _Power);
	template<typename T> Vec2_<T> pow(_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T> pow(_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T> pow(_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T> pow(_in(Vec_<N, T>) _Lhs, int _Power);
	template<size_t N, typename T> Vec_<N, T> pow(_in(Vec_<N, T>) _Lhs, double _Power);
	template<size_t N, typename T> Vec_<N, T> pow(_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs);
	/* </ pow > */

	template<typename T> Vec2_<T> sqrt(_in(Vec2_<T>) _Lhs);
	template<typename T> Vec3_<T> sqrt(_in(Vec3_<T>) _Lhs);
	template<typename T> Vec4_<T> sqrt(_in(Vec4_<T>) _Lhs);
	template<size_t N, typename T> Vec_<N, T> sqrt(_in(Vec_<N, T>) _Lhs);

	template<typename T> Vec2_<T> floor(_in(Vec2_<T>) _Lhs);
	template<typename T> Vec3_<T> floor(_in(Vec3_<T>) _Lhs);
	template<typename T> Vec4_<T> floor(_in(Vec4_<T>) _Lhs);
	template<size_t N, typename T> Vec_<N, T> floor(_in(Vec_<N, T>) _Lhs);

	template<typename T> Vec2_<T> ceil(_in(Vec2_<T>) _Lhs);
	template<typename T> Vec3_<T> ceil(_in(Vec3_<T>) _Lhs);
	template<typename T> Vec4_<T> ceil(_in(Vec4_<T>) _Lhs);
	template<size_t N, typename T> Vec_<N, T> ceil(_in(Vec_<N, T>) _Lhs);

	template<typename T> Vec2_<T> mod(_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T> mod(_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T> mod(_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> Vec_<N, T> mod(_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs);
	template<typename T> Vec2_<T> mod(_in(Vec2_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec3_<T> mod(_in(Vec3_<T>) _Lhs, _in(T) _Rhs);
	template<typename T> Vec4_<T> mod(_in(Vec4_<T>) _Lhs, _in(T) _Rhs);
	template<size_t N, typename T> Vec_<N, T> mod(_in(Vec_<N, T>) _Lhs, _in(T) _Rhs);

	/* < trianglar function > */
	template<typename T> Vec2_<T> sin(_in(Vec2_<T>) _Lhs);
	template<typename T> Vec3_<T> sin(_in(Vec3_<T>) _Lhs);
	template<typename T> Vec4_<T> sin(_in(Vec4_<T>) _Lhs);

	template<typename T> Vec2_<T> cos(_in(Vec2_<T>) _Lhs);
	template<typename T> Vec3_<T> cos(_in(Vec3_<T>) _Lhs);
	template<typename T> Vec4_<T> cos(_in(Vec4_<T>) _Lhs);

	template<typename T> Vec2_<T> tan(_in(Vec2_<T>) _Lhs);
	template<typename T> Vec3_<T> tan(_in(Vec3_<T>) _Lhs);
	template<typename T> Vec4_<T> tan(_in(Vec4_<T>) _Lhs);

	template<typename T> Vec2_<T> asin(_in(Vec2_<T>) _Lhs);
	template<typename T> Vec3_<T> asin(_in(Vec3_<T>) _Lhs);
	template<typename T> Vec4_<T> asin(_in(Vec4_<T>) _Lhs);

	template<typename T> Vec2_<T> acos(_in(Vec2_<T>) _Lhs);
	template<typename T> Vec3_<T> acos(_in(Vec3_<T>) _Lhs);
	template<typename T> Vec4_<T> acos(_in(Vec4_<T>) _Lhs);

	template<typename T> Vec2_<T> atan(_in(Vec2_<T>) _Lhs);
	template<typename T> Vec3_<T> atan(_in(Vec3_<T>) _Lhs);
	template<typename T> Vec4_<T> atan(_in(Vec4_<T>) _Lhs);

	template<size_t N, typename T> Vec_<N, T> sin(_in(Vec_<N, T>) _Lhs);
	template<size_t N, typename T> Vec_<N, T> cos(_in(Vec_<N, T>) _Lhs);
	template<size_t N, typename T> Vec_<N, T> tan(_in(Vec_<N, T>) _Lhs);
	template<size_t N, typename T> Vec_<N, T> asin(_in(Vec_<N, T>) _Lhs);
	template<size_t N, typename T> Vec_<N, T> acos(_in(Vec_<N, T>) _Lhs);
	template<size_t N, typename T> Vec_<N, T> atan(_in(Vec_<N, T>) _Lhs);
	/* </ trianglar function > */

	template<typename T> T dot(_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> T dot(_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> T dot(_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs);
	template<size_t N, typename T> T dot(_in(Vec_<N, T>) _Lhs, _in(Vec_<N, T>) _Rhs);

	template<typename T> Vec2_<T> cross(_in(Vec2_<T>) _Lhs, _in(Vec2_<T>) _Rhs);
	template<typename T> Vec3_<T> cross(_in(Vec3_<T>) _Lhs, _in(Vec3_<T>) _Rhs);
	template<typename T> Vec4_<T> cross(_in(Vec4_<T>) _Lhs, _in(Vec4_<T>) _Rhs, _in(Vec4_<T>) _Rhs2);
	template<typename T> Vec_<3, T> cross(const Vec_<3, T>& _Lhs, const Vec_<3, T>& _Rhs);

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

#define CLMAGIC_OPERATOR_COMM_VECTOR(COMMRETURN, CASTRETURN, LTYPE, RTYPE, LCAST, RCAST) \
	inline COMMRETURN operator+ (LTYPE _Lhs, RTYPE _Rhs) \
		{ return ( CASTRETURN( LCAST(_Lhs) + RCAST(_Rhs) ) ); } \
	\
	inline COMMRETURN operator+ (RTYPE _Rhs, LTYPE _Lhs) \
		{ return ( CASTRETURN( RCAST(_Rhs) + LCAST(_Lhs) ) ); } \
	\
	inline COMMRETURN operator- (LTYPE _Lhs, RTYPE _Rhs) \
		{ return ( CASTRETURN( LCAST(_Lhs) - RCAST(_Rhs) ) ); } \
	\
	inline COMMRETURN operator- (RTYPE _Rhs, LTYPE _Lhs) \
		{ return ( CASTRETURN( RCAST(_Rhs) - LCAST(_Lhs) ) ); } \
	\
	inline COMMRETURN operator* (LTYPE _Lhs, RTYPE _Rhs) \
		{ return ( CASTRETURN( LCAST(_Lhs) * RCAST(_Rhs) ) ); } \
	\
	inline COMMRETURN operator* (RTYPE _Rhs, LTYPE _Lhs) \
		{ return ( CASTRETURN( RCAST(_Rhs) * LCAST(_Lhs) ) ); } \
	\
	inline COMMRETURN operator/ (LTYPE _Lhs, RTYPE _Rhs) \
		{ return ( CASTRETURN( LCAST(_Lhs) / RCAST(_Rhs) ) ); } \
	\
	inline COMMRETURN operator/ (RTYPE _Rhs, LTYPE _Lhs) \
		{ return ( CASTRETURN( RCAST(_Rhs) / LCAST(_Lhs) ) ); } \
	\
	inline COMMRETURN operator% (LTYPE _Lhs, RTYPE _Rhs) \
		{ return ( CASTRETURN( LCAST(_Lhs) % RCAST(_Rhs) ) ); } \
	\
	inline COMMRETURN operator% (RTYPE _Rhs, LTYPE _Lhs) \
		{ return ( CASTRETURN( RCAST(_Rhs) % LCAST(_Lhs) ) ); } \

	
	CLMAGIC_OPERATOR_COMM_VECTOR(Vec2, , Vec2 const&, Vec_2f const&, , reference_cast<Vec2 const>)
	CLMAGIC_OPERATOR_COMM_VECTOR(Vec3, , Vec3 const&, Vec_3f const&, , reference_cast<Vec3 const>)
	CLMAGIC_OPERATOR_COMM_VECTOR(Vec4, , Vec4 const&, Vec_4f const&, , reference_cast<Vec4 const>)

	CLMAGIC_OPERATOR_COMM_VECTOR(Vec2i, , Vec2i const&, Vec_2i const&, , reference_cast<Vec2i const>)
	CLMAGIC_OPERATOR_COMM_VECTOR(Vec3i, , Vec3i const&, Vec_3i const&, , reference_cast<Vec3i const>)
	CLMAGIC_OPERATOR_COMM_VECTOR(Vec4i, , Vec4i const&, Vec_4i const&, , reference_cast<Vec4i const>)

	CLMAGIC_OPERATOR_COMM_VECTOR(Vec2d, , Vec2d const&, Vec_2d const&, , reference_cast<Vec2d const>)
	CLMAGIC_OPERATOR_COMM_VECTOR(Vec3d, , Vec3d const&, Vec_3d const&, , reference_cast<Vec3d const>)
	CLMAGIC_OPERATOR_COMM_VECTOR(Vec4d, , Vec4d const&, Vec_4d const&, , reference_cast<Vec4d const>)

	
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