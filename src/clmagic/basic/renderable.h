#pragma once
#include <memory>
#include <map>
#include <string>
#include <d3d12.h>
#include "../math.h"

/*
								+- geometry
								|
				+-"N": object -+- substance
				|              |
				|              +- transform
				|
				+-"2": object
				|
object_system -+-"0": object
				|
				+-"1": object
				|
				+-"4": object
				|
				+- transform
uniform
*/

namespace clmagic {
	/*- - - - - - - - - - - - - - - - - - - - - - interface - - - - - - - - - - - - - - - - - - - - -*/
	struct renderable {
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList&) = 0;
	};	

	struct animatable {
		virtual void translate(float x, float y, float z) = 0;
		virtual void scale(float sx, float sy, float sz) = 0;
		virtual void rotate(unit_vector3<float> axis, radians<float> angle) = 0;

		virtual void translate_parent(float x, float y, float z) = 0;
		virtual void scale_parent(float sx, float sy, float sz) = 0;
		virtual void rotate_parent(unit_vector3<float> axis, radians<float> angle) = 0;
	};


	struct geometry : public renderable {
		geometry() = default;
		~geometry() { this->release(); }

		virtual void release() { }
		virtual std::shared_ptr<geometry> reduce(size_t _Base_vertex_location, size_t _Start_index_location, size_t _Index_count) = 0;
	};

	struct substance : public renderable{
		substance() = default;
		~substance() { this->release(); }

		virtual void release() {}
	};

	struct transform : public renderable, public animatable {
		void set_parent(transform& _Parent) {
			_Parent._My_sub.push_back(this);
		}
		transform* join(transform* _Parent) {
			_Parent->_My_sub.push_back(this);
			return _Parent;
		}

		virtual std::shared_ptr<transform> clone() const = 0;

		std::vector<transform*> _My_sub;
	};

	struct object : public renderable {
		~object() {
			this->release();
		}

		virtual void release() {
			_My_geometry  = nullptr;
			_My_substance = nullptr;
			_My_transform = nullptr;
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
			if (_My_transform != nullptr) {
				_Object_ptr->_My_transform = _My_transform->clone();
			}
			return _Object_ptr;
		}
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList& _Cmdlist) override {
			if (_My_transform != nullptr) {
				_My_transform->render(_Cmdlist);
			}
			if (_My_substance != nullptr) {
				_My_substance->render(_Cmdlist);
			}
			if (_My_geometry != nullptr) {
				_My_geometry->render(_Cmdlist);
			}
			return _Cmdlist;
		}

		std::shared_ptr<geometry>  _My_geometry;
		std::shared_ptr<substance> _My_substance;
		std::shared_ptr<transform> _My_transform;
	};

	struct object_system : public renderable {
		~object_system() {
			this->release();
		}

		virtual void release() {
			_My_components.clear();
			_My_transform = nullptr;
		}
		virtual ID3D12GraphicsCommandList& render(ID3D12GraphicsCommandList& _Cmdlist) override {
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
		std::shared_ptr<transform> _My_transform;
	};

	struct uniform : public renderable {
		uniform() = default;
		~uniform() { this->release(); }

		virtual void release() { }
		virtual void map(void** _Pptr) = 0;
		virtual void unmap(void** _Pptr) = 0;
	};

}// namespace clmagic