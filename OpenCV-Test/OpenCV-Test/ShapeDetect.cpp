/*
	Use canny to find contours
	Then fill a new Mat with drawContours();

	Get it working with cut-gaps!
	 - need to find best param's for part photos

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
	if (src_img.empty()) exit(1);
	checkImgSize(src_img);
	shape = ShapeDetect::ALL_SHAPES;
}

ShapeDetect::ShapeDetect(Mat img) {
	src_img = img.clone();
	if (src_img.empty()) exit(1);
	checkImgSize(src_img);
	shape = ShapeDetect::ALL_SHAPES;
}

bool ShapeDetect::setImg(Mat img) {
	src_img = img.clone();
	if (src_img.empty()) return false;
	checkImgSize(src_img);
	return true;
}

void ShapeDetect::checkImgSize(Mat& img) {
	if (img.cols >= IMG_SIZE_LIMIT && img.rows >= IMG_SIZE_LIMIT) {
		resize(img, img, Size(700, 700));
	}
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
	// use approxPolyDp() and find 
	Mat src_gray, src_blur, src_canny;
	String str_area;
	vector<vector<Point>> contours;
	vector<Point> approx;
	cvtColor(src_img, src_gray, COLOR_BGR2GRAY);
	//blur(src_gray, src_blur, Size(1, 1));
	//Canny(src_blur, src_canny, 0, 100, 3, true);
	Canny(src_gray, src_canny, 0, 20, 3, true);
	imshow("canny", src_canny);
	waitKey(2000);
	findContours(src_canny, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Mat drawing = Mat::zeros(src_canny.size(), CV_8UC3);
	cout << "contours.size() = " << contours.size() << endl;
	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);
		int vertices = approx.size();
		cout << "contours[i].size() = " << contours[i].size() << endl;
		cout << "vertices = " << vertices << endl;
		cout << "contourArea() = " << contourArea(contours[i]) << "\n\n" << endl;
		str_area = to_string(contourArea(contours[i]));
		setLabel(drawing, str_area, contours[i]);
		if (vertices == 4) {
			cout << "Found a rect... I think??" << endl;
			// tweak shape detection then setLabel()
			//setLabel(drawing, "rect", contours[i]);
			//setLabel(src_img, "rect", contours[i]);
		}
		else if (vertices > 4 && shape == RECTANGLE) {
			// remove unwanted contours
			contours[i].clear();
			continue;	
		}
		Scalar color = Scalar(255, 255, 255);
		drawContours(drawing, contours, i, color, 2);
	}
	vector<Point> convexHullPoints = contoursConvexHull(contours);
	vector<Point> contour_hull;
	// Approximating polygonal curve to convex hull
	approxPolyDP(Mat(convexHullPoints), contour_hull, 0.001, true);
	cout << "Area of convex hull = " << fabs(contourArea(Mat(contour_hull))) << endl;
	str_area = to_string(fabs(contourArea(Mat(contour_hull))));
	setLabel(src_img, str_area, contour_hull);
	polylines(drawing, convexHullPoints, true, Scalar(0, 0, 255), 2);
	imshow("Contours", drawing);
	polylines(src_img, convexHullPoints, true, Scalar(0, 0, 255), 2);
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

bool ShapeDetect::smoothCanny(Mat& src) {
	// smooth canny to filter unwanted edges
	// should do this in the actualy canny() tho
	return true;
}

void ShapeDetect::setLabel(Mat& im, string label, vector<Point>& contour) {
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

void ShapeDetect::runTest() {
	runDetection();
}