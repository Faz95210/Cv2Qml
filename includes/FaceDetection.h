#ifndef FACEDETECTION_H
#define FACEDETECTION_H

#include <opencv2/objdetect.hpp>
#include <QtCore>
#include <QObject>

#include "configopencv.h"
#include "OpenCVtoQml_global.h"
#include "CaptureThread.h"

using namespace cv;

class FaceDetectionThread : public QThread{
    Q_OBJECT
public:
    void                    run() override;
    FaceDetectionThread(CascadeClassifier, bool&, bool);
    void                    setFrontalFaceClassifier(CascadeClassifier classifier);
    void                    setFrame(const Mat &frame);
    std::vector<cv::Rect>   &getFaces();
    void                    setDebugMode(bool);
    void                    setEyeDetection(bool);
    Mat                     &getFrame();
signals:
    void                    detectionIsOver(std::vector<cv::Rect>);

protected:
    std::vector<cv::Rect>   m_faces;
    std::vector<cv::Rect>   m_eyes;
    Mat                     frame;
private:
    CascadeClassifier       frontalFaceClassifier;
    CascadeClassifier       eye_cascade = CascadeClassifier("/usr/local/Cellar/opencv@3/3.4.5/share/OpenCV/haarcascades/haarcascade_eye.xml");

    bool                    &canLaunchAgain;
    bool                    debug;
    bool                    eyeDetection;
};

class OPENCVTOQMLSHARED_EXPORT FaceDetection : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool faceDetectionFlag READ faceDetectionFlag WRITE setFaceDetectionFlag NOTIFY faceDetectionFlagChanged)
    Q_PROPERTY(QString haarCascade READ haarCascade WRITE setHaarCascade NOTIFY haarCascadeChanged)
    Q_PROPERTY(std::vector<cv::Rect> faces READ faces WRITE setFaces NOTIFY facesChanged)
    Q_PROPERTY(std::vector<cv::Rect> eyes READ eyes WRITE setEyes NOTIFY eyesChanged)
    Q_PROPERTY(bool debugMode READ debugMode WRITE setDebugMode NOTIFY debugModeChanged)
    Q_PROPERTY(bool eyeDetectionFlag READ eyeDetectionFlag WRITE setEyeDetectionFlag NOTIFY eyeDetectionFlagChanged)

public:
    FaceDetection(QObject *parent = nullptr);
    ~FaceDetection(){}

    Q_INVOKABLE QVariantList getFoundFaces();
    Q_INVOKABLE QVariantList getFoundEyes();
    bool                    faceDetectionFlag() const;
    QString                 haarCascade() const;
    std::vector<cv::Rect>   faces() const;

    bool debugMode() const
    {
        return m_debug;
    }

    bool eyeDetectionFlag() const
    {
        return m_eyeDetectionFlag;
    }

    std::vector<cv::Rect> eyes() const
    {
        return m_eyes;
    }

public slots:
    Q_INVOKABLE void        runFaceDetection(const Mat &frame);
    Q_INVOKABLE std::vector<cv::Rect> &getFaces();
    void                    setFaceDetectionFlag(bool faceDetectionFlag);
    void                    setHaarCascade(QString haarCascade);
    void setFaces(std::vector<cv::Rect> faces)
    {
        m_faces = faces;
        Mat frame = faceDetectionThread->getFrame();
        if(m_debug && !frame.empty() && frame.cols > 0 && frame.rows > 0){
            imshow("debug", frame);
        }
        emit facesChanged(m_faces);
    }

    void setDebugMode(bool debug)
    {
        m_debug = debug;
        faceDetectionThread->setDebugMode(m_debug);
        emit debugModeChanged(m_debug);
    }

    void setEyeDetectionFlag(bool eyeDetectionFlag)
    {
        if (m_eyeDetectionFlag == eyeDetectionFlag)
            return;

        m_eyeDetectionFlag = eyeDetectionFlag;
        emit eyeDetectionFlagChanged(m_eyeDetectionFlag);
        this->faceDetectionThread->setEyeDetection(m_eyeDetectionFlag);
    }

    void setEyes(std::vector<cv::Rect> eyes)
    {
        if (m_eyes == eyes)
            return;

        m_eyes = eyes;
        emit eyesChanged(m_eyes);
    }

signals:
    void                    faceDetectionFlagChanged(bool);
    void                    haarCascadeChanged(QString);
    void                    facesChanged(std::vector<cv::Rect>);


    void                    debugModeChanged(bool debug);

    void                    eyeDetectionFlagChanged(bool eyeDetectionFlag);

    void eyesChanged(std::vector<cv::Rect> eyes);

private:
    CascadeClassifier       frontalFaceClassifier;
    bool                    m_faceDetectionFlag;
    QString                 m_haarCascade;
    std::vector<cv::Rect>   m_faces;
    std::vector<cv::Rect>   m_eyes;
    FaceDetectionThread     *faceDetectionThread;
    Mat                     lastFrame;
    bool                    canLaunchDetectionAgain = true;
    bool                    m_debug;
    bool                    m_eyeDetectionFlag;
};

#endif // FACEDETECTION_H
