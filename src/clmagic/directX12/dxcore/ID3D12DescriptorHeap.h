#pragma once
#include <d3d12.h>
#include <wrl.h>// Windows 
#include <assert.h>
#include <map>

#include "packaged_comptr.h"
#include "enum_string.h"

namespace d3d12 {
	// I not like ID3D12DescriptorHeap
	
	template<D3D12_DESCRIPTOR_HEAP_TYPE _Type>
	struct _Descriptor_type {// default D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		using type = D3D12_SHADER_RESOURCE_VIEW_DESC;
	};

	template<>
	struct _Descriptor_type<D3D12_DESCRIPTOR_HEAP_TYPE_RTV> {
		using type = D3D12_RENDER_TARGET_VIEW_DESC;
	};
	
	template<>
	struct _Descriptor_type<D3D12_DESCRIPTOR_HEAP_TYPE_DSV> {
		using type = D3D12_DEPTH_STENCIL_VIEW_DESC;
	};
	
	template<D3D12_DESCRIPTOR_HEAP_TYPE _Type>
	struct _Descriptor_creator {// default D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		void operator()(ID3D12Device& _Device, ID3D12Resource& _Resource, D3D12_SHADER_RESOURCE_VIEW_DESC _Desc, D3D12_CPU_DESCRIPTOR_HANDLE _Dest_descriptor) const {
			_Device.CreateShaderResourceView(&_Resource, &_Desc, _Dest_descriptor);
		}
	};

	template<>
	struct _Descriptor_creator<D3D12_DESCRIPTOR_HEAP_TYPE_RTV> {
		void operator()(ID3D12Device& _Device, ID3D12Resource& _Resource, D3D12_RENDER_TARGET_VIEW_DESC _Desc, D3D12_CPU_DESCRIPTOR_HANDLE _Dest_descriptor) const {
			_Device.CreateRenderTargetView(&_Resource, &_Desc, _Dest_descriptor);
		}
	};
	
	template<>
	struct _Descriptor_creator<D3D12_DESCRIPTOR_HEAP_TYPE_DSV> {
		void operator()(ID3D12Device& _Device, ID3D12Resource& _Resource, D3D12_DEPTH_STENCIL_VIEW_DESC _Desc, D3D12_CPU_DESCRIPTOR_HANDLE _Dest_descriptor) const {
			_Device.CreateDepthStencilView(&_Resource, &_Desc, _Dest_descriptor);
		}
	};


	inline D3D12_GPU_DESCRIPTOR_HANDLE get_D3D12_GPU_DESCRIPTOR_HANDLE(ID3D12DescriptorHeap& _Array, size_t _Pos, size_t _Stride) {
		auto _Result = _Array.GetGPUDescriptorHandleForHeapStart();
		_Result.ptr += _Pos * _Stride;
		return _Result;
	}
	
	inline D3D12_CPU_DESCRIPTOR_HANDLE get_D3D12_CPU_DESCRIPTOR_HANDLE(ID3D12DescriptorHeap& _Array, size_t _Pos, size_t _Stride) {
		auto _Result = _Array.GetCPUDescriptorHandleForHeapStart();
		_Result.ptr += _Pos * _Stride;
		return _Result;
	}


	template<D3D12_DESCRIPTOR_HEAP_TYPE _Mytype, size_t _Mysize>
	struct descriptor_array : public packaged_comptr<ID3D12DescriptorHeap>, public uncopyable {
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
		using descriptor_type = typename _Descriptor_type<_Mytype>::type;
		
		descriptor_array() = default;
		descriptor_array(descriptor_array&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		descriptor_array& operator=(descriptor_array&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return *this;
		}

		descriptor_array(ID3D12Device& _Device, D3D12_DESCRIPTOR_HEAP_FLAGS _Flag = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, uint32_t _Nodemask = 0) {
			D3D12_DESCRIPTOR_HEAP_DESC _Desc;
			_Desc.Type           = _Mytype;
			_Desc.NumDescriptors = _Mysize;
			_Desc.Flags          = _Flag;
			_Desc.NodeMask       = _Nodemask;
			assert(SUCCEEDED(
				_Device.CreateDescriptorHeap(&_Desc, IID_PPV_ARGS(&this->_Impl))
			));
			_My_stride = _Device.GetDescriptorHandleIncrementSize(_Mytype);
		}

		void swap(descriptor_array& _Right) {
			_Right._Impl.Swap(this->_Impl);
			std::swap(_Right._My_stride, this->_My_stride);
		}

		size_t size() const {
			return this->_Impl->GetDesc().NumDescriptors;
		}
		D3D12_CPU_DESCRIPTOR_HANDLE get_CPUhandle(size_t _Pos) const {
			assert(_Pos < _Mysize);
			return get_D3D12_CPU_DESCRIPTOR_HANDLE(this->ref(), _Pos, _My_stride);
		}
		D3D12_GPU_DESCRIPTOR_HANDLE get_GPUhandle(size_t _Pos) const {
			assert(_Pos < _Mysize);
			return get_D3D12_GPU_DESCRIPTOR_HANDLE(this->ref(), _Pos, _My_stride);
		}
		void set(size_t _Pos, ID3D12Resource& _Map_resource, descriptor_type _Map_desc) {
			assert(_Pos < _Mysize);
			Microsoft::WRL::ComPtr<ID3D12Device> _Device = get_device(this->ref());
			_Descriptor_creator<_Mytype>()(*_Device.Get(), _Map_resource, _Map_desc, get_CPUhandle(_Pos));
		}

	private:
		UINT _My_stride;
	};
	

	template<typename _Kty, D3D12_DESCRIPTOR_HEAP_TYPE _Mytype, size_t _Mysize>
	struct descriptor_array_map : public packaged_comptr<ID3D12DescriptorHeap>, public uncopyable {
		using descriptor_type = typename _Descriptor_type<_Mytype>::type;

		descriptor_array_map() = default;
		descriptor_array_map(descriptor_array_map&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		descriptor_array_map& operator=(descriptor_array_map&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return *this;
		}
		
		descriptor_array_map(ID3D12Device& _Device, D3D12_DESCRIPTOR_HEAP_FLAGS _Flag = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, uint32_t _Nodemask = 0) {
			D3D12_DESCRIPTOR_HEAP_DESC _Desc;
			_Desc.Type           = _Mytype;
			_Desc.NumDescriptors = _Mysize;
			_Desc.Flags          = _Flag;
			_Desc.NodeMask       = _Nodemask;
			assert(SUCCEEDED(
				_Device.CreateDescriptorHeap(&_Desc, IID_PPV_ARGS(&this->_Impl))
			));
			_My_stride = _Device.GetDescriptorHandleIncrementSize(_Mytype);
		}

		void swap(descriptor_array_map& _Right) {
			_Right._Impl.Swap(this->_Impl);
			std::swap(_Right._My_stride, _My_stride);
			_Right._My_map.swap(_My_map);
		}

		size_t size() const {
			return this->_Impl->GetDesc().NumDescriptors;
		}
		D3D12_CPU_DESCRIPTOR_HANDLE get_CPUhandle(const _Kty& _Kval) const {
			auto _Where = _My_map.find(_Kval);
			assert(_Where != _My_map.end());
			return get_D3D12_CPU_DESCRIPTOR_HANDLE(this->ref(), _Where->second, _My_stride);
		}
		D3D12_GPU_DESCRIPTOR_HANDLE get_GPUhandle(const _Kty& _Kval) const {
			auto _Where = _My_map.find(_Kval);
			assert(_Where != _My_map.end());
			return get_D3D12_GPU_DESCRIPTOR_HANDLE(this->ref(), _Where->second, _My_stride);
		}
		void insert_or_assign(const _Kty& _Keyval, ID3D12Resource& _Map_resource, descriptor_type _Map_desc) {
			auto _Where = _My_map.find(_Keyval);
			if (_Where == _My_map.end()) {
				assert(_My_map.size() < _Mysize);
				// 1. get _Mapval
				size_t _Mapval = _My_map.size();
				// 2. create resource_view into _Mapval-place
				Microsoft::WRL::ComPtr<ID3D12Device> _Device = get_device(this->ref());
				_Descriptor_creator<_Mytype>()(*_Device.Get(), _Map_resource, _Map_desc, get_D3D12_CPU_DESCRIPTOR_HANDLE(this->ref(), _Mapval, _My_stride));
				// 3. sync _My_map
				_My_map.insert_or_assign(_Keyval, _Mapval);
			} else {// re-create
				Microsoft::WRL::ComPtr<ID3D12Device> _Device = get_device(this->ref());
				_Descriptor_creator<_Mytype>()(*_Device.Get(), _Map_resource, _Map_desc, get_D3D12_CPU_DESCRIPTOR_HANDLE(this->ref(), _Where->second, _My_stride));
			}
		}

	private:
		UINT                   _My_stride;
		std::map<_Kty, size_t> _My_map;
	};

}// namespace DX12