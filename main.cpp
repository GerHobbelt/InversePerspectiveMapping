/*
 * Project:  Inverse Perspective Mapping
 * Date:	 22/02/2014
 *
 * File:     main.cpp
 *
 * Contents: Creation, initialisation and usage of IPM object
 *           for the generation of Inverse Perspective Mappings of images or videos
 *
 * Author:   Marcos Nieto
 *			 marcos dot nieto dot doncel at gmail dot com
 *
 * Homepage: http://marcosnietoblog.wordpress.com
 */

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <ctime>

#include "IPM.h"

using namespace cv;
using namespace std;

int main( int _argc, char** _argv )
{
	// Images
	Mat inputImg, inputImgGray;
	Mat outputImg;
	
	if( _argc != 2 )
	{
		cout << "Usage: ipm.exe <videofile>" << endl;
		return 1;
	}

	// Video
	string videoFileName = _argv[1];	
	cv::VideoCapture video;
	if( !video.open(videoFileName) )
		return 1;

	// Show video information
	int width = 0, height = 0, fps = 0, fourcc = 0;	
	width = static_cast<int>(video.get(CV_CAP_PROP_FRAME_WIDTH));
	height = static_cast<int>(video.get(CV_CAP_PROP_FRAME_HEIGHT));
	fps = static_cast<int>(video.get(CV_CAP_PROP_FPS));
	fourcc = static_cast<int>(video.get(CV_CAP_PROP_FOURCC));

	cout << "Input video: (" << width << "x" << height << ") at " << fps << ", fourcc = " << fourcc << endl;
	
	// The 4-points at the input image	
	vector<Point2f> origPoints;
	origPoints.push_back( Point2f(0.0f, static_cast<float>(height)) );
	origPoints.push_back( Point2f(static_cast<float>(width), static_cast<float>(height)) );
	origPoints.push_back( Point2f(static_cast<float>(width)/2+30, 140.0f) );
	origPoints.push_back( Point2f(static_cast<float>(width)/2-50, 140.0f) );

	// The 4-points correspondences in the destination image
	vector<Point2f> dstPoints;
	dstPoints.push_back( Point2f(0.0f, static_cast<float>(height)) );
	dstPoints.push_back( Point2f(static_cast<float>(width), static_cast<float>(height)) );
	dstPoints.push_back( Point2f(static_cast<float>(width), 0.0f) );
	dstPoints.push_back( Point2f(0.0f, 0.0f) );
	
	// IPM object
	IPM ipm( Size(width, height), Size(width, height), origPoints, dstPoints );
	
	// Main loop
	int frameNum = 0;
	for( ; ; )
	{
		printf("FRAME #%6d ", frameNum);
		fflush(stdout);
		frameNum++;

		// Get current image		
		video >> inputImg;
		if( inputImg.empty() )
			break;

		 // Color Conversion
		 if(inputImg.channels() == 3)		 
			 cvtColor(inputImg, inputImgGray, CV_BGR2GRAY);				 		 
		 else	 
			 inputImg.copyTo(inputImgGray);			 		 

		 // Process
		 clock_t begin = clock();
		 ipm.applyHomography( inputImg, outputImg );			
		 clock_t end = clock();
		 double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		 printf("%.2f (ms)\r", 1000*elapsed_secs);
		 ipm.drawPoints(origPoints, inputImg );
		 
		 // ++++++++++++++++++++++++++++++++++++++++++++++
		 // Draw something on the IPM view
		 vector<Point> square;
		 square.push_back( Point(width/3, 2*height/3) );
		 square.push_back( Point(2*width/3, 2*height/3) );
		 square.push_back( Point(2*width/3, height/3) );
		 square.push_back( Point(width/3, height/3) );
		 vector<vector<Point> > vect;
		 vect.push_back(square);
		 cv::fillPoly(outputImg, vect, CV_RGB(255,255,255));

		 // Apply back
		 // On image
		  Mat backProjection;
		 ipm.applyHomographyInv( outputImg, backProjection );
		 imshow("backProjection", backProjection);

		 // On points
		 vector<Point> squareInv;
		 for(size_t i=0; i<square.size(); ++i)
		 {
			 Point2d point(square[i].x, square[i].y);
			 Point2d origPoint = ipm.applyHomographyInv( point );
			 squareInv.push_back( Point( origPoint.x, origPoint.y ) );
		 }
		 vector<vector<Point> > vectOrig;
		 vectOrig.push_back(squareInv);
		 cv::fillPoly(inputImg, vectOrig, CV_RGB(255,255,255));
		 // ++++++++++++++++++++++++++++++++++++++++++++++

		 // View		
		 imshow("Input", inputImg);
		 imshow("Output", outputImg);
		 waitKey(1);
	}

	return 0;	
}		
