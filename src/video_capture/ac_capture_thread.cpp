#include "ac_capture_thread.h"

// ===================================================================
// Constructor (initialise the thread)
// ===================================================================
ACCaptureThread::ACCaptureThread(QObject *parent, const unsigned miliseconds)
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
ACCaptureThread::~ACCaptureThread()
{ }

// ===================================================================
// Consume or read the ready image if an external image pointer
// has been set
// ===================================================================
void ACCaptureThread::consume_new_image()
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

#if 0
// ===================================================================
// Register an image pointer so that the captured image is stored in
// the pointer in addition to its displaying
// ===================================================================
void ACCaptureThread::register_image_pointer(cv::Mat *image_pt)
{
    // Increase the number of registered image pointers
    N_registered_image_pointers++;

    // Add the image pointer to the registered images
    Registered_image_pt.push_back(image_pt);
}

// ===================================================================
// Unregister pointer to image
// ===================================================================
void ACCaptureThread::unregister_image_pointer(cv::Mat *image_pt)
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
// The method that is run when the "Thread_pt" starts
// ===================================================================
void ACCaptureThread::run()
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
                    New_image_ready = capture(Captured_image);

                    if (New_image_ready)
                    {
                        // Is image displaying allowed
                        if (Display_image)
                        {
                            // Resize the input image and output it in the same image
                            //cv::resize(live_image, live_image, cv::Size(640, 480));
                            cv::namedWindow("ACCaptureThread-video");
                            cv::imshow("ACCaptureThread-video", Captured_image);
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
