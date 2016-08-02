#ifndef CCCAPTURESCREENX11_H
#define CCCAPTURESCREENX11_H

// ===================================================
// Capture screen
// ===================================================
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cstdint>
#include <cstring>
#include <vector>

class CCCaptureScreenX11
{

public:

    // Empty constructor
    CCCaptureScreenX11();

    // Empty destructor
    ~CCCaptureScreenX11();

    // Captures image from screen
    //bool image_from_display(cv::Mat &live_image);
    bool image_from_display(std::vector<uint8_t>& pixels,
                            int& width, int& height, int& bitsPerPixel);

};

#endif // CCCAPTURESCREENX11_H
