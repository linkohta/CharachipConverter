#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;

int main(int argc, char* argv[]) {

	int col_num = 1;				// ������̕�����
	int row_num = 4;				// �s�����̕�����

	cv::Mat input_image = cv::imread(argv[1], -1);	// ��������摜�̎擾
	if (input_image.empty() == true) {
		std::cerr << "���͉摜��������Ȃ�" << std::endl;
		return false;
	}

	int width = input_image.cols;	// ���͉摜�̉��̒���
	int height = input_image.rows;	// ���͉摜�̏c�̒���
	std::cout << "Width = " << width << " / Height = " << height << std::endl;

	int s_width = width % col_num;						// �������̗]��
	int* div_width = new int[col_num];		// ������摜�̉��̒���������z��

	for (int c = 0; c < col_num; c++) {

		if (s_width > 0) {
			div_width[c] = width / col_num + 1;    // �]������蓖�Ă�
			s_width--;
		}
		else {
			div_width[c] = width / col_num;
		}
	}

	int s_height = height % row_num;					// �c�����̗]��
	int* div_height = new int[row_num];		// ������摜�̏c�̒���������z��

	for (int r = 0; r < row_num; r++) {

		if (s_height > 0) {
			div_height[r] = height / row_num + 1;    // �]������蓖�Ă�
			s_height--;
		}
		else {
			div_height[r] = height / row_num;
		}
	}

	// �����摜�̏o��
	int x = 0;
	int y = 0;
	cv::Mat new_image;
	cv::Mat split[4];

	for (int r = 0; r < row_num; r++) {
		for (int c = 0; c < col_num; c++) {

			// ��������摜��(x, y, width, height)��Rect�ɓ���
			cv::Rect crop_region = cv::Rect(x, y, div_width[c], div_height[r]);
			std::cout << "DIV " << (c + r * col_num) << "  RECT : " << crop_region << std::endl;

			// �����摜���擾
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

	// �摜�̏o��
	cv::imwrite("test.png", new_image);

	return 0;
}