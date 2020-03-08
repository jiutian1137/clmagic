#include "glsl_shader.h"

using std::fstream;
using std::ifstream;
using std::string;
using std::vector;


namespace clmagic 
{
	//glslshader::glslshader()
	//	: visualshader(),
	//	  _Myprog(-1),
	//	  m_TextureCount(0),
	//	  m_MaxTextureCount(8)
	//	{
	//	// (do nothing)
	//	}
	//
	//glslshader::~glslshader()
	//	{
	//	graphics_interface_bridge::instance().destroy_program(_Myprog);
	//	}

	//
	////----------------------------------------------------------------------------------------
	//bool glslshader::operator<(const visualshader& _Right) const
	//	{
	//	return ( _Myid < _Right.id() );
	//	}

	//bool glslshader::operator>(const visualshader& _Right) const
	//	{
	//	return ( _Myid > _Right.id() );
	//	}

	//bool glslshader::operator==(const visualshader& _Right) const
	//	{
	//	return ( _Myid == _Right.id() );
	//	}

	//bool glslshader::operator!=(const visualshader& _Right) const
	//	{
	//	return ( _Myid != _Right.id() );
	//	}

	//void glslshader::init(std::initializer_list<shaderinfo> _Slinfos)
	//	{
	//	for (auto _Ite = _Slinfos.begin(); _Ite != _Slinfos.end(); ++_Ite)
	//		{
	//		switch (_Ite->type)
	//			{
	//			case shadertypes::VertexShader: _Myvsh = *_Ite; break;
	//			case shadertypes::FragmentShader: _Myfsh = *_Ite; break;
	//			default: break;
	//			}
	//		}
	//	assert(!_Myvsh.filename.empty() || !_Myfsh.filename.empty());

	//	// 2. create script and read script
	//	/*ScriptOfShader shaderScript;
	//	shaderScript.ReadScript(_Scriptfilename);*/

	//	
	//	std::vector<shd_info> _Shaderinfos;
	//	_Shaderinfos.push_back(shd_info{ GL_VERTEX_SHADER, _Myvsh.filename.c_str(), static_cast<unsigned int>(-1) });
	//	_Shaderinfos.push_back(shd_info{ GL_FRAGMENT_SHADER, _Myfsh.filename.c_str(), static_cast<unsigned int>(-1) });
	//	_Myprog = loadShaders(_Shaderinfos, true);
	//	checkFromLinkError(_Myprog, "shader");

	//	this->_Frush_uniform_locations({ _Myvsh.filename, _Myfsh.filename });
	//	this->_Correction_sampler();
	//	//_Myuniforms = shaderScript.ToUniformList();
	//	//m_MaxTextureCount = shaderScript.getMaxTextureCount();
	//	}

	//void glslshader::use()
	//	{
	//	glUseProgram(_Myprog);
	//	auto _Pdata = _Myudata.data();
	//	auto _Itedesc = _Myulist.begin();
	//	while (_Itedesc != _Myulist.end()) 
	//		{
	//		if (_Itedesc->second.is_update)
	//			{
	//			switch (_Itedesc->second.type)
	//				{
	//				case UNIFORM_TYPE::UF_VEC4F: glUniform4fv(_Itedesc->second.location, 1, (const float*)(_Pdata + _Itedesc->second.index)); break;
	//				case UNIFORM_TYPE::UF_mat2: glUniformmatrix2fv(_Itedesc->second.location, 1, false, (const float*)(_Pdata + _Itedesc->second.index)); break;
	//				case UNIFORM_TYPE::UF_mat4: glUniformmatrix4fv(_Itedesc->second.location, 1, false, (const float*)(_Pdata + _Itedesc->second.index)); break;
	//				case UNIFORM_TYPE::UF_mat3X4: glUniformmatrix3x4fv(_Itedesc->second.location, 1, false, (const float*)(_Pdata + _Itedesc->second.index)); break;
	//				case UNIFORM_TYPE::UF_SAMPLER2D: break;
	//				default:
	//					break;
	//				}
	//			_Itedesc->second.is_update = false;
	//			}
	//		++_Itedesc;
	//		}
	//	}

	//void glslshader::set_uniform_data(const std::string& _Identifier, void* _Data, size_t _Size)
	//	{
	//	auto _Where = _Myulist.find(_Identifier);
	//	if (_Where != _Myulist.end())
	//		{
	//		memcpy(_Myudata.data() + _Where->second.index, _Data, _Size);
	//		_Where->second.is_update = true;
	//		}
	//	}

	//void glslshader::set_uniform_data(const std::string& _Identifier, const vec3& _Newdata)
	//	{	// _Myudata[_Identifier] = vec4(_Newdata, 0.0), 1 * sizeof(vec4)
	//	set_uniform_data(_Identifier, const_cast<vec3*>(&_Newdata), sizeof(_Newdata));
	//	}

	//void glslshader::set_uniform_data(const std::string& _Identifier, const vec4& _Newdata)
	//	{	// _Myudata[_Identifier] = _Newdata
	//	set_uniform_data(_Identifier, const_cast<vec4*>(&_Newdata), sizeof(_Newdata));
	//	}

	//void glslshader::set_uniform_data(const std::string& _Identifier, const mat2& _Newdata)
	//	{	// _Myudata[_Identifier] = (void*)_Newdata, 1 * sizeof(vec4)
	//	set_uniform_data(_Identifier, const_cast<mat2*>(&_Newdata), sizeof(_Newdata));
	//	}

	//void glslshader::set_uniform_data(const std::string& _Identifier, const mat4& _Newdata)
	//	{	// _Myudata[_Identifier] = (void*)_Newdata, 4 * sizeof(vec4)
	//	set_uniform_data(_Identifier, const_cast<mat4*>(&_Newdata), sizeof(_Newdata));
	//	}

	//void glslshader::set_uniform_data(const std::string& _Identifier, const mat_<3, 4, float>& _mat3x4)
	//	{
	//	set_uniform_data(_Identifier, const_cast< mat_<3, 4, float>* >(&_mat3x4), sizeof(_mat3x4) );
	//	}

	//void glslshader::set_uniform_data(const uniform& _Uniform)
	//	{
	//	auto _Where = _Myulist.find(_Uniform.name);
	//	if (_Where != _Myulist.end())
	//		{
	//		memcpy( _Myudata.data() + _Where->second.index, _Uniform.data, graphics_interface.size_of(_Uniform.type) );
	//		_Where->second.is_update = true;
	//		}
	//	}

	//void glslshader::set_uniform_data(const visualshader& _Sender)
	//	{	// update _Mydata by _Sender
	//	for (auto _Uniform : _Sender.my_ulist()) 
	//		{
	//		if (_Uniform.second.type == UNIFORM_TYPE::UF_SAMPLER2D) 
	//			{	// sampler not have uniform data
	//			continue;
	//			}
	//		this->set_uniform_data( _Uniform.first, (void*)(&_Sender.my_udata()[_Uniform.second.index]), _Uniform.second.step * sizeof(vec4) );
	//		}
	//	}
	


	//void glslshader::_Load_content_from_file(std::vector<std::string>& _Destination, const std::string& _Filename)
	//	{
	//	std::string _Tempstr;
	//	std::ifstream _Instream;
	//	
	//	_Instream.open(_Filename, std::ios::in);
	//	while (std::getline(_Instream, _Tempstr, '\n'))
	//		{
	//		auto _Index = _Tempstr.find("//");
	//		if (_Index != std::string::npos)
	//			{
	//			_Destination.push_back(_Tempstr.substr(0, _Index));
	//			}
	//		else
	//			{
	//			_Destination.push_back(_Tempstr);
	//			}
	//		}
	//	_Instream.close();
	//	}

	//void glslshader::_Frush_uniform_locations(const std::vector<std::string>& _Shaderfiles)
	//	{
	//	if (_Shaderfiles.empty())
	//		{
	//		return;
	//		}

	//	if (!_Myulist.empty()) 
	//		{
	//		_Myulist.clear();
	//		_Myudata.clear();
	//		}
	//	
	//	graphics_interface_bridge& _Ginterface = graphics_interface_bridge::instance();
	//	std::map<glslshader_interpreter::variant, int> _Templist;
	//	glslshader_interpreter _Shaderinterpreter;
	//	std::map<std::string, uniform_descriptor> _Olddesc = _Myulist;
	//	std::vector<vec4> _Olddata = _Myudata;
	//	_Myulist.clear();
	//	_Myudata.clear();

	//	for (const std::string& _Filename : _Shaderfiles) 
	//		{
	//		if (_Filename.empty())
	//			{	// filename is error
	//			continue;
	//			}

	//		read_uniform_only(_Filename, _Shaderinterpreter);
	//		_Shaderinterpreter.check_struct();
	//		_Shaderinterpreter.check_uniform();
	//		_Templist = _Shaderinterpreter.get_uniform_list();

	//		for (auto _Present = _Templist.begin(); _Present != _Templist.end(); ++_Present)
	//			{
	//			// 1. create uniform_descriptor
	//			uniform_descriptor _Udesc;
	//			// 2. type
	//			_Udesc.type = _Ginterface.convert_uniform_type(_Present->first.first);
	//			if (_Udesc.type == UNIFORM_TYPE::UF_UNKNOWN)
	//				continue;
	//			// 3. name
	//			_Udesc.name = _Present->first.second;
	//			// 4. location
	//			_Udesc.location = _Ginterface.find_uniform_location(_Myprog, _Udesc.name);
	//			// 5. step
	//			_Udesc.step = 1;
	//			switch (_Udesc.type)
	//				{
	//				case UNIFORM_TYPE::UF_VEC4F: _Udesc.step = 1; break;
	//				case UNIFORM_TYPE::UF_mat2: _Udesc.step = 1; break;
	//				case UNIFORM_TYPE::UF_mat3X4: _Udesc.step = 3; break;
	//				case UNIFORM_TYPE::UF_mat4: _Udesc.step = 4; break;
	//				case UNIFORM_TYPE::UF_SAMPLER2D: _Udesc.step = 0; break;
	//				default: break;
	//				}
	//			// 6. state
	//			_Udesc.is_update = true;

	//			// 7. data index
	//			_Udesc.index = _Myudata.size();

	//			// 8. push desc, data
	//			_Myulist.insert_or_assign(_Udesc.name, _Udesc);
	//			for (int i = 0; i != _Udesc.step; ++i) 
	//				{
	//				_Myudata.push_back(vec4());
	//				}
	//			}

	//		_Templist.clear();
	//		_Shaderinterpreter.clear();
	//		}

	//	// restore data
	//	if (!_Olddesc.empty() && !_Olddata.empty()) 
	//		{
	//		auto _Srcdesc = _Olddesc.begin();
	//		while (_Srcdesc != _Olddesc.end()) 
	//			{
	//			auto _Dstdesc = _Myulist.find(_Srcdesc->second.name);

	//			if (_Dstdesc != _Myulist.end() && 
	//				_Dstdesc->second.type == _Srcdesc->second.type)
	//				{	// data descriptor is same
	//				memcpy(_Myudata.data() + _Dstdesc->second.index,
	//					   _Olddata.data() + _Srcdesc->second.index,
	//					   _Srcdesc->second.step * sizeof(vec4)
	//					   );
	//				}

	//			++_Srcdesc;
	//			}
	//		}
	//	}

	//void glslshader::_Set_shader_file_variable(const std::string& _Newval, const std::string& _Filename, const std::vector<std::string>& _Condition, bool _Iseach)
	//	{
	//	if (_Condition.size() == 0)
	//		{
	//		return;
	//		}
	//	for (const std::string& str : _Condition)
	//		{
	//		if ( str.size() > 9999 )
	//			return;
	//		}

	//	std::fstream stream;
	//	std::vector< std::string > fileContent;

	//	// 1. file >> string
	//	std::string strTemp;
	//	stream.open( _Filename, std::ios::in );
	//	while ( getline( stream, strTemp, '\n' ) )
	//		{
	//		fileContent.push_back( strTemp );
	//		}
	//	stream.close( );

	//	// 2. modify variable in fileContent
	//	for ( std::string& str : fileContent )
	//		{
	//		size_t idx;
	//		for ( size_t i = 0; i != _Condition.size(); ++i )
	//			{
	//			idx = str.find( _Condition[i] );
	//			if ( idx == std::string::npos )
	//				{
	//				break;
	//				}
	//			}

	//		if ( idx != std::string::npos )
	//			{	// machie
	//			str = str.substr( 0, idx + _Condition.back().size() ) + " = ";
	//			str += _Newval;
	//			str += ";";
	//			if ( !_Iseach )
	//				{
	//				break;
	//				}
	//			}
	//		}

	//	// 3. string >> file
	//	stream.open( _Filename, std::ios::out );
	//	for ( const std::string& str : fileContent )
	//		{
	//		stream << str << '\n';
	//		}
	//	stream.close( );
	//	}

	//void glslshader::_Set_max_light_count(int _Newval, int& _Currentval, unsigned int& _Prog, shd_info& _Fragshader)
	//	{
	//	// 1. modify file content
	//	_Set_shader_file_variable(
	//		std::to_string( _Newval ),
	//		_Fragshader.filename,
	//		std::vector< std::string >{ KeyWord_Const, KeyWord_Int, UNIFORM_MAXLIGHT_COUNT },
	//		false);

	//	// 2. reload shader
	//	shd_info _Fsh{ GL_FRAGMENT_SHADER, _Myfsh.filename.c_str(), GLuint(_Myfsh.index) };
	//	relinkShaders( _Prog, _Fsh);

	//	// 3.
	//	this->_Frush_uniform_locations({ _Myvsh.filename, _Myfsh.filename });

	//	// 4.
	//	_Currentval = _Newval;
	//	}

	//void glslshader::_Set_max_texture_count(int _Newval, int& _Currentval, unsigned int& _Prog, shd_info& _Fragshader)
	//	{
	//	// 1.
	//	_Set_shader_file_variable(
	//		std::to_string(_Newval),
	//		_Fragshader.filename,
	//		std::vector<std::string>{ KeyWord_Const, KeyWord_Int, UNIFORM_MAXTEXTURE_COUNT },
	//		false);

	//	// 2. reload shader
	//	shd_info _Fsh{ GL_FRAGMENT_SHADER, _Myfsh.filename.c_str(), GLuint(_Myfsh.index) };
	//	relinkShaders(_Prog, _Fsh);

	//	// 3.
	//	this->_Frush_uniform_locations({ _Myvsh.filename, _Myfsh.filename });

	//	// 4. end
	//	_Currentval = _Newval;
	//	}

	//void glslshader::_Correction_sampler()
	//	{
	//	glUseProgram(_Myprog);
	//	for (int i = 0; i != m_MaxTextureCount; ++i) 
	//		{
	//		auto _Where = _Myulist.find(visualshader::UNIFORM_TEXTURES_(i));
	//		if (_Where != _Myulist.end())
	//			glUniform1i(_Where->second.location, i);
	//		}
	//	glUseProgram(0);
	//	}

}// namespace clmagic