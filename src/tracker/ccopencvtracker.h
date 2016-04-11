#ifndef CCOPENCVTRACKER_H
#define CCOPENCVTRACKER_H

#include "catracker.h"
#include <string>

// OpenCV headers
#include <opencv2/core/utility.hpp>
//#include <opencv2/video/tracker.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/highgui.hpp>

class CCOpenCVTracker : public virtual CATracker
{

public:

    // Constructor
    CCOpenCVTracker(const std::string &tracker_name);

    // Empty destructor
    ~CCOpenCVTracker();

    // Sets the pattern to search for
    const unsigned initialise(cv::Mat &image_pt,
                              const unsigned x, const unsigned y,
                              const unsigned half_pattern_size);

    // Returns the x and y position that best matches the pattern
    const unsigned search_pattern(cv::Mat &image_pt, unsigned &x, unsigned &y,
                                  const unsigned half_search_window_size,
                                  double &equivalence_value);

protected:

    // An instance of the tracker from openCV
    cv::Ptr<cv::Tracker> opencv_tracker_pt;

};

#endif // CCOPENCVTRACKER_H
