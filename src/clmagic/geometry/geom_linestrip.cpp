#include "geom_linestrip.h"

namespace clmagic 
{
	void normalize_weight(linestrip<vertex_pos3_weight>& _Spline)
		{
		float _Max = _Spline.front().second;
		for (auto& _Vt : _Spline)
			{
			if (_Vt.second > _Max)
				{
				_Max = _Vt.second;
				}
			}

		float _Maxinv = 1.0f / _Max;

		for (auto& _Vt : _Spline)
			{
			_Vt.second *= _Maxinv;
			}
		}

	void helical_line(std::vector<Vec3>& _Dst, const Vec3& _Start, const Vec3& _End,
		const std::vector<std::pair<float, float>>& _Widths, const std::vector<std::pair<float, float>>& _Tsteps, const std::vector<std::pair<float, float>>& _Rotsteps)
		{
		float _Length = length(_Start, _End);

		float _Tstep = _Length * cCenti;
		float _Width = 0.0f;
		float _Rstep = float(c2Pi * cCenti);
		
		std::vector<Vec3> _Result;

		Vec3 _Dir = normalize(_End - _Start);
		mat4 _Linecoord = mat4::identity();
		if ( !equals( _Dir, cY, Vec3(0.0002f) ) ) 
			{
			_Linecoord = static_cast<mat4>(Quaternion(cross(cY, _Dir), std::acos(dot(cY, _Dir))));
			}
		_Linecoord *= translation_matrix(_Start);

		if (_Widths.empty() && _Tsteps.empty() && _Rotsteps.empty()) 
			{	// this is line
			for (float t = 0.0f, theta = 0.0f; t <= _Length; t += _Tstep, theta += _Rstep)
				{
				theta = mod(theta, c2Pi.val);
				Vec3 _Point = Vec3(_Width, t, 0.0f);
				_Point = Vec3( Vec4(_Point, 1.0f) * _Linecoord );

				auto _Quat = Quaternion(cY, theta);
				_Point = _Quat.rotate(_Point);
				_Dst.push_back(_Point);
				}
			// end
			}
		else 
			{
			size_t _Windex = 0;
			size_t _Tindex = 0;
			size_t _Rindex = 0;

			if (!_Widths.empty() && _Widths.front().first == 0.0f)
				{
				_Width = _Widths.front().second;
				_Windex = 1;
				}

			if (!_Tsteps.empty() && _Tsteps.front().first == 0.0f)
				{
				_Tstep = _Tsteps.front().second;
				_Tindex = 1;
				}

			if (!_Rotsteps.empty() && _Rotsteps.front().first == 0.0f)
				{
				_Rstep = _Rotsteps.front().second;
				_Rindex = 1;
				}

			
			float t = 0.0f, theta = 0.0f;
			for (; t <= _Length; t += _Tstep, theta += _Rstep)
				{
				if (_Windex < _Widths.size() && t >= _Widths[_Windex].first)
					{
					_Width = _Widths[_Windex++].second;
					}

				if (_Tindex < _Tsteps.size() && t >= _Tsteps[_Tindex].first)
					{
					_Tstep = _Tsteps[_Tindex++].second;
					}

				if (_Rindex < _Rotsteps.size() && t >= _Rotsteps[_Rindex].first)
					{
					_Rstep = _Rotsteps[_Rindex++].second;
					}

				auto _Point = Vec3(_Width, t, 0.0f);
				auto _Quat = Quaternion( cY, fmod(theta, c2Pi.val) );
				_Point = _Quat.rotate(_Point);
				_Point = Vec3( Vec4(_Point, 1.0f) * _Linecoord );
				_Dst.push_back(_Point);
				}

			theta = mod(theta, c2Pi.val);
			if ( equals(t, _Length, 0.0002f) && equals(theta, c2Pi.val, 0.0002f) && equals(theta, 0.0f, 0.0002f) )
				{	// do nothing
				}
			else 
				{
				_Dst.push_back(_End);
				}
			}
		}
}// namespace clmagic