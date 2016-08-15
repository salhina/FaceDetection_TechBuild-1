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
	Mat frame_gray, crop, res, gray;
	string text;
	stringstream sstm;
	
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	// Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	Point cen_win(frame.size().width / 2, frame.size().height / 2);
	circle(frame, cen_win, 1, Scalar(0, 0, 255), CV_FILLED);
	
	size_t ic = 0; // ic is index of current element
	unsigned long ac = 0; // ac is area of current element

	size_t ib = 0; // ib is index of biggest element
	unsigned long ab = 0; // ab is area of biggest element
	
	for (ic = 0; ic < faces.size(); ic++) // Iterate through all current elements (detected faces)
	{
		ac = faces[ic].area();
		ab = faces[ib].area();

		if (ac > ab)
			ib = ic; // no need for ic = 0 condition as ib is zero by default
	}
	
	if (faces.size())
	{
		crop = frame(faces[ib]);

		//resize(crop, res, Size(128, 128), 0, 0, INTER_LINEAR); // This will be needed later while saving images. res is also of type Mat. focus is on inputarray.
		//cvtColor(crop, gray, CV_BGR2GRAY); // Convert cropped image to Grayscale

		Point pt1(faces[ib].x, faces[ib].y);
		Point pt2((faces[ib].x + faces[ib].width), (faces[ib].y + faces[ib].height));
		rectangle(frame, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0); //Coords pt1 are of left corner. 

		double buffer_width, buffer_height;
		buffer_width = 0.1*faces[ib].width;
		buffer_height = 0.1*faces[ib].height;

		Point Bpt1(pt1.x + ((faces[ib].width - buffer_width) / 2), pt1.y + ((faces[ib].height - buffer_height) / 2));
		Point Bpt2(pt1.x + ((faces[ib].width + buffer_width) / 2), pt1.y + ((faces[ib].height + buffer_height) / 2));
		rectangle(frame, Bpt2, Bpt1, Scalar(255, 0, 0), 2, 8, 0); //Enclsoing the buffer zone. Defined to deal with slight variations in center of rectangle.

		Point cen_rec(faces[ib].x + faces[ib].width / 2, faces[ib].y + faces[ib].height / 2);
		circle(frame, cen_rec, 1, Scalar(0, 0, 255), CV_FILLED); //Marks the center of detected face.

		arrowedLine(frame, cen_win, Point(cen_win.x, cen_rec.y), Scalar(255, 0, 255), 1, 8, 0, 0.1); //Helps visualise the extent of camera tilt required to center face 
		arrowedLine(frame, cen_win, Point(cen_rec.x, cen_win.y), Scalar(255, 0, 255), 1, 8, 0, 0.1); //Helps viualise the extent of panning required to center face.  


	   //if (faces[ib].width > 0 && faces[ib].width < 40) cout << "Crop area: " << faces[ib].width << " * " << faces[ib].height;
	   //We should probably exclude faces with width less than 40 pixels as face detection with them gives inconsistent results 

	   // Show image
		sstm << "Crop area size: " << faces[ib].width << "x" << faces[ib].height << " Size of window: " << frame.size().width << '*' << frame.size().height;
		text = sstm.str();
		putText(frame, text, cvPoint(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
		sstm.clear();
		sstm.str("");
		
		double percentage;
		//percentage = ab / (frame.size().width * frame.size().height) * 100; doesn't work
		percentage = ((double)ab / (640 * 480)) * 100; 
		sstm << "Percentage: " << percentage;
		text = sstm.str();
		putText(frame, text, cvPoint(30, 50), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
		sstm.clear();
		sstm.str("");
	}
	
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