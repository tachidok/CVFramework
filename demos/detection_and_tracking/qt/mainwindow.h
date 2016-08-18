#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QThread>

// -----------------------------------------------------------------------
// My includes
// -----------------------------------------------------------------------
// Screen capture thread
// -----------------------------------------------------------------------
#include "../../../src/video_capture/cc_capture_from_screen.h"
// -----------------------------------------------------------------------
// Process image thread
// -----------------------------------------------------------------------
#include "ccprocessimagethread.h"
// -----------------------------------------------------------------------

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

private slots:
    void on_btn_start_capture_screen_clicked();

    void on_btn_stop_capture_screen_clicked();

    void on_btn_start_image_processing_clicked();

    void on_btn_stop_image_processing_clicked();

    void on_btn_plus_pattern_window_clicked();

    void on_btn_minus_pattern_window_clicked();

private:

    Ui::MainWindow *ui;

    // -------------------------------------------------------------------
    // Plot stuff
    // -------------------------------------------------------------------
    unsigned Global_counter_for_plot;

    // ---------------------------------------------------------------
    // Thread stuff
    // ---------------------------------------------------------------
    // The thread in charge of capturing and displaying the image
    CCCaptureFromScreen *Capture_image_from_screen_thread_pt;

    // The thread in charge of processing the image
    CCProcessImageThread *Process_image_thread_pt;

    // ---------------------------------------------------------------
    // Image stuff
    // ---------------------------------------------------------------
    // The storage for the captured image
    cv::Mat *Image_pt;

};

#endif // MAINWINDOW_H
