#include "../src/clmagic/math.h"
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

void test_foundation() {
	using namespace::clmagic;

#define OUTPUT( X ) std::cout << X << std::endl

	constexpr Vec4 _T_constexpr{ real_t(4000), real_t(2122), real_t(22222222222222), real_t(1111111) };
	OUTPUT( _T_constexpr );

	Vec_<9, real_t> _T_constexpr_2 = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f };
	OUTPUT( _T_constexpr_2 );

	Vec3 _T_downward_convert_1 = _T_constexpr_2;
	OUTPUT(_T_downward_convert_1);

	Vec2 _T_downward_convert_2 = _T_constexpr;
	OUTPUT(_T_downward_convert_2);

	Vec2 _T_upward_convert_1 = { 1.f, 2.f };
	Vec3 _T_upward_convert_2 = Vec3(_T_upward_convert_1, 3.f);
	Vec4 _T_upward_convert_3 = Vec4(_T_upward_convert_2, 4.f);
	Vec_<4,  real_t> _T_VecN_convert_1 = Vec_<4, real_t>(_T_upward_convert_3);
	Vec_<13, real_t> _T_VecN_convert_2 = _T_VecN_convert_1;
	Vec_<13, real_t> _T_VecN_convert_3 = Vec_<13, real_t>(_T_VecN_convert_2.begin(), _T_VecN_convert_2.end());
	OUTPUT(_T_upward_convert_1);
	OUTPUT(_T_upward_convert_2);
	OUTPUT(_T_upward_convert_3);
	OUTPUT(_T_VecN_convert_1);
	OUTPUT(_T_VecN_convert_2);
	OUTPUT(_T_VecN_convert_3);
	OUTPUT(_T_VecN_convert_3.V<6>("xyzwxx"));
	OUTPUT( pow(_T_VecN_convert_2, 2) );
	OUTPUT( pow(_T_VecN_convert_3, 2) );
}

void test_general() {
	using namespace::clmagic;
	float A = 7.f / 5.f;
	float B = 14.f / 25.f;
	float t0, t1;
	quadratic(A, B, t0, t1);
	sort(t0, t1);
	std::cout << t0 << " " << t1 << std::endl;

	quadratic(A, B, 0.f, t0, t1);
	sort(t0, t1);
	std::cout << t0 << " " << t1 << std::endl;
}


#pragma comment(lib, "opencv_world411d.lib")
clmagic::Vec4 fragment_stage(_in(clmagic::Vec2) _Texcoord, clmagic::real_t t);
void test_raytracing() {
	using namespace::clmagic;
	using namespace::std::chrono;
	microseconds _Time_start = duration_cast<microseconds>(system_clock::now().time_since_epoch());
	decltype(_Time_start) _Time_current;

	std::string _Window_name = "spherical";
	int _Window_width = 200, _Window_height = 200;
	cv::namedWindow(_Window_name);
	while (true) {
		_Time_current = duration_cast<microseconds>(system_clock::now().time_since_epoch());
		real_t t = 0.001f * static_cast<real_t>((_Time_current - _Time_start).count());

		cv::Mat _Src(_Window_height, _Window_width, CV_32FC4);
		for (int i = 0; i != _Src.rows; ++i) {
			for (int j = 0; j != _Src.cols; ++j) {
				auto _Texcoord = Vec2(float(j), float(i));
				     _Texcoord = _Texcoord * 2.f - Vec2(float(_Window_width), float(_Window_height));
				auto _Color = fragment_stage( _Texcoord, t * 0.01f );
				_Src.at<cv::Vec4f>(i, j) = cv::Vec4f(_Color.b, _Color.g, _Color.r, _Color.a);
			}
		}
		cv::imshow(_Window_name, _Src);
		cv::waitKey(2);
	}

}





clmagic::Vec4 fragment_stage( _in(clmagic::Vec2) _Texcoord, clmagic::real_t t) {
	using namespace::clmagic;

	Ray_<real_t> _Myeye;
	_Myeye.o = Vec3_<real_t>(_Texcoord, -10.f);
	_Myeye.d = Vec3_<real_t>(0.f, 0.f, 1.f);

	Sphere_<real_t> _Test_obj1;
	_Test_obj1.r = 66.f;
	_Test_obj1.world = rotation_matrix(cY, t);

	Hit_<real_t> _Hit;

	if ( intersect(_Myeye, _Test_obj1, _Hit) ) {
		return ( Vec4(_Hit._Myuv, 0.f, 1.f) );
	}

	return ( Vec4(0.f) );
}


int main(int argc, char** argv) {
	std::cout << "test_clmagic_math" << std::endl;
	std::cout << clmagic::cX << std::endl;

	test_foundation();
	test_general();
	test_raytracing();

	

	std::cin.get();
	return (0);
}