#pragma once
#ifndef __CLMAGIC_CORE_BASIC___SINGLETON___H__
#define __CLMAGIC_CORE_BASIC___SINGLETON___H__

namespace clmagic 
{
	/*
	@_stantard singleton:
	*/
	template<typename _Sig>
	class singleton 
	{
	public:
		static _Sig& instance()
			{
			static _Sig* _Instance = new _Sig();
			return (*_Instance);
			}
	};


	/*
	@_modifiable singleton:
		initial is nullptr!!!!

		singleton_modif::modify( new singleton_modif() );
	*/
	template<typename _Sig>
	class singleton_modif
	{
	public:
		static _Sig** instance_ptr() 
			{
			static _Sig* _Instance = nullptr;
			return (&_Instance);
			}

		static _Sig& instance()
			{
			return (**instance_ptr());
			}

		static void destroy_impl(_Sig** _Instanceptr)
			{
			_Sig*& _Rinsptr = *_Instanceptr;
			if (_Rinsptr != nullptr )
				{
				delete _Rinsptr;
				_Rinsptr = nullptr;
				}
			}

		static void modify_impl(_Sig* _Right)
			{
			_Sig** _Instanceptr = instance_ptr();
			destroy_impl( _Instanceptr );
			( *_Instanceptr ) = _Right;
			}
	};


	/*
	@_multi singletion
	*/
	template<typename _Sig>
	class singleton_multi 
	{
	public:
		static _Sig* new_instance()
			{
			return (new _Sig());
			}
	};

}// namespace clmagic 


#endif