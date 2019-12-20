#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_MATH_OTHER___COLORSPACE___H__
#define __CLMAGIC_CORE_GEOMETRY_MATH_OTHER___COLORSPACE___H__
#include "../other.h"

namespace clmagic 
{
	inline std::string to_hexstring(int _Hexnumber)
		{
		std::string _Str = "00000000x0";
		for (int i = 0; i != 8; ++i)
			{
			int _Tmp = _Hexnumber & 0xf;
			if (_Tmp > 9)
				{
				_Str[i] = 'a' + static_cast<char>(_Tmp - 10);
				}
			else
				{
				_Str[i] = '0' + static_cast<char>(_Tmp);
				}
			_Hexnumber >>= 4;
			}

		reverse(_Str.begin(), _Str.end());
		return (_Str);
		}

	/* < RGB > */ 
	inline ColorRGB::operator Vec3() const
		{
		const float _Invcolor = 1.0f / 255.0f;
		const auto _Linearcolor = Vec3(
			static_cast<float>(red) * _Invcolor,
			static_cast<float>(green) * _Invcolor,
			static_cast<float>(blue) * _Invcolor
			);
		return (_Linearcolor);
		}

	inline ColorRGB::operator std::string() const
		{
		std::string _Str = "rgb[" + std::to_string(red) + "," + std::to_string(green) + "," + std::to_string(blue) + "]";
		return (_Str);
		}

	inline std::string ColorRGB::to_hexstring() const
		{
		return clmagic::to_hexstring((red << 16) + (green << 8) + (blue));
		}
	/* </ RGB > */


	/* < HSV > */
	inline ColorHSV::operator Vec3() const
		{	// convert to linear color
		const float _Hue$60 = hue / 60.0f;
		const float _Hue$60_floor = std::floor(_Hue$60);
		const float _Hue$60_fraction = _Hue$60 - _Hue$60_floor;

		const std::array<float, 4> _Values = {
			value,
			value * (1.0f - saturation),
			value * (1.0f - (_Hue$60_fraction * saturation)),
			value * (1.0f - (1.0f - _Hue$60_fraction) * saturation)
			};

		const std::array<Vec3i, 6> _Swizzle = {
			Vec3i{ 0, 3, 1 },
			Vec3i{ 2, 0, 1 },
			Vec3i{ 1, 0, 3 },
			Vec3i{ 1, 2, 0 },
			Vec3i{ 3, 1, 0 },
			Vec3i{ 0, 1, 2 }
			};
		const size_t _Swizzleindex = static_cast<size_t>(_Hue$60_floor) % 6;

		return (Vec3(
			_Values[_Swizzle[_Swizzleindex][0]],
			_Values[_Swizzle[_Swizzleindex][1]],
			_Values[_Swizzle[_Swizzleindex][2]]));
		}

	inline ColorHSV::operator std::string() const
		{
		std::string _Str = "hsv[" + std::to_string(hue) + "," + std::to_string(saturation) + "," + std::to_string(value) + "]";
		return (_Str);
		}
	/* </ HSV > */

}// namespace clmagic 

#endif