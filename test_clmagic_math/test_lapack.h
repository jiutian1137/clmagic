#pragma once
#include <chrono>
#include <random>
#include <iostream>
#include "../src/clmagic/math.h"

enum class eTestLapack {
	General,
	Matrix,
	DiagMatrix,
	OrthoMatrix,
	WorldMatrix
};


template<size_t M, size_t N>
void test_diag(const clmagic::fmatrix<M, N>& _Diag) {
	std::cout << _Diag << std::endl;
}

inline void test_lapack(eTestLapack _Enum) {
	using namespace::clmagic;

	if (_Enum == eTestLapack::General) {
		float pA[] = {
			 1.f, 0.f, 20.f,  5.f,
			 0.f, 1.f, 0.f, 12.f,
			 10.f, 50.f, 1.f,  3.f,
		};

		float pAinv[] = {
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f
		};

		auto A = matrix_slice<float>(pA, 3, 4);
		auto Ainv = matrix_slice<float>(pAinv, 3, 4);
		Lu(A, Ainv);
		for (size_t i = 0; i != A.rows(); ++i) {
			for (size_t j = 0; j != A.cols(); ++j) {
				std::cout << A.at(i, j) << " ";
			}
			std::cout << std::endl;
		}

		for (size_t i = 0; i != Ainv.rows(); ++i) {
			for (size_t j = 0; j != Ainv.cols(); ++j) {
				std::cout << Ainv.at(i, j) << " ";
			}
			std::cout << std::endl;
		}
	} else if (_Enum == eTestLapack::Matrix) {
	
	} else if (_Enum == eTestLapack::DiagMatrix) {
		srand(unsigned int(time(nullptr)));

		fmatrix<3, 4> A;
		for (size_t i = 0; i != A.size(); ++i) {
			*A.ptr(i) = randf(-10.f, 99.f);
		}

		std::cout << "A: " << A << std::endl;
		std::cout << "A solve: " << A.solve() << std::endl;
		std::cout << "A solve: " << A << std::endl;

		diag_matrix<4, 4> B = { 2.f, 3.f, 5.f, 10.f };
		std::cout << "B: " << B << std::endl;
		auto C = A * B;
		std::cout << "A * B = " << C << std::endl;

		fmatrix<4, 4> Bt = {
			2.f, 0.f, 0.f, 0.f,
			0.f, 3.f, 0.f, 0.f,
			0.f, 0.f, 5.f, 0.f,
			0.f, 0.f, 0.f, 10.f };
		std::cout << "Bt: " << Bt << std::endl;
		auto Ct = A * Bt;
		std::cout << "A * Bt = " << Ct << std::endl;

		std::cout << "template function argument --------------" << std::endl;
		test_diag(B);
		test_diag(Bt);

		std::cout << "diagonal matrix operation -----------------" << std::endl;
		diag_matrix<3, 4> Ad = { 1.f, 2.f, 3.f };
		diag_matrix<3, 4> Bd = { 3.f, 2.f, 1.f };
		diag_matrix<4, 6> Cd = { 5.f, 10.f, 2.f, 8.f };
		std::cout << "Ad: " << Ad << std::endl;
		std::cout << "Bd: " << Bd << std::endl;
		std::cout << "Cd: " << Cd << std::endl;
		std::cout << "Ad + Bd = " << Ad + Bd << std::endl;
		std::cout << "Ad - Bd = " << Ad - Bd << std::endl;
		std::cout << "Ad + 1.0 = " << Ad + 1.f << std::endl;
		std::cout << "Ad - 1.0 = " << Ad - 1.f << std::endl;
		std::cout << "Ad * 2.0 = " << Ad * 2.f << std::endl;
		std::cout << "Ad / 2.0 = " << Ad / 2.f << std::endl;
		std::cout << "Ad * Cd = " << Ad * Cd << std::endl;
		fmatrix<3, 4> Adt = Ad;
		fmatrix<4, 6> Cdt = Cd;
		std::cout << "Adt * Cdt = " << Adt * Cdt << std::endl;
		std::cout << "inv(Ad) = " << Ad.inv() << std::endl;
		std::cout << "inv(Ad) = " << Ad << std::endl;
		std::cout << "inv(Adt) = " << Adt.inv() << std::endl;
		std::cout << "inv(Adt) = " << Adt << std::endl;
		std::cout << "inv(inv(Adt)) = " << inverse(Adt) << std::endl;


		for (size_t i = 0; i != Ad.diags(); ++i) {
			std::cout << Ad[i] << " ";
		}
		std::cout << std::endl;

		diag_matrix<4, 4, float, false> _World = { 2.f, 2.f, 2.f, 1.f };
		vector4<float> _Position = { 10.f, 1.8f, 10.f, 1.f };
		std::cout << _Position * _World << std::endl;
	}
}