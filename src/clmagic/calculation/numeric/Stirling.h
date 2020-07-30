//--------------------------------------------------------------------------------------
// Copyright (c) (Unknown) James-Stirling(1692-1770)
// All Rights Reserved
//--------------------------------------------------------------------------------------
#pragma once
#include "real.h"

namespace Stirling {
	template<typename _Ty>
	_Ty factorial(_Ty n) {// sqrt(2Pi*n) * pow(n/e, n)
		return static_cast<_Ty>( _CSTD sqrt(6.283 * n) * _CSTD pow(n / 2.71828, n) );
	}
}// Stirling
