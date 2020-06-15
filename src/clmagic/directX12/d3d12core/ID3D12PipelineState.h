#pragma once
#include "ID3D12Device.h"

namespace d3d12 {

	struct shader_pipeline : public packaged_comptr<ID3D12PipelineState>, public uncopyable {
		shader_pipeline() = default;
		shader_pipeline(shader_pipeline&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		shader_pipeline& operator=(shader_pipeline&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}
		
		shader_pipeline(ID3D12Device& _Device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& _Desc) {
			assert(SUCCEEDED( 
				_Device.CreateGraphicsPipelineState(&_Desc, IID_PPV_ARGS(&this->_Impl)) 
			));
		}
		shader_pipeline(ID3D12Device& _Device,
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
			_My_puniform = &_Uniform_register;
			_My_varying  = _Varying_register;
		}
	
		void swap(shader_pipeline& _Right) {
			std::swap(_Right._Impl, this->_Impl);
			std::swap(_Right._My_puniform, this->_My_puniform);
			std::swap(_Right._My_varying, this->_My_varying);
		}

		void release() {
			this->_Impl = nullptr;
			this->_My_puniform = nullptr;
		}

		ID3D12RootSignature*    _My_puniform;
		D3D12_INPUT_LAYOUT_DESC _My_varying;
	};

}// namespace dx12