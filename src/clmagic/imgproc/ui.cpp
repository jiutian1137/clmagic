#include "ui.h"

namespace clmagic {
	cv::mat ui::generate(cv::Rect _Range) {
		if (_Range.area() <= 0) {
			_Range = cv::Rect(0, 0, _Mytex.cols, _Mytex.rows);
		}

		Hit _Hit;
		for (size_t i = _Range.y, i_end = i + _Range.height; i < i_end; ++i) {
			for (size_t j = _Range.x, j_end = j + _Range.width; j < j_end; ++j) {
				Ray _Ray;
				_Ray.o = Vec3(real_t(j), real_t(i), real_t(1));
				_Ray.d = Vec3(real_t(0), real_t(0), real_t(-1));

				for (const std::shared_ptr<ui_item_basic>& _Item : _Myitems) {
					if (_Item->intersect(_Ray, _Hit)) {
						_Mytex.at<cv::Vec4f>(i, j) = const_reference_cast<const cv::Vec4f>(_Item->_Mymtl->color(_Hit._Myuv));
						break;
					}
				}
			}
		}
		return (_Mytex);
	}

}// namespace clmagic