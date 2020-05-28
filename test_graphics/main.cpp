#include "../src/clmagic/basic/timer_wheel.h"
#include "../src/clmagic/basic/concurrent_resource.h"
#include "../src/clmagic/math.h"
#include "../src/clmagic/directX12/directX12.h"
#include <iostream>
#include <chrono>
#include <map>
#include <DirectXColors.h>
#include "Common/d3dApp.h"
#include "Common/GeometryGenerator.h"
#include "Waves.h"
#include "Inc/DDSTextureLoader.h"
#include "assimp/scene.h"
#include "assimp/cexport.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#pragma comment(lib, "lib/assimp-vc142-mt.lib")
#include "opencv/opencv.hpp"
#pragma comment(lib, "lib/opencv_world345d.lib")

using namespace::clmagic;

struct PER_OBJECT {
	matrix4x4<float> Mworld;
};

struct PER_FRAME {
	matrix4x4<float> Mview;
	matrix4x4<float> MProj;
	vector3<float>   Peye;
	float __pand__;
	vector2<float>   Resolution;
	float            TotalTime;
	float            DeltaTime;
};

struct PER_MATERIAL {
	surface<float>      Mtl;
};

struct RENDERING {
	environment<float>  Env;
	light_source<float> Lights[3];
};

struct VInput {
	vector3<float> POSITION0;
	vector3<float> NORMAL0;
	vector2<float> TEXCOORD0;
};

struct FrameResource : public d3d12::Basic_Frame_Resource {
	FrameResource() = default;

	FrameResource(ID3D12Device& _Device, D3D12_COMMAND_LIST_TYPE _Type)
		: d3d12::Basic_Frame_Resource(_Device, _Type) {
		// single descriptor
		cb_PER_FRAME = d3d12::dynamic_buffer<PER_FRAME>(_Device, 1);
		cb_RENDERING = d3d12::dynamic_buffer<RENDERING>(_Device, 1);
	}

	d3d12::dynamic_buffer<PER_FRAME> cb_PER_FRAME;// b0
	d3d12::dynamic_buffer<RENDERING> cb_RENDERING;// b3
};



template<typename _Ty, typename _Rs>
struct surface_trait : public surface<_Ty> {
	using _Mysurface = surface<_Ty>;

	_Rs*   _GPU_resource;
	size_t _GPU_resource_index;
};
#define SURFACE_TRAIT surface_trait< float, d3d12::static_buffer<surface<float>> >

template<typename _Ty, typename _Rsv, typename _Rsi>
struct basic_shape {
	_Ty    type;
	_Rsv*  _GPU_vertex_resource;
	_Rsi*  _GPU_index_resource;
	size_t base_vertex_location;
	size_t start_index_location;
	size_t index_count;
};
#define SHAPE0 basic_shape<D3D_PRIMITIVE_TOPOLOGY, d3d12::static_buffer<VInput>, d3d12::static_buffer<uint32_t>>
#define SHAPE1 basic_shape<D3D_PRIMITIVE_TOPOLOGY, d3d12::dynamic_buffer<VInput>, d3d12::static_buffer<uint32_t>>

template<typename _Traits, typename _Shapes>
struct renderable {
	matrix4x4<float, __m128> _Myworld;
	_Traits* _Mytrait;
	_Shapes* _Myshape;
};
#define RENDERABLE0 renderable<SURFACE_TRAIT, SHAPE0>
#define RENDERABLE1 renderable<SURFACE_TRAIT, SHAPE1>

d3d12::static_buffer< surface<float> > GPU_static_traits;
d3d12::static_buffer< VInput >         GPU_static_vertices;
d3d12::static_buffer< uint32_t >       GPU_static_indices;
d3d12::dynamic_buffer< VInput >        GPU_dynamic_vertices;
//DX12::Static_Texture2D<DXGI_FORMAT_R32G32B32_FLOAT> GPU_static_textures;
Microsoft::WRL::ComPtr<ID3D12Resource> GPU_static_textures;
std::map<std::string, SURFACE_TRAIT> CPU_static_traits;
std::map<std::string, SHAPE0>        CPU_static_shapes;
std::map<std::string, SHAPE1>        CPU_dynamic_vertex_shapes;
std::map<std::string, RENDERABLE0> CPU_static_Actors;
std::map<std::string, RENDERABLE1> CPU_dynamic_vertex_Actors;
timer_wheel<FrameResource> TW_frame_resource;



void FlushCommandQueue(ID3D12CommandQueue& _Cmdqueue, ID3D12Fence& _Fence, UINT64& _Fence_val) {
	// Advance the fence value to mark commands up to this fence point.
	_Fence_val++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	HRESULT hr = _Cmdqueue.Signal(&_Fence, _Fence_val);
	assert(SUCCEEDED(hr));

	// Wait until the GPU has completed commands up to this fence point.
	if (_Fence.GetCompletedValue() < _Fence_val) {
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		_Fence.SetEventOnCompletion(_Fence_val, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);

		CloseHandle(eventHandle);
	}
}

class ShapeApp : public D3DApp {
	using _Mybase = D3DApp;

	d3d12::fence _Myfence;
public:
	ShapeApp(HINSTANCE hInstance) : _Mybase(hInstance) {}

	virtual bool Initialize() override {
		if (!_Mybase::Initialize())
			return false;

		size_t _Nframe = 1;

		_Myfence = d3d12::fence(mFence.Get());

		// 1.
		//gWaves = Waves(256, 256, 1.f, 0.03f, 6.0f, 0.2f);

		for (size_t i = 0; i != _Nframe; ++i) {
			TW_frame_resource.expend_cycle(FrameResource(*md3dDevice.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT));
		}


		CD3DX12_DESCRIPTOR_RANGE _Range0;
		_Range0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		std::vector<D3D12_ROOT_PARAMETER> _Parameters(5);
		CD3DX12_ROOT_PARAMETER::InitAsConstantBufferView(_Parameters[0], 0);
		CD3DX12_ROOT_PARAMETER::InitAsConstants(_Parameters[1], 16, 1);
		CD3DX12_ROOT_PARAMETER::InitAsConstantBufferView(_Parameters[2], 2);
		CD3DX12_ROOT_PARAMETER::InitAsConstantBufferView(_Parameters[3], 3);
		CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(_Parameters[4], 1, &_Range0);

		std::vector<D3D12_STATIC_SAMPLER_DESC> _Samplers(1);
		_Samplers[0] = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_ANISOTROPIC);

		gRootSignature = d3d12::shader_signature(*md3dDevice.Get(), _Parameters, _Samplers);

		descriptor_heap = d3d12::descriptor_heap(*md3dDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

		Microsoft::WRL::ComPtr<ID3D12Resource> _Temp_upload;
		mDirectCmdListAlloc->Reset();
		mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr);
		DirectX::LoadDDSTextureFromFile(*md3dDevice.Get(), L"media/WoodCrate01.dds",)
		DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(), mCommandList.Get(), L"media/WoodCrate01.dds", GPU_static_textures, _Temp_upload);
		mCommandList->Close();
		_Myfence.flush(*mCommandQueue.Get(), *mCommandList.Get(), mCurrentFence);

		D3D12_SHADER_RESOURCE_VIEW_DESC _Desc;
		_Desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		_Desc.Format = GPU_static_textures->GetDesc().Format;
		_Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		_Desc.Texture2D.MostDetailedMip = 0;
		_Desc.Texture2D.MipLevels = GPU_static_textures->GetDesc().MipLevels;
		_Desc.Texture2D.ResourceMinLODClamp = 0.f;
		_Desc.Texture2D.PlaneSlice = 0;
		md3dDevice->CreateShaderResourceView(GPU_static_textures.Get(), &_Desc, descriptor_heap.cpu_handle(0));

		d3d12::shader_input _Varyings;
		_Varyings.push_back("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
		_Varyings.push_back("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
		_Varyings.push_back("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);

		gShaders["standardVS"] = hlsl::shader_compile(L"shader/norm.hlsl", nullptr, "VS", "vs_5_0");
		gShaders["standardPS"] = hlsl::shader_compile(L"shader/norm.hlsl", nullptr, "PS", "ps_5_0");

		gPipelineStates["opaque"] = d3d12::shader_program(*md3dDevice.Get(), gRootSignature.ref(), _Varyings.get(), 
			gShaders["standardVS"].get_dx12(), gShaders["standardPS"].get_dx12(),
			mBackBufferFormat, mDepthStencilFormat, 
			DXGI_SAMPLE_DESC{ UINT(m4xMsaaState ? 4 : 1), UINT(m4xMsaaState ? (m4xMsaaQuality - 1) : 0) });

		build_geometry();
		build_material();
		build_lights();
		build_object();

		return true;
	}

	void build_geometry() {
		GeometryGenerator geoGen;
		GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
		GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
		GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
		GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

		//
		// We are concatenating all the geometry into one big vertex/index buffer.  So
		// define the regions in the buffer each submesh covers.
		//

		// Cache the vertex offsets to each object in the concatenated vertex buffer.
		UINT boxVertexOffset = 0;
		UINT gridVertexOffset = (UINT)box.Vertices.size();
		UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
		UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

		// Cache the starting index for each object in the concatenated index buffer.
		UINT boxIndexOffset = 0;
		UINT gridIndexOffset = (UINT)box.Indices32.size();
		UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
		UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

		//
		// Extract the vertex elements we are interested in and pack the
		// vertices of all the meshes into one vertex buffer.
		//

		auto totalVertexCount =
			box.Vertices.size() +
			grid.Vertices.size() +
			sphere.Vertices.size() +
			cylinder.Vertices.size();

		std::vector<VInput> _Static_vertices(totalVertexCount);
		UINT k = 0;
		for (size_t i = 0; i < box.Vertices.size(); ++i, ++k) {
			_Static_vertices[k].POSITION0 = reinterpret_cast<vector3<float>&>(box.Vertices[i].Position);
			_Static_vertices[k].NORMAL0   = reinterpret_cast<vector3<float>&>(box.Vertices[i].Normal);
			_Static_vertices[k].TEXCOORD0 = reinterpret_cast<vector2<float>&>(box.Vertices[i].TexC);
		}

		for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k) {
			_Static_vertices[k].POSITION0 = reinterpret_cast<vector3<float>&>(grid.Vertices[i].Position);
			_Static_vertices[k].NORMAL0   = reinterpret_cast<vector3<float>&>(grid.Vertices[i].Normal);
			_Static_vertices[k].TEXCOORD0 = reinterpret_cast<vector2<float>&>(grid.Vertices[i].TexC);
		}

		for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k) {
			_Static_vertices[k].POSITION0 = reinterpret_cast<vector3<float>&>(sphere.Vertices[i].Position);
			_Static_vertices[k].NORMAL0   = reinterpret_cast<vector3<float>&>(sphere.Vertices[i].Normal);
			_Static_vertices[k].TEXCOORD0 = reinterpret_cast<vector2<float>&>(sphere.Vertices[i].TexC);
		}

		for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k) {
			_Static_vertices[k].POSITION0 = reinterpret_cast<vector3<float>&>(cylinder.Vertices[i].Position);
			_Static_vertices[k].NORMAL0   = reinterpret_cast<vector3<float>&>(cylinder.Vertices[i].Normal);
			_Static_vertices[k].TEXCOORD0 = reinterpret_cast<vector2<float>&>(cylinder.Vertices[i].TexC);
		}

		std::vector<uint32_t> _Static_indices;
		_Static_indices.insert(_Static_indices.end(), box.Indices32.begin(), box.Indices32.end());
		_Static_indices.insert(_Static_indices.end(), grid.Indices32.begin(), grid.Indices32.end());
		_Static_indices.insert(_Static_indices.end(), sphere.Indices32.begin(), sphere.Indices32.end());
		_Static_indices.insert(_Static_indices.end(), cylinder.Indices32.begin(), cylinder.Indices32.end());

		{
			auto&   _Source        = _Static_vertices;
			GPU_static_vertices    = d3d12::static_buffer<VInput>(*md3dDevice.Get(), _Source.size());
			auto&   _Dest          = GPU_static_vertices;
			auto    _Intermediator = d3d12::make_dynamic_buffer(*md3dDevice.Get(), _Source);

			mDirectCmdListAlloc->Reset();
			mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr);
			mCommandList->ResourceBarrier(1, &_Dest.transition(D3D12_RESOURCE_STATE_COPY_DEST));
			mCommandList->CopyBufferRegion(_Dest.get(), 0, _Intermediator.get(), 0, _Intermediator.capacity());
			mCommandList->ResourceBarrier(1, &_Dest.transition(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
			mCommandList->Close();
			_Myfence.flush(*mCommandQueue.Get(), *mCommandList.Get(), mCurrentFence);
			
			_Source.clear();
			_Intermediator.release();
		}

		{
			auto&     _Source        = _Static_indices;
			GPU_static_indices       = d3d12::static_buffer<uint32_t>(*md3dDevice.Get(), _Source.size());
			auto&     _Dest          = GPU_static_indices;
			auto      _Intermediator = d3d12::make_dynamic_buffer(*md3dDevice.Get(), _Source);
			mDirectCmdListAlloc->Reset();
			mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr);
			mCommandList->ResourceBarrier(1, &_Dest.transition(D3D12_RESOURCE_STATE_COPY_DEST));
			mCommandList->CopyBufferRegion(_Dest.get(), 0, _Intermediator.get(), 0, _Intermediator.capacity());
			mCommandList->ResourceBarrier(1, &_Dest.transition(D3D12_RESOURCE_STATE_INDEX_BUFFER));
			mCommandList->Close();
			_Myfence.flush(*mCommandQueue.Get(), *mCommandList.Get(), mCurrentFence);
			
			_Source.clear();
			_Intermediator.release();
		}

		CPU_static_shapes["Box"].type = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		CPU_static_shapes["Box"]._GPU_vertex_resource = &GPU_static_vertices;
		CPU_static_shapes["Box"].base_vertex_location = boxVertexOffset;
		CPU_static_shapes["Box"]._GPU_index_resource  = &GPU_static_indices;
		CPU_static_shapes["Box"].start_index_location = boxIndexOffset;
		CPU_static_shapes["Box"].index_count          = box.Indices32.size();

		CPU_static_shapes["Grid"].type = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		CPU_static_shapes["Grid"]._GPU_vertex_resource = &GPU_static_vertices;
		CPU_static_shapes["Grid"].base_vertex_location = gridVertexOffset;
		CPU_static_shapes["Grid"]._GPU_index_resource  = &GPU_static_indices;
		CPU_static_shapes["Grid"].start_index_location = gridIndexOffset;
		CPU_static_shapes["Grid"].index_count          = grid.Indices32.size();

		CPU_static_shapes["Sphere"].type = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		CPU_static_shapes["Sphere"]._GPU_vertex_resource = &GPU_static_vertices;
		CPU_static_shapes["Sphere"].base_vertex_location = sphereVertexOffset;
		CPU_static_shapes["Sphere"]._GPU_index_resource  = &GPU_static_indices;
		CPU_static_shapes["Sphere"].start_index_location = sphereIndexOffset;
		CPU_static_shapes["Sphere"].index_count          = sphere.Indices32.size();

		CPU_static_shapes["Cylinder"].type = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		CPU_static_shapes["Cylinder"]._GPU_vertex_resource = &GPU_static_vertices;
		CPU_static_shapes["Cylinder"].base_vertex_location = cylinderVertexOffset;
		CPU_static_shapes["Cylinder"]._GPU_index_resource  = &GPU_static_indices;
		CPU_static_shapes["Cylinder"].start_index_location = cylinderIndexOffset;
		CPU_static_shapes["Cylinder"].index_count          = cylinder.Indices32.size();
	}

	void build_material() {
		std::vector<surface<float>> static_traits;

		// chemical element of atomic number 22
		CPU_static_traits["Titanium"].roughness_x       = 0.f;
		CPU_static_traits["Titanium"].roughness_y       = 0.f;
		CPU_static_traits["Titanium"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Titanium"].reflect_value     = { 0.542f, 0.497f, 0.449f };
		CPU_static_traits["Titanium"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Titanium"]._GPU_resource_index = 0;
		static_traits.push_back( CPU_static_traits["Titanium"] );

		// chemical element of atomic number 24
		CPU_static_traits["Chromium"].roughness_x       = 0.f;
		CPU_static_traits["Chromium"].roughness_y       = 0.f;
		CPU_static_traits["Chromium"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Chromium"].reflect_value     = { 0.549f, 0.556f, 0.554f };
		CPU_static_traits["Chromium"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Chromium"]._GPU_resource_index = 1;
		static_traits.push_back( CPU_static_traits["Chromium"] );

		// chemical element of atomic number 26
		CPU_static_traits["Iron"].roughness_x       = 0.f;
		CPU_static_traits["Iron"].roughness_y       = 0.f;
		CPU_static_traits["Iron"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Iron"].reflect_value     = { 0.562f, 0.565f, 0.578f };
		CPU_static_traits["Iron"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Iron"]._GPU_resource_index = 2;
		static_traits.push_back( CPU_static_traits["Iron"] );

		constexpr auto _FFFF = 0.578f * 256;

		// chemical elelemtn of atomic number 28
		CPU_static_traits["Nickel"].roughness_x       = 0.f;
		CPU_static_traits["Nickel"].roughness_y       = 0.f;
		CPU_static_traits["Nickel"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Nickel"].reflect_value     = { 0.660f, 0.609f, 0.526f };
		CPU_static_traits["Nickel"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Nickel"]._GPU_resource_index = 3;
		static_traits.push_back( CPU_static_traits["Nickel"] );

		// chemical element of atomic number 78
		CPU_static_traits["Platinum"].roughness_x       = 0.f;
		CPU_static_traits["Platinum"].roughness_y       = 0.f;
		CPU_static_traits["Platinum"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Platinum"].reflect_value     = { 0.673f, 0.637f, 0.585f };
		CPU_static_traits["Platinum"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Platinum"]._GPU_resource_index = 4;
		static_traits.push_back( CPU_static_traits["Platinum"] );

		// chemical element of atomic number 29
		CPU_static_traits["Copper"].roughness_x       = 0.f;
		CPU_static_traits["Copper"].roughness_y       = 0.f;
		CPU_static_traits["Copper"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Copper"].reflect_value     = { 0.955f, 0.638f, 0.538f };
		CPU_static_traits["Copper"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Copper"]._GPU_resource_index = 5;
		static_traits.push_back( CPU_static_traits["Copper"] );

		CPU_static_traits["Palladium"].roughness_x       = 0.f;
		CPU_static_traits["Palladium"].roughness_y       = 0.f;
		CPU_static_traits["Palladium"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Palladium"].reflect_value     = { 0.733f, 0.697f, 0.652f };
		CPU_static_traits["Palladium"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Palladium"]._GPU_resource_index = 6;
		static_traits.push_back( CPU_static_traits["Palladium"] );

		CPU_static_traits["Mercury"].roughness_x       = 0.f;
		CPU_static_traits["Mercury"].roughness_y       = 0.f;
		CPU_static_traits["Mercury"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Mercury"].reflect_value     = { 0.781f, 0.780f, 0.778f };
		CPU_static_traits["Mercury"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Mercury"]._GPU_resource_index = 7;
		static_traits.push_back( CPU_static_traits["Mercury"] );

		CPU_static_traits["Brass"].roughness_x       = 0.f;
		CPU_static_traits["Brass"].roughness_y       = 0.f;
		CPU_static_traits["Brass"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Brass"].reflect_value     = { 0.910f, 0.778f, 0.423f };
		CPU_static_traits["Brass"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Brass"]._GPU_resource_index = 8;
		static_traits.push_back( CPU_static_traits["Brass"] );

		// chemical element of atomic number 30
		CPU_static_traits["Zinc"].roughness_x       = 0.f;
		CPU_static_traits["Zinc"].roughness_y       = 0.f;
		CPU_static_traits["Zinc"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Zinc"].reflect_value     = { 0.664f, 0.824f, 0.850f };
		CPU_static_traits["Zinc"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Zinc"]._GPU_resource_index = 9;
		static_traits.push_back( CPU_static_traits["Zinc"] );

		// chemical element of atomic number 79
		CPU_static_traits["Gold"].roughness_x       = 0.f;
		CPU_static_traits["Gold"].roughness_y       = 0.f;
		CPU_static_traits["Gold"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Gold"].reflect_value     = { 1.000f, 0.782f, 0.344f };
		CPU_static_traits["Gold"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Gold"]._GPU_resource_index = 10;
		static_traits.push_back( CPU_static_traits["Gold"] );

		// chemical element of atomic number 13
		CPU_static_traits["Aluminum"].roughness_x       = 0.f;
		CPU_static_traits["Aluminum"].roughness_y       = 0.f;
		CPU_static_traits["Aluminum"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Aluminum"].reflect_value     = { 0.913f, 0.922f, 0.924f };
		CPU_static_traits["Aluminum"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Aluminum"]._GPU_resource_index = 11;
		static_traits.push_back( CPU_static_traits["Aluminum"] );

		// chemical element of atomic number 47
		CPU_static_traits["Silver"].roughness_x       = 0.f;
		CPU_static_traits["Silver"].roughness_y       = 0.f;
		CPU_static_traits["Silver"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Silver"].reflect_value     = { 0.972f, 0.960f, 0.915f };
		CPU_static_traits["Silver"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Silver"]._GPU_resource_index = 12;
		static_traits.push_back( CPU_static_traits["Silver"] );

		CPU_static_traits["Diamond"].roughness_x       = 0.f;
		CPU_static_traits["Diamond"].roughness_y       = 0.f;
		CPU_static_traits["Diamond"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Diamond"].reflect_value     = { 0.171f, 0.172f, 0.176f };
		CPU_static_traits["Diamond"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Diamond"]._GPU_resource_index = 13;
		static_traits.push_back( CPU_static_traits["Diamond"] );

		// chemical element of atomic number 14
		CPU_static_traits["Silicon"].roughness_x       = 0.f;
		CPU_static_traits["Silicon"].roughness_y       = 0.f;
		CPU_static_traits["Silicon"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
		CPU_static_traits["Silicon"].reflect_value     = { 0.345f, 0.369f, 0.426f };
		CPU_static_traits["Silicon"]._GPU_resource     = &GPU_static_traits;
		CPU_static_traits["Silicon"]._GPU_resource_index = 14;
		static_traits.push_back(CPU_static_traits["Diamond"]);

		auto& _Source        = static_traits;
		GPU_static_traits    = d3d12::static_buffer<surface<float>>(*md3dDevice.Get(), _Source.size());
		auto& _Dest          = GPU_static_traits;
		auto  _Intermediator = d3d12::make_dynamic_buffer(*md3dDevice.Get(), _Source);
		
		mDirectCmdListAlloc->Reset();
		mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr);
		mCommandList->ResourceBarrier(1, &_Dest.transition(D3D12_RESOURCE_STATE_COPY_DEST));
		mCommandList->CopyBufferRegion(_Dest.get(), 0, _Intermediator.get(), 0, _Intermediator.capacity());
		mCommandList->ResourceBarrier(1, &_Dest.transition(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
		mCommandList->Close();
		_Myfence.flush(*mCommandQueue.Get(), *mCommandList.Get(), mCurrentFence);
	}
	
	void build_lights() {
		for (size_t i = 0 ; i != TW_frame_resource.cycle(); ++i, TW_frame_resource.turn()) {
			RENDERING* _Ptr = nullptr;
			TW_frame_resource->cb_RENDERING->Map(0, nullptr, reinterpret_cast<void**>(&_Ptr));
			_Ptr->Env.particles_ratio = 0.09f;
			_Ptr->Env.particles_color = {0.f, 0.f, 0.f};

			_Ptr->Lights[0].color     = { 0.7f, 0.7f, 0.7f };
			_Ptr->Lights[0].intensity = 100.f;
			_Ptr->Lights[0].position  = { 0.f, 30.f, 0.f };
			_Ptr->Lights[0].direction = { 0.f, -1.f, 0.f };
			_Ptr->Lights[0].penumbra  = 3.14f;
			_Ptr->Lights[0].umbra     = 3.14f;

			_Ptr->Lights[1].color     = { 1.f, 1.f, 1.f };
			_Ptr->Lights[1].intensity = 50.f;
			_Ptr->Lights[1].position  = { -20.f, 2.f, 0.f };
			_Ptr->Lights[1].direction = { 1.f, 0.f, 0.f };
			_Ptr->Lights[1].penumbra  = 3.14f;
			_Ptr->Lights[1].umbra     = 3.14f;

			_Ptr->Lights[2].color     = { 1.f, 1.f, 1.f };
			_Ptr->Lights[2].intensity = 50.f;
			_Ptr->Lights[2].position  = { 0.f, 2.f, -10.f };
			_Ptr->Lights[2].direction = { 0.f, 0.f, 1.f };
			_Ptr->Lights[2].penumbra  = 3.14f;
			_Ptr->Lights[2].umbra     = 3.14f;
			TW_frame_resource->cb_RENDERING->Unmap(0, nullptr);
		}
	}
	
	void build_object() {
		using matrix4x4 = clmagic::matrix4x4<float, __m128>;

		CPU_static_Actors["box"]._Myworld = translation<matrix4x4>::get_matrix(0.f, 1.5f, 11.f) * scaling<matrix4x4>::get_matrix(2.f);
		CPU_static_Actors["box"]._Myworld = matrix4x4(1.f);
		CPU_static_Actors["box"]._Myshape = &CPU_static_shapes["Box"];
		CPU_static_Actors["box"]._Mytrait = &CPU_static_traits["Copper"];

		CPU_static_Actors["grid"]._Myworld = matrix4x4(1.f);
		CPU_static_Actors["grid"]._Myshape = &CPU_static_shapes["Grid"];
		CPU_static_Actors["grid"]._Mytrait = &CPU_static_traits["Iron"];

		for (int i = 0; i < 5; ++i) {
			CPU_static_Actors["leftCyl" + std::to_string(i)]._Myworld = translation<matrix4x4>::get_matrix(-5.0f, 1.5f, -10.0f + i * 5.0f);
			CPU_static_Actors["leftCyl" + std::to_string(i)]._Myshape = &CPU_static_shapes["Cylinder"];
			CPU_static_Actors["leftCyl" + std::to_string(i)]._Mytrait = &CPU_static_traits["Aluminum"];

			CPU_static_Actors["rightCyl" + std::to_string(i)]._Myworld = translation<matrix4x4>::get_matrix(+5.0f, 1.5f, -10.0f + i * 5.0f);
			CPU_static_Actors["rightCyl" + std::to_string(i)]._Myshape = &CPU_static_shapes["Cylinder"];
			CPU_static_Actors["rightCyl" + std::to_string(i)]._Mytrait = &CPU_static_traits["Gold"];

			CPU_static_Actors["leftSph" + std::to_string(i)]._Myworld = translation<matrix4x4>::get_matrix(-5.0f, 3.f, -10.0f + i * 5.0f);
			CPU_static_Actors["leftSph" + std::to_string(i)]._Myshape = &CPU_static_shapes["Sphere"];
			CPU_static_Actors["leftSph" + std::to_string(i)]._Mytrait = &CPU_static_traits["Silver"];

			CPU_static_Actors["rightSph" + std::to_string(i)]._Myworld = translation<matrix4x4>::get_matrix(+5.0f, 3.f, -10.0f + i * 5.0f);
			CPU_static_Actors["rightSph" + std::to_string(i)]._Myshape = &CPU_static_shapes["Sphere"];
			CPU_static_Actors["rightSph" + std::to_string(i)]._Mytrait = &CPU_static_traits["Silver"];
		}
	}

	virtual void OnResize() override {
		_Mybase::OnResize();
		_Mycamera.reset_perspective(radians<float>(degrees<float>(40)), AspectRatio(), 3.f, 1000.f);
	}

	virtual void Update(const GameTimer& gt) override {
		using _Ts = float;
		using _Tb = __m128;
		using unit_vector3 = clmagic::unit_vector3<float, __m128>;
		using matrix4x4    = clmagic::matrix4x4<float, __m128>;
		

		TW_frame_resource.turn();// !!!!!
		if (TW_frame_resource->fence != 0 && mFence->GetCompletedValue() < TW_frame_resource->fence) {
			HANDLE _Event = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
			mFence->SetEventOnCompletion(TW_frame_resource->fence, _Event);
			WaitForSingleObject(_Event, INFINITE);
			CloseHandle(_Event);
		}

		PER_FRAME* _Cbpass = nullptr;
		TW_frame_resource->cb_PER_FRAME->Map(0, nullptr, reinterpret_cast<void**>(&_Cbpass));
		_Cbpass->Mview  = matrix_cast<clmagic::matrix4x4<float>>(transpose(_Mycamera.view_matrix()));
		_Cbpass->MProj  = matrix_cast<clmagic::matrix4x4<float>>(transpose(_Mycamera.projection_matrix()));
		_Cbpass->Peye   = vector_cast<clmagic::vector<float,3>>(_Mycamera.eye_position());
		_Cbpass->DeltaTime   = gt.DeltaTime();
		_Cbpass->TotalTime   = gt.TotalTime();
		TW_frame_resource->cb_PER_FRAME->Unmap(0, nullptr);
		
		/*MessageBoxA(nullptr, to_string(_Pass.view_matrix).c_str(), "view", MB_OK);
		MessageBoxA(nullptr, to_string(_Pass.proj_matrix).c_str(), "view", MB_OK);*/

		// Every quarter second, generate a random wave.
		//static float t_base = 0.0f;
		//if ((mTimer.TotalTime() - t_base) >= 0.25f) {
		//	t_base += 0.25f;

		//	int i = MathHelper::Rand(4, gWaves.RowCount() - 5);
		//	int j = MathHelper::Rand(4, gWaves.ColumnCount() - 5);

		//	float r = MathHelper::RandF(0.2f, 0.5f);

		//	gWaves.Disturb(i, j, r);
		//}

		//// Update the wave simulation.
		//gWaves.Update(gt.DeltaTime());

		//// Update the wave vertex buffer with the new solution.
		//VInput* _Ptr2 = nullptr;
		//auto& _Wave_vertex_resource = *(CPU_dynamic_vertex_shapes["wave"]._GPU_vertex_resource);
		//      _Wave_vertex_resource->Map(0, nullptr, reinterpret_cast<void**>(&_Ptr2));
		//for (int i = 0; i < gWaves.VertexCount(); ++i, ++_Ptr2) {
		//	_Ptr2->POSITION0 = reinterpret_cast<const vector<float, 3>&>(gWaves.Position(i));
		//	_Ptr2->NORMAL0   = reinterpret_cast<const vector<float, 3>&>(gWaves.Normal(i));
		//}
		//_Wave_vertex_resource->Unmap(0, nullptr);
	}

	virtual void Draw(const GameTimer& gt) override {
		auto _Cmdlist_alloc = TW_frame_resource->command_allocator_ptr();
		_Cmdlist_alloc->Reset();
		//mCommandList->Reset(_Cmdlist_alloc, gPipelineStates["opaque_wireframe"].get());
		mCommandList->Reset(_Cmdlist_alloc, gPipelineStates["opaque"].get());
		
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		
		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		ID3D12DescriptorHeap* descriptorHeaps[] = { descriptor_heap.get() };
		mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		mCommandList->SetGraphicsRootSignature(gRootSignature.get());
		mCommandList->SetGraphicsRootConstantBufferView(0, TW_frame_resource->cb_PER_FRAME->GetGPUVirtualAddress());
		mCommandList->SetGraphicsRootConstantBufferView(3, TW_frame_resource->cb_RENDERING->GetGPUVirtualAddress());

		{// draw
			auto       _First    = CPU_static_Actors.begin();
			const auto _Last     = CPU_static_Actors.end();
			for (; _First != _Last; ++_First) {

				mCommandList->SetGraphicsRoot32BitConstants(1, 16, transpose(_First->second._Myworld).ptr(), 0);

				auto& _Shape = *(_First->second._Myshape);
				mCommandList->IASetPrimitiveTopology(_Shape.type);
				mCommandList->IASetVertexBuffers(0, 1, &(_Shape._GPU_vertex_resource->as_D3D12_VERTEX_BUFFER_VIEW()));
				mCommandList->IASetIndexBuffer(        &(_Shape._GPU_index_resource->as_D3D12_INDEX_BUFFER_VIEW()));

				auto& _Trait = *(_First->second._Mytrait);
				mCommandList->SetGraphicsRootConstantBufferView(2, _Trait._GPU_resource->get()->GetGPUVirtualAddress() + sizeof(surface<float>) * _Trait._GPU_resource_index);
				mCommandList->SetGraphicsRootDescriptorTable(4, descriptor_heap.gpu_handle(0));

				mCommandList->DrawIndexedInstanced(_Shape.index_count, 1, _Shape.start_index_location, _Shape.base_vertex_location, 0);
			}
		}
		{// same above
			auto       _First    = CPU_dynamic_vertex_Actors.begin();
			const auto _Last     = CPU_dynamic_vertex_Actors.end();
			for (; _First != _Last; ++_First) {
				auto& _Trait = *(_First->second._Mytrait);
				auto& _Shape = *(_First->second._Myshape);

				mCommandList->IASetPrimitiveTopology(_Shape.type);
				mCommandList->SetGraphicsRoot32BitConstants(1, 16, transpose(_First->second._Myworld).ptr(), 0);
				mCommandList->SetGraphicsRootConstantBufferView(2, _Trait._GPU_resource->get()->GetGPUVirtualAddress() + sizeof(surface<float>) * _Trait._GPU_resource_index);

				mCommandList->IASetVertexBuffers(0, 1, &(_Shape._GPU_vertex_resource->as_D3D12_VERTEX_BUFFER_VIEW()));
				mCommandList->IASetIndexBuffer(        &(_Shape._GPU_index_resource->as_D3D12_INDEX_BUFFER_VIEW()));
				mCommandList->DrawIndexedInstanced(_Shape.index_count, 1, _Shape.start_index_location, _Shape.base_vertex_location, 0);
			}
		}

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		mCommandList->Close();
		ID3D12CommandList* _Cmdlists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(1, _Cmdlists);

		mSwapChain->Present(0, 0);
		mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
		//FlushCommandQueue();
		TW_frame_resource->fence = ++mCurrentFence;
		mCommandQueue->Signal(mFence.Get(), mCurrentFence);
		/*_My_frame_resource_current->fence = size_t(++mCurrentFence);
		mCommandQueue->Signal(mFence.Get(), mCurrentFence);*/
	}

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override { 
		_My_LastMousePos.x = x;
		_My_LastMousePos.y = y;
		SetCapture(mhMainWnd);
	}

	virtual void OnMouseUp(WPARAM btnState, int x, int y) override {
		_My_LastMousePos.x = x;
		_My_LastMousePos.y = y;
		ReleaseCapture();
	}

	virtual void OnMouseMove(WPARAM btnState, int x, int y) override{ 
		using namespace::clmagic;
		if (btnState & MK_LBUTTON) {
			auto dx = degrees<float>(0.10f * static_cast<float>(x - _My_LastMousePos.x));
			auto dy = degrees<float>(0.10f * static_cast<float>(y - _My_LastMousePos.y));
			_Mycamera.yaw(dx);
			_Mycamera.pitch(dy);
			_Mycamera.update_viewmatrix();

			/*_Log_str += to_string(_Mycamera.forward_vector()) + '\n';
			_Log_counter += 1;
			if (_Log_counter % 10 == 0) {
				MessageBoxA(nullptr, _Log_str.c_str(), nullptr, MB_OK);
				_Log_str.clear();
				_Log_counter = 0;
			}*/
		} else if(btnState & MK_RBUTTON){
			float dx = 0.1f * static_cast<float>(x - _My_LastMousePos.x);
			float dy = 0.1f * static_cast<float>(y - _My_LastMousePos.y);
			_Mycamera.walk(dy);
			_Mycamera.strafe(dx);
			_Mycamera.update_viewmatrix();
			
			/*gLights["light0"].penumbra -= dy;
			gLights["light0"].umbra    -= dx;
			gLights["light0"].umbra = min(gLights["light0"].umbra, gLights["light0"].penumbra);*/
		}

		_My_LastMousePos.x = x;
		_My_LastMousePos.y = y;
	}

private:
	
	std::string _Log_str;
	int _Log_counter = 0;

	Waves gWaves;
	perspective_camera<float, __m128> _Mycamera;

	d3d12::descriptor_heap descriptor_heap;
	std::unordered_map<std::string, hlsl::shader> gShaders;
	std::unordered_map<std::string, d3d12::shader_program> gPipelineStates;
	d3d12::shader_signature gRootSignature;

	degrees<float> sun_theta = 270;
	degrees<float> sun_phi = 90;
	POINT _My_LastMousePos;
};


int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try {
		ShapeApp _App(hInstance);
		_App.Initialize();
		_App.Run();
	} catch (const std::exception& e) {
		MessageBoxA(nullptr, e.what(), "Error", MB_OK);
	}

	return 0;
}


#include <complex>
#include <thread>
#include <iomanip>
#include <variant>
#include <algorithm>


template<typename _Ts, typename _Tb>
unit_vector3<_Ts, _Tb> compute_normal(vector3<_Ts, _Tb> P0, vector3<_Ts, _Tb> P1, vector3<_Ts, _Tb> P2) {// normalize( cross(P1-P0, P2-P0) ) 
	return unit_vector3<_Ts, _Tb>(clmagic::cross3(P1-P0, P2-P0));
}

template<typename _Ts, typename _Tb>
void compute_normal(vector3<_Ts, _Tb> P0, vector3<_Ts, _Tb> P1, vector3<_Ts, _Tb> P2, unit_vector3<_Ts,_Tb>& N) {// normalize( cross(P1-P0, P2-P0) ) 
	N += compute_normal(compute_normal(P0, P1, P2));
	N.normalize();
}

template<typename _Ts, typename _Tb>
void compute_normal(vector3<_Ts, _Tb> P0, vector3<_Ts, _Tb> P1, vector3<_Ts, _Tb> P2, 
	unit_vector3<_Ts,_Tb>& N0, unit_vector3<_Ts, _Tb>& N1, unit_vector3<_Ts, _Tb>& N2, unit_vector3<_Ts, _Tb>& N3) {
	auto _Result = compute_normal(P0, P1, P2);
	N0 += _Result;
	N1 += _Result;
	N2 += _Result;
	N3 += _Result;
	N0.normalize();
	N1.normalize();
	N2.normalize();
	N3.normalize();
}

template<typename _Ts, typename _Tb>
clmagic::box<_Ts, _Tb> get_bound(const std::vector<VECTOR3>& P) {
#undef min
#undef max
	VECTOR3 _Min = VECTOR3(std::numeric_limits<_Ts>::max());
	VECTOR3 _Max = VECTOR3(std::numeric_limits<_Ts>::min());

	auto       _First = P.begin();
	const auto _Last  = P.end();
	for (; _First != _Last; ++_First) {
		auto x = (*_First)[0];
		auto y = (*_First)[1];
		auto z = (*_First)[2];
		if (x < _Min[0]) _Min[0] = x;
		if (y < _Min[1]) _Min[1] = y;
		if (z < _Min[2]) _Min[2] = z;

		if (x > _Max[0]) _Max[0] = x;
		if (y > _Max[1]) _Max[1] = y;
		if (z > _Max[2]) _Max[2] = z;
	}

	return clmagic::box<_Ts, _Tb>{_Min, _Max};
}


int main() {
	using namespace::clmagic;
	/*matrix<float, 5, 5, __m128, _COL_MAJOR_> M(1.f);
	for (auto _First = M.begin(), _Last = M.end(); _First != _Last; ++_First) {
		*_First = randf(10.f, 30.f);
	}

	diagonal_matrix<float, 5, 5, __m128, _COL_MAJOR_> Md{ 1.f, 2.f, 3.f, 4.f ,5.f };
	
	std::cout << M << std::endl << std::endl;
	std::cout << Md * M << std::endl;*/
	const auto n1 = 1.33;
	const auto n2 = 1.00001;
	std::cout << ::asin(n2 / n1) * (180.0 / 3.14)<< std::endl;

	//cv::Mat _Source = cv::imread("media/leaf.png", cv::IMREAD_UNCHANGED);
	//std::cout << (_Source.type() == CV_8UC4) << std::endl;
	//cv::resize(_Source, _Source, cv::Size(256, 256));
	///*size_t dx = _Source.cols;
	//size_t dy = _Source.rows;
	//cv::Mat _Dest = cv::Mat(_Source.rows * 8, _Source.cols * 8, _Source.type());
	//for (size_t x = 0; x < _Dest.cols; x += dx) {
	//	for (size_t y = 0; y < _Dest.rows; y += dy) {
	//			_Source.copyTo(_Dest(cv::Rect(x, y, dx, dy)));
	//	}
	//}
	//for (size_t i = 0; i != 3; ++i) {
	//	cv::pyrDown(_Dest, _Dest);
	//}*/
	////while (true) {
	////	auto    _Rect = cv::selectROI("img", _Dest);
	////	cv::Mat _Ref  = _Dest(_Rect);
	////	cv::blur(_Ref, _Ref, cv::Size(3, 3));
	////	cv::imshow("img", _Dest);
	////	/*cv::Mat _Dest2;
	////	cv::bilateralFilter(_Dest, _Dest2, 10, 10, 100);
	////	cv::imshow("src", _Dest2);*/
	////	if (cv::waitKey() == 'q') {
	////		break;
	////	}
	////}
	//std::cout << "Please enter output name_:" ;
	//std::string _Name;
	//std::cin >> _Name;
	//cv::imwrite(_Name, _Source);

	using _Ts = float;
	using _Tb = __m128;

	struct face {
		uint32_t _Myindices[4];// quad
	};

	struct geometry {
		std::vector<VECTOR3> _Mypositions;
		std::vector<UNIT_VECTOR3> _Mynormals;
		std::vector<VECTOR2> _Mytexcoords;
		std::vector<face>    _Myfaces;
	};

	/*
	 _____________
	 /\0.2[m]    |
	|  |         |
	|  |         |
	|  |         |
	|  |         |
	|  |         |
	|  |        3[m]
	|  |         |
	|  |         |
	|  |         |
	|  |         | 
	|  |         |
	+--+-----------
	 width=0.04[m]
	+----------+
	|          |
	|     *    |
	|          |
	+----------+
	*/
	auto _Generate_sharp = [](VECTOR3 _Center) ->geometry {
		float _Cone_height     = 0.2f;
		float _Cylinder_width  = 0.04f;
		float _Cylinder_height = 3.f - _Cone_height;
		geometry _Result;
		_Result._Mypositions.push_back(_Center + VECTOR3{ -_Cylinder_width / 2, 0.f, -_Cylinder_width / 2 });
		_Result._Mypositions.push_back(_Center + VECTOR3{ +_Cylinder_width / 2, 0.f, -_Cylinder_width / 2 });
		_Result._Mypositions.push_back(_Center + VECTOR3{ +_Cylinder_width / 2, 0.f, +_Cylinder_width / 2 });
		_Result._Mypositions.push_back(_Center + VECTOR3{ -_Cylinder_width / 2, 0.f, +_Cylinder_width / 2 });
		_Result._Mypositions.push_back(_Center + VECTOR3{ -_Cylinder_width / 2, _Cylinder_height, -_Cylinder_width / 2 });
		_Result._Mypositions.push_back(_Center + VECTOR3{ +_Cylinder_width / 2, _Cylinder_height, -_Cylinder_width / 2 });
		_Result._Mypositions.push_back(_Center + VECTOR3{ +_Cylinder_width / 2, _Cylinder_height, +_Cylinder_width / 2 });
		_Result._Mypositions.push_back(_Center + VECTOR3{ -_Cylinder_width / 2, _Cylinder_height, +_Cylinder_width / 2 });
		_Result._Mypositions.push_back(_Center + VECTOR3{ -0.001f, _Cylinder_height+_Cone_height, -0.001f });
		_Result._Mypositions.push_back(_Center + VECTOR3{ +0.001f, _Cylinder_height+_Cone_height, -0.001f });
		_Result._Mypositions.push_back(_Center + VECTOR3{ +0.001f, _Cylinder_height+_Cone_height, +0.001f });
		_Result._Mypositions.push_back(_Center + VECTOR3{ -0.001f, _Cylinder_height+_Cone_height, +0.001f });
		_Result._Myfaces.push_back({0, 1, 2, 3});
		_Result._Myfaces.push_back({1, 0, 0+4, 1+4});
		_Result._Myfaces.push_back({2, 1, 1+4, 2+4});
		_Result._Myfaces.push_back({3, 2, 2+4, 3+4});
		_Result._Myfaces.push_back({0, 3, 3+4, 0+4});
		_Result._Myfaces.push_back({1+4, 0+4, 0+8, 1+8 });
		_Result._Myfaces.push_back({2+4, 1+4, 1+8, 2+8 });
		_Result._Myfaces.push_back({3+4, 2+4, 2+8, 3+8 });
		_Result._Myfaces.push_back({0+4, 3+4, 3+8, 0+8 });
		_Result._Myfaces.push_back({3+8, 2+8, 1+8, 0+8 });
		_Result._Mynormals.resize(_Result._Mypositions.size());
		_Result._Mytexcoords.resize(_Result._Mypositions.size());

		auto _Ffirst = _Result._Myfaces.begin();
		auto _Flast  = _Result._Myfaces.end();
		for ( ; _Ffirst != _Flast; ++_Ffirst) {
			const auto& P0 = _Result._Mypositions[_Ffirst->_Myindices[0]];
			const auto& P1 = _Result._Mypositions[_Ffirst->_Myindices[1]];
			const auto& P3 = _Result._Mypositions[_Ffirst->_Myindices[3]];
			auto&       N0 = _Result._Mynormals[_Ffirst->_Myindices[0]];
			auto&       N1 = _Result._Mynormals[_Ffirst->_Myindices[1]];
			auto&       N2 = _Result._Mynormals[_Ffirst->_Myindices[2]];
			auto&       N3 = _Result._Mynormals[_Ffirst->_Myindices[3]];
			compute_normal(P0, P1, P3, N0, N1, N2, N3);
		}

		const auto _Bound     = get_bound(_Result._Mypositions);
		const auto _InvCenter = VECTOR3{ -(_Bound.diff()[0])/2, 0.f, -(_Bound.diff()[2])/2 } - _Center;
		for (size_t v = 0; v != _Result._Mypositions.size(); ++v) {
			const auto& P  = _Result._Mypositions[v];
			auto&       UV = _Result._Mytexcoords[v];
			auto     _Temp =  (P + _InvCenter) / (_Bound.diff());
			UV[0] = _Temp[0];
			UV[1] = _Temp[1];
		}

		return _Result;
	};

	/*
	--*--*--*--*--
	x*sharp_width[m] + (x+1)*interval[m] = 100[m]
	interval = 0.2[m]
	x*0.04[m] + (x+1)*0.2[m]      = 100[m]
	x*0.04[m] + x*0.2[m] + 0.2[m] = 100[m]
	x*0.24[m]                     = 99.8[m]
	x = 415.83

	x = 399
	399*0.04[m] + 400*interval[m] = 100[m]
	400*interval[m]               = 84.04[m]
	interval = 0.2101[m]

	x*sharp_width[m] + (x+1)*interval[m] = 47.5[m]
	x*0.24[m]                            = 47.3[m]
	x = 197.083
	x = 199
	199*0.04[m] + 200*interval[m] = 47.5[m]
	7.96 + 200*interval[m]        = 47.5[m]
	200*interval[m] = 39.54[m]
	interval = 0.1977
	*/
	/*auto r = 0.1f / cos(static_cast<float>(radians<float>(degrees<float>(60))));
	std::cout << r * sin(static_cast<float>(radians<float>(degrees<float>(60)))) << std::endl;*/
	std::cout << cos(static_cast<float>(radians<float>(degrees<float>(30)))) * 100 << std::endl;

	float interval = 0.1977f;
	size_t number  = 200;
	std::vector<geometry> _Result;
	
	float x = interval + 0.02f;
	for (size_t i = 1; i <= number; ++i, x+=interval+ 0.04f) {
		_Result.push_back( _Generate_sharp(VECTOR3{ x, 0.f, 0.f }) );
	}

	//for (size_t i = 0; i != aiGetExportFormatCount(); ++i) {
	//	const aiExportFormatDesc* _Desc = aiGetExportFormatDescription(i);
	//	std::cout << _Desc->id << std::endl;
	//	std::cout << _Desc->description << std::endl;
	//	std::cout << _Desc->fileExtension <<"\n"<< std::endl;
	//	//delete _Desc;
	//}

	std::vector<aiVector3D> _All_vertices;
	std::vector<aiVector3D> _All_normals;
	std::vector<aiVector3D> _All_texcoords;
	std::vector<aiFace> _All_faces;
	uint32_t _Vertex_offset = 0;
	for (size_t i = 0; i != _Result.size(); ++i) {
		const auto& _Geo = _Result[i];
		
		auto _Ffirst = _Geo._Myfaces.begin();
		auto _Flast = _Geo._Myfaces.end();
		for ( ; _Ffirst != _Flast; ++_Ffirst) {
			aiFace _Face;
			_Face.mNumIndices = 4;
			_Face.mIndices    = new uint32_t[4]{ _Ffirst->_Myindices[0], _Ffirst->_Myindices[1], _Ffirst->_Myindices[2], _Ffirst->_Myindices[3] };
			for (size_t f = 0; f != _Face.mNumIndices; ++f) {
				_Face.mIndices[f] += _Vertex_offset;
			}
			_All_faces.push_back(_Face);
		}

		auto _Vfirst = _Geo._Mypositions.begin();
		auto _Vlast  = _Geo._Mypositions.end();
		for ( ; _Vfirst != _Vlast; ++_Vfirst) {
			_All_vertices.push_back(reinterpret_cast<const aiVector3D&>(*_Vfirst));
		}
		_Vertex_offset += _Geo._Mypositions.size();

		auto _Nfirst = _Geo._Mynormals.begin();
		auto _Nlast  = _Geo._Mynormals.end();
		for ( ; _Nfirst != _Nlast; ++_Nfirst) {
			_All_normals.push_back(reinterpret_cast<const aiVector3D&>(*_Nfirst));
		}

		auto _Tfirst = _Geo._Mytexcoords.begin();
		auto _Tlast  = _Geo._Mytexcoords.end();
		for ( ; _Tfirst != _Tlast; ++_Tfirst) {
			_All_texcoords.push_back(aiVector3D((*_Tfirst)[0], (*_Tfirst)[1], 0.f));
		}
	}

	aiMesh* mesh1 = new aiMesh();
	mesh1->mName        = "sharp";
	mesh1->mPrimitiveTypes = aiPrimitiveType::aiPrimitiveType_POLYGON;
	mesh1->mNumFaces    = _All_faces.size();
	mesh1->mFaces       = new aiFace[_All_faces.size()];
	assert(_All_vertices.size() == _All_normals.size() && _All_vertices.size() == _All_texcoords.size());
	mesh1->mNumVertices = _All_vertices.size();
	mesh1->mVertices    = new aiVector3D[_All_vertices.size()];
	mesh1->mNormals     = new aiVector3D[_All_normals.size()];
	mesh1->mTextureCoords[0] = new aiVector3D[_All_texcoords.size()];
	std::copy(_All_faces.begin(), _All_faces.end(), mesh1->mFaces);
	std::copy(_All_vertices.begin(), _All_vertices.end(), mesh1->mVertices);
	std::copy(_All_normals.begin(), _All_normals.end(), mesh1->mNormals);
	std::copy(_All_texcoords.begin(), _All_texcoords.end(), mesh1->mTextureCoords[0]);

	const aiScene* _Scene = aiImportFile("media/untitled.obj", aiProcessPreset_TargetRealtime_MaxQuality);
	std::swap(_Scene->mMeshes[0], mesh1);

	aiExportScene(_Scene, "objnomtl", "media/sharp.obj", 0);



	return 0;
}