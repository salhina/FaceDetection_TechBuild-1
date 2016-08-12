#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

// Function Headers
void detectAndDisplay(Mat frame);
void CallBackFunc(int event, int x, int y, int flags, void* userdata);

// Global variables
// Copy this file from opencv/data/haarscascades to target folder
string face_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
string window_name = "Capture - Face detection";


// Function main
int main(void)
{
	VideoCapture capture(0);

	if (!capture.isOpened())  // check if we succeeded
		return -1;

	// Load the cascade
	if (!face_cascade.load(face_cascade_name))
	{
		printf("--(!)Error loading\n");
		return (-1);
	};

	// Read the video stream
	Mat frame;
	cout << "okay";
	for (;;)
	{
		capture >> frame;

		// Apply the classifier to the frame
		if (!frame.empty())
			detectAndDisplay(frame);

		else
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}

		int c = waitKey(10);

		if (27 == char(c))
			break;
	}

	return 0;
}

// Function detectAndDisplay
void detectAndDisplay(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;
	Mat crop;
	Mat res;
	Mat gray;
	string text;
	stringstream sstm;
	Point cen_win(frame.size().width / 2, frame.size().height / 2);

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	// Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	// Set Region of Interest
	cv::Rect roi_b;
	cv::Rect roi_c;

	size_t ic = 0; // ic is index of current element
	int ac = 0; // ac is area of current element

	size_t ib = 0; // ib is index of biggest element
	int ab = 0; // ab is area of biggest element

	for (ic = 0; ic < faces.size(); ic++) // Iterate through all current elements (detected faces)
	{
		roi_c.x = faces[ic].x; // x coordinate of top left corner of the rectangle with index ic. 
		roi_c.y = faces[ic].y; // y coordinate of top left coordiante of the rectangle with index ic
		roi_c.width = (faces[ic].width); // width of rectangle 
		roi_c.height = (faces[ic].height); //height of rectangle

		ac = roi_c.width * roi_c.height; // Get the area of current element (detected face). Above code is redunadant.

		roi_b.x = faces[ib].x;
		roi_b.y = faces[ib].y;
		roi_b.width = (faces[ib].width);
		roi_b.height = (faces[ib].height);

		ab = roi_b.width * roi_b.height; // Get the area of biggest element, at beginning it is same as "current" element

		if (ac > ab)
		{
			ib = ic;
			roi_b.x = faces[ib].x;
			roi_b.y = faces[ib].y;
			roi_b.width = (faces[ib].width);
			roi_b.height = (faces[ib].height);
		}

		crop = frame(roi_b); //roi_b is an object of type Rect. crop is an image container. passing roi_b as an argument of frame modifies frame to creates an image container with the dimensions of the rectangle.  

		resize(crop, res, Size(128, 128), 0, 0, INTER_LINEAR); // This will be needed later while saving images. res is also of type Mat. focus is on inputarray.
		cvtColor(crop, gray, CV_BGR2GRAY); // Convert cropped image to Grayscale

		Point pt1(faces[ib].x, faces[ib].y);
		Point pt2((faces[ib].x + faces[ib].width), (faces[ib].y + faces[ib].height));
		Point cen_rec(roi_b.x + roi_b.width / 2, roi_b.y + roi_b.height / 2);

		rectangle(frame, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0); //How is rectangle formed? Unsure whether coords are of left corner. 
		circle(frame, cen_rec, 1, Scalar(0, 0, 255), CV_FILLED);
	}

	cout << "Center of rectangle = ( " << roi_b.x + roi_b.width / 2 << ", " << roi_b.y + roi_b.height / 2 << ")\n";
	
	// Show image
	sstm << "Crop area size: " << roi_b.width << "x" << roi_b.height << " Size of window: " << frame.size().width << '*' << frame.size().height;
	text = sstm.str();


	putText(frame, text, cvPoint(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
	circle(frame, cen_win, 1, Scalar(0, 0, 255), CV_FILLED);

	namedWindow("original", WINDOW_NORMAL);
	setMouseCallback("original", CallBackFunc, NULL);
	imshow("original", frame);


	if (!crop.empty())
	{
		imshow("detected", crop);
	}
	else
		destroyWindow("detected");
}

void CallBackFunc(int event, int x, int y, int flags, void * userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_RBUTTONDOWN)
	{
		cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MBUTTONDOWN)
	{
		cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	/*else if (event == EVENT_MOUSEMOVE)
	{
	cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;

	} */
}