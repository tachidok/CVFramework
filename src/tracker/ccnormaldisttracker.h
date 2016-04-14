#ifndef CCNORMALDISTTRACKER_H
#define CCNORMALDISTTRACKER_H

#include "catracker.h"

class CCNormalDistTracker : public virtual CATracker
{

public:

    // Empty constructor
    CCNormalDistTracker();

    // Empty constructor
    ~CCNormalDistTracker();

    // Returns the centroid of the window that best matches the pattern
    bool search_pattern(cv::Mat &image_pt,
                        unsigned &centroid_x,
                        unsigned &centroid_y);

    // Read-write access to minimum probability
    double &minimum_probability() {return Minimum_probability;}

    // Read only minimum probability
    double minimum_probability() const {return Minimum_probability;}

protected:

    double Minimum_probability;
    double Maximum_probability;

};

#endif // CCNORMALDISTTRACKER_H
