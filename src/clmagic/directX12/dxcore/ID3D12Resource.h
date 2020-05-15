#pragma once
#include <d3d12.h>
#include <wrl.h>// Windows 
#include <assert.h>

#include "d3dx12.h"
#include "uncopyable.h"
#include "enum_string.h"

namespace d3d12 {
	// _Resource.infomation to std::string
	inline std::string to_string(ID3D12Resource& _Resource) {
		const D3D12_RESOURCE_DESC _Rdesc = _Resource.GetDesc();
		D3D12_HEAP_PROPERTIES     _Hprop;
		D3D12_HEAP_FLAGS          _Hflag;
		_Resource.GetHeapProperties(&_Hprop, &_Hflag);

		std::string _Str = "ID3D12Resource{\n";
		_Str += "\tD3D12_HEAP_PROPERTIES{\n";
		_Str += "\t\tType: "             + enum_string(_Hprop.Type) + '\n';
		_Str += "\t\tCreationNodeMask: " + std::to_string(_Hprop.CreationNodeMask) + '\n';
		_Str += "\t\tVisibleNodeMask: "  + std::to_string(_Hprop.VisibleNodeMask) + '\n';
		_Str += "\t}\n";
		_Str += "\tD3D12_HEAP_FLAGS: " + enum_string(_Hflag) + '\n';
		_Str += "\tD3D12_RESOURCE_DESC{\n";
		_Str += "\t\tDimension: " + enum_string(_Rdesc.Dimension) + '\n';
		_Str += "\t\tAlignment: " + std::to_string(_Rdesc.Alignment) + '\n';
		_Str += "\t\tWidth: "     + std::to_string(_Rdesc.Width) + '\n';
		_Str += "\t\tHeight: "    + std::to_string(_Rdesc.Height) + '\n';
		_Str += "\t\tDepthOrArraySize: " + std::to_string(_Rdesc.DepthOrArraySize) + '\n';
		_Str += "\t\tMipLevels: " + std::to_string(_Rdesc.MipLevels) + '\n';
		_Str += "\t\tFormat: ...\n";
		_Str += "\t\tSampleDesc{\n";
		_Str += "\t\t\tCount: "   + std::to_string(_Rdesc.SampleDesc.Count) +'\n';
		_Str += "\t\t\tQuality: " + std::to_string(_Rdesc.SampleDesc.Quality) + '\n';
		_Str += "\t\t}\n";
		_Str += "\t\tD3D12_TEXTURE_LAYOUT: " + enum_string(_Rdesc.Layout) + '\n';
		_Str += "\t\tD3D12_RESOURCE_FLAGS: " + enum_string(_Rdesc.Flags) + '\n';
		_Str += "\t}";
		return _Str;
	}

	// src_buffer copy to dst_buffer
	inline void buffer_copy(ID3D12GraphicsCommandList& cmdList, 
		ID3D12Resource& src, UINT64 srcoffset,
		ID3D12Resource& dst, UINT64 dstoffset, UINT64 width) {
		// same of glCopyImageSubData(...)
		cmdList.CopyBufferRegion(&dst, dstoffset, &src, srcoffset, width);
	}
	// src_resource copy to dst_resource
	inline void resource_copy(ID3D12GraphicsCommandList& cmdList, 
		const CD3DX12_TEXTURE_COPY_LOCATION& src, size_t srcX, size_t srcY, size_t srcZ, 
		const CD3DX12_TEXTURE_COPY_LOCATION& dst, size_t dstX, size_t dstY, size_t dstZ, size_t width, size_t height, size_t depth) {
		// same of glCopyImageSubData(...)
		CD3DX12_BOX _Srcbox(srcX, srcY + height, srcZ + depth, srcX + width, srcY, srcZ);
		cmdList.CopyTextureRegion(&dst, dstX, dstY, dstZ, &src, &_Srcbox);
	}
	// src_texture copy to dst_texture
	inline void texture_copy(ID3D12GraphicsCommandList& cmdList, 
		ID3D12Resource& src, size_t srcX, size_t srcY, size_t srcZ, 
		ID3D12Resource& dst, size_t dstX, size_t dstY, size_t dstZ, size_t width, size_t height, size_t depth) {
		// same of glCopyImageSubData(...)
		CD3DX12_TEXTURE_COPY_LOCATION _Dst(&dst, (UINT)0);
		CD3DX12_TEXTURE_COPY_LOCATION _Src(&dst, (UINT)0);
		resource_copy(cmdList, _Src, srcX, srcY, srcZ, _Dst, dstX, dstY, dstZ, width, height, depth);
	}
	// src_resource upload to dst_resource
	inline void resource_upload(ID3D12GraphicsCommandList& cmdList, 
		ID3D12Resource& _Upload_heap_resource, size_t srcX, size_t srcY, size_t srcZ,
		ID3D12Resource& _Default_heap_resource, size_t dstX, size_t dstY, size_t dstZ, size_t width, size_t height, size_t depth) {
		const auto _Dimension0 = _Upload_heap_resource.GetDesc().Dimension;
		const auto _Dimension1 = _Default_heap_resource.GetDesc().Dimension;
		assert(_Dimension0 == _Dimension1);

		if (_Dimension0 == D3D12_RESOURCE_DIMENSION_BUFFER) {
			buffer_copy(cmdList, _Upload_heap_resource, srcX*srcY*srcZ, _Default_heap_resource, dstX*dstY*dstZ, width*height*depth);
		} else {
			texture_copy(cmdList, _Upload_heap_resource, srcX, srcY, srcZ, _Default_heap_resource, dstX, dstY, dstZ, width, height, depth);
		}
	}


	/*- - - - - - - - - - - - - - - - - ID3D12Resource - - - - - - - - - - - - - - - - - */
	struct _Resource_impl : public uncopyable {
		_Resource_impl() = default;
		_Resource_impl(ID3D12Device& _Device, const D3D12_RESOURCE_DESC& _Rdesc, D3D12_RESOURCE_STATES _Rstate, const D3D12_HEAP_PROPERTIES& _Hprop, D3D12_HEAP_FLAGS _Hflag, 
			const D3D12_CLEAR_VALUE* _Pclear = nullptr) {
			HRESULT hr = _Device.CreateCommittedResource(&_Hprop, _Hflag, &_Rdesc, _Rstate, _Pclear, IID_PPV_ARGS(_Myimpl.GetAddressOf()));
			assert(SUCCEEDED(hr));
			_Mystate = _Rstate;
		}

		void release() {
			_Myimpl = nullptr;
		}
		void swap(_Resource_impl& _Right) {
			_Myimpl.Swap(_Right._Myimpl);
			std::swap(_Mystate, _Right._Mystate);
		}
		
		operator ID3D12Resource* () const {
			return _Myimpl.Get();
		}
		ID3D12Resource* get() const {
			return _Myimpl.Get();
		}
		ID3D12Resource* operator->() const {
			return _Myimpl.Get();
		}
		ID3D12Resource& ref() const {
			return *(_Myimpl.Get());
		}
		ID3D12Resource& operator*() const {
			return *(_Myimpl.Get());
		}

		bool   valid() const {// _Mydata.Get() != nullptr
			return static_cast<bool>(_Myimpl);
		}
		UINT64 width() const {
			assert( valid() );
			return _Myimpl->GetDesc().Width;
		}
		UINT   height() const {
			assert( valid() );
			return _Myimpl->GetDesc().Height;
		}
		UINT16 depth() const {
			assert( valid() );
			return _Myimpl->GetDesc().DepthOrArraySize;
		}
		UINT64 capacity() const {// (width * height * depth) [bytes]
			assert( valid() );
			const auto _Desc = _Myimpl->GetDesc();
			return _Desc.Width * _Desc.Height * _Desc.DepthOrArraySize;
		}
		D3D12_RESOURCE_STATES state() const {
			assert( valid() );
			return _Mystate;
		}

	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource> _Myimpl;
		// upload_heapand readback_heap can't transition
		D3D12_RESOURCE_STATES                  _Mystate = D3D12_RESOURCE_STATE_COMMON;
	};

	template<D3D12_HEAP_TYPE _Myht>
	struct _Resource_impl_2 : public _Resource_impl {// D3D12_HEAP_TYPE_DEFAULT
		_Resource_impl_2() = default;
		_Resource_impl_2(ID3D12Device& _Device, const D3D12_RESOURCE_DESC& _Rdesc, D3D12_HEAP_FLAGS _Hflag,
			const D3D12_CLEAR_VALUE* _Pclear = nullptr)
			: _Resource_impl(_Device, _Rdesc, D3D12_RESOURCE_STATE_COMMON, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), _Hflag, _Pclear) {}

		D3D12_RESOURCE_BARRIER transition(D3D12_RESOURCE_STATES _Newstate) {// Used to ID3D12GraphicsCommandList::ResourceBarrier(...)
			assert(this->valid());
			auto _Barrier = CD3DX12_RESOURCE_BARRIER::Transition(this->get(), this->state(), _Newstate);
			_Resource_impl::_Mystate = _Newstate;
			return _Barrier;
		}
		
		/*void swap(_Resource_impl_2& _Right) {
			_Resource_impl::swap(_Right);
		}*/
	};

	template<>
	struct _Resource_impl_2<D3D12_HEAP_TYPE_UPLOAD> : public _Resource_impl {
		_Resource_impl_2() = default;
		_Resource_impl_2(ID3D12Device& _Device, const D3D12_RESOURCE_DESC& _Rdesc, D3D12_HEAP_FLAGS _Hflag,
			const D3D12_CLEAR_VALUE* _Pclear = nullptr)
			: _Resource_impl(_Device, _Rdesc, D3D12_RESOURCE_STATE_GENERIC_READ/*must*/, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), _Hflag, _Pclear) {}
	
		void swap(_Resource_impl_2& _Right) {
			_Resource_impl::swap(_Right);
		}
	};

	template<>
	struct _Resource_impl_2<D3D12_HEAP_TYPE_READBACK> : public _Resource_impl {
		_Resource_impl_2() = default;
		_Resource_impl_2(ID3D12Device& _Device, const D3D12_RESOURCE_DESC& _Rdesc, D3D12_HEAP_FLAGS _Hflag,
			const D3D12_CLEAR_VALUE* _Pclear = nullptr)
			: _Resource_impl(_Device, _Rdesc, D3D12_RESOURCE_STATE_COPY_DEST/*must*/, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK), _Hflag, _Pclear) {}
	
		void swap(_Resource_impl_2& _Right) {
			_Resource_impl::swap(_Right);
		}
	};


	/*
	<ID3D12Device::CreateCommittedResource(...)>
		@_Resource_state:
			When you create a resource together with a D3D12_HEAP_TYPE_UPLOAD heap, you must set InitialResourceState to D3D12_RESOURCE_STATE_GENERIC_READ.
			When you create a resource together with a D3D12_HEAP_TYPE_READBACK heap, you must set InitialResourceState to D3D12_RESOURCE_STATE_COPY_DEST.
		@_Optimized_clear_value:
			When you use D3D12_RESOURCE_DIMENSION_BUFFER, you must set pOptimizedClearValue to nullptr.
		@_from: https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommittedresource
	</ID3D12Device::CreateCommittedResource(...)>
	*/

	/*- - - - - - - - - - - - - - - - - - _Buffer_resource - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ty, D3D12_HEAP_TYPE _Myht>// D3D12_HEAP_TYPE_DEFAULT
	struct _Buffer_resource : public _Resource_impl_2<_Myht> {
		using _Mybase    = _Resource_impl_2<_Myht>;
		using value_type = _Ty;
		
		_Buffer_resource() = default;
		_Buffer_resource(_Buffer_resource&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		_Buffer_resource& operator=(_Buffer_resource&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}
		_Buffer_resource(ID3D12Device& _Device, size_t _Count,
			D3D12_HEAP_FLAGS _Hflag  = D3D12_HEAP_FLAG_NONE)
			: _Mybase(_Device, CD3DX12_RESOURCE_DESC::Buffer(_Count * sizeof(_Ty)), _Hflag, nullptr/*must*/) {}

		D3D12_BOX operator()(size_t _First, size_t _Last) const {
			assert( this->valid() );
			size_t _First_bytes = _First * sizeof(_Ty);
			size_t _Last_bytes  = _Last * sizeof(_Ty);
			assert( _First_bytes <= _Last_bytes );
			assert( _Last_bytes  <= this->width() );
			return CD3DX12_BOX( _First_bytes, _Last_bytes );
		}
	
		D3D12_VERTEX_BUFFER_VIEW as_D3D12_VERTEX_BUFFER_VIEW() const {
			D3D12_VERTEX_BUFFER_VIEW _Desc;
			_Desc.BufferLocation = this->get()->GetGPUVirtualAddress();
			_Desc.SizeInBytes    = static_cast<UINT>(this->capacity());
			_Desc.StrideInBytes  = sizeof(_Ty);
			return _Desc;
		}
		D3D12_INDEX_BUFFER_VIEW as_D3D12_INDEX_BUFFER_VIEW() const {
			D3D12_INDEX_BUFFER_VIEW _Desc;
			_Desc.BufferLocation = this->get()->GetGPUVirtualAddress();
			_Desc.SizeInBytes    = static_cast<UINT>(this->capacity());
			_Desc.Format         = sizeof(_Ty) == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
			return _Desc;
		}
	};

	/*- - - - - - - - - - - - - - - - - - _Texture1D_resource - - - - - - - - - - - - - - - - - - - - -*/
	template<DXGI_FORMAT         _Myfmt, 
		D3D12_RESOURCE_DIMENSION _Mydims = D3D12_RESOURCE_DIMENSION_TEXTURE1D, 
		D3D12_HEAP_TYPE          _Myht   = D3D12_HEAP_TYPE_DEFAULT>
	struct _Texture_resource : public _Resource_impl_2<_Myht> {
		using _Mybase = _Resource_impl_2<_Myht>;
		static constexpr DXGI_FORMAT format = _Myfmt;
		
		_Texture_resource() = default;
		_Texture_resource(_Texture_resource&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		_Texture_resource& operator=(_Texture_resource&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}
		_Texture_resource(ID3D12Device& _Device, size_t _Width, D3D12_HEAP_FLAGS _Hflag = D3D12_HEAP_FLAG_NONE,
			const D3D12_CLEAR_VALUE* _Pclear = nullptr)
			: _Mybase(_Device, CD3DX12_RESOURCE_DESC::Tex1D(_Myfmt, _Width), _Hflag, _Pclear) {}

		D3D12_BOX operator()(size_t _First, size_t _Last) const {
			assert( this->valid() );
			assert( _First <= _Last );
			assert( _Last  <= this->width() );
			return CD3DX12_BOX(_First, _Last);
		}
	
		D3D12_SHADER_RESOURCE_VIEW_DESC as_D3D12_SHADER_RESOURCE_VIEW_DESC(
			UINT Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING) const {
			const auto _Mydesc = this->get()->GetDesc();

			D3D12_SHADER_RESOURCE_VIEW_DESC _Desc;
			_Desc.Format                  = _Mydesc.Format;
			_Desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE1D;
			_Desc.Shader4ComponentMapping = Shader4ComponentMapping;
			_Desc.Texture1D.MostDetailedMip     = 0;
			_Desc.Texture1D.MipLevels           = _Mydesc.MipLevels;
			_Desc.Texture1D.ResourceMinLODClamp = 0.f;
			return _Desc;
		}
	};


	/*- - - - - - - - - - - - - - - - - - Static_Texture2D<_Format> - - - - - - - - - - - - - - - - - - - - -*/
	template<DXGI_FORMAT _Myfmt, D3D12_HEAP_TYPE _Myht>
	struct _Texture_resource<_Myfmt, D3D12_RESOURCE_DIMENSION_TEXTURE2D, _Myht> : public _Resource_impl_2<_Myht> {
		using _Mybase = _Resource_impl_2<_Myht>;
		static constexpr DXGI_FORMAT format = _Myfmt;

		_Texture_resource() = default;
		_Texture_resource(_Texture_resource && _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		_Texture_resource& operator=(_Texture_resource && _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}
		_Texture_resource(ID3D12Device& _Device, size_t _Width, size_t _Height, D3D12_HEAP_FLAGS _Hflag = D3D12_HEAP_FLAG_NONE, 
			const D3D12_CLEAR_VALUE* _Pclear = nullptr)
			: _Mybase(_Device, CD3DX12_RESOURCE_DESC::Tex2D(_Myfmt, _Width, _Height), _Hflag, _Pclear) {}
	
		D3D12_BOX operator()(size_t x, size_t y, size_t x_end, size_t y_end) const {
			assert(this->valid());
			assert(x <= x_end);
			assert(y <= y_end);
			assert(x_end <= this->width());
			assert(y_end <= this->height());
			return CD3DX12_BOX(x, y, x_end, y_end);
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC as_D3D12_SHADER_RESOURCE_VIEW_DESC(
			UINT Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING) const {
			const auto _Mydesc = this->get()->GetDesc();

			D3D12_SHADER_RESOURCE_VIEW_DESC _Desc;
			_Desc.Format                  = _Mydesc.Format;
			_Desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
			_Desc.Shader4ComponentMapping = Shader4ComponentMapping;
			_Desc.Texture2D.MostDetailedMip     = 0;
			_Desc.Texture2D.MipLevels           = _Mydesc.MipLevels;
			_Desc.Texture2D.PlaneSlice          = 0;
			_Desc.Texture2D.ResourceMinLODClamp = 0.f;
			return _Desc;
		}
	};


	/*- - - - - - - - - - - - - - - - - - Static_Texture3D<_Format> - - - - - - - - - - - - - - - - - - - - -*/
	template<DXGI_FORMAT _Myfmt, D3D12_HEAP_TYPE _Myht>
	struct _Texture_resource<_Myfmt, D3D12_RESOURCE_DIMENSION_TEXTURE3D, _Myht> : public _Resource_impl_2<_Myht> {
		using _Mybase = _Resource_impl_2<_Myht>;
		static constexpr DXGI_FORMAT format = _Myfmt;

		_Texture_resource() = default;
		_Texture_resource(_Texture_resource&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		_Texture_resource& operator=(_Texture_resource&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}
		_Texture_resource(ID3D12Device& _Device, size_t _Width, size_t _Height, size_t _Depth, D3D12_HEAP_FLAGS _Hflag = D3D12_HEAP_FLAG_NONE,
			const D3D12_CLEAR_VALUE* _Pclear = nullptr)
			: _Mybase(_Device, CD3DX12_RESOURCE_DESC::Tex3D(_Myfmt, _Width, _Height, _Depth), _Hflag, _Pclear) {}

		D3D12_BOX operator()(size_t x, size_t y, size_t z, size_t x_end, size_t y_end, size_t z_end) const {
			assert(this->valid());
			assert(x <= x_end);
			assert(y <= y_end);
			assert(z <= z_end);
			assert(x_end <= this->width());
			assert(y_end <= this->height());
			assert(z_end <= this->dedpth());
			return CD3DX12_BOX(x, y, z, x_end, y_end, z_end);
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC as_D3D12_SHADER_RESOURCE_VIEW_DESC(
			UINT Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING) const {
			const auto _Mydesc = this->get()->GetDesc();

			D3D12_SHADER_RESOURCE_VIEW_DESC _Desc;
			_Desc.Format                  = _Mydesc.Format;
			_Desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE3D;
			_Desc.Shader4ComponentMapping = Shader4ComponentMapping;
			_Desc.Texture3D.MostDetailedMip     = 0;
			_Desc.Texture3D.MipLevels           = _Mydesc.MipLevels;
			_Desc.Texture3D.ResourceMinLODClamp = 0.f;
			return _Desc;
		}
	};

}// namespace d3d12