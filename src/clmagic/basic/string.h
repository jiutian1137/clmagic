#pragma once
#ifndef __CLMAGIC_CORE_BASIC___STRING___H__
#define __CLMAGIC_CORE_BASIC___STRING___H__
#include <regex>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <Windows.h>


namespace clmagic 
{
	// return string(empty) if syntex error
	std::string 
		to_string(const std::wstring& In_Wstr);

	std::wstring 
		to_wstring(const std::string& In_Str);

	std::string 
		strclamp(const std::string& _Str, char _Left, char _Right);

	/*
	@_example:
		using namespace::clmagic;

		std::string _Str = "<aaaaaaaaa>";
		std::cout << strclamp(_Str, "<", ">") << std::endl; // "aaaaaaaaa"

		std::string _Str2 = "arr[12345]";
		std::cout << strclamp(_Str2, "[", "]") << std::endl; // "12345"

		std::string _Str3 = "L<12345>J";
		std::cout << strclamp(_Str3, "L<", ">J") << std::endl; // "12345"
	*/
	std::string 
		strclamp(const std::string& _Str, const std::string& _Left, const std::string _Right);

	/*
	@_example:
		using namespace::clmagic;

		std::string _Str = "  abcdefg";
		std::cout << strtrim(_Str) << std::endl;// "abcdefg"

		std::string _Str2 = "      abcdefg     ";
		std::cout << strtrim(_Str2) << std::endl;// "abcdefg"

		std::string _Str3 = "           ";
		std::cout << strtrim(_Str3) << std::endl;// _Str is ""

		std::string _Str4 = "bbbbbbbabcdefghhhhhhh";
		std::cout << strtrim(_Str4, 'b', 'h') << std::endl;// "abcdefg"
	*/
	std::string 
		strtrim(const std::string& _Str, char _Left = ' ', char _Right = ' ');

	/*
	@_example:
		using namespace::clmagic;

		std::string _Str = "abssssss";
		std::cout << strerase(_Str, 's') << std::endl;// "ab"
	*/
	std::string 
		strerase(const std::string& _Str, char _Target);

	/*
	@_example:
		using namespace::clmagic;

		std::string _Str;// empty
		std::cout << strreplace(_Str, '2', '9') << std::endl;// empty string

		std::string _Str2 = "yangxia1111";
		std::cout << strreplace(_Str2, '1') << std::endl;// "yangxia"

		std::string _Str3 = "jin222ti2222an";
		std::cout << strreplace(_Str3, '2', '9') << std::endl;// "jin999ti9999an"
	*/
	std::string 
		strreplace(const std::string& _Str, char _Old, char _New = '\0');

	/*
	@_example:
		using namespace::clmagic;

		auto _Ouput_function = [](const std::vector<std::string>& _Str) 
			{
			for (int i = 0; i != _Str.size(); ++i) 
				{
				std::cout << "[" << std::to_string(i) << "]" << "{\"" << _Str[i] << "\"}" << " ";
				}
			std::cout << std::endl;
			};

		std::string _Str;// empty
		auto _Result = strsplit(_Str, 's');// empty vector<string>
		_Ouput_function(_Result);

		std::string _Str2 = "aaa, ssss,ccc";
		auto _Result2 = strsplit(_Str2, ',');// [0]{"aaa"} [1]{" ssss"} [2]{"ccc"}
		_Ouput_function(_Result2);

		std::string _Str3 = "aaa,ccc,";
		auto _Result3 = strsplit(_Str3, ',');// [0]{"aaa"} [1]{"ccc"} [2]{""}
		_Ouput_function(_Result3);

		std::string _Str4 = ",aaa,ccc";
		auto _Result4 = strsplit(_Str4, ',');// [0]{""} [1]{"aaa"} [2]{"ccc"}
		_Ouput_function(_Result4);
	*/
	std::vector<std::string> 
		strsplit(const std::string& _Str, char _Contidtion);

	std::string::size_type
		strcount(const std::string& _Str, char _Condition);

	std::string::size_type 
		strfind(char _Target, const std::string& _Str, int _Offset = 0);

	std::string 
		to_valign(const std::string& In_Str);

	inline bool 
		is_number(char _Number) { return (_Number >= '0' || _Number <= '9'); }
	
	bool 
		is_number(const std::string& _Number);

	enum STR_NUMBER
	{
		STR_NAN = 0x00000000,
		STR_INF = 0x10000000,
		STR_INT32 = 0x00000001,
		STR_UINT32 = 0x00000002,
		STR_INT64 = 0x00000010,
		STR_UINT64 = 0x00000020,
		STR_FLOAT16 = 0x00000100,
		STR_FLOAT32 = 0x00000200,
		STR_FLOAT64 = 0x00000300
	};
	/*
	@_example:
		using namespace::clmagic;

		std::string _Tmp;
		while (std::cin >> _Tmp && _Tmp.find("quit") == std::string::npos) 
			{
			switch (str_number(_Tmp))
				{	
				case STR_NUMBER::STR_NAN:	 std::cout << "Nan" << std::endl; break;
				case STR_NUMBER::STR_INF:	 std::cout << "Infinite" << std::endl; break;
				case STR_NUMBER::STR_INT32:  std::cout << "int32" << std::endl; break;
				case STR_NUMBER::STR_UINT32: std::cout << "uint32" << std::endl; break;
				case STR_NUMBER::STR_INT64:  std::cout << "int64" << std::endl; break;
				case STR_NUMBER::STR_UINT64: std::cout << "uint64" << std::endl; break;
				case STR_NUMBER::STR_FLOAT16: std::cout << "float16" << std::endl; break;
				case STR_NUMBER::STR_FLOAT32: std::cout << "float32" << std::endl; break;
				case STR_NUMBER::STR_FLOAT64: std::cout << "float64" << std::endl; break;
				}
			_Tmp.clear();
			}
	*/
	STR_NUMBER 
		str_number(const std::string& _Number);

	inline bool
		is_nan(const std::string& _Number) 
		{
		auto _Result = str_number(_Number);
		return ((_Result == STR_NUMBER::STR_NAN || _Result == STR_NUMBER::STR_INF) ? true : false);
		}


	/*
	@example:
		std::string _Filename = "test_str_file";

		clmagic::str_file(_Filename).write(
			"今天和杨霞出去玩了,\n"
			"晚上再和杨霞出去玩,\n"
			"明天还可以和杨霞出去玩,\n"
			"后天还可以和杨霞出去玩,\n"
			"后天的晚上还可以和杨霞出去玩,\n"
			"后后天还是可以和杨霞出去玩,\n"
			"每一天都非常非常快乐"
			);

		std::string _Str = clmagic::str_file(_Filename).read();
		std::cout << _Str << std::endl;
	*/
	class str_file 
	{
	public:
		str_file( const std::string& _Filename, bool _Is_sperate = true )
			: _Myfilename( _Filename ), _Sperate(_Is_sperate)
			{
			}

		inline std::vector<std::string>
			operator() ( ) const { return ( read( ) ); }
		inline void 
			operator() ( const std::vector<std::string>& _Content) const { write(_Content); }

		std::vector<std::string> read() const;
		std::vector<std::string> read(bool (_Predicate)(std::string& _Strline));
		void write(const std::vector<std::string>& _Content) const;

		std::string _Myfilename;
		bool _Sperate;
	};


	template<typename _Ty>
	inline _Ty string_width(const std::string& In_Str, _Ty In_Ftsize)
		{
		int _Tmp = 0;
		int _Maxlen = 0;
		for (char ch : In_Str)
			{
			if (ch != '\n')
				{
				++_Tmp;
				}
			else
				{
				if (_Tmp > _Maxlen)
					{
					_Maxlen = _Tmp;
					}
				_Tmp = 0;
				}
			}

		if (_Tmp > _Maxlen)
			{
			_Maxlen = _Tmp;
			}
		return static_cast<_Ty>(_Maxlen * In_Ftsize * 0.5);
		}

	template<typename _Ty>
	inline _Ty string_height(const std::string& In_Str, _Ty In_Ftsize)
		{
		// \n
		// 1111\n
		// 222\n
		// 333\n
		// \n
		size_t _Count = 1;
		for (char ch : In_Str)
			{
			if (ch == '\n')
				++_Count;
			}

		return static_cast<_Ty>(_Count * In_Ftsize * 0.5);
		}

}// namespace clmagic 

#endif