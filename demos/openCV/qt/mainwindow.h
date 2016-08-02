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
#include "../../../src/screen_capture/cccapturethread.h"
// -----------------------------------------------------------------------
// Process image
// -----------------------------------------------------------------------
#include "ccprocessimagefromfile.h"
#include "ccprocessimagefromscreen.h"
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// General includes
#include "../../../src/general/general_includes.h"
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

private:
    Ui::MainWindow *ui;

protected:

    // ---------------------------------------------------------------
    // Image stuff
    // ---------------------------------------------------------------
    // Where the image is taken from: file, camera or screen.
    bool Image_from_file;
    bool Image_from_camera;
    bool Image_from_screen;

    // Objects to process images
    CCProcessImageFromFile *Process_image_from_file;

    // Kernel size or "mask" for filters
    unsigned Filter_kernel_size;

    // Contrast and brightness global variables (ugh!)
    double Contrast;
    double Brightness;

    // ---------------------------------------------------------------
    // Thread stuff
    // ---------------------------------------------------------------
    // The thread in charge of capturing and displaying the image
    CCCaptureThread *Capture_image_thread_pt;
    // The thread in charge of "eating" and processing images
    CCProcessImageFromScreen *Process_image_from_screen_thread_pt;

private slots:
    void on_btn_get_image_clicked();
    void on_bnt_clear_clicked();
    void on_btn_filter_normalised_clicked();
    void on_btn_kernel_size_plus_clicked();
    void on_btn_kernel_size_minus_clicked();
    void on_btn_filter_gauss_clicked();
    void on_btn_filter_median_clicked();
    void on_sld_contrast_sliderMoved(int position);
    void on_sld_brightness_sliderMoved(int position);
    void on_sld_contrast_valueChanged(int value);
    void on_bnt_histogram_equalize_clicked();
    void on_btn_zoom_up_clicked();
    void on_btn_zoom_down_clicked();
    void on_btn_plus45_clicked();
    void on_horizontalSlider_sliderMoved(int position);
};

#endif // MAINWINDOW_H
