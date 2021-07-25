#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <deque>
using namespace cv;
using namespace std;

/// <summary>
/// /////////////////////// Virtual Painter //////////////////////////////
/// </summary>

int hmin = 0, smin = 0, vmin = 0;
int hmax = 179, smax = 255, vmax = 255;
vector<vector<int>> newPoints; //to storer all points
Mat imgGray, imgBlur, imgCanny, imgDil;

// Color Values
//hmin, smin, vmin, hmax, smax, vmax
/*vector<vector<int>> myColors{{57, 102, 123, 95, 199, 156},//Green
							  {0, 179, 156, 10, 255, 186},//Orange
							  {111, 138, 43, 124, 255, 255},//Blue
							  {75, 0, 66, 106, 154, 186}//eraser
};*/

vector<Scalar> myColorvalues{	{255,0,0},//Blue
								{0,255,0},//Green
								{0,0,255},//Red
								{0,255,255},//Yellow  
							};
int colorIndex = 0;


int getMaxAreaContourId(vector <vector<cv::Point>> contours) {
	double maxArea = 0;
	int maxAreaContourId = -1;
	for (int j = 0; j < contours.size(); j++) {
		double newArea = cv::contourArea(contours.at(j));
		if (newArea > maxArea) {
			maxArea = newArea;
			maxAreaContourId = j;
		} // End if
	} // End for
	return maxAreaContourId;
} // End function

void main() {

	//Creating the trackbars needed for adjusting the marker color 
	namedWindow("Color detectors", (700, 200));
	createTrackbar("Upper Hue", "Color detectors", &hmin, 179);
	createTrackbar("Upper Saturation", "Color detectors", &hmax, 179);
	createTrackbar("Upper Value", "Color detectors", &smin, 179);
	createTrackbar("Lower Hue", "Color detectors", &smax, 255);
	createTrackbar("Lower Saturation", "Color detectors", &vmin, 179);
	createTrackbar("Lower Value", "Color detectors", &hmax, 255);

	//Giving different arrays to handle color points of different colour
	vector<Point> bpoints;
	vector<Point> gpoints;
	vector<Point> rpoints;
	vector<Point> ypoints;
	
	//These indexes will be used to mark the points in particular arrays of specific colour
	auto blue_index = 0;
	int	green_index = 0;
	int	red_index = 0;
	int	yellow_index = 0;
	
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));

	//Here is code for Canvas setup
	Mat paintWindow(300, 500, CV_8UC3, Scalar(471, 636, 3));// +255;

	//rectangle(paintWindow, Point(40,1), Point(140,65), Scalar(0, 255, 0), 5);
	rectangle(paintWindow, Point(40, 1), Point(140, 65), (0, 0, 0), 2);
	rectangle(paintWindow, Point(160, 1), Point(255, 65), myColorvalues[0], -1);
	rectangle(paintWindow, Point(275, 1), Point(370, 65), myColorvalues[1], -1);
	rectangle(paintWindow, Point(390, 1), Point(485, 65), myColorvalues[2], -1);
	rectangle(paintWindow, Point(505, 1), Point(600, 65), myColorvalues[3], -1);
	
	//putText(img, objectType, { boundRect[i].x,boundRect[i].y }, FONT_HERSHEY_PLAIN, 1, Scalar(0, 69, 255), 1);
	putText(paintWindow, "CLEAR", Point(49, 33), FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 2, LINE_AA);
	putText(paintWindow, "BLUE", Point(185, 33), FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2, LINE_AA);
	putText(paintWindow, "GREEN", Point(298, 33), FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2, LINE_AA);
	putText(paintWindow, "RED", Point(420, 33), FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2, LINE_AA);
	putText(paintWindow, "YELLOW", Point(520, 33), FONT_HERSHEY_SIMPLEX, 0.5, (150, 150, 150), 2, LINE_AA);
	namedWindow("Paint", WINDOW_AUTOSIZE);

	//Loading the default webcam of PC.
	VideoCapture cap(0);

	Mat frame, ret;
	while (true) {
		Mat hsv, Mask;
		// Reading the frame from the camera
		cap.read(frame);
		cap.read(ret);

		/*//The kernel to be used for dilation purpose 

		cvtColor(img, imgGray, COLOR_BGR2GRAY);
		GaussianBlur(img, imgBlur, Size(7, 7), 7, 3);
		Canny(imgBlur, imgCanny, 15, 17);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
		dilate(imgCanny, imgDil, kernel);*/

		// Flipping the frame to see same side of yours
		flip(frame, frame, 1);
		//cvtColor(img, imgHSV, COLOR_BGR2HSV);
		cvtColor(frame, hsv, COLOR_BGR2HSV, 0);
		int u_hue = getTrackbarPos("Upper Hue", "Color detectors");
		int u_saturation = getTrackbarPos("Upper Saturation", "Color detectors");
		int u_value = getTrackbarPos("Upper Value", "Color detectors");
		int l_hue = getTrackbarPos("Lower Hue", "Color detectors");
		int l_saturation = getTrackbarPos("Lower Saturation", "Color detectors");
		int l_value = getTrackbarPos("Lower Value", "Color detectors");
		Scalar lower(l_hue, l_saturation, l_value);
		Scalar upper(u_hue, u_saturation, u_value);


		//inRange(imgHSV, lower, upper, Mask);

		// Adding the colour buttons to the live frame for colour access
		rectangle(frame, Point(40, 1), Point(140, 65), (122, 122, 122), -1);
		rectangle(frame, Point(160, 1), Point(255, 65), myColorvalues[0], -1);
		rectangle(frame, Point(275, 1), Point(370, 65), myColorvalues[1], -1);
		rectangle(frame, Point(390, 1), Point(485, 65), myColorvalues[2], -1);
		rectangle(frame, Point(505, 1), Point(600, 65), myColorvalues[3], -1);

		putText(frame, "CLEAR ALL", Point(49, 33), FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2, LINE_AA);
		putText(frame, "BLUE", Point(185, 33), FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2, LINE_AA);
		putText(frame, "GREEN", Point(298, 33), FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2, LINE_AA);
		putText(frame, "RED", Point(420, 33), FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2, LINE_AA);
		putText(frame, "YELLOW", Point(520, 33), FONT_HERSHEY_SIMPLEX, 0.5, (150, 150, 150), 2, LINE_AA);


		// Identifying the pointer by making its mask
		inRange(hsv, lower, upper, Mask);
		//---------------------------------------------------
		erode(Mask, Mask, kernel); //,Point(), 1);

		morphologyEx(Mask, Mask, MORPH_OPEN, kernel);

		dilate(Mask, Mask, kernel);//, Point(), 1);

		// Find contours for the pointer after idetifying it
		// 
		//findContours(imgDil, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		vector<vector<Point>> contours;
		vector<Vec4i> hierachy;
		vector<Point> center;

		findContours(Mask, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		// If the contours are formed

		if (contours.size() > 0) {

			//sorting the contours to find biggest
			vector<Point> biggest{ contours.at(getMaxAreaContourId(contours)) };
			
			//Get the radius of the enclosing circle around the found contour
			Point2f center;
			float radius = 0;
			minEnclosingCircle(biggest, center, radius);

			// Draw the circle around the contour
			circle(frame, center, cvRound(radius), Scalar(0, 255, 255), 2, LINE_AA);
			
			// Calculating the center of the detected contour
			Moments M = moments(biggest);

			//center = (int(M['m10'] / M['m00']), int(M['m01'] / M['m00']));

			center = Point2f(static_cast<float>(M.m10 / (M.m00 + 1e-5)),
				static_cast<float>(M.m01 / (M.m00 + 1e-5)));


			// Now checking if the user wants to click on any button above the screen 
			if (center.y <= 65) {
				if (40 <= center.x <= 140) {
					// Clear Button
					bpoints.clear();
					gpoints.clear();
					rpoints.clear();
					ypoints.clear();

					blue_index = 0;
					green_index = 0;
					red_index = 0;
					yellow_index = 0;

					paintWindow = (300, 500, CV_8UC3, Scalar(471, 636, 3));
				}
				else if (160 <= center.x <= 255) {
					colorIndex = 0; //Blue
				}
				else if (275 <= center.x <= 370) {
					colorIndex = 1;//Green
				}
				else if (390 <= center.x <= 485) {
					colorIndex = 2; //Red
				}
				else if (505 <= center.x <= 600) {
					colorIndex = 3; //Yellow
				}
			} else {
				//vector<int>  parth = {10, 20, 30};
				//auto np = 0; 
				if (colorIndex == 0) {
					bpoints.insert(bpoints.begin() + blue_index, center);
				}
				else if (colorIndex == 1) {
					gpoints.insert(gpoints.begin() + green_index, center);
					//gpoints[green_index].appendleft(center);
				}
				else if (colorIndex == 2) {
					rpoints.insert(rpoints.begin() + red_index, center);
					//rpoints[red_index].appendleft(center);
				}
				else if (colorIndex == 3) {
					ypoints.insert(ypoints.begin() + yellow_index, center);
					//ypoints[yellow_index].appendleft(center);
				}
			}

		}
		else {//-------------------------------------------------------
			// Append the next deques when nothing is detected to avois messing up
			//bpoints.push_back();
			blue_index += 1;
			//gpoints.append(deque(maxlen = 512));
			green_index += 1;
			//rpoints.append(deque(maxlen = 512));
			red_index += 1;
			//ypoints.append(deque(maxlen = 512));
			yellow_index += 1;
		}

		//-------------------------------------------------------------
		// # Draw lines of all the colors on the canvas and frame 
		//	vector<Point> bpoints;
		// {
		//	{
		//		(),(),()
		//	},
		//	{
		//		(),(),()
		//	}
		//}
		vector<int> parth;
		vector<int> parth2;
		vector<int> parth3;
		parth[0] = 1;
		vector<vector<Point>> points = {bpoints, gpoints, rpoints, ypoints};

		for (int i = 0; i < points.size(); i++) {
			for (int j = 0; j < points[i].size() ; j++) {
				//cout << points[i][j] << endl;
				for (int k = 1; k < points[i][j].x ; k++) {
				//	if ((points[i][j])  (points[i][j]) ) {
				//		continue;
				//	}
					line(frame, points[i][j], points[i][j], myColorvalues[i], 2);
					line(paintWindow, points[i][j], points[i][j], myColorvalues[i], 2);
				}
			}
		}
		// Show all the windows
		imshow("Tracking", frame);
		imshow("Paint", paintWindow);
		//imshow("mask", Mask);

		//If the 'q' key is pressed then stop the application
		/*if (waitKey(1) & 0xFF == ord('q')) {
			break;
		}*/

		/*findColor(img);
		drawOnCanvas(newPoints, myColorvalues);
		*/
		imshow("frame",frame);
		imshow("hsv", hsv);
		waitKey(10);
	}
	cap.release();
}