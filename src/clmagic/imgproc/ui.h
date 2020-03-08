#pragma once
#ifndef __CLMAGIC_IMPROC___UI___H__
#define __CLMAGIC_IMPROC___UI___H__
#include "math.h"
#include "imgproc.h"

namespace clmagic {

	class ui_material_basic 
	{
	public:
		virtual Vec4 color(_in(Vec2) _Uv = Vec2(real_t(0))) {
			return (Vec4(0.f));
		}
	};

	class ui_material_bgcolor : public ui_material_basic 
	{
	public:
		ui_material_bgcolor(_in(Vec4) _Color = Vec4(1.f)) : _Mycolor(_Color) {
		}

		virtual Vec4 color(_in(Vec2) _Uv = Vec2(real_t(0))) {
			return (_Mycolor);
		}

		Vec4 _Mycolor;
	};

	class ui_material_gradient : public ui_material_basic 
	{
	public:
		ui_material_gradient(_in(Vec4) _Start = Vec4(0.f), _in(Vec4) _End = Vec4(1.f))
			: _Mycolor0(_Start), _Mycolor1(_End) {
		}

		virtual Vec4 color(_in(Vec2) _Uv = Vec2(real_t(0))) {
			return ( mix( mix(_Mycolor0, _Mycolor1, _Uv.x), 
					      mix(_Mycolor0, _Mycolor1, _Uv.x), 
					      _Uv.y ) );
		}

		Vec4 _Mycolor0;
		Vec4 _Mycolor1;
	};

	class ui_material_image : public ui_material_basic 
	{
	public:
		ui_material_image(const std::string& _Filename)
			: _Mytmp(cv::imread(_Filename, cv::IMREAD_UNCHANGED)), _Myfilename(_Filename){
		}

		virtual Vec4 color(_in(Vec2) _Uv = Vec2(real_t(0))) {
			if (_Mytmp.empty()) {
				reload();
			}
			cv::Vec4b _Tmp;
			if (_Mytmp.channels() == 3) {
				cv::Vec3b _Tmp2 = _Mytmp.at<cv::Vec3b>(
					int(std::fmod(_Uv.y, 1.0f) * real_t(_Mytmp.rows)),
					int(std::fmod(_Uv.x, 1.0f) * real_t(_Mytmp.cols)));
				_Tmp = cv::Vec4b(_Tmp2[0], _Tmp2[1], _Tmp2[2], 255);
			} else {
				_Tmp = _Mytmp.at<cv::Vec4b>(
					int(std::fmod(_Uv.y, 1.0f) * real_t(_Mytmp.rows)),
					int(std::fmod(_Uv.x, 1.0f) * real_t(_Mytmp.cols)));
			}
			return ( Vec4(real_t(_Tmp[0]) * cvtfLinearcolor,
				real_t(_Tmp[1]) * cvtfLinearcolor,
				real_t(_Tmp[2]) * cvtfLinearcolor,
				real_t(_Tmp[3]) * cvtfLinearcolor) );
		}

		void reload() {
			_Mytmp = cv::imread(_Myfilename, cv::IMREAD_UNCHANGED);
		}

		void release() {
			_Mytmp.release();
		}

		cv::mat _Mytmp;
		std::string _Myfilename;
	};

	class ui_item_basic 
	{
	public:
		virtual bool intersect(_in(Ray_<float>) _Ray, _out(Hit_<float>) _Result) const {
			return (false);
		}

		virtual bool intersect(_in(Ray_<float>) _Ray) const {
			return (false);
		}

		virtual rect2d region() const {
			std::cout << "[error]" << "[no support get region()]" << "[ui_item]" << std::endl;
			abort();
		}

		size_t _Myaction = 0;
		ui_material_basic* _Mymtl = nullptr;
	};

	class ui_item_disk : public ui_item_basic, public Disk_<real_t> {
	public:
		virtual bool intersect(_in(Ray_<real_t>) _Ray, _out(Hit_<real_t>) _Result) const {
			return ( clmagic::intersect(_Ray, (*this), _Result) );
		}

		virtual bool intersect(_in(Ray_<float>) _Ray) const {
		/*	real_t t;
			return ( disk_equaltion(_Ray.o.x, _Ray.o.y, _Ray.o.z, 
								    _Ray.d.x, _Ray.d.y, _Ray.d.z, 
									this->_Myradius._Min, this->_Myradius._Max, t) );*/
			Hit_<real_t> _Tmp;
			return (this->intersect(_Ray, _Tmp));
		}

		virtual rect2d region() const {
			Vec2 _Pos = Vec2(_Myworld._Mymat.row(3));
			Vec2 _Size = Vec2(_Myradius._Max * real_t(2)) * maxval(_Myworld._Mymat[0][0], _Myworld._Mymat[1][1]);
			return ( rect2d(_Pos, RECT_COORD::CENTER, _Size) );
		}
	};

	class ui_item_rect : public ui_item_basic, public rect2d {
	public:
		virtual bool intersect(_in(Ray_<real_t>) _Ray, _out(Hit_<real_t>) _Result) const {
			return ( rect2d::intersect(Vec2(_Ray.o), _Result) );
		}

		virtual bool intersect(_in(Ray_<float>) _Ray) const {
			return ( rect2d::intersect(Vec2(_Ray.o)) );
		}

		virtual rect2d region() const {
			return (*this);
		}
	};

	class ui_item_polygon : public ui_item_basic {
	public:
		virtual bool intersect(_in(Ray_<real_t>) _Ray, _out(Hit_<real_t>) _Result) const {
			for (const Triangle_<real_t>& _Tri : _Mytriangles) {
				if ( clmagic::intersect(_Ray, _Tri, _Result) ) {
					return ( true );
				}
			}
			return ( false );
		}

		virtual bool intersect(_in(Ray_<float>) _Ray) const {
			Hit_<real_t> _Tmp;
			return (this->intersect(_Ray, _Tmp));
		}

		virtual rect2d region() const {
			if (_Mytriangles.empty()) {
				return (rect2d());
			}

			real_t _Left = _Mytriangles[0]._Myp0.x,
				  _Right = _Mytriangles[0]._Myp0.x,
				    _Top = _Mytriangles[0]._Myp0.y,
				 _Bottom = _Mytriangles[0]._Myp0.y;
			Vec3 _Center = Vec3(real_t(0));

			for (const Triangle_<real_t>& _Tri : _Mytriangles) {
				_Center += _Tri._Myp0;
				_Center += _Tri._Myp1;
				_Center += _Tri._Myp2;
				_Left = minval(_Left, _Tri._Myp0.x);
				_Left = minval(_Left, _Tri._Myp1.x);
				_Left = minval(_Left, _Tri._Myp2.x);
				_Right = maxval(_Right, _Tri._Myp0.x);
				_Right = maxval(_Right, _Tri._Myp1.x);
				_Right = maxval(_Right, _Tri._Myp2.x);
				_Top = maxval(_Top, _Tri._Myp0.y);
				_Top = maxval(_Top, _Tri._Myp1.y);
				_Top = maxval(_Top, _Tri._Myp2.y);
				_Bottom = minval(_Bottom, _Tri._Myp0.y);
				_Bottom = minval(_Bottom, _Tri._Myp1.y);
				_Bottom = minval(_Bottom, _Tri._Myp2.y);
			}

			return (rect2d(Vec2(_Center / real_t(_Mytriangles.size() * 3)),
					RECT_COORD::CENTER,
					Vec2(_Right - _Left, _Top - _Bottom)) );
		}

		std::vector<Triangle_<real_t>> _Mytriangles;
	};

	class ui 
	{
	public:
		ui() : _Myitems(), _Mytex(0, 0, CV_32FC4), _Myurl(){
		
		}

		void destroy() {
			_Myitems.clear();
			_Mytex.release();
		}

		void initializer(std::string _Url, size_t _Width, size_t _Height) {
			_Mytex = cv::mat(_Height, _Width, CV_32FC4);
			_Myitems.clear();
			_Myurl = _Url;
		}
		
		cv::mat generate(cv::Rect _Range);

		cv::mat refresh() {
			return (generate(cv::Rect()));
		}

		void push_back(std::shared_ptr<ui_item_basic> _Item) {
			_Myitems.push_back(_Item);
			auto _Region = _Item->region();
			this->generate( cv::Rect(_Region.left<int>(),
				_Region.bottom<int>(),
				_Region.width<int>(),
				_Region.height<int>()) );
		}

		std::vector<
			std::shared_ptr<ui_item_basic> > _Myitems;
		cv::mat _Mytex;
		std::string _Myurl;
	};
}// namespace clmagic

#endif