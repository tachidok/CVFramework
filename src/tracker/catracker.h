#ifndef CATRACKER_H
#define CATRACKER_H

#include <QDebug>
#include <QImage>
#include <QVector>
#include <QColor>
#include <QRgb>

#include <cmath>

// ======================================
// OpenCV includes
// ======================================
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

// Abstract class to implement a tracker
class CATracker
{

public:

    // Empty constructor
    CATracker();

    // Empty destructor
    virtual ~CATracker();

    // Initialise the tracker. Sets the pattern to search for
    virtual const unsigned initialise(cv::Mat &image_pt,
                                      const unsigned x, const unsigned y,
                                      const unsigned half_pattern_size);

    // Updates the pattern to search for
    void update_pattern(cv::Mat &image_pt, const unsigned x, const unsigned y);

    // Deletes the pattern
    void delete_pattern();

    // Checks whether the tracker has been initialised
    bool initialised();

    // Pure virtual function to be implemented by derived classes. Returns the
    // x and y position of the window that best matches the pattern
    virtual const unsigned search_pattern(cv::Mat &image_pt, unsigned &x, unsigned &y,
                                          const unsigned half_search_window_size,
                                          double &equivalence_value) = 0;

    // Get the equivalence values
    QVector<double> get_equivalence_value() {return Equivalence_values;}

    // Get access to the pattern
    cv::Mat *pattern_pt() {return Pattern_pt;}

protected:

    // Store the pattern
    cv::Mat *Pattern_pt;

    // The size of the pattern (half size)
    unsigned Half_pattern_size;

    // The size of the search window (half size)
    unsigned Half_search_window_size;

    // The equivalence values
    QVector<double> Equivalence_values;

};

#endif // CATRACKER_H
