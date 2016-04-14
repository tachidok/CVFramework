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
    bool initialise(cv::Mat &image_pt,
                    const unsigned centroid_x,
                    const unsigned centroid_y,
                    const unsigned half_search_window_size,
                    const unsigned half_pattern_size);

    // Returns the x and y position that best matches the pattern
    bool search_pattern(cv::Mat &image_pt,
                        unsigned &centroid_x,
                        unsigned &centroid_y);

protected:

    // An instance of the tracker from openCV
    cv::Ptr<cv::Tracker> opencv_tracker_pt;

    // Store the tracker name
    const std::string Tracker_name;

};

#endif // CCOPENCVTRACKER_H
