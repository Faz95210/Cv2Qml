#include "includes/FaceDetection.h"

void FaceDetectionThread::run(){
    canLaunchAgain = false;
    if (frame.empty()){
        qDebug()<< "Incorrect frame";
        canLaunchAgain = true;
        return;
    }
    Mat gray, smallImg;
    std::vector<Rect> faces;
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
        }
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

FaceDetectionThread::FaceDetectionThread(CascadeClassifier classifier, bool &canLaunchAgain)
    :canLaunchAgain(canLaunchAgain)
{
    this->frontalFaceClassifier = classifier;
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

    faceDetectionThread = new FaceDetectionThread(frontalFaceClassifier, canLaunchDetectionAgain);
    connect(faceDetectionThread, &FaceDetectionThread::detectionIsOver, this, &FaceDetection::setFaces);
}

QVariantList FaceDetection::getFoundFaces(){
    QVariantList returnValue;
    for (auto &face: m_faces) {
        returnValue.append(QRect(face.tl().x, face.tl().y, face.width, face.height));
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
        qDebug() << "Haarcascade classifier succesfully loaded";
    }

    faceDetectionThread->setFrontalFaceClassifier(frontalFaceClassifier);
    emit haarCascadeChanged(m_haarCascade);
}

