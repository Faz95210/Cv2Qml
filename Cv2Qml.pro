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

DESTDIR = ../build/lib


macx {
    OPEN_CV_PATH = /usr/local/Cellar/opencv
    OPEN_CV_VERSION = 4.4
    OPEN_CV_MINOR = 0_2
    OPEN_CV_FULL_PATH = $${OPEN_CV_PATH}/$${OPEN_CV_VERSION}.$${OPEN_CV_MINOR}

    INCLUDEPATH += $${OPEN_CV_FULL_PATH}/include/opencv4/
    INCLUDEPATH += $${OPEN_CV_FULL_PATH}/lib/

    LIBS += \
        -L$${OPEN_CV_FULL_PATH}/lib \
        -lopencv_bgsegm.$${OPEN_CV_VERSION} -lopencv_shape -lopencv_video \
        -lopencv_highgui -lopencv_videoio -lopencv_flann \
        -lopencv_xobjdetect -lopencv_imgcodecs -lopencv_objdetect \
        -lopencv_xphoto -lopencv_imgproc -lopencv_core \
        -lopencv_face
}
linux {
    OPEN_CV_LIBS_PATH = /usr/local/lib/
    INCLUDEPATH += /usr/local/include/opencv4/
    INCLUDEPATH += $${OPEN_CV_LIBS_PATH}

    LIBS += \
        -L$${OPEN_CV_LIBS_PATH}/lib \
        -lopencv_video \
        -lopencv_highgui -lopencv_videoio -lopencv_flann \
        -lopencv_imgcodecs -lopencv_objdetect \
        -lopencv_imgproc -lopencv_core
}
