/*
	OpenCV program to verify install on Windows x64 machine
	author: JDG
*/
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

static void distortImg(const char* img_name) {
	Mat image = imread(img_name);
	int blur_size = 3;
	if (image.empty()) {
		cout << "Could not open or find the image" << endl;
		exit(1);
	}
	resize(image, image, Size(300, 300));
	while (blur_size++ <= 85) {
		blur(image, image, Size(blur_size, blur_size));
		cout << blur_size << endl;
		imshow("Test", image);
		waitKey(10);
	}
}

int main(int argc, char** argv) {
	distortImg("kush.png");
	destroyAllWindows(); 	
	return 0;
}