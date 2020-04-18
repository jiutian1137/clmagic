#include "../src/clmagic/math/general/rational.h"
#include <iostream>
#include <iomanip>
#include <vector>

template<typename _Tf>
std::ostream& print_infolog(std::ostream& _Ostr, _Tf _Val) {
	std::cout << std::setw(30) << clmagic::cvtfloating_rational<intmax_t>(_Val);
	std::cout << std::setw(20) << clmagic::cvtfloating_rational<intmax_t>(_Val).to_floating();
	std::cout << std::setw(20) << static_cast<_Tf>(_Val);
	std::cout << std::endl;
	return _Ostr;
}

template<typename _Tf>
struct floating_set : public std::vector<uint32_t> {
	using _Mybase = std::vector<uint32_t>;

	operator _Tf() const {
		/*
		  dot
		   |
		4442222222
		4442.222222
		*/

		_Tf _Floating = static_cast<_Tf>(0);
		size_t _Where = 0;

		for (; _Where < _Mydot; ++_Where) {
			assert(_Mydot >= 1);// => assert(true)
			_Tf _Rn = static_cast<_Tf>(_Mybase::at(_Where));
			for (size_t j = _Mydot; j != _Where; --j) {
				_Rn *= static_cast<_Tf>(10);
			}
			_Floating += _Rn;
		}

		if (_Where == _Mydot) {
			_Floating += static_cast<_Tf>(_Mybase::at(_Where));
			++_Where;
		}

		for (;  _Where < _Mybase::size(); ++_Where) {
			_Tf _Rn = static_cast<_Tf>(_Mybase::at(_Where));
			for (size_t j = 0; j != _Where; ++j) {
				_Rn *= static_cast<_Tf>(0.1);
			}
			_Floating += _Rn;
		}

		return _Floating;
	}

	friend std::ostream& operator<<(std::ostream& _Ostr, const floating_set& _Info) {
		return print_infolog(_Ostr, static_cast<_Tf>(_Info));
	}

	size_t _Mydot = 0;
};

int main(int argc, char** argv) {
	using namespace::clmagic;

	floating_set<float> _Floating;
	_Floating._Mydot = 0;// x.xxxxx
	_Floating.push_back(0);
	_Floating.push_back(9);
	std::cout << _Floating;
	for (size_t i = 0; i != 6; ++i) {
		_Floating.push_back(9);
		std::cout << _Floating;
	}
	std::cout << "------------------------------------------------------------------------------------------" << std::endl;
	print_infolog(std::cout, 0.9f);
	print_infolog(std::cout, 0.99f);
	print_infolog(std::cout, 0.999f);
	print_infolog(std::cout, 0.9999f);
	print_infolog(std::cout, 0.99999f);
	print_infolog(std::cout, 0.999999f);
	print_infolog(std::cout, 0.9999999f);

	std::cout << "------------------------------------------------------------------------------------------" << std::endl;
	floating_set<double> _Double = reinterpret_cast<floating_set<double>&>(_Floating);
	_Double.erase(_Double.begin() + 2, _Double.end());
	std::cout << _Double;
	for (size_t i = 0; i != 13; ++i) {
		_Double.push_back(9);
		std::cout << _Double;
	}
	std::cout << "------------------------------------------------------------------------------------------" << std::endl;
	print_infolog(std::cout, 0.9);// 1
	print_infolog(std::cout, 0.99);// 2
	print_infolog(std::cout, 0.999);// 3
	print_infolog(std::cout, 0.9999);// 4
	print_infolog(std::cout, 0.99999);// 5
	print_infolog(std::cout, 0.999999);// 6
	print_infolog(std::cout, 0.9999999);// 7
	print_infolog(std::cout, 0.99999999);// 8
	print_infolog(std::cout, 0.999999999);// 9
	print_infolog(std::cout, 0.9999999999);// 10
	print_infolog(std::cout, 0.99999999999);// 11
	print_infolog(std::cout, 0.999999999999);// 12
	print_infolog(std::cout, 0.9999999999999);// 13
	print_infolog(std::cout, 0.99999999999999);// 14

	std::cout << '\n';
	std::cout << "------------------------------------------------------------------------------------------" << std::endl;
	print_infolog(std::cout, 0.9);// 1
	print_infolog(std::cout, 0.99);// 2
	print_infolog(std::cout, 0.990);// 3
	print_infolog(std::cout, 0.9990);// 4
	print_infolog(std::cout, 0.99990);// 5
	print_infolog(std::cout, 0.999990);// 6
	print_infolog(std::cout, 0.9999990);// 7
	print_infolog(std::cout, 0.99999990);// 8
	print_infolog(std::cout, 0.999999990);// 9
	print_infolog(std::cout, 0.9999999990);// 10
	print_infolog(std::cout, 0.99999999990);// 11
	print_infolog(std::cout, 0.999999999990);// 12
	print_infolog(std::cout, 0.9999999999990);// 13
	print_infolog(std::cout, 0.99999999999990);// 14

	std::cout << '\n';
	std::cout << "------------------------------------------------------------------------------------------" << std::endl;
	print_infolog(std::cout, 0.3);// 1
	print_infolog(std::cout, 0.33);// 2
	print_infolog(std::cout, 0.333);// 3
	print_infolog(std::cout, 0.3333);// 4
	print_infolog(std::cout, 0.33333);// 5
	print_infolog(std::cout, 0.333333);// 6
	print_infolog(std::cout, 0.3333333);// 7
	print_infolog(std::cout, 0.33333333);// 8
	print_infolog(std::cout, 0.333333333);// 9
	print_infolog(std::cout, 0.3333333333);// 10
	print_infolog(std::cout, 0.33333333333);// 11
	print_infolog(std::cout, 0.333333333333);// 12
	print_infolog(std::cout, 0.3333333333333);// 13
	print_infolog(std::cout, 0.33333333333333);// 14

	return 0;
}