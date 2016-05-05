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
bool CCOpenCVTracker::initialise(cv::Mat &image_pt,
                                 const unsigned centroid_x,
                                 const unsigned centroid_y,
                                 const unsigned half_search_window_size,
                                 const unsigned half_pattern_size)
{
  // Initialise search window
  Half_search_window_size = half_search_window_size;

  // Set the patter size
  Half_pattern_size = half_pattern_size;

  if (!validate_input_centroid(centroid_x, centroid_y,
                               image_pt.cols, image_pt.rows))
  {
      return false;
  }

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
      return false;
  }

  // Delete the previous pattern
  delete_pattern();

  // Only pass a sub-image of the input image to the tracker
  cv::Mat sub_image(image_pt, cv::Rect(centroid_x-Half_search_window_size,
                                       centroid_y-Half_search_window_size,
                                       Half_search_window_size*2,
                                       Half_search_window_size*2));

  // Change to gray-scale (we can't since the tracker methods work with
  // a color image, they internally call this method and if it is called
  // for an already gray image, then it throws an error)
  //cv::cvtColor(sub_image, sub_image, CV_BGR2GRAY);

  // Get the centre of the sub-image (the pattern centroid MUST be at the
  // centre of the sub-image)
  const unsigned sub_image_centroid_x = sub_image.cols/2;
  const unsigned sub_image_centroid_y = sub_image.rows/2;

  // Create a bounding box based on the sub-image to indicate the pattern
  cv::Rect bounding_box(sub_image_centroid_x-Half_pattern_size,
                        sub_image_centroid_y-Half_pattern_size,
                        Half_pattern_size*2,
                        Half_pattern_size*2);

  // Pass the whole image to the tracker, indicating the bounding box
  if (!(opencv_tracker_pt->init(sub_image, bounding_box)))
  {
      // Error
      return false;
  }


  // Get a copy of the new pattern from the input image
  Pattern_pt = new cv::Mat(sub_image, bounding_box);

  // Change to gray-scale
  cv::cvtColor(*Pattern_pt, *Pattern_pt, CV_BGR2GRAY);

  return true;

}

// ======================================================================
// Returns the x and y position of the window that best matches the pattern
// ======================================================================
bool CCOpenCVTracker::search_pattern(cv::Mat &image_pt,
                                     unsigned &centroid_x,
                                     unsigned &centroid_y)
{

    if (!validate_input_centroid(centroid_x, centroid_y,
                                 image_pt.cols, image_pt.rows))
    {
        return false;
    }

    // We need a pattern to search before continue
    // Is there a pattern already set
    if (Pattern_pt == 0)
    {
        // Return inmediately
        return false;
    }

    // Only pass a sub-image of the input image to the tracker (we are no
    // searching in the whole image)
    cv::Mat sub_image(image_pt, cv::Rect(centroid_x-Half_search_window_size,
                                         centroid_y-Half_search_window_size,
                                         Half_search_window_size*2,
                                         Half_search_window_size*2));

    // Change to gray-scale (we can't since the tracker methods work with
    // a color image, they internally call this method and if it is called
    // for an already gray image, then it throws an error)
    //cv::cvtColor(sub_image, sub_image, CV_BGR2GRAY);

    // Create a bounding box to indicate where is the target
    cv::Rect2d bounding_box;
    if (!(opencv_tracker_pt->update(sub_image, bounding_box)))
    {
        // Error
        return false;
    }

    // Validate the bounding box
    if (bounding_box.x + bounding_box.width >= sub_image.cols ||
            bounding_box.y + bounding_box.height >= sub_image.cols)
    {
        // Error
        return false;
    }

#if 0
    qDebug() << "BBoxx: " << bounding_box.x << "BBoxy: " << bounding_box.y;
    qDebug() << "BBoxw: " << bounding_box.width << "BBoxh: " << bounding_box.height;

    qDebug() << "SImg.cols: " << sub_image.cols << "SImg.rows: " << sub_image.rows;
#endif

    // --------------------------------------------------------------------
    // Update the pattern
    // --------------------------------------------------------------------
    delete_pattern();

    // Get a copy of the new pattern from the input image
    Pattern_pt = new cv::Mat(sub_image, bounding_box);

    // Change to gray-scale
    cv::cvtColor(*Pattern_pt, *Pattern_pt, CV_BGR2GRAY);

    // --------------------------------------------------------------------
    // Return the x and y position based on the complete image
    // --------------------------------------------------------------------
    const unsigned local_centroid_x = bounding_box.x + bounding_box.width/2.0;
    const unsigned local_centroid_y = bounding_box.y + bounding_box.height/2.0;

    // Map from local to global coordinates
    centroid_x = centroid_x - Half_search_window_size + local_centroid_x;
    centroid_y = centroid_y - Half_search_window_size + local_centroid_y;

    return true;

}
