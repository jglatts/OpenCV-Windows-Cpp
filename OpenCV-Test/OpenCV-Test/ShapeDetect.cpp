/*
	Use canny to find contours
	Then fill a new Mat with drawContours();
*/
#include "ShapeDetect.h"
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

ShapeDetect::ShapeDetect(const char* img_name) {
	src_img = imread(img_name);
	shape = ShapeDetect::ALL_SHAPES;
}

ShapeDetect::ShapeDetect(Mat img) {
	src_img = img.clone();
	shape = ShapeDetect::ALL_SHAPES;
}

bool ShapeDetect::setImg(Mat img) {
	src_img = img.clone();
	return src_img.empty();
}

void ShapeDetect::detectShape(Shapes s) {
	detect(s);
}

void ShapeDetect::detect(Shapes s) {
	switch (s) {
	case ShapeDetect::CIRCLE:
		shape = ShapeDetect::CIRCLE;
		break;
	case ShapeDetect::TRIANGLE:
		shape = ShapeDetect::TRIANGLE;
		break;
	case ShapeDetect::RECTANGLE:
		shape = ShapeDetect::RECTANGLE;
		break;
	case ShapeDetect::ALL_SHAPES:
	default:
		break;
	}
	runDetection();
}

void ShapeDetect::runDetection() {
	Mat src_gray, src_blur, src_canny;
	cvtColor(src_img, src_gray, COLOR_BGR2GRAY);
	blur(src_gray, src_blur, Size(3, 3));
	Canny(src_blur, src_canny, 0, 100, 3, true);
	vector<vector<Point>> contours;
	findContours(src_canny, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Mat drawing = Mat::zeros(src_canny.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++) {
		Scalar color = Scalar(255, 255, 255);
		drawContours(drawing, contours, i, color, 2);
	}
	vector<Point> ConvexHullPoints = contoursConvexHull(contours);
	polylines(drawing, ConvexHullPoints, true, Scalar(0, 0, 255), 2);
	imshow("Contours", drawing);
	polylines(src_img, ConvexHullPoints, true, Scalar(0, 0, 255), 2);
	imshow("contoursConvexHull", src_img);
	waitKey();
}

vector<Point> ShapeDetect::contoursConvexHull(vector<vector<Point>> contours) {
	vector<Point> result;
	vector<Point> pts;
	for (size_t i = 0; i < contours.size(); i++)
		for (size_t j = 0; j < contours[i].size(); j++)
			pts.push_back(contours[i][j]);
	convexHull(pts, result);
	return result;
}

void ShapeDetect::runTest() {
	// make better test ()
	// run test with given shape in instance 
	runDetection();
}