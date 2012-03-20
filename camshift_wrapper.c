//Developed By Yash Shah
//camshift_wrapper.c



#include "cv.h"
#include <stdio.h>
#include "camshift_wrapper.h"


// Parameters
int   nHistBins = 30;                 // number of histogram bins
float rangesArr[] = {0,180};          // histogram range
int vmin = 65, vmax = 256, smin = 55; // limits for calculating hue


// File-level variables
IplImage * pHSVImg  = 0; // the input image converted to HSV color mode
IplImage * pHueImg  = 0; // the Hue channel of the HSV image
IplImage * pMask    = 0; // this image is used for masking pixels
IplImage * pProbImg = 0; // the face probability estimates for each pixel
CvHistogram * pHist = 0; // histogram of hue in the original face image

CvRect prevFaceRect;  // location of face in previous frame
CvBox2D faceBox;      // current face-location estimate


int nFrames = 0;


// Declarations for internal functions
void updateHueImage(const IplImage * pImg);


//////////////////////////////////
// createTracker()
//
int createTracker(const IplImage * pImg)
{
	// Allocate the main data structures ahead of time
	float * pRanges = rangesArr;
	pHSVImg  = cvCreateImage( cvGetSize(pImg), 8, 3 );
	pHueImg  = cvCreateImage( cvGetSize(pImg), 8, 1 );
	pMask    = cvCreateImage( cvGetSize(pImg), 8, 1 );
	pProbImg = cvCreateImage( cvGetSize(pImg), 8, 1 );

	pHist = cvCreateHist( 1, &nHistBins, CV_HIST_ARRAY, &pRanges, 1 );

	return 1;
}


//////////////////////////////////
// releaseTracker()
//
void releaseTracker()
{
	// Release all tracker resources
	cvReleaseImage( &pHSVImg );
	cvReleaseImage( &pHueImg );
	cvReleaseImage( &pMask );
	cvReleaseImage( &pProbImg );

	cvReleaseHist( &pHist );
}


//////////////////////////////////
// startTracking()
//
void startTracking(IplImage * pImg, CvRect * pFaceRect)
{
	float maxVal = 0.f;

	// Make sure internal data structures have been allocated
	if( !pHist ) createTracker(pImg);

	// Create a new hue image
	updateHueImage(pImg);

	// Create a histogram representation for the face
    cvSetImageROI( pHueImg, *pFaceRect );
    cvSetImageROI( pMask,   *pFaceRect );
    cvCalcHist( &pHueImg, pHist, 0, pMask );
    cvGetMinMaxHistValue( pHist, 0, &maxVal, 0, 0 );
    cvConvertScale( pHist->bins, pHist->bins, maxVal? 255.0/maxVal : 0, 0 );
    cvResetImageROI( pHueImg );
    cvResetImageROI( pMask );

	// Store the previous face location
	prevFaceRect = *pFaceRect;
}


//////////////////////////////////
// track()
//
CvBox2D track(IplImage * pImg)
{
	CvConnectedComp components;

	// Create a new hue image
	updateHueImage(pImg);

	// Create a probability image based on the face histogram
	cvCalcBackProject( &pHueImg, pProbImg, pHist );
    cvAnd( pProbImg, pMask, pProbImg, 0 );

	// Use CamShift to find the center of the new face probability
    cvCamShift( pProbImg, prevFaceRect,
                cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),
                &components, &faceBox );

	// Update face location and angle
    prevFaceRect = components.rect;
	faceBox.angle = -faceBox.angle;

	return faceBox;
}


//////////////////////////////////
// updateHueImage()
//
void updateHueImage(const IplImage * pImg)
{
	// Convert to HSV color model
	cvCvtColor( pImg, pHSVImg, CV_BGR2HSV );

	// Mask out-of-range values
	cvInRangeS( pHSVImg, cvScalar(0, smin, MIN(vmin,vmax), 0),
	            cvScalar(180, 256, MAX(vmin,vmax) ,0), pMask );

	// Extract the hue channel
	cvSplit( pHSVImg, pHueImg, 0, 0, 0 );
}


//////////////////////////////////
// setVmin()
//
void setVmin(int _vmin)
{ vmin = _vmin; }


//////////////////////////////////
// setSmin()
//
void setSmin(int _smin)
{ smin = _smin; }


