#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH_LAPACK___VECTOR___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH_LAPACK___VECTOR___H__
#include "../lapack.h"

namespace clmagic 
{


	template<typename _Vec> inline
		auto distance(_in(_Vec) _A, _in(_Vec) _B) {
			return ((_B - _A).length());
		}

	template<size_t N, typename T> inline
		T radians(_in(Vector_<N, T>) _A, _in(Vector_<N, T>) _B) 
		{	// return dot(A,B)÷|A|÷|B|
		return (dot(_A, _B) / _A.length() / _B.length());
		}

	template<typename _Nom, typename _Vec> inline
		_Vec faceforward(_in(_Nom) _Normal, _in(_Vec) _I, _in(_Vec) _Nref)
		{	// dot(_I,_Nref) < 0 ? N : -N
		return (dot(_I, _Nref) < 0 ? _Normal : -_Normal);
		}

	template<typename _Vec> inline
		_Vec proj(_in(_Vec) _Vector, _in(_Vec) _Proj) 
		{	// return dot(V,||_Proj||) * |Proj| 
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

	template<typename T>
		Vector3_<T> proj(_in(Vector3_<T>) _Vector, _in(Normal_<T>) _Proj) 
		{	// return dot(V,N)*N
		return (dot3(_Vector, _Proj) * _Proj);
		}

	template<typename T> inline
		Vector3_<T> proj(_in(Vector3_<T>) _Vector, _in(Normal_<T>) _Rt, _in(Normal_<T>) _Up, _in(Normal_<T>) _Fwd) 
		{	// return [dot3(V,r), dot(V,u), dot(V,f)]
		return ( Vector3_<T>(dot3(_Vector, _Rt), dot3(_Vector, _Up), dot3(_Vector, _Fwd)) );
		}

	template<typename _Vec> inline
		_Vec reflect(_in(_Vec) _Direction, _in(_Vec) _Normal) {
			return (Real(2) * proj(_Direction, _Normal) - _Direction);
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

	template<typename T> inline
		Vector3_<T> reflect(_in(Vector3_<T>) _Direction, _in(Normal_<T>) _Normal) {
			return ( Real(2) * proj(_Direction, _Normal) - _Direction );
	}

	template<typename T>
		Vector2_<T> orthogonal(_in(Vector2_<T>) _Lhs) {
			Vector2_<T>(-_Lhs.y, _Lhs.x);
		}

	template<typename T>
		void orthogonal(_inout(std::vector<Vector3_<T>>) _Vectors) {
			for (size_t i = 1; i != _Vectors.size(); ++i) {
				for (size_t j = 0; j != i; ++j) {
					_Vectors[i] -= proj(_Vectors[i], _Vectors[j]);
				}
			}
		}

}// namespace clmagic

#endif