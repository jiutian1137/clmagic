#pragma once
#include "dxcore.h"
#include "../basic/winapi_util.h"
#include <DirectXColors.h>

namespace d3d12 {
	struct xD3D12_VIEWPORT : public D3D12_VIEWPORT {
		xD3D12_VIEWPORT() = default;
		xD3D12_VIEWPORT(winapi_util::xRECT r) {
			this->TopLeftX = 0;
			this->TopLeftY = 0;
			this->Width    = static_cast<float>(r.width());
			this->Height   = static_cast<float>(r.height());
			this->MinDepth = 0.0f;
			this->MaxDepth = 1.0f;
		}
	};



	class window : public winapi_util::window {
		using _Mybase = winapi_util::window;
	public:
		window() = default;

		virtual void release() override {
			_My_color_view.release();
			_My_depth_view.release();
			_My_color_buffer.release();
			_My_depth_buffer.release();
			_My_executer.release();
			_Ref_fence = nullptr;
			_Ref_device = nullptr;
			_Ref_factory = nullptr;
			_Mybase::release();
		}

		D3D12_RECT      get_D3D12_RECT() const {
			assert(_Mybase::_My_win != nullptr);
			return _Mybase::get_RECT().copy_move_to(0, 0);
		}
		xD3D12_VIEWPORT get_D3D12_VIEWPORT() const {
			assert(_Mybase::_My_win != nullptr); 
			return xD3D12_VIEWPORT(_Mybase::get_RECT());
		}
		D3D12_CPU_DESCRIPTOR_HANDLE get_color_view() const {
			return _My_color_view.get_CPUhandle(_My_color_buffer.timer_pointer);
		}
		D3D12_CPU_DESCRIPTOR_HANDLE get_depth_view() const {
			return _My_depth_view->GetCPUDescriptorHandleForHeapStart();
		}

		void init(factory* _Pfactory, device* _Pdevice, fence* _Pfence,
			HINSTANCE hInstance, LPCWSTR windowName, int x, int y, int width, int height, int dwStyle = WS_OVERLAPPEDWINDOW) 
		{
			_Ref_factory = _Pfactory;
			_Ref_device  = _Pdevice;
			_Ref_fence   = _Pfence;
			_Mybase::init(hInstance, windowName, x, y, width, height, dwStyle);
		}
		void clear_buffer(ID3D12GraphicsCommandList& _Cmdlist, const float* _Cclear, float _Dclear = 1.f, UINT _Sclear = 0) {
			_Cmdlist.ClearRenderTargetView(this->get_color_view(), _Cclear, 
				0, nullptr);
			_Cmdlist.ClearDepthStencilView(this->get_depth_view(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, _Dclear, _Sclear, 
				0, nullptr);
		}
		void swap_buffer() {
			_My_color_buffer.chain().Present(0, 0);
			_My_color_buffer.turn();
		}
		void execute_command_lists(UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists) {
			_My_executer->ExecuteCommandLists(NumCommandLists, ppCommandLists);
		}
		void execute_command_list(ID3D12CommandList& _Onlycommand) {
			ID3D12CommandList* _Command_lists[] = { &_Onlycommand };
			_My_executer->ExecuteCommandLists(1, _Command_lists);
		}

		virtual void on_init() override {
			_My_executer = command_queue(_Ref_device->ref(), D3D12_COMMAND_LIST_TYPE_DIRECT);

			const auto _Rect = _Mybase::get_RECT();
	
			_My_color_buffer = swap_chain_rgba8u(_Mybase::_My_win, _Ref_factory->ref(), _My_executer.ref(),
				_Rect.width(), _Rect.height(), { 1, 0 }, 2);
			
			_My_depth_buffer = static_texture2D_d24s8(_Ref_device->ref(), 
				_Rect.width(), _Rect.height(), 1, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, 
				D3D12_HEAP_FLAG_NONE, &CD3DX12_CLEAR_VALUE(d3d12::static_texture2D_d24s8::format, 1.0f, 0));

			_My_color_view = RTV_array<2>(_Ref_device->ref());
			_My_color_view.set(0, _My_color_buffer.clock(0).ref(), make_D3D12_RENDER_TARGET_VIEW_DESC_with_TEXTURE2D(_My_color_buffer.clock(0).ref()));
			_My_color_view.set(1, _My_color_buffer.clock(1).ref(), make_D3D12_RENDER_TARGET_VIEW_DESC_with_TEXTURE2D(_My_color_buffer.clock(1).ref()));
			
			_My_depth_view = DSV_array<1>(_Ref_device->ref());
			_My_depth_view.set(0, _My_depth_buffer.ref(), make_D3D12_DEPTH_STENCIL_VIEW_DESC_with_TEXTURE2D(_My_depth_buffer.ref()));

		}

		virtual void on_frame() override {// Example
			command_allocator _Allocator = command_allocator(_Ref_device->ref(), D3D12_COMMAND_LIST_TYPE_DIRECT);
			graphics_command_list _Cmdlist = graphics_command_list(_Ref_device->ref(), D3D12_COMMAND_LIST_TYPE_DIRECT, _Allocator.ref());
			
			{
				_Allocator->Reset();
				_Cmdlist->Reset(_Allocator.get(), nullptr);
				_Cmdlist->ResourceBarrier(1, &_My_color_buffer->transition(D3D12_RESOURCE_STATE_RENDER_TARGET));
			
				this->clear_buffer(_Cmdlist.ref(), rand() % 2 == 0 ? DirectX::Colors::LightSteelBlue : DirectX::Colors::Red);
				/*_Cmdlist->OMSetRenderTargets(1, &this->get_color_view(), true, &this->get_depth_view());
				_Cmdlist->RSSetViewports(1, &this->get_D3D12_VIEWPORT());
				_Cmdlist->RSSetScissorRects(1, &this->get_D3D12_RECT());*/
	
				_Cmdlist->ResourceBarrier(1, &_My_color_buffer->transition(D3D12_RESOURCE_STATE_PRESENT));
				_Cmdlist->Close();
			}

			this->execute_command_list(_Cmdlist.ref());
			
			this->swap_buffer();
			
			_Ref_fence->flush(_My_executer.ref());
		}

		virtual void on_close() override {}

		factory* _Ref_factory;
		device*  _Ref_device;
		fence*   _Ref_fence;
		command_queue          _My_executer;
		swap_chain_rgba8u      _My_color_buffer;
		static_texture2D_d24s8 _My_depth_buffer;
		RTV_array<2>           _My_color_view;
		DSV_array<1>           _My_depth_view;
	};
}// namespace d3d12
