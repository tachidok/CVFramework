#include "ccopencvtracker.h"

// ======================================================================
// Constructor
// ======================================================================
CCOpenCVTracker::CCOpenCVTracker(const std::string &tracker_name)
    : CATracker(), Tracker_name(tracker_name)
{ }

// ======================================================================
// Empty destructor (memory is automatically unallocated by openCV)
// ======================================================================
CCOpenCVTracker::~CCOpenCVTracker() { }

// ======================================================================
// Initialise the tracker. Sets the pattern to search for
// ======================================================================
const unsigned CCOpenCVTracker::initialise(cv::Mat &image_pt,
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
          static_cast<int>(Half_search_window_size)*2 > image_pt.cols ||
          static_cast<int>(Half_search_window_size)*2 > image_pt.rows)
  {return 1;}

  // Free anything within the tracker
  if (opencv_tracker_pt!=0)
  {
      opencv_tracker_pt->clear();
  }

  // Create a cv::string from the input string
  cv::String cv_tracker_name(Tracker_name);
  // Create an instance of the specified openCV tracker
  opencv_tracker_pt = cv::Tracker::create(cv_tracker_name);
  if (opencv_tracker_pt == 0)
  {
      // Error
      return 1;
  }

  // Set the patter size
  Half_pattern_size = half_pattern_size;

  // Delete the previous pattern
  delete_pattern();

  // Create a bounding box
  cv::Rect bounding_box(centroid_x-Half_pattern_size,
                        centroid_y-Half_pattern_size,
                        Half_pattern_size*2,
                        Half_pattern_size*2);

  // Pass the whole image to the tracker, indicating the bounding box
  if (!(opencv_tracker_pt->init(image_pt, bounding_box)))
  {
      // Error
      return 1;
  }


  // Get a copy of the new pattern from the input image
  Pattern_pt = new cv::Mat(image_pt, bounding_box);

  // Change to gray-scale
  cv::cvtColor(*Pattern_pt, *Pattern_pt, CV_BGR2GRAY);

  return 0;

}

// ======================================================================
// Returns the x and y position of the window that best matches the pattern
// ======================================================================
const unsigned CCOpenCVTracker::search_pattern(cv::Mat &image_pt,
                                               unsigned &centroid_x,
                                               unsigned &centroid_y)
{

    // Check we are inside the limits
    if (Half_search_window_size > centroid_x ||
            Half_search_window_size > centroid_y ||
            static_cast<int>(Half_search_window_size)*2 > image_pt.cols ||
            static_cast<int>(Half_search_window_size)*2 > image_pt.rows)
    {return 1;}

    // We need a pattern to search before continue
    // Is there a pattern already set
    if (Pattern_pt == 0)
    {
        // Return inmediately
        return 1;
    }

    // Create a bounding box to indicate where is the target
    cv::Rect2d bounding_box;
    if (!(opencv_tracker_pt->update(image_pt, bounding_box)))
    {
        // Error
        return 1;
    }

    // --------------------------------------------------------------------
    // Update the pattern
    // --------------------------------------------------------------------
    delete_pattern();

    // Get a copy of the new pattern from the input image
    Pattern_pt = new cv::Mat(image_pt, bounding_box);

    // Change to gray-scale
    cv::cvtColor(*Pattern_pt, *Pattern_pt, CV_BGR2GRAY);

    // --------------------------------------------------------------------
    // Return the x and y position based on the complete image
    // --------------------------------------------------------------------
    //qDebug() << "InputX: " << x << "InputY: " << y;

    centroid_x = bounding_box.x + bounding_box.width/2.0;
    centroid_y = bounding_box.y + bounding_box.height/2.0;

    //qDebug() << "OutputX: " << x << "OutputY: " << y;

    return 0;

}
