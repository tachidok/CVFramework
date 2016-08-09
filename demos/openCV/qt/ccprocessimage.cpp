#include "ccprocessimage.h"

// ===================================================================
// Constructor
// ===================================================================
CCProcessImage::CCProcessImage()
{
    // Initialize contrast and brightness
    Contrast = 1.0;
    Brightness = 0.0;

    // Initialize scale and rotation angle
    Scale = 1.0;
    Angle = 0.0;

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
// Apply zoom
// ===================================================================
void CCProcessImage::zoom(cv::Mat &input_image, cv::Mat &output_image,
                          const double zoom)
{
    Scale=zoom;
    // Get the rotation point
    cv::Point center = cv::Point(input_image.cols*0.5, input_image.rows*0.5);
    cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, Angle, Scale);

    cv::warpAffine(input_image, output_image,
                   rotation_matrix,
                   input_image.size());

    // Apply zoom in to the image
    //cv::pyrUp(input_image, output_image);
    //cv::pyrDown(input_image, output_image);
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

// ===================================================================
// Apply personalised filter
// ===================================================================
void CCProcessImage::apply_personalised_filter(cv::Mat &input_image,
                                               cv::Mat &output_image,
                                               cv::Mat &kernel)
{
    // Set the kernel
    Kernel = kernel.clone();

    // Convert the image to grayscale
    cvtColor(input_image, output_image,CV_BGR2GRAY);

    // Image depth (same depth as source image)
    int depth =  -1;

    // Apply filter
    cv::filter2D(output_image, output_image, depth, Kernel);
}

// ===================================================================
// Canny edge detector
// ===================================================================
void CCProcessImage::canny_edge_detector(cv::Mat &input_image,
                                         cv::Mat &output_image,
                                         const double low_threshold)
{
    const int kernel_size = 3;
    const double ratio = 3.0;

    cv::Mat edges(input_image.size(), input_image.type());

    /// Convert the image to grayscale
    cv::cvtColor(input_image, edges, CV_BGR2GRAY);

    // Reduce noise with a kernel 3x3
    cv::blur(edges, edges, cv::Size(kernel_size, kernel_size));

    // Canny detector
    cv::Canny(edges, edges, low_threshold, low_threshold*ratio, kernel_size);

    output_image = cv::Scalar::all(0);

    input_image.copyTo(output_image, edges);
}
