#include "geom_shape.h"

namespace clmagic 
{
	std::vector<Vec4> 
		convert_mesh_data(std::vector<triangle_index> _Indices, const std::vector<Vec4>& _Data, size_t _Attrib) 
		{
		auto _Result = std::vector<Vec4>(3 * _Indices.size(), Vec4());

		size_t _Ct = 0;
		for (auto _Ite = _Indices.begin(); _Ite != _Indices.end(); ++_Ite)
			{
			const auto& _Indices = (*_Ite);
			_Result[_Ct++] = _Data[_Indices.at(0, _Attrib)];
			_Result[_Ct++] = _Data[_Indices.at(1, _Attrib)];
			_Result[_Ct++] = _Data[_Indices.at(2, _Attrib)];
			}
		return (_Result);
		}

	std::vector<Vec4>
		convert_mesh_data(std::vector<quad_index> _Indices, const std::vector<Vec4>& _Data, size_t _Attrib) 
		{
		auto _Result = std::vector<Vec4>(4 * _Indices.size(), Vec4());

		size_t _Ct = 0;
		for (auto _Ite = _Indices.begin(); _Ite != _Indices.end(); ++_Ite)
			{
			const auto& _Indices = (*_Ite);
			_Result[_Ct++] = _Data[_Indices.at(0, _Attrib)];
			_Result[_Ct++] = _Data[_Indices.at(1, _Attrib)];
			_Result[_Ct++] = _Data[_Indices.at(2, _Attrib)];
			_Result[_Ct++] = _Data[_Indices.at(3, _Attrib)];
			}
		return ( _Result );
		}

	// < QuadGeometry >
	QuadGeometry::QuadGeometry(Vec3 _P0, Vec3 _P1, Vec3 _P2, Vec3 _P3, _in(matrixTransform) _Transform, Vec2_<size_t> _Subdivide)
		{
		/*
		_P3-----_P2	    _P3------_P2
		 |	 |	 |	    /	 /   /
		 |	 |	 |	   /   /    /
		_P0-----_P1  _P0-----_P1
		_Subdivide = Vec2i(2, 1)
		_P3-----_P2	    _P3------_P2
		 |___|___|	    /___/___ /
		 |	 |	 |	   /   /    /
		_P0-----_P1  _P0-----_P1
		_Subdivide = Vec2i(2, 2)

		_P3-+--+-_P2	_P3-------_P2
		 |__|__|__|	    /__/__/__/
		 |	|  |  |	   /  /  /  /
		_P0-+--+-_P1 _P0------_P1
		_Subdivide = Vec2i(3, 2)
		*/
		Vec2_<size_t> _Side = Vec2_<size_t>(_Subdivide[0] + 1, _Subdivide[1] + 1);
		_Mypos.resize(_Side[0] * _Side[1]);
		_Mynorm.resize(1);
		_Mytang.resize(1);
		_Mybtan.resize(1);
		_Mytexc.resize(_Side[0] * _Side[1]);
		_Myindx.resize(_Subdivide[0] * _Subdivide[1]);

		// positions
		for (size_t i = 0; i <= _Subdivide[1]; ++i)
			{
			float _Ratev = float(1.0 / _Subdivide[1] * i);
			auto _A = lerp(_P0, _P3, _Ratev);
			auto _B = lerp(_P1, _P2, _Ratev);
			for (size_t j = 0; j <= _Subdivide[0]; ++j)
				{
				float _Rateh = float(1.0 / _Subdivide[0] * j);
				_Mypos[i * _Side[0] + j]= Vec4(lerp(_A, _B, _Rateh), 1.f);
				}
			}

		// normal, tangent, btangent
		auto _Tbn = tbn(Vec3(_P0), Vec3(_P1), Vec3(_P3),
						Vec2(0.f, 0.f), Vec2(1.f, 0.f), Vec2(0.f, 1.f));
		_Mynorm[0] = Vec4(_Tbn._Mynormal, 0.f);
		_Mytang[0] = Vec4(_Tbn.tangent, 0.f);
		_Mybtan[0] = Vec4(_Tbn.btangent, 0.f);

		// texcoord
		for (size_t i = 0; i <= _Subdivide[1]; ++i)
			{
			float _Ratev = float(1.0 / _Subdivide[1] * i);
			auto _A = lerp(Vec2(0.f, 0.f), Vec2(0.f, 1.f), _Ratev);
			auto _B = lerp(Vec2(1.f, 0.f), Vec2(1.f, 1.f), _Ratev);
			for (size_t j = 0; j <= _Subdivide[0]; ++j)
				{
				float _Rateh = float(1.0 / _Subdivide[0] * j);
				_Mytexc[i * _Side[0] + j] = Vec4(lerp(_A, _B, _Rateh), 0.f, 1.f);
				}
			}

		// indices
		for (size_t i = 0; i < _Subdivide[1]; ++i) 
			{
			for (size_t j = 0; j < _Subdivide[0]; ++j) 
				{
				/*
					 |
			   (i+1)+j-----(i+1)+j + 1
					 |	    |
					i+j_____i+j + 1
				*/
				unsigned int _Indices[4] = {	   i * _Side[0] + j,	       i * _Side[0] + j + 1,
											 (i + 1) * _Side[0] + j + 1, (i + 1) * _Side[0] + j };
				_Myindx.push_back(quad_index{ quad_index::attribute_index{_Indices[0], 0, _Indices[0]},
											  quad_index::attribute_index{_Indices[1], 0, _Indices[1]},
											  quad_index::attribute_index{_Indices[2], 0, _Indices[2]},
											  quad_index::attribute_index{_Indices[3], 0, _Indices[3]} });
				}
			}

		// apply transform
		for (size_t i = 0; i != _Mypos.size(); ++i) 
			{
			_Mypos[i] = Vec4(_Transform.transform( _Mypos[i] ), 1.f);
			}
		for (size_t i = 0; i != _Mynorm.size(); ++i)
			{
			_Mynorm[i] = Vec4(_Transform.transform(_Mynorm[i], _Tag_normal()), 0.f);
			_Mytang[i] = Vec4(_Transform.transform(_Mytang[i], _Tag_normal()), 0.f);
			_Mybtan[i] = Vec4(_Transform.transform(_Mybtan[i], _Tag_normal()), 0.f);
			}
		}

	std::vector<Vec4> 
	QuadGeometry::all_(const std::vector<Vec4>& _Data, size_t _Attrib) const 
		{
		return ( convert_mesh_data(_Myindx, _Data, _Attrib) );
		}
	// </ QuadGeometry >

	// < CubeGeometry >
	CubeGeometry::CubeGeometry(Vec3 _Minpoint, Vec3 _Maxpoint, bool _Is_close)
		{
		/*
		left hand coordinate
		   v7--<-v6
		  /|     /|
		v4-+->-max|
		| min->-|v1
		| /     |/
		v3--<--v2
		*/

		Vec3 _Edge = _Maxpoint - _Minpoint;
		Vec3 _Seek = _Minpoint;

		// min -> v1
		_Seek.x += _Edge.x;
		Vec3 _V1 = _Seek;

		// v1 -> v2
		_Seek.z += _Edge.z;
		Vec3 _V2 = _Seek;

		// v2 -> v3
		_Seek.x -= _Edge.x;
		Vec3 _V3 = _Seek;

		// v3 -> v4
		_Seek.y += _Edge.y;
		Vec3 _V4 = _Seek;

		// max -> v6
		_Seek = _Maxpoint;
		_Seek.z -= _Edge.z;
		Vec3 _V6 = _Seek;

		// v6 -> v7
		_Seek.x -= _Edge.x;
		Vec3 _V7 = _Seek;

		if ( !_Is_close )
			{
			/*
								 v7------- v6
								 |		   |
								 |	 back  |
								 |		   |
								min--------v1
			left			v7 --------- v6
			 |			   /    up	    / v6
			 |	   v7	 v4 -------- max / |
			  ¨K  / |					/  | <----right
				 /  |	v4--------max max  |
			   v4	|	|		   |   |  v1
				|  min	|	front  |   | /
				| /		| 		   |   |/
				|/		v3--------v2  v2
			   v3		  min -------- v1
						 / 	 down	  /
					   v3 --------- v2
			*/
			_Myfront = QuadGeometry(_V3, _V2, _Maxpoint, _V4);
			_Myback  = QuadGeometry(_V1, _Minpoint, _V7, _V6);
			_Myleft  = QuadGeometry(_Minpoint, _V3, _V4, _V7);
			_Myright = QuadGeometry(_V2, _V1, _V6, _Maxpoint);
			_Myup	 = QuadGeometry(_V4, _Maxpoint, _V6, _V7);
			_Mydown  = QuadGeometry(_Minpoint, _V1, _V2, _V3);
			}
		else 
			{
			/*
			@_tess:
					(-1,1,-1)
				        v7----+-----+-----+--- v6(1,1,-1)
				       /|	/|	  /|    /|	  /|
				     /  | /  |	/  |  /  |  /  |
				   /    /----|/----|/----|/----/
			     /    / |	/|    /|    /|   / |
			   /	/   | /  |  /  |  /  | /   |
			v4----------+----+-----|max--/-----/
			|	  |     |	 ||    ||  / |	 / |
			|     |(-1,-1,-1)||     |/   | /   |
			|-----|----min---+|----+|----/---- v1(1,-1,-1)
			|	  |		|	  |		|  /	 /
			|	  |		|	  |  	|/	   /
			|-----|-----|-----|-----|	 /
			|	  |		|	  |	    |  /
			|	  |		|	  | 	|/
			v3----------------------v2(1,-1,1)

			@_texture:
					v7 ---- v6
					|		|
					|  back	|
			v7 ---- min --- v1 ---- v6 ---- v7
			|  left	| down	| right	|  up	|
			|		|		|		|		|
			v4 ---- v3 ---- v2 ---- max --- v4
					| front	|
					|		|
					v4 --- max
			*/

			// 1. init
			_Mypos.resize(8);
			_Mynorm.resize(8);
			_Mytang.resize(8);
			_Mybtan.resize(8);
			_Mytexc.resize(14);
			_Myindx.resize(6);

			// 2. position
			_Mypos[0] = Vec4(_Minpoint, 1.f);
			_Mypos[1] = Vec4(_V1, 1.f);
			_Mypos[2] = Vec4(_V2, 1.f);
			_Mypos[3] = Vec4(_V3, 1.f);
			_Mypos[4] = Vec4(_V4, 1.f);
			_Mypos[5] = Vec4(_Maxpoint, 1.f);
			_Mypos[6] = Vec4(_V6, 1.f);
			_Mypos[7] = Vec4(_V7, 1.f);

			// 3. tangent space
			_Mynorm[0] = Vec4(-1.f, -1.f, -1.f, 0.f);// min
			_Mynorm[1] = Vec4( 1.f, -1.f, -1.f, 0.f);// v1
			_Mynorm[2] = Vec4( 1.f, -1.f,  1.f, 0.f);// v2
			_Mynorm[3] = Vec4(-1.f, -1.f,  1.f, 0.f);// v3
			_Mynorm[4] = Vec4(-1.f,  1.f,  1.f, 0.f);// v4
			_Mynorm[5] = Vec4( 1.f,  1.f,  1.f, 0.f);// max
			_Mynorm[6] = Vec4( 1.f,  1.f, -1.f, 0.f);// v6
			_Mynorm[7] = Vec4(-1.f,  1.f, -1.f, 0.f);// v7
			for (size_t i = 0; i != _Mynorm.size(); ++i) 
				{
				_Mynorm[i] = normalize(_Mynorm[i]);
				}

			// 4. texcoord
			/*
			1.0(V)
			|
			....
			....
			|		 10 ---- 11
			|		 |		 |
			|		 |  back |
			|0 ----- 1 ----- 2 ----- 3 ----- 4
			||  left | down	 | right |  up	 |
			||		 |		 |		 |		 |
			|9 ----- 8 ----- 7 ----- 6 ----- 5
			|		 | front |
			|		 |		 |
			|		 13 ---- 12
			0--------------------------------1.0(U)   per_unit_side is (1.f / 4.f)
			*/
			float _Per_unit_side = 1.f / 4.f;
			_Mytexc[0] = Vec4(0.f * _Per_unit_side, 2.f * _Per_unit_side, 0.f, 1.f);
			_Mytexc[1] = Vec4(1.f * _Per_unit_side, 2.f * _Per_unit_side, 0.f, 1.f);
			_Mytexc[2] = Vec4(2.f * _Per_unit_side, 2.f * _Per_unit_side, 0.f, 1.f);
			_Mytexc[3] = Vec4(3.f * _Per_unit_side, 2.f * _Per_unit_side, 0.f, 1.f);
			_Mytexc[4] = Vec4(4.f * _Per_unit_side, 2.f * _Per_unit_side, 0.f, 1.f);

			_Mytexc[5] = Vec4(4.f * _Per_unit_side, 1.f * _Per_unit_side, 0.f, 1.f);
			_Mytexc[6] = Vec4(3.f * _Per_unit_side, 1.f * _Per_unit_side, 0.f, 1.f);
			_Mytexc[7] = Vec4(2.f * _Per_unit_side, 1.f * _Per_unit_side, 0.f, 1.f);
			_Mytexc[8] = Vec4(1.f * _Per_unit_side, 1.f * _Per_unit_side, 0.f, 1.f);
			_Mytexc[9] = Vec4(0.f * _Per_unit_side, 1.f * _Per_unit_side, 0.f, 1.f);
			_Mytexc[10] = Vec4(1.f * _Per_unit_side, 3.f * _Per_unit_side, 0.f, 1.f);
			_Mytexc[11] = Vec4(2.f * _Per_unit_side, 3.f * _Per_unit_side, 0.f, 1.f);
			_Mytexc[12] = Vec4(2.f * _Per_unit_side, 0.f * _Per_unit_side, 0.f, 1.f);
			_Mytexc[13] = Vec4(1.f * _Per_unit_side, 0.f * _Per_unit_side, 0.f, 1.f);

			// 5. index
			/*
			@_indices:
			   7------6
			  /|     /|
			4--+----5 |
			|  0----| 1
			| /     |/
			3-------2
					   [7]-->-- 6
					    |	    |
					    |  back |
					    0 --<-- 1
			7 -->--[0] [0]-->-- 1  1 -->-- 6  6 -->-- 7
			|  left |   |  down |  | right |  |  up	  |
			|		|   |	    |  |	   |  |		  |
			4 --<-- 3   3 ----- 2 [2]--<-- 5  5 --<--[4]
					   [3]-->-- 2
					    | front |
					    | 	    |
					    4 --<-- 5
			*/
			std::vector<unsigned int> _Indices = { 3, 2, 5, 4, //front
												   7, 6, 1, 0, //back
												   0, 3, 4, 7, //left 
												   2, 1, 6, 5, //right
												   4, 5, 6, 7, // up
												   0, 1, 2, 3 };// down
			for (size_t i = 0, _Ct = 0; i != _Myindx.size(); ++i)
				{
				_Myindx[i][0] = quad_index::attribute_index{ _Indices[_Ct], _Indices[_Ct], _Indices[_Ct++] };
				_Myindx[i][1] = quad_index::attribute_index{ _Indices[_Ct], _Indices[_Ct], _Indices[_Ct++] };
				_Myindx[i][2] = quad_index::attribute_index{ _Indices[_Ct], _Indices[_Ct], _Indices[_Ct++] };
				_Myindx[i][3] = quad_index::attribute_index{ _Indices[_Ct], _Indices[_Ct], _Indices[_Ct++] };
				}

			// 6. correction texcoord index
			/*
					   [7]-->-- 6
					    |	    |
					    |  back |
					    0 --<-- 1
			7 -->--[0] [0]-->-- 1  1 -->-- 6  6 -->-- 7
			|  left |   |  down |  | right |  |  up	  |
			|		|   |	    |  |	   |  |		  |
			4 --<-- 3   3 ----- 2 [2]--<-- 5  5 --<--[4]
					   [3]-->-- 2
					    | front |
					    | 	    |
					    4 --<-- 5

					 10 ---- 11
					 |		 |
					 |  back |
			 0 ----- 1 ----- 2 ----- 3 ----- 4
			 |  left | down	 | right |  up	 |
			 |		 |		 |		 |		 |
			 9 ----- 8 ----- 7 ----- 6 ----- 5
					 | front |
					 |		 |
					 13 ---- 12
			*/
			_Myindx[0].set( 2, { 8, 7, 12, 13 } );// front
			_Myindx[1].set( 2, { 7, 6,  1,  0 } );// back
			_Myindx[2].set( 2, { 1, 0,  9,  8 } );// left
			_Myindx[3].set( 2, { 7, 2,  3,  6 } );// right
			_Myindx[4].set( 2, { 5, 6,  3,  4 } );// up
			_Myindx[5].set( 2, { 1, 2,  7,  8 } );// down
			}
		}
		
	std::vector<Vec4>
	CubeGeometry::all_(const std::vector<Vec4>& _Data, size_t _Attrib) const
		{
		return ( convert_mesh_data(_Myindx, _Data, _Attrib) );
		}
	// </ CubeGeometry >

	// < SphereGeometry >
	SphereGeometry::SphereGeometry(float _Radius, size_t _Slicecount, size_t _Stackcount, _in(matrixTransform) _Transform)
		{
		using std::sin;
		using std::cos;

		float _Stepphi = cPi / static_cast<float>(_Stackcount);
		float _Steptheta = c2Pi / static_cast<float>(_Slicecount);

		// 1. position normal tangent btangent
		_Mypos.push_back(Vec4(0.f, _Radius, 0.f, 1.f));
		_Mynorm.push_back(Vec4(0.f, 1.f, 0.f, 0.f));
		_Mytang.push_back(Vec4(1.f, 0.f, 0.f, 0.f));
		_Mytexc.push_back(Vec4(0.5f, 0.f, 0.f, 1.f));
		_Mybtan.push_back(cross(_Mynorm.back().V<3>("xyz"), _Mytang.back().V<3>("xyz")));

		for (size_t i = 1; i <= _Stackcount - 1; ++i)
			{
			float _Phi = _Stepphi * static_cast<float>(i);

			for (size_t j = 0; j <= _Slicecount; ++j) 
				{
				float _Theta = _Steptheta * static_cast<float>(j);

				// position
				_Mypos.push_back(Vec4(
					_Radius * sin(_Phi) * cos(_Theta), 
					_Radius * cos(_Phi),
					_Radius * sin(_Phi) * sin(_Theta),
					1.f)
					);

				// normal
				_Mynorm.push_back(normalize(_Mypos.back()));

				// tangent
				_Mytang.push_back(Vec4(
					-_Radius * sin(_Phi)*sin(_Theta),
					0.f,
					_Radius*sin(_Phi)*cos(_Theta),
					0.f)
					);
				_Mytang.back() = normalize(_Mytang.back());

				// btangent
				_Mybtan.push_back(cross(_Mynorm.back().V<3>("xyz"), _Mytang.back().V<3>("xyz")));

				// texcoord
				_Mytexc.push_back(Vec4(_Theta / c2Pi, _Phi / cPi, 0.f, 1.f));
				}
			}

		_Mypos.push_back(Vec4(0.f, -_Radius, 0.f, 1.f));
		_Mynorm.push_back(Vec4(0.f, -1.f, 0.f, 0.f));
		_Mytang.push_back(Vec4(1.f,  0.f, 0.f, 0.f));
		_Mytexc.push_back(Vec4(0.5f,  1.f, 0.f, 1.f));
		_Mybtan.push_back(cross(_Mynorm.back().V<3>("xyz"), _Mytang.back().V<3>("xyz")));

		// 2. indices
		// top index
		/*
		      ---- ----   
		   /  /	/ | \ \  \
		/	/  /  |	 \	\   \
		*/
		for (unsigned int i = 1; i <= _Slicecount; ++i)
			{
			unsigned int _Index[3] = { 0, i, i + 1 };
			_Myindx.push_back(triangle_index{ triangle_index::attribute_index{_Index[0], _Index[0], _Index[0]},
											  triangle_index::attribute_index{_Index[1], _Index[1], _Index[1]},
											  triangle_index::attribute_index{_Index[2], _Index[2], _Index[2]} });
			}

		unsigned int _Baseidx = 1;
		unsigned int _Ringcount = unsigned int(_Slicecount + 1);
		for (unsigned int i = 0; i < _Stackcount - 2; ++i) 
			{
			for (unsigned int j = 0; j < _Slicecount; ++j)
				{
				unsigned int _Index[4] = { _Baseidx + i		  * _Ringcount + j,
										   _Baseidx + (i + 1) * _Ringcount + j,
										   _Baseidx + (i + 1) * _Ringcount + j + 1,
										   _Baseidx + i		  * _Ringcount + j + 1 };

				_Myindx.push_back(triangle_index{ triangle_index::attribute_index{_Index[0], _Index[0], _Index[0]},
												  triangle_index::attribute_index{_Index[1], _Index[1], _Index[1]},
												  triangle_index::attribute_index{_Index[2], _Index[2], _Index[2]} } );
				_Myindx.push_back(triangle_index{ triangle_index::attribute_index{_Index[2], _Index[2], _Index[2]},
												  triangle_index::attribute_index{_Index[3], _Index[3], _Index[3]},
												  triangle_index::attribute_index{_Index[0], _Index[0], _Index[0]} });
				}
			}

		unsigned int _Southpole = static_cast<unsigned int>(_Mypos.size() - 1);
		_Baseidx = _Southpole - _Ringcount;

		for (unsigned int i = 0; i < _Slicecount; ++i)
			{
			unsigned int _Index[3] = { _Southpole, _Baseidx + i, _Baseidx + i + 1 };
			_Myindx.push_back(triangle_index{ triangle_index::attribute_index{_Index[0], _Index[0], _Index[0]},
											  triangle_index::attribute_index{_Index[1], _Index[1], _Index[1]},
											  triangle_index::attribute_index{_Index[2], _Index[2], _Index[2]} });
			}

		// 3. translate
		for (size_t i = 0; i != _Mypos.size(); ++i) 
			{
			_Mypos[i] = Vec4( _Transform.transform( _Mypos[i]), 1.f );
			}
		for (size_t i = 0; i != _Mynorm.size(); ++i) 
			{
			_Mynorm[i] = Vec4(_Transform.transform(_Mynorm[i], _Tag_normal()), 0.f);
			_Mytang[i] = Vec4(_Transform.transform(_Mytang[i], _Tag_normal()), 0.f);
			_Mybtan[i] = Vec4(_Transform.transform(_Mybtan[i], _Tag_normal()), 0.f);
			}
		}
	
	std::vector<Vec4> 
	SphereGeometry::all_(const std::vector<Vec4>& _Data, size_t _Attrib) const 
		{
		return ( convert_mesh_data(_Myindx, _Data, _Attrib) );
		}
	// </ SphereGeometry >

	// < DiskGeometry >
	DiskGeometry::DiskGeometry(size_t _Subdivide, _in(matrixTransform) _InnerT, _in(matrixTransform) _OuterT)
		{
		/*
			
			     --- ---
			  /	    |	  \
		    / \		|     / \ 
		  /		\---|---/	  \
		 /		|		|	   \
		|-------+		+-------|
		|		|		|		|
		 \		/---|---\	   /
		   \  /		|	  \  /
			 \		|	   /
			    ---- ----
		_Subdivide = 8
		_Side = 8
		_Block = 8
		*/
		// 1. init
		_Mypos.resize(_Subdivide * 2);
		_Mynorm.resize(_Subdivide * 2);
		_Mytang.resize(_Subdivide * 2);
		_Mybtan.resize(_Subdivide * 2);
		_Mytexc.resize(_Subdivide * 2);
		_Myindx.resize(_Subdivide);
		float t, dt, x, y;
		dt = float(c2Pi / (_Subdivide));

		// 2. vertices
		for (size_t i = 0; i < _Subdivide; ++i)
			{
			t = float(i) * dt;
			x = cosf(t);
			y = sinf(t);

			// outside
			_Mypos[i]  = Vec4(_OuterT.transform(Vec3(  x,   y, 0.f)), 1.f);
			_Mynorm[i] = Vec4(_OuterT.transform(Vec3(0.f, 0.f, 1.f), _Tag_normal()), 0.f);
			_Mytang[i] = Vec4(_OuterT.transform(Vec3( -y,   x, 0.f), _Tag_normal()), 0.f);
			_Mybtan[i] = Vec4(_OuterT.transform(Vec3(  x,   y, 0.f), _Tag_normal()), 0.f);
			_Mytexc[i] = (_Mypos[i] + Vec4(1.f, 1.f, 0.f, 1.f)) * 0.5f;

			// inside
			_Mypos [i + _Subdivide] = Vec4(_InnerT.transform(Vec3(  x,   y, 0.f)), 1.f);
			_Mynorm[i + _Subdivide] = Vec4(_InnerT.transform(Vec3(0.f, 0.f, 1.f), _Tag_normal()), 0.f);
			_Mytang[i + _Subdivide] = Vec4(_InnerT.transform(Vec3( -y,   x, 0.f), _Tag_normal()), 0.f);
			_Mybtan[i + _Subdivide] = Vec4(_InnerT.transform(Vec3(  x,   y, 0.f), _Tag_normal()), 0.f);
			_Mytexc[i + _Subdivide] = (_Mypos[i + _Subdivide] + Vec4(1.f, 1.f, 0.f, 1.f)) * 0.5f;
			}

		// 3. indices
		for (size_t i = 0; i < _Subdivide; ++i)
			{
			unsigned int _V0, _V1, _V2, _V3;

			unsigned int ip = (i == _Subdivide - 1 ? 0 : i + _Subdivide + 1);
			if (i != _Subdivide - 1) 
				{
				_V0 = i;  _V1 = i + 1;
				_V2 = i + _Subdivide + 1; _V3 = i + _Subdivide;
				}
			else 
				{
				_V0 = i;  _V1 = 0;
				_V2 = _Subdivide; _V3 = i + _Subdivide;
				}

			_Myindx[i] = quad_index{ quad_index::attribute_index{_V0, _V0, _V0},
									 quad_index::attribute_index{_V1, _V1, _V1},
									 quad_index::attribute_index{_V2, _V2, _V2},
									 quad_index::attribute_index{_V3, _V3, _V3} };
			}
		}

	std::vector<Vec4>
	DiskGeometry::all_(const std::vector<Vec4>& _Data, size_t _Attrib) const
		{
		return (convert_mesh_data(_Myindx, _Data, _Attrib));
		}
	// </ DiskGeometry >





	std::array<vertex3d, 6> 
		gen_quad(const Vec3& _V0, const Vec3& _V1, const Vec3& _V2, const Vec3& _V3)
		{
		/*
		v3-----v2	  v3------v2
		|		|	  /	     /
		|		|	 /	    /
		v0-----v1  v0-----v1
		*/
		Vec3 _Normal = normalize(cross(_V1 - _V0, _V3 - _V0));

		vertex3d _Vert0{ _V0, _Normal, Vec2(0.0f, 0.0f) },
				 _Vert1{ _V1, _Normal, Vec2(1.0f, 0.0f) },
				 _Vert2{ _V2, _Normal, Vec2(1.0f, 1.0f) },
				 _Vert3{ _V3, _Normal, Vec2(0.0f, 1.0f) };

		return std::array<vertex3d, 6>{_Vert0, _Vert1, _Vert2, _Vert2, _Vert3, _Vert0};
		}

	/*
	this is error function
	*/
	std::array<vertex3d, 6> 
		gen_rect3d(const Vec3& leftBottom, const Vec3& rightTop)
		{
		bool isHorizal = false;
		Vec3 result = normalize(
			cross(normalize(rightTop - leftBottom),
				Vec3(1.0f, 0.0f, 0.0f))
			);

		if (std::abs(result.x) - 1.0f < 0.1f)
			{
			isHorizal = true;
			}

		Vec3 v0 = leftBottom;
		Vec3 v1 = Vec3(rightTop.x, leftBottom.y, isHorizal ? leftBottom.z : rightTop.z);
		Vec3 v2 = rightTop;
		Vec3 v3 = Vec3(leftBottom.x, rightTop.y, isHorizal ? rightTop.z : leftBottom.z);
		return gen_quad(v0, v1, v2, v3);
		}

	std::array<vertex3d, 36> 
		gen_unit_cube(float _Width, bool _Rhcoordinate)
		{	// get 'width=1.0f' CubeGeometry from ( min(-1.0, -1.0, -1.0), max(1.0, 1.0, 1.0) )
		/*
		   v7----v6
		  /|     /|
		v3-+---max|
		| min---|v5
		| /     |/
		v0-->--v1
		*/
		Vec3 _Min = Vec3(_Width * -0.5f, _Width * -0.5f, _Width * -0.5f);
		Vec3 _Seek = _Min;

		// min -> v5
		_Seek.x += _Width;
		Vec3 _V5 = _Seek;

		// v5 -> v6
		_Seek.y += _Width;
		Vec3 _V6 = _Seek;

		// v6 -> v7
		_Seek.x -= _Width;
		Vec3 _V7 = _Seek;

		// v7 -> v3
		_Seek.z += _Width;
		Vec3 _V3 = _Seek;

		// v3 -> v0
		_Seek.y -= _Width;
		Vec3 _V0 = _Seek;

		// v0 -> v1
		_Seek.x += _Width;
		Vec3 _V1 = _Seek;

		// v1 -> max
		_Seek.y += _Width;
		Vec3 _Max = _Seek;

		auto _Front = gen_quad( _V0, _V1,  _Max, _V3);// front
		auto _Up    = gen_quad( _V3, _Max, _V6,  _V7);// up
		auto _Back  = gen_quad( _V7, _V6,  _V5,  _Min);// back
		auto _Down  = gen_quad(_Min, _V5,  _V1,  _V0); // down
		auto _Right = gen_quad( _V1, _V5,  _V6,  _Max);// right
		auto _Left  = gen_quad(_Min, _V0,  _V3,  _V7);// left

		return std::array<vertex3d, 36>{
			_Front[0], _Front[1], _Front[2], _Front[3], _Front[4], _Front[5],
			   _Up[0],    _Up[1],    _Up[2],    _Up[3],    _Up[4],    _Up[5],
			 _Back[0],  _Back[1],  _Back[2],  _Back[3],  _Back[4],  _Back[5],
			 _Down[0],  _Down[1],  _Down[2],  _Down[3],  _Down[4],  _Down[5],
			_Right[0], _Right[1], _Right[2], _Right[3], _Right[4], _Right[5],
			 _Left[0],  _Left[1],  _Left[2],  _Left[3],  _Left[4],  _Left[5],
			};
		}

	std::array<vertex3d, 36> 
		gen_cube(const Vec3& _Min, const Vec3& _Max)
		{	// right hand coordinate
		/*
		   v7-----v6
		  /|     / |
		v3-+---max |
		| min---|-v5
		| /     | /
		v0------v1
		*/
		std::array<vertex3d, 36> _Cube = gen_unit_cube(1.0f, true);

		Vec3 _Scale = abs(_Max - _Min);
		for (int i = 0; i != _Cube.size(); ++i)
			{
			_Cube[i].first = _Cube[i].first * _Scale;
			}

		float _Minx, _Miny, _Minz;
		_Minx = _Cube[0].first.x;
		_Miny = _Cube[0].first.y;
		_Minz = _Cube[0].first.z;
		for (int i = 0; i != _Cube.size(); ++i)
			{
			const Vec3& _Rvert = _Cube[i].first;
			if (_Rvert.x < _Minx)
				_Minx = _Rvert.x;
			if (_Rvert.y < _Miny)
				_Miny = _Rvert.y;
			if (_Rvert.z < _Minz)
				_Minz = _Rvert.z;
			}
		Vec3 _Translation = _Min - Vec3(_Minx, _Miny, _Minz);
		for (int i = 0; i != _Cube.size(); ++i)
			{
			_Cube[i].first = _Cube[i].first + _Translation;
			}

		return (_Cube);
		}

	std::array<vertex_pos3, 8> 
		gen_quadline(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& v3)
		{
		return std::array<vertex_pos3, 8>{
			v0, v1,
			v1, v2,
			v2, v3,
			v3, v0
			};
		}

	std::array<vertex_pos3, 48> 
		gen_cubeline(const Vec3& min, const Vec3& max)
		{
		/*
		v0 = min    v6 = max
				v7-----v6
			  ¨J¡ý    / ¡ü
			v3¡û¡ý¡ûv2  ¡ü
			|  v4 -¡ú¡ü-v5
			| /      ¡ü/
			v0--¡ú-¡úv1
		*/
		float x = max.x - min.x;
		float y = max.y - min.y;
		float z = max.z - min.z;
		Vec3 start = min;

		Vec3 v0 = min;
		start.x += x;
		Vec3 v1 = start;
		start.y += y;
		Vec3 v2 = start;
		start.x -= x;
		Vec3 v3 = start;
		start.z += z;
		Vec3 v7 = start;
		start.y -= y;
		Vec3 v4 = start;
		start.x += x;
		Vec3 v5 = start;
		Vec3 v6 = max;

		std::array<vertex_pos3, 48> cubeLines;

		std::array< std::array<vertex_pos3, 8>, 6 >  wireFrame = {
			gen_quadline(v0, v1, v2, v3),
			gen_quadline(v3, v2, v6, v7),
			gen_quadline(v7, v6, v5, v4),
			gen_quadline(v4, v5, v1, v0),
			gen_quadline(v0, v4, v7, v3),
			gen_quadline(v1, v5, v6, v2)
			};

		for (size_t i = 0; i != 6; ++i)
			for (size_t j = 0; j != 8; ++j)
				cubeLines[i * 8 + j] = wireFrame[i][j];

		return (cubeLines);
		}

	std::array<vertex2d, 6> 
		gen_quad(const Vec2& _V0, const Vec2& _V1, const Vec2& _V2, const Vec2& _V3)
		{
		/*
		v3-----v2	  v3------v2
		|		|	  /	     /
		|		|	 /	    /
		v0-----v1  v0-----v1
		*/

		vertex2d _Vert0{ _V0, Vec2(0.0f, 1.0f) },
				 _Vert1{ _V1, Vec2(1.0f, 1.0f) },
				 _Vert2{ _V2, Vec2(1.0f, 0.0f) },
				 _Vert3{ _V3, Vec2(0.0f, 0.0f) };

		return std::array<vertex2d, 6>{ _Vert0, _Vert1, _Vert2, _Vert2, _Vert3, _Vert0 };
		}

	std::array<vertex2d, 6> 
		gen_rect(const Vec2& In_Min, const Vec2& In_Max)
		{
		/*
		v3-----max
		|		|
		|		|
		min----v1
		*/
		return gen_quad( In_Min, Vec2(In_Max.x, In_Min.y), In_Max, Vec2(In_Min.x, In_Max.y) );
		}

	std::array<vertex2d, 6> 
		gen_rect(const Vec2& In_Lefttop, float In_Width, float In_Height) 
		{
		return gen_rect(In_Lefttop.x, In_Lefttop.y, In_Width, In_Height);
		}

	std::array<vertex2d, 6> 
		gen_rect(float In_Left, float In_Top, float In_Width, float In_Height)
		{
		/*
		(left,top)
			v3---------v2
			|		    | height
			|		    |
			v0---------v1
				width
		*/
		Vec2 _Vert0 = Vec2(In_Left, In_Top - In_Height),
			_Vert1 = Vec2(In_Left + In_Width, In_Top - In_Height),
			_Vert2 = Vec2(In_Left + In_Width, In_Top),
			_Vert3 = Vec2(In_Left, In_Top);

		return gen_quad(_Vert0, _Vert1, _Vert2, _Vert3);
		}

	std::array<vertex2d, 6> 
		gen_rect(const rect2d& In_Rect) 
		{
		return gen_quad(In_Rect.leftbottom<float>(), In_Rect.rightbottom<float>(), In_Rect.righttop<float>(), In_Rect.lefttop<float>());
		}

	std::array<Vec2, 2>
		genLine2d_OnlyPos(const Vec2& v0, const Vec2& v1)
		{
		return (std::array<Vec2, 2>{v0, v1});
		}

	std::vector<Vec3> 
		gen_lines(const std::vector<Vec3>& _Linestrip) 
		{
		// P0   P1   P2                            Pn
		// |----|----|----|----|----|----|----|----|
		// first                                  last
		Vec3 _Start = _Linestrip.front();
		Vec3 _End;

		std::vector<Vec3> _Result;
		_Result.resize((_Linestrip.size() - 1) * 2);
		size_t _Index = 0;

		for (auto _First = _Linestrip.begin() + 1; _First != _Linestrip.end(); ++_First) 
			{	// each[1, n]
			_End = (*_First);

			_Result[_Index++] = _Start;
			_Result[_Index++] = _End;

			_Start = _End;
			}

		return (_Result);
		}

	std::array<vertex_pos3, 7> 
		gen_unit_dirline() 
		{
			//                    _Back_|_front(0.4)
			//							|
			//							 P2			 ---
			//							 |\			  |		 ___
			// P0------------------------P1 \ P3	 0.3f     | 0.08f
			// P6------------------------P5 /		  |		 ---
			//						     |/ P4		__L__
			//		 --->_Linedir-->
			// _Mypos0                 _Mypos1
			// |<----------1.0f------------->|

		float _Back_and_front[2] = { 0.6f, 0.4f };
		const float _Linewidth = 0.08f;
		const float _Jintouwidth = 0.3f;

		Vec3 _P0 = Vec3(0.0f,				_Linewidth * 0.5f,    0.0f);
		Vec3 _P1 = Vec3(_Back_and_front[0], _Linewidth * 0.5f,    0.0f);
		Vec3 _P2 = Vec3(_Back_and_front[0], _Jintouwidth * 0.5f,  0.0f);
		Vec3 _P3 = Vec3(1.0f,				0.0f,				  0.0f);
		Vec3 _P4 = Vec3(_Back_and_front[0], -_Jintouwidth * 0.5f, 0.0f);
		Vec3 _P5 = Vec3(_Back_and_front[0], -_Linewidth * 0.5f,	  0.0f);
		Vec3 _P6 = Vec3(0.0f,				-_Linewidth * 0.5f,   0.0f);

		_P2.x -= 0.1f;	// offset
		_P4.x -= 0.1f;
		return std::array<vertex_pos3, 7>{ _P0, _P1, _P2, _P3, _P4, _P5, _P6 };
		}

	std::array<vertex_pos3, 7> 
		gen_dirline(const Vec3& _Pos0, const Vec3& _Pos1) 
		{
		using std::acos;

		float _Len = length(_Pos1, _Pos0);
		Vec3 _Linedir = (_Pos1- _Pos0) / _Len;
		float Radians = acos(_Linedir.x);
		if (_Linedir.y < 0.0f)
			Radians = -(Radians);
		
		const float _Linewidth = _Len * 0.4f;
		const float _Minwidth = 128.0f;

		Vec3 _Scale(_Len, _Linewidth, 1.0f);
		if (_Len < _Minwidth)
			{	// short
			_Scale.y = _Len / _Minwidth * _Linewidth;
			}

		Quaternion _Quat(Vec3(0.0f, 0.0f, 1.0f), Radians);
		auto _Vertices = gen_unit_dirline();
		for (auto& _Pervert : _Vertices) 
			{	// transform: 1.scale 2.rotate 
			_Pervert.first = _Pervert.first * _Scale;
			_Pervert.first = _Quat.rotate(_Pervert.first);
			_Pervert.first = _Pervert.first + _Pos0;
			}

		return (_Vertices);
		}

	std::vector<Vec3>
		gen_linestrip_Sline(std::vector<float> In_Radian,
			const Vec3& _P0, float _Radius, float _Fade, float In_Thresould)
		{
		/*
			   ~~\ _Radian0
				   \
			_P0		|
				   /
	_Radian1 + 1 / _Lastpoint | _Radian1
			   /
			  !    _P1
			   \         / _Radian2
				 \ _ _ /
						   i<n
			_Totalradian =  ¦² _Radian(i) - _Radian(i-1)
						   i=1
			_Eachstep[i] = (_Eachradian) / _Totalradian * _Step

			_P[i] = _P[i-1] + normalize(_Endpoint - _P[i-1]) * _Radius * pow(_Fade, _Eachstep[i])
		*/

		using std::cos; using std::sin;

		auto _First = In_Radian.begin();

		std::vector<Vec3> _Result;
		Vec3 _Point = _P0;
		float _Seek = *_First++;
		float _Radian = 0.0f;
		float _Thresould = 0.0f;
		float _Radianabs = 0.0f;

		_Result.push_back(_Point + Vec3(_Radius * cos(_Seek), _Radius * sin(_Seek), 0.0f));

		while (_First != In_Radian.end())
			{
			if (_Result.size() > 1) 
				{
				Vec3 _Dir = normalize(_Result.back() - _Point);
				// next point
				_Point = _Result.back() + _Dir * _Radius;
				// start radian
				_Seek = sin(normalize(_Result.back() - _Point).y);
				}
			_Radian = *_First++;
			_Thresould = _Radian > 0.0f ? In_Thresould : -In_Thresould;
			_Radianabs = std::abs(_Radian);

			// each:[0]
			_Seek += _Thresould;

			// each:(0, radian]
			while ( std::abs(_Seek) <= _Radianabs)
				{
				_Result.push_back(_Point + Vec3(_Radius * cos(_Seek), _Radius * sin(_Seek), 0.0f));
				_Radius *= _Fade;
				_Seek += _Thresould;
				}

			if ( std::abs(_Seek) < _Radianabs && !equals(_Seek, _Radian, 0.0002f) )
				{
				_Result.push_back(_Point + Vec3(_Radius * cos(_Seek), _Radius * sin(_Seek), 0.0f));
				}
			}

		return (_Result);
		}

	std::array<Vec2, 16> 
		gen_unit_border1(float _Rate)
		{
		//
		// V0----- ------------- ------V1
		// | \(-0.5,0.5)_____(0.5,0.5)/|
		// | |                       | |
		// | |                       | |
		// | |                       | |
		// | |                       | |
		// | |                       | |
		// | |                       | |
		// | |                       | |
		// | (-0.5,-0.5)___(0.5,-0.5)| |
		// | /                       \ |
		// V3----- ------------- ------V2
		//
		// pand = _Rate
		// V0 = _Inner.lefttop() + Vec2(-_Rate, _Rate);
		// V1 = _Inner.righttop() + Vec2(_Rate);
		// V2 = _Inner.rightbottom() + Vec2(_Rate, -_Rate);
		// V3 = _Inner.leftbottom() + Vec2(-_Rate);

		rect2d _Inner(Vec2(-0.5f), RECT_COORD::LEFTBOTTOM, Vec2(1.0f));
		Vec2 _V0 = _Inner.lefttop() + Vec2(-_Rate, _Rate);
		Vec2 _V1 = _Inner.righttop() + Vec2(_Rate);
		Vec2 _V2 = _Inner.rightbottom() + Vec2(_Rate, -_Rate);
		Vec2 _V3 = _Inner.leftbottom() + Vec2(-_Rate);

		std::array<Vec2, 16> _Vertices{
			_V0, _V1,
			_V1, _V2,
			_V2, _V3,
			_V3, _V0,
			_V0, _Inner.lefttop(),
			_V1, _Inner.righttop(),
			_V2, _Inner.rightbottom(),
			_V3, _Inner.leftbottom()
			};

		return (_Vertices);
		}

	std::vector<Vec3> 
		gen_linestrip_bezier( const std::vector< Vec3 >& _Points, size_t _Step ) 
		{	// B(n, i, t) =  C(n, i) * pow(t, i) * pow(1-t, n-i), t¡Ê[0, 1]
		assert(_Points.size() >= 2);

		int _N = static_cast<int>(_Points.size() - 1);
		std::vector<int> _C = binomial_coefficient<int>(_N);
		// P0(0.0)--p--p--...-p--Pi------------------------Pi-------------------------Pi---------------P1(1.0)
		//       |  p.size:_Step |
		float _Weight = 1.0f / static_cast<float>(_C.size() - 1);
		_Weight = _Weight / static_cast<float>(_Step + 1);

		std::vector< Vec3 > _Result;
		_Result.resize( _C.size() + _Step * (_C.size() - 1) );
		float _T = 0.0f;
		for ( int _Idx = 0; _Idx != _Result.size(); ++_Idx )
			{
			// Bernstein(n, i) = C(n, i) * pow(t, i) * pow(1-t, n-i); 
			_Result[ _Idx ] = Vec3(0.0f);
			for (decltype(_N) i = 0; i <= _N; ++i)
				{
				_Result[ _Idx ] += ( _Points[i] * Bernstein( _N, i, _T ) );
				}
			_T += _Weight;
			}

		return (_Result);
		}

	std::vector<Vec2> circle(const std::complex<float>& _Center, const std::complex<float>& _P0, size_t _Step)
		{
		std::vector<Vec2> _Result;
		float _Radians = 0.0f;
		if (_Step < 2)
			{
			_Radians = c2Pi / 361.0f;
			_Result.resize(361);
			}
		else
			{
			_Radians = c2Pi / float(_Step + 1);
			_Result.resize(_Step + 1);
			}

		auto _Vec = (_P0 - _Center);
		for (int i = 0; i != _Result.size(); ++i)
			{
			auto _Mul = to_complex(1.0f, float(i) * _Radians);
			auto _Res = _Vec * _Mul + _Center;
			_Result[i] = Vec2(_Res.real(), _Res.imag());
			}

		return (_Result);
		}

}// namespace clmagic 