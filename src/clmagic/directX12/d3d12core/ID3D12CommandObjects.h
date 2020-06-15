#pragma once
#include "ID3D12Device.h"

namespace d3d12 {

	struct _Command_allocator_impl : public packaged_comptr<ID3D12CommandAllocator>, public uncopyable {
		_Command_allocator_impl() = default;
		_Command_allocator_impl(_Command_allocator_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		_Command_allocator_impl& operator=(_Command_allocator_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return *this;
		}

		_Command_allocator_impl(ID3D12Device& _Device, D3D12_COMMAND_LIST_TYPE _Type) {
			assert(SUCCEEDED(
				_Device.CreateCommandAllocator(_Type, IID_PPV_ARGS(&this->_Impl))
			));
		}
	};

	struct _Graphics_command_list_impl : public packaged_comptr<ID3D12GraphicsCommandList>, public uncopyable {
		_Graphics_command_list_impl() = default;
		_Graphics_command_list_impl(_Graphics_command_list_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		_Graphics_command_list_impl& operator=(_Graphics_command_list_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return *this;
		}

		_Graphics_command_list_impl(ID3D12Device& _Device, D3D12_COMMAND_LIST_TYPE _Type, ID3D12CommandAllocator& _Allocator) {
			assert(SUCCEEDED(
				_Device.CreateCommandList(0, _Type, &_Allocator, nullptr, IID_PPV_ARGS(&this->_Impl))
			));
			this->_Impl->Close();
		}
	};

	struct _Command_queue_impl : public packaged_comptr<ID3D12CommandQueue>, public uncopyable {
		_Command_queue_impl() = default;
		_Command_queue_impl(_Command_queue_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		_Command_queue_impl& operator=(_Command_queue_impl&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return *this;
		}

		_Command_queue_impl(ID3D12Device& _Device, const D3D12_COMMAND_QUEUE_DESC& _Desc) {
			assert(SUCCEEDED(
				_Device.CreateCommandQueue(&_Desc, IID_PPV_ARGS(&this->_Impl))
			));
		}
		_Command_queue_impl(ID3D12Device& _Device, D3D12_COMMAND_LIST_TYPE _Type, UINT _Priority = 0, 
			D3D12_COMMAND_QUEUE_FLAGS _Flag = D3D12_COMMAND_QUEUE_FLAG_NONE, UINT _Nodemask = 0) {
			D3D12_COMMAND_QUEUE_DESC _Desc;
			_Desc.Type     = _Type;
			_Desc.Priority = _Priority;
			_Desc.Flags    = _Flag;
			_Desc.NodeMask = _Nodemask;
			assert(SUCCEEDED(
				_Device.CreateCommandQueue(&_Desc, IID_PPV_ARGS(&this->_Impl))
			));
		}
	};

	struct command_objects : public uncopyable {
		command_objects() = default;
		command_objects(command_objects&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		command_objects& operator=(command_objects&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return *this;
		}

		command_objects(ID3D12Device& _Device, D3D12_COMMAND_LIST_TYPE _Type = D3D12_COMMAND_LIST_TYPE_DIRECT, UINT _Priority = 0,
			D3D12_COMMAND_QUEUE_FLAGS _Flag = D3D12_COMMAND_QUEUE_FLAG_NONE) {
			_My_command_alloc = _Command_allocator_impl(_Device, _Type);
			_My_command_list  = _Graphics_command_list_impl(_Device, _Type, _My_command_alloc.ref());
			_My_command_queue = _Command_queue_impl(_Device, _Type, _Priority, _Flag);
		}

		ID3D12GraphicsCommandList* operator->() const {
			return _My_command_list.get();
		}
		void reset() {
			_My_command_alloc->Reset();
			_My_command_list->Reset(_My_command_alloc.get(), nullptr);
		}
		void reset(ID3D12PipelineState& _Pipeline) {
			_My_command_alloc->Reset();
			_My_command_list->Reset(_My_command_alloc.get(), &_Pipeline);
		}
		void close_execute() {
			_My_command_list->Close();
			ID3D12CommandList* _Cmdlists[] = { _My_command_list.get() };
			_My_command_queue->ExecuteCommandLists(1, _Cmdlists);
		}

		void release() {
			this->_My_command_alloc.release();
			this->_My_command_list.release();
			this->_My_command_queue.release();
		}
		void swap(command_objects& _Right) {
			_Right._My_command_alloc.swap(this->_My_command_alloc);
			_Right._My_command_list.swap(this->_My_command_list);
			_Right._My_command_queue.swap(this->_My_command_queue);
		}
		
		_Command_allocator_impl     _My_command_alloc;
		_Graphics_command_list_impl _My_command_list;
		_Command_queue_impl         _My_command_queue;
	};

}// namespace d3d12