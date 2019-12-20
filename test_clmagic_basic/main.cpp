#include "../src/clmagic/basic.h"
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

int main(int argc, char** argv) {
	
	tset_fopen();

	std::cin.get();
	return (0);
}