#include "ccprocessimage.h"

// ===================================================================
// Constructor
// ===================================================================
CCProcessImage::CCProcessImage()
{
    // Initialize contrast and brightness
    Contrast = 1.0;
    Brightness = 0.0;
}

// ===================================================================
// Apply the normalised filter to the captured image
// ===================================================================
void CCProcessImage::apply_normalised_filter(cv::Mat &input_image,
                                             cv::Mat &output_image)
{
    cv::blur(input_image, output_image,
             cv::Size( Filter_kernel_size, Filter_kernel_size ));
}

// ===================================================================
// Apply the gaussiang filter to the captured image
// ===================================================================
void CCProcessImage::apply_gaussian_filter(cv::Mat &input_image,
                                           cv::Mat &output_image)
{
    cv::GaussianBlur(input_image, output_image,
                     cv::Size( Filter_kernel_size, Filter_kernel_size),
                     0, 0);
}

// ===================================================================
// Apply the median filter to the captured image
// ===================================================================
void CCProcessImage::apply_median_filter(cv::Mat &input_image,
                                         cv::Mat &output_image)
{
    cv::medianBlur(input_image, output_image,
                   Filter_kernel_size);
}

// ===================================================================
// Contrast
// ===================================================================
void CCProcessImage::apply_contrast(cv::Mat &input_image,
                                    cv::Mat &output_image,
                                    const double contrast)
{
    // Set the contrast
    Contrast = contrast;
    // Apply the contrast
    input_image.convertTo(output_image, -1, Contrast, Brightness);
}

// ===================================================================
// Brightness
// ===================================================================
void CCProcessImage::apply_brightness(cv::Mat &input_image,
                                      cv::Mat &output_image,
                                      const double brightness)
{
    // Set the brightness
    Brightness = brightness;
    // Apply the contrast
    input_image.convertTo(output_image, -1, Contrast, Brightness);
}

// ===================================================================
// Histogram equalisation
// ===================================================================
void CCProcessImage::apply_histogram_equalisation(cv::Mat &input_image,
                                                  cv::Mat &output_image)
{
    // Convert to grayscale
    cv::cvtColor(input_image, output_image, CV_BGR2GRAY);

    // Apply Histogram Equalization
    cv::equalizeHist(output_image, output_image);
}

// ===================================================================
// Apply zoom in
// ===================================================================
void CCProcessImage::zoom_in(cv::Mat &input_image, cv::Mat &output_image)
{
    // Apply zoom in to the image
    cv::pyrUp(input_image, output_image);
}

// ===================================================================
// Apply zoom out
// ===================================================================
void CCProcessImage::zoom_out(cv::Mat &input_image, cv::Mat &output_image)
{
    // Apply zoom up to the image
    cv::pyrDown(input_image, output_image);
}

// ===================================================================
// Rotate an image
// ===================================================================
void CCProcessImage::rotate(cv::Mat &input_image, cv::Mat &output_image,
                            const double angle, const double scale)
{
    Angle = angle;
    Scale = scale;
    // Get the rotation point
    cv::Point center = cv::Point(input_image.cols*0.5, input_image.rows*0.5);
    cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, Angle, Scale);

    cv::warpAffine(input_image, output_image,
                   rotation_matrix,
                   input_image.size());
}
