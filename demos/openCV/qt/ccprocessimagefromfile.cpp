#include "ccprocessimagefromfile.h"

// ===================================================================
// Constructor
// ===================================================================
CCProcessImageFromFile::CCProcessImageFromFile()
{
    // Initialise variables
    Image_read = false;
}

// ===================================================================
// Destructor
// ===================================================================
CCProcessImageFromFile::~CCProcessImageFromFile()
{
    // Free image
    free_stuff();
}

// ===================================================================
// Read the image from file
// ===================================================================
bool CCProcessImageFromFile::read_image(std::string &image_name)
{
    // Create and show the window where the image is shown
    cv::namedWindow(Window_original_image);

    // Load the image
    OriginalImage = cv::imread(image_name.c_str(), CV_LOAD_IMAGE_COLOR);

    // Check for correct image reading
    if(!OriginalImage.data)
    {
        std::cout <<  "Could not open or find the image" << std::endl;
        return false;
    }

    // Make a copy of the original image into the processed image
    ProcessedImage = OriginalImage.clone();

    // The image has been read
    Image_read = true;

    // Show the image
    show_image();

    return true;

}

// ===================================================================
// Show image in the selected window
// ===================================================================
void CCProcessImageFromFile::show_image()
{
    if (Image_read)
    {
        // Just show the images
        imshow(Window_original_image, OriginalImage);
        imshow(Window_processed_image, ProcessedImage);

    }
}

// ===================================================================
// Free some stuff but do not kill the object
// ===================================================================
void CCProcessImageFromFile::free_stuff()
{
    // Set as image not read
    Image_read = false;

    // Destroy the window showing the image
    cv::destroyWindow(Window_original_image);
    cv::destroyWindow(Window_processed_image);

}

// ===================================================================
// Clear applied process
// ===================================================================
void CCProcessImageFromFile::clear()
{
    if (Image_read)
    {
        // Copy back the original image into the processed image
        ProcessedImage = OriginalImage.clone();
        show_image();
    }
}
