#include "../src/clmagic/math.h"
#include "../src/clmagic/basic.h"
#include <array>
#include <bitset>
#include <chrono>
#include <random>
#include <iomanip>
#include <iostream>
#include <opencv.hpp>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/noise.hpp"

#pragma comment(lib, "lib/opencv_world411d.lib")

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
//#pragma comment(lib, "opencv_world411d.lib")
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
//		cv::mat _Src(_Window_height, _Window_width, CV_32FC4);
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
//	_Test_obj1.world = mat4(Quaternion(cY, t));
//
//	Hit_<real_t> _Hit;
//
//	if ( intersect(_Myeye, _Test_obj1, _Hit) ) {
//		return ( Vec4(_Hit._Myuv, 0.f, 1.f) );
//	}
//
//	return ( Vec4(0.f) );
//}

#ifdef __cuda_cuda_h__

#endif




size_t test_func(size_t i) {
	return i;
}




//
//template<typename _Ret, typename ..._Args>
//struct has_arguments : public std::bool_constant<true> {
//};

template<typename _Fn>
void output_func(_Fn _Func) {
	std::cout << std::is_floating_point_v<void (*)(int)> << std::endl;
	std::cout << std::is_function_v<void (*)(int)>;
	std::cout << std::is_member_function_pointer_v<_Fn> << std::endl;
	std::cout << clmagic::has_arguments_v<_Fn> << std::endl;
}

void task_1() {	
	using namespace::clmagic;
	dmatrix<3, 3> A1 = {
		4,  3, 1,
		1, -2, 3,
		5,  7, 0
	};
	dmatrix<3, 1> B1 = {
		7,
		2,
		1
	};
	std::cout << "\n1. = "<< A1 * B1 << std::endl;

	dmatrix<1, 3> A2 = { 1, 2, 3 };
	dmatrix<3, 1> B2 = { 3, 2, 1 };
	std::cout << "\n2. = " << A2 * B2 << std::endl;

	dmatrix<3, 1> A3 = { 
		2,
		1, 
		3 };
	dmatrix<1, 2> B3 = {
		-1, 2
	};
	std::cout << "\n3. = " << A3 * B3 << std::endl;

	dmatrix<2, 4> A4 = {
		2, 1, 4, 0,
		1, -1, 3, 4
	};
	dmatrix<4, 3> B4 = {
		1,  3, 1,
		0, -1, 2,
		1, -3, 1,
		4,  0, -2
	};
	std::cout << "\n4. = " << A4 * B4 << std::endl;
}

void task_2() {
	using namespace::clmagic;
	dmatrix<3, 3> A = {
		1,  1,  1,
		1,  1, -1,
		1, -1,  1
	};

	dmatrix<3, 3> B = {
		 1,  2, 3,
		-1, -2, 4,
		 0,  5, 1
	};

	std::cout << "\n2." << std::endl;
	std::cout << (A * B * 3 - A * 2) << std::endl;
	std::cout << transpose(A) * B << std::endl;
}

void task_3() {
	using namespace::clmagic;
	square_matrix<double, 2> A = {
		1, 2,
		1, 3
	};
	square_matrix<double, 2> B = {
		1, 0,
		1, 2
	};

	std::cout << "\n3." << std::endl;
	std::cout << "A*B=" << A * B << std::endl;
	std::cout << "B*A" << B * A << std::endl;
	std::cout << "(A+B)²=" << pow(A + B, 2) << std::endl;
	std::cout << "A²+2AB+B²=" << pow(A, 2) +2*A*B + pow(B, 2) << std::endl;
	std::cout << "(A+B)(A-B)=" << (A + B) * (A - B) << std::endl;
	std::cout << "A²-B²=" << pow(A, 2) - pow(B, 2) << std::endl;
}

void task_4() {
	using namespace::clmagic;
	dmatrix<3, 3> A = {
		 2,  1, -3,
		 1,  2, 10,
		-3, 10,  2
	};
	dmatrix<3, 3> B = {
		4, 2, 13,
		4, 7, 6,
		22, 8, 9
	};

	std::cout << "\n4." << std::endl;
	std::cout << B * A * transpose(B) << std::endl;
	std::cout << transpose(B) * A * B << std::endl;
}

void task_5() {
	using namespace::clmagic;

	std::cout << "\n5." << std::endl;

	dmatrix<2, 2> A = {
		1, 2,
		2, 5
	};
	std::cout << inverse(A) << std::endl;

	dmatrix<3, 3> A2 = {
		1, 2, -1,
		3, 4, -2,
		5, -4, 1
	};
	std::cout << inverse(A2) << std::endl;
}

void task_6() {
	using namespace::clmagic;
	std::cout << "\n6." << std::endl;

	// AX = B
	dmatrix<2, 2> A = {
		2, 5,
		1, 3
	};
	dmatrix<2, 2> B = {
		4, -6,
		2,  1
	};
	std::cout << inverse(A) * B << std::endl;

	// XA=B
	dmatrix<3, 3> A1 = {
		2, 1, -1,
		2, 1,  0,
		1, -1, 1
	};
	dmatrix<2, 3> B1 = {
		1, -1, 3,
		4,  3, 2
	};
	std::cout << B1 * inverse(A1) << std::endl;

	// AXB = C
	dmatrix<2, 2> A2 = {
		1, 4,
		-1, 2
	};
	dmatrix<2, 2> B2 = {
		 2, 0, 
		-1, 1
	};
	dmatrix<2, 2> C2 = {
		3, 1, 
		0, -1
	};
	std::cout << inverse(A2) * C2 * inverse(B2) << std::endl;
}

void test() {
	using namespace::clmagic;

	dmatrix<4, 4> A = {
		1, 1, 1, 1,
		2, 3, 4, 5,
		2*2, 3*3, 4*4, 5*5,
		2*2*2, 3*3*3, 4*4*4, 5*5*5
	};
	std::cout << A << std::endl;

	dmatrix<4, 5> Ab = {
		1, 1, 1, 1, 0,
		2, 3, 4, 5, 0,
		2 * 2, 3 * 3, 4 * 4, 5 * 5, 0,
		2 * 2 * 2, 3 * 3 * 3, 4 * 4 * 4, 5 * 5 * 5, 0
	};
	Ab.col(4).assign(std::initializer_list<double>{7.0, 7.0, 7.0, 4.0});
	std::cout << Ab << std::endl;
	auto Ab_pos = major_iterator< dmatrix<4, 5>::common_matrix_type>(Ab);
	while ( matrix_row_transform<dmatrix<4, 5>::common_matrix_type>::solve_down<true>( Ab, Ab_pos) ) {}
	while (matrix_row_transform<dmatrix<4, 5>::common_matrix_type>::solve_up<true>(Ab, Ab_pos)) {}
	std::cout << Ab << std::endl;
	std::cout << std::endl;

	auto L           = dmatrix<4, 4>(1.0);
	auto _Lu_process = LU<double, 4, normal_vector_tag>();
	if (_Lu_process(A, L, A)) {
		std::cout << L << std::endl;
		std::cout << A << std::endl;
		std::cout << L * A  << std::endl;

		dmatrix<4, 1> b = {
			7, 7, 7, 4
		};

		auto x = _Lu_process(L, A, b);
		std::cout << x << std::endl;
	}

	/*std::cout << A << std::endl;
	dmatrix<4, 4> Ainv = dmatrix < 4, 4>(1.f);

	using AMat4x8 = _Augmented_matrix<double, 4, 4, 4, normal_vector_tag, normal_vector_tag>;

	auto ALU = AMat4x8(A, Ainv);
	std::cout << ALU << std::endl;
	auto _Pos = major_iterator<AMat4x8>(ALU);

	while (matrix_row_transform<AMat4x8>::solve_down<true>(ALU, _Pos) ) {
		std::cout << ALU << std::endl;
	}
	while (matrix_row_transform<AMat4x8>::solve_up(ALU, _Pos) ) {
		std::cout << ALU << std::endl;
	}*/


	/*dmatrix<4, 4> L;
	dmatrix<4, 4> U;
	for (size_t i = 0; i != 4; ++i) {
		for (size_t j = 0; j != 4; ++j) {
			U.at(i, j) = ALU.at(i, j);
		}
	}
	for (size_t i = 0; i != 4; ++i) {
		for (size_t j = 0; j != 4; ++j) {
			L.at(i, j) = ALU.at(i, j + 4);
		}
	}
	std::cout << "L:" << L << std::endl;
	std::cout << "U:" << U << std::endl;
	std::cout << "LU=" << inverse(L) * U << std::endl;
	std::cout << L * A << std::endl;*/
}

int main() {
	//diagonal_matrix_operation();
	/*task_1();
	task_2();
	task_3();
	task_4();
	task_5();
	task_6();*/
	test();

	std::cin.get();
	return (0);
}