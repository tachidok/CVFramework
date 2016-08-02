#include "cccapturethread.h"

// ===================================================================
// Constructor (initialise the thread)
// ===================================================================
CCCaptureThread::CCCaptureThread(QObject *parent, const unsigned miliseconds)
    : QObject(parent),
      Miliseconds(miliseconds)
{
    // The thread is run by default
    Stop = false;

    // Initialise state variables
    New_image_ready = false;

    // Display the image by default
    Display_image = true;

    // The thread is not capturing when first created
    Capture = false;

    // The initial capture region is not valid
    Capture_region_valid = false;

#if 0
    // Initialise the number of registered images
    N_registered_image_pointers = 0;
#endif

    // Connect the signal from the thread to the slots of this
    // object
    connect(&Thread, SIGNAL(started()), this, SLOT(run()));
    connect(this, SIGNAL(finished()), &Thread, SLOT(quit()));
    connect(&Thread, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(&Thread, SIGNAL(finished()), &Thread, SLOT(deleteLater()));
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));

    // Move to thread
    moveToThread(&Thread);
}

// ===================================================================
// Destructor
// ===================================================================
CCCaptureThread::~CCCaptureThread()
{ }

// ===================================================================
// Consume or read the ready image if an external image pointer
// has been set
// ===================================================================
void CCCaptureThread::consume_new_image()
{
    // "Serialise" this section by locking the mutex
    if (Mutex_capturing_image.tryLock())
    {
        // Check if we have a new image
        if (is_new_image_ready())
        {
            New_image_ready = false;
        }
        else // There is not ready image to 'consume'
        {
            // Do nothing
        }

        // Free mutex
        Mutex_capturing_image.unlock();
    }

}

// ===================================================================
// Set capture geometry. A cv::Rect object to define the initial
// 2D coordinate, the width and the height of the capture rectangule
// ===================================================================
void CCCaptureThread::set_capture_geometry(const unsigned x,
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

#if 0
// ===================================================================
// Register an image pointer so that the captured image is stored in
// the pointer in addition to its displaying
// ===================================================================
void CCCaptureThread::register_image_pointer(cv::Mat *image_pt)
{
    // Increase the number of registered image pointers
    N_registered_image_pointers++;

    // Add the image pointer to the registered images
    Registered_image_pt.push_back(image_pt);
}

// ===================================================================
// Unregister pointer to image
// ===================================================================
void CCCaptureThread::unregister_image_pointer(cv::Mat *image_pt)
{
    // Get the position of the input "image_pt" pointer
    int index_to_delete = Registered_image_pt.indexOf(image_pt);
    // Delete the registered image pointer based on the index
    Registered_image_pt.remove(index_to_delete);

    // Decrease the number of registered images
    N_registered_image_pointers--;
}
#endif

// ===================================================================
// The capture method, this is the one that does the magic!!!
// ===================================================================
bool CCCaptureThread::capture_screen(cv::Mat &image)
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

        // Free any content in the
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

// ===================================================================
// The method that is run when the "Thread_pt" starts
// ===================================================================
void CCCaptureThread::run()
{

    // Repeat until not stop received
    while(!Stop)
    {

        if (Capture)
        {
            // Only capture if previous image has been consumed
            if (!New_image_ready)
            {
                // "Serialise" this section by locking the mutex
                if (Mutex_capturing_image.tryLock())
                {
                    // Capture screen
                    New_image_ready = capture_screen(Captured_image);

                    if (New_image_ready)
                    {
                        // Is image displaying allowed
                        if (Display_image)
                        {
                            // Resize the input image and output it in the same image
                            //cv::resize(live_image, live_image, cv::Size(640, 480));
                            cv::namedWindow("CCCaptureThread-video");
                            cv::imshow("CCCaptureThread-video", Captured_image);
                        }

#if 0
                        // Set the pointer of the image to all the observers
                        for (unsigned i = 0; i < N_registered_image_pointers; i++)
                        {
                            Registered_image_pt[i] = Image_pt;
                        }

                        // If there is not image pointer registered then "consume"
                        // or read my own image
                        if (n_registered_image_pointers() == 0)
                        {
                            // The new image has been worked out
                            New_image_ready = false;
                        }
#endif

                        // Auto-consume
                        //New_image_ready = false;

                    } // if (New_image_ready)

                    // Free mutex
                    Mutex_capturing_image.unlock();

                }


            } // if (!New_image_ready)

        }

        // Give some time for the others threads to read the
        // image
        Thread.msleep(Miliseconds);

    }

    emit finished();

    // Wait while anything else ends
    Thread.sleep(1);

}
