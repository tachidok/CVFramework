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

    // Returns the x and y position of the window that best matches the pattern
    const unsigned search_pattern(cv::Mat &image_pt, unsigned &x, unsigned &y,
                                  const unsigned half_search_window_size,
                                  double &equivalence_value);

    // Updates the pattern to search for
    void update_pattern(cv::Mat &image_pt, const unsigned x, const unsigned y);

    // Read-write access to minimum probability
    double &minimum_probability() {return Minimum_probability;}

    // Read only minimum probability
    double minimum_probability() const {return Minimum_probability;}

protected:

    double Minimum_probability;
    double Maximum_probability;

};

#endif // CCNORMALDISTTRACKER_H
