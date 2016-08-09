#ifndef CCPROCESSIMAGEFROMSCREEN_H
#define CCPROCESSIMAGEFROMSCREEN_H

#include <QObject>
#include <QThread>

// -----------------------------------------------------------------------
// Include for the screen capturer
#include "../../../src/video_capture/cc_capture_from_screen.h"
// -----------------------------------------------------------------------
// The base class
#include "ccprocessimage.h"

class CCProcessImageFromScreen : public QObject, virtual public CCProcessImage
{
    Q_OBJECT

public:

    // Constructor. The miliseconds variable is used to specify the
    // time the thread sleeps after getting an image while others read
    // it and work on it
    CCProcessImageFromScreen(const unsigned miliseconds = 30);

    // Destructor
    ~CCProcessImageFromScreen();

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
    inline void set_capture_thread_pt(CCCaptureFromScreen *capture_thread_pt)
    {Capture_from_screen_thread_pt = capture_thread_pt;}

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
    // Enable the application of zoom
    inline void enable_zoom(){Apply_zoom = true;}

    // Disable the application of zoom
    inline void disable_zoom(){Apply_zoom = false;}

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

    // -------------------------------------------------------------------
    // Personalised filter
    // -------------------------------------------------------------------
    // Enable the application of a personalised filter
    inline void enable_personalised_filter(){Apply_personalised_filter=true;}
    // Disable the application of a personalised filter
    inline void disable_personalised_filter(){Apply_personalised_filter=false;}

    // Set canny filter threshold
    double &low_threshold_canny_filter(){return Low_threshold_canny_filter;}

    // Reset the application of a personalised filter
    void reset_personalised_filter();

    // -------------------------------------------------------------------
    // Canny filter
    // -------------------------------------------------------------------
    // Enable application of Canny filter
    inline void enable_canny_filter(){Apply_canny_filter=true;}
    // Disable application of Canny filter
    inline void disable_canny_filter(){Apply_canny_filter=false;}

    // Reset the application of canny filter
    void reset_canny_filter();

protected:

    // -------------------------------------------------------------------
    // Image capturing
    // -------------------------------------------------------------------
    // The screen capturer
    CCCaptureFromScreen *Capture_from_screen_thread_pt;

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
    char *Window_original_video = "CCCaptureFromScreen - Original";
    // The name of the window that shows the processing of the image
    char *Window_processed_video = "CCCaptureFromScreen - Processed";

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
    bool Apply_zoom;

    // -------------------------------------------------------------------
    // Rotations
    // -------------------------------------------------------------------
    bool Apply_rotation;

    // -------------------------------------------------------------------
    // Personalised filter
    // -------------------------------------------------------------------
    bool Apply_personalised_filter;

    // -------------------------------------------------------------------
    // Canny filter
    // -------------------------------------------------------------------
    bool Apply_canny_filter;
    double Low_threshold_canny_filter;


signals:

    void finished();

public slots:

    // The method that is run when the "Thread_pt" starts
    void run();

};

#endif // CCPROCESSIMAGEFROMSCREEN_H
