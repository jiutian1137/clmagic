#include "imgproc.h"

namespace clmagic 
{
	class _Lightleak_mask
	{
	public:
		_Lightleak_mask(const cv::Mat& _Src, const cv::Mat& _Mask)
			: _Mysrc(_Src), _Mymask(_Mask)
			{	// 
			}

		void operator() (cv::Mat& _Dst) const
			{
			using namespace::cv;

			auto _Algrthm = [](int _Val, int _Maskval) -> unsigned char
				{
				return (_Maskval <= 128
					? saturate_cast<unsigned char>(_Maskval * _Val / 128.0)
					: saturate_cast<unsigned char>(255 - (255 - _Maskval) * (255 - _Val) / 128.0)
					);
				};

			_Dst = Mat(_Mysrc.size(), _Mysrc.type());

			for (int i = 0; i != _Mysrc.rows; ++i)
				{
				for (int j = 0; j != _Mysrc.cols; ++j)
					{
					auto& _Bgr = _Mysrc.at<Vec3b>(i, j);
					auto& _Bgrmask = _Mymask.at<Vec3b>(i, j);

					_Dst.at<Vec3b>(i, j) = Vec3b(_Algrthm(_Bgr[0], _Bgrmask[0]),
						_Algrthm(_Bgr[1], _Bgrmask[1]),
						_Algrthm(_Bgr[2], _Bgrmask[2]));
					}
				}
			}

		void generate(cv::Mat& _Dst) const 
			{
			(*this)(_Dst);
			}

		cv::Mat _Mysrc;
		cv::Mat _Mymask;
	};


	void lightleak(cv::InputArray _Src, cv::InputArray _Mask, cv::OutputArray _Dst)
		{
		cv::Mat _Lightleakmask;

		_Lightleak_mask _Generator(_Src.getMat(), _Mask.getMat());
		_Generator.generate(_Lightleakmask);

		cv::add( _Lightleakmask, _Src.getMat(), _Dst.getMatRef() );
		}

}// namespace clmagic 