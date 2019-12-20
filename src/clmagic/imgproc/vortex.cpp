#include "imgproc.h"

namespace clmagic 
{
	class _Vortex_map 
	{
	public:
		_Vortex_map(const cv::Size& _Dstsize, float _Radians, float _Radius, cv::Point _Anchor)
			: _Mysize(_Dstsize), _Myradians(_Radians), _Myradius(_Radius), _Myanchor(_Anchor)
			{
			}


		/*      

									| | | | | |
							  | | | | | | | | | | | |
							| | | | | | | | | | | | | | | | |
						| | | | | | | | | | | | | | | | | | | | | | 
					  | | | | | | | | | | | | | | | | | | | | | | | | | | 
					| | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
				  | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
				| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
				| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
			  | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
			| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
			--------------------------------------------------------------------------------------
		dis 0                  dis < 0.5 * _Radius                                          _Radius

		this is sin(log(x)) function
		*/
		void operator() (cv::Mat& _Remap) const
			{
			_Remap = cv::Mat(_Mysize, CV_32FC2);

			for (int i = 0; i != _Mysize.height; ++i)
				{
				for (int j = 0; j != _Mysize.width; ++j)
					{
					int _Dx = j - _Myanchor.x;
					int _Dy = i - _Myanchor.y;

					float _Dis = std::sqrtf(float(_Dx * _Dx) + float(_Dy * _Dy));
					if (_Dis >= _Myradius) 
						{	// out of bound
						_Remap.at<cv::Vec2f>(i, j) = cv::Vec2f( float(j), float(i) );
						continue;
						}
					float _Theta = std::atan2f(float(_Dx), float(_Dy));
					
					// [ 0.0 -> 1.0 ]
					float _Off = (_Dis / _Myradius); 

					// [ 1.0 -> 256.0 ]
					_Off = _Off * 255.0f + 1.0f;

					// [ 0.0 -> 8.0 ]
					_Off = log2f(_Off);

					// [ 0.0 -> 1.0 -> 0.0]
					_Off = sin( (3.14f / 8.0f) * _Off );

					//[ 0.0 -> _Myradians -> 0.0 ]
					_Off *= _Myradians;
					
					float _X = _Dis * std::cos(_Theta + _Off) + _Myanchor.x;
					float _Y = _Dis * std::sin(_Theta + _Off) + _Myanchor.y;
					/*if (_X > -1 && _Y > -1 && _X < _Mysize.height && _Y < _Mysize.height) 
						{
						_Remap.at<cv::Vec2f>(_Y, _X) = cv::Vec2f(j, i);
						}*/
					_Remap.at<cv::Vec2f>(i, j) = cv::Vec2f(_X, _Y);
					}
				}
			}

		void generate(cv::Mat& _Remap) const
			{
			(*this)(_Remap);
			}

		cv::Size _Mysize;
		float _Myradians;
		float _Myradius;
		cv::Point _Myanchor;
	};


	void vortex(cv::InputArray _Src, cv::OutputArray _Dst, int _Angle, double _Radius, cv::Point _Anchor)
		{
		using namespace::cv;

		if ( _Angle == 0 ) 
			{	// no modify
			if ( _Dst.getMat().data != _Src.getMat().data ) 
				{
				_Dst.getMatRef() = _Src.getMat().clone();
				}
			return;
			}

		if (_Radius <= 0.0)
			{	// correct radius
			_Radius = std::max( _Src.cols(), _Src.rows() ) ;
			}

		if (_Anchor.x < 0 || _Anchor.y < 0) 
			{	// default center point
			_Anchor = _Src.size() / 2;
			}

		cv::Mat _Mapping;

		_Vortex_map _Generator( _Src.size(), float(_Angle) * 3.14f / 180.0f + 1.57f, float(_Radius), _Anchor );
		_Generator.generate(_Mapping);
		flip(_Mapping, _Mapping, 1);

		cv::remap( _Src, _Dst, _Mapping, cv::Mat(), INTER_LINEAR, BORDER_REPLICATE);
		}
}// namespace clmagic