#ifndef __SHAPEDETECT__H__
#define __SHAPEDETECT__H__

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#define IMG_SIZE_LIMIT 2000
#define USE_WEBCAM     true
#define NO_WEBCAM      false

class ShapeDetect {
public:
	typedef enum Shapes {
		CIRCLE,
		TRIANGLE,
		RECTANGLE,
		ALL_SHAPES
	};
	ShapeDetect(const char*);
	ShapeDetect(cv::Mat);
	cv::Mat getImg();
	const char* getImgName();
	bool setImg(cv::Mat);
	bool smoothCanny(cv::Mat&);
	void detectShape(Shapes);
	void runTest();

private:
	Shapes shape;
	cv::Mat src_img;
	std::vector<cv::Point> contoursConvexHull(std::vector<std::vector<cv::Point>>);
	void checkImgSize(cv::Mat&);
	void setLabel(cv::Mat&, std::string, std::vector<cv::Point>&);
	void runDetection();
	void detect(Shapes);
};

#endif // !__SHAPEDETECT__H__