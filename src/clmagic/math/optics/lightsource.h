#pragma once
#ifndef clmagic_math_optics_LIGHTSOURCE_h_
#define clmagic_math_optics_LIGHTSOURCE_h_
#include "../lapack/vector.h"

namespace clmagic {
	
	template<typename _Ty, typename _Bk>
	struct light_source {
		vector3<_Ty, _Bk> power;

		light_source() = default;

		light_source(const vector3<_Ty, _Bk>& _Power) : power(_Power) {}
	};


	// Klight() = power
	template<typename _Ty, typename _Bk = _Ty>
	struct directional_light : public light_source<_Ty, _Bk> {
		using _Mybase = light_source<_Ty, _Bk>;
		unit_vector3<_Ty, _Bk> direction;

		directional_light() = default;

		directional_light(const vector3<_Ty, _Bk>& _Power, const unit_vector3<_Ty, _Bk>& _Direction)
			: _Mybase(_Power), direction(_Direction) {}

		vector3<_Ty> operator()() const {
			return _Mybase::power;
		}
	};


	// Klight(distance) = power * fdist(distance)
	template<typename _Ty, typename _Bk = _Ty>
	struct point_light : public light_source<_Ty, _Bk> {// punctual-light
		using _Mybase = light_source<_Ty, _Bk>;
		vector3<_Ty, _Bk> position;
		_Ty               radius;

		point_light() = default;

		point_light(const vector3<_Ty, _Bk>& _Power, const vector3<_Ty, _Bk>& _Position, _Ty _Radius)
			: _Mybase(_Power), position(_Position), radius(_Radius) {}

		_Ty fdist(_Ty distance) const {// 1 - pow(d/r,2)
			const auto rate = distance / radius;
			return (static_cast<_Ty>(1) - rate * rate);
		}

		vector3<_Ty> operator()(_Ty distance) const {
			return _Mybase::power * fdist(distance);
		}
	};


	// Klight(distance, L) = power * fdist(distance) * fdir(L)
	// Look Book<<RealTimeRendering 4th>> page:115
	template<typename _Ty, typename _Bk = _Ty>
	struct spot_light : public point_light<_Ty, _Bk> {
		using _Mybase = point_light<_Ty, _Bk>;
		unit_vector3<_Ty, _Bk> direction;
		_Ty cos_umbra;
		_Ty cos_penumbra;

		spot_light() = default;

		spot_light(const point_light<_Ty, _Bk>& _Pointlight, const unit_vector3<_Ty, _Bk>& _Direction, _Ty _Umbra, _Ty _Penumbra)
			: _Mybase(_Pointlight), direction(_Direction), cos_umbra(_Umbra), cos_penumbra(_Penumbra) {}

		spot_light(const vector3<_Ty, _Bk>& _Power, const vector3<_Ty, _Bk> & _Position, _Ty _Radius, const unit_vector3<_Ty, _Bk>& _Direction, _Ty _Umbra, _Ty _Penumbra)
			: _Mybase(_Power, _Position, _Radius), direction(_Direction), cos_umbra(_Umbra), cos_penumbra(_Penumbra) {}

		spot_light(const directional_light<_Ty, _Bk>& _Dirlight, const vector3<_Ty, _Bk>& _Position, _Ty _Radius, _Ty _Umbra, _Ty _Penumbra)
			: spot_light(_Dirlight.power, _Position, _Radius, _Dirlight.direction, _Umbra, _Penumbra) {}

		_Ty fdir(vector3<_Ty, _Bk> L) const {
			const auto t = saturate( dot(-L, direction) - cos_umbra / (cos_penumbra - cos_umbra) );
			return s_curve(t);
		}

		// L: toLight, normlize(light.position - Pws)
		vector3<_Ty> operator()(_Ty distance, vector3<_Ty, _Bk> L) const {
			return _Mybase::power * _Mybase::fdist(distance) * fdir(L);
		}
	};

	
	template<typename _Ty, typename _Bk = _Ty>
	struct common_light_source {
	private:
		// light's direction important than light's position
		enum {
			POWER     = 0,// [0,3)
			RADIUS    = 3,// [3]
			DIRECTION = 4,// [4,7)
			COS_UMBRA = 7,// [7]
			POSITION  = 8,// [8,11)
			COS_PENUMBRA = 11// [11]
		};

	public:
		common_light_source() = default;
		
		common_light_source(const directional_light<_Ty, _Bk>& _Light) {
			this->power()     = _Light.power;
			this->direction() = _Light.direction;
		}

		common_light_source(const point_light<_Ty, _Bk>& _Light) {
			this->power()     = _Light.power;
			this->position()  = _Light.position;
			this->radius()    = _Light.radius;
		}

		common_light_source(const spot_light<_Ty, _Bk>& _Light) {
			this->power()     = _Light.power;
			this->direction() = _Light.direction;
			this->position()  = _Light.position;
			this->radius()    = _Light.radius;
			this->cos_umbra()    = _Light.cos_umbra;
			this->cos_penumbra() = _Light.cos_penumbra;
		}

		vector3<_Ty, _Bk>& power() {
			return reinterpret_cast< vector3<_Ty, _Bk>& >( _Mydata[POWER] ); }
		vector3<_Ty, _Bk>& direction() {
			return reinterpret_cast< vector3<_Ty, _Bk>& >( _Mydata[DIRECTION] ); }
		vector3<_Ty, _Bk>& position() {
			return reinterpret_cast< vector3<_Ty, _Bk>& >( _Mydata[POSITION] ); }
		_Ty& radius() {
			return reinterpret_cast< _Ty& >( _Mydata[RADIUS] ); }
		_Ty& cos_umbra() {
			return reinterpret_cast< _Ty& >( _Mydata[COS_UMBRA] ); }
		_Ty& cos_penumbra() {
			return reinterpret_cast< _Ty& >( _Mydata[COS_PENUMBRA] ); }

		const vector3<_Ty, _Bk>& power() const {
			return reinterpret_cast< const vector3<_Ty, _Bk>& >( _Mydata[POWER] ); }
		const vector3<_Ty, _Bk>& direction() const {
			return reinterpret_cast< const vector3<_Ty, _Bk>& >( _Mydata[DIRECTION] ); }
		const vector3<_Ty, _Bk>& position() const {
			return reinterpret_cast< const vector3<_Ty, _Bk>& >( _Mydata[POSITION] ); }
		const _Ty& radius() const {
			return reinterpret_cast< const _Ty& >( _Mydata[RADIUS] ); }
		const _Ty& cos_umbra() const {
			return reinterpret_cast< const _Ty& >( _Mydata[COS_UMBRA] ); }
		const _Ty& cos_penumbra() const {
			return reinterpret_cast< const _Ty& >( _Mydata[COS_PENUMBRA] ); }

		bool is_point_light() const {
			const auto dir  = this->direction();
			const auto zero = static_cast<_Ty>(0);
			return dir[0] == zero && dir[1] == zero && dir[2] == zero;
		}

		bool is_directional_light() const {
			return radius() == static_cast<_Ty>(0);
		}

		bool is_spot_light() const {
			const auto cos_zero = cos(static_cast<_Ty>(0));
			return (cos_umbra() != cos_zero && cos_penumbra() != cos_zero);
		}

	private:
		_Ty _Mydata[COS_PENUMBRA + 1];
	};


}// namespace clmagic

#endif // !clmagic_math_optics_LIGHTSOURCE_h_
