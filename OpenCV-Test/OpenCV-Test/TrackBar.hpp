#ifndef __TRACKBAR__H__
#define __TRACKBAR__H__

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

namespace tb {

	using namespace cv;
	using namespace std;
	
	Mat src, src_gray;
	Mat dst, detected_edges;

	int lowThreshold = 0;
	const int max_lowThreshold = 100;
	const int ratio = 3;
	const int kernel_size = 3;
	const char* window_name = "Canny Test";

	static void cannyThreshold(int, void*)
	{
		blur(src_gray, detected_edges, Size(3, 3));
		Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);
		dst = Scalar::all(0);
		src.copyTo(dst, detected_edges);
		imshow(window_name, dst);
	}

	static void testCannyTrackBar() {
		src = imread("Part_OpenCV.jpg");
		dst.create(src.size(), src.type());
		cvtColor(src, src_gray, COLOR_BGR2GRAY);
		namedWindow(window_name, WINDOW_AUTOSIZE);
		createTrackbar("Min Thresh:", window_name, &lowThreshold, max_lowThreshold, cannyThreshold);
		cannyThreshold(0, 0);
		waitKey(0);
	}

}

#endif // !__TRACKBAR__H__
