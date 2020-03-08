#include "imgproc.h"
#include <map>

namespace clmagic 
{
	
	class gradient_mask
	{
	public:
		gradient_mask(const cv::Size& _Dstsize, const cv::Size& _Ksize, const cv::Point& _Anchor)
			: _Mysize(_Dstsize), _Myksize(_Ksize), _Myanchor(_Anchor)
			{
			}

		void operator() (cv::mat& _Dst) const
			{
			using namespace::cv;

			float _Radius = float(sqrt(pow(_Myksize.width / 2, 2) + pow(_Myksize.height / 2, 2)));
			mat _X(_Mysize, CV_32F);
			mat _Y(_Mysize, CV_32F);
			for (int i = 0; i != _X.rows; ++i)
				{
				for (int j = 0; j != _X.cols; ++j)
					{
					_X.at<float>(i, j) = float(j - _Myanchor.x);
					_Y.at<float>(i, j) = float(i - _Myanchor.y);
					}
				}
			mat _Magnitude;
			magnitude(_X, _Y, _Magnitude);
			for (int i = 0; i != _X.rows; ++i) {
				for (int j = 0; j != _X.cols; ++j) {
					if (_Magnitude.at<float>(i, j) > _Radius) {
						_Magnitude.at<float>(i, j) = 0.0f;
					}
				}
			}
			_Magnitude /= _Radius;
			
			/*for (int i = 0; i != _X.rows; ++i)
			{
				for (int j = 0; j != _X.cols; ++j)
				{
					_Magnitude.at<float>(i, j) = fmod(_Magnitude.at<float>(i, j), 1.0f);
				}
			}*/

			_Dst = _Magnitude;

			//const int _8step = 8;
			//const int _4step = 4;

			//_Dst = mat(_Mysize, CV_32F);

			//__m128i _Ksize128_w = _mm_set1_epi32(_Myksize.width);
			//__m128i _Ksize128_h = _mm_set1_epi32(_Myksize.height);

			//__m128i _Anchor128_x = _mm_set1_epi32(_Myanchor.x);
			//__m128i _Anchor128_y = _mm_set1_epi32(_Myanchor.y);

			//__m128 _Dis0 = _mm_set1_ps(float(sqrt(pow(_Myksize.width / 2, 2) + pow(_Myksize.height / 2, 2))));

			//for (int i = 0; i < _Dst.rows; ++i) 
			//	{
			//	for (int j = 0; j < _Dst.cols; j += _4step)
			//		{
			//		/* _Pos = (point(j, i) - anchor) % ksize
			//		   dis1 = sqrt(pow(_Pos.x, 2) + pow(_Pos.y, 2))
			//		   dis0 = sqrt(pow(ksize.x, 2) + pow(ksize.y, 2))
			//		   dis1 / dis0
			//		*/

			//		// _Pos = (point(j, i) - anchor) % ksize
			//		__m128i _Pos128_x = _mm_set_epi32( j + 3, j + 2, j + 1, j );
			//		__m128i _Pos128_y = _mm_set1_epi32( i );

			//		_Pos128_x = _mm_sub_epi32(_Anchor128_x, _Pos128_x);
			//		_Pos128_y = _mm_sub_epi32(_Anchor128_y, _Pos128_y);
			//		/*
			//		@_box effect:
			//		for (int k = 0; k != _4step; ++k) 
			//			{
			//			_Pos128_x.m128i_i32[k] %= _Ksize128_w.m128i_i32[k];
			//			_Pos128_y.m128i_i32[k] %= _Ksize128_h.m128i_i32[k];
			//			}
			//		*/

			//		// dis1 = sqrt( pow(_Pos.x, 2) + pow(_Pos.y, 2) )
			//		_Pos128_x = _mm_mullo_epi32(_Pos128_x, _Pos128_x);
			//		_Pos128_y = _mm_mullo_epi32(_Pos128_y, _Pos128_y);

			//		_Pos128_x = _mm_add_epi32(_Pos128_x, _Pos128_y);
			//		__m128 _Dis1;
			//		for (int k = 0; k != _4step; ++k)
			//			{
			//			_Dis1.m128_f32[k] = sqrtf( float(_Pos128_x.m128i_i32[k]) );
			//			}

			//		// dis1 / dis0
			//		_Dis1 = _mm_div_ps(_Dis1, _Dis0);
			//		for (int k = 0; k != _4step; ++k)
			//			{
			//			_Dis1.m128_f32[k] = fmod(_Dis1.m128_f32[k], 1.0f);
			//			}
			//		_mm_store1_ps(_Dst.ptr<float>(i, j), _Dis1);
			//		}
			//	}
			}

		void generate(cv::mat& _Dst) const
			{
			(*this)(_Dst);
			}

		cv::Size _Mysize;
		cv::Size _Myksize;
		cv::Point _Myanchor;
	};


	class gradient_rgb 
	{
	public:
		gradient_rgb(const cv::mat& _Mask, const std::map<float, cv::Vec3b>& _Colors)
			: _Mymask(_Mask), _Mycolors(_Colors)
			{
			}

		void operator() (cv::mat& _Dst) 
			{
			_Dst = cv::mat(_Mymask.size(), CV_8UC3);

			__m128 _One128 = _mm_set1_ps(1.0f);

			const int _4step = 4;

			for (int i = 0; i < _Mymask.rows; ++i) 
				{
				for (int j = 0; j < _Mymask.cols; ++j)
					{
					float _Value = _Mymask.at<float>(i, j);

					auto _First = _Mycolors.begin();
					std::pair<float, cv::Vec3b> _Color_prev = ( *_First++ );
					std::pair<float, cv::Vec3b> _Color_real;

					for ( ; _First != _Mycolors.end(); ++_First) 
						{
						_Color_real = *_First;

						if (_Value >= _Color_prev.first && _Value <= _Color_real.first) 
							{
							__m128 _Value128 = _mm_set1_ps( (_Value - _Color_prev.first) / (_Color_real.first - _Color_prev.first) );
							__m128 _Lhs128 = _mm_set_ps(0.0f, _Color_prev.second[2], _Color_prev.second[1], _Color_prev.second[0]);
							__m128 _Rhs128 = _mm_set_ps(0.0f, _Color_real.second[2], _Color_real.second[1], _Color_real.second[0]);
							_Value128 = _mm_add_ps( _mm_mul_ps(_mm_sub_ps(_One128, _Value128), _Lhs128), _mm_mul_ps(_Value128, _Rhs128) );

							_Dst.at<cv::Vec3b>(i, j) = cv::Vec3b(
								cv::saturate_cast<unsigned char>(_Value128.m128_f32[0]),
								cv::saturate_cast<unsigned char>(_Value128.m128_f32[1]),
								cv::saturate_cast<unsigned char>(_Value128.m128_f32[2]) );

							break;
							}

						_Color_prev = _Color_real;
						}
					// next
					}
				}
			}

		const cv::mat& _Mymask;
		const std::map<float, cv::Vec3b>& _Mycolors;
	};


	void gradient(cv::OutputArray _Dst, cv::Size _Dstsize, std::map<float, cv::Vec3b> _Colors, cv::Size _Ksize, cv::Point _Anchor)
		{
		if (_Ksize.width < 0 || _Ksize.height < 0)
			{	// default dstsize
			_Ksize = _Dstsize;
			}

		if (_Anchor.x < 0 || _Anchor.y < 0) 
			{	// default center
			_Anchor = _Dstsize / 2;
			}

		cv::mat _Mask;
		auto _Gradgenerator = gradient_mask(_Dstsize, _Ksize, _Anchor);
		_Gradgenerator.generate( _Mask );

		if (_Colors.find(0.0f) == _Colors.end()) 
			{
			_Colors.insert_or_assign(0.0f, cv::Vec3b(0, 0, 0));
			}
		if (_Colors.find(1.0f) == _Colors.end()) 
			{
			_Colors.insert_or_assign(1.0f, cv::Vec3b(255, 255, 255));
			}

		auto _Rgbproc = gradient_rgb(_Mask, _Colors);
		_Rgbproc(_Dst.getmatRef());
		}

}// namespace clmagic