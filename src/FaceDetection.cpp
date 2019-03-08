#include "includes/FaceDetection.h"

FaceDetection::FaceDetection()
{
    frontalFaceClassifier.load(OPENCV_FACE_DETECTION_HAARCASCADE);
}

std::vector<Rect>& FaceDetection::getFoundFaces(){
    return faces;
}

void FaceDetection::update(const Mat &lastFrame) {
    Mat gray, smallImg;
    std::vector<Rect> faces;
    this->faces.clear();
    // Convert frame in smaller gray frame for better recognition
    cvtColor( lastFrame, gray, COLOR_BGR2GRAY ); // Convert to Gray Scale
    resize( gray, smallImg, cv::Size(), 1, 1, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );

    //Detect faces of different sizes using cascade classifier
    frontalFaceClassifier.detectMultiScale( smallImg, faces, 1.1,
                                            2, 0| CASCADE_SCALE_IMAGE, cv::Size(30, 30) );

    for (auto &face:faces) {    // Remove the smallers rectangles. Usually enough to prevent fake detections.
        if (face.width > 100){
            this->faces.push_back(face);
        }
    }
    /// draw Rectangles around faces. Won't work if lastFrame is const.
    //    for(auto &face : faces){
    //            if (face.width > 100) {
    //                cv::rectangle(lastFrame, face, black);
    //            }
    //     }


}
