#pragma once
#ifndef __CLMAGIC_CORE_BASIC___FUNCTION___H__
#define __CLMAGIC_CORE_BASIC___FUNCTION___H__

namespace clmagic 
{
	template<class _Result, class _Ty, class _Arg>
	class mem_fun_1st 
	{
	public:
		using return_type = _Result;
		using type = _Ty;
		using arg_type = _Arg;

		mem_fun_1st()
			: _Pmemfun(nullptr)
			{
			}

		explicit mem_fun_1st(_Result(_Ty::*_Pm)(_Arg))
			: _Pmemfun(_Pm)
			{
			}

		inline _Result operator()(_Ty* _Pleft, _Arg _Right) const
			{
			return ((_Pleft->*_Pmemfun)(_Right));
			}

		inline operator bool() const
			{
			return (!!_Pmemfun);
			}

	private:
		_Result(_Ty::*_Pmemfun)(_Arg);
	};


	template<class _Result, class _Ty, class _Arg0, class _Arg1>
	class mem_fun_2nd
	{
	public:
		using return_type = _Result;
		using type = _Ty;
		using arg0_type = _Arg0;
		using arg1_type = _Arg1;

		mem_fun_2nd()
			: _Pmemfun(nullptr)
			{
			}

		explicit mem_fun_2nd(_Result(_Ty::*_Pm)(_Arg0, _Arg1))
			: _Pmemfun(_Pm)
			{
			}

		inline _Result operator()(_Ty* _Pleft, _Arg0 _Right0, _Arg1 _Right1) const
			{
			return ((_Pleft->*_Pmemfun)(_Right0, _Right1));
			}

		inline operator bool() const
			{
			return (!!_Pmemfun);
			}

	private:
		_Result(_Ty::*_Pmemfun)(_Arg0, _Arg1);
	};


	template<class _Result, class _Ty, class _Arg0, class _Arg1, class _Arg2>
	class mem_fun_3th
	{
	public:
		using return_type = _Result;
		using type = _Ty;
		using arg0_type = _Arg0;
		using arg1_type = _Arg1;
		using arg2_type = _Arg2;

		mem_fun_3th()
			: _Pmemfun(nullptr)
			{
			}

		explicit mem_fun_3th(_Result(_Ty::*_Pm)(_Arg0, _Arg1, _Arg2))
			: _Pmemfun(_Pm)
			{
			}

		inline _Result operator()(_Ty* _Pleft, _Arg0 _Right0, _Arg1 _Right1, _Arg2 _Right2) const
			{
			return ((_Pleft->*_Pmemfun)(_Right0, _Right1, _Right2));
			}

		inline operator bool() const
			{
			return (!!_Pmemfun);
			}

	private:
		_Result(_Ty::*_Pmemfun)(_Arg0, _Arg1, _Arg2);
	};


	template<class _Result, class _Ty, class _Arg>
	mem_fun_1st<_Result, _Ty, _Arg> mem_func(_Result(_Ty::*_Pm)(_Arg))
		{
		return mem_fun_1st<_Result, _Ty, _Arg>(_Pm);
		}

	template<class _Result, class _Ty, class _Arg0, class _Arg1>
	mem_fun_2nd<_Result, _Ty, _Arg0, _Arg1> mem_func(_Result(_Ty::*_Pm)(_Arg0, _Arg1))
		{
		return mem_fun_2nd<_Result, _Ty, _Arg0, _Arg1>(_Pm);
		}

	template<class _Result, class _Ty, class _Arg0, class _Arg1, class _Arg2>
	mem_fun_3th< _Result, _Ty, _Arg0, _Arg1, _Arg2> mem_func(_Result(_Ty::*_Pm)(_Arg0, _Arg1, _Arg2))
		{
		return mem_fun_3th<_Result, _Ty, _Arg0, _Arg1, _Arg2>(_Pm);
		}


	template<class _Fn>
	class signal_1st
	{
	public:
		using func_type = _Fn;
		using return_type = typename func_type::return_type;
		using type = typename func_type::type;
		using arg_type = typename func_type::arg_type;

		signal_1st()
			: _Myfun(), _Myreceiver(nullptr)
			{
			}

		return_type operator()(arg_type _Arg0) const
			{
			return (_Myfun(_Myreceiver, _Arg0));
			}

		bool has_receiver() const
			{
			return (!!_Myreceiver);
			}

		_Fn _Myfun;
		type* _Myreceiver;
	};


	template<class _Fn>
	class signal_2nd
	{
	public:
		using func_type = _Fn;
		using return_type = typename func_type::return_type;
		using type = typename func_type::type;
		using arg0_type = typename func_type::arg0_type;
		using arg1_type = typename func_type::arg1_type;

		signal_2nd()
			: _Myfun(), _Myreceiver(nullptr)
			{
			}

		return_type operator()(arg0_type _Arg0, arg1_type _Arg1) const
			{
			return (_Myfun(_Myreceiver, _Arg0, _Arg1));
			}

		bool has_receiver() const
			{
			return (!!_Myreceiver);
			}

		_Fn _Myfun;
		type* _Myreceiver;
	};


	template<class _Fn>
	class signal_3th
	{
	public:
		using func_type = _Fn;
		using return_type = typename func_type::return_type;
		using type = typename func_type::type;
		using arg0_type = typename func_type::arg0_type;
		using arg1_type = typename func_type::arg1_type;
		using arg2_type = typename func_type::arg2_type;

		signal_3th()
			: _Myfun(), _Myreceiver(nullptr)
			{
			}

		return_type operator()(arg0_type _Arg0, arg1_type _Arg1, arg2_type _Arg2) const
			{
			return (_Myfun(_Myreceiver, _Arg0, _Arg1, _Arg2));
			}

		bool has_receiver() const
			{
			return (!!_Myreceiver);
			}

		_Fn _Myfun;
		type* _Myreceiver;
	};


#define CLMSIGNAL(SENDER, SIGNAL) \
	(SENDER), (SENDER).SIGNAL

#define CLMSLOT(RECEIVER, SLOT) \
	(RECEIVER), clmagic::mem_func(SLOT)

	template<class _Snd, class _Sig, class _Recv, class _Slo>
	void connect(_Snd& _Sender, _Sig& _Signal, _Recv& _Receiver, const _Slo& _Slot)
		{
		_Signal._Myfun = _Slot;
		_Signal._Myreceiver = &_Receiver;
		}

}// namespace clmagic


#endif