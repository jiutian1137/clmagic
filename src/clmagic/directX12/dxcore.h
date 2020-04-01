#pragma once
#ifndef clmagic_directX12_DXCORE_h_
#define clmagic_directX12_DXCORE_h_
#include "enum_string.h"
#include "d3dx12.h"
#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <vector>
#include <assert.h>

#ifndef ThrowIfFailed
/* @_from Book<<3D-Game-Programming-With-DirectX12>> */
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    if(FAILED(hr__)) { throw std::exception( #x "\n" __FILE__); } \
}
#endif

namespace dx12 {
	struct _Uncopyable {
		_Uncopyable() = default;
		_Uncopyable(const _Uncopyable&) = delete;
		_Uncopyable& operator=(const _Uncopyable&) = delete;
	};


	/*
	@_ID3D12RootSignature
	*/
	class packaged_root_signature : public _Uncopyable {
		static void _Initializer(packaged_root_signature& _This, ID3D12Device& _Device, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& _Desc, UINT _Nodemask = 0) {
			Microsoft::WRL::ComPtr<ID3DBlob> _Perror;
			Microsoft::WRL::ComPtr<ID3DBlob> _Pdest;
			D3D12SerializeVersionedRootSignature(&_Desc, _Pdest.GetAddressOf(), _Perror.GetAddressOf());
			if (_Perror != nullptr) {
				MessageBoxA(nullptr, (char*)_Perror->GetBufferPointer(), "->[root_signature<>::root_signature(...)]", MB_OK);
			}

			ThrowIfFailed(_Device.CreateRootSignature(_Nodemask, _Pdest->GetBufferPointer(), _Pdest->GetBufferSize(), IID_PPV_ARGS(&_This._Mydata)));
		}

	public:
		packaged_root_signature() = default;

		packaged_root_signature(packaged_root_signature&& _Right) noexcept : _Mydata(std::move(_Right._Mydata)) {}

		packaged_root_signature(ID3D12Device& _Device, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& _Desc, UINT _Nodemask = 0) {
			_Initializer(*this, _Device, _Desc, _Nodemask);
		}

		packaged_root_signature(ID3D12Device& _Device, const std::vector<D3D12_ROOT_PARAMETER>& _Parameters, const std::vector<D3D12_STATIC_SAMPLER_DESC>& _Samplers,
			D3D12_ROOT_SIGNATURE_FLAGS _Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT, UINT _Nodemask = 0) {//_Device create D3D12RootSignature by _Parameters and _Samplers
			// 1. create D3D12_ROOT_SIGNATURE_DESC 
			D3D12_VERSIONED_ROOT_SIGNATURE_DESC _Desc;
			_Desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
			_Desc.Desc_1_0.NumParameters     = _Parameters.size();
			_Desc.Desc_1_0.pParameters       = _Parameters.data();
			_Desc.Desc_1_0.NumStaticSamplers = _Samplers.size();
			_Desc.Desc_1_0.pStaticSamplers   = _Samplers.data();
			_Desc.Desc_1_0.Flags             = _Flags;

			// 2. D3D12SerializeRootSignature(...) 
			// 3._Device.CreateRootSignature(...)
			_Initializer(*this, _Device, _Desc, _Nodemask);
		}

		packaged_root_signature(ID3D12Device& _Device, const std::vector<D3D12_ROOT_PARAMETER1>& _Parameters, const std::vector<D3D12_STATIC_SAMPLER_DESC>& _Samplers,
			D3D12_ROOT_SIGNATURE_FLAGS _Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT, UINT _Nodemask = 0) {//_Device create D3D12RootSignature by _Parameters and _Samplers
			// 1. create D3D12_ROOT_SIGNATURE_DESC 
			D3D12_VERSIONED_ROOT_SIGNATURE_DESC _Desc;
			_Desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
			_Desc.Desc_1_1.NumParameters     = _Parameters.size();
			_Desc.Desc_1_1.pParameters       = _Parameters.data();
			_Desc.Desc_1_1.NumStaticSamplers = _Samplers.size();
			_Desc.Desc_1_1.pStaticSamplers   = _Samplers.data();
			_Desc.Desc_1_1.Flags             = _Flags;

			// 2. D3D12SerializeRootSignature(...) 
			// 3._Device.CreateRootSignature(...)
			_Initializer(*this, _Device, _Desc, _Nodemask);
		}

		packaged_root_signature& operator=(packaged_root_signature&& _Right) noexcept {
			_Mydata = std::move(_Right._Mydata);
			return (*this);
		}

		ID3D12RootSignature* get() const {
			return _Mydata.Get();
		}

		bool valid() const {// _Mydata.Get() != nullptr
			return static_cast<bool>(_Mydata);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> _Mydata;
	};



	/*
	@_ID3D12PipelineState
	*/
	struct packaged_pipeline_state : public _Uncopyable {
		packaged_pipeline_state() = default;

		packaged_pipeline_state(ID3D12Device& _Device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& _Desc) {
			_Device.CreateGraphicsPipelineState(&_Desc, IID_PPV_ARGS(&_Mydata));
		}

		packaged_pipeline_state(packaged_pipeline_state&& _Right) noexcept : _Mydata(std::move(_Right._Mydata)) {}

		packaged_pipeline_state& operator=(packaged_pipeline_state&& _Right) noexcept {
			_Mydata = std::move(_Right._Mydata);
			return (*this);
		}

		ID3D12PipelineState* operator->() const {
			return _Mydata.Get();
		}

		ID3D12PipelineState* get() const {
			return _Mydata.Get();
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> _Mydata;
	};


	/*
	@_D3D12_INPUT_LAYOUT_DESC
	*/
	class input_layout : public std::vector<D3D12_INPUT_ELEMENT_DESC> {
		using _Mybase = std::vector<D3D12_INPUT_ELEMENT_DESC>;

	public:
		void push_back(const D3D12_INPUT_ELEMENT_DESC& _Desc) {
			_Mybase::push_back(_Desc);
		}

		void push_back(D3D12_INPUT_ELEMENT_DESC&& _Desc) {
			_Mybase::push_back(std::move(_Desc));
		}

		void push_back(
			LPCSTR _Semantic_name,
			UINT   _Semantic_index,
			DXGI_FORMAT _Format,
			UINT _Input_slot/*[0, 15]*/,
			UINT _Aligned_byte_offset,
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
	struct packaged_descriptor_heap : public _Uncopyable {
		static D3D12_DESCRIPTOR_HEAP_DESC generate_desc(D3D12_DESCRIPTOR_HEAP_TYPE _Type, size_t _Size,
			D3D12_DESCRIPTOR_HEAP_FLAGS _Flag, uint32_t NodeMask) {
			D3D12_DESCRIPTOR_HEAP_DESC _Desc;
			_Desc.Type     = _Type;
			_Desc.NumDescriptors = (UINT)_Size;
			_Desc.Flags    = _Flag;
			_Desc.NodeMask = NodeMask;
			return _Desc;
		}

		packaged_descriptor_heap() = default;

		packaged_descriptor_heap(ID3D12Device& _Device, const D3D12_DESCRIPTOR_HEAP_DESC& _Desc) {
			_Device.CreateDescriptorHeap(&_Desc, IID_PPV_ARGS(&_Mydata));
		}

		packaged_descriptor_heap(ID3D12Device& _Device, D3D12_DESCRIPTOR_HEAP_TYPE _Type, size_t _Size,
			D3D12_DESCRIPTOR_HEAP_FLAGS _Flag = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, uint32_t NodeMask = 0) 
			: packaged_descriptor_heap(_Device, generate_desc( _Type, _Size, _Flag, NodeMask)) {}

		packaged_descriptor_heap(packaged_descriptor_heap&& _Right) noexcept : _Mydata(std::move(_Right._Mydata)) {}

		packaged_descriptor_heap& operator=(packaged_descriptor_heap&& _Right) noexcept {
			_Mydata = std::move(_Right._Mydata);
			return *this;
		}

		ID3D12DescriptorHeap* operator->() const {
			return _Mydata.Get();
		}

		ID3D12DescriptorHeap* get() const {
			return _Mydata.Get();
		}

		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle(size_t _Offset/*bytes*/) const {
			auto _Handle = _Mydata->GetGPUDescriptorHandleForHeapStart();
			_Handle.ptr += static_cast<UINT64>(_Offset);
			return _Handle;
		}

		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle(size_t _Size, size_t _Stride/*bytes*/) const {
			return gpu_handle(_Size * _Stride);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle(size_t _Offset/*bytes*/) const {
			auto _Handle = _Mydata->GetCPUDescriptorHandleForHeapStart();
			_Handle.ptr += static_cast<UINT64>(_Offset);
			return _Handle;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle(size_t _Size, size_t _Stride/*bytes*/) const {
			return cpu_handle(_Size * _Stride);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _Mydata;
	};


	inline HRESULT create_blob(ID3DBlob** _Dest, const char* _Data, size_t _Capacity) {
		HRESULT hr = D3DCreateBlob(_Capacity, _Dest);
		if (hr == S_OK && _Data != nullptr) {
			std::copy(_Data, _Data + _Capacity, static_cast<char*>((**_Dest).GetBufferPointer()));
		}
		return hr;
	}


	/* ID3D12Device::CreatePlacedResource(...)
	void* operator new(size_t sz, ID3D12Heap* _Heap, UINT64 _Heap_offset, ..._Args) {
		Microsoft::WRL::ComPtr<ID3D10Resource> _Resource;
		ThrowIfFailed( _Device.CreatePlacedResource(_Heap, _Heap_offset, _Args... );
		return _Resource.Detach();
	}*/
	struct packaged_resource : public _Uncopyable {
		packaged_resource() = default;
		
		packaged_resource(packaged_resource&& _Right) noexcept {
			_Right._Swap(*this);
			_Right.release();
		}
		
		packaged_resource& operator=(packaged_resource&& _Right) noexcept {
			_Right._Swap(*this);
			_Right.release();
			return (*this);
		}
		
		/*
		@_Resource_state:
			When you create a resource together with a D3D12_HEAP_TYPE_UPLOAD heap, you must set InitialResourceState to D3D12_RESOURCE_STATE_GENERIC_READ.
			When you create a resource together with a D3D12_HEAP_TYPE_READBACK heap, you must set InitialResourceState to D3D12_RESOURCE_STATE_COPY_DEST.
		@_Optimized_clear_value:
			When you use D3D12_RESOURCE_DIMENSION_BUFFER, you must set pOptimizedClearValue to nullptr.
		@_from: https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommittedresource
		*/
		packaged_resource(ID3D12Device& _Device, const D3D12_HEAP_PROPERTIES& _Heap_properties, D3D12_HEAP_FLAGS _Heap_flags,
			const D3D12_RESOURCE_DESC& _Resource_desc,
			D3D12_RESOURCE_STATES      _Resource_state,
			const D3D12_CLEAR_VALUE* _Optimized_clear_value = nullptr) : _Mystate(_Resource_state) {
			/* Creates both a resource and an implicit heap,
			   such that the heap is big enough to contain the entire resource,
			   and the resource is mapped to the heap. */
			ThrowIfFailed(_Device.CreateCommittedResource(&_Heap_properties, _Heap_flags,
				&_Resource_desc, _Resource_state, _Optimized_clear_value, IID_PPV_ARGS(&_Mydata)));
		}

		virtual ~packaged_resource() {
			release();
		}

		virtual void release() {
			_Mydata = nullptr;
		}

		ID3D12Resource* operator->() const {
			return _Mydata.Get();
		}

		ID3D12Resource* get() const {
			return _Mydata.Get();
		}

		size_t capacity() const {// (width * height * depth) [bytes]
			if (valid()) {
				const auto _Desc = _Mydata->GetDesc();
				return static_cast<size_t>(_Desc.Width * _Desc.Height * _Desc.DepthOrArraySize);
			} else {
				return 0;
			}
		}

		bool valid() const {// _Mydata.Get() != nullptr
			return static_cast<bool>(_Mydata);
		}

		/*
		@_Return: _Old_state
		@_Note: upload_heap, readback_heap can't transition
		*/
		D3D12_RESOURCE_STATES transition_state(ID3D12GraphicsCommandList& _Cmdlist, D3D12_RESOURCE_STATES _Newstate) {
			auto _Oldstate = _Mystate;
			_Cmdlist.ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_Mydata.Get(), _Oldstate, _Newstate));
			_Mystate = _Newstate;
			return _Oldstate;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> _Mydata;

	protected:
		void _Swap(packaged_resource& _Right) {
			_Mydata.Swap(_Right._Mydata);
			std::swap(_Mystate, _Right._Mystate);
		}

		D3D12_RESOURCE_STATES _Mystate = D3D12_RESOURCE_STATE_COMMON;
	};


	template<D3D12_HEAP_TYPE _Theap>
	struct packaged_resource_ : public packaged_resource {
		using _Mybase = packaged_resource;

		packaged_resource_() = default;

		packaged_resource_(packaged_resource_&& _Right) noexcept
			: _Mybase(std::move(_Right)) {}

		packaged_resource_& operator=(packaged_resource_&& _Right) noexcept { 
			_Mybase::operator=(std::move(_Right));
			return (*this);
		}
		
		packaged_resource_(ID3D12Device& _Device, const D3D12_HEAP_PROPERTIES& _Heap_properties, D3D12_HEAP_FLAGS _Heap_flags,
			const D3D12_RESOURCE_DESC& _Resource_desc,
			D3D12_RESOURCE_STATES      _Resource_state,
			const D3D12_CLEAR_VALUE* _Optimized_clear_value = nullptr)
			: _Mybase(_Device, _Heap_properties, _Heap_flags, _Resource_desc, _Resource_state, _Optimized_clear_value){}
	};


	/*
	@_Upload_heap_resource
	*/
	template<>
	struct packaged_resource_<D3D12_HEAP_TYPE_UPLOAD> : public packaged_resource {
		using _Mybase = packaged_resource;

		static D3D12_HEAP_PROPERTIES heap_properties() {
			return CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		}

		packaged_resource_() = default;

		packaged_resource_(packaged_resource_&& _Right) noexcept {
			_Right._Swap(*this);
			_Right.release();
		}

		packaged_resource_& operator=(packaged_resource_&& _Right) noexcept {
			_Right._Swap(*this);
			_Right.release();
			return *this;
		}
		
		packaged_resource_(ID3D12Device& _Device, const D3D12_RESOURCE_DESC& _Desc, const D3D12_CLEAR_VALUE* _Pval = nullptr, D3D12_HEAP_FLAGS _Heap_flags = D3D12_HEAP_FLAG_NONE) 
			: _Mybase(_Device, heap_properties(), _Heap_flags, _Desc, D3D12_RESOURCE_STATE_GENERIC_READ/*requires*/, _Pval) {
			_Mybase::get()->Map(0, nullptr, reinterpret_cast<void**>(&_Mapping));
		}

		virtual ~packaged_resource_() override {
			release();
		}

		virtual void release() override {
			this->_Unmap();
			_Mybase::release();
		}

		template<typename _Ty>
		_Ty* ptr() {
			return reinterpret_cast<_Ty*>(_Mapping);
		}

		template<typename _Ty>
		_Ty* ptr(size_t _Pos) {
			return (ptr<_Ty>() + _Pos);
		}

		template<typename _Ty>
		_Ty& at(size_t _Pos) {
			return ( * ptr<_Ty>(_Pos) );
		}

		template<typename _Ty>
		_Ty* begin() {
			return ptr<_Ty>();
		}

		template<typename _Ty>
		_Ty* end() {
			return reinterpret_cast<_Ty*>(_Mapping + _Mybase::capacity());
		}

		template<typename _Ty>
		const _Ty* ptr() const {
			return reinterpret_cast<const _Ty*>(_Mapping);
		}

		template<typename _Ty>
		const _Ty* ptr(size_t _Pos) const {
			return (ptr<_Ty>() + _Pos);
		}

		template<typename _Ty>
		const _Ty& at(size_t _Pos) const {
			return (*ptr<_Ty>(_Pos));
		}

		template<typename _Ty>
		const _Ty* begin() const {
			return ptr<_Ty>();
		}

		template<typename _Ty>
		const _Ty* end() const {
			return reinterpret_cast<const _Ty*>(_Mapping + _Mybase::capacity());
		}

		template<typename _Ty>
		void copy_data(const void* _Data, size_t _Size, size_t _Off = 0) {
			if (_Data != nullptr && _Size != 0) {
				assert(_Mybase::valid() && _Mapping != nullptr);
				auto       _First = static_cast<const unsigned char*>(_Data);
				const auto _Last  = _First + _Size;
				auto       _Dest  = _Mapping + _Off;
				std::copy(_First, _Last, _Dest);
			}
		}

	private:
		void _Unmap() {
			if (_Mybase::valid()) {
				_Mybase::get()->Unmap(0, nullptr);
			}
			_Mapping = nullptr;
		}

		unsigned char* _Mapping = nullptr;

	protected:
		void _Swap(packaged_resource_& _Right) {
			_Mybase::_Swap(_Right);
			std::swap(_Mapping, _Right._Mapping);
		}
	};

	using packaged_resource_upload_heap = packaged_resource_<D3D12_HEAP_TYPE_UPLOAD>;


	// read_back_resource

	/*
	@_Default_heap_resource
	*/
	template<>
	struct packaged_resource_<D3D12_HEAP_TYPE_DEFAULT> : public packaged_resource {
		using _Mybase = packaged_resource;

		static CD3DX12_HEAP_PROPERTIES heap_properties() {
			return CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		}

		packaged_resource_() = default;

		packaged_resource_(packaged_resource_&& _Right) noexcept {
			_Right._Swap(*this);
			_Right.release();
		}

		packaged_resource_& operator=(packaged_resource_&& _Right) noexcept {
			_Right._Swap(*this);
			_Right.release();
			return (*this);
		}
		
		packaged_resource_(ID3D12Device& _Device, const D3D12_RESOURCE_DESC& _Desc, D3D12_RESOURCE_STATES _State, const D3D12_CLEAR_VALUE* _Pval = nullptr, D3D12_HEAP_FLAGS _Heap_flags = D3D12_HEAP_FLAG_NONE)
			: _Mybase(_Device, heap_properties(), _Heap_flags, _Desc, _State, _Pval) {}

		virtual ~packaged_resource_() override {
			release();
		}

		virtual void release() override {
			realease_intermediate();
			_Mybase::release();
		}

		bool uploading() const {
			return (_Myintermediate.valid());
		}

		void realease_intermediate() {
			_Myintermediate.release();
		}
	
		void upload(ID3D12Device& _Device, ID3D12GraphicsCommandList& _Cmdlist, const void* _Data, size_t _Capacity) {
			if (_Data != nullptr && _Capacity != 0) {
				if ( uploading() ) {
					const auto _Desc = _Myintermediate.get()->GetDesc();
					if (_Desc.Width == _Capacity) {
						_Upload_uncheck(_Cmdlist, _Data, _Capacity);
						return;
					}
				}

				_Myintermediate = packaged_resource_upload_heap(_Device, CD3DX12_RESOURCE_DESC::Buffer(_Capacity));
				_Upload_uncheck(_Cmdlist, _Data, _Capacity);
			}
		}
	
	private:
		void _Upload_uncheck(ID3D12GraphicsCommandList& _Cmdlist, const void* _Data, size_t _Capacity) {
			D3D12_SUBRESOURCE_DATA _Subresource;
			_Subresource.pData      = _Data;
			_Subresource.RowPitch   = _Capacity;
			_Subresource.SlicePitch = _Subresource.RowPitch;

			auto _Oldstate = _Mybase::transition_state(_Cmdlist, D3D12_RESOURCE_STATE_COPY_DEST);
			UpdateSubresources<1>(&_Cmdlist, _Mybase::get(), _Myintermediate.get(), 0, 0, 1, &_Subresource);
			_Mybase::transition_state(_Cmdlist, _Oldstate);
		}

		packaged_resource_upload_heap _Myintermediate;

	protected:
		void _Swap(packaged_resource_& _Right) {
			_Mybase::_Swap(_Right);
			std::swap(_Myintermediate, _Right._Myintermediate);
		}
	};

	using packaged_resource_default_heap = packaged_resource_<D3D12_HEAP_TYPE_DEFAULT>;

}// namespace dx12

#endif