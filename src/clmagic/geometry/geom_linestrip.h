#pragma once
#ifndef _CLMAGIC_CORE_GEOMETRY_STRIPLINE_H_
#define _CLMAGIC_CORE_GEOMETRY_STRIPLINE_H_
#include "math.h"
#include "vertex.h"
#include <queue>


namespace clmagic
{
	//using _Tyvt = vertex_pos3_weight;
	template<typename _Tyvt>
	class linestrip
	{
	public:
		using value_type = _Tyvt;
		using pointer = _Tyvt * ;
		using const_pointer = const _Tyvt *;
		using reference = _Tyvt & ;
		using const_reference = const _Tyvt&;
		using iterator = typename std::deque<_Tyvt>::iterator;
		using const_iterator = typename std::deque<_Tyvt>::const_iterator;

		explicit operator std::vector<_Tyvt>() const 
			{
			return ( std::vector<_Tyvt>(_Mydata.begin(), _Mydata.end()) );
			}

		explicit operator std::string() const 
			{
			std::string _Str = "linestrip[\n";
			for (const auto& _Vt : _Mydata) 
				{
				_Str += static_cast<std::string>(_Vt);
				_Str += "\n";
				}

			_Str += "]";

			return (_Str);
			}

		reference operator[] (size_t _Idx)
			{
			return (_Mydata[_Idx]);
			}

		const_reference operator[] (size_t _Idx) const
			{
			return (_Mydata[_Idx]);
			}

		void weld(float _Threshold)
			{	// if _Real == _Front, delete _Real
			auto _Front = _Mydata.begin();
			auto _Real = _Mydata.begin();
			++_Real;

			while (_Real != _Mydata.end())
				{
				if (equals(_Real->first, _Front->first, _Threshold)) 
					{	// _Real == _Front
					_Real = _Mydata.erase(_Real);
					_Front = _Real - 1;
					}
				else 
					{
					_Front = _Real;
					++_Real;
					}
				}
			}

		void push_back(const _Tyvt& _Point) 
			{
			_Mydata.emplace_back(_Point);
			}

		void push_back(std::initializer_list<_Tyvt> _Points)
			{
			for (auto _First = _Points.begin(); _First != _Points.end(); ++_First) 
				{
				_Mydata.emplace_back(*_First);
				}
			}

		void push_front(const _Tyvt& _Point) 
			{
			_Mydata.emplace_front(_Point);
			}

		void push_front(std::initializer_list<_Tyvt> _Points)
			{
			for (auto _First = _Points.begin(); _First != _Points.end(); ++_First)
				{
				_Mydata.emplace_front(*_First);
				}
			}

		void erase(const_iterator _Where) 
			{
			_Mydata.erase(_Where);
			}

		template<typename _Iter>
		void insert(const_iterator _Where, _Iter _First, _Iter _Last) 
			{
			_Mydata.insert(_Where, _First, _Last);
			}

		void insert(const_iterator _Where, const _Tyvt& _Point) 
			{
			_Mydata.insert(_Where, _Point);
			}

		void insert(const_iterator _Where, std::initializer_list<_Tyvt> _Points) 
			{
			_Mydata.insert(_Where, _Points);
			}

		_Tyvt& front() 
			{
			return ( _Mydata.front() );
			}

		const _Tyvt& front() const
			{
			return (_Mydata.front());
			}

		_Tyvt& back()
			{
			return (_Mydata.back());
			}

		const _Tyvt& back() const
			{
			return (_Mydata.back());
			}

		iterator begin() 
			{
			return (_Mydata.begin());
			}

		const_iterator begin() const
			{
			return (_Mydata.begin());
			}

		iterator end()
			{
			return (_Mydata.end());
			}

		const_iterator end() const
			{
			return (_Mydata.end());
			}

		bool empty() const 
			{
			return ( _Mydata.empty() );
			}

		size_t size() const
			{
			return (_Mydata.size());
			}

		std::deque<_Tyvt>& _Get_data()
			{
			return (_Mydata);
			}

		const std::deque<_Tyvt>& _Get_data() const
			{
			return (_Mydata);
			}

	protected:
		std::deque<_Tyvt> _Mydata;
	};

	
	/*
	effect: vertex.weight /= max_weight;
	*/
	void normalize_weight(linestrip<vertex_pos3_weight>& _Spline);

	
	/*
	generate helical strip line
	*/
	void helical_line(std::vector<Vec3>& _Dst, const Vec3& _Start, const Vec3& _End, 
		const std::vector<std::pair<float, float>>& _Widths = std::vector<std::pair<float, float>>(),
		const std::vector<std::pair<float, float>>& _Tsteps = std::vector<std::pair<float, float>>(),
		const std::vector<std::pair<float, float>>& _Rotsteps = std::vector<std::pair<float, float>>()
		);

}// namespace clmagic


#endif