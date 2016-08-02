#-------------------------------------------------
#
# Project created by QtCreator 2016-07-27T16:50:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt
TEMPLATE = app

# Include all paths for libraries associated with opencv
INCLUDEPATH += /usr/local/include/opencv

# Link to openCV (include all)
LIBS += -L/usr/local/lib -lopencv_stitching -lopencv_superres\
 -lopencv_videostab -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired\
 -lopencv_ccalib -lopencv_dnn -lopencv_dpm -lopencv_fuzzy\
 -lopencv_line_descriptor -lopencv_optflow -lopencv_plot -lopencv_reg\
 -lopencv_saliency -lopencv_stereo -lopencv_structured_light -lopencv_rgbd\
 -lopencv_surface_matching -lopencv_tracking -lopencv_datasets -lopencv_text\
 -lopencv_face -lopencv_xfeatures2d -lopencv_shape -lopencv_video\
 -lopencv_ximgproc -lopencv_calib3d -lopencv_features2d -lopencv_flann\
 -lopencv_xobjdetect -lopencv_objdetect -lopencv_ml -lopencv_xphoto\
 -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_photo\
 -lopencv_imgproc -lopencv_core

SOURCES += main.cpp\
        mainwindow.cpp \
     ../../../src/video_capture/ac_capture_thread.cpp \
    ccprocessimagefromfile.cpp \
    ccprocessimagefromscreen.cpp \
    ccprocessimage.cpp \
    ../../../src/video_capture/cc_capture_from_screen.cpp \
    ../../../src/video_capture/cc_capture_from_camera.cpp \
    ccprocessimagefromcamera.cpp

HEADERS  += mainwindow.h \
    ../../../src/video_capture/ac_capture_thread.h \
    ccprocessimagefromfile.h \
    ccprocessimagefromscreen.h \
    ccprocessimage.h \
    ../../../src/video_capture/cc_capture_from_screen.h \
    ../../../src/video_capture/cc_capture_from_camera.h \
    ccprocessimagefromcamera.h

FORMS    += mainwindow.ui
