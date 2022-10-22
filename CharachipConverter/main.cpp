#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;

int main(int argc, char* argv[]) {

	int col_num = 1;				// 列方向の分割数
	int row_num = 4;				// 行方向の分割数

	cv::Mat input_image = cv::imread(argv[1], -1);	// 分割する画像の取得
	if (input_image.empty() == true) {
		std::cerr << "入力画像が見つからない" << std::endl;
		return false;
	}

	int width = input_image.cols;	// 入力画像の横の長さ
	int height = input_image.rows;	// 入力画像の縦の長さ
	std::cout << "Width = " << width << " / Height = " << height << std::endl;

	int s_width = width % col_num;						// 横方向の余り
	int* div_width = new int[col_num];		// 分割後画像の横の長さを入れる配列

	for (int c = 0; c < col_num; c++) {

		if (s_width > 0) {
			div_width[c] = width / col_num + 1;    // 余りを割り当てる
			s_width--;
		}
		else {
			div_width[c] = width / col_num;
		}
	}

	int s_height = height % row_num;					// 縦方向の余り
	int* div_height = new int[row_num];		// 分割後画像の縦の長さを入れる配列

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
	cv::imwrite("test.png", new_image);

	return 0;
}