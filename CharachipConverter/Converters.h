#pragma once
#include "opencv2/opencv.hpp"
#include <filesystem>
#include <functional>
#include <string>

using namespace cv;

using LogFunc = std::function<void(const std::string&)>;

void defaultLog(const std::string& message);

cv::Mat PinP_tr(const cv::Mat& srcImg, const cv::Mat& smallImg, const int tx, const int ty);
void convert_bakin(String path, const std::string& outputDir, int col_num, int row_num, const LogFunc& log = defaultLog);
void convert_isekai(String path, const std::string& outputDir, int col_num, int row_num, const LogFunc& log = defaultLog);
void convert_isekai_face(String path, const std::string& outputDir, int col_num, int row_num, const LogFunc& log = defaultLog);
