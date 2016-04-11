#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

// ==============================================
// My includes
// ==============================================
// Additional Qt includes
#include <QDebug>
// To capture clicks from mouse over label (where the image is shown)
#include "extended_label.h"

//#define T_USE_THREAD
#ifdef T_USE_THREAD
//#include <QThread>
//#include "ccpainter.h"
#endif // #ifdef T_USE_THREAD

#define T_USE_TIMER
#ifdef T_USE_TIMER
#include <QTimer>
#endif // #define T_USE_TIMER

// ==============================================
// My classes
// ==============================================
// Trackers
#include "../../../src/tracker/catracker.h"
#include "../../../src/tracker/ccsimpletracker.h"
#include "../../../src/tracker/ccnormaldisttracker.h"

// Kalman Filter
#include "../../../src/kalman/cckalmanfilter.h"

// OpenCV includes
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    // Draws an image in the "label" object of the ui
    void paint_image();

private slots:

    // Mouse events
    void mouse_pos();

    void mouse_pressed();

    void on_btn_start_clicked();

    void on_btn_stop_clicked();

    void on_btn_pause_clicked();

    void on_label_image_linkHovered(const QString &link);

    void on_btn_increase_search_window_clicked();

    void on_btn_decrease_search_window_clicked();

private:
    Ui::MainWindow *ui;

protected:

#ifdef T_USE_THREAD
    // Create the thread that will be in charge or "running" the painter object
    QThread *painter_thread_pt;
    // Create a painter object
    CCPainter *painter_pt;

    // Flag to indicate whether the thread has been already started
    bool Thread_started;
#endif // #ifdef T_USE_THREAD

#ifdef T_USE_TIMER
    QTimer *Qtimer_pt;
    // Flag to indicate whether the timer has been already started
    bool Timer_started;

    // Timer milisecons
    unsigned Timer_milisecond;

#endif // #ifdef T_USE_TIMER

    // ======================================================================
    // Image stuff
    // ======================================================================

    // The path for the directory where the images are stored
    QString *image_path_pt;
    // The name of the image
    QString *image_name_pt;
    // The extention of the image
    QString *image_extention_pt;

    // Total number of images
    unsigned Nimages;
    // The index of the image
    unsigned Index_image;
    // The increment for the index image
    unsigned Increment_image_number;

    // ======================================================================
    // Mouse stuff
    // ======================================================================
    // Current mouse coordinates
    unsigned X_mouse;
    unsigned Y_mouse;
    // History of mouse coordinate
    QVector<QVector<double> > Mouse_coordinates;

    // ======================================================================
    // Tracker stuff
    // ======================================================================
    // Current tracker coordinates
    unsigned X_tracker;
    unsigned Y_tracker;
    // History of tracker coordinate
    QVector<QVector<double> > Tracker_coordinates;

    // Flag to indicate whether the mouse was pressed to get a new pattern (we
    // need to do it this way, otherwise we get a cross -- from the aim -- when
    // getting the pattern because we use the image_pt stored in memory, which
    // is the one with the aim painted over it)
    bool Left_mouse_button_pressed;
    // The tracker object
    CATracker *Tracker_pt;

    // ======================================================================
    // Kalman stuff
    // ======================================================================
    // Do Kalman
    bool Do_Kalman;

    // How many Kalman filters do we want?
    unsigned N_Kalman_filters;

    // Current Kalman coordinates
    unsigned X_Kalman;
    unsigned Y_Kalman;

    // History of Kalman coordinate
    QVector<QVector<double> > Kalman_coordinates;

    // Kalman Filter objects, x and y independently
    CCKalmanFilter **Kalman_filter_pt;

    // ======================================================================
    // Aim stuff
    // ======================================================================

    // Aim size (this has to be an odd number)
    unsigned Aim_size;
    // Aim size increasing step
    unsigned Aim_size_increasing_step;

    // Max aim size
    unsigned Max_aim_size;
    // Min aim size
    unsigned Min_aim_size;

    // Method to draw an aim
    void draw_aim(cv::Mat *image_pt, const unsigned x, const unsigned y,
                  const unsigned half_size,
                  const unsigned r, const unsigned g, const unsigned b);

    // Draw a square
    void draw_square(cv::Mat *image_pt, const unsigned x, const unsigned y,
                     const unsigned half_size,
                     const unsigned r, const unsigned g, const unsigned b);

    // ======================================================================
    // Plot stuff
    // ======================================================================

    // Plot the data
    void plot(const unsigned x_max, const unsigned y_max);

};

namespace ASM {
   inline QImage  cvMatToQImage( const cv::Mat &inMat )
   {
      switch ( inMat.type() )
      {
         // 8-bit, 4 channel
         case CV_8UC4:
         {
            QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32 );

            return image;
         }

         // 8-bit, 3 channel
         case CV_8UC3:
         {
            QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888 );

            return image.rgbSwapped();
         }

         // 8-bit, 1 channel
         case CV_8UC1:
         {
            static QVector<QRgb>  sColorTable;

            // only create our color table once
            if ( sColorTable.isEmpty() )
            {
               for ( int i = 0; i < 256; ++i )
                  sColorTable.push_back( qRgb( i, i, i ) );
            }

            QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8 );

            image.setColorTable( sColorTable );

            return image;
         }

         default:
            qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
            break;
      }

      return QImage();
   }

   inline QPixmap cvMatToQPixmap( const cv::Mat &inMat )
   {
      return QPixmap::fromImage( cvMatToQImage( inMat ) );
   }

}

#endif // MAINWINDOW_H
