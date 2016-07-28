#include "ccprocessimagefromfile.h"

// Constructor
CCProcessImageFromFile::CCProcessImageFromFile(QObject *parent) : QObject(parent)
{
    // Initialise variables
    Image_read = false;
}

// Destructor
CCProcessImageFromFile::~CCProcessImageFromFile()
{
    // Free image
    free_stuff();
}

// Read the image from file
bool CCProcessImageFromFile::read_image(std::string &image_name)
{
    // Create and show the window where the image is shown
    cv::namedWindow(Window_image);

    // Load the image
    Image = cv::imread(image_name.c_str(), CV_LOAD_IMAGE_COLOR);

    // Check for correct image reading
    if(!Image.data)
    {
        std::cout <<  "Could not open or find the image" << std::endl;
        return false;
    }

    // The image has been read
    Image_read = true;

    // Show the image
    show_image();

    return true;

}

 // Show image in the selected window
void CCProcessImageFromFile::show_image()
{
    if (Image_read)
    {
        // Just show the image
        imshow(Window_image, Image);
    }
}

// Free some stuff but do not kill the object
void CCProcessImageFromFile::free_stuff()
{
    // Set as image not read
    Image_read = false;

    // Destroy the window showing the image
    cv::destroyWindow(Window_image);
}
