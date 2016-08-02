#ifndef CCCAPTUREFROMSCREEN_H
#define CCCAPTUREFROMSCREEN_H

// We need to include the following two to avoid compilation errors
#include <QApplication>
#include <QDesktopWidget>

// -----------------------------------------------------------------------
// More includes
// -----------------------------------------------------------------------
#include "ac_capture_thread.h"

class CCCaptureFromScreen : virtual public ACCaptureThread
{

public:

    // Constructor
    CCCaptureFromScreen();

    // Empty destructor
    ~CCCaptureFromScreen();

    // Set capture geometry. A cv::Rect object to define the initial
    // 2D coordinate, the width and the height of the capture rectangule
    void set_capture_geometry(const unsigned x, const unsigned y,
                              const unsigned width, const unsigned height);

protected:

    // Geometry of capture
    cv::Rect Capture_rect;

    // Flag to indicate whether the capture region is valid or not
    bool Capture_region_valid;

    // -------------------------------------------------------------------
    // Capture
    // -------------------------------------------------------------------
    // The capture function, must be implemented for each derived class
    bool capture(cv::Mat &image);

};

#endif // CCCAPTUREFROMSCREEN_H
