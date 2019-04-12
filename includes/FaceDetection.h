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
    FaceDetectionThread(CascadeClassifier, bool&);
    void                    setFrontalFaceClassifier(CascadeClassifier classifier);
    void                    setFrame(const Mat &frame);
    std::vector<cv::Rect>   &getFaces();
signals:
    void                    detectionIsOver(std::vector<cv::Rect>);
protected:
    std::vector<cv::Rect>   m_faces;
    Mat                     frame;
private:
    CascadeClassifier       frontalFaceClassifier;
    bool                    &canLaunchAgain;
};

class OPENCVTOQMLSHARED_EXPORT FaceDetection : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool faceDetectionFlag READ faceDetectionFlag WRITE setFaceDetectionFlag NOTIFY faceDetectionFlagChanged)
    Q_PROPERTY(QString haarCascade READ haarCascade WRITE setHaarCascade NOTIFY haarCascadeChanged)
    Q_PROPERTY(std::vector<cv::Rect> faces READ faces WRITE setFaces NOTIFY facesChanged)

public:
    FaceDetection(QObject *parent = nullptr);
    ~FaceDetection(){}

    Q_INVOKABLE QVariantList getFoundFaces();
    bool                    faceDetectionFlag() const;
    QString                 haarCascade() const;
    std::vector<cv::Rect>   faces() const;


public slots:
    Q_INVOKABLE void        runFaceDetection(const Mat &frame);
    Q_INVOKABLE std::vector<cv::Rect> &getFaces();
    void                    setFaceDetectionFlag(bool faceDetectionFlag);
    void                    setHaarCascade(QString haarCascade);
    void setFaces(std::vector<cv::Rect> faces)
    {
        m_faces = faces;
        emit facesChanged(m_faces);
    }

signals:
    void                    faceDetectionFlagChanged(bool);
    void                    haarCascadeChanged(QString);
    void                    facesChanged(std::vector<cv::Rect>);

private:
    CascadeClassifier       frontalFaceClassifier;
    bool                    m_faceDetectionFlag;
    QString                 m_haarCascade;
    std::vector<cv::Rect>   m_faces;
    FaceDetectionThread     *faceDetectionThread;
    Mat                     lastFrame;
    bool                    canLaunchDetectionAgain = true;
};

#endif // FACEDETECTION_H
