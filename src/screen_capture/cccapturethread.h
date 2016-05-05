#ifndef CCCAPTURETHREAD_H
#define CCCAPTURETHREAD_H

#include <QObject>
#include <QThread>
#include <QPixmap>
#include <QScreen>
#include <QMutex>

// We need to include the following two to avoid compilation errors
#include <QApplication>
#include <QDesktopWidget>

// -----------------------------------------------------------------------
// General includes
#include "../general/general_includes.h"
// -----------------------------------------------------------------------

class CCCaptureThread : public QObject
{
    Q_OBJECT

public:

    // Constructor, the miliseconds variable is used to specify the
    // time the thread sleeps after getting an image while others read
    // it and work on it
    explicit CCCaptureThread(QObject *parent = 0,
                             const unsigned miliseconds = 30);

    // Destructor
    ~CCCaptureThread();

    // Indicate we have a new image ready
    inline bool is_new_image_ready()
    {return New_image_ready;}

    inline cv::Mat *image_pt()
    {return Image_pt;}

    // Consume or read the ready image if an external image pointer
    // has been set
    void consume_new_image();

    // Enable image displaying
    inline void enable_image_displaying()
    {Display_image = true;}

    // Disable image displaying
    inline void disable_image_displaying()
    {Display_image = false;}

    // Start capturing
    inline void start_capturing()
    {Capture = true;}

    // Continue capturing
    inline void continue_capturing()
    {Capture = true;}

    // Interrupt capturing
    inline void interrupt_capturing()
    {Capture = false;}

    // Is the thread capturing?
    inline bool capturing()
    {return Capture;}

    // Start the thread
    inline void start_thread()
    {Thread.start();}

    // Stop the thread forever (can not be re-activated)
    inline void stop()
    {Stop = true;}

    // Set capture geometry. A cv::Rect object to define the initial
    // 2D coordinate, the width and the height of the capture rectangule
    void set_capture_geometry(const unsigned x, const unsigned y,
                              const unsigned width, const unsigned height);

#if 0
    // Register pointer to store captured image
    void register_image_pointer(cv::Mat *image_pt);

    // Unregister pointer to image
    void unregister_image_pointer(cv::Mat *image_pt);

    // Number of registered image pointers
    inline unsigned n_registered_image_pointers()
    {return N_registered_image_pointers;}
#endif

    // Get a pointer to the mutex that avoid accessing the captured Image
    // while capturing ...
    inline QMutex &mutex_capturing_image()
    {return Mutex_capturing_image;}

protected:

    // The thread that executes the "run()" method
    QThread Thread;

    // A mutex to share with other threads
    QMutex Mutex_capturing_image;

    // Flag to indicate whether we have a new image to work with or not
    bool New_image_ready;

    // Flag to indicate whether to display or not the image
    bool Display_image;

    // Flag to do capturing
    bool Capture;

    // Flag to stop the thread and finish
    bool Stop;

    // Geometry of capture
    cv::Rect Capture_rect;

    // Flag to indicate whether the capture region is valid or not
    bool Capture_region_valid;

    // Time in miliseconds the thread gives to read the image from the
    // buffer
    const unsigned Miliseconds;

#if 0
    // --------------------------------------------------------------------
    // Observer design pattern to implement the registered objects that
    // ask for the image
    // --------------------------------------------------------------------
    // Number of registered image pointers
    unsigned N_registered_image_pointers;

    // A vector to store the registered images
    QVector<cv::Mat*> Registered_image_pt;
#endif

    // The image pointer where to store the image, if there are
    // registered objects then they read from this image
    cv::Mat *Image_pt;

    // -------------------------------------------------------------------
    // Capture screen stuff
    // -------------------------------------------------------------------
    bool capture_screen(cv::Mat &image);

signals:

    void finished();

public slots:

    // The method that is run when the "Thread_pt" starts
    void run();

};

#endif // CCCAPTURETHREAD_H
