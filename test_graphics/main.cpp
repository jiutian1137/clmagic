#include "../src/clmagic/math.h"
#include "../src/clmagic/basic/timer_wheel.h"
#include "../src/clmagic/directX12/directX12.h"
#include <iostream>
#include <chrono>
#include <DirectXColors.h>
#include "Common/d3dApp.h"
#include "Common/UploadBuffer.h"
#include "Common/GeometryGenerator.h"
#include "Waves.h"

using namespace::clmagic;

struct __declspec(align(256)) object_constants {
	matrix4x4<float, __m128> world_matrix;
};

struct __declspec(align(256)) pass_constants {
	matrix4x4<float, __m128> ViewMatrix = matrix4x4<float, __m128>(1.f);// 0
	matrix4x4<float, __m128> ProjMatrix = matrix4x4<float, __m128>(1.f);// 64
	vector3<float, __m128> EyePosition  = vector3<float, __m128>();// 128
	vector2<float, __m128> Resolution   = vector2<float, __m128>();// 144
	vector4<float, __m128> AmbientLight; // 160
	float TotalTime = 0.f; // 176
	float DeltaTime = 0.f; // 180
	float _Pand; // 184
	float _Pand2; // 188;
	Light Lights[16];// 192
};

struct __declspec(align(256)) material_constants {
	vector4<float> diffuse;
	vector3<float> fresnelR0;
	float          roughness;
};


struct vertex {
	clmagic::vector3<float> position0;
	clmagic::vector3<float> normal0;
	clmagic::vector2<float> texcoord0;
};


struct frame_resource : public dx12::basic_frame_resource {
	frame_resource() = default;

	frame_resource(ID3D12Device& _Device, D3D12_COMMAND_LIST_TYPE _Type, const Waves& _Wave, size_t _Nobj, size_t _Nmtl)
		: dx12::basic_frame_resource(_Device, _Type),
		cb_pass(_Device, CD3DX12_RESOURCE_DESC::Buffer(clmagic::ceil(sizeof(pass_constants), 256))),
		cb_object(_Device, CD3DX12_RESOURCE_DESC::Buffer(_Nobj * clmagic::ceil(sizeof(object_constants), 256))),
		cb_material(_Device, CD3DX12_RESOURCE_DESC::Buffer(_Nmtl* clmagic::ceil(sizeof(material_constants), 256))),
		wave_vertices(std::make_shared<dx12::packaged_resource_upload_heap>(_Device, CD3DX12_RESOURCE_DESC::Buffer(_Wave.VertexCount() * sizeof(vertex)))) {}

	dx12::packaged_resource_upload_heap cb_pass;
	dx12::packaged_resource_upload_heap cb_object;
	dx12::packaged_resource_upload_heap cb_material;
	std::shared_ptr<dx12::packaged_resource_upload_heap> wave_vertices;
};



struct actor {
	size_t index = -1;
	matrix4x4<float, __m128>  world_matrix = matrix4x4<float, __m128>(1.f);
	Material*                 pmtl         = nullptr;
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
		gWaves = Waves(128, 128, 1.0, 0.03f, 6.0f, 0.2f);
		
		// 2.
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


		// 3.
		auto& _Grass_material = gMaterials["grass"];
		_Grass_material.Name          = "grass";
		_Grass_material.MatCBIndex    = 0;
		_Grass_material.DiffuseAlbedo = DirectX::XMFLOAT4{ 0.2f, 0.6f, 0.2f, 1.0f };
		_Grass_material.FresnelR0     = DirectX::XMFLOAT3{ 0.01f, 0.01f, 0.01f };
		_Grass_material.Roughness     = 0.125f;
		auto& _Water_material = gMaterials["water"];
		_Water_material.Name          = "water";
		_Water_material.MatCBIndex    = 1;
		_Water_material.DiffuseAlbedo = DirectX::XMFLOAT4{ 0.f, 0.2f, 0.6f, 1.f };
		_Water_material.FresnelR0     = DirectX::XMFLOAT3{ 0.1f, 0.1f, 0.1f };
		_Water_material.Roughness     = 0.f;

		// 4.
		Light _Light0;
		DirectX::XMVECTOR lightDir = -MathHelper::SphericalToCartesian(1.0f, sun_theta.to_floating(), sun_phi.to_floating());
		_Light0.Direction = reinterpret_cast<const DirectX::XMFLOAT3&>(lightDir);
		_Light0.Strength = { 1.f, 1.f, 0.9f };
		gLights.push_back(_Light0);

		// 5.
		actor _Actor0;
		_Actor0.world_matrix = matrix4x4<float, __m128>(1.f);
		_Actor0.index = 0;
		_Actor0.pmesh = &gDynGeometries["wave"];
		_Actor0.pmtl  = &gMaterials["water"];
		_Actor0.primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		gActors.push_back(_Actor0);
		actor _Actor1;
		_Actor1.world_matrix = matrix4x4<float, __m128>(1.f);
		_Actor1.index = 1;
		_Actor1.pmesh = &gGeometries["land"];
		_Actor1.pmtl  = &gMaterials["grass"];
		_Actor1.primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		gActors.push_back(_Actor1);

		// 2.
		size_t _Nframes = 1;
		for (size_t i = 0; i != _Nframes; ++i) {
			gFrameResources.expend_cycle(frame_resource(*md3dDevice.Get(), mCommandList->GetType(), gWaves, gActors.size(), gMaterials.size()));
		}
		
		// 3.
		gInputlayout.push_back("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
		gInputlayout.push_back("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
		gInputlayout.push_back("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);

		// 4.
		std::vector<D3D12_ROOT_PARAMETER> _Parameters(3);
		CD3DX12_ROOT_PARAMETER::InitAsConstants(_Parameters[0], 16, 0);
		CD3DX12_ROOT_PARAMETER::InitAsConstantBufferView(_Parameters[1], 1);
		CD3DX12_ROOT_PARAMETER::InitAsConstantBufferView(_Parameters[2], 2);
		std::vector<D3D12_STATIC_SAMPLER_DESC> _Samplers;
		gRootSignature = dx12::packaged_root_signature(*md3dDevice.Get(), _Parameters, _Samplers);

		// 5.
		gShaders["VS"] = hlsl::shader_compile(L"shader/color.hlsl", nullptr, "VS", "vs_5_0");
		gShaders["PS"] = hlsl::shader_compile(L"shader/color.hlsl", nullptr, "PS", "ps_5_0");

		// 7.
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



		mCommandList->Close();
		ID3D12CommandList* _Tasks[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(1, _Tasks);
		FlushCommandQueue();

		return true;
	}

	virtual void OnResize() override {
		_Mybase::OnResize();
		_My_proj = clmagic::PerspectiveLH<float, __m128>::get_matrix(clmagic::degrees(40), AspectRatio(), 13.f, 10000.f);
	}

	virtual void Update(const GameTimer& gt) override {
		auto _Phi   = phi.to_floating();
		auto _Theta = theta.to_floating();
		float x = radius * sinf(_Phi) * cosf(_Theta);
		float z = radius * sinf(_Phi) * sinf(_Theta);
		float y = radius * cosf(_Phi);
		_My_view = clmagic::LookatLH<float, __m128>::get_matrix(vector3<float, __m128>{ x, y, z }, 
			unit_vector3<float, __m128>{ -x, -y, -z }, 
			unit_vector3<float, __m128>({ 0.f, 1.f, 0.f }, true) );
	
		gFrameResources.turn();// !!!!!
		if (gFrameResources->fence != 0 && mFence->GetCompletedValue() < gFrameResources->fence) {
			HANDLE _Event = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
			mFence->SetEventOnCompletion(gFrameResources->fence, _Event);
			WaitForSingleObject(_Event, INFINITE);
			CloseHandle(_Event);
		}

		for (size_t i = 0; i != gActors.size(); ++i) {
			const auto& _Actor = gActors[i];
			gFrameResources->cb_object.at<object_constants>(_Actor.index).world_matrix = transpose(_Actor.world_matrix);
		}

		for (auto _First = gMaterials.begin(); _First != gMaterials.end(); ++_First) {
			const auto& _Src = _First->second;
			auto&       _Dst = gFrameResources->cb_material.at<material_constants>(_Src.MatCBIndex);
			_Dst.diffuse   = reinterpret_cast<const vector4<float>&>(_Src.DiffuseAlbedo);
			_Dst.fresnelR0 = reinterpret_cast<const vector3<float>&>(_Src.FresnelR0);
			_Dst.roughness = _Src.Roughness;
		}

		auto& _Cbpass = gFrameResources->cb_pass.at<pass_constants>(0);
		_Cbpass.ViewMatrix = transpose(_My_view);
		_Cbpass.ProjMatrix = transpose(_My_proj);
		_Cbpass.EyePosition = vector3<float, __m128>{ x, y, z };
		_Cbpass.DeltaTime = gt.DeltaTime();
		_Cbpass.TotalTime = gt.TotalTime();
		_Cbpass.AmbientLight = vector4<float, __m128>{ 0.1f, 0.1f, 0.1f, 1.f };
		for (size_t i = 0; i != gLights.size(); ++i) {
			_Cbpass.Lights[i] = gLights[i];
		}
		/*MessageBoxA(nullptr, to_string(_Pass.view_matrix).c_str(), "view", MB_OK);
		MessageBoxA(nullptr, to_string(_Pass.proj_matrix).c_str(), "view", MB_OK);*/

		// Every quarter second, generate a random wave.
		static float t_base = 0.0f;
		if ((mTimer.TotalTime() - t_base) >= 0.25f)
		{
			t_base += 0.25f;

			int i = MathHelper::Rand(4, gWaves.RowCount() - 5);
			int j = MathHelper::Rand(4, gWaves.ColumnCount() - 5);

			float r = MathHelper::RandF(0.2f, 0.5f);

			gWaves.Disturb(i, j, r);
		}

		// Update the wave simulation.
		gWaves.Update(gt.DeltaTime());

		// Update the wave vertex buffer with the new solution.
		auto* _Waves_ptr = gFrameResources->wave_vertices->ptr<vertex>(0);
		for (int i = 0; i < gWaves.VertexCount(); ++i, ++_Waves_ptr) {
			_Waves_ptr->position0 = reinterpret_cast<const vector3<float>&>(gWaves.Position(i));
			_Waves_ptr->normal0   = reinterpret_cast<const vector3<float>&>(gWaves.Normal(i));
		}
		// Set the dynamic VB of the wave renderitem to the current frame VB.
		gDynGeometries["wave"].bind_vertex(gFrameResources->wave_vertices);
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
		mCommandList->SetGraphicsRootConstantBufferView(2, gFrameResources->cb_pass->GetGPUVirtualAddress());

		// draw
		for (size_t i = 0; i < gActors.size(); ++i) {
			const auto& _Item = gActors[i];
			mCommandList->IASetVertexBuffers(0, 1, &_Item.pmesh->vertex_view());
			mCommandList->IASetIndexBuffer(&_Item.pmesh->index_view());
			mCommandList->IASetPrimitiveTopology(_Item.primitive);
			mCommandList->SetGraphicsRoot32BitConstants(0, 16, _Item.world_matrix.ptr(), 0);
			mCommandList->SetGraphicsRootConstantBufferView(1,
				gFrameResources->cb_material->GetGPUVirtualAddress() + _Item.pmtl->MatCBIndex * sizeof(material_constants));

			mCommandList->DrawIndexedInstanced(_Item.pmesh->index_count, 1, _Item.pmesh->start_index_location, 
				_Item.pmesh->base_vertex_location, 0);
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
			auto dx = degrees(cvtfloating_rational<intmax_t>(0.10f * static_cast<float>(x - _My_LastMousePos.x)));
			auto dy = degrees(cvtfloating_rational<intmax_t>(0.10f * static_cast<float>(y - _My_LastMousePos.y)));
			theta += dx;
			phi   += dy;
			phi = clmagic::clamp(phi, degrees(18), degrees(162));
		} else if(btnState & MK_RBUTTON){
			float dx = 0.25f * static_cast<float>(x - _My_LastMousePos.x);
			float dy = 0.25f * static_cast<float>(y - _My_LastMousePos.y);
			radius += dx - dy;
		}

		_My_LastMousePos.x = x;
		_My_LastMousePos.y = y;
	}

private:
	
	Waves gWaves;
	std::unordered_map<std::string, dx12::static_mesh<vertex>> gGeometries;
	std::unordered_map<std::string, dx12::dynamic_vertex_mesh<vertex>> gDynGeometries;

	std::vector<actor> gActors;
	timer_wheel<frame_resource> gFrameResources;

	dx12::input_layout gInputlayout;
	dx12::packaged_root_signature gRootSignature;
	std::unordered_map<std::string, Texture>      gTextures;
	std::unordered_map<std::string, Material>     gMaterials;
	std::unordered_map<std::string, hlsl::shader> gShaders;
	std::vector<Light>                            gLights;
	std::unordered_map<std::string, dx12::packaged_pipeline_state> gPipelineStates;

	matrix4x4<float, __m128> _My_view;
	matrix4x4<float, __m128> _My_proj;

	clmagic::degrees theta  = 270;
	clmagic::degrees phi    = 45;
	float            radius = 50.f;
	degrees          sun_theta = 270;
	degrees          sun_phi = 45;
	POINT _My_LastMousePos;
};


int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	/*pass_constants DDD; 
	std::cout << size_t(DDD.Lights) - size_t(&DDD) << std::endl;
	std::string _Str = std::string("Light, offset: ") + std::to_string(size_t(DDD.Lights) - size_t(&DDD));
	_Str += "size: " + std::to_string(size_t(&DDD.Lights[1]) - size_t(&DDD.Lights[0]));
	MessageBoxA(nullptr, _Str.c_str(), "mes", MB_OK);*/
	// offset 184, size: 48

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

int main() {

	using namespace::clmagic;
	degrees _Angle1 = 0;
	unit_vector3<float, __m128> _Axis1 = unit_vector3<float, __m128>({0.f, 0.f, 1.f}, true);
	vector4<float, __m128> _Vec1 = { 20.f, 5.f, 5.f, 1.f };// [-20, -5, 5, 1]

	for (; _Angle1 <= degrees(180); _Angle1 += 1) {
		auto _Quat = WilliamRowanHamilton::polar(_Axis1, _Angle1);
		auto _Mat1 = rotation<float, 4, __m128>::get_matrix(_Axis1, _Angle1);
		auto _Mat2 = rotation<float, 4, __m128>::get_matrix(_Quat);
		clmagic::_Augmented_matrix<float, 4, 4, 4, __m128, __m128> _Mat3(_Mat1, _Mat2);
		//std::cout << _Quat << "\tlength: "<< norm(_Quat) << std::endl;
		std::cout << _Mat3 << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}



	/*const auto bound = 2;
	for (size_t i = 0; i != 10000; ++i) {
		std::cout << "i: " << i << "\ttrunc: " << trunc(i, bound) << "\tceil: "<<ceil(i, bound)<<"\tfloor: "<<floor(i, bound) << std::endl;
	}*/
	
	std::cout << scaling<float>::get_matrix(1.f, 2.f, 3.f) << std::endl;
	std::cout << scaling<float>::get_matrix(10.f) << std::endl;
	std::cin.get();


	using std::chrono::milliseconds;
	using std::chrono::steady_clock;
	using std::chrono::duration_cast;
	const auto _Axis = rotation<float, 4, float>::get_matrix(unit_vector3<float>{ 2.f, 3.f, 1.f }, 2);
	const auto _Axis2 = rotation<float, 4, float>::get_matrix(unit_vector3<float>{ 2.f, 3.f, 1.f }, 2);

	for (size_t i = 1000; i != -1; --i) {
		std::cout << i << std::endl;
	}

	size_t _Test_count = 1000000;

	milliseconds _Start1 = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());
	for (size_t i = 0; i != _Test_count; ++i) {
		auto _CC = translation<float, float, true>::get_matrix(_Axis, 10.f, 20.f, 30.f);
	}
	milliseconds _End1 = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());

	milliseconds _Start2 = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());
	for (size_t i = 0; i != _Test_count; ++i) {
		auto _CC = translation<float, float, false>::get_matrix(_Axis2, 10.f, 20.f, 30.f);
	}
	milliseconds _End2 = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());

	std::cout << "col-major: "<<(_End1 - _Start1).count() << std::endl;
	std::cout << "row-major: "<<(_End2 - _Start2).count() << std::endl;

	return 0;
}