#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QThread>

// -----------------------------------------------------------------------
// My includes
// -----------------------------------------------------------------------
// General includes
#include "../../../src/general/general_includes.h"
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
// A thread object
// -----------------------------------------------------------------------
#include "ccthreadedobject.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // -------------------------------------------------------------------
    // Plot stuff
    // -------------------------------------------------------------------
    void plot(const unsigned x_max, const unsigned y_max);

#if 0
    // -------------------------------------------------------------------
    // Capture screen stuff
    // -------------------------------------------------------------------
    void capture_screen();
#endif

private slots:
    void on_btn_start_capture_screen_clicked();

    void on_btn_stop_capture_screen_clicked();

private:

    Ui::MainWindow *ui;

    // -------------------------------------------------------------------
    // Capture screen stuff
    // -------------------------------------------------------------------
    bool Capture_screen;

    // -------------------------------------------------------------------
    // Plot stuff
    // -------------------------------------------------------------------
    unsigned Global_counter_for_plot;

    // -------------------------------------------------------------------
    // Tracker stuff
    // -------------------------------------------------------------------
    // History of Tracker coordinates
    QVector<QVector<double> > Tracker_coordinates;

    // Current tracker coordinates
    int X_tracker;
    int Y_tracker;

    // The number of trackers to use (depending on the number
    // of detected objects)
    unsigned N_trackers;

    // The tracker objects
    CATracker **Tracker_pt;

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
    // Kalman stuff
    // -------------------------------------------------------------------
    // History of Kalman coordinates
    QVector<QVector<double> > Kalman_coordinates;

    // Current Kalman coordinates
    int X_Kalman;
    int Y_Kalman;

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
    CCKalmanFilter **Kalman_filter_pt;

    // Flag to indicate whether the Kalman has been initialised or not
    QVector<bool> Initialised_Kalman;

    // ---------------------------------------------------------------
    // Thread stuff
    // ---------------------------------------------------------------
    // The thread in charge of displaying the image
    CCThreadedObject *Image_thread_pt;

};

#endif // MAINWINDOW_H
