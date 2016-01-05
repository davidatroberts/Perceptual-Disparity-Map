#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Util.hpp"

int main(int argc, char **argv) {
	// check we have all the arguments required
	if (argc < 6) {
		std::cerr << "Command line argument missing" << std::endl;
		exit(1);
	}

	// get the cmd line arguments
	std::string left_str = std::string(argv[1]);
	std::string right_str = std::string(argv[2]);
	std::string target_region_str = std::string(argv[3]);
	std::string window_size_str = std::string(argv[4]);
	std::string file_name = std::string(argv[5]);

	// parse the int values
	const int target_region = std::stoi(target_region_str);
	const int window_size = std::stoi(window_size_str);

	// load the images
	cv::Mat left_im = cv::imread(left_str, CV_LOAD_IMAGE_COLOR);
	cv::Mat right_im = cv::imread(right_str, CV_LOAD_IMAGE_COLOR);

	// convert to luminance
	cv::Mat lum_left, lum_right;
	cv::cvtColor(left_im, lum_left, CV_RGB2GRAY);
	cv::cvtColor(right_im, lum_right, CV_RGB2GRAY);

	// init the maps
	cv::Mat perceptual_disparity(lum_left.size(), CV_8UC1);
	cv::Mat raw_perceptual_disparity(lum_left.size(), CV_8UC1);
	cv::Mat stereo_simularity(lum_left.size(), CV_8UC1);

	// used to calculate the percentage complete
	float total = left_im.rows * left_im.cols;
	float count = 0;
	float percentage_comp = 0;

	// vectors used to store temporary values during computation
	std::vector<unsigned char> ssim_values;
	std::vector<unsigned int> ssim_positions;
	std::vector<unsigned int> max_ssim_positions;

	// iterate through the pixels in the left image
	for (int i=0; i<left_im.rows; ++i) {
		for (int j=0; j<left_im.cols; ++j) {
			// work out the boundaries for the range indices
			int i_min_index = ((i-window_size) < 0) ? 0 : i-window_size;
			int i_max_index = ((i+window_size) 
				> left_im.rows-1) ? left_im.rows-1 : i+window_size;
			int j_min_index = ((j-window_size) < 0) ? 0 : j-window_size;
			int j_max_index = ((j+window_size) 
				> left_im.cols-1) ? left_im.cols-1 : j+window_size;

			// get the src region around the pixel
			cv::Mat src_region = lum_left(
				cv::Range(i_min_index, i_max_index+1), 
				cv::Range(j_min_index, j_max_index+1)); 

			// get the target regions around the pixels in the right image
			for (int jj = j-(target_region); jj <=j+(target_region); ++jj) {
				// work out the boundaries for the target region
				int jj_min_index = ((jj-window_size) < 0) ? 0 : jj-window_size;
				int jj_max_index = ((jj+window_size)
					> right_im.cols-1) ? right_im.cols-1 : jj+window_size;
				
				// check if region is completely outside of bounds
				if (jj_max_index < 0)
					continue;
				if (jj_min_index > right_im.cols-1)
					break;

				// get the target region
				cv::Mat tgt_region = lum_right(
					cv::Range(i_min_index, i_max_index+1), 
					cv::Range(jj_min_index, jj_max_index+1));

				// check if two regions are the same size
				if (src_region.size() != tgt_region.size())
					continue;

				// calculate ssim 
				float ssim = Util::get_mssim(src_region, tgt_region)[0];

				// map to [0-1] and then [0-255]
				float mapped_ssim = (ssim+1.0f)/2.0f;
				unsigned char ssim_c = (unsigned char)(mapped_ssim*255.0f);

				// place into temp arrays
				ssim_values.push_back(ssim_c);
				ssim_positions.push_back((unsigned int)jj);
			}

			// find the highest value from the ssim list
			auto max_ssim_it = std::max_element(std::begin(ssim_values),
				std::end(ssim_values));
			unsigned char max_ssim = *max_ssim_it;

			// get positions of ssim values that match max
			for (std::vector<unsigned char>::size_type ii=0; 
				ii!=ssim_values.size(); ii++) {
				if (ssim_values[ii] == max_ssim) {
					max_ssim_positions.push_back(
						(unsigned int)ssim_positions[ii]);
				}
			}

			// get the closest position to the current pixel
			int closest_val;
			Util::get_closest(max_ssim_positions, j, closest_val);

			// map so that it's between 0-255 for visual map
			int r_diff = j-closest_val;
			unsigned char diff = (unsigned char)(((((float)r_diff)
				+(float)target_region)/(2.0f*target_region))*255.0f);

			// add tgt_reg from [-tgt_reg, tgt_reg] to [0, (2*tgt_reg)]
			unsigned char raw_diff = r_diff + target_region;

			// place into image matrices
			stereo_simularity.at<unsigned short>(i, j) = max_ssim;
			perceptual_disparity.at<unsigned char>(i, j) = diff;
			raw_perceptual_disparity.at<unsigned char>(i, j) = raw_diff;

			// clear temp arrays
			ssim_values.clear();
			ssim_positions.clear();
			max_ssim_positions.clear();
			count++;
		}
		// output percentage complete
		percentage_comp = ((float)(count)/total)*100.0;
		std::cout << "\r" << percentage_comp << "%       " << std::flush;
	}
	std::cout << "Complete" << std::endl;

	// save the images
	cv::imwrite("../images/" + file_name + "_disparity.bmp", 
		perceptual_disparity);
	cv::imwrite("../images/" + file_name + "_raw_disparity.bmp",
		raw_perceptual_disparity);

	return 0;
}