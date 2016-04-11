#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ======================================================================
    // User interface stuff
    // ======================================================================

    ui->btn_stop->setDisabled(true);
    ui->btn_pause->setDisabled(true);

    // Create connections for mouse event
    connect(ui->label_image, SIGNAL(mouse_pos()), this, SLOT(mouse_pos()));
    connect(ui->label_image, SIGNAL(mouse_pressed()), this, SLOT(mouse_pressed()));

#ifdef T_USE_THREAD
    // Create the thread that will be in charge or "running" the painter object
    painter_thread_pt = new QThread();
    // Create a painter object
    painter_pt = new CCPainter();

    // Setup the thread
    painter_pt->setup_thread(*painter_thread_pt);
    // Pass the ui to the painter
    painter_pt->set_main_window(this);
    // Do the "threading" stuff for the painter
    painter_pt->moveToThread(painter_thread_pt);

    // Indicate the thread has not been started
    Thread_started = false;

    // Do some connections
    //connect(painter_pt, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
#endif // #ifdef T_USE_THREAD

#ifdef T_USE_TIMER
    Timer_started = false;
    Qtimer_pt = new QTimer();
    connect(Qtimer_pt, SIGNAL(timeout()), this, SLOT(paint_image()));
    // Set miliseconds for timer
    Timer_milisecond = 100;

#endif // #ifdef T_USE_TIMER

    // ======================================================================
    // OpenCV stuff
    // ======================================================================
    // Opens and shows an image
    cv::Mat this_image = cv::imread("../data/image-0001.jpg");
    cv::imshow("Display Image", this_image);

    // ======================================================================
    // Image stuff
    // ======================================================================

    // The path for the image
    image_path_pt = new QString("../data/");
    //image_path_pt = new QString("/home/work/local/working/tracking/data/aircraft_tracking/occlusion_1/");
    //image_path_pt = new QString("/home/tachidok/local/working/research/tracking/occlusion_1/");
    // The name of the image
    image_name_pt = new QString("image-");
    // The extention of the image
    image_extention_pt = new QString(".jpg");

    // Set the number of images
    Nimages = 1214;
    // Initialise index image
    Index_image = 0;
    // The increment for the image number
    Increment_image_number = 1;

    // ======================================================================
    // Coordinates history stuff
    // ======================================================================
    const unsigned dim = 2;
    Mouse_coordinates.resize(dim);
    Tracker_coordinates.resize(dim);
    Kalman_coordinates.resize(dim);

    // ======================================================================
    // Mouse stuff
    // ======================================================================
    // Current mouse coordinates initialisation
    X_mouse = 0;
    // Current mouse coordinates initialisation
    Y_mouse = 0;

    // ======================================================================
    // Tracker stuff
    // ======================================================================

    // Current tracker coordinates initialisation
    X_tracker = 0;
    // Current tracker coordinates initialisation
    Y_tracker = 0;

    // The tracker
    //Tracker_pt = new CCSimpleTracker();
    //Tracker_pt = new CCNormalDistTracker();
    //Tracker_pt = new CCOpenCVTracker("MIL");
    Tracker_pt = new CCOpenCVTracker("BOOSTING");
    //Tracker_pt = new CCOpenCVTracker("MEDIANFLOW");
    //Tracker_pt = new CCOpenCVTracker("TLD");
    //Tracker_pt = new CCOpenCVTracker("KCF");

    // By defaul the left mouse button is not pressed
    Left_mouse_button_pressed = false;

    // Maximum and minimum aim size
    Max_aim_size = 15;
    Min_aim_size = 3;
    // Initial aim size
    Aim_size = 3;
    // Increment step for aim size
    Aim_size_increasing_step = 2;

    // ======================================================================
    // Kalman stuff
    // ======================================================================
    // Do not do Kalman by default
    Do_Kalman = false;

    // How many Kalman filters do we want
    N_Kalman_filters = 2;

    // Coordinates (filtered) by Kalman filter
    X_Kalman = 0;
    Y_Kalman = 0;

    // Prepare data for Kalman

    // -- dynamic parameters (2, position and velocity)
    const unsigned n_dynamic_parameters = 2;
    // -- measurement parameters (1, position)
    const unsigned n_measurement_parameters = 1;
    // -- control parameters (1, acceleration)
    const unsigned n_control_parameters = 1;

    // Create as many instances of Kalman filters as required
    Kalman_filter_pt = new CCKalmanFilter*[N_Kalman_filters];

    // Create particular instances of Kalman filter (for X)
    Kalman_filter_pt[0] = new CCKalmanFilter(n_dynamic_parameters,
                                             n_measurement_parameters,
                                             n_control_parameters);

    // Create particular instances of Kalman filter (for Y)
    Kalman_filter_pt[1] = new CCKalmanFilter(n_dynamic_parameters,
                                             n_measurement_parameters,
                                             n_control_parameters);

    // ======================================================================
    // Plotting
    // ======================================================================
    // Create the graph for X coordinates
    ui->wdg_plot_x->addGraph(); // Mouse coordinates
    ui->wdg_plot_x->addGraph(); // Tracker coordinates
    ui->wdg_plot_x->addGraph(); // Kalman coordinates

    // Create the graph for Y coordinates
    ui->wdg_plot_y->addGraph(); // Mouse coordinates
    ui->wdg_plot_y->addGraph(); // Tracker coordinates
    ui->wdg_plot_y->addGraph(); // Kalman coordinates

}

MainWindow::~MainWindow()
{
#ifdef T_USE_THREAD
    delete painter_thread_pt;
    delete painter_pt;
#endif // #ifdef T_USE_THREAD

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


    if (Tracker_pt != 0)
    {
        delete Tracker_pt;
        Tracker_pt = 0;
    }

    if (Qtimer_pt != 0)
    {
        delete Qtimer_pt;
        Qtimer_pt = 0;
    }

    delete ui;

}

#if 1
// Draws an image in the "label" object of the ui
void MainWindow::paint_image()
{
    // ----------------------------------------------------------
    // Set the image filename
    // ----------------------------------------------------------
    QString zeroes;
    if (Index_image < 10)
    {
        zeroes = "000";
    }
    else if (Index_image >= 10 && Index_image < 100)
    {
        zeroes = "00";
    }
    else if (Index_image >= 100 && Index_image < 1000)
    {
        zeroes = "0";
    }
    else if (Index_image >= 1000 && Index_image < 10000)
    {
        zeroes = "";
    }

//    qDebug() << "Image name:" << *image_path_pt << *image_name_pt << zeroes << Index_image << *image_extention_pt;

    // The complete String for the image filename
    QString full_image_name;
    full_image_name.append(image_path_pt);
    full_image_name.append(image_name_pt);
    full_image_name.append(zeroes);
    full_image_name.append(QString::number(Index_image));
    full_image_name.append(image_extention_pt);
    qDebug() << full_image_name;
    ui->lbl_current_image_name->setText(full_image_name);

    // ----------------------------------------------------------
    // Load the image into the image object (we do not use a pointer
    // since this is the only place where we use the image)
    cv::Mat image = cv::imread(full_image_name.toStdString());
    // Qimage_pt->load(full_image_name);
    // Increment the image index
    Index_image+=Increment_image_number;

    // ----------------------------------------------------------
    // Tracking
    // ----------------------------------------------------------

    // We get the pattern from the current image
    if (Left_mouse_button_pressed)
    {
        // Get the coordinates of the tracker
        X_tracker = X_mouse;
        Y_tracker = Y_mouse;
        Tracker_pt->initialise(image, X_tracker, Y_tracker, Aim_size);

        Do_Kalman = true;
        if (Do_Kalman)
        {
            // -------------------------------------------------------
            // Initialise Kalmans since this is a new set of data
            // -------------------------------------------------------
            initialise_kalman();

        }

        Left_mouse_button_pressed = false;

    }

    double equivalence_value = 0;
    // ----------------------------------------------------------
    // Do tracking? Only if there is a pattern to search for ...
    if (Tracker_pt->initialised())
    {
        // Draw search window (If we draw the search window here we can see
        // the position selected by the Tracker inside the search window)
        draw_square(&image, X_tracker, Y_tracker, Aim_size*2, 255, 255, 0);
        Tracker_pt->search_pattern(image, X_tracker, Y_tracker,
                                   Aim_size*2, equivalence_value);

        // Apply Kalman
        if (Do_Kalman)
        {
            apply_kalman();
        }

    }

    // ----------------------------------------------------------
    // Modify the image by adding aims and squares
    // ----------------------------------------------------------
    // Draw the aim based on the mouse coordinates

    // Get the mouse coordinates over the label (image)
    X_mouse = ui->label_image->getX();
    Y_mouse = ui->label_image->getY();
    //qDebug() << X << Y;
    draw_aim(&image, X_mouse, Y_mouse, Aim_size, 255, 0, 0);

    //qDebug() << X << Y;

    // ----------------------------------------------------------
    // Draw the pattern? Only if there is a pattern stored
    if (Tracker_pt->initialised())
    {
        // Draw an square based on the tracking coordinates
        draw_square(&image, X_tracker, Y_tracker, Aim_size, 0, 255, 0);
        ui->lbl_status_tracker->setText(QString("X=%1 Y=%2 EV=%3").arg(X_tracker).arg(Y_tracker).arg(equivalence_value));
        ui->lbl_image_search->setPixmap(ASM::cvMatToQPixmap(*(Tracker_pt->pattern_pt())));
        //ui->lbl_image_search->setPixmap(QPixmap::fromImage(*(Tracker_pt->pattern_pt()))); // tachidok

        if (Do_Kalman)
        {
            // Draw an square based on the predicted Kalman coordinates
            draw_square(&image, X_Kalman, Y_Kalman, Aim_size, 0, 0, 255);
            // Output the Kalman coordinates
            ui->lbl_status_kalman->setText(QString("X=%1 Y=%2").arg(X_Kalman).arg(Y_Kalman));
        }

    }

    // ----------------------------------------------------------
    // Plot and draw image
    // ----------------------------------------------------------

    // Plot
    plot(image.rows, image.cols);

    if (Tracker_pt->initialised() && Do_Kalman)
    {
        // Update the tracker using the Kalman output
        //X_tracker = X_Kalman;
        //Y_tracker = Y_Kalman;
    }

    // Draw the image
    ui->label_image->setPixmap(ASM::cvMatToQPixmap(image));
    //ui->label_image->setPixmap(QPixmap::fromImage(*Qimage_pt)); // tachidok

}

#else

// Draws an image in the "label" object of the ui
void MainWindow::paint_image()
{
    //QPixmap pixmap(QPixmap::grabWindow(QWidget::winId(),0,0,400,320));
    QPixmap pixmap(QPixmap::grabWindow(QApplication::desktop()->winId(),
                                       500, 500, QApplication::desktop()->width(), QApplication::desktop()->height()));
    //QPixmap pixmap(QScreen::grabWindow(QWidget::winId()));

    // Load the pixmap into an image to get access to the data
    QImage image(pixmap.toImage());

    Qimage_pt = &image;

    // Increment the image index
    Index_image+=Increment_image_number;

    // ----------------------------------------------------------
    // Modify the image by adding aims and squares
    // ----------------------------------------------------------
    // Draw the aim based on the mouse coordinates

    // Get the mouse coordinates over the label (image)
    X_mouse = ui->label_image->getX();
    Y_mouse = ui->label_image->getY();
    //qDebug() << X << Y;
    draw_aim(X_mouse, Y_mouse, Aim_size, 255, 0, 0);

    // ----------------------------------------------------------
    // Draw an square based on the tracking coordinates
    // Get the coordinates of the tracker
    X_tracker = X_mouse;
    Y_tracker = Y_mouse;
    //qDebug() << X << Y;
    draw_square(X_tracker, Y_tracker, Aim_size, 255, 255, 0);
    ui->lbl_status_tracker->setText(QString("X=%1 Y=%2").arg(X_tracker).arg(Y_tracker));

    // ----------------------------------------------------------
    // Plot and draw image
    plot();

    // Draw the image
    ui->label_image->setPixmap(QPixmap::fromImage(*Qimage_pt));
}
#endif // #if 0

void MainWindow::mouse_pos()
{
    ui->lbl_status_mouse->setText(QString("X=%1 Y=%2").arg(ui->label_image->getX()).arg(ui->label_image->getY()));
    //qDebug() << ui->label_image->getX() << ui->label_image->getY();
}

void MainWindow::mouse_pressed()
{
    Left_mouse_button_pressed = true;
}


void MainWindow::on_btn_start_clicked()
{
#ifdef T_USE_THREAD
    if (!Thread_started)
    {
        Thread_started = true;
        painter_thread_pt->start();
    }
    //dynamic_cast<QWidget*>(ui->btn_start)->enabled = false;
#endif // #ifdef T_USE_THREAD

#ifdef T_USE_TIMER
    if (!Timer_started)
    {
        Timer_started = true;
        Qtimer_pt->start(Timer_milisecond);
    }
#endif // #ifdef T_USE_TIMER

    ui->btn_start->setDisabled(true);
    ui->btn_stop->setEnabled(true);
    ui->btn_pause->setEnabled(true);

}

void MainWindow::on_btn_stop_clicked()
{

    // Reset the tracker
    if (Tracker_pt != 0)
    {
        Tracker_pt->delete_pattern();
    }

    // Set the value for timer started so other method performs the
    // stopping process
    Timer_started = true;
    // Stop the timer
    on_btn_pause_clicked();
    // Reset the image index variable
    Index_image = 0;

    ui->btn_start->setEnabled(true);
    ui->btn_stop->setDisabled(true);
    ui->btn_pause->setDisabled(true);
    ui->btn_pause->setText("Pause");

}

void MainWindow::on_btn_pause_clicked()
{
#ifdef T_USE_THREAD
    if (Thread_started)
    {
        painter_pt->stop();
        Thread_started = false;
    }
#endif // #ifdef T_USE_THREAD

    if (Timer_started)
    {
        Timer_started = false;
        Qtimer_pt->stop();
        ui->btn_pause->setText("Resume");
    }
    else
    {
        on_btn_start_clicked();
        ui->btn_pause->setText("Pause");
    }

}

void MainWindow::on_label_image_linkHovered(const QString &link)
{

}

// ======================================================================
// Method to draw an aim
// ======================================================================
void MainWindow::draw_aim(cv::Mat *image_pt, const unsigned x, const unsigned y,
                          const unsigned half_size,
                          const unsigned r, const unsigned g, const unsigned b)
{ 
    // The centre of the aim MUST be wihtin the limits of the image
    if (static_cast<int>(x) > image_pt->cols) {return;}
    if (static_cast<int>(y) > image_pt->rows) {return;}

    // Get the channels of the image
    const int channels = image_pt->channels();

    // Get the image size
    const unsigned height = image_pt->rows;
    const unsigned width = image_pt->cols * channels;

    // Set the left, right, lower, and upper limits for drawing
    int left_limit = (x-half_size)*channels;
    int right_limit = (x+half_size)*channels;
    int lower_limit = y-half_size;
    int upper_limit = y+half_size;

    if (left_limit < 0)
    {
        left_limit = 0;
    }
    if (right_limit > static_cast<int>(width))
    {
        right_limit = width;
    }

    if (lower_limit < 0)
    {
        lower_limit = 0;
    }
    if (upper_limit > static_cast<int>(height))
    {
        upper_limit = height;
    }

    // -----------------------------------------
    // Paint horizontal line
    // Get a pointer to the y-row of the image
    unsigned char *p = image_pt->ptr<uchar>(y);
    for (unsigned i = left_limit; static_cast<int>(i) < right_limit; i+=channels)
    {
        p[i+0] = b;
        p[i+1] = g;
        p[i+2] = r;
        //p[i+3] = a; // no alpha value
    }

    // -----------------------------------------
    // Paint vertical line
    const unsigned x_position = x*channels;
    for (unsigned i = lower_limit; static_cast<int>(i) < upper_limit; i++)
    {
        // Get a pointer to the i-row of the image
        unsigned char *p = image_pt->ptr<uchar>(i);
        p[x_position+0] = b;
        p[x_position+1] = g;
        p[x_position+2] = r;
        //p[x_position+3] = a; // no alpha value
    }

}

// ======================================================================
// Method to draw a square
// ======================================================================
void MainWindow::draw_square(cv::Mat *image_pt, const unsigned x, const unsigned y,
                             const unsigned half_size,
                             const unsigned r, const unsigned g, const unsigned b)
{
    // The centre of the aim MUST be wihtin the limits of the image
    if (static_cast<int>(x) > image_pt->cols) {return;}
    if (static_cast<int>(y) > image_pt->rows) {return;}

    // Get the channels of the image
    const int channels = image_pt->channels();

    // Get the image size
    const unsigned height = image_pt->rows;
    const unsigned width = image_pt->cols * channels;

    // Set the left, right, lower, and upper limits for drawing
    int left_limit = (x-half_size)*channels;
    int right_limit = (x+half_size)*channels;
    int lower_limit = y-half_size;
    int upper_limit = y+half_size;

    if (left_limit < 0)
    {
        left_limit = 0;
    }
    if (right_limit > static_cast<int>(width))
    {
        right_limit = width;
    }

    if (lower_limit < 0)
    {
        lower_limit = 0;
    }
    if (upper_limit > static_cast<int>(height))
    {
        upper_limit = height;
    }

    // ---------------------------------------------------
    // Paint horizontal lines
    // Get a pointer to the y-row of the image
    unsigned char *p_lower = image_pt->ptr<uchar>(lower_limit);
    unsigned char *p_upper = image_pt->ptr<uchar>(upper_limit-1);
    for (unsigned i = left_limit; static_cast<int>(i) < right_limit; i+=channels)
    {
        p_lower[i+0] = b;
        p_lower[i+1] = g;
        p_lower[i+2] = r;
        //p_lower[i+3] = a; // no alpha value

        p_upper[i+0] = b;
        p_upper[i+1] = g;
        p_upper[i+2] = r;
        //p_upper[i+3] = a; // no alpha value
    }

    // ---------------------------------------------------
    // Paint vertical lines
    for (unsigned i = lower_limit; static_cast<int>(i) < upper_limit; i++)
    {
        // Get a pointer to the i-row of the image
        unsigned char *p = image_pt->ptr<uchar>(i);
        p[left_limit+0] = b;
        p[left_limit+1] = g;
        p[left_limit+2] = r;
        //p[left_limit+3] = a; // no alpha value

        p[(right_limit-1*channels)+0] = b;
        p[(right_limit-1*channels)+1] = g;
        p[(right_limit-1*channels)+2] = r;
        //p[(right_limit-1)+3] = a; // no alpha value
    }

}

// ======================================================================
// Plot the data
// ======================================================================
void MainWindow::plot(const unsigned y_max, const unsigned x_max)
{
    const unsigned step = 10;

    // Add the current mouse coordinates to the data to plot
    Mouse_coordinates[0].push_back(X_mouse);
    Mouse_coordinates[1].push_back(Y_mouse);

#if 0
    static bool upwards = true;
    if (upwards && Y_Kalman > y_max)
    {
        upwards = false;
    }

    if (!upwards && Y_Kalman == 0)
    {
        upwards = true;
    }

    if (upwards)
        Y_Kalman+=step;
    else
        Y_Kalman-=step;

    //X_Kalman+= delete_counter;
#endif // #if 0

    // Add the current tracker coordinates to the data to plot
    Tracker_coordinates[0].push_back(X_tracker);
    Tracker_coordinates[1].push_back(Y_tracker);

    // Add the current Kalman coordinate to the data to plot
    Kalman_coordinates[0].push_back(X_Kalman);
    Kalman_coordinates[1].push_back(Y_Kalman);

    QVector<double> time_data(Index_image);

    for (unsigned i = 0; i < Index_image; i++)
    {time_data[i] = i;}

    // Assign some data
    // ---------------------------------------------------------------
    // Mouse (RED)
    //ui->wdg_plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->wdg_plot_x->graph(0)->setPen(QPen(QColor(255,0,0)));
    ui->wdg_plot_x->graph(0)->setData(time_data, Mouse_coordinates[0]);
    ui->wdg_plot_y->graph(0)->setPen(QPen(QColor(255,0,0)));
    ui->wdg_plot_y->graph(0)->setData(time_data, Mouse_coordinates[1]);
    // ---------------------------------------------------------------
    // Tracker (GREEN)
    ui->wdg_plot_x->graph(1)->setPen(QPen(QColor(0,255,0)));
    ui->wdg_plot_x->graph(1)->setData(time_data, Tracker_coordinates[0]);
    ui->wdg_plot_y->graph(1)->setPen(QPen(QColor(0,255,0)));
    ui->wdg_plot_y->graph(1)->setData(time_data, Tracker_coordinates[1]);
    // ---------------------------------------------------------------
    // Kalman (BLUE)
    ui->wdg_plot_x->graph(2)->setPen(QPen(QColor(0,0,255)));
    ui->wdg_plot_x->graph(2)->setData(time_data, Kalman_coordinates[0]);
    ui->wdg_plot_y->graph(2)->setPen(QPen(QColor(0,0,255)));
    ui->wdg_plot_y->graph(2)->setData(time_data, Kalman_coordinates[1]);

    // Assign labels to axis
    ui->wdg_plot_x->xAxis->setLabel("Time (t)");
    ui->wdg_plot_x->yAxis->setLabel("x-position");
    ui->wdg_plot_y->xAxis->setLabel("Time (t)");
    ui->wdg_plot_y->yAxis->setLabel("y-position");

    // Set range
    ui->wdg_plot_x->xAxis->setRange(0, Nimages);
    ui->wdg_plot_y->xAxis->setRange(0, Nimages);

    // Get the maximum between the image width and height
    ui->wdg_plot_x->yAxis->setRange(0, x_max);
    ui->wdg_plot_y->yAxis->setRange(0, y_max);

    ui->wdg_plot_x->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->wdg_plot_x->replot();
    ui->wdg_plot_y->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->wdg_plot_y->replot();

}

void MainWindow::on_btn_increase_search_window_clicked()
{
    if (Aim_size+Aim_size_increasing_step <= Max_aim_size)
    {
        Aim_size+=Aim_size_increasing_step;
    }

}

void MainWindow::on_btn_decrease_search_window_clicked()
{
    if (Aim_size-Aim_size_increasing_step >= Min_aim_size)
    {
        Aim_size-=Aim_size_increasing_step;
    }
}

void MainWindow::initialise_kalman()
{
    // Kalman X
    // -------------------------------------------------------
    // Before reset set default covariances for process noise
    // and measurement noise
    const double noise_covariance_Q = 1.0e-2; //1.0e-2
    const double noise_covariance_R = 2.0;    //0.5
    Kalman_filter_pt[0]->noise_covariance_Q() = noise_covariance_Q;
    Kalman_filter_pt[0]->noise_covariance_R() = noise_covariance_R;
    // Reset
    Kalman_filter_pt[0]->reset();

    // Set the initial state of the system x_0
    Kalman_filter_pt[0]->x(0) = X_mouse; // x-position
    Kalman_filter_pt[0]->x(1) = 0.0; // x-pixels per frame

    //std::cerr << "State vector (init)" << std::endl;
    //std::cerr << (*Kalman_filter_pt[0]->x()) << std::endl;

    // Initialise covariance matrix P
    const double dt = 1.0;
    Kalman_filter_pt[0]->P(0, 0) = dt*dt*dt*dt/4.0;
    Kalman_filter_pt[0]->P(0, 1) = dt*dt*dt/3.0;
    Kalman_filter_pt[0]->P(1, 0) = dt*dt*dt/3.0;
    Kalman_filter_pt[0]->P(1, 1) = dt*dt;

    // Initilaise process noise covariance Q
    Kalman_filter_pt[0]->Q(0, 0) = dt*dt*dt*dt/4.0 * noise_covariance_Q;
    Kalman_filter_pt[0]->Q(0, 1) = dt*dt*dt/3.0 * noise_covariance_Q;
    Kalman_filter_pt[0]->Q(1, 0) = dt*dt*dt/3.0 * noise_covariance_Q;
    Kalman_filter_pt[0]->Q(1, 1) = dt*dt * noise_covariance_Q;

    // Initialise transformation matrix H
    Kalman_filter_pt[0]->H(0, 0) = 1.0;
    Kalman_filter_pt[0]->H(0, 1) = 0.0;

    // Set measurement (sensors) noise covariance
    Kalman_filter_pt[0]->R(0, 0) = noise_covariance_R;

    // -------------------------------------------------------
    // Kalman Y
    // -------------------------------------------------------
    // Before reset set default covariances for process noise
    // and measurement noise
    Kalman_filter_pt[1]->noise_covariance_Q() = noise_covariance_Q;
    Kalman_filter_pt[1]->noise_covariance_R() = noise_covariance_R;
    // Reset
    Kalman_filter_pt[1]->reset();

    // Set the initial state of the system x_0
    Kalman_filter_pt[1]->x(0) = Y_mouse; // y-position
    Kalman_filter_pt[1]->x(1) = 0.0; // y-pixels per frame

    // Initialise covariance matrix P
    Kalman_filter_pt[1]->P(0, 0) = dt*dt*dt*dt/4.0;
    Kalman_filter_pt[1]->P(0, 1) = dt*dt*dt/3.0;
    Kalman_filter_pt[1]->P(1, 0) = dt*dt*dt/3.0;
    Kalman_filter_pt[1]->P(1, 1) = dt*dt;

    // Initilaise process noise covariance Q
    Kalman_filter_pt[1]->Q(0, 0) = dt*dt*dt*dt/4.0 * noise_covariance_Q;
    Kalman_filter_pt[1]->Q(0, 1) = dt*dt*dt/3.0 * noise_covariance_Q;
    Kalman_filter_pt[1]->Q(1, 0) = dt*dt*dt/3.0 * noise_covariance_Q;
    Kalman_filter_pt[1]->Q(1, 1) = dt*dt * noise_covariance_Q;

    // Initialise transformation matrix H
    Kalman_filter_pt[1]->H(0, 0) = 1.0;
    Kalman_filter_pt[1]->H(0, 1) = 0.0;

    // Set measurement (sensors) noise covariance
    Kalman_filter_pt[1]->R(0, 0) = noise_covariance_R;

}

void MainWindow::apply_kalman()
{
    // To compute dT
#if 0
    const double precTick = ticks;
    const double ticks = (double) cv::getTickCount();
    const double dT = (ticks - precTick) / cv::getTickFrequency(); //seconds
    F->at<double>(0, 1) =  dT;
#endif // #if 0
    // The dt (one frame per time)
    const double dt = 1.0;

    // -------------------------------------------------------
    // PREDICTION stage
    // -------------------------------------------------------
    // Prepara data for each Kalman and do prediction
    // -------------------------------------------------------

    // -------------------------------------------------------
    // Kalman X
    // -------------------------------------------------------
    // Set the transition matrix
    Kalman_filter_pt[0]->F(0, 0) = 1.0;
    Kalman_filter_pt[0]->F(0, 1) = dt;
    Kalman_filter_pt[0]->F(1, 0) = 0.0;
    Kalman_filter_pt[0]->F(1, 1) = 1.0;

    // Set control matrix
    Kalman_filter_pt[0]->B(0, 0) = dt*dt/2.0;
    Kalman_filter_pt[0]->B(1, 0) = dt;

    // Set input control parameters
    // Acceleration
    const double a_x = 0.05; // pixels per frame
    Kalman_filter_pt[0]->u(0) = a_x;

    // -------------------------------------------------------
    // Do prediction (Kalman filter)
    // -------------------------------------------------------
    cv::Mat prediction_x = Kalman_filter_pt[0]->predict();

    // We can use this prediction in case we have no input
    // from the sensor
    // The prediction given by the model
    const double this_x = prediction_x.at<double>(0, 0);
    qDebug() << "KalmanPrediction X ("<< prediction_x.rows
             << ", " << prediction_x.cols << "): ";
    std::cerr << prediction_x << std::endl;

    // -------------------------------------------------------
    // Kalman Y
    // -------------------------------------------------------
    // Set the transition matrix
    Kalman_filter_pt[1]->F(0, 0) = 1.0;
    Kalman_filter_pt[1]->F(0, 1) = dt;
    Kalman_filter_pt[1]->F(1, 0) = 0.0;
    Kalman_filter_pt[1]->F(1, 1) = 1.0;

    // Set control matrix
    Kalman_filter_pt[1]->B(0, 0) = dt*dt/2.0;
    Kalman_filter_pt[1]->B(1, 0) = dt;

    // Set input control parameters
    // Acceleration
    const double a_y = 0.05; // pixels per frame
    Kalman_filter_pt[1]->u(0) = a_y;

    // -------------------------------------------------
    // Do prediction (Kalman filter)
    // -------------------------------------------------
    cv::Mat prediction_y = Kalman_filter_pt[1]->predict();

    // We can use this prediction in case we have no input
    // from the sensor
    // The prediction given by the model
    const double this_y = prediction_y.at<double>(0, 0);
    qDebug() << "KalmanPrediction Y ("<< prediction_y.rows
             << ", " << prediction_y.cols << "): ";
    std::cerr << prediction_y << std::endl;

    // -------------------------------------------------
    // UPDATE stage
    // -------------------------------------------------
    // Use the output of the tracker as the measurement
    // of the sensor
    // -------------------------------------------------
    // Set the measured data for each Kalman
    // -------------------------------------------------
    // Kalman X
    // -------------------------------------------------
    Kalman_filter_pt[0]->z(0) = X_tracker;
    //Kalman_filter_pt[0]->z(0) = X_mouse;
    // Do update (Kalman filter)
    cv::Mat updated_state_x = Kalman_filter_pt[0]->update();

    // -------------------------------------------------
    // Kalman Y
    // -------------------------------------------------
    Kalman_filter_pt[1]->z(0) = Y_tracker;
    //Kalman_filter_pt[1]->z(0) = Y_mouse;
    // Do update (Kalman filter)
    cv::Mat updated_state_y = Kalman_filter_pt[1]->update();

    // -------------------------------------------------
    // Set variables for ploting and output
    // Set the updated state into the X-variable
    X_Kalman = updated_state_x.at<double>(0, 0);
    // Set the updated state into the Y-variable
    Y_Kalman = updated_state_y.at<double>(0, 0);

}
