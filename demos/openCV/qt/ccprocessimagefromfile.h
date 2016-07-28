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

signals:

public slots:

protected:

    // The storage for the captured image
    cv::Mat Image;

    // The name of the window that shows the image
    char *Window_image = "CCProcessImageFromFile::image";

    // Image read status
    bool Image_read;

};

#endif // CCPROCESSIMAGEFROMFILE_H
