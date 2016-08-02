#ifndef CCPROCESSIMAGEFROMSCREEN_H
#define CCPROCESSIMAGEFROMSCREEN_H

#include <QObject>
#include <QThread>

// -----------------------------------------------------------------------
// General includes
#include "../../../src/general/general_includes.h"
// -----------------------------------------------------------------------
// Include for the screen capturer
#include "../../../src/screen_capture/cccapturethread.h"
// -----------------------------------------------------------------------

class CCProcessImageFromScreen : public QObject
{
    Q_OBJECT
public:

    // Constructor. The miliseconds variable is used to specify the
    // time the thread sleeps after getting an image while others read
    // it and work on it
    explicit CCProcessImageFromScreen(QObject *parent = 0,
                                      const unsigned miliseconds = 30);

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

    // Get access to the original image
    inline cv::Mat &originalImage() {return OriginalImage;}

    // Get access to the processed image
    inline cv::Mat &processedImage() {return ProcessedImage;}

    // Set the capture thread pointer
    inline void set_capture_thread_pt(CCCaptureThread *capture_thread_pt)
    {Capture_thread_pt = capture_thread_pt;}

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

    // Set the filter kernel size
    inline unsigned &filter_kernel_size() {return Filter_kernel_size;}

    // Enable the application of the normalised filter
    inline void enable_normalised_filter() {Normalised_filter = true;}

    // Disable the application of the normalised filter
    inline void disable_normalised_filter() {Normalised_filter = false;}

    // Apply the normalised filter to the captured image
    void apply_normalised_filter(cv::Mat &input_image, cv::Mat &output_image);

    // Enable the application of the gaussian filter
    inline void enable_gaussian_filter() {Gaussian_filter = true;}

    // Disable the application of the gaussian filter
    inline void disable_gaussian_filter() {Gaussian_filter = false;}

    // Apply the gaussiang filter to the captured image
    void apply_gaussian_filter(cv::Mat &input_image, cv::Mat &output_image);

    // Enable the application of the median filter
    inline void enable_median_filter() {Median_filter = true;}

    // Disable the application of the median filter
    inline void disable_median_filter() {Median_filter = false;}

    // Apply the median filter to the captured image
    void apply_median_filter(cv::Mat &input_image, cv::Mat &output_image);

protected:

    // -------------------------------------------------------------------
    // Image capturing
    // -------------------------------------------------------------------
    // The screen capturer
    CCCaptureThread *Capture_thread_pt;

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

    // The storage for the captured and processed image
    cv::Mat OriginalImage;
    cv::Mat ProcessedImage;

    // The name of the window that shows the tracking or output
    // from the tracker
    char *Window_original_video = "CCCaptureThread - Original";
    // The name of the window that shows the processing of the image
    char *Window_processed_video = "CCCaptureThread - Processed";

    // -------------------------------------------------------------------
    // Image processing
    // -------------------------------------------------------------------

    // -------------------------------------------------------------------
    // Filters
    // -------------------------------------------------------------------
    unsigned Filter_kernel_size;

    // States what filter should be applied
    bool Normalised_filter;
    bool Gaussian_filter;
    bool Median_filter;

signals:

    void finished();

public slots:

    // The method that is run when the "Thread_pt" starts
    void run();

};

#endif // CCPROCESSIMAGEFROMSCREEN_H
