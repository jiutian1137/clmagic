#include "../src/clmagic/math.h"
#include "../src/clmagic/directX12/directX12.h"
#include <iostream>
#include <chrono>
#include <DirectXColors.h>
#include "Common/d3dApp.h"
#include "Common/UploadBuffer.h"
#include "Common/GeometryGenerator.h"

using namespace::clmagic;

struct __declspec(align(256)) object_constants {
	matrix4x4<float, __m128> world_matrix;
};

struct __declspec(align(256)) pass_constants {
	matrix4x4<float, __m128> view_matrix = matrix4x4<float, __m128>(1.f);// 64
	matrix4x4<float, __m128> proj_matrix = matrix4x4<float, __m128>(1.f);// 128
	vector3<float, __m128> eye_position  = vector3<float, __m128>();// 144
	vector2<float, __m128> resolution    = vector2<float, __m128>();// 160
	float total_time = 0.f;// 164
	float delta_time = 0.f;// 168
	// 176
};




struct frame_resource : public dx12::basic_frame_resource {
	frame_resource() = default;

	frame_resource(ID3D12Device& _Device, D3D12_COMMAND_LIST_TYPE _Type, size_t _Nobj)
		: dx12::basic_frame_resource(_Device, _Type),
		cb_pass(_Device, CD3DX12_RESOURCE_DESC::Buffer(clmagic::ceil(sizeof(pass_constants), 256))),
		cb_object(_Device, CD3DX12_RESOURCE_DESC::Buffer(_Nobj * clmagic::ceil(sizeof(object_constants), 256))) {}

	dx12::packaged_resource_upload_heap cb_pass;
	dx12::packaged_resource_upload_heap cb_object;
};

struct vertex {
	clmagic::vector3<float> position0;
	clmagic::vector4<float> color0;
};


struct actor {
	matrix4x4<float, __m128> world_matrix = matrix4x4<float, __m128>(1.f);
	D3D12_PRIMITIVE_TOPOLOGY primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	dx12::static_mesh<vertex>* pmesh = nullptr;
	size_t index = -1;
};




class ShapeApp : public D3DApp {
	using _Mybase = D3DApp;

public:
	ShapeApp(HINSTANCE hInstance) : _Mybase(hInstance) {}

	virtual bool Initialize() override {
		if (!_Mybase::Initialize())
			return false;

		mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr);

		build_geometry();
		build_render_items();
		build_frame_resources(3);
		build_constant_buffer_view();
		build_root_signature();
		build_shader_inputlayout();
		build_pipeline_state();

		mCommandList->Close();
		ID3D12CommandList* _Tasks[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(1, _Tasks);
		FlushCommandQueue();

		return true;
	}

	virtual void OnResize() override {
		_Mybase::OnResize();
		_My_proj = clmagic::PerspectiveLH<float, __m128>::get_matrix(clmagic::degrees(40), AspectRatio(), 1.f, 1000.f);
	}

	virtual void Update(const GameTimer& gt) override {
		auto _Phi   = phi.to_floating();
		auto _Theta = theta.to_floating();
		float x = radius * sinf(_Phi) * cosf(_Theta);
		float z = radius * sinf(_Phi) * sinf(_Theta);
		float y = radius * cosf(_Phi);
		_My_view = clmagic::LookatLH<float, __m128>::get_matrix(vector3<float, __m128>{ x, y, z }, unit_vector3<float, __m128>{ -x, -y, -z }, unit_vector3<float, __m128>({ 0.f, 1.f, 0.f }, true));
	
		_My_frame_resource_current_index = (_My_frame_resource_current_index + 1) % _My_frame_resources.size();
		_My_frame_resource_current = &_My_frame_resources[_My_frame_resource_current_index];

		if (_My_frame_resource_current->fence != 0 && mFence->GetCompletedValue() < _My_frame_resource_current->fence) {
			HANDLE _Event = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
			mFence->SetEventOnCompletion(_My_frame_resource_current->fence, _Event);
			WaitForSingleObject(_Event, INFINITE);
			CloseHandle(_Event);
		}

		for (size_t i = 0; i != _My_render_items_all.size(); ++i) {
			const auto& _Actor = _My_render_items_all[i];
			_My_frame_resource_current->cb_object.at<object_constants>(i).world_matrix = transpose(_Actor->world_matrix);
			//MessageBoxA(nullptr, to_string(_Cb.world_matrix).c_str(), "world", MB_OK);
		}

		auto& _Cbpass = _My_frame_resource_current->cb_pass.at<pass_constants>(0);
		pass_constants _Pass;
		_Cbpass.delta_time = gt.DeltaTime();
		_Cbpass.total_time = gt.TotalTime();
		_Cbpass.eye_position = vector3<float, __m128>{ x, y, z };
		_Cbpass.view_matrix = transpose(_My_view);
		_Cbpass.proj_matrix = transpose(_My_proj);
		/*MessageBoxA(nullptr, to_string(_Pass.view_matrix).c_str(), "view", MB_OK);
		MessageBoxA(nullptr, to_string(_Pass.proj_matrix).c_str(), "view", MB_OK);*/
	}

	virtual void Draw(const GameTimer& gt) override {
		auto _Cmdlist_alloc = _My_frame_resource_current->command_allocator_ptr();
		_Cmdlist_alloc->Reset();
		mCommandList->Reset(_Cmdlist_alloc, _My_pipeline_states["opaque_wireframe"].get());

		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		
		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		ID3D12DescriptorHeap* _Heaps[] = { _My_descriptor_heap.get() };
		mCommandList->SetDescriptorHeaps(1, _Heaps);
		mCommandList->SetGraphicsRootSignature(_My_root_signature.get());
		size_t _Pass_index  = _My_render_items_all.size() * _My_frame_resources.size() + _My_frame_resource_current_index;
		auto   _Pass_handle = _My_descriptor_heap.gpu_handle(_Pass_index, mCbvSrvUavDescriptorSize);
		mCommandList->SetGraphicsRootDescriptorTable(1, _Pass_handle);

		// draw
		auto _Cbobject = _My_frame_resource_current->cb_object.get();
		for (size_t i = 0; i < _My_render_items_all.size(); ++i) {
			const auto& _Item = _My_render_items_all[i];
			mCommandList->IASetVertexBuffers(0, 1, &_Item->pmesh->vbv());
			mCommandList->IASetIndexBuffer(&_Item->pmesh->ibv());
			mCommandList->IASetPrimitiveTopology(_Item->primitive);


			size_t _Obj_index  = _My_frame_resource_current_index * _My_render_items_all.size() + i;
			auto   _Obj_handle = _My_descriptor_heap.gpu_handle(_Obj_index, mCbvSrvUavDescriptorSize);

			mCommandList->SetGraphicsRootDescriptorTable(0, _Obj_handle);
			mCommandList->DrawIndexedInstanced(_Item->pmesh->_Index_count, 1, _Item->pmesh->_Start_index_location, _Item->pmesh->_Base_vertex_location, 0);
		}

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		mCommandList->Close();
		ID3D12CommandList* _Cmdlists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(1, _Cmdlists);

		mSwapChain->Present(0, 0);
		mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
		//FlushCommandQueue();
		_My_frame_resource_current->fence = ++mCurrentFence;
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
			float dx = 0.05f * static_cast<float>(x - _My_LastMousePos.x);
			float dy = 0.05f * static_cast<float>(y - _My_LastMousePos.y);
			radius += dx - dy;
		}

		_My_LastMousePos.x = x;
		_My_LastMousePos.y = y;
	}

private:
	void build_geometry() {
		GeometryGenerator _Generator;
		auto _Box = _Generator.CreateBox(1.5f, 0.5f, 1.5f, 3);
		auto _Grid = _Generator.CreateGrid(20.f, 30.f, 60, 40);
		auto _Sphere = _Generator.CreateSphere(0.5f, 32, 32);
		auto _Cylinder = _Generator.CreateCylinder(0.5, 0.3f, 3.f, 32, 32);
	
		size_t _Box_vertex_offset = 0;
		size_t _Grid_vertex_offset = _Box.Vertices.size();
		size_t _Sphere_vertex_offset = _Grid_vertex_offset + _Grid.Vertices.size();
		size_t _Cylinder_vertex_offset = _Sphere_vertex_offset + _Sphere.Vertices.size();

		size_t _Box_index_offset = 0;
		size_t _Grid_index_offset = _Box.Indices32.size();
		size_t _Sphere_index_offset = _Grid_index_offset + _Grid.Indices32.size();
		size_t _Cylinder_index_offset = _Sphere_index_offset + _Sphere.Indices32.size();

		const auto _Total_vertices = _Cylinder.Vertices.size() + _Cylinder_vertex_offset;
		const auto _Total_indices = _Cylinder.Indices32.size() + _Cylinder_index_offset;
		
		std::vector<vertex> _Vertices(_Total_vertices);
		{
			auto _First1 = _Box.Vertices.begin();
			auto _Last1  = _Box.Vertices.end();
			auto _Dest   = _Vertices.begin();
			for (; _First1 != _Last1; ++_First1, ++_Dest) {
				_Dest->position0 = reinterpret_cast<clmagic::vector3<float>&>(_First1->Position);
				_Dest->color0    = reinterpret_cast<const clmagic::vector4<float>&>( DirectX::Colors::DarkGreen );
			}
		
			_First1 = _Grid.Vertices.begin();
			_Last1  = _Grid.Vertices.end();
			for (; _First1 != _Last1; ++_First1, ++_Dest) {
				_Dest->position0 = reinterpret_cast<clmagic::vector3<float>&>(_First1->Position);
				_Dest->color0    = reinterpret_cast<const clmagic::vector4<float>&>( DirectX::Colors::ForestGreen );
			}

			_First1 = _Sphere.Vertices.begin();
			_Last1  = _Sphere.Vertices.end();
			for (; _First1 != _Last1; ++_First1, ++_Dest) {
				_Dest->position0 = reinterpret_cast<clmagic::vector3<float>&>(_First1->Position);
				_Dest->color0    = reinterpret_cast<const clmagic::vector4<float>&>( DirectX::Colors::Crimson );
			}

			_First1 = _Cylinder.Vertices.begin();
			_Last1  = _Cylinder.Vertices.end();
			for (; _First1 != _Last1; ++_First1, ++_Dest) {
				_Dest->position0 = reinterpret_cast<clmagic::vector3<float>&>(_First1->Position);
				_Dest->color0    = reinterpret_cast<const clmagic::vector4<float>&>( DirectX::Colors::SteelBlue );
			}
			assert(_Dest == _Vertices.end());
		}

		std::vector<uint32_t> _Indices(_Total_indices);
		{
			auto _Dest = _Indices.begin();
			_Dest = std::move(_Box.Indices32.begin(), _Box.Indices32.end(), _Dest);
			_Dest = std::move(_Grid.Indices32.begin(), _Grid.Indices32.end(), _Dest);
			_Dest = std::move(_Sphere.Indices32.begin(), _Sphere.Indices32.end(), _Dest);
			_Dest = std::move(_Cylinder.Indices32.begin(), _Cylinder.Indices32.end(), _Dest);
			assert(_Dest == _Indices.end());
		}

		auto& _Mesh = _My_geometries["all"];
		_Mesh = dx12::static_mesh<vertex>(*md3dDevice.Get(), _Vertices.size(), _Indices.size());
		_Mesh.assign(*md3dDevice.Get(), *mCommandList.Get(), _Vertices.data(), _Indices.data());

		_Mesh.submesh["box"] = dx12::static_mesh<vertex>(_Mesh, _Box_vertex_offset, _Box_index_offset, _Box.Indices32.size());
		_Mesh.submesh["grid"] = dx12::static_mesh<vertex>(_Mesh, _Grid_vertex_offset, _Grid_index_offset, _Grid.Indices32.size());
		_Mesh.submesh["sphere"] = dx12::static_mesh<vertex>(_Mesh, _Sphere_vertex_offset, _Sphere_index_offset, _Sphere.Indices32.size());
		_Mesh.submesh["cylinder"] = dx12::static_mesh<vertex>(_Mesh, _Cylinder_vertex_offset, _Cylinder_index_offset, _Cylinder.Indices32.size());
	}

	void build_render_items() {
		using Translation = clmagic::Translation<float, __m128>;

		auto _Item_box = std::unique_ptr<actor>(new actor());
		_Item_box->world_matrix = Translation::get_matrix(0.f, 0.5f, 0.f)
			* clmagic::diagonal_matrix<float, 4, 4, __m128>(clmagic::vector4<float, __m128>{ 2.f, 2.f, 2.f, 1.f });
		_Item_box->index = 0;
		_Item_box->pmesh     = &_My_geometries["all"].submesh["box"];
		_Item_box->primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_My_render_items_all.push_back(std::move(_Item_box));

		auto _Item_grid = std::unique_ptr<actor>(new actor());
		_Item_grid->world_matrix = matrix4x4<float, __m128>(1.f);
		_Item_grid->index = 1;
		_Item_grid->pmesh = &_My_geometries["all"].submesh["grid"];
		_Item_grid->primitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_My_render_items_all.push_back(std::move(_Item_grid));
	}

	void build_frame_resources(size_t _Nframe) {
		for (size_t i = 0; i != _Nframe; ++i) {
			_My_frame_resources.push_back(
				frame_resource(*md3dDevice.Get(), mCommandList->GetType(), _My_render_items_all.size()));
		}
		_My_frame_resource_current = &_My_frame_resources[0];
		_My_frame_resource_current_index = 0;
	}
	
	void build_constant_buffer_view() {
		auto _Size = _My_render_items_all.size();

		// descriptor-heap is equivalent to declaring a constant-resource 
		_My_descriptor_heap = dx12::packaged_descriptor_heap(*md3dDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			_My_frame_resources.size() * (_Size + 1), D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

		for (size_t frame = 0; frame < _My_frame_resources.size(); ++frame) {
			ID3D12Resource* _Resource = _My_frame_resources[frame].cb_object.get();

			for (size_t i = 0; i < _Size; ++i) {
				auto _Desc = D3D12_CONSTANT_BUFFER_VIEW_DESC();
				     _Desc.SizeInBytes    = clmagic::ceil(sizeof(object_constants), 256);
					 _Desc.BufferLocation = _Resource->GetGPUVirtualAddress();
					 _Desc.BufferLocation += i * _Desc.SizeInBytes;// offset
				
				auto _Dest = _My_descriptor_heap.cpu_handle(frame * _Size + i, mCbvSrvUavDescriptorSize);

				md3dDevice->CreateConstantBufferView(&_Desc, _Dest);
			}
		}

		for (size_t frame = 0; frame < _My_frame_resources.size(); ++frame) {
			ID3D12Resource* _Resource = _My_frame_resources[frame].cb_pass.get();

			auto _Desc = D3D12_CONSTANT_BUFFER_VIEW_DESC();
			     _Desc.BufferLocation = _Resource->GetGPUVirtualAddress();
			     _Desc.SizeInBytes    = clmagic::ceil(sizeof(pass_constants), 256);
			
			auto _Dest = _My_descriptor_heap.cpu_handle(_My_render_items_all.size() * _My_frame_resources.size() + frame, mCbvSrvUavDescriptorSize);
			
			md3dDevice->CreateConstantBufferView(&_Desc, _Dest);
		}
	}

	void build_root_signature() {// A root signature is an array of root parameters.
		CD3DX12_DESCRIPTOR_RANGE _Range0 = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		CD3DX12_DESCRIPTOR_RANGE _Range1 = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 3);
		auto _Parameters = std::vector<D3D12_ROOT_PARAMETER>(size_t(2));
		CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(_Parameters[0], 1, &_Range0);
		CD3DX12_ROOT_PARAMETER::InitAsDescriptorTable(_Parameters[1], 1, &_Range1);

		std::vector<D3D12_STATIC_SAMPLER_DESC> _Samplers;

		_My_root_signature = std::move( dx12::packaged_root_signature(*md3dDevice.Get(), _Parameters, _Samplers, 
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT) );
	}

	void build_shader_inputlayout() {
		_My_shaders["VS"] = hlsl::shader_compile(L"shader/color.hlsl", nullptr, "VS", "vs_5_1");
		_My_shaders["PS"]   = hlsl::shader_compile(L"shader/color.hlsl", nullptr, "PS", "ps_5_1");

		_My_input_layout.push_back("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
		_My_input_layout.push_back("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
	}

	void build_pipeline_state() {
		D3D12_GRAPHICS_PIPELINE_STATE_DESC _Desc;
		ZeroMemory(&_Desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));// Fills a block of memory with zeros

		_Desc.InputLayout = _My_input_layout.get();
		_Desc.pRootSignature = _My_root_signature.get();
		_Desc.VS = _My_shaders["VS"].get_dx12();
		_Desc.PS = _My_shaders["PS"].get_dx12();
		_Desc.RasterizerState   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		_Desc.BlendState        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		_Desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		_Desc.SampleMask = UINT_MAX;
		_Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		_Desc.NumRenderTargets = 1;
		_Desc.RTVFormats[0] = mBackBufferFormat;
		_Desc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
		_Desc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
		_Desc.DSVFormat = mDepthStencilFormat;
		_My_pipeline_states["opaque"] = dx12::packaged_pipeline_state(*md3dDevice.Get(), _Desc);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = _Desc;
		opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		opaqueWireframePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		_My_pipeline_states["opaque_wireframe"] = dx12::packaged_pipeline_state(*md3dDevice.Get(), opaqueWireframePsoDesc);
	}
	
	std::vector<frame_resource> _My_frame_resources;
	frame_resource* _My_frame_resource_current;
	size_t _My_frame_resource_current_index;

	dx12::input_layout   _My_input_layout;
	dx12::packaged_descriptor_heap _My_descriptor_heap;
	dx12::packaged_root_signature _My_root_signature;
	std::unordered_map<std::string, hlsl::shader> _My_shaders;
	std::unordered_map<std::string, dx12::packaged_pipeline_state> _My_pipeline_states;

	std::unordered_map<std::string, dx12::static_mesh<vertex>> _My_geometries;

	std::vector<std::unique_ptr<actor>> _My_render_items_all;

	matrix4x4<float, __m128> _My_view;
	matrix4x4<float, __m128> _My_proj;

	clmagic::degrees theta  = 270;
	clmagic::degrees phi    = 45;
	float            radius = 15.f;
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




int main() {
	using namespace::clmagic;
	/*const auto bound = 2;
	for (size_t i = 0; i != 10000; ++i) {
		std::cout << "i: " << i << "\ttrunc: " << trunc(i, bound) << "\tceil: "<<ceil(i, bound)<<"\tfloor: "<<floor(i, bound) << std::endl;
	}*/


	using std::chrono::milliseconds;
	using std::chrono::steady_clock;
	using std::chrono::duration_cast;
	const auto _Axis = Rotation<float, 4, float>::get_matrix(unit_vector3<float>{ 2.f, 3.f, 1.f }, 2);
	const auto _Axis2 = Rotation<float, 4, float>::get_matrix(unit_vector3<float>{ 2.f, 3.f, 1.f }, 2);
	
	for (size_t i = 1000; i != -1; --i) {
		std::cout << i << std::endl;
	}

	size_t _Test_count = 1000000;

	milliseconds _Start1 = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());
	for (size_t i = 0; i != _Test_count; ++i) {
		auto _CC = Translation<float, float, true>::get_matrix(_Axis, 10.f, 20.f, 30.f);
	}
	milliseconds _End1 = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());

	milliseconds _Start2 = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());
	for (size_t i = 0; i != _Test_count; ++i) {
		auto _CC = Translation<float, float, false>::get_matrix(_Axis2, 10.f, 20.f, 30.f);
	}
	milliseconds _End2 = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());

	std::cout << "col-major: "<<(_End1 - _Start1).count() << std::endl;
	std::cout << "row-major: "<<(_End2 - _Start2).count() << std::endl;

	return 0;
}