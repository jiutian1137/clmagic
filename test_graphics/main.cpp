#include <iostream>
#include <fstream>
#include <chrono>
#include <map>
#include "../src/clmagic/basic/timer_wheel.h"
#include "../src/clmagic/math.h"
#include "../src/clmagic/directX12/directX12.h"

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

float* _Control_roughness = nullptr;

struct test_window : public d3d12::window {

	virtual void on_mouse(winapi_util::mouse_types button, winapi_util::button_states state) override {
		if (button == winapi_util::mouse_types::right) {
			if (state == winapi_util::button_states::release || state == winapi_util::button_states::repeat) {
				_Change_direction = false;
			} else {
				_Change_direction = true;
			}
		}
	}

	virtual void on_cursor(POINT _Current, POINT _Prev) override {
		if (_Change_direction) {
			_My_camera.yaw_surround({0.f, 0.f, 0.f}, (_Current.x - _Prev.x) * 0.01f);
			_My_camera.pitch_surround({ 0.f, 0.f, 0.f }, (_Current.y - _Prev.y) * 0.01f);
			_My_camera.look_at({ 0.f, 0.f, 0.f });
			//_My_camera.look();
			//MessageBoxA(nullptr, _My_camera.to_string().c_str(), nullptr, MB_OK);
		}
	}

	virtual void on_key(int key, winapi_util::button_states state) override {
		if (state != winapi_util::button_states::release) {
			switch (key) {
			case 'D': _My_camera.strafe(+1.f); break;
			case 'A': _My_camera.strafe(-1.f); break;
			case 'W': _My_camera.walk(+1.f); break;
			case 'S': _My_camera.walk(-1.f); break;
			case 'Q': _My_camera._My_position += _My_camera._My_u_vector * (+1.f); break;
			case 'E': _My_camera._My_position += _My_camera._My_u_vector * (-1.f); break;
			case 'T': *_Control_roughness += 0.02f; break;
			case 'G': *_Control_roughness -= 0.02f; break;
			default:
				break;
			}
			_My_camera.look();
		}
	}

	bool _Change_direction = false;
	bool _Change_position  = true;
	perspective_viewer<float> _My_camera = perspective_viewer<float>({ 0.f, 1.7f, -1.f }, { 0.f, 0.f, 1.f }, clmagic::degrees<float>(60));
};

//struct wave_geometry : public d3d12::dynamic_mesh<norm_hlsl::varying> {
//	using _Mybase = d3d12::dynamic_mesh<norm_hlsl::varying>;
//	virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList& _Cmdlist) override {
//		_Mybase::render(_Cmdlist);
//	}
//	
//	bool  _My_durty = true;
//	Waves _My_generater;
//};


// basic
test_window  gWindow;
d3d12::fence gFence;
#include "Waves.h"
Waves gWaves;

// shader_pipeline
#define NORM_HLSL "shader/norm.hlsl"
#define NORM_HLSL_L L"shader/norm.hlsl"
#include NORM_HLSL
std::map<std::string, d3d12::shader_program> gPrograms;
std::map<std::string, d3d12::shader_input>    gVaryings;
std::map<std::string, d3d12::shader_signature> gUniforms;
std::map<std::string, std::shared_ptr<clmagic::uniform>> gFrameResources;

// resources
d3d12::CBV_SRV_UAV_array_map<std::string, 100> gSRVs;
std::map<std::string, 
	d3d12::static_texture2D<DXGI_FORMAT_BC3_UNORM>> gTextures;
std::map<std::string, 
	std::shared_ptr<clmagic::substance>> gSubstances;
std::map<std::string, 
	std::shared_ptr<clmagic::geometry>> gGeometrys;
std::map<std::string,
	std::shared_ptr<d3d12::dynamic_mesh<norm_hlsl::varying>>> gDynGeometrys;

// composite resources to object_system
std::map<std::string, 
	clmagic::object_system>  gObjectSystems;



void init_shader_pipeline(ID3D12Device& _Device) {
	hlsl::shader _StandardVS = hlsl::shader_compile(NORM_HLSL_L, nullptr, "VS", "vs_5_0");
	hlsl::shader _StandardPS = hlsl::shader_compile(NORM_HLSL_L, nullptr, "PS", "ps_5_0");

	gUniforms[NORM_HLSL] = norm_hlsl::make_shader_signature(_Device);
	gVaryings[NORM_HLSL] = norm_hlsl::make_input_decs();
	gPrograms[NORM_HLSL] = d3d12::shader_program(d3d12::shader_pipeline(_Device, gUniforms[NORM_HLSL], gVaryings[NORM_HLSL].get(), _StandardVS.get_dx12(), _StandardPS.get_dx12(),
		gWindow._My_color_buffer.format, gWindow._My_depth_buffer.format, DXGI_SAMPLE_DESC{ 1, 0 }) );

	gFrameResources[NORM_HLSL] = std::make_shared<norm_hlsl::uniform_frame>(_Device);
}

void init_shader_frame_resources() {
	auto* _Norm_hlsl_fs = dynamic_cast<norm_hlsl::uniform_frame*>(gFrameResources[NORM_HLSL].get());
	norm_hlsl::uniform_frame::_My_data_type* _Ptr = nullptr;
	_Norm_hlsl_fs->map(reinterpret_cast<void**>(&_Ptr));

	_Ptr->env.particles_ratio = 0.09f;
	_Ptr->env.particles_color = { 0.f, 0.f, 0.f };

	_Ptr->light_sources[0].color = { 0.7f, 0.7f, 0.7f };
	_Ptr->light_sources[0].intensity = 100.f;
	_Ptr->light_sources[0].position = { 0.f, 30.f, 0.f };
	_Ptr->light_sources[0].direction = { 0.f, -1.f, 0.f };
	_Ptr->light_sources[0].penumbra = 3.14f;
	_Ptr->light_sources[0].umbra = 3.14f;

	_Ptr->light_sources[1].color = { 1.f, 1.f, 1.f };
	_Ptr->light_sources[1].intensity = 50.f;
	_Ptr->light_sources[1].position = { -20.f, 2.f, 0.f };
	_Ptr->light_sources[1].direction = { 1.f, 0.f, 0.f };
	_Ptr->light_sources[1].penumbra = 3.14f;
	_Ptr->light_sources[1].umbra = 3.14f;

	_Ptr->light_sources[2].color = { 1.f, 1.f, 1.f };
	_Ptr->light_sources[2].intensity = 50.f;
	_Ptr->light_sources[2].position = { 0.f, 2.f, -10.f };
	_Ptr->light_sources[2].direction = { 0.f, 0.f, 1.f };
	_Ptr->light_sources[2].penumbra = 3.14f;
	_Ptr->light_sources[2].umbra = 3.14f;

	_Ptr->view_matrix = matrix_cast<matrix4x4<float, float>>(transpose(gWindow._My_camera.view_matrix()));
	_Ptr->proj_matrix = matrix_cast<matrix4x4<float, float>>(transpose(gWindow._My_camera.proj_matrix()));
	_Ptr->eye_position = vector_cast<vector3<float, float>>(gWindow._My_camera.position());

	_Norm_hlsl_fs->unmap(reinterpret_cast<void**>(&_Ptr));
}

void init_texture_resources(ID3D12Device& _Device) {
	d3d12::command_objects _Command = d3d12::command_objects(_Device);

	DirectX::ResourceUploadBatch _Upload(&_Device);
	_Upload.Begin();
	DirectX::CreateDDSTextureFromFile(&_Device, _Upload, L"media/WoodCrate01.dds", &gTextures["gTexture"]._Impl);
	_Upload.End(_Command._My_command_queue.get());
	gFence.flush(_Command._My_command_queue.ref());

	// create and insert shader_resource_view
	gSRVs = d3d12::CBV_SRV_UAV_array_map<std::string, 100>(_Device, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	gSRVs.insert_or_assign("gTexture", gTextures["gTexture"], d3d12::make_D3D12_SHADER_RESOURCE_VIEW_DESC_with_TEXTURE2D(gTextures["gTexture"]));
}

void init_geometry_resources(ID3D12Device& _Device) {
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
	
	std::vector<norm_hlsl::varying> _Static_vertices(totalVertexCount);
	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k) {
		_Static_vertices[k].position = reinterpret_cast<vector3<float, float>&>(box.Vertices[i].Position);
		_Static_vertices[k].normal   = reinterpret_cast<vector3<float, float>&>(box.Vertices[i].Normal);
		_Static_vertices[k].texcoord = reinterpret_cast<vector2<float, float>&>(box.Vertices[i].TexC);
	}
	
	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k) {
		_Static_vertices[k].position = reinterpret_cast<vector3<float, float>&>(grid.Vertices[i].Position);
		_Static_vertices[k].normal   = reinterpret_cast<vector3<float, float>&>(grid.Vertices[i].Normal);
		_Static_vertices[k].texcoord = reinterpret_cast<vector2<float, float>&>(grid.Vertices[i].TexC);
	}
	
	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k) {
		_Static_vertices[k].position = reinterpret_cast<vector3<float, float>&>(sphere.Vertices[i].Position);
		_Static_vertices[k].normal = reinterpret_cast<vector3<float, float>&>(sphere.Vertices[i].Normal);
		_Static_vertices[k].texcoord = reinterpret_cast<vector2<float, float>&>(sphere.Vertices[i].TexC);
	}
	
	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k) {
		_Static_vertices[k].position = reinterpret_cast<vector3<float, float>&>(cylinder.Vertices[i].Position);
		_Static_vertices[k].normal   = reinterpret_cast<vector3<float, float>&>(cylinder.Vertices[i].Normal);
		_Static_vertices[k].texcoord = reinterpret_cast<vector2<float, float>&>(cylinder.Vertices[i].TexC);
	}
	
	std::vector<uint32_t> _Static_indices;
	_Static_indices.insert(_Static_indices.end(), box.Indices32.begin(), box.Indices32.end());
	_Static_indices.insert(_Static_indices.end(), grid.Indices32.begin(), grid.Indices32.end());
	_Static_indices.insert(_Static_indices.end(), sphere.Indices32.begin(), sphere.Indices32.end());
	_Static_indices.insert(_Static_indices.end(), cylinder.Indices32.begin(), cylinder.Indices32.end());
	
	std::shared_ptr<d3d12::static_mesh<norm_hlsl::varying>> _All_mesh = std::make_shared<d3d12::static_mesh<norm_hlsl::varying>>();
	d3d12::dynamic_buffer<norm_hlsl::varying> _Vertex_intermediator;
	d3d12::dynamic_buffer<uint32_t>           _Index_intermediator;
	{
		auto&   _Source        = _Static_vertices;
		_Vertex_intermediator  = d3d12::make_dynamic_buffer(_Device, _Source);
	}	
	{
		auto&     _Source    = _Static_indices;
		_Index_intermediator = d3d12::make_dynamic_buffer(_Device, _Source);
	}
	_All_mesh->_My_vertices = std::make_shared<d3d12::static_buffer<norm_hlsl::varying>>( _Vertex_intermediator );
	_All_mesh->_My_indices  = std::make_shared<d3d12::static_buffer<uint32_t>>(_Index_intermediator);

	gGeometrys.insert_or_assign("AllGeometry", _All_mesh);
	//gGeometrys["AllGeometry"]->_My_transform = std::make_shared<d3d12::matrix_transform<1, float>>(1.f);
	gGeometrys["Box"] = gGeometrys["AllGeometry"]->reduce(boxVertexOffset, boxIndexOffset, box.Indices32.size());
	gGeometrys["Grid"] = gGeometrys["AllGeometry"]->reduce(gridVertexOffset, gridIndexOffset, grid.Indices32.size());
	gGeometrys["Sphere"] = gGeometrys["AllGeometry"]->reduce(sphereVertexOffset, sphereIndexOffset, sphere.Indices32.size());
	gGeometrys["Cylinder"] = gGeometrys["AllGeometry"]->reduce(cylinderVertexOffset, cylinderIndexOffset, cylinder.Indices32.size());
}

void init_dynamic_geometry_resources(ID3D12Device& _Device) {
	gWaves = Waves(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

	// Waves geometry
	std::vector<std::uint32_t> _Waves_indices(3 * gWaves.TriangleCount()); // 3 indices per face
	assert(gWaves.VertexCount() < 0x0000ffff);

	// Iterate over each quad.
	int m = gWaves.RowCount();
	int n = gWaves.ColumnCount();
	int k = 0;
	for (int i = 0; i < m - 1; ++i)
	{
		for (int j = 0; j < n - 1; ++j)
		{
			_Waves_indices[k] = i * n + j;
			_Waves_indices[k + 1] = i * n + j + 1;
			_Waves_indices[k + 2] = (i + 1) * n + j;

			_Waves_indices[k + 3] = (i + 1) * n + j;
			_Waves_indices[k + 4] = i * n + j + 1;
			_Waves_indices[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}

	auto _Indices = d3d12::make_dynamic_buffer(_Device, _Waves_indices);
	gDynGeometrys["Wave"] = std::make_shared<d3d12::dynamic_mesh<norm_hlsl::varying>>();
	gDynGeometrys["Wave"]->_My_vertices = std::make_shared<d3d12::dynamic_buffer<norm_hlsl::varying>>(_Device, gWaves.VertexCount());
	gDynGeometrys["Wave"]->_My_indices  = std::make_shared<d3d12::dynamic_buffer<uint32_t>>(std::move(_Indices));
	gDynGeometrys["Wave"]->index_count  = _Waves_indices.size();
}

void init_substance_resource(ID3D12Device& _Device) {
	// chemical element of atomic number 22
	//auto _Titanium = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	//_Titanium->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//_Titanium->reflect_value     = { 0.542f, 0.497f, 0.449f };
	//_Titanium->roughness_x       = 0.f;
	//_Titanium->roughness_y       = 0.f;
	//gSubstances["Titanium"] = _Titanium;
	//
	//// chemical element of atomic number 24
	//auto _Chromium = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	//_Chromium->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//_Chromium->reflect_value     = { 0.549f, 0.556f, 0.554f };
	//_Chromium->roughness_x       = 0.f;
	//_Chromium->roughness_y       = 0.f;
	//gSubstances["_Chromium"] = _Chromium;
	//
	//// chemical element of atomic number 26
	//auto _Iron = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	//_Iron->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//_Iron->reflect_value     = { 0.562f, 0.565f, 0.578f };
	//_Iron->roughness_x       = 0.f;
	//_Iron->roughness_y       = 0.f;
	//gSubstances["Iron"] = _Iron;
	//	
	//// chemical elelemtn of atomic number 28
	//auto _Nickel = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	//_Nickel->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//_Nickel->reflect_value     = { 0.660f, 0.609f, 0.526f };
	//_Nickel->roughness_x       = 0.f;
	//_Nickel->roughness_y       = 0.f;
	//gSubstances["Nickel"] = _Nickel;
	//
	//// chemical element of atomic number 78
	//auto _Platinum = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	//_Platinum->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//_Platinum->reflect_value     = { 0.673f, 0.637f, 0.585f };
	//_Platinum->roughness_x       = 0.f;
	//_Platinum->roughness_y       = 0.f;
	//gSubstances["Platinum"] = _Platinum;
	//
	//// chemical element of atomic number 29
	//auto _Copper = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	//_Copper->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//_Copper->reflect_value     = { 0.955f, 0.638f, 0.538f };
	//_Copper->roughness_x       = 0.f;
	//_Copper->roughness_y       = 0.f;
	//gSubstances["Copper"] = _Copper;

	//auto _Palladium = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	//_Palladium->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//_Palladium->reflect_value     = { 0.733f, 0.697f, 0.652f };
	//_Palladium->roughness_x       = 0.f;
	//_Palladium->roughness_y       = 0.f;
	//gSubstances["Palladium"] = _Palladium;
	
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
	auto _Gold = std::make_shared<norm_hlsl::uniform_surface>();
	//_Gold->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	_Gold->set_albedo(gSRVs.get_GPUhandle("gTexture"));
	_Gold->set_reflect( { 1.000f, 0.782f, 0.344f } );
	_Gold->set_roughness(0.6f);
	_Control_roughness = &_Gold->_My_prop.roughness_x;
	gSubstances["Gold"] = _Gold;
	
	// chemical element of atomic number 13
	//auto _Aluminum = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	//_Aluminum->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//_Aluminum->reflect_value     = { 0.913f, 0.922f, 0.924f };
	//_Aluminum->roughness_x       = 0.f;
	//_Aluminum->roughness_y       = 0.f;
	//gSubstances["Aluminum"] = _Aluminum;
	//
	//// chemical element of atomic number 47
	//auto _Silver = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	//_Silver->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//_Silver->reflect_value     = { 0.972f, 0.960f, 0.915f };
	//_Silver->roughness_x       = 0.f;
	//_Silver->roughness_y       = 0.f;
	//gSubstances["Silver"] = _Silver;

	//auto _Diamond = std::make_shared<d3d12::surface_substance<INDEX_SUBSTANCE, float>>();
	//_Diamond->subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//_Diamond->reflect_value     = { 0.171f, 0.172f, 0.176f };
	//_Diamond->roughness_x       = 0.f;
	//_Diamond->roughness_y       = 0.f;
	//gSubstances["Diamond"] = _Diamond;
	
	//// chemical element of atomic number 14
	//CPU_static_traits["Silicon"].roughness_x       = 0.f;
	//CPU_static_traits["Silicon"].roughness_y       = 0.f;
	//CPU_static_traits["Silicon"].subsurface_albedo = { 0.04f, 0.04f, 0.04f };
	//CPU_static_traits["Silicon"].reflect_value     = { 0.345f, 0.369f, 0.426f };
	//CPU_static_traits["Silicon"]._GPU_resource     = &GPU_static_traits;
	//CPU_static_traits["Silicon"]._GPU_resource_index = 14;
	//static_traits.push_back(CPU_static_traits["Diamond"]);
}

void init_object_systems() {
	gPrograms[NORM_HLSL].push_descriptor_heap(gSRVs);

	gPrograms[NORM_HLSL].push_render_object(*gFrameResources[NORM_HLSL]);

	gObjectSystems["box"]._My_transform = std::make_shared<norm_hlsl::uniform_transform>(
		translation<float, float>(0.f, 1.5f, 11.f) * scaling<float, float>(2.f, 2.f, 2.f));
	gObjectSystems["box"]._My_components["1"] = std::make_shared<clmagic::object>();
	gObjectSystems["box"]._My_components["1"]->_My_geometry  = gGeometrys["Box"];
	gObjectSystems["box"]._My_components["1"]->_My_substance = gSubstances["Gold"];
	gPrograms[NORM_HLSL].push_render_object(gObjectSystems["box"]);

	/*gObjectSystems["grid"]._My_transform = std::make_shared<norm_hlsl::uniform_transform>(matrix4x4<float, float>(1.f));
	gObjectSystems["grid"]._My_components["1"] = std::make_shared<clmagic::object>();
	gObjectSystems["grid"]._My_components["1"]->_My_geometry  = gGeometrys["Grid"];
	gObjectSystems["grid"]._My_components["1"]->_My_substance = gSubstances["Gold"];
	gPrograms[NORM_HLSL].push_render_object(gObjectSystems["grid"]);*/
	
	for (int i = 0; i < 5; ++i) {
		gObjectSystems["leftCyl" + std::to_string(i)]._My_transform = std::make_shared<norm_hlsl::uniform_transform>(
			translation<float, float>(-5.0f, 1.5f, -10.0f + i * 5.0f));
		gObjectSystems["leftCyl" + std::to_string(i)]._My_components["1"] = std::make_shared<clmagic::object>();
		gObjectSystems["leftCyl" + std::to_string(i)]._My_components["1"]->_My_geometry  = gGeometrys["Cylinder"];
		gObjectSystems["leftCyl" + std::to_string(i)]._My_components["1"]->_My_substance = gSubstances["Gold"];
		gPrograms[NORM_HLSL].push_render_object(gObjectSystems["leftCyl" + std::to_string(i)]);
	
		gObjectSystems["rightCyl" + std::to_string(i)]._My_transform = std::make_shared<norm_hlsl::uniform_transform>(
			translation<float, float>(+5.0f, 1.5f, -10.0f + i * 5.0f));
		gObjectSystems["rightCyl" + std::to_string(i)]._My_components["1"] = std::make_shared<clmagic::object>();
		gObjectSystems["rightCyl" + std::to_string(i)]._My_components["1"]->_My_geometry  = gGeometrys["Cylinder"];
		gObjectSystems["rightCyl" + std::to_string(i)]._My_components["1"]->_My_substance = gSubstances["Gold"];
		gPrograms[NORM_HLSL].push_render_object(gObjectSystems["rightCyl" + std::to_string(i)]);
	
		gObjectSystems["leftSph" + std::to_string(i)]._My_transform = std::make_shared<norm_hlsl::uniform_transform>(
			translation<float, float>(-5.0f, 3.0f, -10.0f + i * 5.0f));
		gObjectSystems["leftSph" + std::to_string(i)]._My_components["1"] = std::make_shared<clmagic::object>();
		gObjectSystems["leftSph" + std::to_string(i)]._My_components["1"]->_My_geometry  = gGeometrys["Sphere"];
		gObjectSystems["leftSph" + std::to_string(i)]._My_components["1"]->_My_substance = gSubstances["Gold"];
		gPrograms[NORM_HLSL].push_render_object(gObjectSystems["leftSph" + std::to_string(i)]);
	
		gObjectSystems["rightSph" + std::to_string(i)]._My_transform = std::make_shared<norm_hlsl::uniform_transform>(
			translation<float, float>(+5.0f, 3.0f, -10.0f + i * 5.0f));
		gObjectSystems["rightSph" + std::to_string(i)]._My_components["1"] = std::make_shared<clmagic::object>();
		gObjectSystems["rightSph" + std::to_string(i)]._My_components["1"]->_My_geometry  = gGeometrys["Sphere"];
		gObjectSystems["rightSph" + std::to_string(i)]._My_components["1"]->_My_substance = gSubstances["Gold"];
		gPrograms[NORM_HLSL].push_render_object(gObjectSystems["rightSph" + std::to_string(i)]);
	}

	gObjectSystems["sea"]._My_transform = std::make_shared<norm_hlsl::uniform_transform>(matrix4x4<float, float>(1.f));
	gObjectSystems["sea"]._My_components["0"]                = std::make_shared<clmagic::object>();
	gObjectSystems["sea"]._My_components["0"]->_My_geometry  = gDynGeometrys["Wave"];
	gObjectSystems["sea"]._My_components["0"]->_My_substance = gSubstances["Gold"];
	gPrograms[NORM_HLSL].push_render_object(gObjectSystems["sea"]);
}

#include <sstream>

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//std::cout << clmagic::normalize(dddd()) << std::endl;

	try { 
		gWindow.init(hInstance, L"test_graphics", 0, 0, 1600, 900);
		gFence   = d3d12::fence(gWindow.get_ID3D12Device(), 1);
		gWindow.show();

		init_shader_pipeline(gWindow.get_ID3D12Device());
		init_shader_frame_resources();
		init_texture_resources(gWindow.get_ID3D12Device());
		init_geometry_resources(gWindow.get_ID3D12Device());
		init_dynamic_geometry_resources(gWindow.get_ID3D12Device());
		init_substance_resource(gWindow.get_ID3D12Device());
		init_object_systems();
		
		using namespace::d3d12;
		command_allocator     _Cmd_allocator = command_allocator(gWindow.get_ID3D12Device(), D3D12_COMMAND_LIST_TYPE_DIRECT);
		graphics_command_list _Cmd_list      = graphics_command_list(gWindow.get_ID3D12Device(), D3D12_COMMAND_LIST_TYPE_DIRECT, _Cmd_allocator);
		MSG msg = { 0 };

		std::chrono::milliseconds _Total = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());

		//std::vector<clmagic::renderable*> _Renderales;
		while (msg.message != WM_QUIT) {
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			} else {
				auto* _Norm_hlsl_fs = dynamic_cast<norm_hlsl::uniform_frame*>(gFrameResources[NORM_HLSL].get());
				norm_hlsl::uniform_frame::_My_data_type* _Ptr = nullptr;
				_Norm_hlsl_fs->map(reinterpret_cast<void**>(&_Ptr));
				_Ptr->view_matrix  = matrix_cast<matrix4x4<float, float>>(transpose(gWindow._My_camera.view_matrix()));
				_Ptr->eye_position = vector_cast<vector3<float, float>>(gWindow._My_camera.position());
				_Norm_hlsl_fs->unmap(reinterpret_cast<void**>(&_Ptr));
				
				{
					auto  _Now   = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());
					float delta = (_Now - _Total).count() / 1000.0f;
					      _Total = _Now;
				// Every quarter second, generate a random wave.
					static float t_base = 0.0f;
					if((float(_Total.count() / 1000.0) - t_base) >= 0.25f)
					{
						t_base += 0.25f;

						int i = randi(4, gWaves.RowCount() - 5);
						int j = randi(4, gWaves.ColumnCount() - 5);

						float r = randf(0.2f, 0.5f);

						gWaves.Disturb(i, j, r);
					}

					// Update the wave simulation.
					gWaves.Update(delta);

					// Update the wave vertex buffer with the new solution.
					gDynGeometrys["Wave"]->_My_vertices->map();
					auto currWavesVB = gDynGeometrys["Wave"]->_My_vertices->begin();
					for(int i = 0; i < gWaves.VertexCount(); ++i) {
						norm_hlsl::varying v;
						v.position = reinterpret_cast<const clmagic::vector3<float, float>&>(gWaves.Position(i));
						v.normal   = reinterpret_cast<const clmagic::vector3<float, float>&>(gWaves.Normal(i));

						*(currWavesVB + i) = (i, v);
					}
					gDynGeometrys["Wave"]->_My_vertices->unmap();
				}


				gFence.flush(gWindow._My_executer);

				_Cmd_allocator->Reset();
				_Cmd_list->Reset(_Cmd_allocator.get(), nullptr);
				_Cmd_list->ResourceBarrier(1, &gWindow.current_color_buffer().transition(D3D12_RESOURCE_STATE_RENDER_TARGET));
			
				gWindow.clear_current_buffer(_Cmd_list, DirectX::Colors::LightSteelBlue);
				gWindow.set_current_render_target(_Cmd_list);
				_Cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				gPrograms[NORM_HLSL].render(_Cmd_list);
				/*ID3D12DescriptorHeap* _Heaps[] = { gSRVs.get() };
				_Cmd_list->SetDescriptorHeaps(1, _Heaps);
				_Cmd_list->SetPipelineState(gPrograms[NORM_HLSL].get());
				_Cmd_list->SetGraphicsRootSignature(gUniforms[NORM_HLSL].get());
				gFrameResources[NORM_HLSL]->render(_Cmd_list);
				auto       _First = gObjectSystems.begin();
				const auto _Last = gObjectSystems.end();
				for (; _First != _Last; ++_First) {
					_First->second.render(_Cmd_list);
				}*/
	
				_Cmd_list->ResourceBarrier(1, &gWindow.current_color_buffer().transition(D3D12_RESOURCE_STATE_PRESENT));
				_Cmd_list->Close();
				
				gWindow.execute_command_list(_Cmd_list);
				
				gWindow.swap_buffer();
			}
		}
		//d3d12::window::run();
	} catch (const std::exception& e) {
		MessageBoxA(nullptr, e.what(), "Error", MB_OK);
	}

	gFrameResources.clear();
	gObjectSystems.clear();
	gGeometrys.clear();
	gSubstances.clear();
	gUniforms.clear();
	gPrograms.clear();
	gFence.release();
	gWindow.release();

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

	{
		std::cout << make_gravity<vector3<float>, float>(kilograms<float>(10.f))
			             + force<vector3<float>, float>({10.f, 0.f, 0.f}, 10.f) << std::endl;
		std::cout << dot(make_gravity<vector3<float>, float>(kilograms<float>(10.f)), { 0.f, -50.f, 0.f }) << std::endl;
		std::cout << dot(make_gravity<vector3<float>, float>(kilograms<float>(10.f)), { 0.f, -50.f, 0.f }).to_scalar() << std::endl;
		std::cin.get();
	}

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
	using _Tb = clmagic::_SIMD4_t<float>;

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