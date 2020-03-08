#include "glsl_shader_analysis.h"
#include <regex>

namespace clmagic 
{
	class _Tokenizer 
	{
	public:
		std::vector<std::string> operator() (const std::string& _Content) 
			{
			std::string _match_anyhead = "(^|(\\s+)|(?=[;])|(?=[}])|(?=[)])|[=])";
			std::string _match_anytill = "($|(?=\\s+)|(?=[;])|(?=[{])|(?=[(]))";
			std::string _Stmt = "(" + _match_anyhead + "(layout[(].+[)])" + ")"
							 + "|(" + "(^|(\\s*)[{].+[}])" + ")" /*code block*/
							 + "|(" + "(^|(\\s*)[(].+[)])" + ")" /* */
							 + "|(" + "([=](.+)(?=[;]))" + ")"
						 	 + "|(" + _match_anyhead + "(\\w+)" + _match_anytill + ")";
			auto _Re = std::regex(_Stmt);

			auto _First = std::regex_token_iterator<std::string::const_iterator>(_Content.begin(), _Content.end(), _Re);
			auto _Last = std::regex_token_iterator<std::string::const_iterator>();

			_Re = std::regex(".*\\w.*");
			std::smatch _match_result;
			std::vector<std::string> _Result;

			while (_First != _Last)
				{
				const std::string& _Tmp = *_First;
				if (std::regex_match(_Tmp, _match_result, _Re))
					{
					_Result.push_back(_match_result[0]);
					}
				++_First;
				}

			return (_Result);
			}
	};

	class _String_to_var 
	{
		bool operator() (code_var& _Dst)
			{

			return (_Return_isvar);
			}

		std::string _In_Content;
		bool _Return_isvar;
	};

	class _Shader_analysis 
	{
	public:
		_Shader_analysis(std::vector<std::string>& _Content)
			: _Mycontent(_Content), _Mycrtline(0)
			{
			}
	
		void operator() (shader_property& _Dst)
			{
			while (_Mycrtline < _Mycontent.size()) 
				{
				// process...
				++_Mycrtline;
				}
			}

		void process(shader_property& _Dst) 
			{
			(*this)(_Dst);
			}

		std::vector<std::string>& _Mycontent;
		size_t _Mycrtline;

	};


	inline bool ignore_comment(std::string& _Val) 
		{
		size_t _Pos = _Val.find("//");
		if (_Pos != std::string::npos)
			{
			_Val = _Val.substr(0, _Pos + 1);
			}

		return true;
		}

	void shader_analysis(const std::string& _Filename, shader_property& _Dst) 
		{
		auto _Content = str_file(_Filename).read(ignore_comment);

		_Shader_analysis _Analysiser(_Content);
		_Analysiser.process(_Dst);
		}


	glslshader_interpreter::variant
	glslshader_interpreter::variant_cast(const std::string& _Chunk)
		{
		int _Error = 0;
		variant _Variant;
		int _Seek = 0;

		// 1. filter front space
		while (_Seek != _Chunk.size() && _Chunk[_Seek] == ' ')
			{
			++_Seek;
			}

		if (_Seek == _Chunk.size())
			{	// syntax error
			_Error = 1;
			}

		if (_Error == 0)
			{
			// 2. filter type
			while (_Seek != _Chunk.size() && _Chunk[_Seek] != ' ')
				{
				_Variant.first += _Chunk[_Seek];
				++_Seek;
				}

			if (_Seek == _Chunk.size())
				{	// syntax error
				_Error = 2;
				}

			// 3. filter middle space
			if (_Error == 0)
				{
				while (_Seek != _Chunk.size() && _Chunk[_Seek] == ' ')
					{
					++_Seek;
					}

				if (_Seek == _Chunk.size())
					{	// syntax error
					_Error = 3;
					}

				// 4. filter variant name
				if (_Error == 0)
					{	// syntax Ok
					_Variant.second = _Chunk.substr(_Seek);

					size_t _Index = _Variant.second.find(' ');
					if (_Index != std::string::npos)
						{
						_Variant.second = _Variant.second.substr(0, _Index);
						}

					_Index = _Variant.second.find(';');
					if (_Index != std::string::npos)
						{
						_Variant.second = _Variant.second.substr(0, _Index);
						}
					}
				}
			}

		switch (_Error)
			{
			case 0: return (_Variant);

			default:return (variant());
			}
		}

	void
	glslshader_interpreter::push_uniform(const std::string& _Chunk)
		{
		size_t _Keywordindex = _Chunk.find(Keyword_Uniform);
		if (_Keywordindex != std::string::npos)
			{
			variant _Var = variant_cast(_Chunk.substr(_Keywordindex + strlen(Keyword_Uniform)));

			if (_Var.first.empty() || _Var.second.empty())
				{	// syntax error
				return;
				}

			std::string _Arraysize = str_clamp(_Var.second, '[', ']');
			if (_Arraysize.empty())
				{	// 
				m_Uniforms.push_back(_Var);
				}
			else
				{	// this is array
				if (is_nan(_Arraysize))
					{	// array length is const variant
					auto _LengthKeyval = m_Consts.find(_Arraysize);
					if (_LengthKeyval != m_Consts.end())
						{
						int _Length = -1;

						switch (_LengthKeyval->second.m_Type)
							{
							case BASE_TYPE::BS_INT32: _Length = _LengthKeyval->second.m_AsInt; break;
							case BASE_TYPE::BS_UINT32: _Length = static_cast<int>(_LengthKeyval->second.m_AsUint); break;
							default: break;
							}

						if (_Length > 0)
							{	// syntax OK
							_Var.second = _Var.second.substr(0, _Var.second.find('['));
							for (int i = 0; i != _Length; ++i)
								{
								m_Uniforms.push_back(std::pair<std::string, std::string>(_Var.first, _Var.second + '[' + std::to_string(i) + ']'));
								}
							}
						}
					}
				else
					{	// array length is number
					int _Length = std::stoi(_Arraysize);
					_Var.second = _Var.second.substr(0, _Var.second.find('['));
					for (int i = 0; i != _Length; ++i)
						{
						m_Uniforms.push_back(std::pair<std::string, std::string>(_Var.first, _Var.second + '[' + std::to_string(i) + ']'));
						}
					}
				}
			}
		}

	void
	glslshader_interpreter::check_uniform()
		{
		for (auto _Present = m_Uniforms.begin(); _Present != m_Uniforms.end(); ++_Present)
			{
			_Present->first = str_replace(_Present->first, '\n');
			_Present->first = str_replace(_Present->first, '\t');

			_Present->second = str_replace(_Present->second, '\n');
			_Present->second = str_replace(_Present->second, '\t');
			}
		}

	void
	glslshader_interpreter::push_inblock(const std::string& _Chunk)
		{
		size_t _Keywordindex = _Chunk.find(Keyword_In);
		if (_Keywordindex != std::string::npos)
			{
			variant _Var = variant_cast(_Chunk.substr(_Keywordindex + strlen(Keyword_In)));

			// check
			if (_Var.first.empty() || _Var.second.empty())
				{
				return;
				}

			m_InBlock.push_back(_Var);
			}
		}

	void
	glslshader_interpreter::push_outblock(const std::string& _Chunk)
		{
		size_t _Keywordindex = _Chunk.find(Keyword_Out);
		if (_Keywordindex != std::string::npos)
			{
			variant _Var = variant_cast(_Chunk.substr(_Keywordindex + strlen(Keyword_Out)));

			// check
			if (_Var.first.empty() || _Var.second.empty())
				{
				return;
				}

			m_OutBlock.push_back(_Var);
			}
		}

	void
	glslshader_interpreter::push_struct(const std::string& _Chunk)
		{
		std::istringstream _Isstream(_Chunk);
		std::string _Tempstr;

		_Isstream >> _Tempstr;
		if (_Tempstr == Keyword_Struct)
			{
			std::string _Structname;
			_Isstream >> _Structname >> _Tempstr;
			if (_Tempstr == "{")
				{	// grammar is correct
				std::vector<variant> _Structproperties;
				while (std::getline(_Isstream, _Tempstr, ';') && _Tempstr.find("}") == std::string::npos)
					{
					_Tempstr = str_replace(_Tempstr, '\t');
					_Tempstr = str_replace(_Tempstr, '\n');

					variant _Var = variant_cast(_Tempstr);
					if (_Var.first.empty() || _Var.second.empty())
						{
						return;
						}

					_Structproperties.push_back(_Var);
					}// '};'
				m_Structs.insert_or_assign(_Structname, _Structproperties);
				}
			}
		}

	void
	glslshader_interpreter::check_struct()
		{
		std::map<std::string, std::vector<variant>> _Old = m_Structs;
		m_Structs.clear();
		for (auto _Present = _Old.begin(); _Present != _Old.end(); ++_Present)
			{
			std::string _Structname = str_replace(_Present->first, '\n');
			_Structname = str_replace(_Structname, '\t');

			std::vector<variant> _Memberlist(_Present->second.size());
			for (int i = 0; i != _Present->second.size(); ++i)
				{
				const variant& _constVar = _Present->second[i];
				_Memberlist[i].first = str_replace(_constVar.first, '\n');
				_Memberlist[i].first = str_replace(_constVar.first, '\t');
				_Memberlist[i].second = str_replace(_constVar.second, '\n');
				_Memberlist[i].second = str_replace(_constVar.second, '\t');
				}
			m_Structs.insert_or_assign(_Structname, _Memberlist);
			}
		}

	void
	glslshader_interpreter::push_const(const std::string& _Chunk)
		{
		size_t _Keywordindex = _Chunk.find(Keyword_Const);
		size_t _Statementend = _Chunk.find('=');
		if (_Keywordindex != std::string::npos && _Statementend != std::string::npos)
			{	// have 'const' identifier and statement and definition
			size_t _Statementstart = _Keywordindex + strlen(Keyword_Const);
			std::string _Statement = _Chunk.substr(_Statementstart, _Statementend - _Statementstart - 1);
			std::string _Definition = _Chunk.substr(_Statementend + 1);
			variant _Var = variant_cast(_Statement);

			// check
			if (_Var.first.empty() || _Var.second.empty())
				{
				return;
				}

			//
			size_t _Index = _Definition.find(';');
			if (_Index != std::string::npos)
				{
				_Definition = _Definition.substr(0, _Index);
				}

			if (!is_nan(_Definition))
				{	// this is number
				variable32bit2 _Val;
				if (_Definition.find('.') == std::string::npos)
					{	// not is float
					_Val.m_Type = BASE_TYPE::BS_INT32;
					_Val.m_AsInt = std::stoi(_Definition);
					}
				else
					{	// is float
					_Val.m_Type = BASE_TYPE::BS_FLOAT;
					_Val.m_AsFloat = std::stof(_Definition);
					}
				m_Consts.insert_or_assign(_Var.second, _Val);
				}
			}
		}

	void
	glslshader_interpreter::check_const()
		{
		for (auto _Present = m_Consts.begin(); _Present != m_Consts.end(); ++_Present)
			{
			/*replace(_Present->first, '\n');
			replace(_Present->first, '\t');*/
			}
		}

	void
	glslshader_interpreter::push_function(const std::string& _Chunk)
		{
		std::istringstream _Isstream(_Chunk);

		Function _Func;
		_Isstream >> _Func.m_Return;
		if (std::getline(_Isstream, _Func.m_Name, '('))
			{
			std::string _Argschunk;
			if (std::getline(_Isstream, _Argschunk, ')'))
				{	// function arguments
				_Argschunk = str_replace(_Argschunk, '\t');
				_Argschunk = str_replace(_Argschunk, '\n');
				if (!(str_trim(_Argschunk)).empty())
					{
					auto _Args = str_split(_Argschunk, ',');
					for (const auto& _Argstr : _Args)
						{
						_Func.m_Arguments.push_back(variant_cast(_Argstr));
						if (_Func.m_Arguments.back().first.empty() || _Func.m_Arguments.back().second.empty())
							{	// syntax error
							return;
							}
						}
					}

				std::string _Tempstr;
				_Isstream >> _Tempstr;
				size_t _Leftbracketcount = str_count(_Tempstr, '{');
				size_t _Rightbracketcount = str_count(_Tempstr, '}');

				if (_Leftbracketcount != 0 && _Rightbracketcount != 0 && _Leftbracketcount == _Rightbracketcount)
					{
					size_t _Index1 = _Tempstr.find('{');
					size_t _Index2 = _Tempstr.find_last_of('}');
					_Func.m_Body = _Tempstr.substr(_Index1 + 1, _Index2 - (_Index1 + 1));
					}
				else if (_Leftbracketcount >= 1)
					{
					_Func.m_Body = _Tempstr.substr(_Tempstr.find('{') + 1);
					while (_Leftbracketcount > _Rightbracketcount)
						{
						if (!std::getline(_Isstream, _Tempstr, '}'))
							break;
						++_Rightbracketcount;

						size_t _Otherstart = _Tempstr.find('{');
						while (_Otherstart != std::string::npos)
							{
							++_Leftbracketcount;
							_Otherstart = str_find('{', _Tempstr, _Otherstart + 1);
							}

						if (_Leftbracketcount == _Rightbracketcount)
							{
							_Func.m_Body += _Tempstr;
							break;
							}
						else
							{
							_Func.m_Body += _Tempstr + '}';
							}
						}

					if (_Leftbracketcount == _Rightbracketcount)
						{
						_Func.m_Name = str_trim(_Func.m_Name);
						if (_Func.m_Name == Keyword_Main && _Func.m_Return == Keyword_Void && _Func.m_Arguments.empty())
							{
							m_MainFunction = _Func;
							}
						else
							{
							m_Functions.push_back(_Func);
							}
						}
					}

				}
			}
		}

	void
	glslshader_interpreter::check_function()
		{
		for (auto _Present = m_Functions.begin(); _Present != m_Functions.end(); ++_Present)
			{
			_Present->m_Return = str_replace(_Present->m_Return, '\n');
			_Present->m_Return = str_replace(_Present->m_Return, '\t');

			_Present->m_Name = str_replace(_Present->m_Name, '\n');
			_Present->m_Name = str_replace(_Present->m_Name, '\t');

			for (auto _Arg = _Present->m_Arguments.begin(); _Arg != _Present->m_Arguments.end(); ++_Arg)
				{
				_Arg->first = str_replace(_Arg->first, '\n');
				_Arg->first = str_replace(_Arg->first, '\t');

				_Arg->second = str_replace(_Arg->second, '\n');
				_Arg->second = str_replace(_Arg->second, '\t');
				}
			}
		}

	std::map<glslshader_interpreter::variant, int>
	glslshader_interpreter::get_uniform_list()
		{
		std::map<variant, int> _Ulist;
		for (auto _First = m_Uniforms.begin(); _First != m_Uniforms.end(); ++_First)
			{
			auto _Struct = m_Structs.find(_First->first);
			if (_Struct == m_Structs.end())
				{	// simple variant
				_Ulist.insert_or_assign(*_First, -1);
				}
			else
				{
				for (auto _Mem = _Struct->second.begin(); _Mem != _Struct->second.end(); ++_Mem)
					{	// _Mem: vec3:first, attrib0:second
					variant _Var = *_First;
					_Var.second.push_back('.');
					_Var.first = _Mem->first;
					_Var.second += _Mem->second;
					_Ulist.insert_or_assign(_Var, -1);
					}
				}
			}
		return (_Ulist);
		}


	//------------------------------------------------------------------------------------------------
	void
	read(const std::string& In_Filename, std::string& Out_Dst)
		{
		std::string _Tempstr;
		std::ifstream _Fin;

		_Fin.open(In_Filename, std::ios::in);
		while (std::getline(_Fin, _Tempstr, '\n'))
			{
			size_t _Index = _Tempstr.find("//");
			if (_Index != std::string::npos)
				{
				Out_Dst += _Tempstr.substr(0, _Index);
				}
			else
				{
				Out_Dst += _Tempstr;
				}
			Out_Dst += '\n';
			}
		_Fin.close();
		}

	void
	read(const std::string& In_Shaderfile, glslshader_interpreter& Out_Dst)
		{
		// 1. load content
		std::string _Content;
		std::string _Tempstr;
		std::istringstream _Isstream;
		
		read( In_Shaderfile, std::ref(_Content) );
		_Isstream.str(_Content);
		while (_Isstream >> _Tempstr)
			{
			if (_Tempstr == clmagic::glslshader_interpreter::Keyword_In)
				{
				std::getline(_Isstream, _Tempstr, ';');
				Out_Dst.push_inblock(clmagic::glslshader_interpreter::Keyword_In + _Tempstr);
				}
			else if (_Tempstr == clmagic::glslshader_interpreter::Keyword_Out)
				{
				std::getline(_Isstream, _Tempstr, ';');
				Out_Dst.push_outblock(clmagic::glslshader_interpreter::Keyword_Out + _Tempstr);
				}
			else if (_Tempstr == clmagic::glslshader_interpreter::Keyword_Uniform)
				{
				std::getline(_Isstream, _Tempstr, ';');
				Out_Dst.push_uniform(clmagic::glslshader_interpreter::Keyword_Uniform + _Tempstr);
				}
			else if (_Tempstr == clmagic::glslshader_interpreter::Keyword_Const)
				{
				std::getline(_Isstream, _Tempstr, ';');
				Out_Dst.push_const(clmagic::glslshader_interpreter::Keyword_Const + _Tempstr);
				}
			else if (_Tempstr == clmagic::glslshader_interpreter::Keyword_Struct)
				{
				std::getline(_Isstream, _Tempstr, '}');
				Out_Dst.push_struct(clmagic::glslshader_interpreter::Keyword_Struct + _Tempstr + "}");
				}
			else if ((_Tempstr[0] == clmagic::glslshader_interpreter::Keyword_Int[0] && _Tempstr == clmagic::glslshader_interpreter::Keyword_Int)
				|| (_Tempstr[0] == clmagic::glslshader_interpreter::Keyword_Uint[0] && _Tempstr == clmagic::glslshader_interpreter::Keyword_Uint)
				|| (_Tempstr[0] == clmagic::glslshader_interpreter::Keyword_Float[0] && _Tempstr == clmagic::glslshader_interpreter::Keyword_Float)
				|| (_Tempstr[0] == clmagic::glslshader_interpreter::Keyword_Double[0] && _Tempstr == clmagic::glslshader_interpreter::Keyword_Double)
				|| (_Tempstr[0] == clmagic::glslshader_interpreter::Keyword_Vec2[0] && (_Tempstr == clmagic::glslshader_interpreter::Keyword_Void
																				|| _Tempstr == clmagic::glslshader_interpreter::Keyword_Vec2
																				|| _Tempstr == clmagic::glslshader_interpreter::Keyword_Vec3
																				|| _Tempstr == clmagic::glslshader_interpreter::Keyword_Vec4))
				|| (_Tempstr[0] == clmagic::glslshader_interpreter::Keyword_mat2[0] && (_Tempstr == clmagic::glslshader_interpreter::Keyword_mat2
																				|| _Tempstr == clmagic::glslshader_interpreter::Keyword_mat3
																				|| _Tempstr == clmagic::glslshader_interpreter::Keyword_mat4
																				|| _Tempstr == clmagic::glslshader_interpreter::Keyword_mat3x4)))
				{
				std::string _Type;
				std::string _Identifier;
				std::string _Arguments;

				_Type = _Tempstr;
				_Isstream >> _Identifier;
				size_t _Leftbracketcount = clmagic::str_count(_Identifier, '{');
				size_t _Rightbracketcount = clmagic::str_count(_Identifier, '}');

				if (_Leftbracketcount != 0 && _Rightbracketcount != 0 && _Leftbracketcount == _Rightbracketcount)
					{
					Out_Dst.push_function(_Type + " " + _Identifier);
					}
				else if (_Leftbracketcount == 0 && _Rightbracketcount == 0)
					{
					std::getline(_Isstream, _Arguments, '{');
					_Arguments += '{';
					++_Leftbracketcount;
					}

				std::string _Chunk;
				while (_Leftbracketcount > _Rightbracketcount)
					{
					std::getline(_Isstream, _Tempstr, '}');
					++_Rightbracketcount;

					size_t _Otherstart = _Tempstr.find('{');
					while (_Otherstart != std::string::npos)
						{
						++_Leftbracketcount;
						_Otherstart = str_find('{', _Tempstr, _Otherstart + 1);
						}

					if (_Leftbracketcount == _Rightbracketcount)
						{
						_Chunk += _Tempstr;
						break;
						}
					else
						{
						_Chunk += _Tempstr + '}';
						}
					}

				if (_Leftbracketcount == _Rightbracketcount)
					{
					Out_Dst.push_function(_Type + " " + _Identifier + " " + _Arguments + _Chunk + "}");
					}

				}
			}
		}// end read

	void
	read_uniform_only(const std::string& In_Shaderfile, glslshader_interpreter& Out_Dst)
		{
		std::string _Content;
		std::string _Tempstr;
		std::istringstream _Strin;

		read(In_Shaderfile, std::ref(_Content));
		_Strin.str(_Content);
		while (_Strin >> _Tempstr)
			{
			if (_Tempstr == clmagic::glslshader_interpreter::Keyword_Uniform)
				{
				std::getline(_Strin, _Tempstr, ';');
				Out_Dst.push_uniform(clmagic::glslshader_interpreter::Keyword_Uniform + _Tempstr);
				}
			else if (_Tempstr == clmagic::glslshader_interpreter::Keyword_Const)
				{
				std::getline(_Strin, _Tempstr, ';');
				Out_Dst.push_const(clmagic::glslshader_interpreter::Keyword_Const + _Tempstr);
				}
			else if (_Tempstr == clmagic::glslshader_interpreter::Keyword_Struct)
				{
				std::getline(_Strin, _Tempstr, '}');
				Out_Dst.push_struct(clmagic::glslshader_interpreter::Keyword_Struct + _Tempstr + "}");
				}
			}
		}

}// namespace clmagic