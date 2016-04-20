#include "mainwindow.h"

// To capture mouse events
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->lbl_mouse, SIGNAL(mouse_pos()), this, SLOT(mouse_pos()));
    connect(ui->lbl_mouse, SIGNAL(left_mouse_pressed()), this, SLOT(left_mouse_pressed()));
    connect(ui->lbl_mouse, SIGNAL(right_mouse_pressed()), this, SLOT(right_mouse_pressed()));

    // -------------------------------------------------------------------
    // Timer stuff
    // -------------------------------------------------------------------
    Main_timer_started = false;
    qmain_timer_pt = new QTimer();
    connect(qmain_timer_pt, SIGNAL(timeout()), this, SLOT(main_timer_timeout()));
    // Set miliseconds for timer
    Main_timer_miliseconds = 25;

    // -------------------------------------------------------------------
    // Mouse and Kalman coordinates stuff
    // -------------------------------------------------------------------
    const unsigned dim = 2;
    Mouse_coordinates.resize(dim);
    Kalman_coordinates.resize(dim);

    // Initialise coordinates
    X_mouse = 0;
    Y_mouse = 0;
    X_Kalman = 0;
    Y_Kalman = 0;

    // Initialise velocities
    X_vel = 0.0;
    Y_vel = 0.0;

    // Initialise flag
    Left_mouse_pressed_flag = false;
    Right_mouse_pressed_flag = true;

    // The position by the sensor is given by default
    Position_by_sensor = true;

    // -------------------------------------------------------------------
    // Kalman stuff
    // -------------------------------------------------------------------
    // How many Kalman filters do we want (one per each coordinate)
    N_Kalman_filters = 2;

    // Prepare data for Kalman

    // -- dynamic parameters (2, position and velocity)
    const unsigned n_dynamic_parameters = 2;
    // -- measurement parameters (2, position and velocity)
    const unsigned n_measurement_parameters = 2;
    // -- control parameters (1, acceleration)
    const unsigned n_control_parameters = 1;

    // Create as many instances of 1D Kalman filters as required
    Kalman_filter_pt =
            new CCKalmanFilter1DDeadReckoning*[N_Kalman_filters];

    // Create particular instances of Kalman filter (for X)
    Kalman_filter_pt[0] =
            new CCKalmanFilter1DDeadReckoning(n_dynamic_parameters,
                                              n_measurement_parameters,
                                              n_control_parameters);

    // Create particular instances of Kalman filter (for Y)
    Kalman_filter_pt[1] =
            new CCKalmanFilter1DDeadReckoning(n_dynamic_parameters,
                                              n_measurement_parameters,
                                              n_control_parameters);

    // Kalman is not initialised
    Initialised_Kalman = false;

    // -------------------------------------------------------------------
    // Plot stuff
    // -------------------------------------------------------------------
    // Create the graph for X coordinates
    ui->wdg_plot_x->addGraph(); // Mouse coordinates
    ui->wdg_plot_x->addGraph(); // Kalman coordinates

    // Create the graph for Y coordinates
    ui->wdg_plot_y->addGraph(); // Mouse coordinates
    ui->wdg_plot_y->addGraph(); // Kalman coordinates

    // Create the graph for velocity
    ui->wdg_plot_vel->addGraph(); // Velocity

    Global_counter_for_plot = 0;

    // ------------------------------------------------------------------
    // Image stuff
    // ------------------------------------------------------------------
    // Read the image
    cv::Mat tmp_image = cv::imread("../blank_image/black.png");
    Blank_image_pt = new cv::Mat(tmp_image);

    // ------------------------------------------------------------------
    // Interface stuff
    // ------------------------------------------------------------------
    // Disable the start button
    ui->btn_stop_main_timer->setDisabled(true);

}

MainWindow::~MainWindow()
{
    // Free memory for timer
    if (qmain_timer_pt != 0)
    {
        delete qmain_timer_pt;
        qmain_timer_pt = 0;
    }

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

    delete ui;
}


// Mouse events
void MainWindow::mouse_pos()
{

    // Get old mouse positions
    X_mouse_previous = X_mouse;
    Y_mouse_previous = Y_mouse;
    // Get new mouse positions
    X_mouse = ui->lbl_mouse->getX();
    Y_mouse = ui->lbl_mouse->getY();
    //ui->lbl_mouse->setText(QString("X = %1  Y = %2").arg(X_mouse).arg(Y_mouse));
    ui->lbl_mouse_pos->setText(QString("X = %1  Y = %2").arg(X_mouse).arg(Y_mouse));

}

void MainWindow::left_mouse_pressed()
{
    if (Left_mouse_pressed_flag)
    {
        Left_mouse_pressed_flag = false;
    }
    else
    {
        Left_mouse_pressed_flag = true;

        // Is Kalman, initiliased, if not then initialise
        if (!Initialised_Kalman)
        {
            // -------------------------------------------------------
            // Initialise Kalmans since this is a new set of data
            // -------------------------------------------------------
            const double noise_covariance_q = 1.0e-2; //1.0e-2
            const double noise_covariance_r = 0.5;    //0.5

            // Kalman X
            // Initial state vector for Kalman X
            Kalman_filter_pt[0]->x(0) = X_mouse; // x-position
            Kalman_filter_pt[0]->x(1) = 0.0; // x-pixels per frame (velocity)
            const double dt = 1.0;
            Kalman_filter_pt[0]->initialise(dt,
                                            noise_covariance_q,
                                            noise_covariance_r);

            // Kalman Y
            // Initial state vector for Kalman X
            Kalman_filter_pt[1]->x(0) = Y_mouse; // y-position
            Kalman_filter_pt[1]->x(1) = 0.0; // y-pixels per frame (velocity)
            Kalman_filter_pt[1]->initialise(dt,
                                            noise_covariance_q,
                                            noise_covariance_r);

            // Set Kalman as initialised
            Initialised_Kalman = true;

        }

    }

}

void MainWindow::right_mouse_pressed()
{
    if (Right_mouse_pressed_flag)
    {
        Position_by_sensor = false;
        Right_mouse_pressed_flag = false;
    }
    else
    {
        Position_by_sensor = true;
        Right_mouse_pressed_flag = true;
    }
}

// ======================================================================
// Called method when the main timer timed out!
// ======================================================================
void MainWindow::main_timer_timeout()
{

    if (Left_mouse_pressed_flag)
    {
        // ----------------------------------------------------------------
        // ----------------------------------------------------------------
        // Processing zone
        // ----------------------------------------------------------------
        // ----------------------------------------------------------------
        if (Initialised_Kalman)
        {
            // Process Kalman

            // Is the position sensor enabled?
            if (Position_by_sensor)
            {
                Kalman_filter_pt[0]->enable_use_position_sensor();
                Kalman_filter_pt[1]->enable_use_position_sensor();
            }
            else
            {
                Kalman_filter_pt[0]->disable_use_position_sensor();
                Kalman_filter_pt[1]->disable_use_position_sensor();
            }

            const double dt = 1;
            // Get velocity from mouse positions
            double x_vel = (X_mouse - X_mouse_previous) / dt;
            double y_vel = (Y_mouse - Y_mouse_previous) / dt;

            // -------------------------------------------------
            // Kalman X
            // -------------------------------------------------
            // Set measurement
            Kalman_filter_pt[0]->z(0) = X_mouse;
            Kalman_filter_pt[0]->z(1) = x_vel;
            // Apply Kalman
            Kalman_filter_pt[0]->apply(dt);

            // Set variables for ploting and output
            // Take it from updated
            X_Kalman = Kalman_filter_pt[0]->x_updated(0);
            X_vel = Kalman_filter_pt[0]->x_updated(1);

            // -------------------------------------------------
            // Kalman Y
            // -------------------------------------------------
            // Set measurement
            Kalman_filter_pt[1]->z(0) = Y_mouse;
            Kalman_filter_pt[1]->z(1) = y_vel;
            // Apply Kalman
            Kalman_filter_pt[1]->apply(dt);
            // Set variables for ploting and output
            // Take it from updated
            Y_Kalman = Kalman_filter_pt[1]->x_updated(0);
            Y_vel = Kalman_filter_pt[1]->x_updated(1);

        } // if (Initialised_Kalman)

        // ----------------------------------------------------------------
        // ----------------------------------------------------------------
        // Painting zone
        // ----------------------------------------------------------------
        // ----------------------------------------------------------------

        // Modify the image
        cv::rectangle(*Blank_image_pt, cv::Rect(X_mouse, Y_mouse, 3, 3),
                      cv::Scalar(0, 255, 0), 1);

        if (Initialised_Kalman)
        {
            // Modify the image
            cv::rectangle(*Blank_image_pt, cv::Rect(X_Kalman, Y_Kalman, 3, 3),
                          cv::Scalar(255, 0, 0), 1);
        }

        ui->lbl_mouse_vel->setText(QString("Vx = %1  Vy = %2").arg(X_vel).arg(Y_vel));

        // ----------------------------------------------------------------
        // ----------------------------------------------------------------
        // Plotting zone
        // ----------------------------------------------------------------
        // ----------------------------------------------------------------
        // Get the maximum and minimum from the label
        const int x_max = static_cast<unsigned>(ui->lbl_mouse->width());
        const int y_max = static_cast<unsigned>(ui->lbl_mouse->height());
        plot(y_max, x_max);

        // ----------------------------------------------------------------
        // Draw the image
        // ----------------------------------------------------------------
        ui->lbl_mouse->setPixmap(ASM::cvMatToQPixmap(*Blank_image_pt));
    }
    else
    {
        // Do nothing
    }

    // Show whether the position is given by the sensor or not
    if (Position_by_sensor)
    {
        ui->lbl_position_sensor->setText("<font color='green'>Position sensor ON</font>");
    }
    else
    {
        ui->lbl_position_sensor->setText("<font color='red'>Position sensor OFF</font>");
    }

}

// ======================================================================
// Plot the data
// ======================================================================
void MainWindow::plot(const unsigned y_max, const unsigned x_max)
{

    const unsigned range_in_x = 2500;

    // Increase the global counter
    Global_counter_for_plot++;

    // Add the current mouse coordinates to the data to plot
    Mouse_coordinates[0].push_back(X_mouse);
    Mouse_coordinates[1].push_back(Y_mouse);

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
    // Mouse (GREEN)
    //ui->wdg_plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->wdg_plot_x->graph(0)->setPen(QPen(QColor(0,255,0)));
    ui->wdg_plot_x->graph(0)->setData(time_data, Mouse_coordinates[0]);
    // ---------------------------------------------------------------
    // Kalman (BLUE)
    ui->wdg_plot_x->graph(1)->setPen(QPen(QColor(0,0,255)));
    ui->wdg_plot_x->graph(1)->setData(time_data, Kalman_coordinates[0]);

    // Assign labels to axis    ui->wdg_plot_x->xAxis->setLabel("time");
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
    // Mouse (GREEN)
    //ui->wdg_plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->wdg_plot_y->graph(0)->setPen(QPen(QColor(0,255,0)));
    ui->wdg_plot_y->graph(0)->setData(time_data, Mouse_coordinates[1]);
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
    // VELOCITY PLOT
    // ---------------------------------------------------------------
    // Generate line from origin
    // Number of sample points
    const double n_sample = 2;
    QVector<double> y_vel(n_sample);
    QVector<double> x_vel(n_sample);
    x_vel[0] = 0.0;
    y_vel[0] = 0.0;
    x_vel[1] = X_vel;
    y_vel[1] = Y_vel;
    // Assign data to plot
    // ---------------------------------------------------------------
    // Mouse (RED)
    QPen this_pen(QColor(255,0,0));
    this_pen.setWidth(3);
    ui->wdg_plot_vel->graph(0)->setPen(this_pen);
    ui->wdg_plot_vel->graph(0)->setData(x_vel, y_vel);

    // Assign labels to axis
    //ui->wdg_plot_vel->xAxis->setLabel("time");
    //ui->wdg_plot_vel->yAxis->setLabel("y-position");

    // Set range
    ui->wdg_plot_vel->xAxis->setRange(-50, 50);
    ui->wdg_plot_vel->yAxis->setRange(-50, 50);

    ui->wdg_plot_vel->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->wdg_plot_vel->replot();

}

void MainWindow::on_btnstart_main_timer_clicked()
{

    if (!Main_timer_started)
    {
        // Start the main timer
        qmain_timer_pt->start(Main_timer_miliseconds);
        // Enable the stop button
        ui->btn_stop_main_timer->setEnabled(true);
        // Disable the start button
        ui->btnstart_main_timer->setDisabled(true);
        // Set the flag to indicate the main timer has started
        Main_timer_started = true;
    }

}

void MainWindow::on_btn_stop_main_timer_clicked()
{

    if (Main_timer_started)
    {
        // Start the main timer
        qmain_timer_pt->stop();
        // Enable the start button
        ui->btnstart_main_timer->setEnabled(true);
        // Disable the stop button
        ui->btn_stop_main_timer->setDisabled(true);
        // Set the flag to indicate the main timer has started
        Main_timer_started = false;

        // Reinitialise Kalman and mouse pressed flag
        Initialised_Kalman = false;
        Left_mouse_pressed_flag = false;

    }

}
