#pragma once
#ifndef __CLMAGIC_GEOMETRY_VARIABLE_H__
#define __CLMAGIC_GEOMETRY_VARIABLE_H__

namespace clmagic 
{
	enum class BASE_TYPE
	{
		BS_UNKNOWN,
		BS_BOOL, 
		BS_BYTE,
		BS_UBYTE,
		BS_INT32,
		BS_UINT32,
		BS_FLOAT,
		BS_DOUBLE
	};

	
	struct variable32bit 
	{
		union
		{
			bool		  m_AsBool;
			char		  m_AsByte;
			unsigned char m_AsUbyte;
			int			  m_AsInt;
			unsigned int  m_AsUint;
			float		  m_AsFloat;
		};
		variable32bit() : m_AsInt(0) { }
		variable32bit(bool __________Bool__) : m_AsBool(__________Bool__) { }
		variable32bit(char __________Byte__) : m_AsByte(__________Byte__) { }
		variable32bit(unsigned char _UByte_) : m_AsUbyte(_UByte_) { }
		variable32bit(int ___________Int___) : m_AsInt(___________Int___) { }
		variable32bit(unsigned int __Uint__) : m_AsUint(__Uint__) { }
		variable32bit(float _________Float_) : m_AsFloat(_________Float_) { }
	};


	struct variable32bit2
	{
		union
		{
			bool		  m_AsBool;
			char		  m_AsByte;
			unsigned char m_AsUbyte;
			int			  m_AsInt;
			unsigned int  m_AsUint;
			float		  m_AsFloat;
		};
		BASE_TYPE m_Type;

		variable32bit2() : m_AsInt(0), m_Type(BASE_TYPE::BS_INT32) { }
		variable32bit2(bool __________Bool__) : m_AsBool(__________Bool__), m_Type(BASE_TYPE::BS_BOOL) { }
		variable32bit2(char __________Byte__) : m_AsByte(__________Byte__), m_Type(BASE_TYPE::BS_BYTE) { }
		variable32bit2(unsigned char _Ubyte_) : m_AsUbyte(_Ubyte_),		   m_Type(BASE_TYPE::BS_UBYTE) { }
		variable32bit2(int ___________Int___) : m_AsInt(___________Int___), m_Type(BASE_TYPE::BS_INT32) { }
		variable32bit2(unsigned int __Uint__) : m_AsUint(__Uint__),		   m_Type(BASE_TYPE::BS_UINT32) { }
		variable32bit2(float _________Float_) : m_AsFloat(_________Float_), m_Type(BASE_TYPE::BS_FLOAT) { }
	};	// class Varient 


	struct variable128bit 
	{
		variable32bit _Var[4];

		template<typename _Ty>
		void set_val(size_t _Index, BASE_TYPE _Type, _Ty _Val) 
			{
			switch (_Type)
				{
				case clmagic::BASE_TYPE::BS_BOOL:  _Var[_Index].m_AsBool = _Val; break;
				case clmagic::BASE_TYPE::BS_BYTE:  _Var[_Index].m_AsByte = _Val; break;
				case clmagic::BASE_TYPE::BS_UBYTE: _Var[_Index].m_AsUbyte = _Val; break;
				case clmagic::BASE_TYPE::BS_INT32:   _Var[_Index].m_AsInt = _Val; break;
				case clmagic::BASE_TYPE::BS_UINT32:  _Var[_Index].m_AsUint = _Val; break;
				case clmagic::BASE_TYPE::BS_FLOAT: _Var[_Index].m_AsFloat = _Val; break;
				case clmagic::BASE_TYPE::BS_DOUBLE: _Var[_Index].m_AsFloat = static_cast<float>(_Val); break;
				default: break;
				}
			}

		const variable32bit& get_val(size_t _Index) const
			{
			return (_Var[_Index]);
			}

		variable32bit& get_val(size_t _Index)
			{
			return (_Var[_Index]);
			}
	};

}// namespace clmagic 


#endif