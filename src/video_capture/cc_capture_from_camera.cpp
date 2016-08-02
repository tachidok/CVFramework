#include "cc_capture_from_camera.h"

// ===================================================================
// Constructor
// ===================================================================
CCCaptureFromCamera::CCCaptureFromCamera()
{
    // Initialise camera
    Camera.open(0);
}

// ===================================================================
// Empty destructor
// ===================================================================
CCCaptureFromCamera::~CCCaptureFromCamera()
{

}

// ===================================================================
// The capture method, this is the one that does the magic!!!
// ===================================================================
bool CCCaptureFromCamera::capture(cv::Mat &image)
{
    // Free any content in the image
    image.release();

    if (!Camera.isOpened())
    {
        return false;
    }

    Camera >> image;

    return true;

}
