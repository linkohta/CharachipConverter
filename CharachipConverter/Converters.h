#pragma once
#include "opencv2/opencv.hpp"
#include <filesystem>

using namespace cv;

cv::Mat PinP_tr(const cv::Mat& srcImg, const cv::Mat& smallImg, const int tx, const int ty);
void convert_bakin(String path, int col_num, int row_num);
void convert_isekai(String path, int col_num, int row_num);