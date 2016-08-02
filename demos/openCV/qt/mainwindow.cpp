#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialise image source
    Image_from_file = false;
    Image_from_screen = false;
    Image_from_camera = false;

    // Initialize kernel size
    Filter_kernel_size = 7;

    // Initialize contrast and brightness
    Contrast = 1.0;
    Brightness = 0.0;

    // Create objects and let them ready to read or capture
    // images
    Process_image_from_file = new CCProcessImageFromFile();

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
    Process_image_from_screen_thread_pt = new CCProcessImageFromScreen();

    // Now set a pointer to the capture thread in the process image thread
    // so it can ask for a new image
    Process_image_from_screen_thread_pt->
            set_capture_thread_pt(Capture_image_thread_pt);

    // Get the mutex from the screen capturer and set it to the
    // image processor
    Process_image_from_screen_thread_pt->
            set_mutex_pt(&(Capture_image_thread_pt->mutex_capturing_image()));

    // ---------------------------------------------------------------
    // Start the threads
    // ---------------------------------------------------------------
    Capture_image_thread_pt->start_thread();
    Process_image_from_screen_thread_pt->start_thread();
}

MainWindow::~MainWindow()
{
    // Delete all created objects
    delete Process_image_from_file;

    // Stop process image thread
    Process_image_from_screen_thread_pt->stop();

    // Free memory for processing image thread
    delete Process_image_from_screen_thread_pt;
    Process_image_from_screen_thread_pt = 0;

    // Stop capturing thread
    Capture_image_thread_pt->stop();

    // Free memory for capturing thread
    delete Capture_image_thread_pt;
    Capture_image_thread_pt = 0;

    delete ui;
}

void MainWindow::on_btn_get_image_clicked()
{

    // Free image from any previous created object
    if (Image_from_file)
    {
        Process_image_from_file->free_stuff();
    }

    if (Image_from_screen)
    {
        Process_image_from_screen_thread_pt->free_stuff();
    }

    if (Image_from_camera)
    {

    }

    // Check the status of the radio buttons
    if (ui->rbtn_image_from_file->isChecked())
    {
        Image_from_file = true;
        Image_from_screen = false;
        Image_from_camera = false;

        // The image to read
        std::string image_name("../data/ship.jpg");
        // Try to read an image
        Process_image_from_file->read_image(image_name);

    }
    else if (ui->rbtn_image_from_screen->isChecked())
    {
        Image_from_file = false;
        Image_from_screen = true;
        Image_from_camera = false;

        // Start capturing
        Capture_image_thread_pt->start_capturing();
        Process_image_from_screen_thread_pt->start_image_processing();

    }
    else if (ui->rbtn_image_from_camera->isChecked())
    {
        Image_from_file = false;
        Image_from_screen = false;
        Image_from_camera = true;
    }



}

void MainWindow::on_bnt_clear_clicked()
{
    // Clear all process
    if (Image_from_file)
    {
        Process_image_from_file->clear();
    }

    if (Image_from_screen)
    {
        Process_image_from_screen_thread_pt->clear();
    }

    if (Image_from_camera)
    {

    }

}

void MainWindow::on_btn_kernel_size_plus_clicked()
{
    const unsigned max_kernel_size = 101;
    if (Filter_kernel_size < max_kernel_size)
    {
        Filter_kernel_size+=2;
        ui->lbl_kernel_size->setText(QString("Kernel size: %1").arg(Filter_kernel_size));
    }
}

void MainWindow::on_btn_kernel_size_minus_clicked()
{
    const unsigned min_kernel_size = 1;
    if (Filter_kernel_size > min_kernel_size)
    {
        Filter_kernel_size-=2;
        QString lbl_text("Kernel size: " + Filter_kernel_size);
        ui->lbl_kernel_size->setText(QString("Kernel size: %1").arg(Filter_kernel_size));
    }
}

void MainWindow::on_btn_filter_normalised_clicked()
{
    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        cv::blur(Process_image_from_file->originalImage(),
                 Process_image_from_file->processedImage(),
                 cv::Size( Filter_kernel_size, Filter_kernel_size ));
        Process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {
        Process_image_from_screen_thread_pt->
                filter_kernel_size() = Filter_kernel_size;
        Process_image_from_screen_thread_pt->disable_gaussian_filter();
        Process_image_from_screen_thread_pt->disable_median_filter();
        Process_image_from_screen_thread_pt->enable_normalised_filter();
    }

    if (Image_from_camera)
    {

    }

}

void MainWindow::on_btn_filter_gauss_clicked()
{
    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        cv::GaussianBlur(Process_image_from_file->originalImage(),
                         Process_image_from_file->processedImage(),
                         cv::Size( Filter_kernel_size, Filter_kernel_size ),
                         0, 0);
        Process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {
        Process_image_from_screen_thread_pt->
                filter_kernel_size() = Filter_kernel_size;
        Process_image_from_screen_thread_pt->disable_normalised_filter();
        Process_image_from_screen_thread_pt->disable_median_filter();
        Process_image_from_screen_thread_pt->enable_gaussian_filter();
    }

    if (Image_from_camera)
    {

    }

}

void MainWindow::on_btn_filter_median_clicked()
{
    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        cv::medianBlur(Process_image_from_file->originalImage(),
                       Process_image_from_file->processedImage(),
                       Filter_kernel_size);
        Process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {
        Process_image_from_screen_thread_pt->
                filter_kernel_size() = Filter_kernel_size;
        Process_image_from_screen_thread_pt->disable_normalised_filter();
        Process_image_from_screen_thread_pt->disable_gaussian_filter();
        Process_image_from_screen_thread_pt->enable_median_filter();
    }

    if (Image_from_camera)
    {

    }

}

void MainWindow::on_sld_contrast_sliderMoved(int position)
{
    // Temporaly store for image
    cv::Mat tmp_image;

    // Scale the value of the slide
    // Get max. and min. value of slide
    const unsigned max_slide = ui->sld_contrast->maximum();
    const unsigned min_slide = ui->sld_contrast->minimum();

    const double max_new = 3.0;
    const double min_new = 1.0;
    const double scale_factor = (max_new - min_new) / (max_slide - min_slide);

    Contrast = min_new + position * scale_factor;

    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        // Get the image
        tmp_image = Process_image_from_file->originalImage();
        // Transform the image
        tmp_image.convertTo(Process_image_from_file->processedImage(), -1,
                            Contrast, Brightness);
        Process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {

    }

    if (Image_from_camera)
    {

    }

}

void MainWindow::on_sld_brightness_sliderMoved(int position)
{
    // Temporaly store for image
    cv::Mat tmp_image;

    Brightness = position;

    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        // Get the image
        tmp_image = Process_image_from_file->originalImage();
        // Transform the image
        tmp_image.convertTo(Process_image_from_file->processedImage(), -1,
                            Contrast, Brightness);
        Process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {

    }

    if (Image_from_camera)
    {

    }

}

void MainWindow::on_bnt_histogram_equalize_clicked()
{

    // Check where is the image comming from and perform the equalisation
    if (Image_from_file)
    {
        // Convert to grayscale
        cv::cvtColor(Process_image_from_file->processedImage(),
                     Process_image_from_file->processedImage(),
                     CV_BGR2GRAY);

        // Apply Histogram Equalization
        cv::equalizeHist(Process_image_from_file->processedImage(),
                         Process_image_from_file->processedImage());

        Process_image_from_file->show_image();

    }

    if (Image_from_screen)
    {

    }

    if (Image_from_camera)
    {

    }

}

void MainWindow::on_btn_zoom_up_clicked()
{
    // Temporaly store for image
    cv::Mat tmp_image;

    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        // Get the image
        tmp_image = Process_image_from_file->processedImage();
        // Apply zoom up to the image
        cv::pyrUp(tmp_image, Process_image_from_file->processedImage());

        Process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {

    }

    if (Image_from_camera)
    {

    }

}

void MainWindow::on_btn_zoom_down_clicked()
{
    // Temporaly store for image
    cv::Mat tmp_image;

    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        // Get the image
        tmp_image = Process_image_from_file->processedImage();
        // Apply zoom up to the image
        cv::pyrDown(tmp_image, Process_image_from_file->processedImage());

        Process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {

    }

    if (Image_from_camera)
    {

    }

}

void MainWindow::on_btn_plus45_clicked()
{

}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    // Temporaly store for image
    cv::Mat tmp_image;

    // Scale the value of the slide
    // Get max. and min. value of slide
    const unsigned max_slide = ui->sld_contrast->maximum();
    const unsigned min_slide = ui->sld_contrast->minimum();

    const double max_new = 360.0;
    const double min_new = 0.0;
    const double scale_factor = (max_new - min_new) / (max_slide - min_slide);

    // Set the rotation angle and the scale
    const double angle = min_new + position * scale_factor;
    const double scale = 1.0;

    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        // Get the image
        tmp_image = Process_image_from_file->originalImage();

        // Get the rotation point
        cv::Point center = cv::Point(tmp_image.cols*0.5, tmp_image.rows*0.5);
        cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle, scale);

        cv::warpAffine(tmp_image, Process_image_from_file->processedImage(),
                       rotation_matrix, tmp_image.size());
        Process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {

    }

    if (Image_from_camera)
    {

    }
}
