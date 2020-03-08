#include "../src/clmagic/basic.h"
#include <array>
#include <string>
#include <iomanip>
#include <iostream>


template<typename _Iter>
inline void print(_Iter _First, _Iter _Last) {
	while (_First != _Last) {
		std::cout << (*_First++) << ", ";
	}
	std::cout << std::endl;
}

void test_fileproxy(){
	std::string _Str = "   sadasds   ";
	auto _Result = clmagic::strtrim(_Str);

	clmagic::fileproxy _Fileproxy = clmagic::fileproxy("aaaa.txt");
	std::cout << _Fileproxy.source() << std::endl;
}

void tset_fopen() {
	auto _Fout = clmagic::fopen("aaaa.txt", std::ios::out);
	(*_Fout) << "LongjiangNan";
	_Fout->close();
}

void Rabin_Karp(const std::string& _Source, const std::string& _Where, size_t _Digit, size_t _Prime) {
	size_t _N = _Source.size();
	size_t _M = _Where.size();
	size_t _H = static_cast<size_t>(std::pow(_Digit, _M - 1)) % _Prime;

	size_t _W = 0;
	size_t _S = 0;
	for (size_t i = 0; i != _M; ++i) {
		_W = (_Digit * _W + _Where[i]) % _Prime;
		_S = (_Digit * _S + _Source[i]) % _Prime;
	}

	for (size_t i = 0; i != _N - _M; ++i) {
		if (_S == _W) {
			if (std::equal(_Where.begin(), _Where.end(), std::next(_Source.begin(), i))) {
				std::cout << std::setw(i) << " " << i << std::endl;
			}
		}

		if (i < _N - _M) {
			_S = (_Digit * (_S - _Source[i] * _H) + _Source[i + _M]) % _Prime;
		}
	}
}

std::vector<size_t> _KMP_prefix(const std::string& _Where) {
	std::vector<size_t> _Prefix;
	_Prefix.resize(_Where.size());

	_Prefix[0] = -1;
	for (size_t i = 1; i != _Where.size(); ++i) {
		size_t _K = _Prefix[i - 1];
		while (_K != -1 && _Where[i - 1] != _Where[_K]) {
			_K = _Prefix[_K];
		}

		if (_K != -1 && _Where[i - 1] == _Where[_K]) {
			_K = _K + 1;
		}
		else {
			_Prefix[i] = 0;
		}
	}

	return (_Prefix);
}

void KMP(const std::string& _Str, const std::string& _Where) {
	auto _N = _Str.size();
	auto _M = _Where.size();
	auto _Prefix = _KMP_prefix(_Where);
	for (size_t i = 0; i != _N; ++i) {
		size_t _Q = 0;
		/*while (_Q > 0 && _Where[_Q + 1] != _Str[i]) {
			_Q = _Prefix[_Q];
		}
		if (_Where[_Q + 1] == _Str[i]) {
			_Q = _Q + 1;
		}*/
		size_t _R = i;
		while (_Q < _M && _R < _Str.size()) {
			if (_Where[_Q] == _Str[_R]) {
				++_Q;
				++_R;
			}
			else {
				if (_Q == 0) ++_R;
				else _Q = _Prefix[_Q - 1] + 1;
			}
		}

		if (_Q == _M && (_R - i) == _M) {
			std::cout << std::setw(i) << "" << i << std::endl;
			//_Q = _Prefix[_Q];
		}
	}
}

struct Num {

	Num operator-() const {
		return Num{ -this->a };
	}

	int a = 100;
};

template<typename _Ty, typename _Fn>
auto vvvv(const _Ty& _Obj, _Fn _Func) {
	return _Func(_Obj);
}

int main(int argc, char** argv) {
	Num _SSS;
	auto _SSS2 = vvvv(_SSS, _SSS.operator-());


	//tset_fopen();

	/*std::string _Test_PK = "2321123211222112312";
	std::cout << _Test_PK << std::endl;
	KMP(_Test_PK, "11");*/
	
	std::cin.get();
	return (0);
}