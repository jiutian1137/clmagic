#include "../src/clmagic/math.h"
#include "../src/clmagic/basic/timer_wheel.h"
#include "../src/clmagic/basic/concurrent_resource.h"
#include "../src/clmagic/directX12/directX12.h"
#include <iostream>
#include <chrono>
#include <DirectXColors.h>
#include "Common/d3dApp.h"
#include "Common/UploadBuffer.h"
#include "Common/GeometryGenerator.h"
#include "Waves.h"

using namespace::clmagic;

struct __declspec(align(256)) uniform_object {
	matrix4x4<float, __m128> world_matrix;
};

struct __declspec(align(256)) uniform_frame {
	matrix4x4<float, __m128> ViewMatrix = matrix4x4<float, __m128>(1.f);// 0
	matrix4x4<float, __m128> ProjMatrix = matrix4x4<float, __m128>(1.f);// 64
	matrix4x4<float, __m128> TempMatrix = matrix4x4<float, __m128>(1.f);// 64
	vector3<float, __m128> EyePosition  = vector3<float, __m128>();// 128
	vector2<float, __m128> Resolution   = vector2<float, __m128>();// 144
	vector4<float, __m128> AmbientLight; // 160
	float TotalTime = 0.f; // 176
	float DeltaTime = 0.f; // 180
};

struct __declspec(align(256)) uniform_RENDERING {
	environment<float>  Env;
	light_source<float> Lights[1];
	surface<float>      Mtl;
};

struct vertex {
	clmagic::vector3<float> position0;
	clmagic::vector3<float> normal0;
	clmagic::vector2<float> texcoord0;
};


struct frame_resource : public dx12::basic_frame_resource {
	frame_resource() = default;

	frame_resource(ID3D12Device& _Device, D3D12_COMMAND_LIST_TYPE _Type, const Waves& _Wave, size_t _Nobj) : dx12::basic_frame_resource(_Device, _Type) {
		// array of descriptor
		cb_PER_OBJECT = dx12::packaged_resource_upload_heap(_Device,
			CD3DX12_RESOURCE_DESC::Buffer(_Nobj * clmagic::ceil(sizeof(uniform_object), 256)));

		// single descriptor
		cb_PER_FRAME = dx12::packaged_resource_upload_heap(_Device,
			CD3DX12_RESOURCE_DESC::Buffer(sizeof(uniform_frame)));
		cb_RENDERING = dx12::packaged_resource_upload_heap(_Device,
			CD3DX12_RESOURCE_DESC::Buffer(sizeof(uniform_RENDERING)));

		per_frame_wave_vertices = std::make_shared<dx12::packaged_resource_upload_heap>(_Device,
			CD3DX12_RESOURCE_DESC::Buffer(_Wave.VertexCount() * sizeof(vertex)) );
	}

	dx12::packaged_resource_upload_heap cb_PER_OBJECT;
	dx12::packaged_resource_upload_heap cb_PER_FRAME;
	dx12::packaged_resource_upload_heap cb_RENDERING;
	std::shared_ptr<dx12::packaged_resource_upload_heap> per_frame_wave_vertices;
};

struct material {
	size_t index = -1;
	surface<float> data;
};

struct actor {
	size_t index = -1;
	concurrent_resource<matrix4x4<float, __m128>> world_matrix;
	material*                 pmtl         = nullptr;
	dx12::basic_mesh<vertex>* pmesh        = nullptr;
	D3D12_PRIMITIVE_TOPOLOGY  primitive    = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};



class ShapeApp : public D3DApp {
	using _Mybase = D3DApp;

public:
	ShapeApp(HINSTANCE hInstance) : _Mybase(hInstance) {}

	virtual bool Initialize() override {
		if (!_Mybase::Initialize())
			return false;

		mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr);

		// 1.
		gWaves = Waves(256, 256, 1.f, 0.03f, 6.0f, 0.2f);
		size_t _Nframe = 1;

		build_geometry();
		build_material(_Nframe);
		build_lights(_Nframe);
		build_object(_Nframe);
		build_framesource(_Nframe);
		build_rootsignature();
		build_pipelinestate();

		mCommandList->Close();
		ID3D12CommandList* _Tasks[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(1, _Tasks);
		FlushCommandQueue();

		return true;
	}

	void build_geometry() {
		GeometryGenerator _Generator;
		auto _Grid = _Generator.CreateGrid(128.f, 128.f, 200, 200);
		const auto HEIGHT = [](float x, float z) { return 0.3f * (z * sinf(0.1f * x)) + x * cosf(0.1f * z); };
		const auto NORMAL = [](float x, float z) {
			// n = (-df/dx, 1, -df/dz)
			vector3<float> n{
				-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
				1.0f,
				-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z) };
			return normalize(n);
		};

		std::vector<vertex> vertices(_Grid.Vertices.size());
		for (size_t i = 0; i < _Grid.Vertices.size(); ++i) {
			auto& p = _Grid.Vertices[i].Position;
			vertices[i].position0[0] = p.x;
			vertices[i].position0[1] = HEIGHT(p.x, p.z);
			vertices[i].position0[2] = p.z;

			vertices[i].normal0 = NORMAL(p.x, p.z);

			vertices[i].texcoord0 = reinterpret_cast<const vector2<float>&>(_Grid.Vertices[i].TexC);
		}
		std::vector<std::uint32_t> _Indices = _Grid.Indices32;
		
		gGeometries["land"] = dx12::static_mesh<vertex>(*md3dDevice.Get(), *mCommandList.Get(), 
			vertices.data(), vertices.size(), _Indices.data(), _Indices.size());

		_Indices.resize(3 * gWaves.TriangleCount()/*(m-1)(n-1)*2*/);
		size_t _Rows = gWaves.RowCount();
		size_t _Cols = gWaves.ColumnCount();
		for (size_t i = 0, k = 0; i < _Rows - 1; ++i) {
			for (size_t j = 0; j < _Cols - 1; ++j) {
				/*
				 (k+5)         (k+3)
				      +----------+ (k+2)
				  /|\ |         /|
				   |  |     /    | /|\
				      | /        |  |
				(k+4) +----------+
				      k    ->    k+1
				*/
				_Indices[k] = i * _Cols + j;
				_Indices[k + 1] = i * _Cols + (j+1);
				_Indices[k + 2] = (i+1) * _Cols + j;
				
				_Indices[k + 3] = (i+1) * _Cols + j;
				_Indices[k + 4] = i * _Cols + (j+1);
				_Indices[k + 5] = (i+1) * _Cols + (j+1);
				k += 6;
			}
		}
		gDynGeometries["wave"] = dx12::dynamic_vertex_mesh<vertex>(*md3dDevice.Get(), _Indices.size());
		gDynGeometries["wave"].assign(*md3dDevice.Get(), *mCommandList.Get(), _Indices.data());
	}

	void build_material(size_t _Nframes) {
		surface<float> _Grass;
		_Grass.subsurface_albedo  = { 0.2f, 0.6f, 0.2f };
		_Grass.subsurface_albedo  = { 0, 0, 0 };
		_Grass.reflect_value      = { 0.913,0.922,0.924 };
		_Grass.roughness          = 0.125f;
		gMaterials["grass"].data  = _Grass;
		gMaterials["grass"].index = 0;

		surface<float> _Water;
		_Water.subsurface_albedo  = { 0.f, 0.2f, 0.6f };
		_Water.reflect_value      = { 0.1f, 0.1f, 0.1f };
		_Water.roughness          = 0.f;
		gMaterials["water"].data  = _Water;
		gMaterials["water"].index = 1;
	}
	
	void build_lights(size_t _Nframes) {
		gLights["light0"].color     = { 0.5f, 0.5f, 0.5f };
		gLights["light0"].intensity = 100.f;
		gLights["light0"].position  = { 0.5f, 50.f, 0.5f };
		gLights["light0"].direction = { 0.f, -1.f, 0.f };
		gLights["light0"].penumbra = 6.28;
		gLights["light0"].umbra    = 6.28;
	}
	
	void build_object(size_t _Nframes) {
		gActors["water"].world_matrix.set(matrix4x4<float, __m128>(1.f), _Nframes);
		gActors["water"].index     = 0;
		gActors["water"].pmesh     = &gDynGeometries["wave"];
		gActors["water"].pmtl      = &gMaterials["water"];
		gActors["water"].primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		gActors["monton"].world_matrix.set(matrix4x4<float, __m128>(1.f), _Nframes);
		gActors["monton"].index     = 1;
		gActors["monton"].pmesh     = &gGeometries["land"];
		gActors["monton"].pmtl      = &gMaterials["grass"];
		gActors["monton"].primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	void build_framesource(size_t _Nframes) {
		for (size_t i = 0; i != _Nframes; ++i) {
			gFrameResources.expend_cycle(frame_resource( *md3dDevice.Get(), mCommandList->GetType(), gWaves, gActors.size() ));
		}
	}

	void build_rootsignature() {
		gInputlayout.push_back("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
		gInputlayout.push_back("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
		gInputlayout.push_back("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);

		// 4.
		std::vector<D3D12_ROOT_PARAMETER> _Parameters(3);
		CD3DX12_ROOT_PARAMETER::InitAsConstantBufferView(_Parameters[0], 0);
		CD3DX12_ROOT_PARAMETER::InitAsConstantBufferView(_Parameters[1], 1);
		CD3DX12_ROOT_PARAMETER::InitAsConstantBufferView(_Parameters[2], 2);
		std::vector<D3D12_STATIC_SAMPLER_DESC> _Samplers;
		gRootSignature = dx12::packaged_root_signature(*md3dDevice.Get(), _Parameters, _Samplers);
	}

	void build_pipelinestate() {
		gShaders["VS"] = hlsl::shader_compile(L"shader/norm.hlsl", nullptr, "VS", "vs_5_0");
		gShaders["PS"] = hlsl::shader_compile(L"shader/norm.hlsl", nullptr, "PS", "ps_5_0");
		/*gShaders["ShadowVS"] = hlsl::shader_compile(L"shader/color.hlsl", nullptr, "ShadowVS", "vs_5_0");
		gShaders["ShadowPS"] = hlsl::shader_compile(L"shader/color.hlsl", nullptr, "ShadowPS", "ps_5_0");*/

		D3D12_GRAPHICS_PIPELINE_STATE_DESC _PSODesc;
		ZeroMemory(&_PSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		_PSODesc.InputLayout       = gInputlayout.get();
		_PSODesc.pRootSignature    = gRootSignature.get();
		_PSODesc.VS                = gShaders["VS"].get_dx12();
		_PSODesc.PS                = gShaders["PS"].get_dx12();
		_PSODesc.RasterizerState   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		_PSODesc.BlendState        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		_PSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		_PSODesc.SampleMask        = UINT_MAX;
		_PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		_PSODesc.NumRenderTargets  = 1;
		_PSODesc.RTVFormats[0]     = mBackBufferFormat;
		_PSODesc.SampleDesc.Count  = m4xMsaaState ? 4 : 1;
		_PSODesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
		_PSODesc.DSVFormat         = mDepthStencilFormat;
		gPipelineStates["opaque"] = dx12::packaged_pipeline_state(*md3dDevice.Get(), _PSODesc);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC _PSODesc2 = _PSODesc;
		_PSODesc2.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		_PSODesc2.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		gPipelineStates["opaque_wireframe"] = dx12::packaged_pipeline_state(*md3dDevice.Get(), _PSODesc2);
	}

	virtual void OnResize() override {
		_Mybase::OnResize();
		_My_proj = clmagic::perspective<matrix4x4<float,__m128>>::get_matrix(radians<float>(degrees<float>(40)), AspectRatio(), 13.f, 10000.f);
	}

	virtual void Update(const GameTimer& gt) override {
		using vector3      = clmagic::vector3<float, __m128>;
		using vector4      = clmagic::vector4<float, __m128>;
		using unit_vector3 = clmagic::unit_vector3<float, __m128>;
		using matrix4x4    = clmagic::matrix4x4<float, __m128>;

		auto Peye = _My_eyepos.to_<vector3>();
		_My_view  = look_at<matrix4x4>::get_matrix(Peye, unit_vector3{ -Peye }, unit_vector3({ 0.f, 1.f, 0.f }, true) );
	

		gFrameResources.turn();// !!!!!
		if (gFrameResources->fence != 0 && mFence->GetCompletedValue() < gFrameResources->fence) {
			HANDLE _Event = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
			mFence->SetEventOnCompletion(gFrameResources->fence, _Event);
			WaitForSingleObject(_Event, INFINITE);
			CloseHandle(_Event);
		}

		// update actor's update
		{
			auto       _First = gActors.begin();
			const auto _Last  = gActors.end();
			auto       _Dest  = gFrameResources->cb_PER_OBJECT.begin<uniform_object>();
			for (; _First != _Last; ++_First, ++_Dest) {
				if ( _First->second.world_matrix.updated() ) {
					_Dest->world_matrix = transpose( _First->second.world_matrix.get() );
				}
			}
		}

		auto& _Cbpass = gFrameResources->cb_PER_FRAME.at<uniform_frame>(0);
		_Cbpass.ViewMatrix  = transpose(_My_view);
		_Cbpass.ProjMatrix  = transpose(_My_proj);
		_Cbpass.TempMatrix  = transpose(planar_projection<float, __m128>::get_matrix(Peye - vector3{10.f, 10.f, 10.f}, { 0.f, 1.f, 0.f }, 0.f));
		_Cbpass.EyePosition = Peye;
		_Cbpass.DeltaTime   = gt.DeltaTime();
		_Cbpass.TotalTime   = gt.TotalTime();
		_Cbpass.AmbientLight = vector4{ 0.1f, 0.1f, 0.1f, 1.f };
		
		/*MessageBoxA(nullptr, to_string(_Pass.view_matrix).c_str(), "view", MB_OK);
		MessageBoxA(nullptr, to_string(_Pass.proj_matrix).c_str(), "view", MB_OK);*/

		// Every quarter second, generate a random wave.
		static float t_base = 0.0f;
		if ((mTimer.TotalTime() - t_base) >= 0.25f) {
			t_base += 0.25f;

			int i = MathHelper::Rand(4, gWaves.RowCount() - 5);
			int j = MathHelper::Rand(4, gWaves.ColumnCount() - 5);

			float r = MathHelper::RandF(0.2f, 0.5f);

			gWaves.Disturb(i, j, r);
		}

		// Update the wave simulation.
		gWaves.Update(gt.DeltaTime());

		// Update the wave vertex buffer with the new solution.
		auto* _Waves_ptr = gFrameResources->per_frame_wave_vertices->ptr<vertex>(0);
		for (int i = 0; i < gWaves.VertexCount(); ++i, ++_Waves_ptr) {
			_Waves_ptr->position0 = reinterpret_cast<const clmagic::vector3<float>&>(gWaves.Position(i));
			_Waves_ptr->normal0   = reinterpret_cast<const clmagic::vector3<float>&>(gWaves.Normal(i));
		}
		// Set the dynamic VB of the wave renderitem to the current frame VB.
		gDynGeometries["wave"].bind_vertex(gFrameResources->per_frame_wave_vertices);
	}

	virtual void Draw(const GameTimer& gt) override {
		auto _Cmdlist_alloc = gFrameResources->command_allocator_ptr();
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

		mCommandList->SetGraphicsRootSignature(gRootSignature.get());
		mCommandList->SetGraphicsRootConstantBufferView(1, gFrameResources->cb_PER_FRAME->GetGPUVirtualAddress());

		// draw
		{
			auto       _First    = gActors.begin();
			const auto _Last     = gActors.end();
			auto       _Gptr_obj = gFrameResources->cb_PER_OBJECT->GetGPUVirtualAddress();
			auto&      _Rendering_zzzz = gFrameResources->cb_RENDERING;
			for (; _First != _Last; ++_First, _Gptr_obj += sizeof(uniform_object)) {
				mCommandList->IASetPrimitiveTopology(_First->second.primitive);
				mCommandList->SetGraphicsRootConstantBufferView(0, _Gptr_obj);
				auto& _Rendering = _Rendering_zzzz.at<uniform_RENDERING>(0);
				_Rendering.Env.particles_ratio = 0.f;
				_Rendering.Env.particles_color = vector3<float>(0.f);
				_Rendering.Lights[0] = gLights["light0"];
				_Rendering.Mtl.subsurface_albedo = _First->second.pmtl->data.subsurface_albedo;
				_Rendering.Mtl.reflect_value = _First->second.pmtl->data.reflect_value;
				_Rendering.Mtl.roughness = _First->second.pmtl->data.roughness;
				mCommandList->SetGraphicsRootConstantBufferView(2, _Rendering_zzzz->GetGPUVirtualAddress());

				const auto& _Mesh = *(_First->second.pmesh);
				mCommandList->IASetVertexBuffers(0, 1, &_Mesh.vertex_view());
				mCommandList->IASetIndexBuffer(&_Mesh.index_view());
				mCommandList->DrawIndexedInstanced(_Mesh.index_count, 1, _Mesh.start_index_location, _Mesh.base_vertex_location, 0);
			}

			/*_First    = gActors.begin();
			_Gptr_obj = gFrameResources->per_object->GetGPUVirtualAddress();
			mCommandList->SetPipelineState(gPipelineStates["shadow"].get());
			for (; _First != _Last; ++_First, _Gptr_obj += sizeof(uniform_object)) {
				if (_First->first != "water") {
					mCommandList->IASetPrimitiveTopology(_First->second.primitive);
					mCommandList->SetGraphicsRootConstantBufferView(0, _Gptr_obj);
					mCommandList->SetGraphicsRootConstantBufferView(1, _Gptr_mtl + (*_First).second.pmtl->index * sizeof(uniform_matrial));
					
					const auto& _Mesh = *(_First->second.pmesh);
					mCommandList->IASetVertexBuffers(0, 1, &_Mesh.vertex_view());
					mCommandList->IASetIndexBuffer(&_Mesh.index_view());
					mCommandList->DrawIndexedInstanced(_Mesh.index_count, 1, _Mesh.start_index_location, _Mesh.base_vertex_location, 0);
				}
			}*/
		}

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		mCommandList->Close();
		ID3D12CommandList* _Cmdlists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(1, _Cmdlists);

		mSwapChain->Present(0, 0);
		mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
		//FlushCommandQueue();
		gFrameResources->fence = ++mCurrentFence;
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
			_My_eyepos.zenith  += radians<float>(dy).num;
			_My_eyepos.azimuth += radians<float>(dx).num;
			_My_eyepos.zenith = clamp( _My_eyepos.zenith, unit_cast<degrees_ratio, radians_ratio>(18.f), unit_cast<degrees_ratio, radians_ratio>(162.f) );
		} else if(btnState & MK_RBUTTON){
			float dx = 0.001f * static_cast<float>(x - _My_LastMousePos.x);
			float dy = 0.001f * static_cast<float>(y - _My_LastMousePos.y);
			//_My_eyepos.radius += dx - dy;
			gLights["light0"].penumbra -= dy;
			gLights["light0"].umbra    -= dx;
			gLights["light0"].umbra = min(gLights["light0"].umbra, gLights["light0"].penumbra);
		}

		_My_LastMousePos.x = x;
		_My_LastMousePos.y = y;
	}

private:
	
	Waves gWaves;
	std::unordered_map<std::string, dx12::static_mesh<vertex>> gGeometries;
	std::unordered_map<std::string, dx12::dynamic_vertex_mesh<vertex>> gDynGeometries;

	std::unordered_map<std::string, actor> gActors;
	timer_wheel<frame_resource> gFrameResources;

	dx12::input_layout gInputlayout;
	dx12::packaged_root_signature gRootSignature;
	std::unordered_map<std::string, Texture>      gTextures;
	std::unordered_map<std::string, material > gMaterials;
	std::unordered_map < std::string, light_source<float>> gLights;
	std::unordered_map<std::string, hlsl::shader> gShaders;
	std::unordered_map<std::string, dx12::packaged_pipeline_state> gPipelineStates;

	matrix4x4<float, __m128> _My_view;
	matrix4x4<float, __m128> _My_proj;

	spherical_coordinate<float> _My_eyepos = spherical_coordinate<float>(100.f, radians<float>(0), radians<float>(3.14159f / 4.0f));
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


template<typename _Ty>
void fffff(_Ty&&) {
	
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

	vector3<double> F0 = { 0.913,0.922,0.924 };
	unit_vector3<double> N  = { 0, 1, 0 };

	/*const int count   = 1000;
	const auto dphi   = 6.28 / static_cast<double>(count);
	const auto dtheta = 3.141592/2 / static_cast<double>(count);
	 auto Rv = 0.0;
	 std::cout << integrate_n([](double theta) { return cos(theta); }, 0.0, 1.57, 1000) << std::endl;*/
	/* const auto f = [](double theta) { return cos(theta) * sin(theta); };
	 std::cout << integrate_n( [f](double dphi) { return integrate_n(f, 0.0, 1.57, 100); }, 0.0, 6.28, 100) << std::endl;
	 std::cout << integrate_n( [f](double dphi) { return integrate_n(f, 0.0, 1.57, 1000); }, 0.0, 6.28, 1000) << std::endl;
	 std::cout << integrate_n( [f](double dphi) { return integrate_n(f, 0.0, 1.57, 2000); }, 0.0, 6.28, 2000) << std::endl;*/


	const auto rad = revolution<float>(1);
	const auto deg = degrees<float>(rad);
	std::cout << rad.num << std::endl;
	std::cout << deg.num << std::endl;
	std::cout << radians<float>(deg) + deg << std::endl;

	auto PPP = clmagic::perspective<matrix4x4<float, __m128>>::get_matrix(static_cast<radians<float>>(degrees<float>(40.0f)), 0.7f, 13.f, 10000.f);
	std::cout << PPP << std::endl;
	// Pi * BRDF(l,v) * Clight * dot(n,l)
	
	//for(double theta = 0.0f; theta <= 3.141592/2; theta += dtheta){
	//	for (double phi = 0.f; phi <= 6.28; phi += dphi) {
	//		//std::cout << "phi:" << phi << "\ttheta:" << theta << std::endl;
	//		const auto L = spherical_coordinate<double>(1.0, phi, theta).to_<vector3<double>>();
	//		const auto dl = sin(theta) * dtheta * dphi;
	//		Rv += dot(N, L)* dl;
	//	}
	//}
	//std::cout << Rv << std::endl;

	/*
	const auto f = [](double x) { return 2.0 / (x* x); };
	const auto a = -2.0;
	const auto b = -1.0;
	std::cout << clmagic::integrate_n(f, a, b, 50) << std::endl;
	std::cout << clmagic::integrate_n(f, a, b, 100) << std::endl;
	std::cout << clmagic::integrate_n(f, a, b, 500) << std::endl;
	std::cout << clmagic::integrate_n(f, a, b, 1000) << std::endl;
	*/
	std::cin.get();
	
	


	//degrees _Angle1 = 0;
	//unit_vector3<float, __m128> _Axis1 = unit_vector3<float, __m128>({0.f, 0.f, 1.f}, true);
	//vector4<float, __m128> _Vec1 = { 20.f, 5.f, 5.f, 1.f };// [-20, -5, 5, 1]

	//for (; _Angle1 <= degrees(180); _Angle1 += 1) {
	//	auto _Quat = WilliamRowanHamilton::polar(_Axis1, _Angle1);
	//	auto _Mat1 = rotation<float, 4, __m128>::get_matrix(_Axis1, _Angle1);
	//	auto _Mat2 = rotation<float, 4, __m128>::get_matrix(_Quat);
	//	clmagic::_Augmented_matrix<float, 4, 4, 4, __m128, __m128> _Mat3(_Mat1, _Mat2);
	//	std::cout << _Quat << "\t" << rotation<float, 4, __m128>::get_quaternion(_Mat2) << std::endl;
	//	//std::cout << _Quat << "\tlength: "<< norm(_Quat) << std::endl;
	//	//std::cout << _Mat3 << std::endl;
	//	//std::this_thread::sleep_for(std::chrono::seconds(1));
	//}

	/*const auto bound = 2;
	for (size_t i = 0; i != 10000; ++i) {
		std::cout << "i: " << i << "\ttrunc: " << trunc(i, bound) << "\tceil: "<<ceil(i, bound)<<"\tfloor: "<<floor(i, bound) << std::endl;
	}*/


	using std::chrono::milliseconds;
	using std::chrono::steady_clock;
	using std::chrono::duration_cast;
	const auto _Axis  = rotation< matrix4x4<float> >::get_matrix(unit_vector3<float>{ 2.f, 3.f, 1.f }, 2);
	const auto _Axis2 = rotation< matrix4x4<float> >::get_matrix(unit_vector3<float>{ 2.f, 3.f, 1.f }, 2);

	for (size_t i = 1000; i != -1; --i) {
		std::cout << i << std::endl;
	}

	size_t _Test_count = 1000;

	milliseconds _Start1 = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());
	for (size_t i = 0; i != _Test_count; ++i) {
		auto _CC = translation< matrix4x4<float> >::get_matrix(_Axis, 10.f, 20.f, 30.f);
	}
	milliseconds _End1 = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());

	milliseconds _Start2 = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());
	for (size_t i = 0; i != _Test_count; ++i) {
		auto _CC = translation< matrix4x4<float> >::get_matrix(_Axis2, 10.f, 20.f, 30.f);
	}
	milliseconds _End2 = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());

	std::cout << "col-major: "<<(_End1 - _Start1).count() << std::endl;
	std::cout << "row-major: "<<(_End2 - _Start2).count() << std::endl;
	matrix<float, 4, 1> _CCCC = { 1.f, 2.f, 3.f, 1.f };
	matrix<float, 1, 4, float, false> _CCCC2 = { 1.f, 2.f, 3.f, 1.f };
	std::cout << translation<matrix4x4<float>>::get_matrix(matrix<float, 4, 4>(1.f), 10.f, 20.f, 30.f)* _CCCC << std::endl;
	std::cout << _CCCC2 * translation<matrix4x4<float, float, false>>::get_matrix(matrix<float, 4, 4>(1.f), 10.f, 20.f, 30.f) << std::endl;

	return 0;
}