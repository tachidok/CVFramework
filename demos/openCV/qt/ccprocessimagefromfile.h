#ifndef CCPROCESSIMAGEFROMFILE_H
#define CCPROCESSIMAGEFROMFILE_H

// The base class
#include "ccprocessimage.h"

class CCProcessImageFromFile : virtual public CCProcessImage
{

public:

    // Constructor
    CCProcessImageFromFile();

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

protected:

    // The name of the window that shows the image
    char *Window_original_image = "CCProcessImageFromFile - Original";
    char *Window_processed_image = "CCProcessImageFromFile - Processed";

    // Image read status
    bool Image_read;

};

#endif // CCPROCESSIMAGEFROMFILE_H
