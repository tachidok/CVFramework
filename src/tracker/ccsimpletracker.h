#ifndef CCSIMPLETRACKER_H
#define CCSIMPLETRACKER_H

#include "catracker.h"

class CCSimpleTracker : public virtual CATracker
{

public:

    // Empty constructor
    CCSimpleTracker();

    // Empty constructor
    ~CCSimpleTracker();

    // Returns the centroid of the window that best matches the pattern
    const unsigned search_pattern(cv::Mat &image_pt,
                                  unsigned &centroid_x,
                                  unsigned &centroid_y);

};

#endif // CCSIMPLETRACKER_H
