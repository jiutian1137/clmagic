#pragma once
#include <d3d12.h>
#include "d3dx12.h"

#include <assert.h>
#include <array>

#include "packaged_comptr.h"
#include "enum_string.h"

namespace d3d12 {

	struct _Pipelinestate_impl : public packaged_comptr<ID3D12PipelineState>, public uncopyable {
		_Pipelinestate_impl() = default;
		_Pipelinestate_impl(_Pipelinestate_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		_Pipelinestate_impl& operator=(_Pipelinestate_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}
		
		_Pipelinestate_impl(ID3D12Device& _Device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& _Desc) {
			assert(SUCCEEDED( 
				_Device.CreateGraphicsPipelineState(&_Desc, IID_PPV_ARGS(&this->_Impl)) 
			));
		}
		_Pipelinestate_impl(ID3D12Device& _Device,
			ID3D12RootSignature&    _Uniform_register,
			D3D12_INPUT_LAYOUT_DESC _Varying_register,
			D3D12_SHADER_BYTECODE   _Vertex_shader,
			D3D12_SHADER_BYTECODE   _Pixel_shader,
			DXGI_FORMAT             _RTVFormat,
			DXGI_FORMAT             _DSVFormat,
			DXGI_SAMPLE_DESC        _Sample,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE _Primitive = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			D3D12_BLEND_DESC        _Blend      = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
			D3D12_RASTERIZER_DESC   _Rasterizer = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
			D3D12_DEPTH_STENCIL_DESC _DSstate   = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)) {
			D3D12_GRAPHICS_PIPELINE_STATE_DESC _Desc;
			ZeroMemory(&_Desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
			_Desc.pRootSignature    = &_Uniform_register;
			_Desc.VS                = _Vertex_shader;
			_Desc.PS                = _Pixel_shader;
			_Desc.BlendState        = _Blend;
			_Desc.SampleMask        = UINT_MAX;
			_Desc.RasterizerState   = _Rasterizer;
			_Desc.DepthStencilState = _DSstate;
			_Desc.InputLayout       = _Varying_register;
			_Desc.PrimitiveTopologyType = _Primitive;
			_Desc.NumRenderTargets  = 1;
			_Desc.RTVFormats[0]     = _RTVFormat;
			_Desc.DSVFormat         = _DSVFormat;
			_Desc.SampleDesc        = _Sample;
			HRESULT hr = _Device.CreateGraphicsPipelineState(&_Desc, IID_PPV_ARGS(this->_Impl.GetAddressOf()));
			assert(SUCCEEDED(hr));
		}
	};

}// namespace dx12