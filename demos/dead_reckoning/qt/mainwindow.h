#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "my_mouse_label.h"

// ===================================================
// My additional classes
// ===================================================
// OpenCV includes
// ===================================================
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

// Kalman Filter
#include "../../../src/kalman/cckalmanfilter.h"
#include "cckalmanfilter1ddeadreckoning.h"

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
    void plot(const unsigned y_max, const unsigned x_max);

public slots:

    // Called method when the main timer timed out!
    void main_timer_timeout();

private:
    Ui::MainWindow *ui;

    // -------------------------------------------------------------------
    // Timer stuff
    // -------------------------------------------------------------------
    QTimer *qmain_timer_pt;
    // Flag to indicate whether the main timer has started
    bool Main_timer_started;

    // Main timer milisecons
    unsigned Main_timer_miliseconds;

    // -------------------------------------------------------------------
    // Mouse and Kalman coordinates stuff
    // -------------------------------------------------------------------

    // Mouse presed flag
    bool Mouse_pressed_flag;

    // History of mouse and Kalman coordinates
    QVector<QVector<double> > Mouse_coordinates;
    QVector<QVector<double> > Kalman_coordinates;

    // Global mouse and Kalman coordinates
    int X_mouse;
    int Y_mouse;
    int X_Kalman;
    int Y_Kalman;

    // Previous coordinates for mouse
    int X_mouse_previous;
    int Y_mouse_previous;

    // The number of Kalman filter to use
    unsigned N_Kalman_filters;

    // Kalman Filter objects, x and y independently
    CCKalmanFilter **Kalman_filter_pt;

    // Flag to indicate whether Kalman is initialised or not
    bool Initialised_Kalman;

    // -------------------------------------------------------------------
    // Plot stuff
    // -------------------------------------------------------------------
    unsigned Global_counter_for_plot;

    // -------------------------------------------------------------------
    // Image stuff
    // -------------------------------------------------------------------
    cv::Mat *Blank_image_pt;

private slots:

    // Mouse events
    void mouse_pos();
    void mouse_pressed();

    void on_btnstart_main_timer_clicked();
    void on_btn_stop_main_timer_clicked();
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

   // If inImage exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inImage's
   // data with the cv::Mat directly
   //    NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
   inline cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true )
   {
       switch ( inImage.format() )
       {
       // 8-bit, 4 channel
       case QImage::Format_RGB32:
       {
           cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC4, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );

           return (inCloneImageData ? mat.clone() : mat);
       }

       // 8-bit, 3 channel
       case QImage::Format_RGB888:
       {
           if ( !inCloneImageData )
               qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";

           QImage   swapped = inImage.rgbSwapped();

           return cv::Mat( swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine() ).clone();
       }

       // 8-bit, 1 channel
       case QImage::Format_Indexed8:
       {
           cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC1, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );

           return (inCloneImageData ? mat.clone() : mat);
       }

       default:
           qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
           break;
       }

       return cv::Mat();
   }

   // If inPixmap exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inPixmap's data
   // with the cv::Mat directly
   //    NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
   inline cv::Mat QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData = true )
   {
       return QImageToCvMat( inPixmap.toImage(), inCloneImageData );
   }

}

#endif // MAINWINDOW_H
