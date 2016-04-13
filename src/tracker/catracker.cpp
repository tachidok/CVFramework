#include "catracker.h"

// ======================================================================
// Empty constructor
// ======================================================================
CATracker::CATracker()
{
    // Initialise patter to zero
    Pattern_pt = 0;

    // Initialise pattern size
    Half_pattern_size = 0;

    // Initialise search window size
    Half_search_window_size = 0;

}

// ======================================================================
// Empty destructor
// ======================================================================
CATracker::~CATracker() { }

// ======================================================================
// Initialise the tracker. Sets the pattern to search for
// ======================================================================
const unsigned CATracker::initialise(cv::Mat &image_pt,
                                     const unsigned centroid_x,
                                     const unsigned centroid_y,
                                     const unsigned half_search_window_size,
                                     const unsigned half_pattern_size)
{
  // Initialise search window
  Half_search_window_size = half_search_window_size;

  // Check we are inside the limits
  if (Half_search_window_size > centroid_x ||
          Half_search_window_size > centroid_y ||
          Half_search_window_size*2 > image_pt.cols ||
          Half_search_window_size*2 > image_pt.rows)
  {return 1;}

  // Set the patter size
  Half_pattern_size = half_pattern_size;

  // Delete the previous pattern
  delete_pattern();

  // Get a copy of the new pattern from the input image
  Pattern_pt = new cv::Mat(image_pt, cv::Rect(centroid_x-Half_pattern_size,
                                              centroid_y-Half_pattern_size,
                                              Half_pattern_size*2,
                                              Half_pattern_size*2));

  // Change to gray-scale
  cv::cvtColor(*Pattern_pt, *Pattern_pt, CV_BGR2GRAY);

  return 0;

}

// ======================================================================
// Updates the pattern to search for based on the update method
// ======================================================================
void CATracker::update_pattern(cv::Mat &image_pt,
                               std::string &update_method,
                               const unsigned centroid_x,
                               const unsigned centroid_y)
{

    // Update methods
    std::string copy("copy");
    std::string weighted("weighted");

    // zero if they are equal, otherwise any other value
    if (!(update_method.compare(copy)))
    {
        // Copy the sub-image around the given coordinates
        initialise(image_pt, centroid_x, centroid_y,
                   Half_search_window_size, Half_pattern_size);
    }
    else if (!(update_method.compare(weighted)))
    {
        update_pattern_weighted_private(image_pt,
                                        centroid_x,
                                        centroid_y);
    }
    else // default:
    {
        // Copy the sub-image around the given coordinates
        initialise(image_pt, centroid_x, centroid_y,
                   Half_search_window_size, Half_pattern_size);
    }

}

// ======================================================================
// Updates the pattern to search for based on the distance to the centroid,
// gives more weight (or updates more) to the values close to the
// centroid
// ======================================================================
void CATracker::update_pattern_weighted_private(cv::Mat &image_pt,
                                                const unsigned centroid_x,
                                                const unsigned centroid_y)
{
    // Check we are inside the limits
    if (Half_search_window_size > centroid_x ||
            Half_search_window_size > centroid_y ||
            Half_search_window_size*2 > image_pt.cols ||
            Half_search_window_size*2 > image_pt.rows)
    {return;}

    // Backup the previous pattern
    cv::Mat previous_pattern = Pattern_pt->clone();

    // Delete the previous pattern
    delete_pattern();

    // Get a copy of the new pattern from the input image
    Pattern_pt = new cv::Mat(image_pt,
                             cv::Rect(centroid_x-Half_pattern_size,
                                      centroid_y-Half_pattern_size,
                                      Half_pattern_size*2,
                                      Half_pattern_size*2));

    // Change to gray-scale
    cv::cvtColor(*Pattern_pt, *Pattern_pt, CV_BGR2GRAY);

    // Get the number of channels of the pattern
    const unsigned n_channels_pattern = Pattern_pt->channels();

    // Get the size of the smallest pattern to loop around
    const unsigned size_h =
            std::min(previous_pattern.rows, Pattern_pt->rows);
    const unsigned size_w =
            std::min(previous_pattern.cols, Pattern_pt->cols);

    // Maximum distance to centre
    const double max_dist =
            sqrt(((size_h/2.0) * (size_h/2.0)) + ((size_w/2.0) * (size_w/2.0)));

    // Update the pattern giving more weight to the pixels in the center of
    // the Pattern window
    for (unsigned i = 0; i < size_h; i++)
    {
        // Get a pointer to the i-row in the previous_pattern
        unsigned char *previous_pattern_row_pt = previous_pattern.ptr<uchar>(i);
        // Get a pointer to the jp-row in the pattern
        unsigned char *pattern_row_pt = Pattern_pt->ptr<uchar>(i);
        for (unsigned j = 0; j < size_w; j++)
        {
            // Get the distance to the centre
            const double distance =
                    sqrt(((size_h/2.0) - i) * ((size_h/2.0) - i) + ((size_w/2.0) - j) * ((size_w/2.0) - j));
            // Get the weight
            const double weight = 1.0 - (distance * 1.0/max_dist);

            // Extract a pixel from the previous pattern
            const unsigned pixel_previous_pattern =
                    previous_pattern_row_pt[(j * n_channels_pattern) + 0];
            // Extract a pixel from the pattern
            const unsigned pixel_pattern =
                    pattern_row_pt[(j * n_channels_pattern) + 0];
            // Get the new value of the pixel
            //const unsigned new_gray_value_pixel = ((pixel_pattern * weight) + pixel_previous_pattern)/2.0;
            //const unsigned new_gray_value_pixel = ((pixel_pattern + pixel_previous_pattern)/2.0) * weight;
            const unsigned new_gray_value_pixel = pixel_pattern * weight + pixel_previous_pattern * (1.0-weight);
            pattern_row_pt[(j * n_channels_pattern) + 0] = new_gray_value_pixel;
        }
    }

}

// ======================================================================
// Deletes the pattern
// ======================================================================
void CATracker::delete_pattern()
{
    if (Pattern_pt != 0)
    {
      delete Pattern_pt;
      Pattern_pt = 0;
    }

}

// ======================================================================
// Checks whether the tracker has been initialised
// ======================================================================
bool CATracker::initialised()
{
    if (Pattern_pt != 0)
    {
      return true;

    }

    return false;

}
