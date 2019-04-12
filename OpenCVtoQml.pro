#-------------------------------------------------
#
# Project created by QtCreator 2019-03-07T09:50:28
#
#-------------------------------------------------

QT      += quick multimedia multimediawidgets core

TARGET = OpenCVtoQml

TEMPLATE = lib

DEFINES += OPENCVTOQML_LIBRARY

VERSION = 1.1.0

SOURCES += \
        src/FaceDetection.cpp \
        src/OpenCVtoQml.cpp \
        src/SharedImageBuffer.cpp \
        src/CaptureThread.cpp \
        src/MatToQImage.cpp

HEADERS += \
        includes/FaceDetection.h \
        includes/OpenCVtoQml.h \
        includes/Structures.h \
        includes/SharedImageBuffer.h \
        includes/Buffer.h \
        includes/CaptureThread.h \
        includes/MatToQImage.h \
        includes/configopencv.h \
        includes/OpenCVtoQml_global.h


INCLUDEPATH += /usr/local/Cellar/opencv/3.4.5/include
INCLUDEPATH += /usr/local/Cellar/opencv/3.4.5/lib

LIBS += \
    -L/usr/local/Cellar/opencv/3.4.5/lib \
    -lopencv_bgsegm -lopencv_shape -lopencv_video \
    -lopencv_highgui -lopencv_videoio -lopencv_flann \
    -lopencv_xobjdetect -lopencv_imgcodecs -lopencv_objdetect \
    -lopencv_xphoto -lopencv_imgproc -lopencv_core \
    -lopencv_face

