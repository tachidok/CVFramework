#include "ccprocessimagefromcamera.h"

// ===================================================================
// Constructor
// ===================================================================
CCProcessImageFromCamera::CCProcessImageFromCamera(const unsigned miliseconds)
    : Miliseconds(miliseconds)
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

    // Create the window to show the image
    //cv::namedWindow(Window_original_video);
    // Create the window where to show the image processing
    //cv::namedWindow(Window_processed_video);

    // Initiliase all filters as disabled
    reset_filters();

    // Do not apply contrast or brightness
    reset_contrast_and_brightness();

    // Do not apply histogram equalisation
    reset_histogram_equalisation();

    // Do not apply any zoom
    reset_zoom();

    // Do not apply rotations
    reset_rotation();

    // Do not apply personalised filter
    reset_personalised_filter();

}

// ===================================================================
// Destructor
// ===================================================================
CCProcessImageFromCamera::~CCProcessImageFromCamera()
{
    // Destroy the window with the original capture
    cv::destroyWindow(Window_original_video);
    // Destroy the window where the image processing is shown
    cv::destroyWindow(Window_processed_video);
}

// ===================================================================
// Show image in the selected window
// ===================================================================
void CCProcessImageFromCamera::show_image()
{
    // Just show the images
    imshow(Window_original_video, OriginalImage);
    imshow(Window_processed_video, ProcessedImage);
}

// ===================================================================
// Free some stuff but do not kill the object
// ===================================================================
void CCProcessImageFromCamera::free_stuff()
{
    // Destroy the window showing the image
    cv::destroyWindow(Window_original_video);
    cv::destroyWindow(Window_processed_video);
}

// ===================================================================
// Clear applied process
// ===================================================================
void CCProcessImageFromCamera::clear()
{
    // Reset all filters
    reset_filters();
    // Reset contrast and brightness
    reset_contrast_and_brightness();
    // Reset histogram equalisation
    reset_histogram_equalisation();
    // Reset zoom
    reset_zoom();
    // Reset rotations
    reset_rotation();
    // Reset personalied filter
    reset_personalised_filter();
    // Copy back the original image into the processed image
    ProcessedImage = OriginalImage.clone();
}

// ===================================================================
// Reset all filters (set them as disabled)
// ===================================================================
void CCProcessImageFromCamera::reset_filters()
{
    Apply_normalised_filter = false;
    Apply_gaussian_filter = false;
    Apply_median_filter = false;
}

// ===================================================================
// Contrast and brightness
// ===================================================================
void CCProcessImageFromCamera::reset_contrast_and_brightness()
{
    disable_contrast();
    disable_brightness();
    // Initialize contrast and brightness
    Contrast = 1.0;
    Brightness = 0.0;
}

// ===================================================================
// Reset histogram equalisation
// ===================================================================
void CCProcessImageFromCamera::reset_histogram_equalisation()
{
    Apply_histogram_equalisation = false;
}
// ===================================================================
// Reset application of zooms
// ===================================================================
void CCProcessImageFromCamera::reset_zoom()
{
    disable_zoom();
}

// ===================================================================
// Reset the application of rotations
// ===================================================================
void CCProcessImageFromCamera::reset_rotation()
{
    disable_rotation();
}

// ===================================================================
// Reset the application of a personalised filter
// ===================================================================
void CCProcessImageFromCamera::reset_personalised_filter()
{
    disable_personalised_filter();
}

// ===================================================================
// The method that is run when the "Thread_pt" starts
// ===================================================================
void CCProcessImageFromCamera::run()
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
                const bool new_image_ready = Capture_from_camera_thread_pt->is_new_image_ready();

                if (new_image_ready)
                {
                    // Read the image
                    OriginalImage = Capture_from_camera_thread_pt->captured_image();

                    // Immediately copy the original image into the processed
                    // image
                    ProcessedImage = OriginalImage.clone();

                    // Apply filter
                    if (Apply_normalised_filter)
                    {
                        apply_normalised_filter(OriginalImage, ProcessedImage);
                    }
                    else if (Apply_gaussian_filter)
                    {
                        apply_gaussian_filter(OriginalImage, ProcessedImage);
                    }
                    else if (Apply_median_filter)
                    {
                        apply_median_filter(OriginalImage, ProcessedImage);
                    }
                    else
                    {
                        //clear();
                    }

                    if (Apply_contrast || Apply_brightness)
                    {
                        // Apply contrast and brigtness to the image
                        apply_contrast(OriginalImage, ProcessedImage, Contrast);
                        apply_brightness(OriginalImage, ProcessedImage, Brightness);
                    }

                    if (Apply_histogram_equalisation)
                    {
                        // Reset the application of any filter
                        apply_histogram_equalisation(OriginalImage, ProcessedImage);
                    }

                    if (Apply_zoom)
                    {
                        // Apply the zoom
                        zoom(OriginalImage, ProcessedImage, Scale);
                    }

                    if (Apply_rotation)
                    {
                        rotate(OriginalImage, ProcessedImage, Angle, Scale);
                    }

                    if (Apply_personalised_filter)
                    {
                        apply_personalised_filter(OriginalImage, ProcessedImage, Kernel);
                    }

                    qDebug() << "CCProcessImageFromCamera::run()";

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
                Capture_from_camera_thread_pt->consume_new_image();

            }

        }

        // Give some time for others threads to do their stuff
        Thread.msleep(Miliseconds);

    }

    emit finished();

    // Wait while anything else ends
    Thread.sleep(1);

}
