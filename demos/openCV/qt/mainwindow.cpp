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
