[![Qt version](https://img.shields.io/badge/Qt%20version-5.12.1-41cd52.svg?style=flat-square)](http://qt.io)
[![OpenCV version](https://img.shields.io/badge/OpenCV%20version-3.4.5-41cd52.svg?style=flat-square)](https://opencv.org)
[![C++ version](https://img.shields.io/badge/C++%20version-C++11-004482.svg?style=flat-square)](http://www.cplusplus.com/)

# OpenCV2Qml

This library allow to use OpenCV's video output on a qml application. _This library has been developed and tested on Mac on Mojave OS._

## Installation

### Dependencies

To use this library start by installing OpenCV 3.4.5 .

#### MacOS 

On MacOS install the lib with homebrew :  

```bash
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

With brew install QT and OpenCV :

```bash
brew install opencv@3
brew install qt
```

Make sure the .pro has the right informations.  The paths should match the path to your opencv.

```makefile
INCLUDEPATH += /usr/local/Cellar/opencv/3.4.5/include
INCLUDEPATH += /usr/local/Cellar/opencv/3.4.5/lib

LIBS += \
    -L/usr/local/Cellar/opencv/3.4.5/lib \
    -lopencv_bgsegm -lopencv_shape -lopencv_video \
    -lopencv_highgui -lopencv_videoio -lopencv_flann \
    -lopencv_xobjdetect -lopencv_imgcodecs -lopencv_objdetect \
    -lopencv_xphoto -lopencv_imgproc -lopencv_core \
    -lopencv_face
```

### Configuration

Before generating the library you need to do some configuration to make the lib suit your need. Find **configopencv.h** in includes. This files contains some elements altering the way the program works, like the number of FPS, if the program should try to detect face amongst other things.


### Usage

Once the lib generated you can use it like any other lib. To use it from another QT project do so : 
```makefile
PRE_TARGETDEPS  += /PATH/TO/libOpenCVtoQml.a
LIBS            += -L/PATH/TO/DIRECTORY/ROOT/OF/OPENCVTOQML -lOpenCVtoQml
INCLUDEPATH     += /$$PWD/PATH/TO/LIB/INCLUDES/
DEPENDPATH      += /$$PWD/PATH/TO/DIRECTORY/ROOT/OF/OPENCVTOQML
```

Example : 

For a repository organized like that : 

```
├── ShaderFun.pro
├── ShaderFun.pro.user
├── ShaderFun.pro.user.4.8-pre1
├── ShadersList.qml
├── includes
│   ├── Config.h
│   ├── FileReader.h
│   └── ShaderManager.h
├── lib
│   └── cv2qml
│       ├── libOpenCVtoQml\ 2.dylib -> libOpenCVtoQml.1.0.0.dylib
│       ├── libOpenCVtoQml.1.0.0.dylib
│       ├── libOpenCVtoQml.1.0.dylib -> libOpenCVtoQml.1.0.0.dylib
│       ├── libOpenCVtoQml.1.dylib -> libOpenCVtoQml.1.0.0.dylib
│       ├── libOpenCVtoQml.a
│       ├── libOpenCVtoQml.dylib -> libOpenCVtoQml.1.0.0.dylib
│       └── release
│           ├── Buffer.h
│           ├── CaptureThread.h
│           ├── FaceDetection.h
│           ├── MatToQImage.h
│           ├── OpenCVtoQml.h
│           ├── OpenCVtoQml_global.h
│           ├── SharedImageBuffer.h
│           ├── Structures.h
│           └── configopencv.h
├── main.cpp
├── qml
│   ├── Effect.qml
│   └── main.qml
├── qml.qrc
├── shaders
│   ├── billboard.fsh
│   ├── blueseeker.fsh
│   ├── displacement.fsh
│   ├── glow.fsh
│   ├── outline.fsh
│   ├── redseeker.fsh
│   ├── split.fsh
│   ├── thermalCam.fsh
│   ├── warhol.fsh
│   └── yellowseeker.fsh
└── src
    ├── FileReader.cpp
    └── ShaderManager.cpp
```

The .pro file would have this configuration :

```makefile
PRE_TARGETDEPS  += /$$PWD/lib/cv2qml/libOpenCVtoQml.a
LIBS            += -L/$$PWD/lib/cv2qml -lOpenCVtoQml -lOpenCVtoQml
INCLUDEPATH     += /$$PWD/lib/cv2qml/release/
DEPENDPATH      += /$$PWD/lib/cv2qml
```

Then simply `#include <OpenCVtoQml.h>` in your Cpp to start using it.

## How to use

### Video Output

What this lib does is defining a new QML element containing OpenCV's output. To use this element start in the main by registering `OpenCVtoQml` :
``` c++
qmlRegisterType<OpenCVtoQml>("com.company.name", 1, 0, "OCVRenderer");
```

Then declare it in your QML : 

``` qml
OCVRenderer {
    id: mediaplayer
    anchors.fill: parent 
    fillMode: OCVRenderer.PreserveAspectCrop
    mode: "camera" // You can choose here between camera and video.
    cameraId: 0 // Id of the camera by default on MacOS the last camera plugged is the 0
    cameraWidth: parent.width    //
                                 // Resolution of the camera's recording
    cameraHeight: parent.height  // 
    videoSource: "camera" // If you chose the mode camera here goes the video's path
    loop: true
    fps: 30
    Component.onCompleted: { 
        mediaplayer.start();
    }
}
```

### Face Detection

_The Face detection integration is still a Work In Progress_

If the **configopencv.h**'s `OPENCV_FACE_DETECTION` value  has been set to `1` Then the `OCVRenderer` will have a `std::vector<cv::Rect> faces` property exposed containing the coordinates of the angles around the faces found

## Upcoming improvement

- [ ] Finish Face Detection. 
- [ ] More flexibility. (Right now, the video output is sent as is, with no alteration to it)
