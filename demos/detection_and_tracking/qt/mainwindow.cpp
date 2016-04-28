#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // -------------------------------------------------------------------
    // Capture screen stuff
    // -------------------------------------------------------------------
    Capture_screen = false;

    // -------------------------------------------------------------------
    // Coordinates history stuff
    // -------------------------------------------------------------------
    const unsigned dim = 2;
    Tracker_coordinates.resize(dim);
    Kalman_coordinates.resize(dim);

    // -------------------------------------------------------------------
    // Tracker stuff
    // -------------------------------------------------------------------
    N_trackers = 0;

    // -------------------------------------------------------------------
    // --- More tracker stuff
    // -------------------------------------------------------------------

    // Pattern window size
    Pattern_window_size = 21;
    // Search window size
    Search_window_size = Pattern_window_size * 2;
    // Pattern size window increasing step
    Pattern_window_size_increasing_step = 2;

    // Max pattern window size
    Max_pattern_window_size = 31;
    // Min pattern window size
    Min_pattern_window_size = 11;

    // -------------------------------------------------------------------
    // Kalman stuff
    // -------------------------------------------------------------------
    N_Kalman_filters = 0;

    // Prepare data for Kalman
    // -- Dynamic parameters (2, position and velocity)
    N_Kalman_dynamic_parameters = 2;
    // -- measurement parameters (2, position and velocity)
    N_Kalman_measurement_parameters = 2;
    // -- control parameters (1, acceleration)
    N_Kalman_control_parameters = 1;

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
    // Create the thread to display image
    Image_thread_pt = new CCThreadedObject();

}

MainWindow::~MainWindow()
{

    // Stop thread
    Image_thread_pt->stop();

    // Free memory for thread object
    delete Image_thread_pt;
    Image_thread_pt = 0;

    if (N_Kalman_filters > 0)
    {
        // Delete each instance of Kalman filters
        for (unsigned i = 0; i < N_Kalman_filters; i++)
        {
            if (Kalman_filter_pt[i] != 0)
            {
                delete Kalman_filter_pt[i];
                Kalman_filter_pt[i] = 0;
            }
        }

        // Delete the array of Kalman filters
        delete [] Kalman_filter_pt;

    }

    if (N_trackers > 0)
    {
        // Delete each instance of tracker
        for (unsigned i = 0; i < N_trackers; i++)
        {
            if (Tracker_pt[i] != 0)
            {
                delete Tracker_pt[i];
                Tracker_pt[i] = 0;
            }
        }

        // Delete the array of trackers
        delete [] Tracker_pt;

    }

    delete ui;
}

// ======================================================================
// Plot the data
// ======================================================================
void MainWindow::plot(const unsigned x_max, const unsigned y_max)
{
    // Max. range in x
    const unsigned range_in_x = 10000;

    // Add the current tracker coordinates to the data to plot
    Tracker_coordinates[0].push_back(X_tracker);
    Tracker_coordinates[1].push_back(Y_tracker);

    // Add the current Kalman coordinate to the data to plot
    Kalman_coordinates[0].push_back(X_Kalman);
    Kalman_coordinates[1].push_back(Y_Kalman);

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
    ui->wdg_plot_x->graph(0)->setData(time_data, Tracker_coordinates[0]);
    // ---------------------------------------------------------------
    // Kalman (BLUE)
    ui->wdg_plot_x->graph(1)->setPen(QPen(QColor(0,0,255)));
    ui->wdg_plot_x->graph(1)->setData(time_data, Kalman_coordinates[0]);

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
    ui->wdg_plot_y->graph(0)->setData(time_data, Tracker_coordinates[1]);
    // ---------------------------------------------------------------
    // Kalman (BLUE)
    ui->wdg_plot_y->graph(1)->setPen(QPen(QColor(0,0,255)));
    ui->wdg_plot_y->graph(1)->setData(time_data, Kalman_coordinates[1]);

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

#if 0
void MainWindow::capture_screen()
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

    // The image, Transfrom from QPixmap to cv::Mat ('false' argument for
    // not cloning data into cv::Mat)
    cv::Mat live_image = ASM::QPixmapToCvMat(pixmap);// false);

    Image_thread_pt->set_new_image(live_image);

#if 0
    // Resize the input image and output it in the same image
    //cv::resize(live_image, live_image, cv::Size(640, 480));
    cv::namedWindow("Live-video");
    cv::imshow("Live-video", live_image);
#endif

    //ui->lbl_global_status->setText("<font color='red'>ERROR: Getting image</font>");
    //return;
}
#endif

// ======================================================================
// Method to draw an aim
// ======================================================================
void MainWindow::draw_aim(cv::Mat *image_pt, const unsigned x,
                          const unsigned y, const unsigned half_size,
                          const unsigned r, const unsigned g, const unsigned b,
                          const unsigned thickness,
                          const unsigned type_line)
{
    // The centre of the aim MUST be wihtin the limits of the image
    if (static_cast<int>(x) > image_pt->cols) {return;}
    if (static_cast<int>(y) > image_pt->rows) {return;}

    // Horizontal line
    cv::line(*image_pt, cv::Point(x-half_size, y), cv::Point(x+half_size, y),
             cv::Scalar(b, g, r), thickness);
    // Vertical line
    cv::line(*image_pt, cv::Point(x, y-half_size), cv::Point(x, y+half_size),
             cv::Scalar(b, g, r), thickness, type_line);
}

// ======================================================================
// Method to draw a square
// ======================================================================
void MainWindow::draw_square(cv::Mat *image_pt, const unsigned x,
                             const unsigned y, const unsigned half_size,
                             const unsigned r, const unsigned g, const unsigned b,
                             const unsigned thickness,
                             const unsigned type_line)
{
    // The centre of the aim MUST be wihtin the limits of the image
    if (static_cast<int>(x) > image_pt->cols) {return;}
    if (static_cast<int>(y) > image_pt->rows) {return;}

    // Create a Rect object (integer due to no sub-pixels)
    cv::Rect draw_rect(x-half_size, y-half_size, half_size*2, half_size*2);
    cv::rectangle(*image_pt, draw_rect, cv::Scalar(b, g, r), thickness,
                  type_line);
}

void MainWindow::on_btn_start_capture_screen_clicked()
{
    // Set the flag to indicate to capture the screen
    Capture_screen = true;
    Image_thread_pt->let_the_thread_do_its_stuff();

    //while(Capture_screen)
    {
        //capture_screen();
    }

}

void MainWindow::on_btn_stop_capture_screen_clicked()
{

    Image_thread_pt->interrupt_thread();
    // Stop capture screen
    Capture_screen = false;
}
