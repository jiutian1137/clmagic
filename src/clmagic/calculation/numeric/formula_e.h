//--------------------------------------------------------------------------------------
// Copyright (c) 2019 LongJiangnan
// All Rights Reserved
// Free License
//--------------------------------------------------------------------------------------
#pragma once
#include "formula_t.h"

namespace clmagic {

	template<typename _Fml>
	struct formula_result : public _Fml { };

	template<typename _Fml>
	struct formula<formula_result<_Fml>, void, void> {
		using result_formula = formula_result<_Fml>;
		using result_type    = typename result_formula::result_type;
		using result_unit    = typename result_formula::result_unit;
	};


	template<typename _Unit>
	struct _Result_unit_symbol {
		std::string operator()() const {
			return std::string(unit_symbol_v<_Unit>);
		}
	};

	// _Xu {+-*/} _Yu = _Xu ? _Yu
	template<typename _Xu, typename _Yu, typename _BinOp>
	struct _Result_unit_symbol<_Ternary_template<_Xu, _Yu, _BinOp>> {
		std::string operator()() const {
			return _Result_unit_symbol<_Xu>()() + operation_symbol_v<_BinOp>  + _Result_unit_symbol<_Yu>()();
		}
	};

	// _Xu {+-*/} _Number = _Xu
	template<typename _Xu, typename _BinOp>
	struct _Result_unit_symbol<_Ternary_template<_Xu, void, _BinOp>> {
		std::string operator()() const {
			return _Result_unit_symbol<_Xu>()();
		}
	};

	// _Number {+-*/} _Yu = _Yu
	template<typename _Yu, typename _BinOp>
	struct _Result_unit_symbol<_Ternary_template<void, _Yu, _BinOp>> {
		std::string operator()() const {
			return _Result_unit_symbol<_Yu>()();
		}
	};


	// template recursion with funknown
	template<typename _Fml>
	class formula_t< formula_result<_Fml> > {
	public:
		using result_type = typename formula_result<_Fml>::result_type;
		using result_unit = typename formula_result<_Fml>::result_unit;
		
		result_type result_value;

		formula_t() = default;
		formula_t(const formula_t&) = default;

		template<typename ..._Tys, std::enable_if_t<std::is_constructible_v<result_type, _Tys&&...> , int> = 0>
		formula_t(_Tys&&... _Args) : result_value(std::forward<_Tys>(_Args)...) {}

		formula_t& operator=(const formula_t&) = default;

		template<typename _Ty, std::enable_if_t<std::is_convertible_v<_Ty, result_type>, int> = 0>
		formula_t& operator=(const _Ty& _Arg) { 
			result_value = static_cast<result_type>(_Arg); 
			return *this; 
		}
	
		result_type get_result() const {
			return result_value;
		}

		std::string to_string() const {
			using std::to_string;
			return to_string(result_value) + _Unit_symbol_style(_Result_unit_symbol<result_unit>()());
		}
	};

}// namespace clmagic