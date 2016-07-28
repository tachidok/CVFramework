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
//#include "ccprocessimagethread.h"
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
    CCProcessImageFromFile *process_image_from_file;

private slots:
    void on_btn_get_image_clicked();
};

#endif // MAINWINDOW_H
