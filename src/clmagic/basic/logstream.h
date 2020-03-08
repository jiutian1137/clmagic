#pragma once
#ifndef __CLMAGIC_CORE_BASIC___LOGSTREAM___H__
#define __CLMAGIC_CORE_BASIC___LOGSTREAM___H__
#include "singleton.h"
#include "chrono.h"
#include <iostream>
#include <sstream>
#include <fstream>

namespace clmagic 
{
	/*using _Elem = char;
	using _Traits = std::char_traits<_Elem>;
	using _Alloc = std::allocator<_Elem>;*/

	template<typename _Elem,
		typename _Traits = std::char_traits<_Elem>, 
		typename _Alloc = std::allocator<_Elem>>
	class log_stringbuf
		: public std::basic_stringbuf<char, _Traits, _Alloc>
	{
		using _Mybase = std::basic_stringbuf<char, _Traits, _Alloc>;

	public:
		using _Mysb = typename _Mybase::_Mysb;
		using _Mystr = typename std::basic_string<_Elem, _Traits, _Alloc>;
		using size_type = typename _Mystr::size_type;
		using filename_type = typename std::string;

		explicit log_stringbuf(
			const filename_type& _Filename,
			std::ios_base::openmode _Mode = std::ios_base::in | std::ios_base::out)
			: _Mybase(_Mode), _Myfilename(_Filename)
			{	// construct empty character buffer from mode
			}

		explicit log_stringbuf(
			const filename_type& _Filename,
			const _Mystr&	     _Str, 
			std::ios_base::openmode _Mode = std::ios_base::in | std::ios_base::out)
			: _Mybase(_Str, _Mode), _Myfilename(_Filename)
			{	// construct character buffer from string, mode
			}

		virtual ~log_stringbuf() 
			{	// please destroy this, when application end
			_Output_to_file();
			}

	protected:
		virtual std::streamsize xsputn(const char *_Ptr, std::streamsize _Count) override
			{
			auto _Return = _Mybase::xsputn(_Ptr, _Count);
			_Mybase::overflow('\n');

			if (full())
				{
				_Output_to_file();
				}

			return (_Return);
			}

		virtual void _Output_to_file() 
			{
			std::fstream _Fout;
			_Fout.open(_Myfilename, std::ios::out | std::ios::app);

			if (_Fout.is_open()) 
				{
				_Fout << system_clock::to_date_str(system_clock::now());// included '\n'
				_Fout << _Mybase::str() << '\n';
				_Mybase::_Tidy();
				}
			
			_Fout.close();
			std::cout << "_Output_to_file()" << std::endl;
			}

	public:
		bool full() const
			{
			return (_Mybase::str().size() >= _Mylimit);
			}

		size_type limit() const
			{
			return (_Mylimit);
			}

		size_type limit(size_type _Newlimit)
			{
			auto _Oldlimit = _Mylimit;
			_Mylimit = _Newlimit;
			return (_Oldlimit);
			}

		filename_type filename() const
			{
			return (_Myfilename);
			}

		filename_type filename(const filename_type& _Newfilename) 
			{
			auto _Oldfilename = _Myfilename;
			_Myfilename = _Newfilename;
			return (_Oldfilename);
			}

	private:
		size_type _Mylimit;
		filename_type _Myfilename;
	};

	
	template<typename _Elem,
		typename _Traits = std::char_traits<_Elem>,
		typename _Alloc = std::allocator<_Elem>>
	class basic_logstream
		: public std::basic_ostream<_Elem, _Traits>, 
		public singleton<basic_logstream<_Elem, _Traits, _Alloc>>
	{
		using _Mybase = std::basic_ostream<_Elem, _Traits>;
		
	public:
		using _Mysb = log_stringbuf<_Elem, _Traits, _Alloc>;
		using _Mystr = typename _Mysb::_Mystr;
		using size_type = typename _Mysb::size_type;
		using filename_type = typename _Mysb::filename_type;
		

		basic_logstream( const std::string& _Str = std::string("log/") + system_clock::now_date_str() + std::string(".txt"),
			std::ios_base::openmode _Mode = std::ios_base::out)
			: _Mybase(&_Stringbuffer),
			  _Stringbuffer(_Str, _Mode | std::ios_base::out)
			{	// construct default logfile by current system_clock
			}

		virtual ~basic_logstream() noexcept
			{	// destroy the object
			}

		_Mysb *rdbuf() const
			{	// return pointer to buffer
			return ((_Mysb *)&_Stringbuffer);
			}

		_Mystr str() const
			{	// return string copy of character array
			return (_Stringbuffer.str());
			}

		void str(const _Mystr& _Newstr)
			{	// replace character array from string
			_Stringbuffer.str(_Newstr);
			}

		size_type limit() const
			{
			return ( _Stringbuffer.limit() );
			}

		size_type limit(size_type _Newlimit)
			{
			return ( _Stringbuffer.limit(_Newlimit) );
			}

		filename_type filename() const
			{
			return ( _Stringbuffer.filename() );
			}

		filename_type filename(const filename_type& _Newfilename)
			{
			return ( _Stringbuffer.filename(_Newfilename) );
			}

	private:
		_Mysb _Stringbuffer;
	};

	using logstream = basic_logstream<char>;
	using wlogstream = basic_logstream<wchar_t>;

}// namespace clmagic 

#endif