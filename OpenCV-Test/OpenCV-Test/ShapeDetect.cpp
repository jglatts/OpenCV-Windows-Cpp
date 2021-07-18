/*

	Get it working with cut-gaps!
	 - need to find best param's for part photos
	 - good photos now in folder
	 - https://iq.opengenus.org/connected-component-labeling/
	 - ^^ look at connectedComponents()
	
	Should find the convexHull of the cut gap to close lines
		- minAreaRect() in the contours to find cutgap then convexHull()
		- getting good output with appyling thresh before
		- hook up to the detection and run conveHull() on the thresh'd image
		

	Use with webcam!

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

ShapeDetect::ShapeDetect(const char* img_name, bool cam_check) {
	src_img = imread(img_name);
	if (src_img.empty()) exit(1);
	checkImgSize(src_img);
	shape = ShapeDetect::ALL_SHAPES;
	use_webcam = (cam_check) ? true : false;
}

ShapeDetect::ShapeDetect(Mat img, bool cam_check) {
	src_img = img.clone();
	if (src_img.empty()) exit(1);
	checkImgSize(src_img);
	shape = ShapeDetect::ALL_SHAPES;
	use_webcam = (cam_check) ? true : false;
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
	// need sharper and higher quality images of parts
	Mat src_gray, src_blur, src_canny;
	vector<vector<Point>> contours;
	vector<Point> approx;
	double contour_length, contour_area;
	cvtColor(src_img, src_gray, COLOR_BGR2GRAY);
	blur(src_gray, src_blur, Size(2, 2));
	imshow("after blur", src_blur);
	waitKey(1000);
	Canny(src_blur, src_canny, 50, 110, 3, true);
	//Canny(src_gray, src_canny, 10, 20, 3, true);
	imshow("canny", src_canny);
	waitKey(1000);
	findContours(src_canny, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Mat drawing = Mat::zeros(src_canny.size(), CV_8UC3);
	cout << "contours.size() = " << contours.size() << endl;
	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);
		int vertices = approx.size();
		contour_area = contourArea(contours[i]);
		contour_length = arcLength(Mat(contours[i]), true);
		cout << "contours[i].size() = " << contours[i].size() << endl;
		cout << "vertices = " << vertices << endl;
		if (fabs(contour_area) >= 100) {
			setLabel(drawing, to_string(contour_area), contours[i]);
		}
		/*
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
		*/
		Scalar color = Scalar(255, 255, 255);
		drawContours(drawing, contours, i, color, 2);
	}
	vector<Point> convexHullPoints = contoursConvexHull(contours);
	vector<Point> contour_hull;
	// Approximating polygonal curve to convex hull
	approxPolyDP(Mat(convexHullPoints), contour_hull, 0.001, true);
	cout << "Area of convex hull = " << fabs(contourArea(Mat(contour_hull))) << endl;
	setLabel(src_img, to_string(fabs(contourArea(Mat(contour_hull)))), contour_hull);
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

void ShapeDetect::detectGap() {
	// look at detectGapTest() -> it's close!
	// Threshold values :
	// 		thresh_val: 62
	// 		thresh_type : 4 (Threshold to Zero)
	// Canny values :
	// 		min_thresh: 37
	// 		kernel_size : 3
	// 		ratio : 4 (min_thresh * ratio per canny alg)
	//
	// Wire Constants Checking
	//	1st check(all wires) -> 102 
	//  2nd check(some gap showing) -> 92 
	//  3rd check(big gap) -> 65 
	//  4th check(big gap and wires from next group) -> 50 
	//
	// use ROI to examine certain area of image
	// show series of imgs (frames from webcam)
	// get ROI in same place every time
	// prolly center of img
	// look for the contours there
	// can play around with OG img that way
	// and then move on to frames (webcam use)
	// ----------------ALG---------------------
	// ..... look at each frame and get contours .....
	// if wires < CONSTANT_FOR_WIRES
	//		found_gap = true;
	//		need to then isolate the contours (edges) and cut the part
	Mat src_gray, src_canny, src_roi, src_thresh;
	vector<vector<Point>> contours;
	vector<Point> approx;
	// see images for checks
	int wire_constant = 65;	
	// isolate ROI and find gap
	src_roi = src_img(Range(50, src_img.size[0]), Range(0, src_img.size[1]));
	cvtColor(src_roi, src_gray, COLOR_BGR2GRAY);
	threshold(src_gray, src_thresh, 62, 255, THRESH_TOZERO_INV);
	imshow("after thresh", src_thresh);
	waitKey(1);
	Canny(src_thresh, src_canny, 37, 111, 3, true);
	imshow("canny", src_canny);
	findContours(src_canny, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	cout << contours.size() << endl;
	if (contours.size() < wire_constant) {
		// found a gap
		// add some detection lines
		//setLabel(src_roi, "Found Cut Gao");
	}
	waitKey();
}

void ShapeDetect::detectGapTest() {
	/*
		7/16/21 WORKING!!!
			- succesfully identifiying gap
			- test with more cut gaps
	*/
	Mat images[5];
	images[0] = imread("Part_Gap_Just_Wires.bmp");
	images[1] = imread("Part_Gap_Wires_With_Small_Gap.bmp");
	images[2] = imread("Part_Gap_With_Big_Gap.bmp");
	images[3] = imread("Part_Gap_With_Big_Gap_And_Next_Group.bmp");
	images[4] = imread("Part_Gap_After_Gap_Found.bmp");
	for (int i = 0; i < 5; ++i) {
		detectGap(images[i]);
		(i < 3) ? destroyAllWindows() : (void) waitKey();
	}
}

void ShapeDetect::detectGap(Mat& src) {
	Mat src_gray, src_canny, src_roi, src_thresh;
	Mat roi_left, roi_right;
	vector<vector<Point>> contours;
	int wire_constant = 65;	
	// isolate ROI and find gap
	src_roi = src(Range(50, src.size[0]), Range(0, src.size[1]));
	cvtColor(src_roi, src_gray, COLOR_BGR2GRAY);
	threshold(src_gray, src_thresh, 62, 255, THRESH_TOZERO_INV);
	Canny(src_thresh, src_canny, 37, 111, 3, true);
	findContours(src_canny, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	cout << "# of Contours " << contours.size() << endl;
	if (contours.size() < wire_constant) {
		// correctly finding cut gap xD
		//
		// split the image into seperate ROI's
		// find contours stored in 2 sets
		// run convex hull on both sets of contours
		// find distance between the convex hulls
		// draw both sets on OG img to examine
		// thats gap!
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
		cout << "rect_left_pos " << rect_left_pos << endl;
		cout << "rect_right_pos " << rect_right_pos << endl;
		cout << "center_pos " << center_pos << endl;
		line(src_roi, Point(rect_right_pos, 40), Point(rect_left_pos, 40), Scalar(0, 0, 255), 2);
		line(src_roi, Point(center_pos, 0), Point(center_pos, src_img.size[0]), Scalar(255, 0, 0), 2);
	}
	imshow("Cut Cap", src_roi);
	waitKey(2000);
}

void ShapeDetect::translateContours(vector<Point>& contour_points, int scale) {
	for (int i = 0; i < contour_points.size(); ++i) {
		contour_points[i].x += scale;
	}
}

void ShapeDetect::findCutGap() {
	// Getting somehwere using minAreaRect()
	// https://docs.opencv.org/master/d3/dc0/group__imgproc__shape.html#ga014b28e56cb8854c0de4a211cb2be656
	Mat src_gray, src_blur, src_canny, src_thresh;
	vector<vector<Point>> contours;
	vector<Point> approx;
	// for use with boundingRect() -> experiment with this guy 
	//vector<vector<Point> > contours_poly;
	//vector<Rect> bound_rect;
	vector<RotatedRect> min_contour_rects;	// store the min_rects then draw on new Mat
	int low_thresh = 150;
	int ratio = 2;
	cvtColor(src_img, src_gray, COLOR_BGR2GRAY);
	blur(src_gray, src_blur, Size(5, 5));
	threshold(src_blur, src_thresh, 103, 255, THRESH_BINARY);
	imshow("after blur", src_blur);
	imshow("after thresh", src_thresh);
	waitKey(1000);
	Canny(src_thresh, src_canny, low_thresh, low_thresh * ratio, 3, true);
	imshow("canny", src_canny);
	waitKey(1000);
	findContours(src_canny, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	// loop through contours and find min_rect's
	cout << contours.size() << endl;
	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);
		min_contour_rects.push_back(minAreaRect(contours[i]));
		cout << "Number of vertices found in approxPolyDP() = " << approx.size() << endl;
		// tripping up on the oval
		if (approx.size() == 4 && fabs(contourArea(contours[i])) > 100) {
			//setLabel(src_img, "RECT... I think?", approx);
		}	
	}
	/*
		Prolly don't have to use the min_rect
		Use connectedComponents()! and check out 
	*/
	// loop through min_rects to find cut caps
	for (int i = 0; i < min_contour_rects.size(); ++i) {
		float rect_width = min_contour_rects[i].size.width;
		float rect_height = min_contour_rects[i].size.height;
		// run a convexHull on the detected rects
		//if (rect_width >= 1 && rect_height >= 1) {
			cout << "Min_rect = " << min_contour_rects[i].size << endl;
			// draw rectangle
			Point2f rect_points[4];
			Point2f rect_points_two[4];
			min_contour_rects[i].points(rect_points);
			for (int j = 0; j < 4; j++) {
				line(src_img, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 0));
			}
			// draw rectangle around contours to highlight gap!
	//	}
	}
	//find distance between contours
	// needs to be tweaked, but is drawing lines across contours
	// not sure if this needed - overkill
	/*
	for (int i = 0; i < min_contour_rects.size() - 1; ++i) {
		Point2f rect_points[4];
		Point2f rect_points_two[4];
		min_contour_rects[i].points(rect_points);
		min_contour_rects[i+1].points(rect_points_two);
		// get distance between points
		// order is bottomLeft, topLeft, topRight, bottomRight
		// distance between topRight lines
		float dist = rect_points_two[2].x - rect_points[2].x;
		cout << "Distance between contours = " << dist << endl;
		if (dist > 0 && dist < 150) {
			// draw line between contours 
			line(src_img, rect_points_two[2], rect_points[2], Scalar(0, 255, 0));
		}
	}
	*/
	Mat drawing = Mat::zeros(src_canny.size(), CV_8UC3);
	vector<Point> convexHullPoints = contoursConvexHull(contours);
	vector<Point> contour_hull;
	// Approximating polygonal curve to convex hull
	approxPolyDP(Mat(convexHullPoints), contour_hull, 0.001, true);
	cout << "Area of convex hull = " << fabs(contourArea(Mat(contour_hull))) << endl;
	setLabel(src_img, to_string(fabs(contourArea(Mat(contour_hull)))), contour_hull);
	polylines(drawing, convexHullPoints, true, Scalar(0, 0, 255), 2);
	imshow("Contours", drawing);
	polylines(src_img, convexHullPoints, true, Scalar(0, 0, 255), 2);
	imshow("contoursConvexHull", src_img);
	waitKey();
	imshow("src_img", src_img);
	waitKey();
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
