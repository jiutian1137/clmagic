#include "../src/clmagic/improc.h"
using namespace::clmagic;


class brush {
public:

	virtual void draw_in(cv::mat& _Src) const {
		gradient(
			_Mymask,
			cv::Size(_Mysize, _Mysize),
			{ std::pair<float, cv::Vec3b>(0.0f, cv::Vec3b(255, 255, 255)), std::pair<float, cv::Vec3b>(1.0f, cv::Vec3b(0, 0, 0)) }
		);
		cv::mat _Roi_src = _Src(cv::Rect(cv::Point(_Mypos.x, _Mypos.y), cv::Size(_Mysize, _Mysize)));
		cv::addWeighted(_Roi_src, (1.0 - _Myopacity), _Mymask, _Myopacity, 1.0f, _Roi_src);

		/*cv::Vec3f _Color = cv::Vec3f(255.f, 0.f, 0.f);

		cv::mat _Roi_src = _Src( cv::Rect(cv::Point(_Mypos.x, _Mypos.y), cv::Size(_Mysize, _Mysize)) );
		cv::Point _Src_center = _Roi_src.size() / 2;
		for (int i = 0; i != _Roi_src.rows; ++i) {
			for (int j = 0; j != _Roi_src.cols; ++j) {
				double _Dis = std::sqrt( pow(_Src_center.x - j, 2) + pow(_Src_center.y - i, 2) );
				if (_Dis < _Mysize / 2.0) {
					auto& _Rval = _Roi_src.at<cv::Vec3b>(i, j);
					_Rval = static_cast<cv::Vec3b>(cv::Vec3f(_Rval) * (1.0f - _Myopacity) + _Color * _Myopacity);
				}
			}
		}*/

	}

	float _Mysize;
	Vec2 _Mypos;
	float _Myopacity = 1.0f;
	cv::mat _Mymask;
};

brush g_test_brush;

struct particle_emmit {
	float _Period;
	float _Size;
	cv::Point2f _Pos;
	cv::Point2f _Dir;
	cv::Vec3b _Color0;
	cv::Vec3b _Color1;


	void draw_in(cv::mat& _Src, float t) {
		t = fmod(t, _Period);
		auto _Point = _Pos + t * _Dir;
		if (_Point.x >= 0 && _Point.y >= 0 && _Point.x < _Src.cols-1 && _Point.y < _Src.rows-1)
			_Src.at<cv::Vec3b>(_Point) += cv::Vec3b(
				lerp(lerp(cv::Vec3f(_Color0), cv::Vec3f(_Color1), t / _Period),
					 cv::Vec3f(_Src.at<cv::Vec3b>(_Point)),
					 0.7f
					)
				);
	}
};

cv::mat _Src;
cv::mat _Dst;
particle_emmit g_test_emmit;


void onMouseCallback(int event, int x, int y, int flags, void* userdata) {
	if (flags == cv::EVENT_MOUSEMOVE) {
		g_test_brush._Mypos = Vec2(x - g_test_brush._Mysize / 2.0f, y - g_test_brush._Mysize / 2.0f);
	}
}

int main(int argc, char** argv) {
	_Src = cv::imread("YangXia_eye.jpg");
	cv::resize(_Src, _Src, _Src.size() / 2);
	_Dst = _Src.clone();

	g_test_brush._Mypos = Vec2(_Src.cols / 2, _Src.rows / 2);
	g_test_brush._Mysize = 100.0f;
	g_test_brush._Myopacity = 0.3f;

	g_test_emmit._Pos = cv::Point2f(450.f, 450.f);
	g_test_emmit._Dir = cv::Point2f(-1.f, -1.f);
	g_test_emmit._Color0 = cv::Vec3b(213, 30, 22);
	g_test_emmit._Color1 = cv::Vec3b(120, 30, 22);
	g_test_emmit._Period = 10.0f;

	constexpr int _Emit_size = 200;
	particle_emmit _Emits[_Emit_size];
	for (int i = 0; i != _Emit_size; ++i) {
		_Emits[i]._Pos = cv::Point2f(200.0f + (rand() % 200), 200.0f + (rand() % 200));
		_Emits[i]._Dir = cv::Point2f(-1.f, -1.f);
		_Emits[i]._Color0 = cv::Vec3b(255, 30, 22);
		_Emits[i]._Color1 = cv::Vec3b(213, 30, 22);
		_Emits[i]._Period = 100.0f;
	}

	cv::namedWindow("show");
	cv::setMouseCallback("show", onMouseCallback);

	clock_t _Start = clock();
	while (true) {
		float t = float(clock() - _Start) * 0.01f;
		for (int i = 0; i != _Emit_size; ++i) {
			_Emits[i].draw_in(_Dst, t);
		}
		g_test_brush.draw_in(_Dst);
		
		cv::imshow("show", _Dst);
		cv::waitKey(33);
		_Dst = _Src.clone();
	}

	return (0);
}