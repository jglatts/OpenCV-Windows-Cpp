/*
	OpenCV program to verify install on Windows x64 machine
	Also testing some library functionality
	author: JDG
*/
#ifndef __UTILS__H__
#define __UTILS__H__

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

namespace jdg {

	#define ESC_KEY    27
	#define NO_WEBCAM  1
	#define USE_WEBCAM 0
	
	uint16_t face_count = 0;
	
	static double angle(Point pt1, Point pt2, Point pt0) {
		double dx1 = pt1.x - pt0.x;
		double dy1 = pt1.y - pt0.y;
		double dx2 = pt2.x - pt0.x;
		double dy2 = pt2.y - pt0.y;
		return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
	}

	static void setLabel(Mat& im, string label, vector<Point>& contour) {
		int font = FONT_HERSHEY_SIMPLEX;
		double scale = 0.4;
		int thickness = 1;
		int baseline = 0;
		Size text = getTextSize(label, font, scale, thickness, &baseline);
		Rect r = boundingRect(contour);
		Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
		rectangle(im, pt + Point(0, baseline), pt + Point(text.width, -text.height), CV_RGB(255, 255, 255), FILLED);
		putText(im, label, pt, font, scale, CV_RGB(0, 0, 0), thickness, 8);
	}

	static void detectRect(Mat& src, bool check) {
		Mat bw, dst;
		vector<vector<Point>> contours;
		vector<Point> approx;
		int ratio = 3;
		int low_thresh = 50;
		dst = src.clone();
		// https://docs.opencv.org/3.4/da/d5c/tutorial_canny_detector.html
		Canny(dst, bw, low_thresh, low_thresh * ratio, 3);
		findContours(bw.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		for (int i = 0; i < contours.size(); i++) {
			// find the angles -> should be reviewed and tweaked
			approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);
			if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx)) continue;
			int vertices = approx.size();
			vector<double> cosine_corners;
			// add cosine angles 
			for (int j = 2; j < vertices; j++) {
				cosine_corners.push_back(angle(approx[j % vertices], approx[j - 2], approx[j - 1]));
			}
			sort(cosine_corners.begin(), cosine_corners.end());
			double min_cos = cosine_corners.front();
			double max_cos = cosine_corners.back();
			// label the found rectangle
			if (vertices == 4 && min_cos >= -0.1 && max_cos <= 0.3) {
				cout << "Found a Rectangle" << endl;
				setLabel(dst, "RECT", contours[i]);
				setLabel(bw, "RECT", contours[i]);
			}
			else if (vertices == 4) {
				cout << "min_cos: " << min_cos << endl;
				cout << "max_cos: " << max_cos << endl;
				setLabel(dst, "RECT", contours[i]);
				setLabel(bw, "RECT", contours[i]);
			}
			else cout << "No Rect Found" << endl;
		}
		imshow("dst", dst);
		imshow("edge map", bw);
		if (check) waitKey(0);
	}

	static void drawTextCenter(Mat& img, const char* text) {
		/* Implement to draw centered text on image
		int font = FONT_HERSHEY_SIMPLEX;
		double scale = 0.4;
		int thickness = 1;
		int baseline = 0;
		Size text = getTextSize(text, font, scale, thickness, &baseline);
		Rect r = boundingRect(img);
		Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
		rectangle(im, pt + Point(0, baseline), pt + Point(text.width, -text.height), CV_RGB(255, 255, 255), FILLED);
		putText(im, label, pt, font, scale, CV_RGB(0, 0, 0), thickness, 8);
		*/
	}

	static void runRectDetection(bool check) {
		VideoCapture cap;
		Mat frame;
		Mat img = imread("Part_OpenCV.jpg");
		if (check) detectRect(img, NO_WEBCAM);
		else {
			if (!cap.open(0)) {
				cout << "Could not open or find the webam" << endl;
				exit(1);
			}
			while (1) {
				cap.read(frame);
				if (frame.empty()) break;
				detectRect(frame, USE_WEBCAM);
				if (waitKey(1) == ESC_KEY) break;
			}
		}
	}

	Mat applyThresh(Mat& img, int thresh) {
		//https://docs.opencv.org/3.4/db/d8e/tutorial_threshold.html
		Mat dst, gray;
		int thresh_type = 3; // threshold truncate
		cvtColor(img, gray, COLOR_BGR2GRAY);
		threshold(gray, dst, thresh, 255, thresh_type);
		return dst;
	}

	static void testThresh(bool check) {
		if (check) {
			// get a clearer image of the part 
			Mat img = imread("Part_3.png");
			// working on the best fine-tuned thresh params
			Mat img_thresh = applyThresh(img, 103);
			Mat dst;
			resize(img_thresh, dst, Size(300, 300));
			detectRect(dst, NO_WEBCAM);
		}
		else {
			Mat frame;
			VideoCapture cap;
			if (!cap.open(0)) {
				cout << "Could not open or find the webam" << endl;
				exit(1);
			}
			while (1) {
				cap.read(frame);
				if (frame.empty()) break;
				Mat thresh = applyThresh(frame, 103);
				detectRect(frame, USE_WEBCAM);
				if (waitKey(1) == ESC_KEY) break;
			}
		}
	}

	static void detectFace(Mat& img, CascadeClassifier& cascade, CascadeClassifier& nested_cascade) {
		vector<Rect> faces;
		Mat gray, small_img;
		cvtColor(img, gray, COLOR_BGR2GRAY);
		resize(gray, small_img, Size(), 1.0, 1.0, INTER_LINEAR);
		// face detection 
		cascade.detectMultiScale(small_img, faces, 1.3, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
		for (size_t i = 0; i < faces.size(); i++) {
			Rect r = faces[i];
			Mat img_roi;
			vector<Rect> eyes;
			Point center;
			Scalar color = Scalar(255, 0, 0);
			Scalar eye_color = Scalar(0, 255, 0);
			int radius;
			double aspect_ratio = (double)r.width / r.height;
			if (0.75 < aspect_ratio && aspect_ratio < 1.3) {
				center.x = cvRound((r.x + r.width * 0.5));
				center.y = cvRound((r.y + r.height * 0.5));
				radius = cvRound((r.width + r.height) * 0.25);
				circle(img, center, radius, color, 3, 8, 0);
			}
			img_roi = small_img(r);	// construct a Mat object with a detected Face(ROI)
			imshow("ROI FaceImage", img_roi);
			// eye detection 
			nested_cascade.detectMultiScale(img_roi, eyes, 1.3, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
			for (size_t j = 0; j < eyes.size(); j++) {
				Rect nr = eyes[j];
				center.x = cvRound((r.x + nr.x + nr.width * 0.5));
				center.y = cvRound((r.y + nr.y + nr.height * 0.5));
				radius = cvRound((nr.width + nr.height) * 0.25);
				circle(img, center, radius, eye_color, FILLED, 8, 0);
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

	static void testOpenCV() {
		setBreakOnError(true);
		testThresh(USE_WEBCAM);
		destroyAllWindows();
	}
}

#endif // !__UTILS__H__
