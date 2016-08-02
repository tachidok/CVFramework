#include "cc_capture_from_screen.h"

// ===================================================================
// Constructor
// ===================================================================
CCCaptureFromScreen::CCCaptureFromScreen()
{
    // The initial capture region is not valid
    Capture_region_valid = false;
}

// ===================================================================
// Empty destructor
// ===================================================================
CCCaptureFromScreen::~CCCaptureFromScreen() { }

// ===================================================================
// Set capture geometry. A cv::Rect object to define the initial
// 2D coordinate, the width and the height of the capture rectangule
// ===================================================================
void CCCaptureFromScreen::set_capture_geometry(const unsigned x,
                                               const unsigned y,
                                               const unsigned width,
                                               const unsigned height)
{
    // Get the size of the desktop
    int desktop_width = QApplication::desktop()->width();
    int desktop_height = QApplication::desktop()->height();

    // Is the capture region INSIDE the desktop .... ?
    if (x + width < desktop_width &&
            y + height < desktop_height &&
            x + width >= 0 &&
            y + height >= 0)
    {
        Capture_rect.x = x;
        Capture_rect.y = y;
        Capture_rect.width = width;
        Capture_rect.height = height;

        Capture_region_valid = true;
    }
    else
    {
        Capture_region_valid = false;
    }

}

// ===================================================================
// The capture method, this is the one that does the magic!!!
// ===================================================================
bool CCCaptureFromScreen::capture(cv::Mat &image)
{
    if (Capture_region_valid)
    {
#if 0
        QPixmap pixmap(QPixmap::grabWindow(QApplication::desktop()->winId(),
                                           Capture_rect.x,
                                           Capture_rect.y,
                                           Capture_rect.width,
                                           Capture_rect.height));
#endif

        QScreen *my_screen = QApplication::primaryScreen();
        QPixmap pixmap(my_screen->grabWindow(QApplication::desktop()->winId(),
                                             Capture_rect.x,
                                             Capture_rect.y,
                                             Capture_rect.width,
                                             Capture_rect.height));

        // Free any content in the image
        image.release();

        // The image, Transfrom from QPixmap to cv::Mat ('false' argument for
        // not cloning data into cv::Mat)
        image = ASM::QPixmapToCvMat(pixmap);

        return true;
    }
    else
    {
        return false;
    }

}
