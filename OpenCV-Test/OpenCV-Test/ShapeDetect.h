#ifndef __SHAPEDETECT__H__
#define __SHAPEDETECT__H__

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#define IMG_SIZE_LIMIT 2000

class ShapeDetect {
public:
	typedef enum Shapes {
		CIRCLE,
		TRIANGLE,
		RECTANGLE,
		ALL_SHAPES
	};
	ShapeDetect(const char*, bool);
	ShapeDetect(cv::Mat, bool);
	bool setImg(cv::Mat);
	void detectShape(Shapes);
	void detectGap();
	void detectGapTest();

private:
	Shapes shape;
	cv::Mat src_img;
	std::vector<cv::Point> contoursConvexHull(std::vector<std::vector<cv::Point>>);
	void checkImgSize(cv::Mat&);
	void setLabel(cv::Mat&, std::string, std::vector<cv::Point>&);
	void translateContours(std::vector<cv::Point>&, int);
	void detect(Shapes);
	void detectGap(cv::Mat&);
	bool use_webcam;
};

#endif // !__SHAPEDETECT__H__
