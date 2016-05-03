#ifndef CCPROCESSIMAGETHREAD_H
#define CCPROCESSIMAGETHREAD_H

#include <QObject>
#include <QThread>

// -----------------------------------------------------------------------
// General includes
#include "../../../src/general/general_includes.h"
// -----------------------------------------------------------------------
// Include for the screen capturer
#include "../../../src/screen_capture/cccapturethread.h"
// -----------------------------------------------------------------------
// Kalman Filter
#include "../../../src/kalman/cckalmanfilter.h"
// -----------------------------------------------------------------------
// Trackers
#include "../../../src/tracker/catracker.h"
#include "../../../src/tracker/ccsimpletracker.h"
#include "../../../src/tracker/ccnormaldisttracker.h"
#include "../../../src/tracker/ccopencvtracker.h"
// -----------------------------------------------------------------------

namespace Mouse
{
void callbackMouse( int event, int x, int y, int, void* );
}

class CCProcessImageThread : public QObject
{
    Q_OBJECT

public:

    // Constructor. The miliseconds variable is used to specify the
    // time the thread sleeps after getting an image while others read
    // it and work on it
    explicit CCProcessImageThread(QObject *parent = 0,
                                  const unsigned miliseconds = 30);

    // Destructor
    ~CCProcessImageThread();

    // -------------------------------------------------------------------
    // Image capturing
    // -------------------------------------------------------------------

    // Stop the thread forever (can not be re-activated)
    inline void stop()
    {Stop = true;}

    // Start the thread
    inline void start_thread()
    {Thread.start();}

    // Get a pointer to the image
    inline cv::Mat *image_pt()
    {return Image_pt;}

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
    // Visualisation stuff (painting over image)
    // -------------------------------------------------------------------

    // Method to draw an aim
    void draw_aim(cv::Mat *image_pt, const unsigned x, const unsigned y,
                  const unsigned half_size,
                  const unsigned r, const unsigned g, const unsigned b,
                  const unsigned thickness = 1,
                  const unsigned type_line = cv::LINE_8);

    // Draw a square
    void draw_square(cv::Mat *image_pt, const unsigned x, const unsigned y,
                     const unsigned half_size,
                     const unsigned r, const unsigned g, const unsigned b,
                     const unsigned thickness = 1,
                     const unsigned type_line = cv::LINE_8);

    // -------------------------------------------------------------------
    // Tracker stuff
    // -------------------------------------------------------------------
    // The history of the tracker coordinates for all tracked objects
    QVector<QVector<double> > &tracker_coordinates()
    {return Tracker_coordinates;}

    // The history of the tracker coordinates for the i-th tracked object
    QVector<double> &tracker_coordinates(const unsigned i)
    {return Tracker_coordinates[i];}

    // The current x_tracker coordinates of all tracked objects
    QVector<double> &x_tracker()
    {return X_tracker;}

    // The current x_tracker coordinates of the i-th object
    inline double x_tracker(const unsigned i)
    {return X_tracker[i];}

    // The current y_tracker coordinates of all tracked objects
    QVector<double> &y_tracker()
    {return Y_tracker;}

    // The current y_tracker coordinates of the i-th object
    inline double y_tracker(const unsigned i)
    {return Y_tracker[i];}

    // The number of currently tracking objects
    inline unsigned n_tracker()
    {return N_trackers;}

    // -------------------------------------------------------------------
    // Kalman stuff
    // -------------------------------------------------------------------
    // The history of the Kalman coordinates
    QVector<QVector<double> > &kalman_coordinates()
    {return Kalman_coordinates;}

    // The history of the Kalman coordinates for the i-th object
    QVector<double> &kalman_coordinates(const unsigned i)
    {return Kalman_coordinates[i];}

    // The current x_kalman coordinates of all tracked objects
    QVector<double> &x_kalman()
    {return X_Kalman;}

    // The current x_kalman coordinates of the i-th object
    inline double x_kalman(const unsigned i)
    {return X_Kalman[i];}

    // The current y_kalman coordinates of all tracked objects
    QVector<double> &y_kalman()
    {return Y_Kalman;}

    // The current y_kalman coordinates of the i-th object
    inline double y_kalman(const unsigned i)
    {return Y_Kalman[i];}

    // The number of currently Kalman filters being applied
    inline unsigned n_kalman_filters()
    {return N_Kalman_filters;}

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

    // Time in miliseconds the thread gives to read the image from the
    // buffer
    const unsigned Miliseconds;

    // The image pointer
    cv::Mat *Image_pt;

    // The name of the window
    char *Window_video = "CCCaptureThread::video";

    // -------------------------------------------------------------------
    // Tracker stuff
    // -------------------------------------------------------------------
    // History of Tracker coordinates
    QVector<QVector<double> > Tracker_coordinates;

    // Current tracker coordinates
    QVector<double> X_tracker;
    QVector<double> Y_tracker;

    // The number of trackers to use (depending on the number
    // of detected objects)
    unsigned N_trackers;

    // The tracker objects
    QVector<CATracker*> Tracker_pt;

    // -------------------------------------------------------------------
    // --- More tracker stuff
    // -------------------------------------------------------------------

    // Pattern window size
    unsigned Pattern_window_size;
    // Search window size
    unsigned Search_window_size;
    // Pattern size window increasing step
    unsigned Pattern_window_size_increasing_step;

    // Max pattern window size
    unsigned Max_pattern_window_size;
    // Min pattern window size
    unsigned Min_pattern_window_size;

    // -------------------------------------------------------------------
    // Kalman stuff
    // -------------------------------------------------------------------
    // History of Kalman coordinates
    QVector<QVector<double> > Kalman_coordinates;

    // Current Kalman coordinates
    QVector<double> X_Kalman;
    QVector<double> Y_Kalman;

    // The number of Kalman filters to use (depending on the number
    // of detected objects)
    unsigned N_Kalman_filters;

    // -- Dynamic parameters
    unsigned N_Kalman_dynamic_parameters;
    // -- measurement parameters
    unsigned N_Kalman_measurement_parameters;
    // -- control parameters
    unsigned N_Kalman_control_parameters;

    // Kalman Filter objects, x and y independently
    QVector<CCKalmanFilter*> Kalman_filter_pt;

    // Flag to indicate whether the Kalman has been initialised or not
    QVector<bool> Initialised_Kalman;

signals:

    void finished();

public slots:

    // The method that is run when the "Thread_pt" starts
    void run();

};

#endif // CCPROCESSIMAGETHREAD_H
