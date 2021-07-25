#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void main() {
	string s = "Resources/test.jpg";
	Mat img = imread(s);
	imshow("Image", img);
	waitKey(0);

}