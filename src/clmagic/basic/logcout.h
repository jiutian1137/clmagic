#ifndef __CLMAGIC_CORE_BASIC___LOGCOUT___H__
#define __CLMAGIC_CORE_BASIC___LOGCOUT___H__
#include <iostream>
#include <string>

namespace clmagic
{
	struct logcout_wrapper
	{
		operator std::ostream&() const noexcept
			{
			return (*std::_Ptr_cout);
			}

		std::ostream& operator<< (const char* _Ptr) const 
			{
			(*std::_Ptr_cout) << _Ptr;
			return (*this);
			}

		std::ostream& operator<< (const std::string& _Str) const 
			{
			(*std::_Ptr_cout) << _Str;
			return (*this);
			}

		std::ostream& operator<< (std::ostream& (__cdecl *_Pfn)(std::ostream&))
			{	// call basic_ostream manipulator
			(*std::_Ptr_cout) << _Pfn;
			return (*this);
			}

		std::ostream* set(std::ostream& _Newstr) 
			{
			std::ostream* _Oldstr = std::_Ptr_cout;
			std::_Ptr_cout = &_Newstr;
			return (_Oldstr);
			}
	};

	extern logcout_wrapper logcout;

	/*
	stantard format:
		message_type[ message ]
			in[ function_name ]
			at[ _FILE_ ]
			at[ _LINE_ ]
	*/
	std::string std_mes(const std::string& _Tymes,
		const std::string& _Mes, const std::string& _Pos_func, const std::string& _Pos_file, size_t _Pos_line);

	void std_assert(bool _Expr, 
		const std::string& _Mes, const std::string& _Pos_func, const std::string& _Pos_file, size_t _Pos_line);

	inline void debug_std_assert(bool _Expr,
		const std::string& _Mes, const std::string& _Pos_func, const std::string& _Pos_file, size_t _Pos_line)
		{
#ifndef _DEBUG
		std_assert(_Expr, _Mes, _Pos_func, _Pos_file, _Pos_line);
#endif
		}


	/*
	@_clmagic_error:
		op_error: this operation error
		arg_error: argument error
		calc_error: calculate error
		nullptr_error: null pointer error

		code_error: code is bad [Test]
		rand_error: random result error
		thread_error: thread error
	*/
#define clmagic_assert(_Expr, _Mes, _Pos_func) std_assert(_Expr, _Mes, _Pos_func, __FILE__, __LINE__)
#define clmagic_debug_assert(_Expr, _Mes, _Pos_func) debug_std_assert(_Expr, _Mes, _Pos_func, __FILE__, __LINE__)

}// namespace clmagic

#endif