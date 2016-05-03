#include "ccprocessimagethread.h"

namespace Mouse
{

void callbackMouse( int event, int x, int y, int, void* )
{
    if  ( event == cv::EVENT_LBUTTONDOWN )
    {
        std::cerr << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
    }
    else if  ( event == cv::EVENT_RBUTTONDOWN )
    {
        std::cerr << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
    }
    else if  ( event == cv::EVENT_MBUTTONDOWN )
    {
        std::cerr << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
    }
    else if ( event == cv::EVENT_MOUSEMOVE )
    {
        std::cerr << "Mouse move over the window - position (" << x << ", " << y << ")" << std::endl;

    }
}

}

CCProcessImageThread::CCProcessImageThread(QObject *parent,
                                           const unsigned miliseconds)
    : QObject(parent),
      Miliseconds(miliseconds)
{
    // -------------------------------------------------------------------
    // Image capturing
    // -------------------------------------------------------------------

    // The thread is run by default
    Stop = false;

    // Do not do image processing by default
    Do_image_processing = false;

    // Create the window to show the image
    cv::namedWindow(Window_video);
    // ... and add the call back
    cv::setMouseCallback(Window_video, Mouse::callbackMouse, 0);

    // Connect the signal from the thread to the slots of this
    // object
    connect(&Thread, SIGNAL(started()), this, SLOT(run()));
    connect(this, SIGNAL(stop()), &Thread, SLOT(quit()));
    connect(&Thread, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(&Thread, SIGNAL(finished()), &Thread, SLOT(deleteLater()));
    connect(this, SIGNAL(stop()), this, SLOT(deleteLater()));

    // Move to thread
    moveToThread(&Thread);

    // -------------------------------------------------------------------
    // Coordinates history stuff
    // -------------------------------------------------------------------
    const unsigned dim = 2;
    Tracker_coordinates.resize(dim);
    Kalman_coordinates.resize(dim);

    // -------------------------------------------------------------------
    // Tracker stuff
    // -------------------------------------------------------------------
    N_trackers = 0;

    // -------------------------------------------------------------------
    // --- More tracker stuff
    // -------------------------------------------------------------------

    // Pattern window size
    Pattern_window_size = 21;
    // Search window size
    Search_window_size = Pattern_window_size * 2;
    // Pattern size window increasing step
    Pattern_window_size_increasing_step = 2;

    // Max pattern window size
    Max_pattern_window_size = 31;
    // Min pattern window size
    Min_pattern_window_size = 11;

    // -------------------------------------------------------------------
    // Kalman stuff
    // -------------------------------------------------------------------
    N_Kalman_filters = 0;

    // Prepare data for Kalman
    // -- Dynamic parameters (2, position and velocity)
    N_Kalman_dynamic_parameters = 2;
    // -- measurement parameters (2, position and velocity)
    N_Kalman_measurement_parameters = 2;
    // -- control parameters (1, acceleration)
    N_Kalman_control_parameters = 1;

}

CCProcessImageThread::~CCProcessImageThread()
{
    if (N_Kalman_filters > 0)
    {
        // Delete each instance of Kalman filters
        for (unsigned i = 0; i < N_Kalman_filters; i++)
        {
            if (Kalman_filter_pt[i] != 0)
            {
                delete Kalman_filter_pt[i];
                Kalman_filter_pt[i] = 0;
            }
        }

        // Delete the array of Kalman filters
        Kalman_filter_pt.clear();

    }

    if (N_trackers > 0)
    {
        // Delete each instance of tracker
        for (unsigned i = 0; i < N_trackers; i++)
        {
            if (Tracker_pt[i] != 0)
            {
                delete Tracker_pt[i];
                Tracker_pt[i] = 0;
            }
        }

        // Delete the array of trackers
        Tracker_pt.clear();

    }

    // Destroy the window
    cv::destroyWindow(Window_video);
}

// ===================================================================
// The method that is run when the "Thread_pt" starts
// ===================================================================
void CCProcessImageThread::run()
{
    // Repeat until not stop received
    while(!Stop)
    {
        // Do image processing
        if (Do_image_processing)
        {
            std::cerr << "CCProcessImageThread::run() [TRY TO LOCK]\n";
            // Lock to ask for a new image
            if (Mutex_pt->tryLock())
            {
                std::cerr << "CCProcessImageThread::run() [LOCKED]\n";

                // Is there an image ready from the capturer
                const bool new_image_ready = Capture_thread_pt->is_new_image_ready();

                if (new_image_ready)
                {
                    qDebug() << "Image ready [YES]";
                    Image_pt = Capture_thread_pt->image_pt();
                    // Just paint it by now
                    //cv::namedWindow(Window_name);
                    cv::imshow(Window_video, *Image_pt);
                }
                else
                {
                    qDebug() << "Image ready [NO]";
                }

                // Free the mutex (or lock)
                Mutex_pt->unlock();
                std::cerr << "CCProcessImageThread::run() [UNLOCKED]\n";

                // Once all the image processing has been done mark the image
                // as consumed
                Capture_thread_pt->consume_new_image();

            }

        }

        // Give some time for others threads to do their stuff
        Thread.msleep(Miliseconds);

    }

    emit finished();
}

// ======================================================================
// Method to draw an aim
// ======================================================================
void CCProcessImageThread::draw_aim(cv::Mat *image_pt, const unsigned x,
                                    const unsigned y, const unsigned half_size,
                                    const unsigned r, const unsigned g, const unsigned b,
                                    const unsigned thickness,
                                    const unsigned type_line)
{
    // The centre of the aim MUST be wihtin the limits of the image
    if (static_cast<int>(x) > image_pt->cols) {return;}
    if (static_cast<int>(y) > image_pt->rows) {return;}

    // Horizontal line
    cv::line(*image_pt, cv::Point(x-half_size, y), cv::Point(x+half_size, y),
             cv::Scalar(b, g, r), thickness);
    // Vertical line
    cv::line(*image_pt, cv::Point(x, y-half_size), cv::Point(x, y+half_size),
             cv::Scalar(b, g, r), thickness, type_line);
}

// ======================================================================
// Method to draw a square
// ======================================================================
void CCProcessImageThread::draw_square(cv::Mat *image_pt, const unsigned x,
                                       const unsigned y, const unsigned half_size,
                                       const unsigned r, const unsigned g, const unsigned b,
                                       const unsigned thickness,
                                       const unsigned type_line)
{
    // The centre of the aim MUST be wihtin the limits of the image
    if (static_cast<int>(x) > image_pt->cols) {return;}
    if (static_cast<int>(y) > image_pt->rows) {return;}

    // Create a Rect object (integer due to no sub-pixels)
    cv::Rect draw_rect(x-half_size, y-half_size, half_size*2, half_size*2);
    cv::rectangle(*image_pt, draw_rect, cv::Scalar(b, g, r), thickness,
                  type_line);
}
