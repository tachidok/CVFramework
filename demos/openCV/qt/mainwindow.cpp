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

    // ---------------------------------------------------------------
    // Stuff to read images from file
    // ---------------------------------------------------------------
    Process_image_from_file = new CCProcessImageFromFile();

    // ---------------------------------------------------------------
    // Stuff to read images from screen
    // ---------------------------------------------------------------
    // Create the thread to capture and display the image
    Capture_image_from_screen_thread_pt = new CCCaptureFromScreen();

    // Set the capture geometry (region of interest or ROI)
    int width_capture = 640;//160;//640;
    int height_capture = 480;//120;//480;
    int desktop_width = QApplication::desktop()->width();
    int desktop_height = QApplication::desktop()->height();
    Capture_image_from_screen_thread_pt->
            set_capture_geometry(desktop_width - width_capture - 1, 0,
                                 width_capture, height_capture);

    // Do not show the captured image
    Capture_image_from_screen_thread_pt->disable_image_displaying();

    // ---------------------------------------------------------------
    // Initialise the image processer
    Process_image_from_screen_thread_pt = new CCProcessImageFromScreen();

    // Now set a pointer to the capture thread in the process image thread
    // so it can ask for a new image
    Process_image_from_screen_thread_pt->
            set_capture_thread_pt(Capture_image_from_screen_thread_pt);

    // Get the mutex from the screen capturer and set it to the
    // image processor
    Process_image_from_screen_thread_pt->
            set_mutex_pt(&(Capture_image_from_screen_thread_pt->mutex_capturing_image()));

    // ---------------------------------------------------------------
    // Start the threads
    // ---------------------------------------------------------------
    Capture_image_from_screen_thread_pt->start_thread();
    Process_image_from_screen_thread_pt->start_thread();

    // ---------------------------------------------------------------
    // Stuff to read images from camera
    // ---------------------------------------------------------------
    // Create the thread to capture and display the image
    Capture_image_from_camera_thread_pt = new CCCaptureFromCamera();

    // Do not show the captured image
    Capture_image_from_camera_thread_pt->disable_image_displaying();

    // ---------------------------------------------------------------
    // Initialise the image processer
    Process_image_from_camera_thread_pt = new CCProcessImageFromCamera();

    // Now set a pointer to the capture thread in the process image thread
    // so it can ask for a new image
    Process_image_from_camera_thread_pt->
            set_capture_thread_pt(Capture_image_from_camera_thread_pt);

    // Get the mutex from the screen capturer and set it to the
    // image processor
    Process_image_from_camera_thread_pt->
            set_mutex_pt(&(Capture_image_from_camera_thread_pt->mutex_capturing_image()));

    // ---------------------------------------------------------------
    // Start the threads
    // ---------------------------------------------------------------
    Capture_image_from_camera_thread_pt->start_thread();
    Process_image_from_camera_thread_pt->start_thread();

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
    Capture_image_from_screen_thread_pt->stop();

    // Free memory for capturing thread
    delete Capture_image_from_screen_thread_pt;
    Capture_image_from_screen_thread_pt = 0;

    // Stop process image thread
    Process_image_from_camera_thread_pt->stop();

    // Free memory for processing image thread
    delete Process_image_from_camera_thread_pt;
    Process_image_from_camera_thread_pt = 0;

    // Stop capturing thread
    Capture_image_from_camera_thread_pt->stop();

    // Free memory for capturing thread
    delete Capture_image_from_camera_thread_pt;
    Capture_image_from_camera_thread_pt = 0;

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
        // Start capturing
        Capture_image_from_screen_thread_pt->interrupt_capturing();
        Process_image_from_screen_thread_pt->interrupt_image_processing();
        Process_image_from_screen_thread_pt->free_stuff();
    }

    if (Image_from_camera)
    {
        // Start capturing
        Capture_image_from_camera_thread_pt->interrupt_capturing();
        Process_image_from_camera_thread_pt->interrupt_image_processing();
        Process_image_from_camera_thread_pt->free_stuff();
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
        Capture_image_from_screen_thread_pt->start_capturing();
        Process_image_from_screen_thread_pt->start_image_processing();

    }
    else if (ui->rbtn_image_from_camera->isChecked())
    {
        Image_from_file = false;
        Image_from_screen = false;
        Image_from_camera = true;

        // Start capturing
        Capture_image_from_camera_thread_pt->start_capturing();
        Process_image_from_camera_thread_pt->start_image_processing();
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
        Process_image_from_camera_thread_pt->clear();
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
        Process_image_from_file->
                filter_kernel_size() = Filter_kernel_size;
        Process_image_from_file->
                apply_normalised_filter(Process_image_from_file->originalImage(),
                                        Process_image_from_file->processedImage());
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
        Process_image_from_camera_thread_pt->
                filter_kernel_size() = Filter_kernel_size;
        Process_image_from_camera_thread_pt->disable_gaussian_filter();
        Process_image_from_camera_thread_pt->disable_median_filter();
        Process_image_from_camera_thread_pt->enable_normalised_filter();
    }

}

void MainWindow::on_btn_filter_gauss_clicked()
{
    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        Process_image_from_file->
                filter_kernel_size() = Filter_kernel_size;
        Process_image_from_file->
                apply_gaussian_filter(Process_image_from_file->originalImage(),
                                      Process_image_from_file->processedImage());
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
        Process_image_from_camera_thread_pt->
                filter_kernel_size() = Filter_kernel_size;
        Process_image_from_camera_thread_pt->disable_normalised_filter();
        Process_image_from_camera_thread_pt->disable_median_filter();
        Process_image_from_camera_thread_pt->enable_gaussian_filter();
    }

}

void MainWindow::on_btn_filter_median_clicked()
{
    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        Process_image_from_file->
                filter_kernel_size() = Filter_kernel_size;
        Process_image_from_file->
                apply_median_filter(Process_image_from_file->originalImage(),
                                    Process_image_from_file->processedImage());
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
        Process_image_from_camera_thread_pt->
                filter_kernel_size() = Filter_kernel_size;
        Process_image_from_camera_thread_pt->disable_normalised_filter();
        Process_image_from_camera_thread_pt->disable_gaussian_filter();
        Process_image_from_camera_thread_pt->enable_median_filter();
    }

}

void MainWindow::on_sld_contrast_sliderMoved(int position)
{
    // Scale the value of the slide
    // Get max. and min. value of slide
    const unsigned max_slide = ui->sld_contrast->maximum();
    const unsigned min_slide = ui->sld_contrast->minimum();

    const double max_new = 3.0;
    const double min_new = 1.0;
    const double scale_factor = (max_new - min_new) / (max_slide - min_slide);

    const double contrast = min_new + position * scale_factor;

    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        Process_image_from_file->
                apply_contrast(Process_image_from_file->originalImage(),
                               Process_image_from_file->processedImage(),
                               contrast);
        Process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {
        // Enable application of contrast
        Process_image_from_screen_thread_pt->enable_contrast();
        // Set the contrast
        Process_image_from_screen_thread_pt->contrast() = contrast;
    }

    if (Image_from_camera)
    {
        // Enable application of contrast
        Process_image_from_camera_thread_pt->enable_contrast();
        // Set the contrast
        Process_image_from_camera_thread_pt->contrast() = contrast;
    }

}

void MainWindow::on_sld_brightness_sliderMoved(int position)
{
    const double brightness = position;

    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        Process_image_from_file->
                apply_brightness(Process_image_from_file->originalImage(),
                                 Process_image_from_file->processedImage(),
                                 brightness);
        Process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {
        // Enable application of brightness
        Process_image_from_screen_thread_pt->enable_brightness();
        // Set the brightness
        Process_image_from_screen_thread_pt->brightness() = brightness;
    }

    if (Image_from_camera)
    {
        // Enable application of brightness
        Process_image_from_camera_thread_pt->enable_brightness();
        // Set the brightness
        Process_image_from_camera_thread_pt->brightness() = brightness;
    }

}

void MainWindow::on_bnt_histogram_equalize_clicked()
{

    // Check where is the image comming from and perform the equalisation
    if (Image_from_file)
    {
        Process_image_from_file->
                apply_histogram_equalisation(Process_image_from_file->originalImage(),
                                             Process_image_from_file->processedImage());
        Process_image_from_file->show_image();

    }

    if (Image_from_screen)
    {
        Process_image_from_screen_thread_pt->enable_histogram_equalisation();
    }

    if (Image_from_camera)
    {
        Process_image_from_camera_thread_pt->enable_histogram_equalisation();
    }

}

void MainWindow::on_btn_zoom_up_clicked()
{
    // Temporaly store for image
    cv::Mat tmp_image;

    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        Process_image_from_file->
                zoom_in(Process_image_from_file->processedImage(),
                        Process_image_from_file->processedImage());
        Process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {
        // First disable applied zooms
        Process_image_from_screen_thread_pt->reset_zoom();
        // .. then apply the desired zoom
        Process_image_from_screen_thread_pt->enable_one_zoom_in_step();
    }

    if (Image_from_camera)
    {
        // First disable applied zooms
        Process_image_from_camera_thread_pt->reset_zoom();
        // .. then apply the desired zoom
        Process_image_from_camera_thread_pt->enable_one_zoom_in_step();
    }

}

void MainWindow::on_btn_zoom_down_clicked()
{
    // Temporaly store for image
    cv::Mat tmp_image;

    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        Process_image_from_file->
                zoom_out(Process_image_from_file->processedImage(),
                         Process_image_from_file->processedImage());
        Process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {
        // First disable applied zooms
        Process_image_from_screen_thread_pt->reset_zoom();
        // .. then apply the desired zoom
        Process_image_from_screen_thread_pt->enable_one_zoom_out_step();
    }

    if (Image_from_camera)
    {
        // First disable applied zooms
        Process_image_from_camera_thread_pt->reset_zoom();
        // .. then apply the desired zoom
        Process_image_from_camera_thread_pt->enable_one_zoom_out_step();
    }

}

void MainWindow::on_btn_plus45_clicked()
{

}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
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
        Process_image_from_file->
                rotate(Process_image_from_file->originalImage(),
                       Process_image_from_file->processedImage(),
                       angle, scale);
        Process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {
        Process_image_from_screen_thread_pt->enable_rotation();
        Process_image_from_screen_thread_pt->angle() = angle;
        Process_image_from_screen_thread_pt->scale() = scale;
    }

    if (Image_from_camera)
    {
        Process_image_from_camera_thread_pt->enable_rotation();
        Process_image_from_camera_thread_pt->angle() = angle;
        Process_image_from_camera_thread_pt->scale() = scale;
    }
}
