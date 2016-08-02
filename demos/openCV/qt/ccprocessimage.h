#ifndef CCPROCESSIMAGE_H
#define CCPROCESSIMAGE_H

// -----------------------------------------------------------------------
// General includes
#include "../../../src/general/general_includes.h"
// -----------------------------------------------------------------------

class CCProcessImage
{

public:

    // Constructor
    CCProcessImage();

    // Empty destructor
    virtual ~CCProcessImage() { }

    // Get access to the original image
    inline cv::Mat &originalImage() {return OriginalImage;}

    // Get access to the processed image
    inline cv::Mat &processedImage() {return ProcessedImage;}

    // -------------------------------------------------------------------
    // Image processing
    // -------------------------------------------------------------------

    // -------------------------------------------------------------------
    // Filters
    // -------------------------------------------------------------------

    // Set the filter kernel size
    inline unsigned &filter_kernel_size() {return Filter_kernel_size;}

    // Apply the normalised filter to the captured image
    void apply_normalised_filter(cv::Mat &input_image, cv::Mat &output_image);

    // Apply the gaussiang filter to the captured image
    void apply_gaussian_filter(cv::Mat &input_image, cv::Mat &output_image);

    // Apply the median filter to the captured image
    void apply_median_filter(cv::Mat &input_image, cv::Mat &output_image);

    // -------------------------------------------------------------------
    // Contrast
    // -------------------------------------------------------------------
    void apply_contrast(cv::Mat &input_image,
                        cv::Mat &output_image,
                        const double contrast);

    // Set contrast
    inline double &contrast() {return Contrast;}

    // -------------------------------------------------------------------
    // Brightness
    // -------------------------------------------------------------------
    void apply_brightness(cv::Mat &input_image,
                          cv::Mat &output_image,
                          const double brightness);

    // Set brightness
    inline double &brightness() {return Brightness;}

    // -------------------------------------------------------------------
    // Histogram equalisation
    // -------------------------------------------------------------------
    void apply_histogram_equalisation(cv::Mat &input_image,
                                      cv::Mat &output_image);

    // -------------------------------------------------------------------
    // Zoom
    // -------------------------------------------------------------------
    // Apply zoom in
    void zoom_in(cv::Mat &input_image, cv::Mat &output_image);
    // Apply zoom out
    void zoom_out(cv::Mat &input_image, cv::Mat &output_image);

    // -------------------------------------------------------------------
    // Rotations
    // -------------------------------------------------------------------
    // Rotate an image
    void rotate(cv::Mat &input_image, cv::Mat &output_image,
                const double angle, const double scale);

    // Set the scale factor for rotation
    inline double &scale(){return Scale;}

    // Set the angle factor for rotation
    inline double &angle(){return Angle;}

protected:

    // The storage for the captured and processed image
    cv::Mat OriginalImage;
    cv::Mat ProcessedImage;

    // -------------------------------------------------------------------
    // Image processing
    // -------------------------------------------------------------------

    // Contrast and brightness (common variables for all images)
    double Contrast;
    double Brightness;

    // -------------------------------------------------------------------
    // Filters
    // -------------------------------------------------------------------
    unsigned Filter_kernel_size;

    // -------------------------------------------------------------------
    // Rotations
    // -------------------------------------------------------------------
    // The scale required to perform rotations
    double Scale;
    // The rotation angle
    double Angle;

};

#endif // CCPROCESSIMAGE_H
