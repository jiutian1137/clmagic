#pragma once
#include "ID3D12Resource.h"

namespace d3d12 {

	/*- - - - - - - - - - - - - - - - - - - - - dynamic_buffer - - - - - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ty>
	struct dynamic_buffer : public packaged_comptr<ID3D12Resource>, public uncopyable {
		static_assert(!std::is_pointer_v<_Ty>, "_Ty must be value type");
		using value_type = _Ty;

		dynamic_buffer() = default;
		dynamic_buffer(dynamic_buffer&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		dynamic_buffer& operator=(dynamic_buffer&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}

		dynamic_buffer(ID3D12Device& _Device, size_t _Count = 1,
			D3D12_RESOURCE_FLAGS _Rflag = D3D12_RESOURCE_FLAG_NONE, D3D12_HEAP_FLAGS _Hflag = D3D12_HEAP_FLAG_NONE) 
		{
			this->_Impl = make_dynamic_buffer_resource<_Ty>(_Device, _Count, _Rflag, _Hflag);
			_My_size    = _Count;
		}

		void release() {
			if (this->valid()) {
				if (this->is_map()) {
					this->unmap();
				}
				this->_Impl = nullptr;
				_My_size    = 0;
			}
		}
		void swap(dynamic_buffer& _Right) {
			std::swap(_Right._Impl, this->_Impl);
			std::swap(_Right._My_size, this->_My_size);
			std::swap(_Right._Mapping, this->_Mapping);
		}

		D3D12_RESOURCE_FLAGS get_D3D12_RESOURCE_FLAGS() const {
			assert(this->valid());
			const auto _Desc = this->get()->GetDesc();
			return _Desc.Flags;
		}
		D3D12_HEAP_FLAGS get_D3D12_HEAP_FLAGS() const {
			assert(this->valid());
			D3D12_HEAP_FLAGS _Hflag;
			this->get()->GetHeapProperties(nullptr, &_Hflag);
			//return _Hflag;
			return D3D12_HEAP_FLAG_NONE;
		}
		D3D12_RESOURCE_STATES get_D3D12_RESOURCE_STATES() const {
			assert(this->valid());
			return D3D12_RESOURCE_STATE_GENERIC_READ;
		}

		size_t size() const {
			return _My_size;
		}
		size_t capacity() const {
			const auto _Desc = this->get()->GetDesc();
			return _Desc.Width * _Desc.Height * _Desc.DepthOrArraySize;
		}
		bool empty() const {
			return (_My_size == 0);
		}
		void map() {
			assert(this->valid());
			this->get()->Map(0, nullptr, reinterpret_cast<void**>(&_Mapping));
		}
		void unmap() {
			assert(this->valid());
			this->get()->Unmap(0, nullptr);
			_Mapping = nullptr;
		}
		bool is_map() const {
			return _Mapping != nullptr;
		}

		_Ty* begin() {
			assert(this->is_map());
			return _Mapping;
		}
		_Ty* end() {
			assert(this->is_map());
			return _Mapping + this->size();
		}
		_Ty& operator[](size_t _Pos) {
			assert(this->is_map());
			return _Mapping[_Pos];
		}
		_Ty& at(size_t _Pos) {
			assert(this->is_map());
			return _Mapping[_Pos];
		}

		template<typename _Valty>
		void emplace_back(_Valty&& _Val) {
			assert(this->is_map());
			if (_Has_capacity()) {
				this->_Emplace_back_uncheck(std::forward<_Valty>(_Val));
			} else {
				this->reserve(static_cast<size_t>(this->capacity() * 1.3));
				assert(_Has_capacity());
				this->_Emplace_back_uncheck(std::forward<_Valty>(_Val));
			}
		}
		void push_back(_Ty&& _Val) {
			assert(this->is_map());
			this->emplace_back(std::move(_Val));
		}
		void push_back(const _Ty& _Val) {
			assert(this->is_map());
			this->emplace_back(_Val);
		}
		void pop_back() {
			assert(_My_size != 0);
			--_My_size;
		}

		void reserve(size_t _New_capacity) {// _My_size invarient, _My_capacity varient
			assert(this->is_map());
			const size_t _My_capacity      = this->capacity();
			const auto   _My_resource_flag = this->get_D3D12_RESOURCE_FLAGS();
			const auto   _My_heap_flag     = this->get_D3D12_HEAP_FLAGS();
			if (_New_capacity < _My_capacity) {
				_New_capacity = _My_capacity + 1;
			}

			// create new_resource
			Microsoft::WRL::ComPtr<ID3D12Device>   _My_device    = get_ID3D12NoRaytracingDevice(this->get()); 
			Microsoft::WRL::ComPtr<ID3D12Resource> _New_resource = make_dynamic_buffer_resource<_Ty>(*_My_device.Get(), _New_capacity, _My_resource_flag, _My_heap_flag);
			// copy data
			_Ty* _New_mapping = nullptr;
			_New_resource->Map(0, nullptr, reinterpret_cast<void**>(&_New_mapping));
			std::copy(this->begin(), this->end(), _New_mapping);

			this->unmap();
			this->_Impl    = _New_resource;// _Old_resource auto-release
			this->_Mapping = _New_mapping;
		}

	private:
		bool _Has_capacity() {
			return this->size() < this->capacity();
		}

		template<typename _Valty>
		void _Emplace_back_uncheck(_Valty&& _Val) {
			(*this)[_My_size++] = std::forward<_Valty>(_Val);
		}
		
		size_t _My_size;
		_Ty*   _Mapping = nullptr;
	};

	template<typename _Ty>
	dynamic_buffer<_Ty> make_dynamic_buffer(ID3D12Device& _Device, const std::vector<_Ty>& _Source) {
		dynamic_buffer<_Ty> _Result = dynamic_buffer<_Ty>(_Device, _Source.size());
		_Ty*                _Ptr    = nullptr;
		_Result->Map(0, nullptr, reinterpret_cast<void**>(&_Ptr));
		std::copy(_Source.begin(), _Source.end(), _Ptr);
		_Result->Unmap(0, nullptr);
		return _Result;
	}

	/*- - - - - - - - - - - - - - - - - - - - - static_buffer - - - - - - - - - - - - - - - - - - - - - - - - -*/
	template<typename _Ty>
	struct static_buffer : public packaged_comptr<ID3D12Resource>, public uncopyable {
		static_assert(!std::is_pointer_v<_Ty>, "_Ty must be value type");
		using value_type = _Ty;

		static_buffer() = default;
		static_buffer(static_buffer&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
		}
		static_buffer& operator=(static_buffer&& _Right) noexcept {
			_Right.swap(*this);
			_Right.release();
			return (*this);
		}

		static_buffer(ID3D12Device& _Device, size_t _Count,
			D3D12_RESOURCE_FLAGS _Rflag = D3D12_RESOURCE_FLAG_NONE, D3D12_HEAP_FLAGS _Hflag = D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATES _Rstate = D3D12_RESOURCE_STATE_COMMON) 
		{
			this->_Impl = make_static_buffer_resource<_Ty>(_Device, _Count, _Rflag, _Hflag, _Rstate);
			_My_state   = _Rstate;
		}

		static_buffer(const dynamic_buffer<_Ty>& _Right) {
			this->copy_from(_Right);
		}
		
		static_buffer(dynamic_buffer<_Ty>&& _Right) {
			this->move_from(std::move(_Right));
		}

		void swap(static_buffer<_Ty>& _Right) {
			std::swap(_Right._Impl, this->_Impl);
			std::swap(_Right._My_state, this->_My_state);
		}

		D3D12_RESOURCE_FLAGS get_D3D12_RESOURCE_FLAGS() const {
			const auto _Desc = this->get()->GetDesc();
			return _Desc.Flags;
		}
		D3D12_HEAP_FLAGS get_D3D12_HEAP_FLAGS() const {
			D3D12_HEAP_FLAGS _Hflag;
			this->get()->GetHeapProperties(nullptr, &_Hflag);
			return _Hflag;
		}
		D3D12_RESOURCE_STATES get_D3D12_RESOURCE_STATES() const {
			return _My_state;
		}
		D3D12_RESOURCE_BARRIER transition(D3D12_RESOURCE_STATES _New_state) {
			auto _Transition = CD3DX12_RESOURCE_BARRIER::Transition(this->get(), _My_state, _New_state);
			_My_state = _New_state;
			return _Transition;
		}

	private:
		template<typename _Bufty>
		void _Copy_from(const _Bufty& _Right) {
			assert( _Right.valid() );
			// create resource
			Microsoft::WRL::ComPtr<ID3D12Device>   _Device   = get_ID3D12NoRaytracingDevice(_Right.get());
			Microsoft::WRL::ComPtr<ID3D12Resource> _Resource = make_static_buffer_resource<_Ty>(*_Device.Get(), _Right.size(), _Right.get_D3D12_RESOURCE_FLAGS(), _Right.get_D3D12_HEAP_FLAGS());
			// create copy_executor
			command_objects _Executor = command_objects(*_Device.Get());
			fence           _Fence    = fence(*_Device.Get(), 1);
			// execute copy
			_Executor.reset();
			_Executor->CopyBufferRegion(_Resource.Get(), 0, _Right.get(), 0, _Right.size()*sizeof(_Ty));
			_Executor.close_execute_flush(_Fence);
			// sync
			this->_Impl = _Resource;
			_My_state   = D3D12_RESOURCE_STATE_COMMON;
		}

		template<typename _Bufty>
		void _Merge_from(const _Bufty& _Right) {
			assert( _Right.valid() );
			if (!this->valid()) {
				this->_Copy_from(_Right);
			} else {
				const size_t _New_size = this->size() + _Right.size();
				// create resource
				Microsoft::WRL::ComPtr<ID3D12Device>   _Device   = get_ID3D12NoRaytracingDevice(_Right.get());
				Microsoft::WRL::ComPtr<ID3D12Resource> _Resource = make_static_buffer_resource<_Ty>(*_Device.Get(), _New_size, this->get_D3D12_RESOURCE_FLAGS(), this->get_D3D12_HEAP_FLAGS(), this->get_D3D12_RESOURCE_STATES());
				// create copy_executor
				command_objects _Executor = command_objects(*_Device.Get());
				fence           _Fence    = fence(*_Device.Get(), 0);
				// execute copy
				_Executor.reset();
				_Executor->CopyBufferRegion(_Resource.Get(),                        0,  this->get(), 0, this->size()*sizeof(_Ty));
				_Executor->CopyBufferRegion(_Resource.Get(), this->size()*sizeof(_Ty), _Right.get(), 0, _Right.size()*sizeof(_Ty));
				_Executor.close_execute_flush(_Fence);
				// sync
				this->_Impl = _Resource;
			}
		}
		
	public:
		void copy_from(const dynamic_buffer<_Ty>& _Right) {
			assert( _Right.valid() );
			this->_Impl = make_static_buffer_resource<_Ty>(_Right, 0, _Right.size());
			_My_state   = D3D12_RESOURCE_STATE_COMMON;
		}
		void copy_from(const static_buffer<_Ty>& _Right) {
			assert( _Right.valid() );
			this->_Impl = make_static_buffer_resource<_Ty>(_Right, 0, _Right.size());
			_My_state   = D3D12_RESOURCE_STATE_COMMON;
		}
		void merge_from(const dynamic_buffer<_Ty>& _Right) {
			this->_Merge_from(_Right);
		}
		void merge_from(const static_buffer<_Ty>& _Right) {
			this->_Merge_from(_Right);
		}

		void move_from(dynamic_buffer<_Ty>&& _Right) {
			assert( _Right.valid() );
			this->copy_from(_Right);
			_Right.release();
		}
		void move_from(static_buffer<_Ty>&& _Right) {
			_Right.swap(*this);
			_Right.release();
		}

		size_t size() const {
			const auto _Desc = this->get()->GetDesc();
			return static_cast<size_t>(_Desc.Width * _Desc.Height * _Desc.DepthOrArraySize);
		}	
	
	private:
		D3D12_RESOURCE_STATES _My_state;
	};

}// namespace d3d12