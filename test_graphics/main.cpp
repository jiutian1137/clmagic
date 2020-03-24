#include "../src/clmagic/math.h"
#include <iostream>
#include <chrono>
#include "Common/d3dApp.h"
#include "Common/UploadBuffer.h"

using matrix4x4f = clmagic::matrix4x4<float, true, __m128>;
using vector3f   = clmagic::vector3<float, __m128>;// aligned 16
using vector2f   = clmagic::vector2<float, __m128>;
using unit_vector3f = clmagic::unit_vector3<float, __m128>;// aligned 16


struct object_constants {
	matrix4x4f world_matrix;
};

struct pass_constants {
	matrix4x4f view_matrix = matrix4x4f(1.f);// 64
	matrix4x4f proj_matrix = matrix4x4f(1.f);// 128
	vector3f eye_position  = vector3f();// 144
	vector2f resolution    = vector2f();// 160
	float total_time = 0.f;// 164
	float delta_time = 0.f;// 168
	// 176
};

#include <tuple>

namespace dx12 {

template<typename _Ty>
struct buffer {
	static size_t get_cbuffer_size() {// DirectX-constant-buffer-size must multiple of 256
		const size_t _Mask = (256 - 1);
		return (sizeof(_Ty) + _Mask) & (~_Mask);
	}

	buffer(ID3D12Device* _Device, size_t _Elmsize) {
		const auto _Heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const auto _Resource_desc   = CD3DX12_RESOURCE_DESC(get_cbuffer_size() * _Elmsize);

		_Device->CreateCommittedResource(&_Heap_properties, D3D12_HEAP_FLAG_NONE, &_Resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			__uuidof(*gpu_buffer), &gpu_buffer.get());
		gpu_buffer->Map(0, nullptr, reinterpret_cast<void**>(cpu_buffer));
	}

	~buffer() {
		if (gpu_buffer != nullptr) {
			gpu_buffer->Unmap(0, nullptr);
		}
		cpu_buffer = nullptr;
	}


	std::unique_ptr<ID3D12Resource> gpu_buffer;
	unsigned char* cpu_buffer;
};

template<typename ..._Args>
struct frame_resource : public std::tuple<_Args...> {
	frame_resource(ID3D12Device* _Device, size_t _Npass, size_t _Nobj) {
		_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(*command_allocator), &command_allocator.get());
	}

	std::unique_ptr<ID3D12CommandAllocator> command_allocator;
	size_t fence;
};
}

struct vertex {
	clmagic::vector3<float> position0;
	clmagic::vector4<float> color0;
};

class ShapeApp : public D3DApp {
	using _Mybase = D3DApp;

public:


private:
	clmagic::degrees theta  = 270;
	clmagic::degrees phi    = 45;
	float            radius = 5.f;
	POINT _My_LastMousePos;
};
#include <stack>

int main() {
	using namespace::clmagic;


	return 0;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	constexpr auto SSS = sizeof(pass_constants);

	try {
		ShapeApp _App(hInstance);
		_App.Initialize();
		_App.Run();
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}