#pragma once
#ifndef __CLMAGIC_CORE_BASIC___SEQUENCE___H__
#define __CLMAGIC_CORE_BASIC___SEQUENCE___H__
#include "singleton.h"
#include <map>
#include <string>


namespace clmagic 
{
#ifdef _WIN64
	using sequ_t = long long;
#else
	using sequ_t = int;
#endif

	class sequence : public singleton<sequence>
	{
	public:

		/*
		@_sequences_engine:
			three method
				1. get next value(int)
				2. get next value(std::string)
				3. clear increment
		*/
		class engine 
		{
		public:
			sequ_t _Myinit;
			sequ_t _Myincrement;
			sequ_t _Myseq;

			engine(decltype(_Myinit) _Initialval = 1, decltype(_Myincrement) _Increment = 1);
			operator decltype(_Myseq)();

			decltype(_Myseq) nextval();
			std::string nextval_str();

			void clear();
			void reset(decltype(_Myinit) _Initialval, decltype(_Myincrement) _Increment);
		};

		static engine& sys_engine(const std::string& _Seqname);

		sequence();

		engine& operator[](const std::string& _Seqname);

		bool insert_assign(const std::string& _Seqname, decltype(engine::_Myinit) _Initialval = 1, decltype(engine::_Myincrement) _Increment = 1);

		void erase(const std::string& _Seqname);

		std::map<std::string, engine>::iterator find(const std::string& _Seqname);
		std::map<std::string, engine>::const_iterator find(const std::string& _Seqname) const;

		const std::string& my_log() const;

	private:
		std::map<std::string, engine> _Myengine;
		std::string _Mylog;
	};


	// <sequences>

	// <engine>

	inline sequence::engine::engine(decltype(_Myinit) _Initialval, decltype(_Myincrement) _Increment)
		: _Myinit(_Initialval), _Myincrement(_Increment), _Myseq(_Initialval)
		{
		if (_Myincrement == 0)
			{	// check increment
			_Myincrement = 1;
			}
		}

	inline sequence::engine::operator decltype(sequence::engine::_Myseq)()
		{
		return (nextval());
		}

	inline decltype(sequence::engine::_Myseq)
		sequence::engine::nextval()
		{
		size_t _Result = _Myseq;
		_Myseq = _Myseq + _Myincrement;
		return (_Result);
		}

	inline std::string
		sequence::engine::nextval_str()
		{
		return std::to_string(this->nextval());
		}

	inline void
		sequence::engine::clear()
		{
		_Myseq = _Myinit;
		}

	inline void
		sequence::engine::reset(decltype(_Myinit) _Initialval, decltype(_Myincrement) _Increment)
		{
		_Myinit = _Initialval;
		_Myincrement = _Increment;
		_Myseq = _Initialval;
		if (_Myincrement == 0)
			{	// check increment
			_Myincrement = 1;
			}
		}

	// </engine>

	// <static>
	inline sequence::engine& 
		sequence::sys_engine(const std::string& _Seqname)
		{
		auto& _Engine = sequence::instance()[_Seqname];
		return _Engine;
		}
	// </static>

	inline sequence::sequence() : _Myengine()
		{	// construct empty sequences
		}

	inline sequence::engine&
		sequence::operator[](const std::string& _Seqname)
		{
		return (_Myengine[_Seqname]);
		}

	inline bool
		sequence::insert_assign(const std::string& _Seqname, decltype(engine::_Myinit) _Initialval, decltype(engine::_Myincrement) _Increment)
		{
		if (_Myengine.find(_Seqname) == _Myengine.end())
			{
			_Myengine.insert_or_assign(_Seqname, engine(_Initialval, _Increment));
			return (true);
			}
		else
			{
			_Mylog = "error[exist engine] in[" + _Seqname + "]";
			return (false);
			}
		}

	inline void
		sequence::erase(const std::string& _Seqname)
		{
		_Myengine.erase(_Seqname);
		}

	inline std::map<std::string, sequence::engine>::iterator
		sequence::find(const std::string& _Seqname)
		{
		return (_Myengine.find(_Seqname));
		}

	inline std::map<std::string, sequence::engine>::const_iterator
		sequence::find(const std::string& _Seqname) const 
		{
		return (_Myengine.find(_Seqname));
		}

	inline const std::string&
		sequence::my_log() const
		{
		return (_Mylog);
		}

	// </sequences>

}// namespace clmagic 

#endif