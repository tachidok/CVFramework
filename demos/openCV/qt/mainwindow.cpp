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
    process_image_from_file = new CCProcessImageFromFile();

}

MainWindow::~MainWindow()
{
    // Delete all created objects
    delete process_image_from_file;

    delete ui;
}

void MainWindow::on_btn_get_image_clicked()
{

    // Free image from any previous created object
    if (Image_from_file)
    {
        process_image_from_file->free_stuff();
    }

    if (Image_from_screen)
    {

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
        process_image_from_file->read_image(image_name);

    }
    else if (ui->rbtn_image_from_screen->isChecked())
    {
        Image_from_file = false;
        Image_from_screen = true;
        Image_from_camera = false;
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
        process_image_from_file->clear();
    }

    if (Image_from_screen)
    {

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
        cv::blur(process_image_from_file->originalImage(),
                 process_image_from_file->processedImage(),
                 cv::Size( Filter_kernel_size, Filter_kernel_size ));
        process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {

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
        cv::GaussianBlur(process_image_from_file->originalImage(),
                         process_image_from_file->processedImage(),
                         cv::Size( Filter_kernel_size, Filter_kernel_size ),
                         0, 0);
        process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {

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
        cv::medianBlur(process_image_from_file->originalImage(),
                       process_image_from_file->processedImage(),
                       Filter_kernel_size);
        process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {

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

    const double max_new = 2.0;
    const double scale_factor = max_new / (max_slide - min_slide);

    qDebug() << "Min.S: " << min_slide;
    qDebug() << "Max.S: " << max_slide;
    qDebug() << "SF: " << scale_factor;
    Contrast = position * scale_factor;
    qDebug() << "C: " << Contrast;

    // Check where is the image comming from and apply the filter
    if (Image_from_file)
    {
        // Get the image
        tmp_image = process_image_from_file->originalImage();
        // Transform the image
        tmp_image.convertTo(process_image_from_file->processedImage(), -1,
                            Contrast, Brightness);
        process_image_from_file->show_image();
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
        tmp_image = process_image_from_file->originalImage();
        // Transform the image
        tmp_image.convertTo(process_image_from_file->processedImage(), -1,
                            Contrast, Brightness);
        process_image_from_file->show_image();
    }

    if (Image_from_screen)
    {

    }

    if (Image_from_camera)
    {

    }
}
