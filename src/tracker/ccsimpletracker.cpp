#include "ccsimpletracker.h"

// ======================================================================
// Empty constructor
// ======================================================================
CCSimpleTracker::CCSimpleTracker() : CATracker() { }

// ======================================================================
// Empty destructor
// ======================================================================
CCSimpleTracker::~CCSimpleTracker() { }

// ======================================================================
// Returns the x and y position of the window that best matches the pattern
// ======================================================================
const unsigned CCSimpleTracker::search_pattern(cv::Mat &image_pt,
                                               unsigned &x, unsigned &y,
                                               const unsigned half_search_window_size,
                                               double &equivalence_value)
{

    // Check we are inside the limits
    if (Half_search_window_size > x || Half_search_window_size > y ||
        Half_search_window_size*2 > image_pt.cols ||
        Half_search_window_size*2 > image_pt.rows)
    {return 1;}

    // We need a pattern to search before continue
    // Is there a pattern already set
    if (Pattern_pt == 0)
    {
        // Return inmediately
        return 1;
    }

    // Set the size for the search window
    Half_search_window_size = half_search_window_size;

    // --------------------------------------------------------------------
    // Get a copy of the sub-image with size "half_search_window_size"
    // --------------------------------------------------------------------
    // Get a copy of the new pattern from the input image
    cv::Mat sub_image(image_pt, cv::Rect(x-Half_search_window_size,
                                         y-Half_search_window_size,
                                         Half_search_window_size*2,
                                         Half_search_window_size*2));

    // Change to gray-scale
    cv::cvtColor(sub_image, sub_image, CV_BGR2GRAY);

    // Get the number of channels of the sub-image
    const unsigned n_channels_sub_image = sub_image.channels();

    // Get the size of the sub-image
    const unsigned width_sub_image = sub_image.cols;
    const unsigned height_sub_image = sub_image.rows;

    // Get the number of channels of the pattern
    const unsigned n_channels_pattern = Pattern_pt->channels();

    // Get the size of the pattern
    const unsigned w_pattern = Pattern_pt->cols;
    const unsigned h_pattern = Pattern_pt->rows;

    // --------------------------------------------------------------------
    // Do the search
    // --------------------------------------------------------------------
    // Get the limits to search in the search window
    const unsigned left_limit = w_pattern/2;
    const unsigned right_limit = width_sub_image - w_pattern/2;
    const unsigned lower_limit = h_pattern/2;
    const unsigned upper_limit = height_sub_image - h_pattern/2;

    // Get the range for the search window
    const unsigned range_h = std::abs(upper_limit - lower_limit);
    const unsigned range_w = std::abs(right_limit - left_limit);

    // Create a matrix to store the equivalence values (for each position
    // searched in the serach window)
    QVector<QVector<double> > equivalences(range_h);
    for (unsigned i = 0; i < range_h; i++)
    {
        equivalences[i].resize(range_w);
    }

    // Get the equivalences by comparing the pattern with each sub-pattern
    // window in the search window
    for (unsigned js = lower_limit; js < upper_limit; js++)
    {
        for (unsigned is = left_limit; is < right_limit; is++)
        {
            // Initialise the differences for this (js, is) point
            double differences = 0;
            for (unsigned jp = 0; jp < h_pattern; jp++)
            {
                // Get a pointer to the (js- h_pattern/2 + jp)-row in the sub-image
                unsigned char *sub_image_row_pt = sub_image.ptr<uchar>(js - h_pattern/2 + jp);
                // Get a pointer to the jp-row in the pattern
                unsigned char *pattern_row_pt = Pattern_pt->ptr<uchar>(jp);
                for (unsigned ip = 0; ip < w_pattern; ip++)
                {
                    // Extract the corresponding pixel in the search window
                    const unsigned pixel_sub_image =
                            sub_image_row_pt[((is - w_pattern/2 + ip)*n_channels_sub_image) + 0];
                    // Extract a pixel from the pattern
                    const unsigned pixel_pattern =
                            pattern_row_pt[(jp * n_channels_pattern) + 0];
                    // Get the difference between both pixels and normalise
                    differences+=
                     sqrt((pixel_pattern - pixel_sub_image)*
                          (pixel_pattern - pixel_sub_image)) / 255; // Normalised
                }
            }

            // Get the normalised average of the differences between all the
            // pixels in the pattern and the current sub-image
            // in the search window
            const double norm_averaged_differences =
                    differences / (w_pattern * h_pattern);

            // The greater the difference, the worse the correspondence
            equivalences[js - h_pattern/2][is - w_pattern/2] = 1.0 - norm_averaged_differences;

        } // for (is < right_limit)

    } // for (js < upper_limit)

    // --------------------------------------------------------------------
    // Get the maximum the equivalence data and the position in the search
    // window
    // --------------------------------------------------------------------
    double max = 0;
    unsigned maxi = 0;
    unsigned maxj = 0;

    // Get the maximum in the matrix of equivalence data. Also get the
    // position of the max
    for (unsigned j = 0; j < range_h; j++)
    {
        for (unsigned i = 0; i < range_w; i++)
        {
            if (equivalences[j][i] > max)
            {
                max = equivalences[j][i];
                maxi = i;
                maxj = j;
            }
        }
    }

#if 0
    // Print the equivalence matrix
    qDebug() << "Equivalences matrix:";
    for (unsigned j= 0; j < range_h; j++)
    {
        QString print_string;
        for (unsigned i = 0; i < range_w; i++)
        {
            print_string.append(QString::number(equivalences[j][i]));
            print_string.append(" ");
        }
        qDebug() << print_string;
    }
#endif // #if 0

    // Set the equivalence value to be read from external method
    Equivalence_values.resize(1);
    Equivalence_values[0] = equivalence_value = max;

    qDebug() << "Equivalence value: " << Equivalence_values[0];

    // --------------------------------------------------------------------
    // Update the pattern
    // --------------------------------------------------------------------
    update_pattern(image_pt, x, y);

    // --------------------------------------------------------------------
    // Return the x and y position based on the complete image
    // --------------------------------------------------------------------
    qDebug() << "InputX: " << x << "InputY: " << y;

    x = x - range_w/2 + maxi;
    y = y - range_h/2 + maxj;

    qDebug() << "OutputX: " << x << "OutputY: " << y;

    return 0;

}
