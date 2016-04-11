#-------------------------------------------------
#
# Project created by QtCreator 2016-03-22T11:41:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = tracker_and_kalman
TEMPLATE = app

# Include opencv
INCLUDEPATH += /usr/local/include/opencv /usr/local/include/opencv2
# Link to opencv (very weird, why is kalman filter in opencv_video module?)
#LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs\
# -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc\
# -lopencv_video

# Include all
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
    extended_label.cpp \
    ../../../src/tracker/catracker.cpp \
    ../../../src/tracker/ccnormaldisttracker.cpp \
    ../../../src/tracker/ccsimpletracker.cpp \
    ../../../src/kalman/cckalmanfilter.cpp \
    ../../../external_src/qcustomplot/qcustomplot/qcustomplot.cpp \
    ../../../src/tracker/ccopencvtracker.cpp

HEADERS  += mainwindow.h \
    extended_label.h \
    ../../../src/tracker/catracker.h \
    ../../../src/tracker/ccnormaldisttracker.h \
    ../../../src/tracker/ccsimpletracker.h \
    ../../../src/kalman/cckalmanfilter.h \
    ../../../external_src/qcustomplot/qcustomplot/qcustomplot.h \
    ../../../src/tracker/ccopencvtracker.h

FORMS    += mainwindow.ui
