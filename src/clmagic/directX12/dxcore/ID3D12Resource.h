#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include <assert.h>

#include "packaged_comptr.h"
#include "enum_string.h"
/*
dynamic_buffer ----->----- static_buffer ----->----- \
	                                                   \	
dynamic_texture ------>---- static_texture ---->---- resource 
*/

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
		_Str += "\tD3D12_HEAP_FLAGS: ..."/* + enum_string(_Hflag)*/ + '\n';
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
	inline D3D12_RESOURCE_STATES inital_resource_state(D3D12_HEAP_TYPE _Htype) {
		switch (_Htype) {
		case D3D12_HEAP_TYPE_UPLOAD:
			return D3D12_RESOURCE_STATE_GENERIC_READ;
		case D3D12_HEAP_TYPE_READBACK:
			return D3D12_RESOURCE_STATE_COPY_DEST;
		
		case D3D12_HEAP_TYPE_DEFAULT:
		case D3D12_HEAP_TYPE_CUSTOM:
		default:
			return D3D12_RESOURCE_STATE_COMMON;
		}
	}

	inline D3D12_RESOURCE_STATES inital_resource_state(D3D12_HEAP_TYPE _Htype, D3D12_RESOURCE_STATES _Rstate) {
		switch (_Htype) {
		case D3D12_HEAP_TYPE_UPLOAD:
			return D3D12_RESOURCE_STATE_GENERIC_READ;
		case D3D12_HEAP_TYPE_READBACK:
			return D3D12_RESOURCE_STATE_COPY_DEST;

		case D3D12_HEAP_TYPE_DEFAULT:
		case D3D12_HEAP_TYPE_CUSTOM:
		default:
			return _Rstate;
		}
	}

	/*- - - - - - - - - - - - - - - - - ID3D12Resource - - - - - - - - - - - - - - - - - */
	struct resource : public packaged_comptr<ID3D12Resource>, public uncopyable {
		resource() = default;
		resource(ID3D12Device& _Device, const D3D12_RESOURCE_DESC& _Rdesc, D3D12_RESOURCE_STATES _Rstate, const D3D12_HEAP_PROPERTIES& _Hprop, D3D12_HEAP_FLAGS _Hflag,
			const D3D12_CLEAR_VALUE* _Pclear = nullptr) {
			HRESULT _Result = _Device.CreateCommittedResource(&_Hprop, _Hflag, &_Rdesc, _Rstate, _Pclear, IID_PPV_ARGS(this->_Impl.GetAddressOf()));
			assert(SUCCEEDED(_Result));
			_Mystate = _Rstate;
		}
		
		UINT64 width() const {
			assert( this->valid() );
			return this->_Impl->GetDesc().Width;
		}
		UINT   height() const {
			assert( this->valid() );
			return this->_Impl->GetDesc().Height;
		}
		UINT16 depth() const {
			assert( this->valid() );
			return this->_Impl->GetDesc().DepthOrArraySize;
		}
		UINT64 capacity() const {// (width * height * depth) [bytes]
			assert( this->valid() );
			const auto _Desc = this->_Impl->GetDesc();
			return _Desc.Width * _Desc.Height * _Desc.DepthOrArraySize;
		}
		D3D12_RESOURCE_STATES state() const {
			assert( this->valid() );
			return _Mystate;
		}

	protected:
		// upload_heapand readback_heap can't transition
		D3D12_RESOURCE_STATES _Mystate = D3D12_RESOURCE_STATE_COMMON;
	};

}// namespace d3d12