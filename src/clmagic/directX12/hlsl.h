#pragma once
#ifndef clmagic_directX12_HLSL_h_
#define clmagic_directX12_HLSL_h_
#include "dxcore.h"
#include <d3dcompiler.h>

namespace hlsl {
	enum D3DCompileFlags {
		eDEBUG                    = D3DCOMPILE_DEBUG,
		eSKIP_VALIDATION          = D3DCOMPILE_SKIP_VALIDATION,
		eSKIP_OPTIMIZATION        = D3DCOMPILE_SKIP_OPTIMIZATION,
		ePACK_MATRIX_ROW_MAJOR    = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR,
		ePACK_MATRIX_COLUMN_MAJOR = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR,
		ePARTIAL_PRECISION        = D3DCOMPILE_PARTIAL_PRECISION,
		eFORCE_VS_SOFTWARE_NO_OPT = D3DCOMPILE_FORCE_VS_SOFTWARE_NO_OPT,
		eFORCE_PS_SOFTWARE_NO_OPT = D3DCOMPILE_FORCE_PS_SOFTWARE_NO_OPT,
		eNO_PRESHADER             = D3DCOMPILE_NO_PRESHADER,
		eAVOID_FLOW_CONTROL       = D3DCOMPILE_AVOID_FLOW_CONTROL,
		ePREFER_FLOW_CONTROL      = D3DCOMPILE_PREFER_FLOW_CONTROL,
		eENABLE_STRICTNESS        = D3DCOMPILE_ENABLE_STRICTNESS,
		eENABLE_BACKWARDS_COMPATIBILITY = D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY,
		eIEEE_STRICTNESS          = D3DCOMPILE_IEEE_STRICTNESS,
		eOPTIMIZATION_LEVEL0      = D3DCOMPILE_OPTIMIZATION_LEVEL0,
		eOPTIMIZATION_LEVEL1      = D3DCOMPILE_OPTIMIZATION_LEVEL1,
		eOPTIMIZATION_LEVEL2      = D3DCOMPILE_OPTIMIZATION_LEVEL2,
		eOPTIMIZATION_LEVEL3      = D3DCOMPILE_OPTIMIZATION_LEVEL3,
		eRESERVED16               = D3DCOMPILE_RESERVED16,
		eRESERVED17               = D3DCOMPILE_RESERVED17,
		eWARNINGS_ARE_ERRORS      = D3DCOMPILE_WARNINGS_ARE_ERRORS,
		eRESOURCES_MAY_ALIAS      = D3DCOMPILE_RESOURCES_MAY_ALIAS,
		eENABLE_UNBOUNDED_DESCRIPTOR_TABLES = D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES,
		eALL_RESOURCES_BOUND      = D3DCOMPILE_ALL_RESOURCES_BOUND,
		eDEBUG_NAME_FOR_SOURCE    = D3DCOMPILE_DEBUG_NAME_FOR_SOURCE,
		eDEBUG_NAME_FOR_BINARY    = D3DCOMPILE_DEBUG_NAME_FOR_BINARY
	};

	/* enum D3DCompileTargets 
		cs_5_1 = "cs_5_1"
		ds_5_1 = "ds_5_1"
		gs_5_1 = "gs_5_1"
		hs_5_1 = "hs_5_1"
		ps_5_1 = "ps_5_1"
		vs_5_1 = "vs_5_1"
	Direct3D 11.0 and 11.1 feature levels
		cs_5_0 = "cs_5_0"
		ds_5_0 = "ds_5_0"
		gs_5_0 = "gs_5_0"
		hs_5_0 = "hs_5_0"
		ps_5_0 = "ps_5_0"
		vs_5_0 = "vs_5_0"
	Direct3D 10.1 feature level
		cs_4_1 = "cs_4_1"
		gs_4_1 = "gs_4_1"
		ps_4_1 = "ps_4_1"
		vs_4_1 = "vs_4_1"
	Direct3D 10.0 feature level
		cs_4_0 = "cs_4_0"
		gs_4_0 = "gs_4_0"
		ps_4_0 = "ps_4_0"
		vs_4_0 = "vs_4_0"
	Direct3D 9.1, 9.2, and 9.3 feature levels
		ps_4_0_level_9_1 = "ps_4_0_level_9_1"
		ps_4_0_level_9_3 = "ps_4_0_level_9_3"
		vs_4_0_level_9_1 = "vs_4_0_level_9_1"
		vs_4_0_level_9_3 = "vs_4_0_level_9_3"
	Legacy Direct3D 9 Shader Model 3.0
		ps_3_0  = "ps_3_0"
		ps_3_sw = "ps_3_sw"
		vs_3_0  = "vs_3_0"
		vs_3_sw = "vs_3_sw"
	...
	@_from: https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/specifying-compiler-targets
	@_Note: ID3D12Device::CheckFeatureSupport(///)
	*/

	Microsoft::WRL::ComPtr<ID3DBlob> shader_compile(std::string _Source/*must ASCII*/, const D3D_SHADER_MACRO* _Defines, LPCSTR _Entrypoint, LPCSTR _Target/*@_D3DCompileTargets */, 
		LPCSTR _Source_name = nullptr/*ErrorMessage*/, D3DCompileFlags _Flag1 = eOPTIMIZATION_LEVEL1, D3DCompileFlags _Flag2 = eOPTIMIZATION_LEVEL1) {
		/* https://docs.microsoft.com/en-us/windows/win32/api/d3dcompiler/nf-d3dcompiler-d3dcompile */
		HRESULT hr = S_OK;

		Microsoft::WRL::ComPtr<ID3DBlob> _Pdest;
		Microsoft::WRL::ComPtr<ID3DBlob> _Perror;
		hr = D3DCompile(_Source.c_str(), _Source.size(), _Source_name, 
			_Defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, _Entrypoint, _Target, _Flag1, _Flag2, _Pdest.GetAddressOf(), _Perror.GetAddressOf());

		if (_Perror != nullptr) {
			MessageBoxA(nullptr, (char*)_Perror->GetBufferPointer(), "->[root_signature<>::root_signature(...)]", MB_OK);
		}

		ThrowIfFailed(hr);
		return _Pdest;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> shader_compile(LPCWSTR _Filename/*must ASCII-file*/, const D3D_SHADER_MACRO* _Defines, LPCSTR _Entrypoint, LPCSTR _Target/*@_D3DCompileTargets */,
		D3DCompileFlags _Flag1 = eOPTIMIZATION_LEVEL1, D3DCompileFlags _Flag2 = eOPTIMIZATION_LEVEL1) {
		/* https://docs.microsoft.com/en-us/windows/win32/api/d3dcompiler/nf-d3dcompiler-d3dcompile */
		HRESULT hr = S_OK;

		Microsoft::WRL::ComPtr<ID3DBlob> _Pdest;
		Microsoft::WRL::ComPtr<ID3DBlob> _Perror;
		hr = D3DCompileFromFile(_Filename, _Defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, _Entrypoint, _Target, _Flag1, _Flag2, _Pdest.GetAddressOf(), _Perror.GetAddressOf());

		if (_Perror != nullptr) {
			MessageBoxA(nullptr, (char*)_Perror->GetBufferPointer(), "->[root_signature<>::root_signature(...)]", MB_OK);
		}

		ThrowIfFailed(hr);
		return _Pdest;
	}

	struct shader {
		shader() = default;
		shader(const shader&) = delete;
		shader(shader&& _Right) noexcept : _Mydata(std::move(_Right._Mydata)) {}
		shader(Microsoft::WRL::ComPtr<ID3DBlob>&& _Bytecode) : _Mydata(std::move(_Bytecode)) {}

		shader& operator=(const shader&) = delete;
		shader& operator=(shader&& _Right) noexcept {
			_Mydata = std::move(_Right._Mydata);
			return (*this);
		}
		shader& operator=(Microsoft::WRL::ComPtr<ID3DBlob>&& _Bytecode) {
			_Mydata = std::move(_Bytecode);
			return (*this);
		}

		D3D12_SHADER_BYTECODE get_dx12() const {
			return D3D12_SHADER_BYTECODE{ _Mydata->GetBufferPointer(), _Mydata->GetBufferSize() };
		}

	private:
		Microsoft::WRL::ComPtr<ID3DBlob> _Mydata;
	};

}// namespace hlsl

#endif