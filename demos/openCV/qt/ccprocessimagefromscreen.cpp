#include "ccprocessimagefromscreen.h"

// ===================================================================
// Constructor
// ===================================================================
CCProcessImageFromScreen::CCProcessImageFromScreen(QObject *parent,
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

    // Initialise the size of the filter kernel
    Filter_kernel_size = 7;

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
CCProcessImageFromScreen::~CCProcessImageFromScreen()
{

    // Destroy the window with the original capture
    cv::destroyWindow(Window_original_video);
    // Destroy the window where the image processing is shown
    cv::destroyWindow(Window_processed_video);

}

// ===================================================================
// Show image in the selected window
// ===================================================================
void CCProcessImageFromScreen::show_image()
{
    // Just show the images
    imshow(Window_original_video, OriginalImage);
    imshow(Window_processed_video, ProcessedImage);
}

// ===================================================================
// Free some stuff but do not kill the object
// ===================================================================
void CCProcessImageFromScreen::free_stuff()
{
    // Destroy the window showing the image
    cv::destroyWindow(Window_original_video);
    cv::destroyWindow(Window_processed_video);
}

// ===================================================================
// Clear applied process
// ===================================================================
void CCProcessImageFromScreen::clear()
{
    // Copy back the original image into the processed image
    ProcessedImage = OriginalImage.clone();
}

// ===================================================================
// Apply the normalised filter to the captured image
// ===================================================================
void CCProcessImageFromScreen::apply_normalised_filter(cv::Mat &input_image,
                                                       cv::Mat &output_image)
{
    cv::blur(input_image, output_image,
             cv::Size( Filter_kernel_size, Filter_kernel_size ));
}

// ===================================================================
// Apply the gaussiang filter to the captured image
// ===================================================================
void CCProcessImageFromScreen::apply_gaussian_filter(cv::Mat &input_image,
                                                     cv::Mat &output_image)
{
    cv::GaussianBlur(input_image, output_image,
                     cv::Size( Filter_kernel_size, Filter_kernel_size ),
                     0, 0);
}

// ===================================================================
// Apply the median filter to the captured image
// ===================================================================
void CCProcessImageFromScreen::apply_median_filter(cv::Mat &input_image,
                                                   cv::Mat &output_image)
{
    cv::medianBlur(input_image, output_image,
                   Filter_kernel_size);
}

// ===================================================================
// The method that is run when the "Thread_pt" starts
// ===================================================================
void CCProcessImageFromScreen::run()
{
    herhehrehrhe // TODO: Check where to move this stuff such that the
            // windows do not appear when starting the application

    // Create the window to show the image
    cv::namedWindow(Window_original_video);
    // Create the window where to show the image processing
    cv::namedWindow(Window_processed_video);

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
                    OriginalImage = Capture_thread_pt->captured_image();

                    // Apply filter
                    if (Normalised_filter)
                    {
                        apply_normalised_filter(OriginalImage, ProcessedImage);
                    }
                    else if (Gaussian_filter)
                    {
                        apply_gaussian_filter(OriginalImage, ProcessedImage);
                    }
                    else if (Median_filter)
                    {
                        apply_median_filter(OriginalImage, ProcessedImage);
                    }
                    else
                    {
                        clear();
                    }

                    qDebug() << "CCProcessImageFromScreen::run()";

                    show_image();

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
