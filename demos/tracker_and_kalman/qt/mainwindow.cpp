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
    Timer_milisecond = 50;

#endif // #ifdef T_USE_TIMER

    // ======================================================================
    // OpenCV stuff
    // ======================================================================
    // Set mouse callback
#if 0
    cv::setMouseCallback("Video", onMouse, 0);
    // Opens and shows an image
    cv::Mat live_image = cv::imread("../data/image-0001.jpg");
    cv::namedWindow("Video");
    cv::imshow("Video", live_image);
#endif // #if 0

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

    // Initialise found pattern flag
    Found_pattern = false;

    // By defaul the left mouse button is not pressed
    Left_mouse_button_pressed = false;

    // Maximum and minimum pattern size
    Max_pattern_window_size = 33;
    Min_pattern_window_size = 7;
    // Initial pattern window size
    Pattern_window_size = 17;
    // Increment step for pattern window size
    Pattern_window_size_increasing_step = 2;
    // Search window is based on the pattern window size
    Search_window_size = Pattern_window_size*2;

    // Characterise a lost in the tracker
    // Difference in pixels (jump by the target position)
    Max_allowed_difference = 10;

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

#define T_IMAGE_FROM_FILE
//#define T_IMAGE_FROM_CAPTURE

#if 1
// Draws an image in the "label" object of the ui
void MainWindow::paint_image()
{
//#ifdef T_IMAGE_FROM_CAPTURE
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
    cv::Mat live_image = ASM::QPixmapToCvMat(pixmap, false);

    // Resize the input image and output it in the same image
    //cv::resize(live_image, live_image, cv::Size(640, 480));
    cv::namedWindow("Live-video");
    cv::imshow("Live-video", live_image);

    //ui->lbl_global_status->setText("<font color='red'>ERROR: Getting image</font>");
    //return;
//#endif // #ifdef T_IMAGE_FROM_CAPTURE

#ifdef T_IMAGE_FROM_FILE
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

    cv::Mat file_image = cv::imread(full_image_name.toStdString());

    // Qimage_pt->load(full_image_name);
    // Increment the image index
    Index_image+=Increment_image_number;

#endif // #ifdef T_IMAGE_FROM_FILE

    // ----------------------------------------------------------
    // Tracking
    // ----------------------------------------------------------

    // We get the pattern from the current image
    if (Left_mouse_button_pressed)
    {
        // Get the coordinates of the tracker
        X_tracker = X_mouse;
        Y_tracker = Y_mouse;
#ifdef T_IMAGE_FROM_FILE
        if ((Tracker_pt->initialise(file_image, X_tracker, Y_tracker, Search_window_size, Pattern_window_size)))
#endif // #ifdef T_IMAGE_FROM_FILE
#ifdef T_IMAGE_FROM_CAPTURE
        if ((Tracker_pt->initialise(live_image, X_tracker, Y_tracker, Search_window_size, Pattern_window_size)))
#endif // #ifdef T_IMAGE_FROM_CAPTURE
        {
            ui->lbl_global_status->setText("<font color='green'>OK: Tracker initialised correctly</font>");
        }
        else
        {
            ui->lbl_global_status->setText("<font color='red'>ERROR: Tracker was not initialised correctly</font>");
        }

        Do_Kalman = true;
        if (Do_Kalman)
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

        }

        // Disable some buttons
        //ui->btn_decrease_search_window->setDisabled(true);
        //ui->btn_increase_search_window->setDisabled(true);

        Left_mouse_button_pressed = false;

    }

    double equivalence_value = 0;
    // Restart found pattern flag
    Found_pattern = false;
    // ----------------------------------------------------------
    // Do tracking? Only if there is a pattern to search for ...
    if (Tracker_pt->initialised())
    {
        // Backup the old positions
        const int old_x_tracker = X_tracker;
        const int old_y_tracker = Y_tracker;
        // Draw search window (If we draw the search window here we can see
        // the position selected by the Tracker inside the search window)
#ifdef T_IMAGE_FROM_FILE
        draw_square(&file_image, X_tracker, Y_tracker, Search_window_size, 255, 255, 0, 2);
#endif // #ifdef T_IMAGE_FROM_FILE
#ifdef T_IMAGE_FROM_CAPTURE
        draw_square(&live_image, X_tracker, Y_tracker, Search_window_size, 255, 255, 0, 2);
#endif // #ifdef T_IMAGE_FROM_CAPTURE
#ifdef T_IMAGE_FROM_FILE
        if (Tracker_pt->search_pattern(file_image, X_tracker, Y_tracker))
#endif // #ifdef T_IMAGE_FROM_FILE
#ifdef T_IMAGE_FROM_CAPTURE
        if (Tracker_pt->search_pattern(live_image, X_tracker, Y_tracker))
#endif // #ifdef T_IMAGE_FROM_CAPTURE
        {
            if (std::abs(old_x_tracker - static_cast<int>(X_tracker)) > Max_allowed_difference ||
                    std::abs(old_y_tracker - static_cast<int>(Y_tracker)) > Max_allowed_difference)
            {
                //qDebug() << "DifferencesX:" << std::abs(old_x_tracker - static_cast<int>(X_tracker));
                //qDebug() << "DifferencesX:" << std::abs(old_y_tracker - static_cast<int>(Y_tracker));
                Found_pattern = false;
                ui->lbl_global_status->setText("<font color='red'>ERROR: Tracker did not found the pattern</font>");
            }
            else
            {
                Found_pattern = true;
                ui->lbl_global_status->setText("<font color='green'>OK: Tracker found the pattern</font>");
            }
        }
        else
        {
            Found_pattern = false;
            ui->lbl_global_status->setText("<font color='red'>ERROR: Tracker did not found the pattern</font>");
        }

        // Apply Kalman
        if (Do_Kalman)
        {
            // -------------------------------------------------
            // Kalman X
            // -------------------------------------------------
            // Set measurement
            Kalman_filter_pt[0]->z(0) = X_tracker;
            //Kalman_filter_pt[0]->z(0) = X_mouse;

            // The dt (one frame per time)
            const double dt = 1.0;
            Kalman_filter_pt[0]->apply(dt, !Found_pattern);
            // Set variables for ploting and output
            if (Found_pattern)
            {
                // Take it from updated
                X_Kalman = Kalman_filter_pt[0]->x_updated(0);
            }
            else
            {
                // Take it from predicted
                X_Kalman = Kalman_filter_pt[0]->x_predicted(0);
            }

            // -------------------------------------------------
            // Kalman Y
            // -------------------------------------------------
            // Set measurement
            Kalman_filter_pt[1]->z(0) = Y_tracker;
            //Kalman_filter_pt[1]->z(0) = Y_mouse;

            Kalman_filter_pt[1]->apply(dt, !Found_pattern);
            // Set variables for ploting and output
            if (Found_pattern)
            {
                // Take it from updated
                Y_Kalman = Kalman_filter_pt[1]->x_updated(0);
            }
            else
            {
                // Take it from predicted
                Y_Kalman = Kalman_filter_pt[1]->x_predicted(0);
            }

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
    // Green aim for mouse
#ifdef T_IMAGE_FROM_FILE
    draw_aim(&file_image, X_mouse, Y_mouse, Pattern_window_size/2, 0, 255, 0);
#endif // #ifdef T_IMAGE_FROM_FILE
#ifdef T_IMAGE_FROM_CAPTURE
    draw_aim(&live_image, X_mouse, Y_mouse, Pattern_window_size/2, 0, 255, 0);
#endif // #ifdef T_IMAGE_FROM_CAPTURE
#ifdef T_IMAGE_FROM_FILE
    draw_square(&file_image, X_mouse, Y_mouse, Pattern_window_size, 0, 255, 0);
#endif // #ifdef T_IMAGE_FROM_FILE
#ifdef T_IMAGE_FROM_CAPTURE
    draw_square(&live_image, X_mouse, Y_mouse, Pattern_window_size, 0, 255, 0);
#endif // #ifdef T_IMAGE_FROM_CAPTURE

    //qDebug() << X << Y;

    // ----------------------------------------------------------
    // Draw the pattern? Only if there is a pattern stored
    if (Tracker_pt->initialised())
    {
        if (Found_pattern)
        {
            // Draw an square based on the tracking coordinates (red for tracker)
#ifdef T_IMAGE_FROM_FILE
            draw_square(&file_image, X_tracker, Y_tracker, Pattern_window_size, 255, 0, 0);
#endif // #ifdef T_IMAGE_FROM_FILE
#ifdef T_IMAGE_FROM_CAPTURE
            draw_square(&live_image, X_tracker, Y_tracker, Pattern_window_size, 255, 0, 0);
#endif // #ifdef T_IMAGE_FROM_CAPTURE
            ui->lbl_status_tracker->setText(QString("X=%1 Y=%2 EV=%3").arg(X_tracker).arg(Y_tracker).arg(equivalence_value));
        }

        // Show me the patter you are looking for
        cv::namedWindow("Pattern");
        cv::Mat output_pattern;
        cv::resize(*(Tracker_pt->pattern_pt()), output_pattern, cv::Size(160, 120));
        cv::imshow("Pattern", output_pattern);
        ui->lbl_image_search->setPixmap(ASM::cvMatToQPixmap(*(Tracker_pt->pattern_pt())));
        //ui->lbl_image_search->setPixmap(QPixmap::fromImage(*(Tracker_pt->pattern_pt()))); // tachidok

        if (Do_Kalman)
        {
            // Draw an square based on the corrected Kalman coordinates
            // (blue for Kalman)
#ifdef T_IMAGE_FROM_FILE
            draw_square(&file_image, X_Kalman, Y_Kalman, Pattern_window_size, 0, 0, 255);
#endif // #ifdef T_IMAGE_FROM_FILE
#ifdef T_IMAGE_FROM_CAPTURE
            draw_square(&live_image, X_Kalman, Y_Kalman, Pattern_window_size, 0, 0, 255);
#endif // #ifdef T_IMAGE_FROM_CAPTURE
            // Output the Kalman coordinates
            ui->lbl_status_kalman->setText(QString("X=%1 Y=%2").arg(X_Kalman).arg(Y_Kalman));
        }

    }

    // ----------------------------------------------------------
    // Plot and draw image
    // ----------------------------------------------------------

    // Plot
#ifdef T_IMAGE_FROM_FILE
    plot(file_image.rows, file_image.cols);
#endif // #ifdef T_IMAGE_FROM_FILE
#ifdef T_IMAGE_FROM_CAPTURE
    plot(live_image.rows, live_image.cols);
#endif // #ifdef T_IMAGE_FROM_CAPTURE

    if (Tracker_pt->initialised() && Do_Kalman)
    {
        // Update the tracker using the Kalman output
        //X_tracker = X_Kalman;
        //Y_tracker = Y_Kalman;
    }

    // Draw the image
#ifdef T_IMAGE_FROM_FILE
    ui->label_image->setPixmap(ASM::cvMatToQPixmap(file_image));
#endif // #ifdef T_IMAGE_FROM_FILE
#ifdef T_IMAGE_FROM_CAPTURE
    ui->label_image->setPixmap(ASM::cvMatToQPixmap(live_image));
#endif // #ifdef T_IMAGE_FROM_CAPTURE
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

// ======================================================================
// Plot the data
// ======================================================================
void MainWindow::plot(const unsigned y_max, const unsigned x_max)
{
    // Add the current mouse coordinates to the data to plot
    Mouse_coordinates[0].push_back(X_mouse);
    Mouse_coordinates[1].push_back(Y_mouse);

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
    // Mouse (GREEN)
    //ui->wdg_plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->wdg_plot_x->graph(0)->setPen(QPen(QColor(0,255,0)));
    ui->wdg_plot_x->graph(0)->setData(time_data, Mouse_coordinates[0]);
    ui->wdg_plot_y->graph(0)->setPen(QPen(QColor(0,255,0)));
    ui->wdg_plot_y->graph(0)->setData(time_data, Mouse_coordinates[1]);
    // ---------------------------------------------------------------
    // Tracker (RED)
    ui->wdg_plot_x->graph(1)->setPen(QPen(QColor(255,0,0)));
    ui->wdg_plot_x->graph(1)->setData(time_data, Tracker_coordinates[0]);
    ui->wdg_plot_y->graph(1)->setPen(QPen(QColor(255,0,0)));
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
    ui->wdg_plot_x->xAxis->setRange(0, Nimages*2);
    ui->wdg_plot_y->xAxis->setRange(0, Nimages*2);

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
    if (Pattern_window_size+Pattern_window_size_increasing_step <= Max_pattern_window_size)
    {
        Pattern_window_size+=Pattern_window_size_increasing_step;
        Search_window_size = Pattern_window_size*2;
    }

}

void MainWindow::on_btn_decrease_search_window_clicked()
{
    if (Pattern_window_size-Pattern_window_size_increasing_step >= Min_pattern_window_size)
    {
        Pattern_window_size-=Pattern_window_size_increasing_step;
        Search_window_size = Pattern_window_size*2;
    }
}

#if 0
void MainWindow::initialise_kalman()
{
    // Kalman X
    // -------------------------------------------------------
    // Before reset set default covariances for process noise
    // and measurement noise
    const double noise_covariance_Q = 1.0e-2; //1.0e-2
    const double noise_covariance_R = 0.5;    //0.5
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

void MainWindow::apply_kalman(const bool predict_only)
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
    //qDebug() << "KalmanPrediction X ("<< prediction_x.rows
    //         << ", " << prediction_x.cols << "): ";
    //std::cerr << prediction_x << std::endl;

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
    //qDebug() << "KalmanPrediction Y ("<< prediction_y.rows
    //         << ", " << prediction_y.cols << "): ";
    //std::cerr << prediction_y << std::endl;

    // Only do prediction step, do not update using sensores measurements
    if (predict_only)
    {
        // Set variables for ploting and output
        // Set the predicted state into the X-variable
        X_Kalman = prediction_x.at<double>(0, 0);
        // Set the predicted state into the Y-variable
        Y_Kalman = prediction_y.at<double>(0, 0);
        return;
    }

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
#endif // #if 0

#if 0
static bool selectObject = false;
static bool startSelection = false;
static cv::Rect bounding_box;
static bool paused;

static void onMouse( int event, int x, int y, int, void *data)
{
  if( !selectObject )
  {
    switch ( event )
    {
      // Left button pushed down
      case cv::EVENT_LBUTTONDOWN:
        //set origin of the bounding box
        startSelection = true;
        boundingBox.x = x;
        boundingBox.y = y;
        boundingBox.width = boundingBox.height = 0;
        break;
      // Left button released
      case cv::EVENT_LBUTTONUP:
        //sei with and height of the bounding box
        boundingBox.width = std::abs( x - boundingBox.x );
        boundingBox.height = std::abs( y - boundingBox.y );
        paused = false;
        selectObject = true;
        break;
      case cv::EVENT_MOUSEMOVE:

        if( startSelection && !selectObject )
        {
          //draw the bounding box
          cv::Mat currentFrame = static_cast<cv::Mat>(*data);
          image.copyTo( currentFrame );
          rectangle( currentFrame, Point((int) boundingBox.x, (int)boundingBox.y ), Point( x, y ), Scalar( 255, 0, 0 ), 2, 1 );
          imshow( "Tracking API", currentFrame );
        }
        break;
    default:

        break;
    }
  }
}
#endif // #if 0
