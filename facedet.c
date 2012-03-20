//Developed By Yash Shah
//facedet.c


#include "cv.h"
#include <stdio.h>
#include "facedet.h"


// File-level variables
map<string, CvHaarClassifierCascade *> pCascades;  // the face detector
CvMemStorage * pStorage = 0;             // memory for detector to use
CvSeq * pFaceRectSeq;                    // memory-access interface


//////////////////////////////////
// initFaceDet()
//
int initFaceDet(string haarCascadePath, map<string, string> cascadeMap)
{
	if( !(pStorage = cvCreateMemStorage(0)) )
	{
		fprintf(stderr, "Can\'t allocate memory for face detection\n");
		return 0;
	}
	
	pCascades["face"] = (CvHaarClassifierCascade *)cvLoad( (haarCascadePath + cascadeMap["face"]).c_str(), 0, 0, 0 );
	pCascades["leftEye"] = (CvHaarClassifierCascade *)cvLoad( (haarCascadePath + cascadeMap["leftEye"]).c_str(), 0, 0, 0 );
	pCascades["rightEye"] = (CvHaarClassifierCascade *)cvLoad( (haarCascadePath + cascadeMap["rightEye"]).c_str(), 0, 0, 0 );
	pCascades["mouth"] = (CvHaarClassifierCascade *)cvLoad( (haarCascadePath + cascadeMap["mouth"]).c_str(), 0, 0, 0 );
	return 1;
}


//////////////////////////////////
// closeFaceDet()
//
void closeFaceDet()
{
/*	if(pCascade) cvReleaseHaarClassifierCascade(&pCascade);*/
	if(pStorage) cvReleaseMemStorage(&pStorage);
}


//////////////////////////////////
// detectFace()
//
CvRect * detectObject(IplImage * pImg, string object)
{
	CvRect* r = 0;

	// detect faces in image
	int minFaceSize = pImg->width / 5;
	pFaceRectSeq = cvHaarDetectObjects
		(pImg, pCascades[object], pStorage,
		1.1,                       // increase search scale by 10% each pass
		6,                         // require six neighbors
		CV_HAAR_DO_CANNY_PRUNING,  // skip regions unlikely to contain a face
		cvSize(minFaceSize, minFaceSize));

	// if one or more faces are detected, return the first one
	if( pFaceRectSeq && pFaceRectSeq->total )
		r = (CvRect*)cvGetSeqElem(pFaceRectSeq, 0);

	return r;
}



