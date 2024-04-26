#pragma once
#include "opencv2/opencv.hpp"
#include <filesystem>

using namespace cv;

cv::Mat PinP_tr(const cv::Mat& srcImg, const cv::Mat& smallImg, const int tx, const int ty)
{
	//背景画像の作成
	cv::Mat dstImg;
	srcImg.copyTo(dstImg);

	//前景画像の変形行列
	cv::Mat mat = (cv::Mat_<double>(2, 3) << 1.0, 0.0, tx, 0.0, 1.0, ty);

	//アフィン変換の実行
	cv::warpAffine(smallImg, dstImg, mat, dstImg.size());
	return dstImg;
}

void convert_bakin(String path, int col_num, int row_num) {

	std::filesystem::path filepath = path;
	cv::Mat input_image = cv::imread(filepath.generic_string(), -1); // 分割する画像の取得

	if (input_image.empty() == true) {
		std::cerr << "入力画像が見つからない" << std::endl;
		return;
	}

	int width = input_image.cols; // 入力画像の横の長さ
	int height = input_image.rows; // 入力画像の縦の長さ
	std::cout << "Width = " << width << " / Height = " << height << std::endl;

	int s_width = width % col_num; // 横方向の余り
	int* div_width = new int[col_num]; // 分割後画像の横の長さを入れる配列

	for (int c = 0; c < col_num; c++) {

		if (s_width > 0) {
			div_width[c] = width / col_num + 1; // 余りを割り当てる
			s_width--;
		}
		else {
			div_width[c] = width / col_num;
		}
	}

	int s_height = height % row_num; // 縦方向の余り
	int* div_height = new int[row_num]; // 分割後画像の縦の長さを入れる配列

	for (int r = 0; r < row_num; r++) {

		if (s_height > 0) {
			div_height[r] = height / row_num + 1;    // 余りを割り当てる
			s_height--;
		}
		else {
			div_height[r] = height / row_num;
		}
	}
	// 分割画像の出力
	int x = 0;
	int y = 0;
	cv::Mat new_image;
	cv::Mat split[4];

	for (int r = 0; r < row_num; r++) {
		for (int c = 0; c < col_num; c++) {

			// 分割する画像の(x, y, width, height)をRectに入力
			cv::Rect crop_region = cv::Rect(x, y, div_width[c], div_height[r]);
			std::cout << "DIV " << (c + r * col_num) << "  RECT : " << crop_region << std::endl;

			// 分割画像を取得
			cv::Mat div_image = input_image(crop_region);

			split[r] = div_image;

			x += div_width[c];
		}
		y += div_height[r];
		x = 0;
	}

	cv::vconcat(split[0], split[1], new_image);
	cv::vconcat(new_image, split[3], new_image);
	cv::vconcat(new_image, split[2], new_image);

	// 画像の出力
	cv::imwrite("./output/bakin_" + filepath.filename().string(), new_image);
}

void convert_isekai(String path, int col_num, int row_num) {

	std::filesystem::path filepath = path;
	cv::Mat input_image = cv::imread(filepath.generic_string(), -1); // 分割する画像の取得

	if (input_image.empty() == true) {
		std::cerr << "入力画像が見つからない" << std::endl;
		return;
	}

	int width = 96;
	int height = 256;

	cv::Mat convert_mat, work_mat;
	//前処理　余った余白は黒で埋める
	work_mat = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

	//リサイズ
	int i_width = input_image.cols; // 入力画像の横の長さ
	int i_height = input_image.rows; // 入力画像の縦の長さ
	std::cout << "Width = " << i_width << " / Height = " << i_height << std::endl;

	int s_width = i_width % col_num; // 横方向の余り
	int* div_width = new int[col_num]; // 分割後画像の横の長さを入れる配列

	for (int c = 0; c < col_num; c++) {

		if (s_width > 0) {
			div_width[c] = i_width / col_num + 1; // 余りを割り当てる
			s_width--;
		}
		else {
			div_width[c] = i_width / col_num;
		}
	}

	int s_height = i_height % row_num; // 縦方向の余り
	int* div_height = new int[row_num]; // 分割後画像の縦の長さを入れる配列

	for (int r = 0; r < row_num; r++) {

		if (s_height > 0) {
			div_height[r] = i_height / row_num + 1;    // 余りを割り当てる
			s_height--;
		}
		else {
			div_height[r] = i_height / row_num;
		}
	}

	// 分割画像の出力
	int x = 0;
	int y = 0;
	cv::Mat new_image;
	cv::Mat wsplit[3];
	cv::Mat hsplit[4];

	for (int r = 0; r < row_num; r++) {
		for (int c = 0; c < col_num; c++) {

			// 分割する画像の(x, y, width, height)をRectに入力
			cv::Rect crop_region = cv::Rect(x, y, div_width[c], div_height[r]);
			std::cout << "DIV " << (c + r) << "  RECT : " << crop_region << std::endl;

			// 分割画像を取得
			cv::Mat div_image = input_image(crop_region);
			cv::Mat clip = cv::Mat::zeros(cv::Size(32, 64), CV_8UC3);
			cv::Mat image = PinP_tr(clip, div_image, 0, 24);
			//cv::imshow("test", image);
			//cv::waitKey(0);
			wsplit[c] = image;

			x += div_width[c];
		}
		cv::hconcat(wsplit[0], wsplit[1], hsplit[r]);
		cv::hconcat(hsplit[r], wsplit[2], hsplit[r]);
		y += div_height[r];
		x = 0;
	}

	cv::vconcat(hsplit[0], hsplit[1], new_image);
	cv::vconcat(new_image, hsplit[2], new_image);
	cv::vconcat(new_image, hsplit[3], new_image);

	//cv::imshow("test", new_image);
	//cv::waitKey(0);

	// 画像の出力
	cv::imwrite("./output/isekai_" + filepath.filename().string(), new_image);
}