#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH___REAL___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH___REAL___H__
#include "clmagic.h"
#include <cmath>
#include <iostream>

namespace clmagic 
{
#if defined(__CLMAGIC_REAL_AS_FLOAT__) || !defined(__CLMAGIC_REAL_AS_DOUBLE__)
	using real_t = float;
	
#elif defined(__CLMAGIC_REAL_AS_DOUBLE__)
	using real_t = double;
#endif
	
	/*
	single component varient
	support any basic type convert
	*/
	struct Real 
	{
		real_t val;

		explicit constexpr Real(char      _Val) : val(static_cast<real_t>(_Val)) {}
		explicit constexpr Real(short     _Val) : val(static_cast<real_t>(_Val)) {}
		explicit constexpr Real(int       _Val) : val(static_cast<real_t>(_Val)) {}
		explicit constexpr Real(long      _Val) : val(static_cast<real_t>(_Val)) {}
		explicit constexpr Real(long long _Val) : val(static_cast<real_t>(_Val)) {}
		explicit constexpr Real(float     _Val) : val(static_cast<real_t>(_Val)) {}
		explicit constexpr Real(double    _Val) : val(static_cast<real_t>(_Val)) {}

		explicit constexpr Real(unsigned char  _Val) : val(static_cast<real_t>(_Val)) {}
		explicit constexpr Real(unsigned short _Val) : val(static_cast<real_t>(_Val)) {}
		explicit constexpr Real(unsigned int   _Val) : val(static_cast<real_t>(_Val)) {}
		explicit constexpr Real(unsigned long  _Val) : val(static_cast<real_t>(_Val)) {}
		explicit constexpr Real(unsigned long long _Val) : val(static_cast<real_t>(_Val)) {}
		
		// < convert >
		explicit constexpr operator char()  const { return ( static_cast<char >(val) ); }
		explicit constexpr operator short() const { return ( static_cast<short>(val) ); }
		explicit constexpr operator int()   const { return ( static_cast< int >(val) ); }
		explicit constexpr operator long()  const { return ( static_cast<long >(val) ); }
		explicit constexpr operator long long() const { return ( static_cast<long long>(val) ); }
		explicit constexpr operator float()  const { return ( static_cast<float >(val) ); }
		explicit constexpr operator double() const { return ( static_cast<double>(val) ); }

		explicit constexpr operator unsigned char()  const { return ( static_cast<char >(val) ); }
		explicit constexpr operator unsigned short() const { return ( static_cast<short>(val) ); }
		explicit constexpr operator unsigned int()   const { return ( static_cast< int >(val) ); }
		explicit constexpr operator unsigned long()  const { return ( static_cast<long >(val) ); }
		explicit constexpr operator unsigned long long() const { return ( static_cast<long long>(val) ); }
		// </ convert >

		constexpr bool operator> (_in(Real) _Rhs) const { return (this->val >  _Rhs.val); }
		constexpr bool operator>=(_in(Real) _Rhs) const { return (this->val >= _Rhs.val); }
		constexpr bool operator< (_in(Real) _Rhs) const { return (this->val <  _Rhs.val); }
		constexpr bool operator<=(_in(Real) _Rhs) const { return (this->val <= _Rhs.val); }
		constexpr bool operator==(_in(Real) _Rhs) const { return (this->val == _Rhs.val); }
		constexpr bool operator!=(_in(Real) _Rhs) const { return (this->val != _Rhs.val); }

		constexpr Real operator-( /* empty */  ) const { return (Real(-val)); }
		constexpr Real operator+(_in(Real) _Rhs) const { return (Real(this->val + _Rhs.val)); }
		constexpr Real operator-(_in(Real) _Rhs) const { return (Real(this->val - _Rhs.val)); }
		constexpr Real operator*(_in(Real) _Rhs) const { return (Real(this->val * _Rhs.val)); }
		constexpr Real operator/(_in(Real) _Rhs) const { return (Real(this->val / _Rhs.val)); }
		constexpr Real operator%(_in(Real) _Rhs) const 
			{
			return ( (*this) - Real( _Rhs.val * std::floor(((*this) / _Rhs).val) ) );
			}
	};

	inline std::istream& operator>>(_inout(std::istream) _Istr, _out(Real) _Real)
		{
		_Istr >> _Real.val;
		return (_Istr);
		}

	inline std::ostream& operator<<(_inout(std::ostream) _Ostr, _in(Real) _Real) 
		{
		_Ostr << _Real.val;
		return (_Ostr);
		}

	
	/*
	these operators require only 1 additional conversion 
	*/
#define __CLMAGIC_BASICTYPE_REAL_OPERATOR__(TYPE) \
	constexpr bool operator> (TYPE _Lhs, _in(Real) _Rhs) { return ( _Lhs >  static_cast<TYPE>(_Rhs) ); } \
	constexpr bool operator>=(TYPE _Lhs, _in(Real) _Rhs) { return ( _Lhs >= static_cast<TYPE>(_Rhs) ); } \
	constexpr bool operator< (TYPE _Lhs, _in(Real) _Rhs) { return ( _Lhs <  static_cast<TYPE>(_Rhs) ); } \
	constexpr bool operator<=(TYPE _Lhs, _in(Real) _Rhs) { return ( _Lhs <= static_cast<TYPE>(_Rhs) ); } \
	constexpr bool operator==(TYPE _Lhs, _in(Real) _Rhs) { return ( _Lhs == static_cast<TYPE>(_Rhs) ); } \
	constexpr bool operator!=(TYPE _Lhs, _in(Real) _Rhs) { return ( _Lhs != static_cast<TYPE>(_Rhs) ); } \
	constexpr TYPE operator+(TYPE _Lhs, _in(Real) _Rhs) { return ( _Lhs + static_cast<TYPE>(_Rhs) ); } \
	constexpr TYPE operator-(TYPE _Lhs, _in(Real) _Rhs) { return ( _Lhs - static_cast<TYPE>(_Rhs) ); } \
	constexpr TYPE operator*(TYPE _Lhs, _in(Real) _Rhs) { return ( _Lhs * static_cast<TYPE>(_Rhs) ); } \
	constexpr TYPE operator/(TYPE _Lhs, _in(Real) _Rhs) { return ( _Lhs / static_cast<TYPE>(_Rhs) ); } \
	constexpr TYPE operator%(TYPE _Lhs, _in(Real) _Rhs) { return ( static_cast<TYPE>( static_cast<Real>(_Lhs) % _Rhs ) ); } \
	constexpr bool operator> (_in(Real) _Lhs, TYPE _Rhs) { return ( _Lhs >  Real(_Rhs) ); } \
	constexpr bool operator>=(_in(Real) _Lhs, TYPE _Rhs) { return ( _Lhs >= Real(_Rhs) ); } \
	constexpr bool operator< (_in(Real) _Lhs, TYPE _Rhs) { return ( _Lhs <  Real(_Rhs) ); } \
	constexpr bool operator<=(_in(Real) _Lhs, TYPE _Rhs) { return ( _Lhs <= Real(_Rhs) ); } \
	constexpr bool operator==(_in(Real) _Lhs, TYPE _Rhs) { return ( _Lhs == Real(_Rhs) ); } \
	constexpr bool operator!=(_in(Real) _Lhs, TYPE _Rhs) { return ( _Lhs != Real(_Rhs) ); } \
	constexpr TYPE operator+(_in(Real) _Lhs, TYPE _Rhs) { return ( static_cast<TYPE>(_Lhs + Real(_Rhs)) ); } \
	constexpr TYPE operator-(_in(Real) _Lhs, TYPE _Rhs) { return ( static_cast<TYPE>(_Lhs - Real(_Rhs)) ); } \
	constexpr TYPE operator*(_in(Real) _Lhs, TYPE _Rhs) { return ( static_cast<TYPE>(_Lhs * Real(_Rhs)) ); } \
	constexpr TYPE operator/(_in(Real) _Lhs, TYPE _Rhs) { return ( static_cast<TYPE>(_Lhs / Real(_Rhs)) ); } \
	constexpr TYPE operator%(_in(Real) _Lhs, TYPE _Rhs) { return ( static_cast<TYPE>(_Lhs % Real(_Rhs)) ); }

	__CLMAGIC_BASICTYPE_REAL_OPERATOR__(char)
	__CLMAGIC_BASICTYPE_REAL_OPERATOR__(short)
	__CLMAGIC_BASICTYPE_REAL_OPERATOR__(int)
	__CLMAGIC_BASICTYPE_REAL_OPERATOR__(long)
	__CLMAGIC_BASICTYPE_REAL_OPERATOR__(long long)
	__CLMAGIC_BASICTYPE_REAL_OPERATOR__(float)
	__CLMAGIC_BASICTYPE_REAL_OPERATOR__(double)
	__CLMAGIC_BASICTYPE_REAL_OPERATOR__(unsigned char)
	__CLMAGIC_BASICTYPE_REAL_OPERATOR__(unsigned short)
	__CLMAGIC_BASICTYPE_REAL_OPERATOR__(unsigned int)
	__CLMAGIC_BASICTYPE_REAL_OPERATOR__(unsigned long)
	__CLMAGIC_BASICTYPE_REAL_OPERATOR__(unsigned long long)

	

	constexpr Real cYotta = Real(1E24);
	constexpr Real cZetta = Real(1E21);
	constexpr Real cExa   = Real(1E18);
	constexpr Real cPeta  = Real(1E15);
	constexpr Real cTera  = Real(1E12);
	constexpr Real cGiga  = Real(1E9);
	constexpr Real cMega  = Real(1E6);
	constexpr Real cKilo  = Real(1E3);
	constexpr Real cHecto = Real(1E2);
	constexpr Real cDeka  = Real(1E1);
	constexpr Real cDeci  = Real(1E-1);
	constexpr Real cCenti = Real(1E-2);
	constexpr Real cMilli = Real(1E-3);
	constexpr Real cMicro = Real(1E-6);
	constexpr Real cNano  = Real(1E-9);
	constexpr Real cPico  = Real(1E-12);
	constexpr Real cFemto = Real(1E-15);
	constexpr Real cAtto  = Real(1E-18);
	constexpr Real cZepto = Real(1E-21);
	constexpr Real cYocto = Real(1E-24);

	constexpr Real ce = Real(2.718281828459);
	constexpr Real cPi = Real(3.14159265358979323846);
	constexpr Real c2Pi = cPi * Real(2);
	constexpr Real cPi$2 = cPi * Real(0.5);
	constexpr Real cPi$3 = cPi / Real(3);
	constexpr Real cPi$4 = cPi * Real(0.25);
	constexpr Real cPi$6 = cPi / Real(6);
	constexpr Real cPi$8 = cPi * Real(0.125);

	/*
	by mul these conversion factors
	@_Example:
		float _Angle = 180;
		auto _Radians = _Angle * cvtfRadians;
		auto _Degrees = _Radians * cvtfDegrees
	*/
	constexpr Real cvtfDegrees = Real(180) / cPi;
	constexpr Real cvtfRadians = cPi / Real(180);
	constexpr Real cvtfLinearcolor = Real(1) / Real(255);

}// namespace clmagic

#endif