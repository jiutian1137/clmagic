#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include <assert.h>
#include <vector>

#include "packaged_comptr.h"
#include "enum_string.h"

namespace d3d12 {

	struct _Rootsignature_impl : public packaged_comptr<ID3D12RootSignature>, public uncopyable {
		static void _Initializer(_Rootsignature_impl& _This, ID3D12Device& _Device, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& _Desc, UINT _Nodemask = 0) {
			Microsoft::WRL::ComPtr<ID3DBlob> _Perror;
			Microsoft::WRL::ComPtr<ID3DBlob> _Pdest;
			D3D12SerializeVersionedRootSignature(&_Desc, _Pdest.GetAddressOf(), _Perror.GetAddressOf());
			assert(_Perror == nullptr);

			HRESULT hr = _Device.CreateRootSignature(_Nodemask, _Pdest->GetBufferPointer(), _Pdest->GetBufferSize(), IID_PPV_ARGS(_This._Impl.GetAddressOf()));
			assert(SUCCEEDED(hr));

			hr = D3D12CreateVersionedRootSignatureDeserializer(_Pdest->GetBufferPointer(), _Pdest->GetBufferSize(), IID_PPV_ARGS(_This._Mydeserializer.GetAddressOf()));
			assert(SUCCEEDED(hr));
		}

		_Rootsignature_impl() = default;
		_Rootsignature_impl(_Rootsignature_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		_Rootsignature_impl& operator=(_Rootsignature_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}

		_Rootsignature_impl(ID3D12Device& _Device,
			const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& _Desc, 
			      UINT                                 _Nodemask = 0) {
			_Initializer(*this, _Device, _Desc, _Nodemask);
		}

		// VERSION_1_0
		_Rootsignature_impl(ID3D12Device& _Device,
			const std::vector<D3D12_ROOT_PARAMETER>&      _Parameters, 
			const std::vector<D3D12_STATIC_SAMPLER_DESC>& _Samplers,
			      D3D12_ROOT_SIGNATURE_FLAGS              _Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT, 
			      UINT                                    _Nodemask = 0) 
		{
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

		// VERSION_1_1
		_Rootsignature_impl(ID3D12Device& _Device,
			const std::vector<D3D12_ROOT_PARAMETER1>&     _Parameters, 
			const std::vector<D3D12_STATIC_SAMPLER_DESC>& _Samplers,
			      D3D12_ROOT_SIGNATURE_FLAGS              _Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT, 
			      UINT                                    _Nodemask = 0) 
		{
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

		ID3D12VersionedRootSignatureDeserializer& deserializer() const {
			assert( valid() );
			return *(_Mydeserializer.Get());
		}

		Microsoft::WRL::ComPtr<ID3D12VersionedRootSignatureDeserializer> _Mydeserializer;
	};

	inline std::string to_string(D3D12_DESCRIPTOR_RANGE _Desc, const std::string& _Prefix = std::string()) {
		std::string _Str;
		_Str  = _Prefix + "D3D12_DESCRIPTOR_RANGE{\n";
		_Str += _Prefix + "\tRangeType: " + enum_string(_Desc.RangeType) + ",\n";
		_Str += _Prefix + "\tNumDescriptors:     " + std::to_string(_Desc.NumDescriptors)     + ",\n";
		_Str += _Prefix + "\tBaseShaderRegister: " + std::to_string(_Desc.BaseShaderRegister) + ",\n";
		_Str += _Prefix + "\tRegisterSpace:      " + std::to_string(_Desc.RegisterSpace)      + ",\n";
		_Str += _Prefix + "\tOffsetInDescriptorsFromTableStart: " + std::to_string(_Desc.OffsetInDescriptorsFromTableStart) + '\n';
		_Str += _Prefix + '}';
		return _Str;
	}

	inline std::string to_string(D3D12_ROOT_DESCRIPTOR_TABLE _Desc, const std::string& _Prefix = std::string()) {
		std::string _Str;
		_Str  = _Prefix + "D3D12_ROOT_DESCRIPTOR_TABLE{\n";
		_Str += _Prefix + "\tNumDescriptorRanges: " + std::to_string(_Desc.NumDescriptorRanges) + ",\n";
		_Str += _Prefix + "\tpDescriptorRanges: {\n";
		for (size_t i = 0; i != _Desc.NumDescriptorRanges; ++i) {
			if (i != _Desc.NumDescriptorRanges - 1) {
			_Str += to_string(_Desc.pDescriptorRanges[i], _Prefix + "\t\t") + ",\n";
			} else {
			_Str += to_string(_Desc.pDescriptorRanges[i], _Prefix + "\t\t") + '\n';
			}
		}
		_Str += _Prefix + "\t}\n";
		_Str += _Prefix + '}';
		return _Str;
	}

	inline std::string to_string(D3D12_ROOT_CONSTANTS _Desc, const std::string& _Prefix = std::string()) {
		std::string _Str;
		_Str  = _Prefix + "D3D12_ROOT_CONSTANTS{\n";
		_Str += _Prefix + "\tShaderRegister: " + std::to_string(_Desc.ShaderRegister) + ",\n";
		_Str += _Prefix + "\tRegisterSpace:  " + std::to_string(_Desc.RegisterSpace)  + ",\n";
		_Str += _Prefix + "\tNum32BitValues: " + std::to_string(_Desc.Num32BitValues) + '\n';
		_Str += _Prefix + '}';
		return _Str;
	}

	inline std::string to_string(D3D12_ROOT_DESCRIPTOR _Desc, const std::string& _Prefix = std::string()) {
		std::string _Str;
		_Str  = _Prefix + "D3D12_ROOT_DESCRIPTOR{\n";
		_Str += _Prefix + "\tShaderRegister: " + std::to_string(_Desc.ShaderRegister) + ",\n";
		_Str += _Prefix + "\tRegisterSpace:  " + std::to_string(_Desc.RegisterSpace)  + '\n';
		_Str += _Prefix + '}';
		return _Str;
	}

	inline std::string to_string(D3D12_ROOT_PARAMETER _Parameter, const std::string& _Prefix = std::string()) {
		switch (_Parameter.ParameterType) {
		case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
			return _Prefix + "D3D12_ROOT_PARAMETER{\n" +
				   _Prefix + "\tDescriptorTable:\n" + to_string(_Parameter.DescriptorTable, _Prefix+"\t\t") + ",\n" +
				   _Prefix + "\tShaderVisibility: " + enum_string(_Parameter.ShaderVisibility) + '\n' + 
				   _Prefix + '}';
		case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
			return _Prefix + "D3D12_ROOT_PARAMETER{\n" +
				   _Prefix + "\tConstants:\n"       + to_string(_Parameter.Constants, _Prefix+"\t\t") + ",\n" +
				   _Prefix + "\tShaderVisibility: " + enum_string(_Parameter.ShaderVisibility) + '\n' + 
				   _Prefix + '}';
		default:
			return _Prefix + "D3D12_ROOT_PARAMETER{\n" +
				   _Prefix + "\tDescriptor:\n"      + to_string(_Parameter.Descriptor, _Prefix+"\t\t") + ",\n" +
				   _Prefix + "\tShaderVisibility: " + enum_string(_Parameter.ShaderVisibility) + '\n' + 
				   _Prefix + '}';
		}
	}

	inline std::string to_string(D3D12_STATIC_SAMPLER_DESC _Desc, const std::string& _Prefix = std::string()) {
		std::string _Str;
		_Str  = _Prefix + "D3D12_STATIC_SAMPLER_DESC{\n";
		_Str += _Prefix + "\tFilter:   " + enum_string(_Desc.Filter) + ",\n";
		_Str += _Prefix + "\tAddressU: " + enum_string(_Desc.AddressU) + ",\n";
		_Str += _Prefix + "\tAddressV: " + enum_string(_Desc.AddressV) + ",\n";
		_Str += _Prefix + "\tAddressW: " + enum_string(_Desc.AddressW) + ",\n";
		_Str += _Prefix + "\tMipLODBias: " + std::to_string(_Desc.MipLODBias) + ",\n";
		_Str += _Prefix + "\tMaxAnisotropy: " + std::to_string(_Desc.MaxAnisotropy) + ",\n";
		_Str += _Prefix + "\tComparisonFunc: " + enum_string(_Desc.ComparisonFunc)  + ",\n";
		_Str += _Prefix + "\tBorderColor: " + enum_string(_Desc.BorderColor) + ",\n";
		_Str += _Prefix + "\tMinLOD: " + std::to_string(_Desc.MinLOD) + ",\n";
		_Str += _Prefix + "\tMaxLOD: " + std::to_string(_Desc.MaxLOD) + ",\n";
		_Str += _Prefix + "\tShaderRegister: " + std::to_string(_Desc.ShaderRegister) + ",\n";
		_Str += _Prefix + "\tRegisterSpace:  " + std::to_string(_Desc.RegisterSpace)  + ",\n";
		_Str += _Prefix + "\tShaderVisibility: " + enum_string(_Desc.ShaderVisibility) + '\n';
		_Str += _Prefix + '}';
		return _Str;
	}

	inline std::string to_string(D3D12_ROOT_SIGNATURE_DESC _Desc, const std::string& _Prefix = std::string()) {
		std::string _Str;
		_Str  = _Prefix + "D3D12_ROOT_SIGNATURE_DESC{\n";
		_Str += _Prefix + "\tNumParameters: " + std::to_string(_Desc.NumParameters) + ",\n";
		
		_Str += _Prefix + "\tpParameters: {\n";
		for (size_t i = 0; i != _Desc.NumParameters; ++i) {
			if (i != _Desc.NumParameters - 1) {
			_Str += to_string(_Desc.pParameters[i], _Prefix+"\t\t") + ",\n";
			} else {
			_Str += to_string(_Desc.pParameters[i], _Prefix+"\t\t") + '\n';
			}
		}
		_Str += _Prefix + "\t},\n";

		_Str += _Prefix + "\tNumStaticSamplers: " + std::to_string(_Desc.NumStaticSamplers) + ",\n";
		_Str += _Prefix + "\tpStaticSamplers: {\n";
		for (size_t i = 0; i != _Desc.NumStaticSamplers; ++i) {
			if (i != _Desc.NumStaticSamplers - 1) {
			_Str += to_string(_Desc.pStaticSamplers[i], _Prefix+"\t\t") + ",\n";
			} else {
			_Str += to_string(_Desc.pStaticSamplers[i], _Prefix+"\t\t") + '\n';
			}
		}
		_Str += _Prefix + "\t},\n";

		_Str += _Prefix + "\tFlags: " + enum_string(_Desc.Flags) + '\n';
		_Str += _Prefix + '}';
		return _Str;
	}

	inline std::string to_string(D3D12_DESCRIPTOR_RANGE1 _Desc, const std::string& _Prefix = std::string()) {
		std::string _Str;
		_Str  = _Prefix + "D3D12_DESCRIPTOR_RANGE{\n";
		_Str += _Prefix + "\tRangeType: " + enum_string(_Desc.RangeType) + ",\n";
		_Str += _Prefix + "\tNumDescriptors:     " + std::to_string(_Desc.NumDescriptors)     + ",\n";
		_Str += _Prefix + "\tBaseShaderRegister: " + std::to_string(_Desc.BaseShaderRegister) + ",\n";
		_Str += _Prefix + "\tRegisterSpace:      " + std::to_string(_Desc.RegisterSpace)      + ",\n";
		_Str += _Prefix + "\tFlags:              " + enum_string(_Desc.Flags) + ",\n";
		_Str += _Prefix + "\tOffsetInDescriptorsFromTableStart: " + std::to_string(_Desc.OffsetInDescriptorsFromTableStart) + '\n';
		_Str += _Prefix + '}';
		return _Str;
	}

	inline std::string to_string(D3D12_ROOT_DESCRIPTOR_TABLE1 _Desc, const std::string& _Prefix = std::string()) {
		std::string _Str;
		_Str  = _Prefix + "D3D12_ROOT_DESCRIPTOR_TABLE{\n";
		_Str += _Prefix + "\tNumDescriptorRanges: " + std::to_string(_Desc.NumDescriptorRanges) + ",\n";
		_Str += _Prefix + "\tpDescriptorRanges: {\n";
		for (size_t i = 0; i != _Desc.NumDescriptorRanges; ++i) {
			if (i != _Desc.NumDescriptorRanges - 1) {
			_Str += to_string(_Desc.pDescriptorRanges[i], _Prefix + "\t\t") + ",\n";
			} else {
			_Str += to_string(_Desc.pDescriptorRanges[i], _Prefix + "\t\t") + '\n';
			}
		}
		_Str += _Prefix + "\t}\n";
		_Str += _Prefix + '}';
		return _Str;
	}

	inline std::string to_string(D3D12_ROOT_DESCRIPTOR1 _Desc, const std::string& _Prefix = std::string()) {
		std::string _Str;
		_Str  = _Prefix + "D3D12_ROOT_DESCRIPTOR1{\n";
		_Str += _Prefix + "\tShaderRegister: " + std::to_string(_Desc.ShaderRegister) + ",\n";
		_Str += _Prefix + "\tRegisterSpace:  " + std::to_string(_Desc.RegisterSpace)  + ",\n";
		_Str += _Prefix + "\tFlags:  " + enum_string(_Desc.Flags)  + '\n';
		_Str += _Prefix + '}';
		return _Str;
	}

	inline std::string to_string(D3D12_ROOT_PARAMETER1 _Parameter, const std::string& _Prefix = std::string()) {
		switch (_Parameter.ParameterType) {
		case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
			return _Prefix + "D3D12_ROOT_PARAMETER{\n" +
				   _Prefix + "\tDescriptorTable:\n" + to_string(_Parameter.DescriptorTable, _Prefix+"\t\t") + ",\n" +
				   _Prefix + "\tShaderVisibility: " + enum_string(_Parameter.ShaderVisibility) + '\n' + 
				   _Prefix + '}';
		case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
			return _Prefix + "D3D12_ROOT_PARAMETER{\n" +
				   _Prefix + "\tConstants:\n"       + to_string(_Parameter.Constants, _Prefix+"\t\t") + ",\n" +
				   _Prefix + "\tShaderVisibility: " + enum_string(_Parameter.ShaderVisibility) + '\n' + 
				   _Prefix + '}';
		default:
			return _Prefix + "D3D12_ROOT_PARAMETER{\n" +
				   _Prefix + "\tDescriptor:\n"      + to_string(_Parameter.Descriptor, _Prefix+"\t\t") + ",\n" +
				   _Prefix + "\tShaderVisibility: " + enum_string(_Parameter.ShaderVisibility) + '\n' + 
				   _Prefix + '}';
		}
	}

	inline std::string to_string(D3D12_ROOT_SIGNATURE_DESC1 _Desc, const std::string& _Prefix = std::string()) {
		std::string _Str;
		_Str  = _Prefix + "D3D12_ROOT_SIGNATURE_DESC{\n";
		_Str += _Prefix + "\tNumParameters: " + std::to_string(_Desc.NumParameters) + ",\n";
		
		_Str += _Prefix + "\tpParameters: {\n";
		for (size_t i = 0; i != _Desc.NumParameters; ++i) {
			if (i != _Desc.NumParameters - 1) {
			_Str += to_string(_Desc.pParameters[i], _Prefix+"\t\t") + ",\n";
			} else {
			_Str += to_string(_Desc.pParameters[i], _Prefix+"\t\t") + '\n';
			}
		}
		_Str += _Prefix + "\t},\n";

		_Str += _Prefix + "\tNumStaticSamplers: " + std::to_string(_Desc.NumStaticSamplers) + ",\n";
		_Str += _Prefix + "\tpStaticSamplers: {\n";
		for (size_t i = 0; i != _Desc.NumStaticSamplers; ++i) {
			if (i != _Desc.NumStaticSamplers - 1) {
			_Str += to_string(_Desc.pStaticSamplers[i], _Prefix+"\t\t") + ",\n";
			} else {
			_Str += to_string(_Desc.pStaticSamplers[i], _Prefix+"\t\t") + '\n';
			}
		}
		_Str += _Prefix + "\t},\n";

		_Str += _Prefix + "\tFlags: " + enum_string(_Desc.Flags) + '\n';
		_Str += _Prefix + '}';
		return _Str;
	}

	inline std::string to_string(ID3D12VersionedRootSignatureDeserializer& _Rtsign) {
		const auto _Desc = _Rtsign.GetUnconvertedRootSignatureDesc();
		std::string _Str;
		_Str  = "D3D12_VERSIONED_ROOT_SIGNATURE_DESC{\n";
		_Str += "\tVersion: " + enum_string(_Desc->Version) + ",\n";
		if (_Desc->Version == D3D_ROOT_SIGNATURE_VERSION_1_0) {
			_Str += "\tDesc_1_0: {\n";
			_Str += to_string(_Desc->Desc_1_0, "\t\t") + '\n';
			_Str += "\t}";
		} else {
			_Str += "\tDesc_1_1: {\n";
			_Str += to_string(_Desc->Desc_1_1, "\t\t") + '\n';
			_Str += "\t}";
		}
		_Str += '}';
		return _Str;
	}

}// namespace DX12