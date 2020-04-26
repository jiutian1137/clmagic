#pragma once
#ifndef clmagic_math_FRACTION_h_
#define clmagic_math_FRACTION_h_
#include "lapack.h"
#include <fstream>

namespace clmagic 
{
	// @_Ty: this _Ty is set precision
	template<typename _Ty>
	struct NoiseGenerator {
		NoiseGenerator(_Ty _Frequency = _Ty(2.76434f), _Ty _Ampt = _Ty(0.51749673f))
			: _Myfreq(_Frequency), _Myampt(_Ampt)
			{
			}

		template<typename _Fnfbm, typename _Fnnoise, typename _Typos, typename ..._Tys>
		inline _Ty operator() (size_t _N, _Fnfbm _Funcfbm, _Fnnoise _Funcnoise, _Typos _Pos, _Tys... _Args) {
			if (_N != _Myoctave.size()) {
				_Myoctave.resize(_N);
			}

			_Ty _Ampl = _Myampt;
			for (size_t i = 0; i != _N; ++i) {
				_Myoctave[i] = _Funcnoise(_Pos, _Args...);
				_Myoctave[i] = _Myoctave[i] * _Ampl;
				_Pos *= _Myfreq;
				_Ampl *= _Myampt;
			}

			return ( _Funcfbm(_Myoctave) );
		}

		_Ty _Myfreq;
		_Ty _Myampt;
		std::vector<_Ty> _Myoctave;
	};


	class Noise {
	public:
		Noise() : _Mysize(), _Mysizeone(0), _Mydata(nullptr) {
			// empty noise
		}

		~Noise() {
			this->destroy();
		}

		void destroy() {
			if (_Mydata != nullptr) {
				delete[] _Mydata;
			}

			_Mysize.clear();
			_Mysizeone = 0;
		}

		size_t dims() const {
			return (_Mysize.size());
		}

		size_t size() const {
			size_t _Cbsize = 1;
			for (size_t i = 0; i != this->dims(); ++i) {
				_Cbsize *= _Mysize[i];
			}
			return (_Cbsize);
		}

		const size_t& size_one() const {
			return (_Mysizeone);
		}

		const size_t& step(size_t i) const {
			return (_Mysize[i]);
		}

		const std::vector<size_t>& step() const {
			return (_Mysize);
		}

		template<typename _Ty>
		_Ty* ptr() {
			return (reinterpret_cast<_Ty*>(_Mydata));
		}

		/*
		[Noise file format]:
		demension \n
		step(0)\n
		step(1)\n
		step(2)\n
		step(n)\n
		size_one \n
		_Mydata
		*/
		void to_file(std::fstream& _Fout) {
			_Fout << this->dims() << '\n';
			for (size_t i = 0; i != _Mysize.size(); ++i) {
				_Fout << this->step(i) << '\n';
			}

			_Fout << this->size_one() << '\n';

			_Fout.write(_Mydata, this->size() * this->size_one());
		}

		void fread(std::fstream& _Fin) {
			if (_Mydata != nullptr) {
				this->destroy();
			}

			std::string _Tmp;
			std::getline(_Fin, _Tmp);

			_Mysize.resize(std::stoi(_Tmp));
			for (size_t i = 0; i != _Mysize.size(); ++i) {
				std::getline(_Fin, _Tmp);
				_Mysize[i] = std::stoi(_Tmp);
			}

			std::getline(_Fin, _Tmp);
			_Mysizeone = std::stoi(_Tmp);

			auto _Totalsize = this->size() * this->size_one();
			_Mydata = new char[_Totalsize];
			_Fin.read(_Mydata, std::streamsize(_Totalsize));
		}

		std::vector<size_t> _Mysize;
		size_t			    _Mysizeone;
		char* _Mydata;
	};



	template<typename _Ty> inline
	_Ty fbm_1(const std::vector<_Ty>& _Octave) 
		{	// ∑Oct
		_Ty _Result = _Octave[0];
		std::for_each(_Octave.begin() + 1, _Octave.end(),
			[&_Result](_Ty oct) { _Result += oct; }
			);
		return ( _Result );
		}

	template<typename _Ty> inline
	_Ty fbm_2(const std::vector<_Ty>& _Octave)
		{	// ∑abs(Oct)
		auto _Result = abs(_Octave[0]);
		std::for_each(_Octave.begin() + 1, _Octave.end(), 
			[&_Result](_Ty oct) { _Result += abs(oct); }
			);
		return (_Result);
		}

	template<typename _Ty> inline
	_Ty fbm_3(const std::vector<_Ty>& _Octave)
		{	// sin( ∑abs(Oct) )
		return ( sin(fbm_2(_Octave)) );
		}

	template<typename _Ty> inline
	_Ty permute(_in(_Ty) x) 
		{// @_Equaltion: (34*x^2 + x) % 289
		using _Sty = scalar_type<_Ty>;
		return mod(static_cast<_Sty>(34) * x * x + x, static_cast<_Sty>(289));
		}

	template<typename _Ty, typename _Fn> vector2<_Ty> cellular(vector2<_Ty> P, _Fn _Hfunc);

	template<typename _Ty, typename _Fn> _Ty gradient2(vector2<_Ty> _Pos, _Fn _Hfunc);
	template<typename _Ty, typename _Fn> _Ty gradient3(vector3<_Ty> _Pos, _Fn _Hfunc);

	// perfermance well
	template<typename _Ty, typename _Fn> _Ty value2(vector2<_Ty> p, _Fn _Hash22);
	template<typename _Ty, typename _Fn> _Ty value3(vector3<_Ty> x, _Fn _Hash33);

	template<typename _Ty, typename _Fn> _Ty worley2(vector2<_Ty> _Pos, _Fn _Hash22);
	template<typename _Ty, typename _Fn> _Ty worley3(vector3<_Ty> _Pos, _Fn _Hash33);

	// Classic Perlin noise
	template<typename _Ty> _Ty cnoise2(vector2<_Ty> _Pos);
	template<typename _Ty> _Ty cnoise3(vector3<_Ty> _Pos);

	// Classic Perlin noise, periodic variant
	template<typename _Ty> _Ty pnoise2(vector2<_Ty> _Pos, vector2<_Ty> _Rep);
	template<typename _Ty> _Ty pnoise3(vector3<_Ty> _Pos, vector3<_Ty> _Rep);


	template<typename _Ty, typename _Fn>
		vector3<_Ty> gradient2d(vector2<_Ty> _Pos, _Fn _Hash22);
	template<typename _Ty, typename _Fn>
		vector4<_Ty> gradient3d(vector3<_Ty> _Pos, _Fn _Hash33);
	template<typename _Ty, typename _Fn> 
		vector4<_Ty> value3d(vector3<_Ty> _Pos, _Fn _Hash33);
}// namespace clmagic

#endif