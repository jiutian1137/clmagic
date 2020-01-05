#include "../src/clmagic/math/simd.h"
#include "../src/clmagic/math.h"
#include "../src/clmagic/basic.h"
#include <random>
#include <iomanip>
#include <iostream>
#include <opencv.hpp>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/fwd.hpp"





#include "Common/d3dApp.h"
#include <DirectXColors.h>

class InitDirect3DApp : public D3DApp {
	using _Mybase = D3DApp;
public:
	InitDirect3DApp(HINSTANCE hInstance) : D3DApp(hInstance) { }
	~InitDirect3DApp() { }

	virtual bool Initialize() override {
		return (_Mybase::Initialize());
	}

private:
	virtual void OnResize() override {
		_Mybase::OnResize();
	}

	virtual void Update(const GameTimer& _Timer) override {
	
	}

	virtual void Draw(const GameTimer& _Timer) override {
		// Reuse the memory associated with command recording.
   // We can only reset when the associated command lists have finished execution on the GPU.
		ThrowIfFailed(mDirectCmdListAlloc->Reset());

		// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
		// Reusing the command list reuses memory.
		ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

		// Indicate a state transition on the resource usage.
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		// Clear the back buffer and depth buffer.
		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		// Specify the buffers we are going to render to.
		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		// Indicate a state transition on the resource usage.
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		// Done recording commands.
		ThrowIfFailed(mCommandList->Close());

		// Add the command list to the queue for execution.
		ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// swap the back and front buffers
		ThrowIfFailed(mSwapChain->Present(0, 0));
		mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

		// Wait until frame commands are complete.  This waiting is inefficient and is
		// done for simplicity.  Later we will show how to organize our rendering code
		// so we do not have to wait per frame.
		FlushCommandQueue();
	}
};

void test_d3d12(HINSTANCE hInstance) {
#if defined(_DEBUG)
	
#endif
	
	try
	{
		InitDirect3DApp _App(hInstance);
		if (_App.Initialize()) {
			_App.Run();
		}
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "HR Failed", MB_OK);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
	test_d3d12(hInstance);

	std::cin.get();
	return (0);
}




//void test_foundation() {
//	using namespace::clmagic;
//
//#define OUTPUT( X ) std::cout << X << std::endl
//
//	constexpr Vec4 _T_constexpr{ real_t(4000), real_t(2122), real_t(22222222222222), real_t(1111111) };
//	OUTPUT( _T_constexpr );
//
//	Vec_<9, real_t> _T_constexpr_2 = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f };
//	OUTPUT( _T_constexpr_2 );
//
//	Vec3 _T_downward_convert_1 = _T_constexpr_2;
//	OUTPUT(_T_downward_convert_1);
//
//	Vec2 _T_downward_convert_2 = _T_constexpr;
//	OUTPUT(_T_downward_convert_2);
//
//	Vec2 _T_upward_convert_1 = { 1.f, 2.f };
//	Vec3 _T_upward_convert_2 = Vec3(_T_upward_convert_1, 3.f);
//	Vec4 _T_upward_convert_3 = Vec4(_T_upward_convert_2, 4.f);
//	Vec_<4,  real_t> _T_VecN_convert_1 = Vec_<4, real_t>(_T_upward_convert_3);
//	Vec_<13, real_t> _T_VecN_convert_2 = _T_VecN_convert_1;
//	Vec_<13, real_t> _T_VecN_convert_3 = Vec_<13, real_t>(_T_VecN_convert_2.begin(), _T_VecN_convert_2.end());
//	OUTPUT(_T_upward_convert_1);
//	OUTPUT(_T_upward_convert_2);
//	OUTPUT(_T_upward_convert_3);
//	OUTPUT(_T_VecN_convert_1);
//	OUTPUT(_T_VecN_convert_2);
//	OUTPUT(_T_VecN_convert_3);
//	OUTPUT(_T_VecN_convert_3.V<6>("xyzwxx"));
//	OUTPUT( pow(_T_VecN_convert_2, 2) );
//	OUTPUT( pow(_T_VecN_convert_3, 2) );
//}
//
//void test_general() {
//	using namespace::clmagic;
//	float A = 7.f / 5.f;
//	float B = 14.f / 25.f;
//	float t0, t1;
//	quadratic(A, B, t0, t1);
//	sort(t0, t1);
//	std::cout << t0 << " " << t1 << std::endl;
//
//	quadratic(A, B, 0.f, t0, t1);
//	sort(t0, t1);
//	std::cout << t0 << " " << t1 << std::endl;
//}
//
//
#pragma comment(lib, "opencv_world411d.lib")
//class CpuShaderProgram {
//	std::vector<char> memory_pool;
//	char* memory_top;
//
//};
//
//clmagic::Vec4 fragment_stage(_in(clmagic::Vec2) _Texcoord, clmagic::real_t t);
//void test_raytracing() {
//	using namespace::clmagic;
//	using namespace::std::chrono;
//	microseconds _Time_start = duration_cast<microseconds>(system_clock::now().time_since_epoch());
//	decltype(_Time_start) _Time_current;
//
//	std::string _Window_name = "spherical";
//	int _Window_width = 200, _Window_height = 200;
//	cv::namedWindow(_Window_name);
//	while (true) {
//		_Time_current = duration_cast<microseconds>(system_clock::now().time_since_epoch());
//		real_t t = 0.001f * static_cast<real_t>((_Time_current - _Time_start).count());
//
//		cv::Mat _Src(_Window_height, _Window_width, CV_32FC4);
//		for (int i = 0; i != _Src.rows; ++i) {
//			for (int j = 0; j != _Src.cols; ++j) {
//				auto _Texcoord = Vec2(float(j), float(i));
//				     _Texcoord = _Texcoord * 2.f - Vec2(float(_Window_width), float(_Window_height));
//				auto _Color = fragment_stage( _Texcoord, t * 0.01f );
//				_Src.at<cv::Vec4f>(i, j) = cv::Vec4f(_Color.b, _Color.g, _Color.r, _Color.a);
//			}
//		}
//		cv::imshow(_Window_name, _Src);
//		cv::waitKey(2);
//	}
//
//}
//
//
//
//
//
//clmagic::Vec4 fragment_stage( _in(clmagic::Vec2) _Texcoord, clmagic::real_t t) {
//	using namespace::clmagic;
//
//	Ray_<real_t> _Myeye;
//	_Myeye.o = Vec3_<real_t>(_Texcoord, -10.f);
//	_Myeye.d = Vec3_<real_t>(0.f, 0.f, 1.f);
//
//	Sphere_<real_t> _Test_obj1;
//	_Test_obj1.r = 66.f;
//	_Test_obj1.world = Mat4(Quaternion(cY, t));
//
//	Hit_<real_t> _Hit;
//
//	if ( intersect(_Myeye, _Test_obj1, _Hit) ) {
//		return ( Vec4(_Hit._Myuv, 0.f, 1.f) );
//	}
//
//	return ( Vec4(0.f) );
//}



void test_simd() {

	//#define Ouput_F32vec4(Vec) std::cout << (Vec)[0] << "," << (Vec)[1] << "," << (Vec)[2] << "," << (Vec)[3] << std::endl
	//	F32vec4 _A(1.f, 2.f, 3.f, 4.f);
	//	F32vec4 _B(5.f);
	//	auto _C = _A * _B;
	//	Ouput_F32vec4(_C);
	//	std::cout << simd_dot(_A, _B) << std::endl;
	//	F32mat4 _Mat(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f);
	//	Ouput_F32vec4(_Mat * _A);
	//	char* _Memory = new char[1024 * 16];
	//	
	//	F32mat4* _Mat_arr = cv::alignPtr((F32mat4*)_Memory);
	//	std::cout << _Mat_arr << std::endl;
	//
	//	F64vec4 _Test_shuffle{ 10.f, 11.f, 12.f, 13.f };
	//	Ouput_F32vec4(_Test_shuffle);
	//
	//#undef Ouput_F32vec4

	using namespace::clmagic;

	SIMDVec_<4, float> _A = SIMDVec_<4, float>( 5.f, 0.f, 0.f, 0.f );
	SIMDVec_<4, float> _B = SIMDVec_<4, float>(5.f, 5.f, 5.f, 1.f);
	SIMDVec_<4, float> _X = { 1.f, 0.f, 0.f, 1.f };
	SIMDVec_<4, float> _Y = { 0.f, 1.f, 0.f, 0.f };
	SIMDVec_<4, float> _Z = { 0.f, 0.f, 1.f, 0.f };

	auto _Test_ = _mm_set_ps(5.f, 7.f, 10.f, 15.f);
	SIMDVec_<12, float> _Test_2 = SIMDVec_<12, float>(_Test_);
	std::cout << _Test_2 << std::endl;
	std::cout << _Test_2.posfix<9>() << std::endl;
	std::cout << _Test_2.prefix<12>() << std::endl;

	Mat_<4, 4, float> _Mat;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			_Mat[i][j] = float(i) * 4.f + float(j);
		}
	}
	std::cout << _Mat << std::endl;
	std::cout << translation_matrix(10.f, 20.f, 0.f) * _X << std::endl;
	std::cout << _X * translation_matrix(10.f, 20.f, 0.f)<< std::endl;

	Mat_<5, 4> _Test_opeartor_plus;
	std::cout << (_Test_opeartor_plus += Mat_<5, 6>()) << std::endl;
}

int main() {
	test_simd();

	std::cin.get();
	return (0);
}