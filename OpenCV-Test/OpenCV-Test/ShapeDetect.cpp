/*
	Working with CutGaps!
	Test with other part images

	Figure out best WIRE_CONSTANT to use 
	Smooth canny() to get crisp edges

	Incorporate Serial and File i/o
		-> setup with a MCU to tell it where to go
		-> in the meantime, can try and write to the file that indexer uses at 

	Try other methods:
		- find contours from src
		- mask contours and inverse -> double checkk
		- something like that 

	Use with webcam (HiDef)!
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

ShapeDetect::ShapeDetect() {
	// add the default-val fields here
	// the c'tor will be called when using Cam
	src_img = Mat();	// make a default image
	out_stream.open(default_file_name);
	shape = ShapeDetect::ALL_SHAPES;
}

ShapeDetect::ShapeDetect(const char* img_name) {
	src_img = imread(img_name);
	if (src_img.empty()) exit(1);
	checkImgSize(src_img);
	out_stream.open(default_file_name);
	shape = ShapeDetect::ALL_SHAPES;
}

ShapeDetect::ShapeDetect(Mat img) {
	if (img.empty()) exit(1);
	src_img = img.clone();
	checkImgSize(src_img);
	out_stream.open(default_file_name);
	shape = ShapeDetect::ALL_SHAPES;
}

bool ShapeDetect::setImg(Mat img) {
	if (src_img.empty()) return false;
	src_img = img.clone();
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
	// Implement... at some point 
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
}

vector<Point> ShapeDetect::contoursConvexHull(vector<vector<Point>> contours) {
	vector<Point> result;
	vector<Point> pts;
	for (size_t i = 0; i < contours.size(); i++)
		for (size_t j = 0; j < contours[i].size(); j++)
			pts.push_back(contours[i][j]);
	if (pts.size() > 0)
		convexHull(pts, result);
	return result;
}

void ShapeDetect::detectGap() {
	detectGap(this->src_img);
	waitKey();
}

void ShapeDetect::detectGapWithCam() {
	// implement better webcam functionality
	// play with selecetROI()
	Mat frame;
	VideoCapture cap;
	if (!cap.open(0)) {
		cout << "Could not open or find the webam" << endl;
		exit(1);
	}
	while (1) {
		cap.read(frame);
		if (frame.empty()) break;
		detectGap(frame);
		waitKey(1);
	}
}

void ShapeDetect::detectGapTest() {
	/*
		7/16/21 WORKING!!!
			- succesfully identifiying gap
			- test with more cut gaps
	*/
	Mat images[] = {
		imread("Part_Gap_Just_Wires.bmp"),
		imread("Part_Gap_Wires_With_Small_Gap.bmp"),
		imread("Part_Gap_With_Big_Gap.bmp"),
		imread("Part_Gap_With_Big_Gap_And_Next_Group.bmp"),
		imread("Part_Gap_After_Gap_Found.bmp"),
		imread("Another_Gap.bmp")
	};
	for (int i = 0; i < 5; ++i) {
		detectGap(images[i]);
		if (i < 5) {
			waitKey(2000);
			destroyAllWindows();
		}
		else waitKey();
	}
}

/*
	Detects Z-Axis Cut Gaps
	Does not block after showing the image
	Caller will have to then call waitKey() for image to stay displayed 
*/
void ShapeDetect::detectGap(Mat& src) {
	Mat src_gray, src_canny, src_roi, src_thresh;
	vector<vector<Point>> contours;
	int wire_constant = 65;	
	// isolate ROI and find gap
	src_roi = src(Range(50, src.size[0]), Range(0, src.size[1]));
	cvtColor(src_roi, src_gray, COLOR_BGR2GRAY);
	threshold(src_gray, src_thresh, 62, 255, THRESH_TOZERO_INV);
	Canny(src_thresh, src_canny, 37, 111, 3, true);	// tweak canny + thresh to get crisper edges
	findContours(src_canny, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	//cout << "# of Contours " << contours.size() << endl;
	if (contours.size() < wire_constant) {
		analyzeFrame(src_roi, src_canny);
	}
	Mat gap_img;
	resize(src_roi, gap_img, Size(350, 650));
	imshow("Canny", src_canny);
	imshow("Cut Gap", gap_img);
}

void ShapeDetect::analyzeFrame(Mat& src_roi, Mat& src_canny) {
	// splits the image into 2 seperate ROI's
	// find contours of both ROI's and stores in 2 sets
	// run convex hull on both sets of contours
	// find distance between the convex hulls
	// draw both sets on OG img to examine
	// thats gap!
	Mat src_gray, src_thresh;
	Mat roi_left, roi_right;
	vector<vector<Point>> contours_set_one, contours_set_two;
	vector<Point> convex_hull_points_one, convex_hull_points_two;
	roi_left = src_canny(Range(0, src_canny.size[0]), Range(0, 47));
	roi_right = src_canny(Range(0, src_canny.size[0]), Range(47, src_canny.size[1]));
	findContours(roi_left, contours_set_one, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	findContours(roi_right, contours_set_two, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	convex_hull_points_one = contoursConvexHull(contours_set_one);
	convex_hull_points_two = contoursConvexHull(contours_set_two);
	translateContours(convex_hull_points_two, 47);
	Rect rect_left = boundingRect(convex_hull_points_one);
	Rect rect_right = boundingRect(convex_hull_points_two);
	rectangle(src_roi, rect_left, Scalar(0, 0, 255), 2);
	rectangle(src_roi, rect_right, Scalar(0, 0, 255), 2);
	int rect_left_pos = rect_left.x + rect_left.width;
	int rect_right_pos = rect_right.x;
	int center_pos = (rect_left_pos + rect_right_pos) / 2;
	cout << "The center pos is " << center_pos << endl;
	line(src_roi, Point(rect_right_pos, 40), Point(rect_left_pos, 40), Scalar(0, 0, 255), 1);
	line(src_roi, Point(center_pos, 0), Point(center_pos, roi_left.size[0]), Scalar(255, 0, 0), 1);
	cout << "Distance to left-edge " << center_pos - rect_left.x << endl;
	cout << "Distance to right-edge " << (rect_right.x + rect_right.width) - center_pos << endl;
	//imwrite("gap_found.bmp", src_roi);

}

void ShapeDetect::translateContours(vector<Point>& contour_points, int scale) {
	for (int i = 0; i < contour_points.size(); ++i)  
		contour_points[i].x += scale;
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
