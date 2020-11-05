#include "includes/FaceDetection.h"


void FaceDetectionThread::setDebugMode(bool debug) {
    this->debug = debug;
}

void FaceDetectionThread::setEyeDetection(bool eyeDetection) {
    this->eyeDetection = eyeDetection;
}

void FaceDetectionThread::run(){
    canLaunchAgain = false;
    if (frame.empty()){
        qDebug()<< "Incorrect frame";
        canLaunchAgain = true;
        return;
    }
    Mat gray, smallImg;
    std::vector<Rect> faces = std::vector<Rect>();
    this->m_faces.clear();

    // Convert frame in smaller gray frame for better recognition
    cvtColor( frame, gray, COLOR_BGR2GRAY ); // Convert to Gray Scale
    resize( gray, smallImg, cv::Size(), 1, 1, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );

    //Detect faces of different sizes using cascade classifier
    frontalFaceClassifier.detectMultiScale( smallImg, faces, 1.1,
                                            2, 0| CASCADE_SCALE_IMAGE, Size(30, 30) );
    for (auto &face : faces) {    // Remove the smallers rectangles. Usually enough to prevent fake detections.
        if (face.width > 100){
            this->m_faces.push_back(face);
            if (debug) {
                cv::rectangle(frame, face, cv::Scalar(255, 0, 0));
            }
        }
    }
    eye_cascade.detectMultiScale( smallImg, faces, 1.1,
                                  2, 0| CASCADE_SCALE_IMAGE, Size(30, 30) );
    for (auto &eye : faces) {    // Remove the smallers rectangles. Usually enough to prevent fake detections.
        this->m_eyes.push_back(eye);
        if (debug) {
            cv::rectangle(frame, eye, cv::Scalar(255, 0, 0));
        }
    }


    if (debug) {
        setFrame(frame);
    }
    emit detectionIsOver(m_faces);
    canLaunchAgain = true;
}

void FaceDetectionThread::setFrame(const Mat &frame){
    this->frame = frame;
}

std::vector<Rect> &FaceDetectionThread::getFaces()
{
    return this->m_faces;
}

void FaceDetectionThread::setFrontalFaceClassifier(CascadeClassifier classifier){
    if (!this->isRunning() || this->isFinished()) {
        this->frontalFaceClassifier = classifier;
    }
}

FaceDetectionThread::FaceDetectionThread(CascadeClassifier classifier, bool &canLaunchAgain, bool debug)
    :canLaunchAgain(canLaunchAgain)
{
    this->frontalFaceClassifier = classifier;
    this->debug = debug;
}

FaceDetection::FaceDetection(QObject* parent)
    :QObject(parent)
{
    qDebug() << "Initializing OpenCVtoQML with FaceDetection ON";
    qRegisterMetaType<cv::Mat>("Mat&");
    qRegisterMetaType<cv::Mat>("Mat");
    qRegisterMetaType<cv::Rect>("cv::Rect");
    qRegisterMetaType<QList<Rect>>("QList<Rect>");
    qRegisterMetaType<std::vector<cv::Rect>>("std::vector<cv::Rect>");
    if (m_debug) {
        qDebug("Enabling face detection debug");
    }
    faceDetectionThread = new FaceDetectionThread(frontalFaceClassifier, canLaunchDetectionAgain, m_debug);
    connect(faceDetectionThread, &FaceDetectionThread::detectionIsOver, this, &FaceDetection::setFaces);
    connect(faceDetectionThread, &FaceDetectionThread::detectionIsOver, this, &FaceDetection::setEyes);
}

QVariantList FaceDetection::getFoundFaces(){
    QVariantList returnValue;
    for (auto &face: m_faces) {
        returnValue.append(QRect(face.tl().x, face.tl().y, face.width, face.height));
    }
    return returnValue;
}

QVariantList FaceDetection::getFoundEyes(){
    QVariantList returnValue;
    for (auto &eye: m_eyes) {
        returnValue.append(QRect(eye.tl().x, eye.tl().y, eye.width, eye.height));
    }
    return returnValue;
}

bool FaceDetection::faceDetectionFlag() const
{
    return m_faceDetectionFlag;
}

void FaceDetection::runFaceDetection(const Mat &lastFrame) {
    if (canLaunchDetectionAgain) {
        //        emit facesChanged(faceDetectionThread->getFaces());
        faceDetectionThread->setFrame(lastFrame);
        faceDetectionThread->start(QThread::HighPriority);
    }
}

Mat &FaceDetectionThread::getFrame(){
    return frame;
}

std::vector<Rect> &FaceDetection::getFaces()
{
    return m_faces;
}

void FaceDetection::setFaceDetectionFlag(bool faceDetectionFlag)
{
    if (m_faceDetectionFlag == faceDetectionFlag)
        return;

    m_faceDetectionFlag = faceDetectionFlag;
    emit faceDetectionFlagChanged(m_faceDetectionFlag);
}

QString FaceDetection::haarCascade() const
{
    return m_haarCascade;
}

std::vector<cv::Rect> FaceDetection::faces() const
{
    return m_faces;
}

void FaceDetection::setHaarCascade(QString haarCascade)
{
    if (m_haarCascade == haarCascade)
        return;
    m_haarCascade = haarCascade;
    frontalFaceClassifier.load(haarCascade.toStdString());
    if (frontalFaceClassifier.empty()){
        qDebug() << "Couldn't load haarcascade classifier";
    } else {
        qDebug() << "Haarcascade " << haarCascade << "classifier succesfully loaded";
    }

    faceDetectionThread->setFrontalFaceClassifier(frontalFaceClassifier);
    emit haarCascadeChanged(m_haarCascade);
}

