#ifndef FACEDETECTION_H
#define FACEDETECTION_H

#include <opencv2/opencv.hpp>
#include <QtCore>

#include "configopencv.h"

using namespace cv;

class FaceDetection
{
public:
    FaceDetection();
    void update(const Mat &lastFrame);
    std::vector<Rect> &getFoundFaces();
private:
    CascadeClassifier frontalFaceClassifier;
    std::vector<Rect> faces;
};

#endif // FACEDETECTION_H
