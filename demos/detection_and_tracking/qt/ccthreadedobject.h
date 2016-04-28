#ifndef CCTHREADEDOBJECT_H
#define CCTHREADEDOBJECT_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QPixmap>

// We need to include the following two to avoid compilation errors
#include <QApplication>
#include <QDesktopWidget>

// -----------------------------------------------------------------------
// General includes
#include "../../../src/general/general_includes.h"
// -----------------------------------------------------------------------

class CCThreadedObject : public QObject
{
    Q_OBJECT

public:

    // Empty constructor
    explicit CCThreadedObject(QObject *parent = 0);

    // Set the flag to let the thread do its stuff
    inline void let_the_thread_do_its_stuff()
    {Let_the_thread_do_its_stuff = true;}

    inline void interrupt_thread()
    {Let_the_thread_do_its_stuff = false;}

    inline void stop()
    {Stop = true;}

protected:

    // The thread that executes the "run()" method
    QThread Thread;

    // Flag to indicate we have a new image to work with
    bool New_image_ready;

    // Flag to let the thread do its stuff
    bool Let_the_thread_do_its_stuff;

    // Flag to stop the thread and finish
    bool Stop;

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

#endif // CCTHREADEDOBJECT_H
