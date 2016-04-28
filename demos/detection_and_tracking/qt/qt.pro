#-------------------------------------------------
#
# Project created by QtCreator 2016-04-27T11:02:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

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
    ../../../external_src/qcustomplot/qcustomplot/qcustomplot.cpp \
    ccthreadedobject.cpp \
    ../../../src/kalman/cckalmanfilter.cpp

HEADERS  += mainwindow.h \
    ../../../external_src/qcustomplot/qcustomplot/qcustomplot.h \
    ccthreadedobject.h \
    ../../../src/kalman/cckalmanfilter.h \
    ../../../src/general/general_includes.h

FORMS    += mainwindow.ui
