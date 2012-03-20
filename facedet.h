//Developed By Yash Shah
//facedet.h
//

#include <map>
#include <string>

using namespace std;

// Public interface for face detection
int      initFaceDet(string haarCascadePath, map<string, string> cascadeMap);
void     closeFaceDet();
CvRect * detectObject(IplImage * pImg, string object);
