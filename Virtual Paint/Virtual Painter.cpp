#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

/// <summary>
/// /////////////////////// Virtual Painter //////////////////////////////
/// </summary>

Mat img;
VideoCapture cap(0);
vector<vector<int>> newPoints; //to storer all points

// Color Values
//hmin, smin, vmin, hmax, smax, vmax
vector<vector<int>> myColors{ {57, 102, 123, 95, 199, 156},//Green
							  {0, 179, 156, 10, 255, 186},//Orange
							  {111, 138, 43, 124, 255, 255},//Blue
							  {75, 0, 66, 106, 154, 186}//eraser
};

vector<Scalar> myColorvalues{ {0,255,0},//Green
								{255,99,71},//Orange
								{255,0,0},//Blue
};

Point getContours(Mat imgDil) {

	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;

	findContours(imgDil, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	Point myPoint(0, 0);

	for (int i = 0; i < contours.size(); i++) {

		int area = contourArea(contours[i]);
		cout << area << endl;
		

		if (area > 100) {
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.03 * peri, true);

			cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]);
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;
			myPoint.y = boundRect[i].y;

			drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
		}
		return myPoint;
	}
}

void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues) {
	for (int i = 0; i < newPoints.size(); i++) {
		circle(img, Point(newPoints[i][0], newPoints[i][1]), 10, myColorValues[newPoints[i][2]], FILLED);
	}
}

void findColor(Mat img) {
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	for (int i = 0; i < myColors.size(); i++) {

		Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);

		Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);

		Mat mask;

		inRange(imgHSV, lower, upper, mask);

		//imshow(to_string(i), mask);
		Point myPoint = getContours(mask);
		if (myPoint.x != 0) {
			newPoints.push_back({ myPoint.x, myPoint.y, i });
		}
	}
}

void main() {

	while (true) {
		cap.read(img);

		findColor(img);

		drawOnCanvas(newPoints, myColorvalues);

		imshow("Image", img);
		waitKey(1);
	}
}