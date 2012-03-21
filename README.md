# LipReader

Detects a face through your webcam and automatically begins tracking it. I also detects eyes and mouth movements and checks if anyone is speaking or not.

## Working
Face detection is actually performed only once every 2 seconds to conserve CPU. The face tracking on our live video is done through CamShift which is fast way to keep track of moving objects.

There are two methods for setting the parameters smin and vmin:

 `setVmin()`

 `setSmin()`

These two parameters control tracking quality.  
They're set to default to the values that worked best.  
You may need to tweak these values, using the above interfaces, to get good tracking results with your setup.


## Using

You will need to modify `OPENCV_ROOT` `(TrackFaces.c, line 20)` to reflect your directory structure.

To use the Module, you'll need to have OpenCV installed. 

Alternatively, you can install `libkface-data` on ubuntu, this code uses the haarcascades that come with it, so you don't need to change any paths if you do this.

To compile, run:

`g++ TrackFaces.c -I /usr/include/opencv/ -I /usr/include/X11/ -lcv -lhighgui -lcxcore -lX11`

(assuming you have similar installation paths)

## Author
Yash Shah

`blazonware@gmail.com`

## Demo

'http://www.youtube.com/watch?v=-fpkZBYBsuc'
