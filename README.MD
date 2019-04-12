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

Before generating the library you need to do some configuration to make the lib suit your need. Find **configopencv.h** in includes. This files contains some elements altering the way the program works, like the number of FPS amongst other things.


### Usage

Once the lib generated you can use it like any other lib. To use it from another QT project do so : 

```makefile
PRE_TARGETDEPS  += /PATH/TO/libOpenCVtoQml.1.0.0.dylib
LIBS            += -L/PATH/TO/DIRECTORY/ROOT/OF/OPENCVTOQML -lOpenCVtoQml.1.0.0
INCLUDEPATH     += /$$PWD/PATH/TO/LIB/INCLUDES/
DEPENDPATH      += /$$PWD/PATH/TO/DIRECTORY/ROOT/OF/OPENCVTOQML/libOpenCVtoQml.1.0.0.dylib
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
PRE_TARGETDEPS  += /$$PWD/lib/cv2qml/libOpenCVtoQml.1.0.0.dylib
LIBS            += -L/$$PWD/lib/cv2qml -lOpenCVtoQml.1.0.0
INCLUDEPATH     += /$$PWD/lib/cv2qml/release/
DEPENDPATH      += /$$PWD/lib/cv2qml/libOpenCVtoQml.1.0.0.dylib
```

Don't forget to add openCV to the new project the same way you did for the library.
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
    cameraWidth: parent.width    // \
                                 //   > Resolution of the camera's recording
    cameraHeight: parent.height  // /
    videoSource: "camera" // If you chose the mode camera here goes the video's path
    loop: true
    fps: 30
    Component.onCompleted: { 
        mediaplayer.start();
    }
    faceDetectionFlag: true //Should the program run face detection
    haarCascade: "/usr/local/Cellar/opencv/3.4.5/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml" //Which haar cascade is gonna be used for face detection
}
```

### Face Detection

_The Face detection integration is still a Work In Progress_

In the qml OCVRenderer object you can set `faceDetectionFlag` to true to have the facedetection running. You need to set the haarcascade the file helping with it `haarCascade`.  Here is the list of different options you may use.
```
.
├── haarcascade_eye.xml
├── haarcascade_eye_tree_eyeglasses.xml
├── haarcascade_frontalcatface.xml
├── haarcascade_frontalcatface_extended.xml
├── haarcascade_frontalface_alt.xml
├── haarcascade_frontalface_alt2.xml
├── haarcascade_frontalface_alt_tree.xml
├── haarcascade_frontalface_default.xml
├── haarcascade_fullbody.xml
├── haarcascade_lefteye_2splits.xml
├── haarcascade_licence_plate_rus_16stages.xml
├── haarcascade_lowerbody.xml
├── haarcascade_profileface.xml
├── haarcascade_righteye_2splits.xml
├── haarcascade_russian_plate_number.xml
├── haarcascade_smile.xml
└── haarcascade_upperbody.xml
```
Then the `OCVRenderer` will have a `std::vector<cv::Rect> faces` property exposed containing the coordinates of the angles around the faces found

## Upcoming improvement

- [ ] Finish Face Detection. 
- [ ] More flexibility. (Right now, the video output is sent as is, with no alteration to it)
