#ifndef __SHAPEDETECT__H__
#define __SHAPEDETECT__H__

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <stdio.h>
#include <fstream>
#include <iostream>


class ShapeDetect {

#define IMG_SIZE_LIMIT 2000
#define USE_WEBCAM true
#define NO_WEBCAM  false

public:
	typedef enum Shapes {
		CIRCLE,
		TRIANGLE,
		RECTANGLE,
		ALL_SHAPES
	};
	ShapeDetect();
	ShapeDetect(const char*);
	ShapeDetect(cv::Mat);
	bool setImg(cv::Mat);
	void detectShape(Shapes);
	void detectGap();
	void detectGapWithCam();
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
	void analyzeFrame(cv::Mat&, cv::Mat&);
	std::ofstream out_stream; 
	FILE* outfile;            
	const char* default_file_name = "motor.txt";
	// put WIRE_CONSTANT val here 
	// so can not be mut'd by user 
};

#endif // !__SHAPEDETECT__H__
