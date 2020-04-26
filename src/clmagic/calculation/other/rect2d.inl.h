#pragma once
#ifndef __CLMAGIC_CORE_GEOMETRY_matH_OTHER___RECT2D___H__
#define __CLMAGIC_CORE_GEOMETRY_matH_OTHER___RECT2D___H__

namespace clmagic 
{
	// <other>
	template<typename T> inline
		typename rect2d<T>::position_type rect2d<T>::rectcoords_cast(RECT_COORD In_Coord) {
			switch (In_Coord) {
				case RECT_COORD::LEFTTOP:	  return position_type(0.0f, 1.0f);
				case RECT_COORD::LEFTBOTTOM:  return position_type(0.0f, 0.0f);
				case RECT_COORD::RIGHTTOP:    return position_type(1.0f, 1.0f);
				case RECT_COORD::RIGHTBOTTOM: return position_type(1.0f, 0.0f);
				case RECT_COORD::CENTER:	  return position_type(0.5f, 0.5f);
				default: return position_type(0.0f, 0.0f);
			}
		}

	template<typename T> inline
		std::string rect2d<T>::to_string(RECT_COORD _Rectcoord) {
			switch (_Rectcoord) {
				case clmagic::RECT_COORD::LEFTTOP:    return (std::string("LEFTTOP"));
				case clmagic::RECT_COORD::LEFTBOTTOM: return (std::string("LEFTBOTTOM"));
				case clmagic::RECT_COORD::RIGHTTOP:   return (std::string("RIGHTTOP"));
				case clmagic::RECT_COORD::RIGHTBOTTOM: return (std::string("RIGHTBOTTOM"));
				case clmagic::RECT_COORD::CENTER:	  return (std::string("CENTER"));
				default: return (std::string());
			}
		}
	// </other>


	// <method>
	template<typename T> inline 
		int rect2d<T>::intersect(const position_type& _Point) const {
			// 0.0,1.0    1.0,1.0
			//	lt------+-------rt
			//  |  2    |   1   |
			//	|_____center____|
			//	|       |       |
			//  |  3    |   4   |
			//	lb------+-------rb 1.0,0.0
			//0.0,0.0(_Mypos)
			//
			vector2<T> _Local = _Point - _Mypos;
			vector2<T> _Uv = _Local / _Mysize;

			if (_Uv[0] > T(0) && _Uv[0] < T(1) && _Uv[1] > T(0) && _Uv[1] < T(1)) {
				if (_Uv[0] >= 0.5f) {// { 1, 4 }
					if (_Uv[0] >= 0.5f) {
						return (1);
					} else {
						return (4);
					}
				} else {// { 2, 3 }
					if (_Uv[1] >= 0.5f) {
						return (2);
					} else {
						return (3);
					}
				}
			}

			return (0);
		}

	template<typename T> inline
		bool rect2d<T>::intersect(const position_type& _Point, _out(Hit_<T>) _Result) const {
			vector2<T> _Local = _Point - _Mypos;
			vector2<T> _Uv = _Local / _Mysize;

			if (_Uv[0] > T(0) && _Uv[0] < T(1) && _Uv[1] > T(0) && _Uv[1] < T(1)) {
				_Result._Myresult = eShapeType::ePLANE;
				_Result._Myt      = T(0);
				_Result._Myorigin = Hit_<T>::position_type(_Point, T(0));
				_Result._Mynormal = unit_vector<T>(0.f, 0.f, 1.f);
				_Result._Myuv     = _Uv;
				return ( true );
			}

			return (false);
		}

	template<typename T> inline 
		typename rect2d<T>::position_type rect2d<T>::translate(T _dX, T _dY) {
			vector2<T> _Oldpos = _Mypos;
			_Mypos = _Mypos + Vec2(_dX, _dY);
			return (_Oldpos);
		}

	template<typename T> inline
		typename rect2d<T>::region_type rect2d<T>::scale_step(T _sX, T _sY) {
			region_type _Oldsize = _Mysize;
			_Mysize = _Mysize + region_type(_sX, _sY);
			_Mypos = _Mypos - position_type(_sX * T(0.5), _sY * T(0.5));
			return (_Oldsize);
		}

	template<typename T> inline
		typename rect2d<T>::region_type rect2d<T>::scale_rate(T _sX, T _sY) {
			return scale_step((_sX - T(1)) * width(), (_sY - T(1)) * height());
		}

	template<typename T> template< typename _Ty > inline
		vector2< _Ty > rect2d<T>::get_pos(const position_type& _Uv) const {
			auto _Pos = _Mypos + _Mysize * _Uv;
			return vector2<_Ty>(static_cast<_Ty>(_Pos[0]), static_cast<_Ty>(_Pos[1]));
		}

	template<typename T> template<typename _Ty> inline
		vector2< _Ty > rect2d<T>::get_pos(T _U, T _V) const { 
			return get_pos<_Ty>( position_type(_U, _V) );
		}

}// namespace clmagic

#endif