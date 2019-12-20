#pragma once
#ifndef __CLMAGIC_CORE_BASIC___SYSTEM_CLOCK___H__
#define __CLMAGIC_CORE_BASIC___SYSTEM_CLOCK___H__
#include <chrono>
#include <string>

namespace clmagic 
{
	struct system_clock : public std::chrono::system_clock
	{
	private:
		using _Mybase = std::chrono::system_clock;

	public:
		static std::string  now_str() 
			{
			return ( std::to_string( _Mybase::to_time_t( _Mybase::now() ) ) );
			}
	
		static tm to_date_t(const time_point& _Time) 
			{
			tm _Result;
			time_t _T = to_time_t( _Time );

			localtime_s( &_Result, &_T );
			return ( _Result );
			}

		static std::string to_date_str(const time_point& _Time) 
			{
			time_t _T = to_time_t(_Time);

			char _Buf[128];
			ctime_s(_Buf, 99, &_T);
			return (_Buf);
			}

		static std::string now_date_str()
			{
			tm _Date = to_date_t(_Mybase::now());
			// yyyyMMdd
			int _Date2 = static_cast< int >( (_Date.tm_year + 1900) * 10000 + (_Date.tm_mon + 1) * 100 + _Date.tm_mday );
			return ( std::to_string(_Date2) );
			}
	};
}// namespace clmagic 

#endif