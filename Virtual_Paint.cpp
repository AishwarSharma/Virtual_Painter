#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/objdetect.hpp>
#include<iostream>

using namespace std;
using namespace cv;

//////////////////////////////////// Virtual Paint //////////////////////////////////////



//RED:156,187,71,179,255,195
// We tuned all the values using color detection.// We have all the values of color in this vector.
// We also need to define what colors do we will display when these are detected.

Mat img;

vector<vector<int>> myColors{ {156,187,71,179,255,195}, //Red  
	{133,73,226,171,196,255},  //pink
	{1,82,0,25,255,134} }; // orange 


vector<Scalar> myColorValues{ {0,0,255}, // Red
							  {203,192,255}, //pink
							  {0,69,255} }; // orange 

vector<vector<int>> newPoints;  // {	{x,y,0}}; // 0 means red ... and so on .

Point getContour(Mat imgDil)
{
	vector<vector<Point>> contours; // 2D vector of Points, where Points is a type, probably a class
	vector<Vec4i> hierarchy; //A Vec4i is a type holding 4 integers.

	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());
	Point myPoint(0, 0);

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);   // Calculates area of contours
		//cout << area << endl;

		if (area > 1000)
		{
			// Finding the bounding boxes around objects.

			float peri = arcLength(contours[i], true);  // true means object is closed
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);   // Finding the number of curvers(corners) the polygon has.

			//cout << conPoly[i].size() << endl;   // return number of edges/vertices.

			boundRect[i] = boundingRect(conPoly[i]);   // This will find a rectangle around each object.
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;  // Centre point.(tip)
			myPoint.y = boundRect[i].y;

			// we use bounding box to get the tip value.
			
			drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);  
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
	
			
		}
	}
	return myPoint;
}

vector<vector<int>> findColor(Mat img)
{
	Mat imgHSV,mask;

	cvtColor(img, imgHSV, COLOR_BGR2HSV);
	for (int i = 0; i < myColors.size(); i++)
	{
		Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
		Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);
		inRange(imgHSV, lower, upper, mask);
		//imshow(to_string(i), mask);
		Point myPoint = getContour(mask);
		if (myPoint.x != 0 && myPoint.y != 0) 
		{
			newPoints.push_back({ myPoint.x,myPoint.y,i });  // i is color value.
		}
	}
	return newPoints;
}

void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues)
{
	for (int i = 0; i < newPoints.size(); i++)
	{
		circle(img, Point(newPoints[i][0],newPoints[i][1]), 10, myColorValues[newPoints[i][2]],FILLED);
	}
}

void main()	
{
	VideoCapture cap(0);

	while (true)
	{
		cap.read(img);
		newPoints = findColor(img);
		drawOnCanvas(newPoints,myColorValues);

		imshow("Image", img);
		waitKey(1);
	}
}

/*
Virtual Paint:

Concept:

we will first detect the colours using the HSV space and once we have the c
olour image we are going to pass it to the contour method , to find the contours.
Once we find the contours we going to take x,y position of those and then we will 
create a circle at that point
*/