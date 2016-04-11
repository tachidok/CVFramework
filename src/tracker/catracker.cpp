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
// Sets the pattern to search for
// ======================================================================
const unsigned CATracker::set_pattern(cv::Mat &image_pt,
                                      const unsigned x, const unsigned y,
                                      const unsigned half_pattern_size)
{
  // Check we are inside the limits
  if (Half_search_window_size > x || Half_search_window_size > y ||
      Half_search_window_size*2 > image_pt.cols ||
      Half_search_window_size*2 > image_pt.rows)
  {return 1;}

  // Set the patter size
  Half_pattern_size = half_pattern_size;

  // Delete the previous pattern
  delete_pattern();

  // Get a copy of the new pattern from the input image
  Pattern_pt = new cv::Mat(image_pt, cv::Rect(x-Half_pattern_size,
                                              y-Half_pattern_size,
                                              Half_pattern_size*2,
                                              Half_pattern_size*2));

  // Change to gray-scale
  cv::cvtColor(*Pattern_pt, *Pattern_pt, CV_BGR2GRAY);

  return 0;

}

// ======================================================================
// Updates the pattern to search for
// ======================================================================
void CATracker::update_pattern(cv::Mat &image_pt, const unsigned x,
                               const unsigned y)
{

    // Updates the pattern
    set_pattern(image_pt, x, y, Half_pattern_size);

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
// Checks whether the pattern has been set
// ======================================================================
bool CATracker::is_pattern_set()
{
    if (Pattern_pt != 0)
    {
      return true;

    }

    return false;

}
