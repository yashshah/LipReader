//Developed By Yash Shah
//TrackFaces.c
//


///////////////////////////////////////////////////////////////////////////////
// An example program that It detects a face in live video, and automatically
// begins tracking it, using the Simple Camshift Wrapper.

#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include "capture.c"
#include "facedet.c"
#include "camshift_wrapper.c"
#include <time.h>
#include <map>
#include <string>

using namespace cv;
using namespace std;

//// Constants
const char * DISPLAY_WINDOW = "CroppedFace";
#define OPENCV_ROOT  "/usr/share/kde4/apps/libkface/haarcascades/"


//// Global variables
IplImage  * pVideoFrameCopy = 0;
IplImage  * mouthImage = 0;
int speakingSequence = 0;

//// Function definitions
int initAll();
void exitProgram(int code);
void captureVideoFrame();

/*Mat rotateImage(const Mat& source, double angle)*/
/*{*/
/*    Point2f src_center(source.cols/2.0F, source.rows/2.0F);*/
/*    Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);*/
/*    Mat dst;*/
/*    warpAffine(source, dst, rot_mat, source.size());*/
/*    return dst;*/
/*}*/

map<string, string> cascadeMap;

void drawElements(IplImage* img2) {

	float vthresh = img2->height*(1-1/1.6);
	float vfactor = img2->height/9;
	float hthreshL = img2->width/4;
	float hthreshR = img2->width*6/9;
	float hfactor = img2->width/6;
	
	
	float mvthresh = img2->height*(1-1/3.6);
	float mvfactor = img2->height/8;
	float mhthresh = img2->width/2;
	float mhfactor = img2->width/4;
	
	cvRectangle(img2,cvPoint(hthreshL-hfactor, vthresh-vfactor), cvPoint(hthreshL+hfactor,vthresh+vfactor), CV_RGB(255,0,0), 3, 8, 0);
	cvRectangle(img2,cvPoint(hthreshR-hfactor, vthresh-vfactor), cvPoint(hthreshR+hfactor,vthresh+vfactor), CV_RGB(255,0,0), 3, 8, 0);		
	cvRectangle(img2,cvPoint(mhthresh-mhfactor, mvthresh-mvfactor), cvPoint(mhthresh+mhfactor,mvthresh+mvfactor), CV_RGB(0,0,255), 3, 8, 0);
	
}
void isSpeaking(IplImage* img) {
		
	IplImage *img2 = cvCreateImage(cvSize(400,400),img->depth,img->nChannels);
	cvResize(img, img2);	
	
	int mvthresh = img2->height*(1-1/4.6);
	int mvfactor = img2->height/8;
	int mhthresh = img2->width/2;
	int mhfactor = img2->width/4;

	CvRect rect = cvRect(mhthresh-mhfactor, mvthresh-mvfactor,2*mhfactor,2*mvfactor);

	cvSetImageROI( img2, rect);
    IplImage *mouth = cvCreateImage(cvGetSize(img2),img2->depth,img2->nChannels);
	cvCopy(img2, mouth, NULL);	                   		
	IplImage *diff = cvCreateImage(cvGetSize(img2),img2->depth,img2->nChannels);
             
	if(!mouthImage){
		mouthImage = cvCreateImage(cvGetSize(img2),img2->depth,img2->nChannels);
	}
	

   	cvAbsDiff(mouth, mouthImage, diff);
  	cvNamedWindow("Mouth");
  	cvShowImage("Mouth",diff);
  	
	int height    = diff->height;
	int width     = diff->width;
	int step      = diff->widthStep;
	int channels  = diff->nChannels;
	uchar* data= (uchar *)diff->imageData;
	int sum = 0;

	for(int i=0;i<height;i++) {
		for(int j=0;j<width;j++) {
			int gray = 0;
			for(int k=0;k<channels;k++) {
				gray += data[i*step+j*channels+k];
			}
			sum+=gray/3; 
		}
	}
							
	if(sum/1000>200) {
		speakingSequence++;
		printf("%d: Speaking: TRUE\n", speakingSequence);
	}
	

 
	cvCopy(mouth, mouthImage, NULL);
    cvResetImageROI(img2);
		
}

//////////////////////////////////
// main()
//
int main( int argc, char** argv )
{
	CvRect * pFaceRect = 0;
	CvRect * pMouthRect = 0;
	if( !initAll() ) exitProgram(-1);
	time_t start;
	// Capture and display video frames until a face
	// is detected
	bool second = false;
	bool knownFace = false;
	bool flag=false;
	cvNamedWindow("Live");
	
	while(1){
		
		captureVideoFrame();
		cvShowImage("Live",pVideoFrameCopy);
		start = time (NULL);
				
		if(start%2==0&&flag)
			second = true;			
		
		if(start%2!=0)
			flag=true;
		
		if(second) {
			second=false;
			flag=false;
			pFaceRect = detectObject(pVideoFrameCopy, "face"); 	
			if(pFaceRect !=0) knownFace=true;
			else knownFace = false;	
			}
		
		if(knownFace) {
		// Show the display image		
			if( (char)27==cvWaitKey(1) ) exitProgram(0);
			
			// initialize tracking
				startTracking(pVideoFrameCopy, pFaceRect);
				CvBox2D faceBox;
		

				// track the face in the new video frame
				faceBox = track(pVideoFrameCopy);

				// outline face ellipse
				//cvEllipseBox(pVideoFrameCopy, faceBox, CV_RGB(255,0,0), 3, CV_AA, 0 );
		
				CvRect fastRect = cv::RotatedRect(faceBox).boundingRect();		
				cvSetImageROI( pVideoFrameCopy, fastRect);		
				IplImage *img2 = cvCreateImage(cvGetSize(pVideoFrameCopy),pVideoFrameCopy->depth,pVideoFrameCopy->nChannels);
				                       
				cvCopy(pVideoFrameCopy, img2, NULL);
				isSpeaking(img2);
				drawElements(img2);
		
				cvShowImage( DISPLAY_WINDOW, img2 );
				cvResetImageROI(pVideoFrameCopy);
				cvReleaseImage(&img2);
		
				if( (char)27==cvWaitKey(1) ) break;

			}
	}

	exitProgram(0);
	return 0;
}


//////////////////////////////////
// initAll()
//
int initAll()
{
	cascadeMap["face"] = "haarcascade_frontalface_alt.xml";
	cascadeMap["mouth"] ="haarcascade_mcs_mouth.xml";
	cascadeMap["leftEye"] = "haarcascade_mcs_lefteye.xml";
	cascadeMap["rightEye"] = "haarcascade_mcs_righteye.xml";
	if( !initCapture() ) return 0;
	if( !initFaceDet(OPENCV_ROOT,
		cascadeMap))
		return 0;

	// Startup message tells user how to begin and how to exit
	printf( "\n********************************************\n"
	        "To exit, click inside the video display,\n"
	        "then press the ESC key\n\n"
			"Press <ENTER> to begin"
			"\n********************************************\n" );
	fgetc(stdin);

	// Create the display window
	cvNamedWindow( DISPLAY_WINDOW, 1 );

	// Initialize tracker
	captureVideoFrame();
	if( !createTracker(pVideoFrameCopy) ) return 0;

	// Set Camshift parameters
	setVmin(60);
	setSmin(50);

	return 1;
}


//////////////////////////////////
// exitProgram()
//
void exitProgram(int code)
{
	// Release resources allocated in this file
	cvDestroyWindow( DISPLAY_WINDOW );
	cvReleaseImage( &pVideoFrameCopy );

	// Release resources allocated in other project files
	closeCapture();
	closeFaceDet();
	releaseTracker();

	exit(code);
}


//////////////////////////////////
// captureVideoFrame()
//
void captureVideoFrame()
{
	// Capture the next frame
	IplImage  * pVideoFrame = nextVideoFrame();
	if( !pVideoFrame ) exitProgram(-1);

	// Copy it to the display image, inverting it if needed
	if( !pVideoFrameCopy )
		pVideoFrameCopy = cvCreateImage( cvGetSize(pVideoFrame), 8, 3 );
	cvCopy( pVideoFrame, pVideoFrameCopy, 0 );
	pVideoFrameCopy->origin = pVideoFrame->origin;

	if( 1 == pVideoFrameCopy->origin ) // 1 means the image is inverted
	{
		cvFlip( pVideoFrameCopy, 0, 0 );
		pVideoFrameCopy->origin = 0;
	}
}



