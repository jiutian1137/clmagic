#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH___FRACTION___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH___FRACTION___H__
#include "lapack.h"
#include <fstream>

namespace clmagic 
{
#ifndef _CLMAGIC_NOISE_HASH_FUNC_POINTER
	/*
	@_hash_function_pointer:
		name rule: _Hash[return component][argument component]_func_pointer
	*/
	template<typename _Ty>
	using _Hash11_func_pointer = _Ty(*)(_Ty);

	template<typename _Ty>
	using _Hash21_func_pointer = Vec2_<_Ty>(*)(_Ty);

	template<typename _Ty>
	using _Hash12_func_pointer = _Ty(*)(Vec2_<_Ty>);

	template<typename _Ty>
	using _Hash22_func_pointer = Vec2_<_Ty>(*)(Vec2_<_Ty>);

	template<typename _Ty>
	using _Hash32_func_pointer = Vec3_<_Ty>(*)(Vec2_<_Ty>);

	template<typename _Ty>
	using _Hash33_func_pointer = Vec3_<_Ty>(*)(Vec3_<_Ty>);

	template<typename _Ty>
	using _Hash43_func_pointer = Vec4_<_Ty>(*)(Vec3_<_Ty>);

	template<typename _Ty>
	using _Hash44_func_pointer = Vec4_<_Ty>(*)(Vec4_<_Ty>);

	template<typename _Ty>
	using _Hash34_func_pointer = Vec3_<_Ty>(*)(Vec4_<_Ty>);
#endif // !_CLMAGIC_NOISE_HASH_FUNC_POINTER


	


	// @_Ty: this _Ty is set precision
	template<typename _Ty>
	class NoiseGenerator
	{
	public:
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
		{
		_Ty _Result = _Ty(0);
		for (size_t i = 0; i != _Octave.size(); ++i) _Result += _Octave[i];
		return ( _Result );
		}

	template<typename _Ty> inline
		_Ty fbm_2(const std::vector<_Ty>& _Octave)
		{
		_Ty _Result = _Ty(0);
		for (size_t i = 0; i != _Octave.size(); ++i) 
			{
			_Result += std::abs(_Octave[i]);
			}
		return (_Result);
		}

	template<typename _Ty> inline
		_Ty fbm_3(const std::vector<_Ty>& _Octave)
		{
		_Ty _Result = _Ty(0);
		for (size_t i = 0; i != _Octave.size(); ++i)
			{
			_Result += std::abs(_Octave[i]);
			}
		return ( std::sin(_Result) );
		}

	template<typename _Ty> inline
	_Ty permute(_in(_Ty) x) {// @_Equaltion: (34*x^2 + x) % 289
		return ((Real(34) * x * x + x) % Real(289));
	}

	template<typename _Ty> Vec2_<_Ty> cellular(Vec2_<_Ty> P, _Hash22_func_pointer<_Ty>);

	template<typename _Ty> _Ty gradient2(Vec2_<_Ty> _Pos, _Hash22_func_pointer<_Ty> _Hash);
	template<typename _Ty> _Ty gradient3(Vec3_<_Ty> _Pos, _Hash33_func_pointer<_Ty> _Hash);

	template<typename _Ty> Vec3_<_Ty> gradientd2(Vec2_<_Ty> _Pos, _Hash22_func_pointer<_Ty> _Hash);
	template<typename _Ty> Vec4_<_Ty> gradientd3(Vec3_<_Ty> _Pos, _Hash33_func_pointer<_Ty> _Hash);

	// Classic Perlin noise
	template<typename _Ty> _Ty cnoise2(Vec2_<_Ty> _Pos);
	template<typename _Ty> _Ty cnoise3(Vec3_<_Ty> _Pos);

	// Classic Perlin noise, periodic variant
	template<typename _Ty> _Ty pnoise2(Vec2_<_Ty> _Pos, Vec2_<_Ty> _Rep);
	template<typename _Ty> _Ty pnoise3(Vec3_<_Ty> _Pos, Vec3_<_Ty> _Rep);

	// perfermance well
	template<typename _Ty> _Ty value2(Vec2_<_Ty> p, _Hash22_func_pointer<_Ty> _Hash);
	template<typename _Ty> _Ty value3(Vec3_<_Ty> x, _Hash33_func_pointer<_Ty> _Hash);

	template<typename _Ty> Vec4_<_Ty> valued3(Vec3_<_Ty> x, _Hash33_func_pointer<_Ty>);

	template<typename _Ty> _Ty worley2(Vec2_<_Ty> _Pos, _Hash22_func_pointer<_Ty> _Hash);
	template<typename _Ty> _Ty worley3(Vec3_<_Ty> _Pos, _Hash33_func_pointer<_Ty> _Hash);

}// namespace clmagic

#endif