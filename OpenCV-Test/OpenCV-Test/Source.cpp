/*
	OpenCV program to verify install on Windows x64 machine
	author: JDG
*/
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

static void getWebCam() {
	VideoCapture cap;
	Mat frame;
	if (!cap.open(0)) {
		cout << "Could not open or find the webam" << endl;
		exit(1);
	}
	while (1) {
		cap.read(frame);
		if (frame.empty()) break; 
		blur(frame, frame, Size(35, 35));
		imshow("WebCam", frame);
		if (waitKey(10) == 27) break; 
	}

}

static void distortImg(const char* img_name) {
	Mat image = imread(img_name);
	int blur_size = 0;
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
	//distortImg("kush.png");
	getWebCam();
	destroyAllWindows(); 	
	return 0;
}/*
	OpenCV program to verify install on Windows x64 machine
	author: JDG
*/
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

static void getWebCam() {
	VideoCapture cap;
	Mat frame;
	if (!cap.open(0)) {
		cout << "Could not open or find the webam" << endl;
		exit(1);
	}
	while (1) {
		cap.read(frame);
		if (frame.empty()) break; 
		blur(frame, frame, Size(35, 35));
		imshow("WebCam", frame);
		if (waitKey(10) == 27) break; 
	}

}

static void distortImg(const char* img_name) {
	Mat image = imread(img_name);
	int blur_size = 0;
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
	//distortImg("kush.png");
	getWebCam();
	destroyAllWindows(); 	
	return 0;
}
