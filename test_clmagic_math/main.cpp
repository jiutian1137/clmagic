﻿#include "../src/clmagic/math.h"
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

void task_linear_related() {
	using namespace::clmagic;

	//dmatrix<3, 2> A;
	//A.col(0).assign(std::initializer_list<double>{0, 1, 1});
	//A.col(1).assign(std::initializer_list<double>{1, 1, 0});

	//dmatrix<3, 3> B;
	//B.col(0).assign(std::initializer_list<double>{ -1, 0, 1 });
	//B.col(1).assign(std::initializer_list<double>{ 1, 2, 1 });
	//B.col(2).assign(std::initializer_list<double>{ 3, 2, -1 });


	//using Amat3x2_3 = augumented_matrix_row_transform<double, 3, 2, double, 3, double>;
	//auto Acopy = A;
	//auto Bcopy = B;
	//Amat3x2_3::solve_until(Acopy, Bcopy);
	//std::cout << Acopy << std::endl;
	//std::cout << Bcopy << std::endl;
	//bool _Ok_1 = Acopy.rank() == Bcopy.rank();

	//using Amat3x3_2 = augumented_matrix_row_transform<double, 3, 3, double, 2, double>;
	//Acopy = A;
	//Bcopy = B;
	//Amat3x3_2::solve_until(Bcopy, Acopy);
	//std::cout << Acopy << std::endl;
	//std::cout << Bcopy << std::endl;
	//bool _Ok_2 = Acopy.rank() == Bcopy.rank();

	//if (_Ok_1 == _Ok_2) {
	//	// solve[A,B] R(A) == R(B),
	//	// solve[B,A] R(B) == R(A)
	//	// vector equalizer
	//	std::cout << "Result: A == B" << std::endl;
	//} else {
	//	std::cout << "Result: A != B" << std::endl;
	//}

	
}

inline double f(double x) {
	return _CSTD pow(2.71828, -x);
}


template<typename _Tp>
void output_A(_Tp* A) {
    for (size_t i = 0; i != 3; ++i) {
        for (size_t j = 0; j != 3; ++j) {
            std::cout << *(A + (3 * i + j)) << " ";
        }
        std::cout << std::endl;
    }
}

template<typename _Tp>
void output_W(_Tp* A) {
    for (size_t i = 0; i != 3; ++i) {
        std::cout << *(A + i) << " ";
        std::cout << std::endl;
    }
}


template<typename _Ty, size_t _Rows>
void Jacobi_help(const clmagic::matrix<_Ty, _Rows, _Rows>& A, size_t k, size_t* indR, size_t* indC) {
    if (k < _Rows - 1) {
        size_t m  = k + 1;
        _Ty    mv = std::abs(A.at(k, m));
        for (size_t i = k + 2; i < _Rows; ++i) {// [k+2, N)
            const auto val = std::abs(A.at(k, i));
            if (mv < val)
                mv = val, m = i;
        }
        indR[k] = m;
    }

    if (k > 0) {// (0, n]
        size_t m  = 0;
        _Ty    mv = std::abs(A.at(0, k));
        for (size_t i = 1; i < k; i++) {// [1, k)
            const auto val = std::abs(A.at(i, k));
            if (mv < val)
                mv = val, m = i;
        }
        indC[k] = m;
    }
}

template<typename _Tp, size_t _Rows> bool
JacobiImpl_(clmagic::matrix<_Tp, _Rows, _Rows>& A, clmagic::matrix<_Tp, _Rows, 1>& W, clmagic::matrix<_Tp, _Rows, _Rows>* V) {
    // from OpenCV-library www.opencv.org
    const _Tp eps = std::numeric_limits<_Tp>::epsilon();
    int i, k;

    if (V != nullptr)  {
        V->assign(static_cast<_Tp>(1));
    }

    int iters, maxIters = _Rows * _Rows * 30;

    size_t indR[_Rows];
    size_t indC[_Rows];

    for (k = 0; k < _Rows; k++)
    {
        W.at(k, 0) = A.at(k, k);
        Jacobi_help(A, k, indR, indC);
      
    }

    _Tp mv;

    if (_Rows > 1) for (iters = 0; iters < maxIters; iters++) {
        // find index (k,l) of pivot p

        for (k = 0, mv = std::abs(A.at(0, indR[0])), i = 1; i < _Rows - 1; i++) {
            _Tp val = std::abs(A.at(i, indR[i]));
            if (mv < val)
                mv = val, k = i;
        }
        int l = indR[k];
        for (i = 1; i < _Rows; i++) {
            _Tp val = std::abs(A.at(indC[i], i));
            if (mv < val)
                mv = val, k = indC[i], l = i;
        }

        _Tp p = A.at(k, l);
        if (std::abs(p) <= eps)
            break;
        _Tp y = (W.at(l, 0) - W.at(k, 0)) * (_Tp)0.5;
        _Tp t = std::abs(y) + hypot(p, y);
        _Tp s = hypot(p, t);
        _Tp c = t / s;
        s = p / s; t = (p / t) * p;
        if (y < 0)
            s = -s, t = -t;
        A.at(k, l) = (_Tp)0;

        W.at(k, 0) -= t;
        W.at(l, 0) += t;

        _Tp a0, b0;

#undef rotate
#define rotate(v0, v1) a0 = v0, b0 = v1, v0 = a0*c - b0*s, v1 = a0*s + b0*c

        // rotate rows and columns k and l
        for (i = 0; i < k; i++)
            rotate(A.at(i, k), A.at(i, l));
        for (i = k + 1; i < l; i++)
            rotate(A.at(k, i), A.at(i, l));
        for (i = l + 1; i < _Rows; i++)
            rotate(A.at(k, i), A.at(l, i));

        // rotate eigenvectors
        if (V)
            for (i = 0; i < _Rows; i++)
                rotate(V->at(k, i), V->at(l, i));

#undef rotate


        Jacobi_help(A, k, indR, indC);
        Jacobi_help(A, l, indR, indC);
    }

    // sort eigenvalues & eigenvectors
    /*for (k = 0; k < n - 1; k++)
    {
        m = k;
        for (i = k + 1; i < n; i++)
        {
            if (W[m] < W[i])
                m = i;
        }
        if (k != m)
        {
            std::swap(W[m], W[k]);
            if (V)
                for (i = 0; i < n; i++)
                    std::swap(V[vstep * m + i], V[vstep * k + i]);
        }
    }*/

    return true;
}

template<typename _Ty, size_t _Rows> 
void power_iteration(const clmagic::square_matrix<_Ty, _Rows> _Matrix, size_t _Count, _Ty& _Eval, clmagic::matrix<_Ty, _Rows, 1>& _Ev) {
    clmagic::matrix<_Ty, _Rows, 1> _X;
    for (auto _First = _X.begin(); _First != _X.end(); ++_First) {
        *_First = clmagic::randf(1.0, 2.0);
    }

    while (_Count--) {
        auto _Y = _Matrix * _X;
        //_X.col(0) = clmagic::normalize(_Y.col(0));
    }
    
    const auto t0 = _X.transpose_copy() * _Matrix * _X;
    _Ev = _X;
    _Eval = t0.at(0, 0);
}



void test() {
	using namespace::clmagic;

}

void test_vectorany() {// vector_any<_Ty, _Block> is News, don't used it
    using namespace::clmagic;
	std::cout << "test vector_any<>" << std::endl;

    vector_any<float, __m256> _Vec;
    for (float f = 2.f; f <= 100.f; ++f) {
        _Vec.push_back(f);
    }
    std::cout << _Vec << std::endl;
    std::cout << "block-capacity: " << _Vec.capacity() << std::endl;
    std::cout << "scalar-capacity: " << _Vec.capacity() * block_traits<__m128>::size() << std::endl;

	while (_Vec.size() > 5) {
		_Vec.pop_back();
	}
	std::cout << _Vec << std::endl;
	std::cout << "block-capacity: " << _Vec.capacity() << std::endl;
	std::cout << "scalar-capacity: " << _Vec.capacity() * block_traits<__m128>::size() << std::endl;

	_Vec.clear();
	std::cout << _Vec << std::endl;
	std::cout << "block-capacity: " << _Vec.capacity() << std::endl;
	std::cout << "scalar-capacity: " << _Vec.capacity() * block_traits<__m128>::size() << std::endl;
}


int main() {
	std::vector<clmagic::vector4<float, __m128>> _Data(10000002);
	for (size_t i = 0; i != _Data.size(); ++i) {
		_Data[i] = { clmagic::randf(), clmagic::randf(), clmagic::randf() };
	}

	using std::chrono::microseconds;
	auto _Clock_0 = std::chrono::steady_clock::now();
	std::cout << clmagic::parallel_accumulate<6>(_Data.begin(), _Data.end(), clmagic::vector4<float, __m128>(0.f)) << std::endl;
	auto _Clock_0_end = std::chrono::steady_clock::now();

	auto _Clock_1 = std::chrono::steady_clock::now();
	std::cout << std::accumulate(_Data.begin(), _Data.end(), clmagic::vector4<float, __m128>(0.f)) << std::endl;
	auto _Clock_1_end = std::chrono::steady_clock::now();


	std::cout << std::chrono::duration_cast<microseconds>(_Clock_0_end - _Clock_0).count() << std::endl;
	std::cout << std::chrono::duration_cast<microseconds>(_Clock_1_end - _Clock_1).count() << std::endl;

	std::cin.get();
    srand(static_cast<uint32_t>(time(0)));
	//diagonal_matrix_operation();
	task_1();
	task_2();
	task_3();
	task_4();
	task_5();
	task_6();
	//test();
	test_vectorany();

	return (0);
}