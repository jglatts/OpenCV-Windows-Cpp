/*
	OpenCV program to verify install on Windows x64 machine
	Also testing some library functionality
	author: JDG
*/
#include "Utils.hpp"
#include "TrackBar.hpp"
#include "ShapeDetect.h"

int main(int argc, char** argv) {
	ShapeDetect sd("Part_Gap_With_Big_Gap_And_Next_Group.bmp", false);
	sd.detectGap();
	//jdg::testOpenCV();
	//tb::testCannyTrackBar();
	return 0;
}
