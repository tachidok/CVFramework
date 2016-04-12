#include "cccapturescreenx11.h"

// ======================================================================
// Empty constructor
// ======================================================================
CCCaptureScreenX11::CCCaptureScreenX11()
{ }

// ======================================================================
// Empty destructor
// ======================================================================
CCCaptureScreenX11::~CCCaptureScreenX11()
{ }

// ======================================================================
// Empty constructor
// ======================================================================
bool CCCaptureScreenX11::image_from_display(std::vector<uint8_t> &pixels,
                                            int &width, int &height,
                                            int &bitsPerPixel)
{
#if 0
    // The pixels
    std::vector<uint8_t> pixels;
    // Widthl, height and bits per pixel of image
    int width = 0;
    int height = 0;
    int bitsPerPixel = 0;
#endif // #if 0

    width = 0;
    height = 0;

    Display* display = XOpenDisplay(nullptr);
    Window root = DefaultRootWindow(display);

    XWindowAttributes attributes = {0};
    XGetWindowAttributes(display, root, &attributes);

    width = attributes.width;
    height = attributes.height;

    XImage* img = XGetImage(display, root, 0, 0 , width, height, AllPlanes, ZPixmap);
    bitsPerPixel = img->bits_per_pixel;
    pixels.resize(width * height * 4);

    memcpy(&pixels[0], img->data, pixels.size());

    XFree(img);
    //XDestroyImage(img);
    XCloseDisplay(display);

#if 0
    // Is there something in the buffer?
    if (width && height)
    {
        live_image = cv::Mat(height, width, bitsPerPixel > 24 ? CV_8UC4 : CV_8UC3, &pixels[0]);
        //Mat(Size(Height, Width), Bpp > 24 ? CV_8UC4 : CV_8UC3, &Pixels[0]);

        cv::namedWindow("WindowTitle", cv::WINDOW_AUTOSIZE);
        cv::imshow("Live image", live_image);
    }
#endif // #if 0

    if (width && height)
    {
        return 1;
    }

    return 0;

}
