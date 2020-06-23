#pragma once
#ifndef clmagic_calculation_physic_DYNAMICS_h_
#define clmagic_calculation_physic_DYNAMICS_h_
#include "../lapack/vector.h"
#include "quantity.h"

namespace clmagic {

	// [2020.06.16, LongJiangnan]
	template<typename _Tv, typename _Ts = scalar_t<_Tv>,
		typename _Length_unit = meter, 
		typename _Time_unit   = second>
	struct velocity {
		using scalar_type = _Ts;
		using vector_type = _Tv;

		using length_type = quantity<vector_type, _Length_unit>;
		using time_type   = quantity<scalar_type, _Time_unit>;

		length_type ddistance;
		time_type   dtime;
		// velocity = ddistance/dtime
		
		velocity() = default;
		
		velocity(const velocity&) = default;

		velocity(vector_type _Vector, time_type _Dtime = SECONDS(1))
			: ddistance(_Vector), dtime(_Dtime) {}

		velocity(length_type _Ddistance, time_type _Dtime)
			: ddistance(_Ddistance), dtime(_Dtime) {}

		velocity& operator=(const velocity&) = default;

		velocity& operator=(vector_type _Vector) {
			ddistance = length_type(_Vector);
			dtime     = SECONDS(1);
		}

		const vector_type to_vector() const {
			auto _SI_dx = unit_cast<meter>(ddistance);
			auto _SI_dt = unit_cast<second>(dtime);
			return static_cast<vector_type>(_SI_dx) / static_cast<scalar_type>(_SI_dt);
		}

		const std::string to_string() const {
			return ddistance.to_string() + " / " + dtime.to_string();
		}

		void set_time(time_type _New_time) {
			/*<graph>
				<--------5[s] == <----------------2.5[s]
			</graph>
			<formula>
				v = a_dx/a_dt
				v = c_dx/b_dt
				v = a_dx/a_dt * (a_dt/b_dt) * (b_dt/a_dt)
				v = a_dx/b_dt * (b_dt/a_dt)
				v = a_dx*(b_dt/a_dt) / b_dt
				c_dx = a_dx * (b_dt/a_dt)
				new_ddistance = old_ddistance * (new_dtime / old_dtime)
			</formula>*/
			this->ddistance *= (_New_time / this->dtime);
			this->dtime      = _New_time;
		}

		const velocity operator-() const {
			return velocity(-ddistance, dtime);
		}

		const velocity operator+(velocity _Right) const {
			_Right.set_time(this->dtime);
			return velocity(this->ddistance + _Right.ddistance, this->dtime);
		}

		const velocity operator-(velocity _Right) const {
			_Right.set_time(this->dtime);
			return velocity(this->ddistance - _Right.ddistance, this->dtime);
		}

		const length_type operator*(time_type t) const {
			/*<formula>
			     dx              t
				---- * t = dx * ----
				 dt              dt
			</formula>*/
			return ( ddistance * (t / dtime) );
		}

		friend const length_type operator*(time_type _Left, velocity _Right) {
			return (_Right * _Left);
		}

		friend std::ostream& operator<<(std::ostream& _Ostr, velocity _Left) {
			return (_Ostr << _Left.to_string());
		}
	};

	// [2020.06.16, LongJiangnan]
	template<typename _Tv, typename _Ts = scalar_t<_Tv>,
		typename _Length_unit = meter, 
		typename _Time_unit   = second>
	struct acceleration {
		using scalar_type = _Ts;
		using vector_type = _Tv;

		using length_type   = quantity<vector_type, _Length_unit>;
		using time_type     = quantity<scalar_type, _Time_unit>;
		using velocity_type = velocity<_Tv, _Ts, _Length_unit, _Time_unit>;

		velocity_type dvelocity;
		time_type     ddtime;
		// acceleration = dv/ddt

		acceleration() = default;

		acceleration(const acceleration&) = default;

		explicit acceleration(vector_type _Vector)
			: dvelocity(length_type(_Vector), SECONDS(1)), ddtime(SECONDS(1)) {}

		explicit acceleration(length_type _Ddistance, time_type _Dtime = SECONDS(1), time_type _Ddtime = SECONDS(1))
			: dvelocity(_Ddistance, _Dtime), ddtime(_Ddtime) {}

		acceleration(velocity_type _Dvelocity, time_type _Ddtime)
			: dvelocity(_Dvelocity), ddtime(_Ddtime) {}

		acceleration& operator=(const acceleration&) = default;
		
		acceleration& operator=(vector_type _Vector) {
			this->dvelocity.dx = length_type(_Vector);
			this->dvelocity.dt = SECONDS(1);
			this->ddtime       = SECONDS(1);
			return *this;
		}

		const vector_type to_vector() const {
			auto _SI_ddt = unit_cast<second>(ddtime);
			return dvelocity.to_vector() / static_cast<scalar_type>(_SI_ddt);
		}

		const std::string to_string() const {
			return dvelocity.to_string() + " / " + ddtime.to_string();
		}

		void set_time(time_type _New_time) {
			/*<graph>
				<--------5[s] == <----------------2.5[s]
			</graph>*/
			this->dvelocity.ddistance *= (_New_time / this->ddtime);
			this->ddtime               = _New_time;
		}
	
		const acceleration operator-() const {
			return acceleration(-dvelocity, ddtime);
		}

		const acceleration operator+(acceleration _Right) const {
			_Right.set_time(this->ddtime);
			return acceleration(this->dvelocity + _Right.dvelocity, this->ddtime);
		}

		const acceleration operator-(acceleration _Right) const {
			_Right.set_time(this->ddtime);
			return acceleration(this->dvelocity - _Right.dvelocity, this->ddtime);
		}

		const velocity_type operator*(time_type t) const {
			/*<formula>
				   dx          dx     t
				------- * t = ---- * ---
				 dt*ddt        dt    ddt
			</formula>*/
			return velocity_type(this->dvelocity.ddistance * (t / this->ddtime),
				                 this->dvelocity.dtime );
		}

		friend const velocity_type operator*(time_type _Left, acceleration _Right) {
			return (_Right * _Left);
		}

		friend std::ostream& operator<<(std::ostream& _Ostr, acceleration _Left) {
			return (_Ostr << _Left.to_string());
		}
	};

	// [2020.06.16, LongJiangnan]
	template<typename _Tv, typename _Ts = scalar_t<_Tv>, 
		typename _Length_unit = meter, 
		typename _Time_unit   = second, 
		typename _Mass_unit   = kilogram>
	struct linear_momentum {
		using vector_type = _Tv;
		using scalar_type = _Ts;
		
		using length_type = quantity<vector_type, _Length_unit>;
		using time_type   = quantity<scalar_type, _Time_unit>;
		using mass_type   = quantity<scalar_type, _Mass_unit>;
		using velocity_type = velocity<_Tv, _Ts, _Length_unit, _Time_unit>;

		velocity_type velocity;
		mass_type     mass;
		// linear_momentum = velocity * mass

		linear_momentum() = default;

		linear_momentum(const linear_momentum&) = default;

		explicit linear_momentum(vector_type _Vector, mass_type _Mass = KILOGRAMS(1))
			: velocity(length_type(_Vector), SECONDS(1)), mass(_Mass) {}

		explicit linear_momentum(length_type _Ddistance, time_type _Dtime = SECONDS(1), mass_type _Mass = KILOGRAMS(1))
			: velocity(_Ddistance, _Dtime), mass(_Mass) {}

		linear_momentum(velocity_type _Velocity, mass_type _Mass)
			: velocity(_Velocity), mass(_Mass) {}

		linear_momentum& operator=(const linear_momentum&) = default;

		linear_momentum& operator=(vector_type _Vector) {
			this->velocity.ddistance = length_type(_Vector);
			this->velocity.dtime     = SECONDS(1);
			this->mass               = KILOGRAMS(1);
			return *this;
		}
	
		const vector_type to_vector() const {
			auto _SI_mass = unit_cast<meter>(mass);
			return velocity.to_vector() * static_cast<scalar_type>(_SI_mass);
		}

		const std::string to_string() const {
			return velocity.to_string() + " * " + mass.to_string();
		}

		void set_mass(mass_type _New_mass) {
			this->velocity.ddistance *= (this->mass / _New_mass);
			this->mass                = _New_mass;
		}

		const linear_momentum operator-() const {
			return linear_momentum(-velocity, mass);
		}

		const linear_momentum operator+(linear_momentum _Right) const {
			_Right.set_mass(this->mass);
			return linear_momentum(this->velocity + _Right.velocity, this->mass);
		}
		
		const linear_momentum operator-(linear_momentum _Right) const {
			_Right.set_mass(this->mass);
			return linear_momentum(this->velocity - _Right.velocity, this->mass);
		}

		friend std::ostream& operator<<(std::ostream& _Ostr, const linear_momentum& _Left) {
			return (_Ostr << _Left.to_string());
		}
	};

	// [2020.06.16, LongJiangnan]
	template<typename _Tv, typename _Ts = scalar_t<_Tv>,
		typename _Length_unit = meter, 
		typename _Time_unit   = second, 
		typename _Mass_unit   = kilogram>
	struct force {
		using vector_type = _Tv;
		using scalar_type = _Ts;
		
		using length_type = quantity<vector_type, _Length_unit>;
		using time_type   = quantity<scalar_type, _Time_unit>;
		using mass_type   = quantity<scalar_type, _Mass_unit>;
		
		using acceleration_type = acceleration<_Tv, _Ts, _Length_unit, _Time_unit>;
		using linear_momentum_type = linear_momentum<_Tv, _Ts, _Length_unit, _Time_unit, _Mass_unit>;

		acceleration_type acceleration;
		mass_type         mass;
		// force = accel * mass, accel = dx / dt / ddt

		force() = default;

		force(const force&) = default;
		
		explicit force(vector_type _Vector, mass_type _Mass = KILOGRAMS(1))
			: acceleration(length_type(_Vector), SECONDS(1), SECONDS(1)), mass(_Mass) {}

		explicit force(length_type _Ddistance, time_type _Dtime = SECONDS(1), time_type _Ddtime = SECONDS(1), mass_type _Mass = KILOGRAMS(1))
			: acceleration(_Ddistance, _Dtime, _Ddtime), mass(_Mass) {}

		force(acceleration_type _Accel, mass_type _Mass)
			: acceleration(_Accel), mass(_Mass) {}

		force& operator=(const force&) = default;
		
		force& operator=(vector_type _v) {
			acceleration.dvelocity.ddistance = length_type(_v);
			acceleration.dvelocity.dtime     = SECONDS(1);
			acceleration.ddtime              = SECONDS(1);
			mass                             = KILOGRAMS(1);
			return *this;
		}

		const unit_vector3<_Ts> direction() const {
			return unit_vector3<_Ts>(this->acceleration.dvelocity.ddistance.num);
		}
	
		const vector_type to_vector() const {
			auto _SI_mass = unit_cast<second>(mass);
			return acceleration.to_vector() * static_cast<scalar_type>(_SI_mass);
		}

		const std::string to_string() const {
			return (acceleration.to_string() + " * " + mass.to_string());
		}

		void set_mass(mass_type _New_mass) {
			/*<graph>
				<------5[kg] == <------------2.5[kg]
			</graph>
			_New_dx = dx * (a_mass/b_mass)
			*/
			this->acceleration.dvelocity.ddistance *= (this->mass / _New_mass);
			this->mass                              = _New_mass;
		}

		const force operator-() const {
			return force(-acceleration, mass);
		}

		const force operator+(force _Right) const {
			_Right.set_mass(this->mass);
			return force(this->acceleration + _Right.acceleration, this->mass);
		}
		
		const force operator-(force _Right) const {
			_Right.set_mass(this->mass);
			return force(this->acceleration - _Right.acceleration, this->mass);
		}

		const linear_momentum_type operator*(time_type t) const {
			/*<formula>
				  dx                        t
				------- * m * t = dv * m * ---
				 dt*ddt                    ddt
			</formula>*/
			auto _Accel  = this->acceleration;
			auto _Dveloc = _Accel.dvelocity;
			_Dveloc.ddistance *= (t / _Accel.ddtime);
			return linear_momentum_type(_Dveloc, this->mass);
		}

		friend std::ostream& operator<<(std::ostream& _Ostr, const force& _Left) {
			return (_Ostr << _Left.to_string());
		}
	};

	// [2020.06.17, LongJiangnan]
	template<typename _Tv, typename _Ts = scalar_t<_Tv>,
		typename _Length_unit = meter, 
		typename _Time_unit   = second, 
		typename _Mass_unit   = kilogram>
	struct energy {
		using vector_type = _Tv;
		using scalar_type = _Ts;
		
		using length_type = quantity<vector_type, _Length_unit>;
		using time_type   = quantity<scalar_type, _Time_unit>;
		using mass_type   = quantity<scalar_type, _Mass_unit>;

		using force_type  = force<_Tv, _Ts, _Length_unit, _Time_unit, _Mass_unit>;

		force_type  force;
		length_type distance;
		// energy = F * dx
		/*
		 dx
		sigma( F ) = sum_of(force, 0, distance) = dot(force, distance)
		  0
		*/

		energy() = default;

		energy(const energy&) = default;

		energy(vector_type _Force, vector_type _Distance)
			: force(length_type(_Force), SECONDS(1), SECONDS(1), KILOGRAMS(1)), distance(length_type(_Distance)) {}

		energy(force_type _Force, vector_type _Distance) 
			: force(_Force), distance(length_type(_Distance)) {}

		energy(force_type _Force, length_type _Distance)
			: force(_Force), distance(_Distance) {}

		scalar_type to_scalar() const {
			auto _SI_dis = unit_cast<meter>(distance);
			return dot(force.to_vector(), static_cast<vector_type>(_SI_dis));
		}

		std::string to_string() const {
			return (force.to_string() + " * " + distance.to_string());
		}

		friend std::ostream& operator<<(std::ostream& _Ostr, const energy& _Left) {
			return (_Ostr << _Left.to_string());
		}
	};


	template<typename _Tv, typename _Ts> inline
	energy<_Tv, _Ts> dot(force<_Tv, _Ts> F, quantity<_Tv, meter> dx) {
		return energy<_Tv, _Ts>(F, dx);
	}

	template<typename _Tv, typename _Ts> inline
	energy<_Tv, _Ts> dot(force<_Tv, _Ts> F, _Tv dx) {
		return energy<_Tv, _Ts>(F, dx);
	}

	template<typename _Tv, typename _Ts> inline
	force<_Tv, _Ts> make_gravity(KILOGRAMS _Mass) {
		return force<_Tv, _Ts>({ 0.f, 9.8f, 0.f }, _Mass);
	}

	/*<formula>
		       v = v0 + a * t
		                   1
		  x - x0 = v0*t + ---*a*pow(t,2)
		                   2
		pow(v,2) = pow(v0,2) + 2*a*(x - x0)
		            1
		  x - x0 = ---*(v0+v)*t
		            2
					      1
		  x - x0 = v*t - ---*a*pow(t,2)
		                  2
	</formula>*/

}// namespace clmagic

#endif