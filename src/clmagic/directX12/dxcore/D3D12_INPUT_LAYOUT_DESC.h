#pragma once
#include <d3d12.h>
#include <vector>

namespace d3d12 {

	class _Vector_input_element_desc : public std::vector<D3D12_INPUT_ELEMENT_DESC> {
		using _Mybase = std::vector<D3D12_INPUT_ELEMENT_DESC>;

	public:
		void push_back(const D3D12_INPUT_ELEMENT_DESC& _Desc) {
			_Mybase::push_back(_Desc);
		}

		void push_back(D3D12_INPUT_ELEMENT_DESC&& _Desc) {
			_Mybase::push_back(std::move(_Desc));
		}

		void push_back(
			LPCSTR      _Semantic_name,
			UINT        _Semantic_index,
			DXGI_FORMAT _Format,
			UINT        _Input_slot/*[0, 15]*/,
			UINT        _Aligned_byte_offset,
			D3D12_INPUT_CLASSIFICATION _Input_slot_class,
			UINT _Instance_data_step_rate = 0) {
			_Mybase::push_back(
				D3D12_INPUT_ELEMENT_DESC{
					_Semantic_name,
					_Semantic_index,
					_Format,
					_Input_slot,
					_Aligned_byte_offset,
					_Input_slot_class,
					_Instance_data_step_rate });
		}

		D3D12_INPUT_LAYOUT_DESC get() const {
			return D3D12_INPUT_LAYOUT_DESC{ _Mybase::data(), static_cast<UINT>(_Mybase::size()) };
		}
	};

}// namespace dx12