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
    virtual bool initialise(cv::Mat &image_pt,
                            const unsigned centroid_x,
                            const unsigned centroid_y,
                            const unsigned half_search_window_size,
                            const unsigned half_pattern_size);

    // Updates the pattern to search for based on the update method
    void update_pattern(cv::Mat &image_pt,
                        std::string &update_method,
                        const unsigned centroid_x,
                        const unsigned centroid_y);

    // Deletes the pattern
    void delete_pattern();

    // Checks whether the tracker has been initialised
    bool initialised();

    // Pure virtual function to be implemented by derived classes. Returns the
    // centroid of the window that best matches the pattern
    virtual bool search_pattern(cv::Mat &image_pt,
                                unsigned &centroid_x,
                                unsigned &centroid_y) = 0;

    // Get the equivalence values
    QVector<double> get_equivalence_value() {return Equivalence_values;}

    // Get access to the pattern
    cv::Mat *pattern_pt() {return Pattern_pt;}

protected:

    // Check that input centroid is valid
    bool validate_input_centroid(unsigned centroid_x,
                                 unsigned centroid_y,
                                 const unsigned image_widht,
                                 const unsigned image_height);

    // Store the pattern
    cv::Mat *Pattern_pt;

    // The size of the pattern (half size)
    unsigned Half_pattern_size;

    // The size of the search window (half size)
    unsigned Half_search_window_size;

    // The equivalence values
    QVector<double> Equivalence_values;

private:

    // Updates the pattern to search for based on the distance to the
    // centroid, gives more weight (or updates more) to the values
    // close to the centroid
    void update_pattern_weighted_private(cv::Mat &image_pt,
                                         const unsigned centroid_x,
                                         const unsigned centroid_y);

};

#endif // CATRACKER_H
