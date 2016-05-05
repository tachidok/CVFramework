#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // -------------------------------------------------------------------
    // Plot stuff
    // -------------------------------------------------------------------
    // Initialise the global counter
    Global_counter_for_plot = 0;

    // Create the graph for X coordinates
    ui->wdg_plot_x->addGraph(); // Tracker coordinates
    ui->wdg_plot_x->addGraph(); // Kalman coordinates

    // Create the graph for Y coordinates
    ui->wdg_plot_y->addGraph(); // Tracker coordinates
    ui->wdg_plot_y->addGraph(); // Kalman coordinates

    // ---------------------------------------------------------------
    // Thread stuff
    // ---------------------------------------------------------------
    // Initialise and configure threads
    // ---------------------------------------------------------------

    // ---------------------------------------------------------------
    // Create the thread to capture and display the image
    Capture_image_thread_pt = new CCCaptureThread();

    // Set the capture geometry (region of interest or ROI)
    int width_capture = 640;//160;//640;
    int height_capture = 480;//120;//480;
    int desktop_width = QApplication::desktop()->width();
    int desktop_height = QApplication::desktop()->height();
    Capture_image_thread_pt->
            set_capture_geometry(desktop_width - width_capture - 1, 0,
                                 width_capture, height_capture);

    // Do not show the captured image
    Capture_image_thread_pt->disable_image_displaying();

    // ---------------------------------------------------------------
    // Initialise the image processer
    Process_image_thread_pt = new CCProcessImageThread();

#if 0
    // Get a pointer to the image in the processing thread
    cv::Mat *process_image_pt = Process_image_thread_pt->image_pt();

    // Register the image pointer to store the image there
    Capture_image_thread_pt->register_image_pointer(process_image_pt);
#endif

    // Now set a pointer to the capture thread in the process image thread
    // so it can ask for a new image
    Process_image_thread_pt->set_capture_thread_pt(Capture_image_thread_pt);

    // Get the mutex from the screen capturer and set it to the
    // image processor
    Process_image_thread_pt->
            set_mutex_pt(&(Capture_image_thread_pt->mutex_capturing_image()));

    // ---------------------------------------------------------------
    // Start the threads
    // ---------------------------------------------------------------
    Capture_image_thread_pt->start_thread();
    Process_image_thread_pt->start_thread();

}

MainWindow::~MainWindow()
{

    // Stop process image thread
    Process_image_thread_pt->stop();

    // Free memory for processing image thread
    delete Process_image_thread_pt;
    Process_image_thread_pt = 0;

    // Stop capturing thread
    Capture_image_thread_pt->stop();

    // Free memory for capturing thread
    delete Capture_image_thread_pt;
    Capture_image_thread_pt = 0;

    delete ui;
}

// ======================================================================
// Plot the data
// ======================================================================
void MainWindow::plot(const unsigned x_max, const unsigned y_max)
{
    // Max. range in x
    const unsigned range_in_x = 10000;

    // Get access to the tracker and Kalman coordinates of the first
    // object only
    QVector<QVector<double> > tracker_coord =
            Process_image_thread_pt->tracker_coordinates(0);

    QVector<QVector<double> > kalman_coord =
            Process_image_thread_pt->kalman_coordinates(0);

    // Add the current tracker coordinates to the data to plot (if
    // there are some)
    if (Process_image_thread_pt->n_tracker() > 0)
    {
        tracker_coord[0].push_back(Process_image_thread_pt->x_tracker(0));
        tracker_coord[1].push_back(Process_image_thread_pt->y_tracker(0));
    }

    // Add the current Kalman coordinate to the data to plot (if
    // there are some)
    if (Process_image_thread_pt->n_kalman_filters())
    {
        kalman_coord[0].push_back(Process_image_thread_pt->x_kalman(0));
        kalman_coord[1].push_back(Process_image_thread_pt->y_kalman(0));
    }

    QVector<double> time_data(Global_counter_for_plot);

    for (unsigned i = 0; i < Global_counter_for_plot; i++)
    {time_data[i] = i;}

    // ---------------------------------------------------------------
    // X PLOT
    // ---------------------------------------------------------------
    // Assign data to plot
    // ---------------------------------------------------------------
    // Tracker (GREEN)
    ui->wdg_plot_x->graph(0)->setPen(QPen(QColor(0,255,0)));
    ui->wdg_plot_x->graph(0)->setData(time_data, tracker_coord[0]);
    // ---------------------------------------------------------------
    // Kalman (BLUE)
    ui->wdg_plot_x->graph(1)->setPen(QPen(QColor(0,0,255)));
    ui->wdg_plot_x->graph(1)->setData(time_data, kalman_coord[0]);

    // Assign labels to axis
    ui->wdg_plot_x->xAxis->setLabel("time");
    ui->wdg_plot_x->yAxis->setLabel("x-position");

    // Set range
    ui->wdg_plot_x->xAxis->setRange(0, range_in_x);
    ui->wdg_plot_x->yAxis->setRange(0, x_max);

    ui->wdg_plot_x->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->wdg_plot_x->replot();

    // ---------------------------------------------------------------
    // Y PLOT
    // ---------------------------------------------------------------
    // Assign data to plot
    // ---------------------------------------------------------------
    // Tracker (GREEN)
    ui->wdg_plot_y->graph(0)->setPen(QPen(QColor(0,255,0)));
    ui->wdg_plot_y->graph(0)->setData(time_data, tracker_coord[1]);
    // ---------------------------------------------------------------
    // Kalman (BLUE)
    ui->wdg_plot_y->graph(1)->setPen(QPen(QColor(0,0,255)));
    ui->wdg_plot_y->graph(1)->setData(time_data, kalman_coord[1]);

    // Assign labels to axis
    ui->wdg_plot_y->xAxis->setLabel("time");
    ui->wdg_plot_y->yAxis->setLabel("y-position");

    // Set range
    ui->wdg_plot_y->xAxis->setRange(0, range_in_x);
    ui->wdg_plot_y->yAxis->setRange(0, y_max);

    ui->wdg_plot_y->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->wdg_plot_y->replot();

    // ---------------------------------------------------------------
    // Increase the global counter
    // ---------------------------------------------------------------
    Global_counter_for_plot++;

}

void MainWindow::on_btn_start_capture_screen_clicked()
{
    Capture_image_thread_pt->start_capturing();
}

void MainWindow::on_btn_stop_capture_screen_clicked()
{
    Capture_image_thread_pt->interrupt_capturing();
}

void MainWindow::on_btn_start_image_processing_clicked()
{
    Process_image_thread_pt->start_image_processing();
}

void MainWindow::on_btn_stop_image_processing_clicked()
{
    Process_image_thread_pt->interrupt_image_processing();
}

void MainWindow::on_btn_plus_pattern_window_clicked()
{
    Process_image_thread_pt->plus_pattern_window();
}

void MainWindow::on_btn_minus_pattern_window_clicked()
{
        Process_image_thread_pt->minus_pattern_window();
}
