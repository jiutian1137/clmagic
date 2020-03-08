#pragma once
#ifndef _CLMAGIC_CORE_GEOMETRY_GEOM_SHAPE_H_
#define _CLMAGIC_CORE_GEOMETRY_GEOM_SHAPE_H_
#include "vertex.h"


namespace clmagic 
{
	template<size_t _Count, size_t _Attrs, typename _Ty = unsigned int>
	struct vertex_index 
	{
		using attribute_index = std::array<_Ty, _Attrs>;
		using const_attribute_index = const std::array<_Ty, _Attrs>;

		_Ty& at(size_t _Vert, size_t _Attrib) { return (_Myindices[_Vert][_Attrib]); }
		const _Ty& at(size_t _Vert, size_t _Attrib) const { return (_Myindices[_Vert][_Attrib]); }

		attribute_index& at(size_t _Vert) { return (_Myindices[_Vert]); }
		const_attribute_index& at(size_t _Vert) const { return (_Myindices[_Vert]); }

		attribute_index& operator[](size_t _Vert) { return (_Myindices[_Vert]); }
		const_attribute_index& operator[](size_t _Vert) const { return (_Myindices[_Vert]); }

		void set(size_t _Attrib, const std::array<_Ty, _Count>& _Val) 
			{
			for (size_t i = 0; i != _Count; ++i) 
				{
				_Myindices[i][_Attrib] = _Val[i];
				}
			}

		size_t size() const { return (_Myindices.size()); }

		std::array<attribute_index, _Count> _Myindices; // ~uint32 _Myindices[_Count][_Attrs];
	};

	using point_index = vertex_index<1, 3>;
	using line_index = vertex_index<2, 3>;
	using triangle_index = vertex_index<3, 3>;
	using quad_index = vertex_index<4, 3>;

	template<size_t _Attrs, typename _Ty>
	struct patch_index 
	{
		using attribute_index = std::array<_Ty, _Attrs>;
		using const_attribute_index = const std::array<_Ty, _Attrs>;

		_Ty& at(size_t _Vert, size_t _Attrib) { return (_Myindices[_Vert][_Attrib]); }
		const _Ty& at(size_t _Vert, size_t _Attrib) const { return (_Myindices[_Vert][_Attrib]); }

		attribute_index& at(size_t _Vert) { return (_Myindices[_Vert]); }
		const_attribute_index& at(size_t _Vert) const { return (_Myindices[_Vert]); }

		attribute_index& operator[](size_t _Vert) { return (_Myindices[_Vert]); }
		const_attribute_index& operator[](size_t _Vert) const { return (_Myindices[_Vert]); }

		size_t size() const { return (_Myindices.size()); }

		std::vector< attribute_index > _Myindices; // ~uint32 _Myindices[ any ][_Attrs];
	};


	std::vector<Vec4> convert_mesh_data(std::vector<triangle_index> _Indices, const std::vector<Vec4>& _Data, size_t _Attrib);
	std::vector<Vec4> convert_mesh_data(std::vector<quad_index> _Indices, const std::vector<Vec4>& _Data, size_t _Attrib);


	class SimpleGeometry 
	{
	public:
		virtual size_t nb_vertices() const = 0;
		virtual std::vector<Vec4> all_(const std::vector<Vec4>& _Data, size_t _Attrib) const = 0;

		std::vector<Vec4> positions() const { return (all_(_Mypos, 0)); }
		std::vector<Vec4> normals()   const { return (all_(_Mynorm, 1)); }
		std::vector<Vec4> tangents()  const { return (all_(_Mytang, 1)); }
		std::vector<Vec4> btangents() const { return (all_(_Mybtan, 1)); }
		std::vector<Vec4> texcoords() const { return (all_(_Mytexc, 2)); }

		void copy_positions(void* _Dst, size_t _Cpysize, size_t _Stride = 0) const
			{
			assert(_Dst != nullptr);
			assert(_Cpysize != 0 && _Cpysize <= sizeof(Vec4));
			_Copy_data_unchecked(_Dst, _Cpysize, _Stride, this->positions());
			}

		void copy_normals(void* _Dst, size_t _Cpysize, size_t _Stride = 0) const
			{
			assert(_Dst != nullptr);
			assert(_Cpysize != 0 && _Cpysize <= sizeof(Vec4));
			_Copy_data_unchecked(_Dst, _Cpysize, _Stride, this->normals());
			}

		void copy_tangents(void* _Dst, size_t _Cpysize, size_t _Stride = 0) const
			{
			assert(_Dst != nullptr);
			assert(_Cpysize != 0 && _Cpysize <= sizeof(Vec4));
			_Copy_data_unchecked(_Dst, _Cpysize, _Stride, this->tangents());
			}

		void copy_btangents(void* _Dst, size_t _Cpysize, size_t _Stride = 0) const
			{
			assert(_Dst != nullptr);
			assert(_Cpysize != 0 && _Cpysize <= sizeof(Vec4));
			_Copy_data_unchecked(_Dst, _Cpysize, _Stride, this->btangents());
			}

		void copy_texcoords(void* _Dst, size_t _Cpysize, size_t _Stride = 0) const
			{
			assert(_Dst != nullptr);
			assert(_Cpysize != 0 && _Cpysize <= sizeof(Vec4));
			_Copy_data_unchecked(_Dst, _Cpysize, _Stride, this->texcoords());
			}

	protected:
		void _Copy_data_unchecked(void* _Dst, size_t _Cpysize, size_t _Stride, const std::vector<Vec4>& _Src) const
			{
			if (_Stride == 0)
				_Stride = _Cpysize;
			insert(_Dst, _Stride, _Src.data(), _Cpysize, sizeof(Vec4), _Src.size());
			}

		std::vector<Vec4> _Mypos;
		std::vector<Vec4> _Mynorm;
		std::vector<Vec4> _Mytang;
		std::vector<Vec4> _Mybtan;
		std::vector<Vec4> _Mytexc;
	};


	class QuadGeometry : public SimpleGeometry
	{
	public:
		QuadGeometry() { /* empty */ }
		QuadGeometry(Vec3 _P0, Vec3 _P1, Vec3 _P2, Vec3 _P3,
			_in(matrixTransform) _Transform = matrixTransform(), Vec2_<size_t> _Subdivide = Vec2_<size_t>(1));

		virtual size_t nb_vertices() const { return (_Myindx.size() * 4); }
		virtual std::vector<Vec4> all_(const std::vector<Vec4>& _Data, size_t _Attrib) const override;

		std::vector<quad_index> _Myindx;
	};


	class CubeGeometry : public SimpleGeometry
	{
	public:
		CubeGeometry(Vec3 _Minpoint, Vec3 _Maxpoint, bool _Is_close = true);
		
		virtual size_t nb_vertices() const { return (_Myindx.size() * 4); }
		virtual std::vector<Vec4> all_(const std::vector<Vec4>& _Data, size_t _Attrib) const override;

		
		std::vector<quad_index> _Myindx;
	protected:
		QuadGeometry _Myfront;
		QuadGeometry _Myback;
		QuadGeometry _Myleft;
		QuadGeometry _Myright;
		QuadGeometry _Myup;
		QuadGeometry _Mydown;
	};


	class SphereGeometry : public SimpleGeometry
	{
	public:
		SphereGeometry(float _Radius, size_t _Countslice, size_t _Countstack, _in(matrixTransform) _Transform = matrixTransform());

		virtual size_t nb_vertices() const { return (_Myindx.size() * 3); }
		virtual std::vector<Vec4> all_(const std::vector<Vec4>& _Data, size_t _Attrib) const override;

		std::vector<triangle_index> _Myindx;
	};


	class DiskGeometry : public SimpleGeometry
	{
	public:
		DiskGeometry(size_t _Subdivide, _in(matrixTransform) _InnerT, _in(matrixTransform) _OuterT = matrixTransform());

		virtual size_t nb_vertices() const { return (_Myindx.size() * 4); }
		virtual std::vector<Vec4> all_(const std::vector<Vec4>& _Data, size_t _Attrib) const override;

		std::vector<quad_index> _Myindx;
	};



	std::array<vertex3d, 6>
		gen_quad(
			const Vec3& _V0, const Vec3& _V1, const Vec3& _V2, const Vec3& _V3
			);

	/*
	this is error
	*/
	std::array<vertex3d, 6>
		gen_rect3d(
			const Vec3& _Lb, const Vec3& _Rt
			);

	std::array<vertex3d, 36>
		gen_unit_cube(
			float _Width = 1.0f, bool _Rhcoordinate = true
			);

	std::array<vertex3d, 36>
		gen_cube(
			const Vec3& _Min, const Vec3& _Max
			);

	std::array<vertex_pos3, 8>
		gen_quadline(
			const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& v3
			);

	std::array<vertex_pos3, 48>
		gen_cubeline(
			const Vec3& min, const Vec3& max
			);

	std::array<vertex2d, 6>
		gen_quad(
			const Vec2& _V0, const Vec2& _V1, const Vec2& _V2, const Vec2& _V3
			);

	std::array<vertex2d, 6> 
		gen_rect( 
			const Vec2& In_Min, const Vec2& In_Max
			);

	std::array<vertex2d, 6> 
		gen_rect(
			const Vec2& In_Lefttop, float In_Width, float In_Height
			);

	std::array<vertex2d, 6> 
		gen_rect(
			float In_Left, float In_Top, float In_Width, float In_Height
			);

	std::array<vertex2d, 6>
		gen_rect(
			const rect2d& In_Rect
			);

	std::array<Vec2, 2> 
		genLine2d_OnlyPos(
			const Vec2& v0, const Vec2& v1
			);

	std::vector<Vec3> 
		gen_lines(
			const std::vector<Vec3>& _Linestrip
			);

	/*
	@_shape:
				                  P2
								  |\
		P0------------------------P1 \ P3
		P6------------------------P5 /
								  |/ P4
	*/
	std::array<vertex_pos3, 7> 
		gen_unit_dirline(
			);

	std::array<vertex_pos3, 7> 
		gen_dirline(
			const Vec3& _Pos0, const Vec3& _Pos1
			);

	/*
	@_shape: S
	*/
	std::vector<Vec3>
		gen_linestrip_Sline(
			std::vector<float> _Radian,
			const Vec3& _P0, float _Radius, float _Fade, float _Thresould
			);

	/*
	@_shape:
		// V0--------------V1
		// | \___________ /|
		// | |			 | |
		// | |			 | |
		// | |	         | |
		// | |___________| |
		// | /           \ |
		// V3--------------V2
	*/
	std::array<Vec2, 16> 
		gen_unit_border1(
			float _Rate
			);

	/*
	@_shape:
		P0            P2
         \      /^\
		   \  _/    \
		P1   ~        P3
	*/
	std::vector<Vec3> 
		gen_linestrip_bezier(
			const std::vector<Vec3>& _Points, size_t _Step
			);

	std::vector<Vec2> circle(const std::complex<float>& _Center, const std::complex<float>& _P0, size_t _Step = 0);

}// namespace clmagic 




#endif