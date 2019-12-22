#include "../src/clmagic/math/simd.h"
#include <iostream>
#include <opencv.hpp>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/fwd.hpp"


/*
a * x^2 + b * x + c = 0

x^2 - a(3x-2a+b)-b^2 = 0
x^2 - 3*x*a + 2*a^2 - a*b - b^2 = 0
x^2 - x*(3*a) + (2*a^2-a*b-b^2) = 0
A = 1
B = -3a
C = 3*a^2-ab-b^2

3a¡À¡Ì(9a-12*a^2-4ab-4*b^2) 
---------------
      2
*/

//void test_foundation() {
//	using namespace::clmagic;
//
//#define OUTPUT( X ) std::cout << X << std::endl
//
//	constexpr Vec4 _T_constexpr{ real_t(4000), real_t(2122), real_t(22222222222222), real_t(1111111) };
//	OUTPUT( _T_constexpr );
//
//	Vec_<9, real_t> _T_constexpr_2 = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f };
//	OUTPUT( _T_constexpr_2 );
//
//	Vec3 _T_downward_convert_1 = _T_constexpr_2;
//	OUTPUT(_T_downward_convert_1);
//
//	Vec2 _T_downward_convert_2 = _T_constexpr;
//	OUTPUT(_T_downward_convert_2);
//
//	Vec2 _T_upward_convert_1 = { 1.f, 2.f };
//	Vec3 _T_upward_convert_2 = Vec3(_T_upward_convert_1, 3.f);
//	Vec4 _T_upward_convert_3 = Vec4(_T_upward_convert_2, 4.f);
//	Vec_<4,  real_t> _T_VecN_convert_1 = Vec_<4, real_t>(_T_upward_convert_3);
//	Vec_<13, real_t> _T_VecN_convert_2 = _T_VecN_convert_1;
//	Vec_<13, real_t> _T_VecN_convert_3 = Vec_<13, real_t>(_T_VecN_convert_2.begin(), _T_VecN_convert_2.end());
//	OUTPUT(_T_upward_convert_1);
//	OUTPUT(_T_upward_convert_2);
//	OUTPUT(_T_upward_convert_3);
//	OUTPUT(_T_VecN_convert_1);
//	OUTPUT(_T_VecN_convert_2);
//	OUTPUT(_T_VecN_convert_3);
//	OUTPUT(_T_VecN_convert_3.V<6>("xyzwxx"));
//	OUTPUT( pow(_T_VecN_convert_2, 2) );
//	OUTPUT( pow(_T_VecN_convert_3, 2) );
//}
//
//void test_general() {
//	using namespace::clmagic;
//	float A = 7.f / 5.f;
//	float B = 14.f / 25.f;
//	float t0, t1;
//	quadratic(A, B, t0, t1);
//	sort(t0, t1);
//	std::cout << t0 << " " << t1 << std::endl;
//
//	quadratic(A, B, 0.f, t0, t1);
//	sort(t0, t1);
//	std::cout << t0 << " " << t1 << std::endl;
//}
//
//
#pragma comment(lib, "opencv_world411d.lib")
//class CpuShaderProgram {
//	std::vector<char> memory_pool;
//	char* memory_top;
//
//};
//
//clmagic::Vec4 fragment_stage(_in(clmagic::Vec2) _Texcoord, clmagic::real_t t);
//void test_raytracing() {
//	using namespace::clmagic;
//	using namespace::std::chrono;
//	microseconds _Time_start = duration_cast<microseconds>(system_clock::now().time_since_epoch());
//	decltype(_Time_start) _Time_current;
//
//	std::string _Window_name = "spherical";
//	int _Window_width = 200, _Window_height = 200;
//	cv::namedWindow(_Window_name);
//	while (true) {
//		_Time_current = duration_cast<microseconds>(system_clock::now().time_since_epoch());
//		real_t t = 0.001f * static_cast<real_t>((_Time_current - _Time_start).count());
//
//		cv::Mat _Src(_Window_height, _Window_width, CV_32FC4);
//		for (int i = 0; i != _Src.rows; ++i) {
//			for (int j = 0; j != _Src.cols; ++j) {
//				auto _Texcoord = Vec2(float(j), float(i));
//				     _Texcoord = _Texcoord * 2.f - Vec2(float(_Window_width), float(_Window_height));
//				auto _Color = fragment_stage( _Texcoord, t * 0.01f );
//				_Src.at<cv::Vec4f>(i, j) = cv::Vec4f(_Color.b, _Color.g, _Color.r, _Color.a);
//			}
//		}
//		cv::imshow(_Window_name, _Src);
//		cv::waitKey(2);
//	}
//
//}
//
//
//
//
//
//clmagic::Vec4 fragment_stage( _in(clmagic::Vec2) _Texcoord, clmagic::real_t t) {
//	using namespace::clmagic;
//
//	Ray_<real_t> _Myeye;
//	_Myeye.o = Vec3_<real_t>(_Texcoord, -10.f);
//	_Myeye.d = Vec3_<real_t>(0.f, 0.f, 1.f);
//
//	Sphere_<real_t> _Test_obj1;
//	_Test_obj1.r = 66.f;
//	_Test_obj1.world = Mat4(Quaternion(cY, t));
//
//	Hit_<real_t> _Hit;
//
//	if ( intersect(_Myeye, _Test_obj1, _Hit) ) {
//		return ( Vec4(_Hit._Myuv, 0.f, 1.f) );
//	}
//
//	return ( Vec4(0.f) );
//}

#include <random>

struct Test_Moroc {
#define Operator_AA(AAA) void output##AAA(){std::cout <<#AAA<<std::endl; }
	Operator_AA(1111);
#undef Operator_AA
};

void test_simd() {
	Test_Moroc _test_coroc;
	_test_coroc.output1111();


#define Ouput_F32vec4(Vec) std::cout << (Vec)[0] << "," << (Vec)[1] << "," << (Vec)[2] << "," << (Vec)[3] << std::endl
	F32vec4 _A(1.f, 2.f, 3.f, 4.f);
	F32vec4 _B(5.f);
	auto _C = _A * _B;
	Ouput_F32vec4(_C);
	std::cout << simd_dot(_A, _B) << std::endl;
	F32mat4 _Mat(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f);
	Ouput_F32vec4(_Mat * _A);
	char* _Memory = new char[1024 * 16];
	
	F32mat4* _Mat_arr = cv::alignPtr((F32mat4*)_Memory);
	std::cout << _Mat_arr << std::endl;

	F64vec4 _Test_shuffle{ 10.f, 11.f, 12.f, 13.f };
	Ouput_F32vec4(_Test_shuffle);

#undef Ouput_F32vec4

	using namespace::clmagic;
	SIMDVec4i _SIMDInteger{ 11, 12, 13, 14 };
	SIMDVec4f _SIMDFloat{ 9.f, 8.f, 7.f, 6.f };
	SIMDVec4d _SIMDDouble{ 1.0, 2.0, 3.0, 4.0 };
	//std::cout << _SIMDInteger << std::endl;
	std::cout << _SIMDFloat << std::endl;
	std::cout << _SIMDDouble << std::endl;
	std::cout << (_SIMDFloat * 5.f) << std::endl;
	
	std::cout << &_SIMDInteger << std::endl;
	std::cout << &_SIMDFloat << std::endl;
	std::cout << &_SIMDDouble << std::endl;

	std::cout << simd_cross3(SIMDVec4d(0.0, 5.0, 0, 0), SIMDVec4d(5.0, 0.0, 0.0, 0)) << std::endl;
}


template<typename _OutIt, typename _InIt>
_OutIt merge(_OutIt _Desc, std::pair<_InIt, _InIt> _Pairit) 
	{
	while (_Pairit.first != _Pairit.second) 
		{
		*_Desc++ = *(_Pairit.first)++;
		}
	return (_Desc);
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
_OutIt merge(_OutIt _Desc, std::pair<_InIt, _InIt> _Pairit, _Iters... _More) 
	{
	while (_Pairit.first != _Pairit.second) 
		{
		*_Desc++ = *(_Pairit.first)++;
		}
	return ( merge(_Desc, _More...) );
	}



template<typename _Elem, typename _InIt, typename _InIt2, typename _InIt3>
std::basic_string<_Elem> _Replace_range(_in(std::basic_string<_Elem>) _Cont, _InIt _First, _InIt _Last,
	_InIt2 _Old_first, _InIt2 _Old_last, _InIt3 _New_first, _InIt3 _New_last) 
	{	// replace _Cont [_Old_first, _Old_last) to [_New_first, _New_last) in [_First, _Last)
	const auto _N = static_cast<size_t>(std::distance(_First, _Last));
	const auto _M = static_cast<size_t>(std::distance(_Old_first, _Old_last));
	const auto _P = static_cast<size_t>(std::distance(_New_first, _New_last));

	auto _Result = std::basic_string<_Elem>(_N, _Elem());
	auto _Result_it = _Result.begin();

	const auto _Last2 = std::prev(_Last, _M);
	while ( _First != _Last2)
		{	// _First to _Last - distance(_Old_first, _Old_last)
		if ( std::equal(_Old_first, _Old_last, _First) ) 
			{
			_Result_it = _Result.insert(_Result_it, _New_first, _New_last);
			std::advance(_Result_it, _P);
			std::advance(_First,     _M);
			} 
		else 
			{
			*_Result_it++ = *_First++;
			}
		}

	_Result_it = std::move(_Last2, _Last, _Result_it);// 

	return ( std::basic_string<_Elem>(_Result.begin(), _Result_it) );
	}

template<typename _Elem, typename _Iter, typename _InIt2, typename _InIt3>
std::basic_string<_Elem>& str_replace(_inout(std::basic_string<_Elem>) _Cont, _Iter _First, _Iter _Last,
	_InIt2 _Old_first, _InIt2 _Old_last, _InIt3 _New_first, _InIt3 _New_last) 
	{	// replace _Cont [_Old_first, _Old_last) to [_New_first, _New_last) in [_First, _Last)
	_Cont.replace(_First, _Last, _Replace_range(_Cont, _First, _Last, 
		_Old_first, _Old_last, _New_first, _New_last) );
	return ( _Cont );
	}

template<typename _Elem, typename _Iter>
std::basic_string<_Elem>& str_replace(_inout(std::basic_string<_Elem>) _Cont, _Iter _First, _Iter _Last, 
	const char* _Old, const char* _New)
	{	// replace _Cont _Old to _New in [_First, _Last)
	return ( str_replace(_Cont, _First, _Last, /*first*/_Old, /*last*/_Old + strlen(_Old), /*first*/_New, /*last*/_New + strlen(_New)) );
	}

template<typename _Elem, typename _InIt, typename _InIt2, typename _InIt3>
std::basic_string<_Elem> str_replace_copy(_in(std::basic_string<_Elem>) _Cont, _InIt _First, _InIt _Last,
	_InIt2 _Old_first, _InIt2 _Old_last, _InIt3 _New_first, _InIt3 _New_last) 
	{
	auto _Clone = _Cont;

	if (_First == _Cont.begin() && _Last == _Cont.end()) 
		{
		return (str_replace(_Clone, _Clone.begin(), _Clone.end(), _Old_first, _Old_last, _New_first, _New_last));
		}
	else 
		{
		auto _Off_first = static_cast<size_t>(std::distance( _Cont.begin(), decltype(_Cont.begin())(_First) ));
		auto _Off_last  = static_cast<size_t>(std::distance( decltype(_Cont.begin())(_Last), _Cont.end() ));
		str_replace(_Clone, std::next(_Clone.begin(), _Off_first), std::prev(_Clone.end(), _Off_last),
			_Old_first, _Old_last, _New_first, _New_last);
		return (_Clone);
		}
	}

template<typename _Elem, typename _InIt>
std::basic_string<_Elem> str_replace_copy(_in(std::basic_string<_Elem>) _Cont, _InIt _First, _InIt _Last, 
	const char* _Old, const char* _New)
	{	// replace _Cont _Old to _New in [_First, _Last)
	return (str_replace_copy(_Cont, _First, _Last, /*first*/_Old, /*last*/_Old + strlen(_Old), /*first*/_New, /*last*/_New + strlen(_New)));
	}


int main(int argc, char** argv) {

	/*std::cout << "test_clmagic_math" << std::endl;
	std::cout << clmagic::cX << std::endl;*/

	//test_simd();
	std::string _Template = "Vec2_& operator##OP##(_in(Vec2_) _B) { x ##OP## _B.x; y ##OP## _B.y; return (*this); }";
	std::string _Old = "##OP##";
	std::string _Result;

	std::vector<std::string> _Operators = { "+=", "-=", "*=", "/=" };
	for (auto _Op : _Operators) {
		_Result = str_replace_copy(_Template, _Template.begin(), _Template.end(), _Old.c_str(), _Op.c_str());
		std::cout << _Result << std::endl;
	}

	std::cin.get();
	return (0);
}