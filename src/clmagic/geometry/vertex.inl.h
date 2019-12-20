#pragma once
namespace clmagic
{
	// <vertex_component1>
	template<typename _Ty1> inline
		vertex_component1<_Ty1>::vertex_component1(const _Ty1& _Attr1)
		: first(_Attr1)
		{
		}

	template<typename _Ty1> inline
		vertex_component1<_Ty1>::operator std::string() const
		{
		using std::to_string;

		std::string _Str = "vertex[\n";
		_Str += "first[" + to_string(first) + "]\n";
		_Str += "]";
		return (_Str);
		}

	template<typename _Ty1> inline
		vertex_descriptor
		vertex_component1<_Ty1>::descriptor()
		{
		constexpr attribute_descriptor _Desc1{ sizeof(_Ty1) / sizeof(variable32bit), BASE_TYPE::BS_FLOAT, false, sizeof(_Ty1), 0 };
		return (vertex_descriptor{ _Desc1 });
		}
	// </vertex_component1>


	// <vertex_component2>
	template<typename _Ty1, typename _Ty2> inline
		vertex_component2<_Ty1, _Ty2>::vertex_component2(const _Ty1& _Attr1, const _Ty2& _Attr2)
		: first(_Attr1), second(_Attr2)
		{
		}

	template<typename _Ty1, typename _Ty2> inline
		vertex_component2<_Ty1, _Ty2>::operator std::string() const
		{
		using std::to_string;

		std::string _Str = "vertex[\n";
		_Str += "first["  + to_string(first)  + "],\n";
		_Str += "second[" + to_string(second) + "]\n";
		_Str += "]";
		return (_Str);
		}

	template<typename _Ty1, typename _Ty2> inline
		vertex_descriptor 
		vertex_component2<_Ty1, _Ty2>::descriptor()
		{
		constexpr size_t _Stride = sizeof(_Ty1) + sizeof(_Ty2);
		constexpr attribute_descriptor _Desc1{ sizeof(_Ty1) / sizeof(variable32bit), BASE_TYPE::BS_FLOAT, false, _Stride, 0 };
		constexpr attribute_descriptor _Desc2{ sizeof(_Ty2) / sizeof(variable32bit), BASE_TYPE::BS_FLOAT, false, _Stride, sizeof(_Ty1) };
		return ( vertex_descriptor{ _Desc1, _Desc2 } );
		}
	// </vertex_component2>


	// <vertex_component3>
	template<typename _Ty1, typename _Ty2, typename _Ty3> inline
		vertex_component3<_Ty1, _Ty2, _Ty3>::vertex_component3(const _Ty1& _Attr1, const _Ty2& _Attr2, const _Ty3& _Attr3)
		: first(_Attr1), second(_Attr2), third(_Attr3)
		{
		}

	template<typename _Ty1, typename _Ty2, typename _Ty3> inline
		vertex_component3<_Ty1, _Ty2, _Ty3>::operator std::string() const
		{
		using std::to_string;

		std::string _Str = "vertex[\n";
		_Str += "first["  + to_string(first)  + "],\n";
		_Str += "second[" + to_string(second) + "],\n";
		_Str += "third["  + to_string(third)  + "]\n";
		_Str += "]";
		return (_Str);
		}

	template<typename _Ty1, typename _Ty2, typename _Ty3>
		vertex_descriptor
		vertex_component3<_Ty1, _Ty2, _Ty3>::descriptor()
		{
		constexpr size_t _Stride = sizeof(_Ty1) + sizeof(_Ty2) + sizeof(_Ty3);
		constexpr attribute_descriptor _Desc1{ sizeof(_Ty1) / sizeof(variable32bit), BASE_TYPE::BS_FLOAT, false, _Stride, 0 };
		constexpr attribute_descriptor _Desc2{ sizeof(_Ty2) / sizeof(variable32bit), BASE_TYPE::BS_FLOAT, false, _Stride, sizeof(_Ty1) };
		constexpr attribute_descriptor _Desc3{ sizeof(_Ty3) / sizeof(variable32bit), BASE_TYPE::BS_FLOAT, false, _Stride, sizeof(_Ty1) + sizeof(_Ty2) };
		return ( vertex_descriptor{ _Desc1, _Desc2, _Desc3 } );
		}
	// </vertex_component3>


	// <vertex_component4>
	template<typename _Ty1, typename _Ty2, typename _Ty3, typename _Ty4> inline
		vertex_component4<_Ty1, _Ty2, _Ty3, _Ty4>::vertex_component4(const _Ty1& _Attr1, const _Ty2& _Attr2, const _Ty3& _Attr3, const _Ty4& _Attr4)
		: first(_Attr1), second(_Attr2), third(_Attr3), fourth(_Attr4)
		{
		}

	template<typename _Attrib1, typename _Attrib2, typename _Attrib3, typename _Attrib4> inline
		vertex_component4<_Attrib1, _Attrib2, _Attrib3, _Attrib4>::operator std::string() const 
		{
		using std::to_string;

		std::string _Str = "vertex[\n";
		_Str += "first["  + to_string(first)  + "],\n";
		_Str += "second[" + to_string(second) + "],\n";
		_Str += "third["  + to_string(third)  + "],\n";
		_Str += "fourth[" + to_string(fourth) + "]\n";
		_Str += "]";
		return (_Str);
		}

	template<typename _Ty1, typename _Ty2, typename _Ty3, typename _Ty4>
		vertex_descriptor
		vertex_component4<_Ty1, _Ty2, _Ty3, _Ty4>::descriptor()
		{
		constexpr size_t _Stride = sizeof(_Ty1) + sizeof(_Ty2) + sizeof(_Ty3) + sizeof(_Ty4);
		constexpr attribute_descriptor _Desc1{ sizeof(_Ty1) / sizeof(variable32bit), BASE_TYPE::BS_FLOAT, false, _Stride, 0 };
		constexpr attribute_descriptor _Desc2{ sizeof(_Ty2) / sizeof(variable32bit), BASE_TYPE::BS_FLOAT, false, _Stride, sizeof(_Ty1) };
		constexpr attribute_descriptor _Desc3{ sizeof(_Ty3) / sizeof(variable32bit), BASE_TYPE::BS_FLOAT, false, _Stride, sizeof(_Ty1) + sizeof(_Ty2) };
		constexpr attribute_descriptor _Desc4{ sizeof(_Ty4) / sizeof(variable32bit), BASE_TYPE::BS_FLOAT, false, _Stride, sizeof(_Ty1) + sizeof(_Ty2) + sizeof(_Ty3) };
		return ( vertex_descriptor{ _Desc1, _Desc2, _Desc3, _Desc4 } );
		}
	// </vertex_component4>

}// namespace clmagic 