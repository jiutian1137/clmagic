#pragma once
/*<html><input value='yangxia'/>*/
#ifndef clmagic_math_MSTRING_h_
#define clmagic_math_MSTRING_h_
#include <string>

namespace clmagic {

	template<typename _Ty, typename _Elem>
	class basic_math_string {
		using _String_type = std::basic_string<_Elem>;

	public:
		using traits_type     = typename _String_type::traits_type;
		using allocator_type  = typename _String_type::allocator_type;

		using value_type      = _Elem;
		using size_type       = typename _String_type::size_type;
		using difference_type = typename _String_type::difference_type;
		using pointer         = typename _String_type::pointer;
		using const_pointer   = typename _String_type::const_pointer;
		using reference       = value_type&;
		using const_reference = const value_type&;

		using iterator        = typename _String_type::iterator;
		using const_iterator  = typename _String_type::const_iterator;

		using reverse_iterator = typename _String_type::reverse_iterator;
		using const_reverse_iterator = typename _String_type::const_reverse_iterator;

		using scalar_type     = _Ty;

		basic_math_string() = default;

		basic_math_string(int _Number) {
			_Mystr.assign(std::to_string(_Number));
		}

		basic_math_string(const _Ty& _Number) {
			using std::to_string;
			_Mystr.assign(to_string(_Number));
		}

		basic_math_string(const _Elem* const _Ptr)
			: _Mystr(_Ptr) {
			//
		}

		template <class _Iter, class = std::enable_if_t<std::_Is_iterator_v<_Iter>>>
		basic_math_string(_Iter _First, _Iter _Last)
			: _Mystr(_First, _Last){
			//
		}
		//basic_math_string(_Elem* const _First, _Elem* const _Last)
		//	: _Mystr(_First, _Last) {
		//	// 
		//}

		basic_math_string(const std::string& _Str)
			: _Mystr(_Str) {
			//
		}

		bool is_nan() const {
			return (this->_Mystr == "nan");
		}

		bool is_inf() const {
			return (this->_Mystr == "inf" || this->_Mystr == "-inf");
		}

		bool is_zero() const {
			return (this->_Mystr == "0" || this->_Mystr == "0.0");
		}

		bool is_one() const {
			return (this->_Mystr == "1" || this->_Mystr == "1.0");
		}

		bool has_sign() const {
			return (this->_Mystr[0] == '-' || this->_Mystr[1] == '+'); 
		}

		bool is_neg() const {
			return (this->_Mystr[0] == '-');
		}

		bool is_covered() const {
			auto _Front = _Mystr[this->has_sign() ? 1 : 0];
			auto _Back  = _Mystr.back();
			return ((_Front == '(' && _Back == ')') 
				 || (_Front == '[' && _Back == ']')
				 || (_Front == '{' && _Back == '}'));
		}

		static basic_math_string neg(const basic_math_string& _Left) {
			if (_Left.is_neg()) { // -a or -(a+b)
				if (_Left.is_covered()) { 
					return basic_math_string(&_Left._Mystr[2], &_Left._Mystr[_Left._Mystr.size() - 1]); 
				} else {
					return basic_math_string(_Left._Mystr.begin() + 1, _Left._Mystr.end());
				}
			} else {// a or a+b
				return ("-(" + _Left._Mystr + ")");
			}
		}
		
		static basic_math_string abs(const basic_math_string& _Left) {
			return (_Left.is_neg() ? neg(_Left) : _Left);
		}

		static basic_math_string add(const basic_math_string& _Left, const basic_math_string& _Right) {
			if (_Right.is_zero()) {
				return (_Left);
			}else if (_Left.is_zero()) {
				return (_Right);
			}else if (_Right.is_zero() && _Left.is_zero()) {
				return ("0");
			} else if (_Right.is_neg()) {
				return basic_math_string(_Left._Mystr + "-" + neg(_Right)._Mystr);
			} else {
				return basic_math_string(_Left._Mystr + "+" + _Right._Mystr);
			}
		}

		static basic_math_string sub(const basic_math_string& _Left, const basic_math_string& _Right) {
			if (_Right.is_zero()) {
				return (_Left);
			} else if (_Left.is_zero()) {
				return ("-(" + _Right._Mystr + ")");
			} else if (_Right.is_neg()) {
				return basic_math_string(_Left._Mystr + "+" + neg(_Right)._Mystr);
			} else {
				return basic_math_string(_Left._Mystr + "-" + _Right._Mystr);
			}
		}

		static basic_math_string mul(const basic_math_string& _Left, const basic_math_string& _Right) {
			if (_Left.is_zero() || _Right.is_zero()) {// 0 * R or L * 0
				return ("0");
			} else if (_Left.is_one()) {// 1 * R
				return (_Right);
			} else if (_Right.is_one()) {// L * 1
				return (_Left);
			} else if (_Left.is_neg() && _Right.is_neg()) {
				return basic_math_string(abs(_Left)._Mystr + "*" + abs(_Right)._Mystr);
			} else if (_Left.is_neg() || _Right.is_neg()) {
				return basic_math_string("-" + abs(_Left)._Mystr + "*" + abs(_Right)._Mystr);
			} else {
				return basic_math_string(_Left._Mystr + "*" + _Right._Mystr);
			}
		}

		static basic_math_string div(const basic_math_string& _Left, const basic_math_string& _Right) {
			if (_Right.is_zero()) {
				return ("nan");
			}
			else if (_Right.is_one()) {
				return (_Left);
			}
			else if (_Left.is_zero()) {
				return ("0");
			}
			else {
				return basic_math_string(_Left._Mystr + "/" + _Right._Mystr);
			}
		}

		basic_math_string& operator+=(const basic_math_string& _Right) {
			(*this) = add(*this, _Right);
			return (*this);
		}
		basic_math_string& operator-=(const basic_math_string& _Right) {
			(*this) = sub(*this, _Right);
			return (*this);
		}
		basic_math_string& operator*=(const basic_math_string& _Right) {
			(*this) = mul(*this, _Right);
			return (*this);
		}
		basic_math_string& operator/=(const basic_math_string& _Right) {
			(*this) = div(*this, _Right);
			return (*this);
		}

		basic_math_string operator-() const {
			return neg(*this);
		}
		basic_math_string operator+(const basic_math_string& _Right) const {
			return add(*this, _Right);
		}
		basic_math_string operator-(const basic_math_string& _Right) const {
			return sub(*this, _Right);
		}
		basic_math_string operator*(const basic_math_string& _Right) const {
			return mul(*this, _Right);
		}
		basic_math_string operator/(const basic_math_string& _Right) const {
			return div(*this, _Right);
		}

		friend std::string to_string(const basic_math_string& _Str) {
			return (_Str._Mystr);
		}

		friend std::ostream& operator<<(std::ostream& _Ostr, const basic_math_string& _Str) {
			return (_Ostr << to_string(_Str));
		}

	private:
		_String_type _Mystr;
	};

	template<typename _Ty>
	using  math_string = basic_math_string<_Ty, char>;

}// namespace clmagic

#endif
/*</html>*/