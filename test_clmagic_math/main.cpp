#include "../src/clmagic/math/simd.h"
#include "../src/clmagic/basic.h"
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


//#define Ouput_F32vec4(Vec) std::cout << (Vec)[0] << "," << (Vec)[1] << "," << (Vec)[2] << "," << (Vec)[3] << std::endl
//	F32vec4 _A(1.f, 2.f, 3.f, 4.f);
//	F32vec4 _B(5.f);
//	auto _C = _A * _B;
//	Ouput_F32vec4(_C);
//	std::cout << simd_dot(_A, _B) << std::endl;
//	F32mat4 _Mat(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f);
//	Ouput_F32vec4(_Mat * _A);
//	char* _Memory = new char[1024 * 16];
//	
//	F32mat4* _Mat_arr = cv::alignPtr((F32mat4*)_Memory);
//	std::cout << _Mat_arr << std::endl;
//
//	F64vec4 _Test_shuffle{ 10.f, 11.f, 12.f, 13.f };
//	Ouput_F32vec4(_Test_shuffle);
//
//#undef Ouput_F32vec4

	using namespace::clmagic;
	SIMDVec4f _SIMDFloat{ 9.f, 8.f, 7.f, 6.f };
	SIMDVec4d _SIMDDouble{ 1.0, 2.0, 3.0, 4.0 };
	//std::cout << _SIMDInteger << std::endl;
	std::cout << _SIMDFloat << std::endl;
	std::cout << _SIMDDouble << std::endl;
	std::cout << (_SIMDFloat * 5.f) << std::endl;
	
	std::cout << &_SIMDFloat << std::endl;
	std::cout << &_SIMDDouble << std::endl;

	std::cout << cross3(SIMDVec4d(0.0, 5.0, 0, 0), SIMDVec4d(5.0, 0.0, 0.0, 0)) << std::endl;
}











int main(int argc, char** argv) {
	/*std::cout << "test_clmagic_math" << std::endl;
	std::cout << clmagic::cX << std::endl;*/


	test_simd();
	std::string _Template = "friend Vec3_ operator##OP##(_in(Vec3_) _A, _in(Vec3_) _B){ return (Vec3_(_A.x##OP##_B.x, _A.y##OP##_B.y, _A.z##OP##_B.z)); }";
	std::string _Old = "##OP##";
	std::string _Result;

	std::vector<std::string> _Operators = { "+", "-", "*", "/" };
	for (auto _Op : _Operators) {
		_Result = clmagic::str_replace_copy(_Template, _Template.begin(), _Template.end(), _Old.c_str(), _Op.c_str());
		std::cout << _Result << std::endl;
	}

	std::cin.get();
	return (0);
}