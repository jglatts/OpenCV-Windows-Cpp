/*
	OpenCV program to verify install on Windows x64 machine
	author: JDG
*/
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/fast_math.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

#define ESC_KEY 27

static void detectFace(Mat& img) {
	vector<Rect> faces, faces2;
	Mat gray, smallImg;
	CascadeClassifier cascade, nestedCascade;
	String path_nested = "C:\\Users\\johng\\opencv\\sources\\data\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
	String path_cas = "C:\\Users\\johng\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
	if (!(nestedCascade.load(path_nested))) {
		cout << "Didnt Load NestedCascade" << endl;
		exit(1);
	}
	if (!(cascade.load(path_cas))) {
		cout << "Didnt Load Cascade" << endl;
		exit(1);
	}
	cvtColor(img, gray, COLOR_BGR2GRAY);
	resize(gray, smallImg, Size(), 1.0, 1.0, INTER_LINEAR);
	equalizeHist(smallImg, smallImg);
	cascade.detectMultiScale(smallImg, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	for (size_t i = 0; i < faces.size(); i++) {
		Rect r = faces[i];
		Mat smallImgROI;
		vector<Rect> nestedObjects;
		Point center;
		Scalar color = Scalar(255, 0, 0); 
		int radius;
		double scale = 1.0;
		double aspect_ratio = (double)r.width / r.height;
		if (0.75 < aspect_ratio && aspect_ratio < 1.3) {
			center.x = cvRound((r.x + r.width * 0.5) * scale);
			center.y = cvRound((r.y + r.height * 0.5) * scale);
			radius = cvRound((r.width + r.height) * 0.25 * scale);
			circle(img, center, radius, color, 3, 8, 0);
		}
		else {
			rectangle(img, Point(cvRound(r.x * scale), cvRound(r.y * scale)),
				Point(cvRound((r.x + r.width - 1) * scale),
					cvRound((r.y + r.height - 1) * scale)), color, 3, 8, 0);
		}
		if (nestedCascade.empty()) continue;
		smallImgROI = smallImg(r);
		nestedCascade.detectMultiScale(smallImgROI, nestedObjects, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
		for (size_t j = 0; j < nestedObjects.size(); j++) {
			Rect nr = nestedObjects[j];
			center.x = cvRound((r.x + nr.x + nr.width * 0.5) * scale);
			center.y = cvRound((r.y + nr.y + nr.height * 0.5) * scale);
			radius = cvRound((nr.width + nr.height) * 0.25 * scale);
			circle(img, center, radius, color, 3, 8, 0);
		}
	}
	imshow("Face Detection", img);
}

static void runDetection() {
	VideoCapture cap;
	Mat frame;
	if (!cap.open(0)) {
		cout << "Could not open or find the webam" << endl;
		exit(1);
	}
	while (1) {
		cap.read(frame);
		if (frame.empty()) break;
		detectFace(frame);
		if (waitKey(10) == ESC_KEY) break;
	}
}

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
		//blur(frame, frame, Size(35, 35));
		imshow("WebCam", frame);
		if (waitKey(10) == ESC_KEY) break; 
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
	//getWebCam();
	setBreakOnError(true);
	runDetection();
	destroyAllWindows(); 	
	return 0;
}
