#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <vector>
#include <map>
#include <opencv2/opencv.hpp>


namespace Util
{
	cv::Scalar get_mssim(const cv::Mat &i1, const cv::Mat &i2);
	int get_closest(std::vector<unsigned int> &vec, int value, int &closest_val);
}

#endif