/*
	OpenCV program to verify install on Windows x64 machine
	Also testing some library functionality
	author: JDG
*/
#include "Utils.hpp"
#include "TrackBar.hpp"
#include "ShapeDetect.h"

int main(int argc, char** argv) {
	ShapeDetect sd("shapes.jpg");
	sd.runTest();
	//jdg::testOpenCV();
	//tb::testCannyTrackBar();
	return 0;
}
