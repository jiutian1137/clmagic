#pragma once
#ifndef _CLMAGIC_IMGPROC_H_
#define _CLMAGIC_IMGPROC_H_
#include <opencv.hpp>

namespace clmagic 
{

	void gradient(cv::OutputArray _Dst, cv::Size _Dstsize, std::map<float, cv::Vec3b> _Colors, 
		cv::Size _Ksize = cv::Size(-1, -1), cv::Point _Anchor = cv::Point(-1, -1));

	void vortex(cv::InputArray _Src, cv::OutputArray _Dst,
		int _Angle, double _Radius = -1.0, cv::Point _Anchor = cv::Point(-1, -1));

	void lightleak(cv::InputArray _Src, cv::InputArray _Mask, cv::OutputArray _Dst);

}// namespace clmagic

#endif