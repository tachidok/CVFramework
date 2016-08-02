#ifndef CCPROCESSIMAGEFROMCAMERA_H
#define CCPROCESSIMAGEFROMCAMERA_H

#include <QObject>
#include <QThread>

// -----------------------------------------------------------------------
// Include for the screen capturer
#include "../../../src/video_capture/cc_capture_from_camera.h"
// -----------------------------------------------------------------------
// The base class
#include "ccprocessimage.h"

class CCProcessImageFromCamera : public QObject, virtual public CCProcessImage
{
    Q_OBJECT
public:

    // Constructor. The miliseconds variable is used to specify the
    // time the thread sleeps after getting an image while others read
    // it and work on it
    CCProcessImageFromCamera(const unsigned miliseconds = 30);

    // Destructor
    ~CCProcessImageFromCamera();

    // -------------------------------------------------------------------
    // Image displaying
    // -------------------------------------------------------------------

    // Show image in the selected window
    void show_image();

    // Free some stuff but do not kill the object
    void free_stuff();

    // Clear applied process
    void clear();

    // -------------------------------------------------------------------
    // Image capturing
    // -------------------------------------------------------------------

    // Stop the thread forever (can not be re-activated)
    inline void stop()
    {Stop = true;}

    // Start the thread
    inline void start_thread()
    {Thread.start();}

    // Set the capture thread pointer
    inline void set_capture_thread_pt(CCCaptureFromCamera *capture_thread_pt)
    {Capture_from_camera_thread_pt = capture_thread_pt;}

    // Set mutex to avoid reading an image when it is been written
    // by the capturer
    inline void set_mutex_pt(QMutex *mutex_pt)
    {Mutex_pt = mutex_pt;}

    // Start image processing
    void start_image_processing()
    {Do_image_processing = true;}

    // Continue image processing
    void continue_image_processing()
    {Do_image_processing = true;}

    // Interrupt image processing
    void interrupt_image_processing()
    {Do_image_processing = false;}

    // -------------------------------------------------------------------
    // Image processing
    // -------------------------------------------------------------------

    // -------------------------------------------------------------------
    // Filters
    // -------------------------------------------------------------------

    // Enable the application of the normalised filter
    inline void enable_normalised_filter() {Apply_normalised_filter = true;}

    // Disable the application of the normalised filter
    inline void disable_normalised_filter() {Apply_normalised_filter = false;}

    // Enable the application of the gaussian filter
    inline void enable_gaussian_filter() {Apply_gaussian_filter = true;}

    // Disable the application of the gaussian filter
    inline void disable_gaussian_filter() {Apply_gaussian_filter = false;}

    // Enable the application of the median filter
    inline void enable_median_filter() {Apply_median_filter = true;}

    // Disable the application of the median filter
    inline void disable_median_filter() {Apply_median_filter = false;}

    // Reset all filters (set them as disabled)
    void reset_filters();

    // -------------------------------------------------------------------
    // Contrast and brightness
    // -------------------------------------------------------------------
    // Enable the application of contrast
    void enable_contrast(){ Apply_contrast = true;}

    // Diable the application of contrast
    void disable_contrast(){ Apply_contrast = false;}

    // Enable the application of brightness
    void enable_brightness(){ Apply_brightness = true;}

    // Disable the application of brightness
    void disable_brightness(){ Apply_brightness = false;}

    // Reset the application of contrast and brightness
    void reset_contrast_and_brightness();

    // -------------------------------------------------------------------
    // Histogram equalisation
    // -------------------------------------------------------------------
    // Enable the application of the histogram equalisation
    inline void enable_histogram_equalisation() {Apply_histogram_equalisation = true;}

    // Disable the application of the histogram equalisation
    inline void disable_histogram_equalisation() {Apply_histogram_equalisation = false;}

    // Reset histogram equalisation
    void reset_histogram_equalisation();

    // -------------------------------------------------------------------
    // Zoom
    // -------------------------------------------------------------------
    // Enable the application of one and only one zoom in step
    inline void enable_one_zoom_in_step(){Apply_one_zoom_in_step = true;}

    // Disable the application of one zoom in
    inline void disable_one_zoom_in_step(){Apply_one_zoom_in_step = false;}

    // Enable the application of one and only one zoom out step
    inline void enable_one_zoom_out_step(){Apply_one_zoom_out_step = true;}

    // Disable the application of one zoom out
    inline void disable_one_zoom_out_step(){Apply_one_zoom_out_step = false;}

    // Reset application of zooms
    void reset_zoom();

    // -------------------------------------------------------------------
    // Rotations
    // -------------------------------------------------------------------
    // Enable the application of rotations
    inline void enable_rotation(){Apply_rotation = true;}
    // Disable the application of rotations
    inline void disable_rotation(){Apply_rotation = false;}

    // Reset the application of rotations
    void reset_rotation();

protected:

    // -------------------------------------------------------------------
    // Image capturing
    // -------------------------------------------------------------------
    // The screen capturer
    CCCaptureFromCamera *Capture_from_camera_thread_pt;

    // The mutex from the screen capturer
    QMutex *Mutex_pt;

    // The thread that executes the "run()" method
    QThread Thread;

    // Do image processing
    bool Do_image_processing;

    // Flag to stop the thread and finish
    bool Stop;

    // -------------------------------------------------------------------
    // Image displaying
    // -------------------------------------------------------------------

    // Time in miliseconds the thread gives to read the image from the
    // buffer
    const unsigned Miliseconds;

    // The name of the window that shows the tracking or output
    // from the tracker
    char *Window_original_video = "CCCaptureFromCamera - Original";
    // The name of the window that shows the processing of the image
    char *Window_processed_video = "CCCaptureFromCamera - Processed";

    // -------------------------------------------------------------------
    // Image processing
    // -------------------------------------------------------------------

    // -------------------------------------------------------------------
    // Filters
    // -------------------------------------------------------------------
    // States what filter should be applied
    bool Apply_normalised_filter;
    bool Apply_gaussian_filter;
    bool Apply_median_filter;

    // Contrast and brightness
    bool Apply_contrast;
    bool Apply_brightness;

    // -------------------------------------------------------------------
    // Histogram equalisation
    // -------------------------------------------------------------------
    bool Apply_histogram_equalisation;

    // -------------------------------------------------------------------
    // Zoom
    // -------------------------------------------------------------------
    bool Apply_one_zoom_in_step;
    bool Apply_one_zoom_out_step;

    // -------------------------------------------------------------------
    // Rotations
    // -------------------------------------------------------------------
    bool Apply_rotation;

signals:

    void finished();

public slots:

    // The method that is run when the "Thread_pt" starts
    void run();

};

#endif // CCPROCESSIMAGEFROMCAMERA_H
