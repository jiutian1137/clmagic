#pragma once
#include <memory>
#include <map>
#include <string>
#include <d3d12.h>

namespace clmagic {
	// not manage memory
	struct renderable {
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList&) = 0;
	};

	struct mesh : public renderable {
		size_t base_vertex_location = 0;
		size_t start_index_location = 0;
		size_t index_count = 0;
		
		mesh() = default;
		~mesh() { this->release(); }

		virtual void release() {}
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList&) override = 0;
		virtual std::shared_ptr<mesh> reduce(size_t _Base_vertex_location, size_t _Start_index_location, size_t _Index_count) = 0;
	};

	struct transform : public renderable {
		void set_parent(const transform& _Parent) {
			_My_parent = &_Parent;
		}

		transform* join(transform* _Parent) {
			_My_parent = _Parent;
			return _Parent;
		}

		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList& _Cmdlist) override = 0;
		virtual std::shared_ptr<transform> clone() = 0;
		virtual void translate(float x, float y, float z) = 0;
		virtual void rotate(float axis_x, float axis_y, float axis_z, float _Radians) = 0;
		virtual void scale(float scale_x, float scale_y, float scale_z) = 0;

		const transform* _My_parent = nullptr;
	};

	struct geometry : public renderable {
		geometry() = default;
		~geometry() { this->release(); }

		virtual void release() {
			_My_mesh      = nullptr;
			_My_transform = nullptr;
		}
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList& _Cmdlist) override {
			if (_My_transform != nullptr) {
				_My_transform->render(_Cmdlist);
			}
			if (_My_mesh != nullptr) {
				_My_mesh->render(_Cmdlist);
			}
			return _Cmdlist;
		}
		virtual std::shared_ptr<geometry> reduce(size_t _Base_vertex_location, size_t _Start_index_location, size_t _Index_count) {
			// mesh reduce, transform clone
			std::shared_ptr<geometry> _Geometry_ptr = std::make_shared<geometry>();
			if (_My_mesh != nullptr) {
				_Geometry_ptr->_My_mesh      = _My_mesh->reduce(_Base_vertex_location, _Start_index_location, _Index_count);
			}
			if (_My_transform != nullptr) {
				_Geometry_ptr->_My_transform = _My_transform->clone();
			}
			return _Geometry_ptr;
		}

		std::shared_ptr<mesh> _My_mesh;
		std::shared_ptr<transform> _My_transform;
	};

	struct substance : public renderable{
		substance() = default;
		~substance() { this->release(); }

		virtual void release() {}
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList&) override = 0;
	};

	struct object {
		~object() {
			this->release();
		}

		virtual void release() {
			_My_geometry  = nullptr;
			_My_substance = nullptr;
		}
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList& _Cmdlist) {
			if (_My_substance != nullptr) {
				_My_substance->render(_Cmdlist);
			}
			if (_My_geometry != nullptr) {
				_My_geometry->render(_Cmdlist);
			}
			return _Cmdlist;
		}
		virtual std::shared_ptr<object> reduce(size_t _Base_vertex_location, size_t _Start_index_location, size_t _Index_count) {
			// geometry reduce, substance shared
			std::shared_ptr<object> _Object_ptr = std::make_shared<object>();
			if (_My_geometry != nullptr) {
				_Object_ptr->_My_geometry  = _My_geometry->reduce(_Base_vertex_location, _Start_index_location, _Index_count);
			}
			if (_My_substance != nullptr) {
				_Object_ptr->_My_substance = _My_substance;
			}
			return _Object_ptr;
		}

		std::shared_ptr<geometry>  _My_geometry;
		std::shared_ptr<substance> _My_substance;
	};

	struct object_system {
		~object_system() {
			this->release();
		}

		virtual void release() {
			_My_components.clear();
			_My_transform = nullptr;
		}
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList& _Cmdlist) {
			if (_My_transform != nullptr) {
				_My_transform->render(_Cmdlist);
			}

			auto       _First = _My_components.begin();
			const auto _Last  = _My_components.end();
			for ( ; _First != _Last; ++_First) {
				if (_First->second != nullptr) {
					_First->second->render(_Cmdlist);
				}
			}
			return _Cmdlist;
		}
		
		std::map<std::string, std::shared_ptr<object>> _My_components;
		std::shared_ptr<transform>                     _My_transform;
	};

}// namespace clmagic