#pragma once
#include "ID3D12Resource.h"

namespace d3d12 {

	template<DXGI_FORMAT _Fmt>
	Microsoft::WRL::ComPtr<ID3D12Resource> make_texture_resource(ID3D12Device& _Device, 
		D3D12_RESOURCE_DIMENSION _Dims, size_t _Width, size_t _Height, size_t _Depth_or_arraysize, size_t _Miplevels,
		D3D12_HEAP_TYPE       _Htype, 
		D3D12_RESOURCE_FLAGS  _Rflag    = D3D12_RESOURCE_FLAG_NONE, 
		D3D12_HEAP_FLAGS      _Hflag    = D3D12_HEAP_FLAG_NONE, 
		const D3D12_CLEAR_VALUE* _Pclear = nullptr,
		D3D12_RESOURCE_STATES _Rstate   = D3D12_RESOURCE_STATE_COMMON, 
		DXGI_SAMPLE_DESC      _Sam_desc = {1, 0}, 
		D3D12_TEXTURE_LAYOUT  _Swizzle  = D3D12_TEXTURE_LAYOUT_UNKNOWN)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> _Result;

		const auto _Rdesc = CD3DX12_RESOURCE_DESC(_Dims, 0, _Width, _Height, (UINT16)_Depth_or_arraysize, (UINT16)_Miplevels, 
			_Fmt, _Sam_desc.Count, _Sam_desc.Quality, _Swizzle, _Rflag);
		          _Rstate = inital_resource_state(_Htype, _Rstate);
		assert(SUCCEEDED(
			_Device.CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(_Htype), _Hflag, &_Rdesc, _Rstate, _Pclear, IID_PPV_ARGS(&_Result))
		));
		return std::move(_Result);
	}

	template<DXGI_FORMAT _Fmt>
	Microsoft::WRL::ComPtr<ID3D12Resource> make_dynamic_texture2D_resource(ID3D12Device& _Device, size_t _Width, size_t _Height, size_t _Miplevels, 
		D3D12_RESOURCE_FLAGS  _Rflag    = D3D12_RESOURCE_FLAG_NONE, 
		D3D12_HEAP_FLAGS      _Hflag    = D3D12_HEAP_FLAG_NONE,
		const D3D12_CLEAR_VALUE* _Pclear = nullptr,
		DXGI_SAMPLE_DESC      _Sam_desc = { 1, 0 },
		D3D12_TEXTURE_LAYOUT  _Swizzle  = D3D12_TEXTURE_LAYOUT_UNKNOWN)
	{
		return make_texture_resource<_Fmt>(_Device, D3D12_RESOURCE_DIMENSION_TEXTURE2D, _Width, _Height, 1, _Miplevels, 
			D3D12_HEAP_TYPE_UPLOAD, _Rflag, _Hflag, _Pclear, D3D12_RESOURCE_STATE_GENERIC_READ, _Sam_desc, _Swizzle);
	}
	
	template<DXGI_FORMAT _Fmt>
	Microsoft::WRL::ComPtr<ID3D12Resource> make_static_texture_resource(ID3D12Device& _Device, size_t _Width, size_t _Height, size_t _Miplevels, 
		D3D12_RESOURCE_FLAGS  _Rflag    = D3D12_RESOURCE_FLAG_NONE, 
		D3D12_HEAP_FLAGS      _Hflag    = D3D12_HEAP_FLAG_NONE, 
		const D3D12_CLEAR_VALUE* _Pclear = nullptr,
		D3D12_RESOURCE_STATES _Rstate   = D3D12_RESOURCE_STATE_COMMON,
		DXGI_SAMPLE_DESC      _Sam_desc = { 1, 0 },
		D3D12_TEXTURE_LAYOUT  _Swizzle  = D3D12_TEXTURE_LAYOUT_UNKNOWN)
	{
		return make_texture_resource<_Fmt>(_Device, D3D12_RESOURCE_DIMENSION_TEXTURE2D, _Width, _Height, 1, _Miplevels,
			D3D12_HEAP_TYPE_DEFAULT, _Rflag, _Hflag, _Pclear, _Rstate, _Sam_desc, _Swizzle);
	}

	D3D12_RENDER_TARGET_VIEW_DESC make_D3D12_RENDER_TARGET_VIEW_DESC_with_TEXTURE2D(ID3D12Resource& _Resource, size_t _Mipslice = 0, size_t _Planeslice = 0) {
		D3D12_RENDER_TARGET_VIEW_DESC _Result;
		const auto _Desc = _Resource.GetDesc();
		_Result.Format        = _Desc.Format;
		_Result.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		_Result.Texture2D.MipSlice   = static_cast<UINT>(_Mipslice);
		_Result.Texture2D.PlaneSlice = static_cast<UINT>(_Planeslice);
		return _Result;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC make_D3D12_DEPTH_STENCIL_VIEW_DESC_with_TEXTURE2D(ID3D12Resource& _Resource, size_t _Mipslice = 0, D3D12_DSV_FLAGS _Flag = D3D12_DSV_FLAG_NONE) {
		D3D12_DEPTH_STENCIL_VIEW_DESC _Result;
		const auto _Desc = _Resource.GetDesc();
		_Result.Flags         = _Flag;
		_Result.Format        = _Desc.Format;
		_Result.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		_Result.Texture2D.MipSlice = _Mipslice;
		return _Result;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC make_D3D12_SHADER_RESOURCE_VIEW_DESC_with_TEXTURE2D(ID3D12Resource& _Resource, size_t _Slice = 0) {
		D3D12_SHADER_RESOURCE_VIEW_DESC _Result;
		const auto _Desc = _Resource.GetDesc();
		assert(_Desc.DepthOrArraySize >= _Slice);
		_Result.Format                  = _Desc.Format;
		_Result.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		_Result.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		_Result.Texture2D.MostDetailedMip     = 0;
		_Result.Texture2D.MipLevels           = _Desc.MipLevels;
		_Result.Texture2D.PlaneSlice          = static_cast<UINT>(_Slice);
		_Result.Texture2D.ResourceMinLODClamp = 0.f;
		return _Result;
	}
	
	D3D12_SHADER_RESOURCE_VIEW_DESC make_D3D12_SHADER_RESOURCE_VIEW_DESC_with_TEXTURE2DARRAY(ID3D12Resource& _Resource, size_t _Array_begin, size_t _Array_end, size_t _Slice = 0) {
		D3D12_SHADER_RESOURCE_VIEW_DESC _Result;
		const auto _Desc = _Resource.GetDesc();
		assert(_Array_begin < _Array_end);
		assert(_Array_end  <= _Desc.DepthOrArraySize);
		_Result.Format                  = _Desc.Format;
		_Result.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		_Result.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		_Result.Texture2DArray.ArraySize       = static_cast<UINT>(_Array_end - _Array_begin);
		_Result.Texture2DArray.FirstArraySlice = static_cast<UINT>(_Array_begin);
		_Result.Texture2DArray.PlaneSlice      = static_cast<UINT>(_Slice);
		_Result.Texture2DArray.MostDetailedMip = 0;
		_Result.Texture2DArray.MipLevels       = _Desc.MipLevels;
		_Result.Texture2DArray.ResourceMinLODClamp = 0.f;
		return _Result;
	}


	/*- - - - - - - - - - - - - - - - - - static_texture2D - - - - - - - - - - - - - - - - - - - - -*/
	template<DXGI_FORMAT _My_fmt>
	struct dynamic_texture2D : public packaged_comptr<ID3D12Resource>, public uncopyable {
		constexpr static DXGI_FORMAT format = _My_fmt;

		dynamic_texture2D() = default;
		dynamic_texture2D(dynamic_texture2D&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		dynamic_texture2D& operator=(dynamic_texture2D&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}
	
		D3D12_RESOURCE_FLAGS get_D3D12_RESOURCE_FLAGS() const {
			const auto _Desc = this->get()->GetDesc();
			return _Desc.Flags;
		}
		D3D12_HEAP_FLAGS get_D3D12_HEAP_FLAGS() const {
			D3D12_HEAP_FLAGS _Hflag;
			this->get()->GetHeapProperties(nullptr, &_Hflag);
			return _Hflag;
		}
	};

	template<DXGI_FORMAT _My_fmt>
	struct static_texture2D : public packaged_comptr<ID3D12Resource>, public uncopyable {
		constexpr static DXGI_FORMAT format = _My_fmt;
		
		static_texture2D() = default;
		static_texture2D(static_texture2D&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		static_texture2D& operator=(static_texture2D&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}

		static_texture2D(ID3D12Device& _Device, size_t _Width, size_t _Height, size_t _Miplevels,
			D3D12_RESOURCE_FLAGS  _Rflag = D3D12_RESOURCE_FLAG_NONE,
			D3D12_HEAP_FLAGS      _Hflag = D3D12_HEAP_FLAG_NONE,
			const D3D12_CLEAR_VALUE* _Pclear = nullptr,
			D3D12_RESOURCE_STATES _Rstate = D3D12_RESOURCE_STATE_COMMON,
			DXGI_SAMPLE_DESC      _Sam_desc = { 1, 0 },
			D3D12_TEXTURE_LAYOUT  _Swizzle = D3D12_TEXTURE_LAYOUT_UNKNOWN) {
			this->_Impl = make_static_texture_resource<_My_fmt>(_Device, _Width, _Height, _Miplevels, _Rflag, _Hflag, _Pclear, _Rstate, _Sam_desc, _Swizzle);
			_My_state   = _Rstate;
		}

		D3D12_RESOURCE_FLAGS get_D3D12_RESOURCE_FLAGS() const {
			const auto _Desc = this->get()->GetDesc();
			return _Desc.Flags;
		}
		D3D12_HEAP_FLAGS get_D3D12_HEAP_FLAGS() const {
			D3D12_HEAP_FLAGS _Hflag;
			this->get()->GetHeapProperties(nullptr, &_Hflag);
			return _Hflag;
		}
		D3D12_RESOURCE_STATES get_D3D12_RESOURCE_STATES() const {
			return _My_state;
		}

		D3D12_RESOURCE_BARRIER transition(D3D12_RESOURCE_STATES _New_state) {
			auto _Transition = CD3DX12_RESOURCE_BARRIER::Transition(this->get(), _My_state, _New_state);
			_My_state = _New_state;
			return _Transition;
		}

	private:
		D3D12_RESOURCE_STATES _My_state;
	};

}// namespace d3d12