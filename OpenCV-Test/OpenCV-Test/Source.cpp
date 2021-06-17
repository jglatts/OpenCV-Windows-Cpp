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

uint16_t face_count = 0;

static void detectFace(Mat& img, CascadeClassifier& cascade, CascadeClassifier& nestedCascade) {
	vector<Rect> faces;
	Mat gray, small_img;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	resize(gray, small_img, Size(), 1.0, 1.0, INTER_LINEAR);
	// face detection 
	cascade.detectMultiScale(small_img, faces, 1.3, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	for (size_t i = 0; i < faces.size(); i++)
	{
		Rect r = faces[i];
		Mat img_roi;
		vector<Rect> eyes;
		Point center;
		Scalar color = Scalar(255, 0, 0); 
		Scalar eye_color = Scalar(0, 255, 0); 
		int radius;
		double aspect_ratio = (double)r.width / r.height;
		if (0.75 < aspect_ratio && aspect_ratio < 1.3){
			center.x = cvRound((r.x + r.width * 0.5));
			center.y = cvRound((r.y + r.height * 0.5));
			radius = cvRound((r.width + r.height) * 0.25);
			circle(img, center, radius, color, 3, 8, 0);
		}
		img_roi = small_img(r);	// construct a Mat object with a detected Face(ROI)
		imshow("ROI FaceImage", img_roi);
		// eye detection 
		nestedCascade.detectMultiScale(img_roi, eyes, 1.3, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
		for (size_t j = 0; j < eyes.size(); j++) {
			Rect nr = eyes[j];
			center.x = cvRound((r.x + nr.x + nr.width * 0.5));
			center.y = cvRound((r.y + nr.y + nr.height * 0.5));
			radius = cvRound((nr.width + nr.height) * 0.25);
			circle(img, center, radius, eye_color, -2, 8, 0);
		}
		cout << "Face Count: " << face_count++ << endl;
	}
	imshow("Face Detection", img);
}

static void runDetection() {
	VideoCapture cap;
	Mat frame;
	CascadeClassifier face_cascade, eye_cascade;
	String path_nested = "C:\\Users\\johng\\opencv\\sources\\data\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
	String path_cas = "C:\\Users\\johng\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
	if (!(eye_cascade.load(path_nested))) {
		cout << "Didnt Load EyeCascade" << endl;
		exit(1);
	}
	if (!(face_cascade.load(path_cas))) {
		cout << "Didnt Load FaceCascade" << endl;
		exit(1);
	}
	if (!cap.open(0)) {
		cout << "Could not open or find the webam" << endl;
		exit(1);
	}
	while (1) {
		cap.read(frame);
		if (frame.empty()) break;
		detectFace(frame, face_cascade, eye_cascade);
		if (waitKey(1) == ESC_KEY) break;
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
