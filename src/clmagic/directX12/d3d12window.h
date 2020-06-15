#pragma once
#include "../basic/winapi_util.h"
#include "d3d12core.h"
#include "d3d12renderable.h"
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


	inline void set_render_targets(ID3D12GraphicsCommandList& _Cmdlist, 
		UINT NumRenderTargetDescriptors,
		const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors,
		const D3D12_VIEWPORT* pViewports,
		const D3D12_RECT* pRects,
		BOOL RTsSingleHandleToDescriptorRange,
		const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor) 
	{
		_Cmdlist.OMSetRenderTargets(NumRenderTargetDescriptors, pRenderTargetDescriptors, RTsSingleHandleToDescriptorRange, pDepthStencilDescriptor);
		_Cmdlist.RSSetViewports(NumRenderTargetDescriptors, pViewports);
		_Cmdlist.RSSetScissorRects(NumRenderTargetDescriptors, pRects);
	}

	inline void set_render_target(ID3D12GraphicsCommandList& _Cmdlist, 
		D3D12_CPU_DESCRIPTOR_HANDLE _RenderTargetDescriptor,
		D3D12_VIEWPORT _Viewport, 
		D3D12_RECT _Rect, 
		const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor = nullptr) {
		_Cmdlist.OMSetRenderTargets(1, &_RenderTargetDescriptor, true, pDepthStencilDescriptor);
		_Cmdlist.RSSetViewports(1, &_Viewport);
		_Cmdlist.RSSetScissorRects(1, &_Rect);
	}


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
			_My_device.release();
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
		ID3D12NoRaytracingDevice& get_ID3D12Device() const {
			return _My_device.ref();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE current_color_view() const {
			return _My_color_view.get_CPUhandle(_My_color_buffer.timer_pointer);
		}
		D3D12_CPU_DESCRIPTOR_HANDLE current_depth_view() const {
			return _My_depth_view->GetCPUDescriptorHandleForHeapStart();
		}
		static_texture2D_rgba8u& current_color_buffer() {
			return _My_color_buffer.current();
		}
		static_texture2D_d24s8& current_depth_buffer() {
			return _My_depth_buffer;
		}

		void clear_current_buffer(ID3D12GraphicsCommandList& _Cmdlist, const float* _Cclear, float _Dclear = 1.f, UINT _Sclear = 0) {
			_Cmdlist.ClearRenderTargetView(this->current_color_view(), _Cclear,
				0, nullptr);
			_Cmdlist.ClearDepthStencilView(this->current_depth_view(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, _Dclear, _Sclear,
				0, nullptr);
		}
		void set_current_render_target(ID3D12GraphicsCommandList& _Cmdlist) {
			d3d12::set_render_target(_Cmdlist, this->current_color_view(), this->get_D3D12_VIEWPORT(), this->get_D3D12_RECT(), &this->current_depth_view());
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
		template<typename _Iter>
		static void render(_Iter _First, _Iter _Last, ID3D12GraphicsCommandList& _Cmdlist, 
			ID3D12PipelineState& _Shader_pipeline, 
			ID3D12RootSignature& _Shader_signature, 
			UINT NumDescriptorHeaps, ID3D12DescriptorHeap* const* ppDescriptorHeaps) 
		{
			_Cmdlist.SetPipelineState(&_Shader_pipeline);
			_Cmdlist.SetGraphicsRootSignature(&_Shader_signature);
			_Cmdlist.SetDescriptorHeaps(NumDescriptorHeaps, ppDescriptorHeaps);
			for ( ; _First != _Last; ++_First) {
				(*_First)->render(_Cmdlist);
			}
		}

		virtual void on_init() override {
			_My_device   = device(D3D_FEATURE_LEVEL_11_0);
			_My_executer = command_queue(_My_device, D3D12_COMMAND_LIST_TYPE_DIRECT);

			const auto _Rect    = _Mybase::get_RECT();
	
			_My_color_buffer = swap_chain_rgba8u(_Mybase::_My_win, _My_executer, _Rect.width(), _Rect.height(), 2, { 1, 0 });
			
			_My_depth_buffer = static_texture2D_d24s8(_My_device, _Rect.width(), _Rect.height(), 1, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, 
				D3D12_HEAP_FLAG_NONE, &CD3DX12_CLEAR_VALUE(d3d12::static_texture2D_d24s8::format, 1.0f, 0));

			_My_color_view = RTV_array<2>(_My_device);
			_My_color_view.set( 0, _My_color_buffer[0], make_D3D12_RENDER_TARGET_VIEW_DESC_with_TEXTURE2D(_My_color_buffer[0]) );
			_My_color_view.set( 1, _My_color_buffer[1], make_D3D12_RENDER_TARGET_VIEW_DESC_with_TEXTURE2D(_My_color_buffer[1]) );
			
			_My_depth_view = DSV_array<1>(_My_device);
			_My_depth_view.set( 0, _My_depth_buffer, make_D3D12_DEPTH_STENCIL_VIEW_DESC_with_TEXTURE2D(_My_depth_buffer) );
		}

		virtual void on_frame() override {// Example
			command_allocator     _Allocator = command_allocator(_My_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
			graphics_command_list _Cmdlist   = graphics_command_list(_My_device, D3D12_COMMAND_LIST_TYPE_DIRECT, _Allocator.ref());
			fence                 _Fence     = fence(_My_device);

			{
				_Allocator->Reset();
				_Cmdlist->Reset(_Allocator.get(), nullptr);
				_Cmdlist->ResourceBarrier(1, &this->current_color_buffer().transition(D3D12_RESOURCE_STATE_RENDER_TARGET));
			
				this->clear_current_buffer(_Cmdlist.ref(), rand() % 2 == 0 ? DirectX::Colors::LightSteelBlue : DirectX::Colors::Red);
				/*_Cmdlist->OMSetRenderTargets(1, &this->get_color_view(), true, &this->get_depth_view());
				_Cmdlist->RSSetViewports(1, &this->get_D3D12_VIEWPORT());
				_Cmdlist->RSSetScissorRects(1, &this->get_D3D12_RECT());*/
	
				_Cmdlist->ResourceBarrier(1, &this->current_color_buffer().transition(D3D12_RESOURCE_STATE_PRESENT));
				_Cmdlist->Close();
			}

			this->execute_command_list(_Cmdlist);
			
			this->swap_buffer();
			
			_Fence.flush(_My_executer);
		}

		virtual void on_close() override {}

		device                 _My_device;
		command_queue          _My_executer;
		swap_chain_rgba8u      _My_color_buffer;
		static_texture2D_d24s8 _My_depth_buffer;
		RTV_array<2>           _My_color_view;
		DSV_array<1>           _My_depth_view;
	};
}// namespace d3d12
