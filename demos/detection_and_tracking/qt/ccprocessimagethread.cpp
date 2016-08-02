#include "ccprocessimagethread.h"

namespace Mouse
{
    // ======================================================================
    // The method called when a mouse event is detected on the window
    // ======================================================================
    void callbackMouseTracking( int event, int x, int y, int, void* )
    {
        if  ( event == cv::EVENT_LBUTTONDOWN )
        {
            // Store the mouse position when left clicking
            CCProcessImageThread::X_mouse = x;
            CCProcessImageThread::Y_mouse = y;
            CCProcessImageThread::Left_button_pressed = true;
            std::cerr << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
        }
        else if  ( event == cv::EVENT_RBUTTONDOWN )
        {
            CCProcessImageThread::Right_button_pressed = true;
            //std::cerr << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
        }
        else if  ( event == cv::EVENT_MBUTTONDOWN )
        {
            //std::cerr << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
        }
        else if ( event == cv::EVENT_MOUSEMOVE )
        {
            // Mark the aim for drawing
            // Store the mouse position when left clicking
            CCProcessImageThread::X_mouse = x;
            CCProcessImageThread::Y_mouse = y;
            std::cerr << "Mouse move over the window - position (" << x << ", " << y << ")" << std::endl;
        }

    }

}

// Initialise mouse position and drawing variables
// -------------------------------------------------------------------
// Mouse moving stuff
// -------------------------------------------------------------------
unsigned CCProcessImageThread::X_mouse = 0;
unsigned CCProcessImageThread::Y_mouse = 0;

bool CCProcessImageThread::Left_button_pressed = false;
bool CCProcessImageThread::Right_button_pressed = false;

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
    cv::setMouseCallback(Window_video, Mouse::callbackMouseTracking, 0);

    // Create the window where to show the image processing
    cv::namedWindow(Window_processing);

    // Connect the signal from the thread to the slots of this
    // object
    connect(&Thread, SIGNAL(started()), this, SLOT(run()));
    connect(this, SIGNAL(finished()), &Thread, SLOT(quit()));
    connect(&Thread, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(&Thread, SIGNAL(finished()), &Thread, SLOT(deleteLater()));
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));

    // Move to thread
    moveToThread(&Thread);

    // -------------------------------------------------------------------
    // Mouse stuff
    // -------------------------------------------------------------------
    X_mouse = 0;
    Y_mouse = 0;

    // Do draw the aim by default

    // -------------------------------------------------------------------
    // Coordinates history stuff
    // -------------------------------------------------------------------
    const unsigned dim = 2;
    Tracker_coordinates.resize(dim);
    Kalman_coordinates.resize(dim);

    // -------------------------------------------------------------------
    // Tracker stuff
    // -------------------------------------------------------------------
    // At least one tracker
    N_trackers = 1;

    // Allocate memory for at least one tracked object
    X_tracker.resize(N_trackers);
    Y_tracker.resize(N_trackers);

    // Create at least one tracker
    Tracker_pt.resize(N_trackers);

    //Tracker_pt[0] = new CCSimpleTracker();
    //Tracker_pt[0] = new CCNormalDistTracker();
    //Tracker_pt[0] = new CCOpenCVTracker("MIL");
    Tracker_pt[0] = new CCOpenCVTracker("BOOSTING");
    //Tracker_pt[0] = new CCOpenCVTracker("MEDIANFLOW");
    //Tracker_pt[0] = new CCOpenCVTracker("TLD");
    //Tracker_pt[0] = new CCOpenCVTracker("KCF");

    // Do not do tracking by default
    Do_tracking = false;

    // Do capture by default
    Capture_pattern = true;

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
    // At least as many as the number of trackers
    N_Kalman_filters = N_trackers;

    Kalman_filter_pt.resize(N_Kalman_filters);

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

    // Destroy the window where the tracking is performed
    cv::destroyWindow(Window_video);
    // Destroy the window where the image processing is shown
    cv::destroyWindow(Window_processing);

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
            // Lock to ask for a new image
            if (Mutex_pt->tryLock())
            {
                // Is there an image ready from the capturer
                const bool new_image_ready = Capture_thread_pt->is_new_image_ready();

                if (new_image_ready)
                {
                    // Read the image
                    //Image_pt = Capture_thread_pt->image_pt();
                    Image_pt = Capture_thread_pt->captured_image();

                    // Check whether we should stop tracking
                    if (Right_button_pressed)
                    {
                        // Stop tracking
                        Do_tracking = false;

                        // Reset the tracker
                        if (Tracker_pt[0] != 0)
                        {
                            Tracker_pt[0]->delete_pattern();
                        }

                        // Reset state
                        Right_button_pressed = false;

                    } // Check whether we should do tracking or not
                    else if (Left_button_pressed)
                    {
                        // Capture the pattern
                        Capture_pattern = true;
                        // Activate tracking
                        Do_tracking = true;

                        // Reset state
                        Left_button_pressed = false;
                    }

                    if (Do_tracking)
                    {
                        if (Capture_pattern)
                        {
                            // Get the coordinates of the tracker
                            X_tracker[0] = X_mouse;
                            Y_tracker[0] = Y_mouse;
                            if ((Tracker_pt[0]->initialise(*Image_pt,
                                                           X_tracker[0], Y_tracker[0],
                                                           Search_window_size,
                                                           Pattern_window_size)))
                            {
                                // Pattern captured, we can go to the tracking
                                // stage
                                Capture_pattern = false;
                            }
                            else // Abort tracking
                            {
                                Do_tracking = false;
                            }

                        } // Do tracking instead
                        else if (Tracker_pt[0]->initialised())
                        {
                            // Backup the old positions
                            const int old_x_tracker = X_tracker[0];
                            const int old_y_tracker = Y_tracker[0];
                            if (Tracker_pt[0]->search_pattern(*Image_pt, X_tracker[0], Y_tracker[0]))
                            {
                                // OK, pattern found
                            }
                            else
                            {
                                // FAILED, pattern lost
                                Do_tracking = false;
                            }

                        }

                    } // if (Do_tracking)

                    // ------------------------------------------------------
                    // Drawing stage
                    // ------------------------------------------------------
                    if (Do_tracking)
                    {
                        // Draw an square showing the tracking results
                        draw_square(Image_pt, static_cast<unsigned>(X_tracker[0]),
                                 static_cast<unsigned>(Y_tracker[0]),
                                 Pattern_window_size, 255, 0, 0, 1);

                        // Show me the patter you are looking for
                        //cv::Mat output_pattern;
                        //cv::resize(*(Tracker_pt->pattern_pt()), output_pattern, cv::Size(160, 120));
                        cv::imshow(Window_processing, *(Tracker_pt[0]->pattern_pt()));

                    }

                    // Should we draw an aim
                    if (Draw_aim)
                    {
                        // Draw an aim following the mouse position
                        draw_aim(Image_pt, static_cast<unsigned>(X_mouse),
                                 static_cast<unsigned>(Y_mouse),
                                 Pattern_window_size/3, 0, 255, 0, 1);
                        draw_square(Image_pt, static_cast<unsigned>(X_mouse),
                                 static_cast<unsigned>(Y_mouse),
                                 Pattern_window_size, 0, 255, 0, 1);
                    }

                    // Just paint it by now
                    //cv::namedWindow(Window_name);
                    cv::imshow(Window_video, *Image_pt);
                }
                else
                {
                    // Do nothing
                }

                // Free the mutex (or lock)
                Mutex_pt->unlock();

                // Once all the image processing has been done mark the image
                // as consumed
                Capture_thread_pt->consume_new_image();

            }

        }

        // Give some time for others threads to do their stuff
        Thread.msleep(Miliseconds);

    }

    emit finished();

    // Wait while anything else ends
    Thread.sleep(1);

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

// ======================================================================
// Increase pattern window
// ======================================================================
void CCProcessImageThread::plus_pattern_window()
{
    if (Pattern_window_size+Pattern_window_size_increasing_step <= Max_pattern_window_size)
    {
        Pattern_window_size+=Pattern_window_size_increasing_step;
        Search_window_size = Pattern_window_size*2;
    }
}

// ======================================================================
// Decrease pattern window
// ======================================================================
void CCProcessImageThread::minus_pattern_window()
{
    if (Pattern_window_size-Pattern_window_size_increasing_step >= Min_pattern_window_size)
    {
        Pattern_window_size-=Pattern_window_size_increasing_step;
        Search_window_size = Pattern_window_size*2;
    }
}
