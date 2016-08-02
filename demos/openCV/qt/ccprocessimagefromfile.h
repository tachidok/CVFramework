#ifndef CCPROCESSIMAGEFROMFILE_H
#define CCPROCESSIMAGEFROMFILE_H

#include <QObject>

// -----------------------------------------------------------------------
// General includes
#include "../../../src/general/general_includes.h"
// -----------------------------------------------------------------------

class CCProcessImageFromFile : public QObject
{
    Q_OBJECT
public:
    explicit CCProcessImageFromFile(QObject *parent = 0);

    // Destructor
    ~CCProcessImageFromFile();

    // Read the image from file
    bool read_image(std::string &image_name);

    // Show image in the selected window
    void show_image();

    // Free some stuff but do not kill the object
    void free_stuff();

    // Clear applied process
    void clear();

    // Get access to the original image
    inline cv::Mat &originalImage() {return OriginalImage;}

    // Get access to the processed image
    inline cv::Mat &processedImage() {return ProcessedImage;}

signals:

public slots:

protected:

    // The storage for the captured and processed image
    cv::Mat OriginalImage;
    cv::Mat ProcessedImage;

    // The name of the window that shows the image
    char *Window_original_image = "CCProcessImageFromFile - Original";
    char *Window_processed_image = "CCProcessImageFromFile - Processed";

    // Image read status
    bool Image_read;

};

#endif // CCPROCESSIMAGEFROMFILE_H
