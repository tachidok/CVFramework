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

    // Returns the x and y position of the window that best matches the pattern
    const unsigned search_pattern(cv::Mat &image_pt, unsigned &x, unsigned &y,
                                  const unsigned half_search_window_size,
                                  double &equivalence_value);

};

#endif // CCSIMPLETRACKER_H
