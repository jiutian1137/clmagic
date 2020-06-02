#include "../src/clmagic/basic/timer_wheel.h"
#include "../src/clmagic/math.h"
#include "../src/clmagic/directX12/directX12.h"
#include <iostream>
#include <chrono>
#include <map>
#include "Common/GeometryGenerator.h"
#include "Inc/DDSTextureLoader.h"
#include "Inc/ResourceUploadBatch.h"
#include "assimp/scene.h"
#include "assimp/cexport.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#pragma comment(lib, "lib/assimp-vc142-mt.lib")
//#include "opencv/opencv.hpp"
//#pragma comment(lib, "lib/opencv_world345d.lib")
#pragma comment(lib, "lib/DirectXTK12.lib")

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

struct FrameResource : public d3d12::basic_frame_resource {
	FrameResource() = default;

	FrameResource(ID3D12Device& _Device, D3D12_COMMAND_LIST_TYPE _Type)
		: d3d12::basic_frame_resource(_Device, _Type) {
		// single descriptor
		cb_PER_FRAME = d3d12::dynamic_buffer<PER_FRAME>(_Device, 1);
		cb_RENDERING = d3d12::dynamic_buffer<RENDERING>(_Device, 1);
	}

	virtual void release() override {
		d3d12::basic_frame_resource::release();
		cb_PER_FRAME.release();
		cb_RENDERING.release();
	}

	d3d12::dynamic_buffer<PER_FRAME> cb_PER_FRAME;// b0
	d3d12::dynamic_buffer<RENDERING> cb_RENDERING;// b3
};



d3d12::factory gFactory;
d3d12::device  gDevice;
d3d12::fence   gFence;
d3d12::window  gWindow;

d3d12::shader_signature gSignature;
constexpr uint32_t INDEX_FRAME = 0;
constexpr uint32_t INDEX_TRANSFORM = 1;
constexpr uint32_t INDEX_SUBSTANCE = 2;
constexpr uint32_t INDEX_RENDERING = 3;// light and environment

d3d12::shader_program gProgram;

d3d12::CBV_SRV_UAV_array_map<std::string, 100> gSRVs;

d3d12::static_texture2D<DXGI_FORMAT_BC3_UNORM> gTexture;

std::map<std::string, 
	std::shared_ptr<clmagic::geometry>> gGeometrys;

std::map<std::string, 
	std::shared_ptr<clmagic::substance>> gSubstances;

std::map<std::string, 
	clmagic::object_system>  gObjectSystems;

timer_wheel<FrameResource> gFrameResources;

std::map<std::string, perspective_camera<float, __m128>> gCameras;



void initial_geometry(ID3D12Device& _Device);
void initial_material(ID3D12Device& _Device);
void initial_light();
void initial_object();

void initializer(ID3D12Device& _Device) {
	d3d12::command_objects _Command = d3d12::command_objects(_Device);

	{// gShaderSignature = ...
		std::vector<D3D12_ROOT_PARAMETER> _Parameters(5);
		CD3DX12_ROOT_PARAMETER::InitAsConstantBufferView(_Parameters[INDEX_FRAME], 0/*b0*/);
		CD3DX12_ROOT_PARAMETER::InitAsConstants(_Parameters[INDEX_TRANSFORM], 16, 1/*b1*/);
		CD3DX12_ROOT_PARAMETER::InitAsConstants(_Parameters[INDEX_SUBSTANCE], 8, 2/*b2*/);
		CD3DX12_ROOT_PARAMETER::InitAsConstantBufferView(_Parameters[INDEX_RENDERING], 3/*b3*/);
		CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(_Parameters[4], 1, &CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0));
		
		std::vector<D3D12_STATIC_SAMPLER_DESC> _Samplers(1);
		_Samplers[0] = CD3DX12_STATIC_SAMPLER_DESC(0/*s0*/, D3D12_FILTER_ANISOTROPIC);
		
		gSignature = d3d12::shader_signature(_Device, _Parameters, _Samplers);

		//// load texture
		//DirectX::ResourceUploadBatch _Upload(_Device.get());
		//_Upload.Begin();
		//DirectX::CreateDDSTextureFromFile(_Device.get(), _Upload, L"media/WoodCrate01.dds", &gTexture._Impl);
		//_Upload.End(_Command._My_command_queue.get());
		//gFence.flush(_Command._My_command_queue.ref());

		//// create and insert shader_resource_view
		//gSRVs = d3d12::CBV_SRV_UAV_array_map<std::string, 100>(_Device.ref(), D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		//gSRVs.insert_or_assign("gTexture", gTexture.ref(), d3d12::make_D3D12_SHADER_RESOURCE_VIEW_DESC_with_TEXTURE2D(gTexture.ref()));
	}

	{// gShaderProgram = ...
		d3d12::shader_input _Varyings;
		_Varyings.push_back("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
		_Varyings.push_back("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
		_Varyings.push_back("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
	
		hlsl::shader _StandardVS = hlsl::shader_compile(L"shader/norm.hlsl", nullptr, "VS", "vs_5_0");
		hlsl::shader _StandardPS = hlsl::shader_compile(L"shader/norm.hlsl", nullptr, "PS", "ps_5_0");

		gProgram = d3d12::shader_program(_Device, gSignature.ref(), _Varyings.get(), _StandardVS.get_dx12(), _StandardPS.get_dx12(),
			gWindow._My_color_buffer.format, gWindow._My_depth_buffer.format, DXGI_SAMPLE_DESC{ 1, 0 });
	}

	for (size_t i = 0; i != 1; ++i) {
		gFrameResources.expend_cycle(FrameResource(_Device, D3D12_COMMAND_LIST_TYPE_DIRECT));
	}

	gCameras["default"] = perspective_camera<float, __m128>();

	initial_geometry(_Device);
	initial_material(_Device);
	initial_light();
	initial_object();
}


//
//void display() {
//	TW_frame_resource.turn();
//	if (TW_frame_resource->fence != 0 && gFence->GetCompletedValue() < TW_frame_resource->fence) {
//		HANDLE _Event = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
//		gFence->SetEventOnCompletion(TW_frame_resource->fence, _Event);
//		WaitForSingleObject(_Event, INFINITE);
//		CloseHandle(_Event);
//	}
//
//	
//
//	/*auto  _Color_buffer_view = gRTVs.get_CPUhandle(gBackbuffer_color.timer_pointer);
//	auto  _Depth_buffer_view = gDSVs.get_CPUhandle(0);*/
//
//	auto _Cmdlist_alloc = TW_frame_resource->command_allocator_ptr();
//	_Cmdlist_alloc->Reset();
//	gCommandObjects->Reset(_Cmdlist_alloc, gShaderProgram.get());
//
//	gCommandObjects->RSSetViewports(1, &gWindow.get_D3D12_VIEWPORT());
//	gCommandObjects->RSSetScissorRects(1, &gWindow.get_D3D12_RECT());
//	gCommandObjects->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gBackbuffer_color.clock().get(),
//		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
//
//	/*gCommandObjects->ClearRenderTargetView(_Color_buffer_view, DirectX::Colors::LightSteelBlue, 0, nullptr);
//	gCommandObjects->ClearDepthStencilView(_Depth_buffer_view, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
//	gCommandObjects->OMSetRenderTargets(1, &_Color_buffer_view, true, &_Depth_buffer_view);*/
//
//	
//	//ID3D12DescriptorHeap* descriptorHeaps[] = { gSRVs.get() };
//	//gCommandObjects->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
//	gCommandObjects->SetGraphicsRootSignature(gShaderSignature.get());
//	gCommandObjects->SetGraphicsRootConstantBufferView(0, TW_frame_resource->cb_PER_FRAME->GetGPUVirtualAddress());
//	gCommandObjects->SetGraphicsRootConstantBufferView(3, TW_frame_resource->cb_RENDERING->GetGPUVirtualAddress());
//
//	{// draw
//		auto       _First = CPU_static_Actors.begin();
//		const auto _Last = CPU_static_Actors.end();
//		for (; _First != _Last; ++_First) {
//
//			gCommandObjects->SetGraphicsRoot32BitConstants(1, 16, transpose(_First->second._Myworld).ptr(), 0);
//
//			auto& _Shape = *(_First->second._Myshape);
//			gCommandObjects->IASetPrimitiveTopology(_Shape.type);
//			gCommandObjects->IASetVertexBuffers(0, 1, &(_Shape._GPU_vertex_resource->as_D3D12_VERTEX_BUFFER_VIEW()));
//			gCommandObjects->IASetIndexBuffer(&(_Shape._GPU_index_resource->as_D3D12_INDEX_BUFFER_VIEW()));
//
//			auto& _Trait = *(_First->second._Mytrait);
//			gCommandObjects->SetGraphicsRootConstantBufferView(2, (*_Trait._GPU_resource)[_Trait._GPU_resource_index]);
//			//gCommandObjects->SetGraphicsRootDescriptorTable(4, gSRVs->GetGPUDescriptorHandleForHeapStart());
//
//			gCommandObjects->DrawIndexedInstanced(_Shape.index_count, 1, _Shape.start_index_location, _Shape.base_vertex_location, 0);
//		}
//	}
//
//	gCommandObjects->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gBackbuffer_color.clock().get(),
//		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
//	gCommandObjects.close_execute();
//
//	gBackbuffer_color.chain().Present(0, 0);
//	gBackbuffer_color.turn();
//	//FlushCommandQueue();
//	TW_frame_resource->fence = ++gFence_value;
//	gCommandObjects._My_command_queue->Signal(gFence.get(), gFence_value);
//	//gFence.flush(gCommandObjects._My_command_queue.ref(), gFence_value);
//}
void initial_geometry(ID3D12Device& _Device) {
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
	
	std::shared_ptr<d3d12::static_mesh<VInput>> _All_mesh = std::make_shared<d3d12::static_mesh<VInput>>();
	d3d12::dynamic_buffer<VInput>   _Vertex_intermediator;
	d3d12::dynamic_buffer<uint32_t> _Index_intermediator;
	{
		auto&   _Source        = _Static_vertices;
		_Vertex_intermediator  = d3d12::make_dynamic_buffer(_Device, _Source);
	}	
	{
		auto&     _Source    = _Static_indices;
		_Index_intermediator = d3d12::make_dynamic_buffer(_Device, _Source);
	}
	_All_mesh->_My_vertices = std::make_shared<d3d12::static_buffer<VInput>>(_Vertex_intermediator);
	_All_mesh->_My_indices  = std::make_shared<d3d12::static_buffer<uint32_t>>(_Index_intermediator);

	gGeometrys.insert_or_assign("AllGeometry", std::make_shared<clmagic::geometry>());
	gGeometrys["AllGeometry"]->_My_mesh      = _All_mesh;
	//gGeometrys["AllGeometry"]->_My_transform = std::make_shared<d3d12::matrix_transform<1, float>>(1.f);

	gGeometrys["Box"] = gGeometrys["AllGeometry"]->reduce(boxVertexOffset, boxIndexOffset, box.Indices32.size());
	gGeometrys["Grid"] = gGeometrys["AllGeometry"]->reduce(gridVertexOffset, gridIndexOffset, grid.Indices32.size());
	gGeometrys["Sphere"] = gGeometrys["AllGeometry"]->reduce(sphereVertexOffset, sphereIndexOffset, sphere.Indices32.size());
	gGeometrys["Cylinder"] = gGeometrys["AllGeometry"]->reduce(cylinderVertexOffset, cylinderIndexOffset, cylinder.Indices32.size());
}

void initial_material(ID3D12Device& _Device) {
	std::vector<surface<float>> static_traits;
	
	// chemical element of atomic number 22
	auto _Titanium = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	_Titanium->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	_Titanium->reflect_value     = { 0.542f, 0.497f, 0.449f };
	_Titanium->roughness_x       = 0.f;
	_Titanium->roughness_y       = 0.f;
	gSubstances["Titanium"] = _Titanium;
	
	// chemical element of atomic number 24
	auto _Chromium = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	_Chromium->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	_Chromium->reflect_value     = { 0.549f, 0.556f, 0.554f };
	_Chromium->roughness_x       = 0.f;
	_Chromium->roughness_y       = 0.f;
	gSubstances["_Chromium"] = _Chromium;
	
	// chemical element of atomic number 26
	auto _Iron = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	_Iron->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	_Iron->reflect_value     = { 0.562f, 0.565f, 0.578f };
	_Iron->roughness_x       = 0.f;
	_Iron->roughness_y       = 0.f;
	gSubstances["Iron"] = _Iron;
		
	// chemical elelemtn of atomic number 28
	auto _Nickel = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	_Nickel->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	_Nickel->reflect_value     = { 0.660f, 0.609f, 0.526f };
	_Nickel->roughness_x       = 0.f;
	_Nickel->roughness_y       = 0.f;
	gSubstances["Nickel"] = _Nickel;
	
	// chemical element of atomic number 78
	auto _Platinum = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	_Platinum->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	_Platinum->reflect_value     = { 0.673f, 0.637f, 0.585f };
	_Platinum->roughness_x       = 0.f;
	_Platinum->roughness_y       = 0.f;
	gSubstances["Platinum"] = _Platinum;
	
	// chemical element of atomic number 29
	auto _Copper = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	_Copper->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	_Copper->reflect_value     = { 0.955f, 0.638f, 0.538f };
	_Copper->roughness_x       = 0.f;
	_Copper->roughness_y       = 0.f;
	gSubstances["Copper"] = _Copper;

	auto _Palladium = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	_Palladium->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	_Palladium->reflect_value     = { 0.733f, 0.697f, 0.652f };
	_Palladium->roughness_x       = 0.f;
	_Palladium->roughness_y       = 0.f;
	gSubstances["Palladium"] = _Palladium;
	
	//CPU_static_traits["Mercury"].roughness_x       = 0.f;
	//CPU_static_traits["Mercury"].roughness_y       = 0.f;
	//CPU_static_traits["Mercury"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//CPU_static_traits["Mercury"].reflect_value     = { 0.781f, 0.780f, 0.778f };
	//CPU_static_traits["Mercury"]._GPU_resource     = &GPU_static_traits;
	//CPU_static_traits["Mercury"]._GPU_resource_index = 7;
	//static_traits.push_back( CPU_static_traits["Mercury"] );
	//
	//CPU_static_traits["Brass"].roughness_x       = 0.f;
	//CPU_static_traits["Brass"].roughness_y       = 0.f;
	//CPU_static_traits["Brass"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//CPU_static_traits["Brass"].reflect_value     = { 0.910f, 0.778f, 0.423f };
	//CPU_static_traits["Brass"]._GPU_resource     = &GPU_static_traits;
	//CPU_static_traits["Brass"]._GPU_resource_index = 8;
	//static_traits.push_back( CPU_static_traits["Brass"] );
	//
	//// chemical element of atomic number 30
	//CPU_static_traits["Zinc"].roughness_x       = 0.f;
	//CPU_static_traits["Zinc"].roughness_y       = 0.f;
	//CPU_static_traits["Zinc"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//CPU_static_traits["Zinc"].reflect_value     = { 0.664f, 0.824f, 0.850f };
	//CPU_static_traits["Zinc"]._GPU_resource     = &GPU_static_traits;
	//CPU_static_traits["Zinc"]._GPU_resource_index = 9;
	//static_traits.push_back( CPU_static_traits["Zinc"] );
	//
	// chemical element of atomic number 79
	auto _Gold = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	_Gold->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	_Gold->reflect_value     = { 1.000f, 0.782f, 0.344f };
	_Gold->roughness_x       = 0.f;
	_Gold->roughness_y       = 0.f;
	gSubstances["Gold"] = _Gold;
	
	// chemical element of atomic number 13
	auto _Aluminum = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	_Aluminum->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	_Aluminum->reflect_value     = { 0.913f, 0.922f, 0.924f };
	_Aluminum->roughness_x       = 0.f;
	_Aluminum->roughness_y       = 0.f;
	gSubstances["Aluminum"] = _Aluminum;
	
	// chemical element of atomic number 47
	auto _Silver = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	_Silver->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	_Silver->reflect_value     = { 0.972f, 0.960f, 0.915f };
	_Silver->roughness_x       = 0.f;
	_Silver->roughness_y       = 0.f;
	gSubstances["Silver"] = _Silver;

	auto _Diamond = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	_Diamond->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	_Diamond->reflect_value     = { 0.171f, 0.172f, 0.176f };
	_Diamond->roughness_x       = 0.f;
	_Diamond->roughness_y       = 0.f;
	gSubstances["Diamond"] = _Diamond;
	
	//// chemical element of atomic number 14
	//CPU_static_traits["Silicon"].roughness_x       = 0.f;
	//CPU_static_traits["Silicon"].roughness_y       = 0.f;
	//CPU_static_traits["Silicon"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//CPU_static_traits["Silicon"].reflect_value     = { 0.345f, 0.369f, 0.426f };
	//CPU_static_traits["Silicon"]._GPU_resource     = &GPU_static_traits;
	//CPU_static_traits["Silicon"]._GPU_resource_index = 14;
	//static_traits.push_back(CPU_static_traits["Diamond"]);
}

void initial_light() {
	for (size_t i = 0; i != gFrameResources.cycle(); ++i, gFrameResources.turn()) {
		RENDERING* _Ptr = nullptr;
		gFrameResources->cb_RENDERING->Map(0, nullptr, reinterpret_cast<void**>(&_Ptr));
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
		gFrameResources->cb_RENDERING->Unmap(0, nullptr);

		PER_FRAME* _Cbpass = nullptr;
		gFrameResources->cb_PER_FRAME->Map(0, nullptr, reinterpret_cast<void**>(&_Cbpass));
		_Cbpass->Mview = matrix_cast<clmagic::matrix4x4<float>>(transpose(gCameras["default"].view_matrix()));
		_Cbpass->MProj = matrix_cast<clmagic::matrix4x4<float>>(transpose(gCameras["default"].projection_matrix()));
		_Cbpass->Peye = vector_cast<clmagic::vector<float, 3>>(gCameras["default"].eye_position());
		gFrameResources->cb_PER_FRAME->Unmap(0, nullptr);
	}
}

void initial_object() {
	using matrix4x4 = clmagic::matrix4x4<float>;
	
	gObjectSystems["box"]._My_transform = std::make_shared<d3d12::matrix_transform<INDEX_TRANSFORM, float>>(
		translation<matrix4x4>::get_matrix(0.f, 1.5f, 11.f) * scaling<matrix4x4>::get_matrix(2.f));
	gObjectSystems["box"]._My_components["1"] = std::make_shared<clmagic::object>();
	gObjectSystems["box"]._My_components["1"]->_My_geometry  = gGeometrys["Box"];
	gObjectSystems["box"]._My_components["1"]->_My_substance = gSubstances["Copper"];
	
	gObjectSystems["grid"]._My_transform = std::make_shared<d3d12::matrix_transform<INDEX_TRANSFORM, float>>(matrix4x4(1.f));
	gObjectSystems["grid"]._My_components["1"] = std::make_shared<clmagic::object>();
	gObjectSystems["grid"]._My_components["1"]->_My_geometry  = gGeometrys["Grid"];
	gObjectSystems["grid"]._My_components["1"]->_My_substance = gSubstances["Iron"];
	
	for (int i = 0; i < 5; ++i) {
		gObjectSystems["leftCyl" + std::to_string(i)]._My_transform = std::make_shared<d3d12::matrix_transform<INDEX_TRANSFORM, float>>(
			translation<matrix4x4>::get_matrix(-5.0f, 1.5f, -10.0f + i * 5.0f));
		gObjectSystems["leftCyl" + std::to_string(i)]._My_components["1"] = std::make_shared<clmagic::object>();
		gObjectSystems["leftCyl" + std::to_string(i)]._My_components["1"]->_My_geometry  = gGeometrys["Cylinder"];
		gObjectSystems["leftCyl" + std::to_string(i)]._My_components["1"]->_My_substance = gSubstances["Aluminum"];
	
		gObjectSystems["rightCyl" + std::to_string(i)]._My_transform = std::make_shared<d3d12::matrix_transform<INDEX_TRANSFORM, float>>(
			translation<matrix4x4>::get_matrix(+5.0f, 1.5f, -10.0f + i * 5.0f));
		gObjectSystems["rightCyl" + std::to_string(i)]._My_components["1"] = std::make_shared<clmagic::object>();
		gObjectSystems["rightCyl" + std::to_string(i)]._My_components["1"]->_My_geometry  = gGeometrys["Cylinder"];
		gObjectSystems["rightCyl" + std::to_string(i)]._My_components["1"]->_My_substance = gSubstances["Gold"];
	
		gObjectSystems["leftSph" + std::to_string(i)]._My_transform = std::make_shared<d3d12::matrix_transform<INDEX_TRANSFORM, float>>(
			translation<matrix4x4>::get_matrix(-5.0f, 3.0f, -10.0f + i * 5.0f));
		gObjectSystems["leftSph" + std::to_string(i)]._My_components["1"] = std::make_shared<clmagic::object>();
		gObjectSystems["leftSph" + std::to_string(i)]._My_components["1"]->_My_geometry  = gGeometrys["Sphere"];
		gObjectSystems["leftSph" + std::to_string(i)]._My_components["1"]->_My_substance = gSubstances["Silver"];
	
		gObjectSystems["rightSph" + std::to_string(i)]._My_transform = std::make_shared<d3d12::matrix_transform<INDEX_TRANSFORM, float>>(
			translation<matrix4x4>::get_matrix(+5.0f, 3.0f, -10.0f + i * 5.0f));
		gObjectSystems["rightSph" + std::to_string(i)]._My_components["1"] = std::make_shared<clmagic::object>();
		gObjectSystems["rightSph" + std::to_string(i)]._My_components["1"]->_My_geometry  = gGeometrys["Sphere"];
		gObjectSystems["rightSph" + std::to_string(i)]._My_components["1"]->_My_substance = gSubstances["Silver"];
	}
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try { 
		gFactory = d3d12::factory(1);
		gDevice  = d3d12::device(gFactory.ref());
		gFence   = d3d12::fence(gDevice.ref(), 0);
		gWindow.init(&gFactory, &gDevice, &gFence, hInstance, L"test_graphics", 0, 0, 1600, 900);
		gWindow.show();

		initializer(gDevice.ref());
		
		using namespace::d3d12;
		MSG msg = { 0 };
		graphics_command_list _Cmdlist = graphics_command_list(gDevice.ref(), D3D12_COMMAND_LIST_TYPE_DIRECT, *gFrameResources->command_allocator_ptr());
		while (msg.message != WM_QUIT) {
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			} else {
				gFrameResources->command_allocator_ptr()->Reset();
				_Cmdlist->Reset(gFrameResources->command_allocator_ptr(), gProgram.get());
				_Cmdlist->ResourceBarrier(1, &gWindow._My_color_buffer->transition(D3D12_RESOURCE_STATE_RENDER_TARGET));
			
				gWindow.clear_buffer(_Cmdlist.ref(),DirectX::Colors::LightSteelBlue);
					
				{
					_Cmdlist->OMSetRenderTargets(1, &gWindow.get_color_view(), true, &gWindow.get_depth_view());
					_Cmdlist->RSSetViewports(1, &gWindow.get_D3D12_VIEWPORT());
					_Cmdlist->RSSetScissorRects(1, &gWindow.get_D3D12_RECT());
					_Cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
						
					_Cmdlist->SetGraphicsRootSignature(gSignature.get());
					_Cmdlist->SetGraphicsRootConstantBufferView(INDEX_FRAME, gFrameResources->cb_PER_FRAME->GetGPUVirtualAddress());
					_Cmdlist->SetGraphicsRootConstantBufferView(INDEX_RENDERING, gFrameResources->cb_RENDERING->GetGPUVirtualAddress());
					auto       _First = gObjectSystems.begin();
					const auto _Last  = gObjectSystems.end();
					for ( ; _First != _Last; ++_First) {
						_First->second.render(_Cmdlist.ref());
					}
				}
	
				_Cmdlist->ResourceBarrier(1, &gWindow._My_color_buffer->transition(D3D12_RESOURCE_STATE_PRESENT));
				_Cmdlist->Close();
			}

			gWindow.execute_command_list(_Cmdlist.ref());
			
			gWindow.swap_buffer();
			
			gFence.flush(gWindow._My_executer.ref());
		}
		//d3d12::window::run();
	} catch (const std::exception& e) {
		MessageBoxA(nullptr, e.what(), "Error", MB_OK);
	}

	gCameras.clear();
	gFrameResources.clear();
	gObjectSystems.clear();
	gGeometrys.clear();
	gSubstances.clear();
	gSignature.release();
	gProgram.release();
	gWindow.release();
	gFence.release();
	gDevice.release();
	gFactory.release();

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