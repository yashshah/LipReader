//Developed By Yash Shah
//camshift_wrapper.h


// Public interface for the Simple Camshift Wrapper

#ifndef __SIMPLE_CAMSHIFT_WRAPPER_H
#define __SIMPLE_CAMSHIFT_WRAPPER_H

// Main Control functions
int     createTracker(const IplImage * pImg);
void    releaseTracker();
void    startTracking(IplImage * pImg, CvRect * pRect);
CvBox2D track(IplImage *);

// Parameter settings
void setVmin(int vmin);
void setSmin(int smin);

#endif
