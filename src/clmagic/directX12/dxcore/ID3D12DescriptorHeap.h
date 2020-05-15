#pragma once
#include <d3d12.h>
#include <wrl.h>// Windows 
#include <assert.h>

#include "uncopyable.h"
#include "enum_string.h"

namespace d3d12 {

	// I not like ID3D12DescriptorHeap
	struct _Descriptorheap_impl : public uncopyable {
		/*
		@_ID3D12DescriptorHeap
		in CPU:
		   D3D12_CONSTANT_BUFFER_VIEW_DESC 0
		   D3D12_CONSTANT_BUFFER_VIEW_DESC 1
		   D3D12_CONSTANT_BUFFER_VIEW_DESC 2
		   D3D12_CONSTANT_BUFFER_VIEW_DESC 3
		   ...
		   ...
		   D3D12_CONSTANT_BUFFER_VIEW_DESC N
		@_GetGPUVirtualHandle():
			Get GPU-handle by D3D12_CONSTANT_BUFFER_VIEW_DESC in descriptor_heap, D3D12_CONSTANT_BUFFER_VIEW_DESC at CPU-memory
		*/

		_Descriptorheap_impl() = default;
		_Descriptorheap_impl(_Descriptorheap_impl&& _Right) {
			_Right.swap(*this);
			_Right.release();
		}
		_Descriptorheap_impl& operator=(_Descriptorheap_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return *this;
		}

		_Descriptorheap_impl(ID3D12Device& _Device, const D3D12_DESCRIPTOR_HEAP_DESC& _Desc) {
			HRESULT hr = _Device.CreateDescriptorHeap(&_Desc, IID_PPV_ARGS(&_Myimpl));
			assert(SUCCEEDED(hr));
		}

		_Descriptorheap_impl(ID3D12Device& _Device, D3D12_DESCRIPTOR_HEAP_TYPE _Type, size_t _Size,
			D3D12_DESCRIPTOR_HEAP_FLAGS _Flag = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, uint32_t NodeMask = 0) {
			D3D12_DESCRIPTOR_HEAP_DESC _Desc;
			_Desc.Type           = _Type;
			_Desc.NumDescriptors = static_cast<UINT>(_Size);
			_Desc.Flags          = _Flag;
			_Desc.NodeMask       = NodeMask;
			HRESULT hr = _Device.CreateDescriptorHeap(&_Desc, IID_PPV_ARGS(&_Myimpl));
			assert(SUCCEEDED(hr));
		}

		void release() {
			_Myimpl = nullptr;
		}
		void swap(_Descriptorheap_impl& _Right) {
			_Myimpl.Swap(_Right._Myimpl);
		}

		operator ID3D12DescriptorHeap* () const {
			return _Myimpl.Get();
		}
		ID3D12DescriptorHeap* operator->() const {
			return _Myimpl.Get();
		}
		ID3D12DescriptorHeap* get() const {
			return _Myimpl.Get();
		}
		ID3D12DescriptorHeap& operator*() const {
			return *(_Myimpl.Get());
		}
		ID3D12DescriptorHeap& ref() const {
			return *(_Myimpl.Get());
		}

		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle(size_t _Offset/*bytes*/) const {
			auto _Handle = _Myimpl->GetGPUDescriptorHandleForHeapStart();
			_Handle.ptr += static_cast<UINT64>(_Offset);
			return _Handle;
		}

		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle(size_t _Size, size_t _Stride/*bytes*/) const {
			return gpu_handle(_Size * _Stride);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle(size_t _Offset/*bytes*/) const {
			auto _Handle = _Myimpl->GetCPUDescriptorHandleForHeapStart();
			_Handle.ptr += static_cast<UINT64>(_Offset);
			return _Handle;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle(size_t _Size, size_t _Stride/*bytes*/) const {
			return cpu_handle(_Size * _Stride);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _Myimpl;
	};

}// namespace DX12