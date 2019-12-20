#include "string.h"

namespace clmagic 
{
#define EMPTY_VALUE_RETURN(VAL, RETURN) \
	if (VAL.empty()) return ( RETURN );

	std::string to_string( const std::wstring& In_Wstr )
		{
		EMPTY_VALUE_RETURN(In_Wstr, std::string())

		int _Len = WideCharToMultiByte( CP_ACP, 0, In_Wstr.c_str(), static_cast<int>(In_Wstr.size()), nullptr, 0, nullptr, nullptr );

		auto _Str = std::string( static_cast<size_t>(_Len), '\0' );
		
		WideCharToMultiByte( CP_ACP, 0, In_Wstr.c_str(), static_cast<int>(In_Wstr.size()), const_cast<char*>(_Str.c_str()), _Len, nullptr, nullptr );
		
		return (_Str);
		}

	std::wstring to_wstring( const std::string& In_Str )
		{
		EMPTY_VALUE_RETURN(In_Str, std::wstring())

		int _Len = MultiByteToWideChar(CP_ACP, 0, In_Str.c_str(), static_cast<int>(In_Str.size()), nullptr, 0);

		auto _Wstr = std::wstring( static_cast<size_t>(_Len), L'\0' );
		
		MultiByteToWideChar(CP_ACP, 0, In_Str.c_str(), static_cast<int>(In_Str.size()), const_cast<wchar_t*>(_Wstr.c_str()), _Len);
		
		return ( _Wstr );
		}

	std::string strclamp(const std::string& _Str, char _Left, char _Right)
		{
		EMPTY_VALUE_RETURN(_Str, std::string())

		size_t _Index1 = _Str.find(_Left);
		size_t _Index2 = _Str.find_last_of(_Right);

		if ( _Index1 != std::string::npos && _Index2 != std::string::npos
			&& _Index2 - 1 > _Index1 )
			{
			return ( _Str.substr(_Index1 + 1, _Index2 - _Index1 - 1) );
			}
		else
			{	// syntax error
			return std::string( );
			}
		}

	std::string strclamp( const std::string& _Str, const std::string& _Left, const std::string _Right )
		{
		EMPTY_VALUE_RETURN(_Str, std::string())

		size_t _Index1 = _Str.find(_Left);
		size_t _Index2 = _Str.find_last_of(_Right);
		if ( _Index1 == std::string::npos || _Index2 == std::string::npos )
			{
			return std::string();
			}

		_Index1 += _Left.size();
		_Index1 -= 1;// keep a character width
		_Index2 -= _Right.size();
		_Index2 += 1;// keep a character width

		if (_Index2 - 1 > _Index1)
			{
			return ( _Str.substr(_Index1 + 1, _Index2 - _Index1 - 1) );
			}
		else
			{	// syntax error
			return std::string();
			}
		}

	std::string strtrim(const std::string& _Str, char _Left, char _Right) {
		EMPTY_VALUE_RETURN(_Str, std::string())
		std::string::size_type _Start = 0;
		std::string::size_type _End = _Str.size() - 1;

		while (_Start != _Str.size() && _Str[_Start] == _Left)
			{	// right move
			++_Start;// cannot add 2
			}
		if (_Start == _Str.size())
			{	// end
			return (std::string());
			}

		while (_Str[_End] == _Right)
			{	// left move
			--_End;
			}

		// return _Str[_Start, _End]
		return (_Str.substr(_Start, _End - _Start + 1));
	}

	std::string strerase(const std::string& _Str, char _Target)
		{
		EMPTY_VALUE_RETURN(_Str, std::string())

		std::string _Result;
		for (char _Ch : _Str) 
			if (_Ch != _Target)
				_Result.push_back(_Ch);

		return (_Result);
		}

	std::string strreplace(const std::string& _Str, char _Old, char _New)
		{
		EMPTY_VALUE_RETURN(_Str, std::string())

		if (_New == '\0') 
			{	// erase charactor in _Str
			return strerase(_Str, _Old);
			}

		std::string _Result = _Str;

		// replace _Old to _New
		for (char& _Rch : _Result)
			if (_Rch == _Old)
				_Rch = _New;

		return (_Result);
		}

	std::vector<std::string> strsplit(const std::string& _Str, char _Contidtion)
		{
		EMPTY_VALUE_RETURN(_Str, std::vector<std::string>())

		std::vector<std::string> _Splited;

		size_t _Off = 0, _Seek = 0;

		while ( _Seek != _Str.size() )
			{
			if ( _Str[ _Seek ] == _Contidtion )
				{
				_Splited.push_back( _Str.substr(_Off, _Seek - _Off) );// because not need to current char, so cannot add 1
				_Off = _Seek + 1;
				}
			++_Seek;
			}

		if ( _Off == _Str.size() )
			{	// last section is string(empty)
			_Splited.push_back(std::string());
			}
		else
			{
			_Splited.push_back( _Str.substr(_Off) );
			}

		return ( _Splited );
		}

	std::string::size_type strcount(const std::string& _Str, char _Condition)
		{
		std::string::size_type _Count = 0;
		for ( std::string::value_type _Ch : _Str )
			if ( _Ch == _Condition )
				++_Count;
		return ( _Count );
		}

	std::string::size_type strfind(char _Target, const std::string& _Str, int _Offset)
		{	// return _Target index of _Str after _Offset
		for (int i = _Offset; i != _Str.size(); ++i)
			if (_Str[i] == _Target)
				return (i);

		return (std::string::npos);
		}

	std::string to_valign(const std::string& In_Str)
		{
		// Input:
		// abcdefg\n
		// hijklmn\n
		// opqrst\n
		// 
		// Output:
		// aho\n
		// bip\n
		// cjq\n
		// dkr\n
		// els\n
		// fnt\n
		// gk
		std::wstring _Raw = to_wstring(In_Str);

		std::vector<std::wstring> _Lcontent;
		_Lcontent.push_back(std::wstring());

		for (wchar_t _Lch : _Raw)
			{
			if (_Lch != '\n')
				_Lcontent.back().push_back(_Lch);
			else
				_Lcontent.push_back(std::wstring());
			}

		size_t _Align = _Lcontent[0].size();
		for (const std::wstring& _Lline : _Lcontent)
			{
			if (_Lline.size() > _Align)
				{	// greater of _Align
				_Align = _Lline.size();
				}
			}

		std::wstring _Lresult;
		_Lresult.reserve(_Lcontent.size() * (_Align + 1));

		// row convert to col
		for (size_t _Col = 0; _Col != _Align; ++_Col)
			{
			for (size_t _Row = 0; _Row != _Lcontent.size(); ++_Row)
				{
				if (_Lcontent[_Row].size() > _Col)
					{
					_Lresult.push_back(_Lcontent[_Row][_Col]);
					}
				else
					{	// push space char
					_Lresult.push_back(L' ');
					}
				}
			// push '\n' char
			_Lresult.push_back(L'\n');
			}

		return (to_string(_Lresult));
		}

	bool is_number(const std::string& _Number)
		{
		for (char _Ch : _Number)
			if (_Ch < '0' || _Ch > '9')
				return (false);

		return (true);
		}

	STR_NUMBER str_number(const std::string& _Number)
		{
		std::string _Str = strtrim(_Number);
		EMPTY_VALUE_RETURN(_Str, STR_NUMBER::STR_NAN)

		const std::string _Int32max = std::to_string(INT_MAX);
		const std::string _Uint32max = std::to_string(UINT_MAX);
		const std::string _Int64max = std::to_string(LLONG_MAX);
		const std::string _Uint64max = std::to_string(ULLONG_MAX);

		// 1. check signed
		bool _Unsigned = true;
		if (_Str.front() == '-') 
			{
			_Str = _Str.substr(1);
			_Unsigned = false;
			}

		// 2. check dot
		size_t _Dotcount = strcount(_Str, '.');
		if ( _Str.front() == '.' || _Dotcount > 1 )
			{
			return (STR_NUMBER::STR_NAN);
			}

		if (_Dotcount != 0) 
			{
			_Str = strerase(_Str, '.');
			}
		
		// 3. check type
		if (_Dotcount != 0) 
			{	// this is float number
			if (toupper(_Str.back()) == 'F') 
				{	// float
				if (is_number(std::string(_Str.begin(), _Str.end() - 1)))
					{
					return (STR_NUMBER::STR_FLOAT32);
					}
				}
			else 
				{	// double
				if (is_number(_Str))
					{
					return (STR_NUMBER::STR_FLOAT64);
					}
				}
			}
		else if (toupper(_Str[_Str.size() - 1]) == 'L' && toupper(_Str[_Str.size() - 2]) == 'L')
			{	// this is long long
			if (is_number(std::string(_Str.begin(), _Str.end() - 2)))
				{
				return (STR_NUMBER::STR_INT64);
				}
			}
		else 
			{	// integer
			auto _Numberlength = _Str.size();

			if (_Numberlength >= _Int32max.size())
				{	// _Number >= 1000000000
				if (_Numberlength == _Int32max.size()
					&& (is_number(_Str[0]) && _Str[0] <= _Int32max[0])
					&& (is_number(_Str[1]) && _Str[1] <= _Int32max[1])
					&& (is_number(_Str[2]) && _Str[2] < _Int32max[2]))
					{	// _Number < int32
					if (is_number(_Str)) 
						{
						return (STR_NUMBER::STR_INT32);
						}
					}
				else if ( _Unsigned 
					&& _Numberlength <= _Uint32max.size()
					&& (is_number(_Str[0]) && _Str[0] <= _Uint32max[0])
					&& (is_number(_Str[1]) && _Str[1] < _Uint32max[1]))
					{	// _Number is unsigned && _Number < uint32
					if (is_number(_Str))
						{
						return (STR_NUMBER::STR_UINT32);
						}
					}
				else
					{
					if (_Numberlength >= _Int64max.size()) 
						{	// _Number >= 1223372036854775807
						if (_Numberlength == _Int64max.size()
							&& (is_number(_Str[0]) && _Str[0] <= _Int64max[0])
							&& (is_number(_Str[1]) && _Str[1] < _Int64max[1]))
							{
							if (is_number(_Str))
								{
								return (STR_NUMBER::STR_INT64);
								}
							}
						else if (_Unsigned 
							&& _Numberlength <= _Uint64max.size()
							&& (is_number(_Str[0]) && _Str[0] <= _Uint64max[0])
							&& (is_number(_Str[1]) && _Str[1] < _Uint64max[1]))
							{
							if (is_number(_Str))
								{
								return (STR_NUMBER::STR_UINT64);
								}
							}
						}
					else 
						{	// _Number != uint32 && _Number < int64
						if (is_number(_Str))
							{
							return (STR_NUMBER::STR_INT64);
							}
						}
					}
				}
			else 
				{
				if (is_number(_Str))
					{
					return (STR_NUMBER::STR_INT32);
					}
				}
			}
		
		return ( STR_NUMBER::STR_NAN );
		}

	// <str_file>
	std::vector<std::string> str_file::read() const
		{
		using namespace::std;

		std::vector<std::string> _Result;

		fstream _Fin;
		_Fin.open( _Myfilename, ios::in );

		if (_Fin.is_open()) 
			{
			if (_Sperate) 
				{
				std::string _Tmp;
				while (std::getline(_Fin, _Tmp)) 
					{
					_Result.push_back(_Tmp);
					}
				}
			else 
				{
				_Result.push_back(std::string());
				while (_Fin.good())
					_Result.front().push_back(_Fin.get());
				}
			}
			
		_Fin.close();

		return ( _Result );
		}

	std::vector<std::string> str_file::read(bool (_Predicate)(std::string& _Strline))
		{
		using namespace::std;

		std::vector<std::string> _Result;

		fstream _Fin;
		_Fin.open(_Myfilename, ios::in);

		if (_Fin.is_open()) 
			{	// read start
			if (_Sperate)
				{	// mode 1
				std::string _Tmp;
				while (std::getline(_Fin, _Tmp))
					{
					if (_Predicate(_Tmp))
						{
						_Result.push_back(_Tmp);
						}
					}
				}
			else
				{	// mode 2
				_Result.push_back( std::string() );

				std::string _Tmp;
				while (std::getline(_Fin, _Tmp))
					{
					if (_Predicate(_Tmp))
						{
						_Result.front() += (_Tmp);
						}
					}
				}
			}

		_Fin.close();

		return (_Result);
		}

	void str_file::write(const std::vector<std::string>& _Content) const
		{
		using namespace::std;

		fstream _Fout;
		_Fout.open(_Myfilename, ios::out | ios::ate);

		if (_Fout.is_open()) 
			{
			std::for_each(_Content.begin(), _Content.end(),
				[&_Fout](const std::string& _Val) 
					{
					_Fout << _Val;
					}
				);
			}

		_Fout.close();
		}
	// </str_file>

}// namespace clmagic 