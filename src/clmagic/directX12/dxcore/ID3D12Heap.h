#pragma once
#include <d3d12.h>
#include <wrl.h>// Windows 
#include <assert.h>
#include <vector>

#include "d3dx12.h"
#include "uncopyable.h"
#include "enum_string.h"
#include "ID3D12Resource.h"

namespace d3d12 {

	template<typename _Ti>
	_Ti ceil(_Ti _Val, _Ti _Bound) {
		const auto _Mask = (_Bound - 1);
		return (_Val + _Mask) & (~_Mask);
	}

	template<D3D12_HEAP_TYPE _Mytype>
	struct heap : public uncopyable {
		heap(ID3D12Device& _Device, const D3D12_HEAP_DESC& _Desc) {
			HRESULT hr = _Device.CreateHeap(&_Desc, IID_PPV_ARGS(_Myptr.GetAddressOf()));
			assert(SUCCEEDED(hr));
		}

		heap(ID3D12Device& _Device,
			UINT64           _Capacity, 
			D3D12_HEAP_FLAGS _Hflag     = D3D12_HEAP_FLAG_NONE, 
			UINT64           _Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT) 
		{
			D3D12_HEAP_DESC _Desc;
			_Desc.SizeInBytes = _Capacity;
			_Desc.Properties  = CD3DX12_HEAP_PROPERTIES(_Mytype);
			_Desc.Alignment   = _Alignment;
			_Desc.Flags       = _Hflag;
			HRESULT hr = _Device.CreateHeap(&_Desc, IID_PPV_ARGS(_Myptr.GetAddressOf()));
			assert( SUCCEEDED(hr) );
		}

		D3D12_HEAP_FLAGS flag() const {
			return _Myptr->GetDesc().Flags;
		}
		D3D12_HEAP_TYPE type() const {
			return _Mytype;
		}
		UINT64 alignment() const {// [bytes]
			return _Myptr->GetDesc().Alignment;
		}
		UINT64 capasity() const {// [bytes]
			return _Myptr->GetDesc().SizeInBytes;
		}
		UINT64 size() const {
			return _Mysize;
		}

		Microsoft::WRL::ComPtr<ID3D12Resource> new_resource(ID3D12Device& _Device, 
			const D3D12_RESOURCE_DESC&  _Rdesc, 
			      D3D12_RESOURCE_STATES _Rstate = D3D12_RESOURCE_STATE_COMMON,
			const D3D12_CLEAR_VALUE*    _Pclear = nullptr, 
			      UINT                  _Nodemask = 0) {		
			// 1. Get resource_allocation_info 
			D3D12_RESOURCE_ALLOCATION_INFO _Rinfo = _Device.GetResourceAllocationInfo(_Nodemask, 1, &_Rdesc);

			// 2. Check resource_allocation_info match _Mydesc
			auto _Mydesc      = _Myptr->GetDesc();
			auto _Mycapacity  = _Mydesc.SizeInBytes;
			auto _Myalignment = _Mydesc.Alignment;

			assert(_Rinfo.Alignment == _Myalignment);
			UINT64 _Heapoffset = d3d12::ceil(this->size(), _Myalignment);
			assert(_Heapoffset % _Myalignment == 0);// Error: if _Myalignment is not exponent of 2
			assert(_Heapoffset + _Rinfo.SizeInBytes <= _Mycapacity);

			// 3. Create
			Microsoft::WRL::ComPtr<ID3D12Resource> _Resource;
			_Device.CreatePlacedResource(_Myptr.Get(), _Heapoffset, &_Rdesc, _Rstate, _Pclear, IID_PPV_ARGS(_Resource.GetAddressOf()));

			return _Resource;
		}

		void swap(heap& _Right) {
			_Myptr.Swap(_Right._Myptr);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Heap> _Myptr;
		UINT64 _Mysize = 0;
	};

}// namespace dx12