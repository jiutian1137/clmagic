#include "logcout.h"

namespace clmagic
{
	logcout_wrapper logcout;


	std::string std_mes(const std::string& _Tymes, const std::string& _Mes, 
		const std::string& _Pos_func, const std::string& _Pos_file, size_t _Pos_line)
		{
		constexpr const char* _Format =
			"%s[ %s ]\n"
			"\t in[ %s ]\n"
			"\t at[ %s ]\n"
			"\t at[ %s ]";
		constexpr const char* _Maxline = "100000";

		const size_t _Fmtlen = strnlen_s(_Format, 100);
		const size_t _Mllen = strnlen_s(_Maxline, 100);

		std::string _Stdmes;
		_Stdmes.resize(_Fmtlen + _Tymes.size() + _Mes.size() + _Pos_func.size() + _Pos_file.size() + _Mllen);

		size_t _Len = snprintf( const_cast<char*>(_Stdmes.data()), _Stdmes.size(), _Format,
			_Tymes.c_str(), _Mes.c_str(),
			_Pos_func.c_str(),
			_Pos_file.c_str(),
			std::to_string(_Pos_line).c_str() );

		return (_Stdmes);
		}

	void std_assert(bool _Expr, const std::string& _Mes, 
		const std::string& _Pos_func, const std::string& _Pos_file, size_t _Pos_line)
		{
		if (!_Expr) 
			{
			logcout << std_mes("error", _Mes, _Pos_func, _Pos_file, _Pos_line);
			auto* _Ostrptr = logcout.set(std::cout);
			if (_Ostrptr != &std::cout) 
				{
				delete _Ostrptr;// call deconstructor
				}

			abort();
			}
		}


}// namespace clmagic