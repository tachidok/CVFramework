#include "ccthreadedobject.h"

// -------------------------------------------------------------------
// Empty constructor
// -------------------------------------------------------------------
CCThreadedObject::CCThreadedObject(QObject *parent) : QObject(parent)
{

    // The thread is run by default
    Stop = false;

    // Initialise state variables
    New_image_ready = false;

    // The thread does not do its stuff immediately
    Let_the_thread_do_its_stuff = false;

    // Connect the signal from the thread to the slots of this
    // object
    connect(&Thread, SIGNAL(started()), this, SLOT(run()));
    connect(this, SIGNAL(stop()), &Thread, SLOT(quit()));
    connect(&Thread, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(&Thread, SIGNAL(finished()), &Thread, SLOT(deleteLater()));
    connect(this, SIGNAL(stop()), this, SLOT(deleteLater()));

    // Move to thread
    moveToThread(&Thread);

    // Start it up!!!
    Thread.start();
}

bool CCThreadedObject::capture_screen(cv::Mat &image)
{
    //QPixmap pixmap(QPixmap::grabWindow(QWidget::winId(),0,0,400,320));
    //QPixmap pixmap(QPixmap::grabWindow(QApplication::desktop()->winId(),
    //                                   500, 500, QApplication::desktop()->width(), QApplication::desktop()->height()));
    //QPixmap pixmap(QPixmap::grabWindow(QWidget::winId()));

    // Capture the right-upper region of the desktop
    int width_capture = 640;//160;//640;
    int height_capture = 480;//120;//480;
    int desktop_width = QApplication::desktop()->width();
    int desktop_height = QApplication::desktop()->height();
    QPixmap pixmap(QPixmap::grabWindow(QApplication::desktop()->winId(),
                                       desktop_width - width_capture,
                                       0,
                                       width_capture, height_capture));

    // Free any content in the image
    image.release();

    // The image, Transfrom from QPixmap to cv::Mat ('false' argument for
    // not cloning data into cv::Mat)
    image = ASM::QPixmapToCvMat(pixmap);

    return true;

}

// -------------------------------------------------------------------
// The method that is run when the "Thread_pt" starts
// -------------------------------------------------------------------
void CCThreadedObject::run()
{

    unsigned i = 0;

    // Repeat until not stop received
    while(!Stop)
    {

        if (Let_the_thread_do_its_stuff)
        {

            // Before capturing set the flag to indicate there is no image
            // available
            New_image_ready = false;
            // Capture screen
            cv::Mat live_image;
            New_image_ready = capture_screen(live_image);

            if (New_image_ready)
            {
                qDebug() << "New image ready [" << i++ <<"]";

                // Resize the input image and output it in the same image
                cv::resize(live_image, live_image, cv::Size(640, 480));
                cv::namedWindow("Captured-video");
                cv::imshow("Captured-video", live_image);

                // The new image has been worked out
                New_image_ready = false;
            }

        }

    }

    emit finished();

}
