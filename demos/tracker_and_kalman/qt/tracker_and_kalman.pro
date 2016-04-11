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
INCLUDEPATH += /usr/local/include/opencv
# Link to opencv (very weird, why is kalman filter in opencv_video module?)
LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs\
 -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc\
 -lopencv_video

SOURCES += main.cpp\
        mainwindow.cpp \
    extended_label.cpp \
    ../../../src/tracker/catracker.cpp \
    ../../../src/tracker/ccnormaldisttracker.cpp \
    ../../../src/tracker/ccsimpletracker.cpp \
    ../../../src/kalman/cckalmanfilter.cpp \
    ../../../external_src/qcustomplot/qcustomplot/qcustomplot.cpp

HEADERS  += mainwindow.h \
    extended_label.h \
    ../../../src/tracker/catracker.h \
    ../../../src/tracker/ccnormaldisttracker.h \
    ../../../src/tracker/ccsimpletracker.h \
    ../../../src/kalman/cckalmanfilter.h \
    ../../../external_src/qcustomplot/qcustomplot/qcustomplot.h

FORMS    += mainwindow.ui
