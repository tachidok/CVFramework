#ifndef CCCAPTUREFROMCAMERA_H
#define CCCAPTUREFROMCAMERA_H

// -----------------------------------------------------------------------
// More includes
// -----------------------------------------------------------------------
#include "ac_capture_thread.h"

class CCCaptureFromCamera : public ACCaptureThread
{

public:

    // Empty constructor
    CCCaptureFromCamera();

    // Empty destructor
    ~CCCaptureFromCamera();

    // -------------------------------------------------------------------
    // Capture
    // -------------------------------------------------------------------
    // The capture function, must be implemented for each derived class
    bool capture(cv::Mat &image);

protected:

    cv::VideoCapture Camera;

};

#endif // CCCAPTUREFROMCAMERA_H
