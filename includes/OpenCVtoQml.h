#ifndef OPENCVTOQML_H
#define OPENCVTOQML_H

#include <QImage>
#include <QObject>
#include <QQuickPaintedItem>

#include "Structures.h"
#include "SharedImageBuffer.h"
#include "CaptureThread.h"
#include "OpenCVtoQml_global.h"
#include "FaceDetection.h"


class OPENCVTOQMLSHARED_EXPORT OpenCVtoQml : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(QString mode READ getMode WRITE setMode)
    Q_PROPERTY(bool faceDetectionFlag READ faceDetectionFlag WRITE setFaceDetectionFlag NOTIFY faceDetectionFlagChanged)
    Q_PROPERTY(QString haarCascade READ haarCascade WRITE setHaarCascade NOTIFY haarCascadeChanged)
    Q_PROPERTY(int cameraId READ getCameraId WRITE setCameraId)
    Q_PROPERTY(int cameraWidth READ getCameraWidth WRITE setCameraWidth)
    Q_PROPERTY(int cameraHeight READ getCameraHeight WRITE setCameraHeight)
    Q_PROPERTY(bool loop READ loop WRITE setLoop NOTIFY loopChanged)
    Q_PROPERTY(QString videoSource READ getVideoSource WRITE setVideoSource NOTIFY videoSourceChanged)
    Q_PROPERTY(int fps READ fps WRITE setFps NOTIFY fpsChanged)

    Q_PROPERTY(std::vector<Rect> faces READ faces WRITE setFaces NOTIFY facesChanged)


    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_ENUMS(FillMode)


public:
    OpenCVtoQml(QQuickItem *parent = nullptr);
    ~OpenCVtoQml();

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void changeVideoSource(QString videoSource);


    enum FillMode { Stretch, PreserveAspectFit, PreserveAspectCrop };

    void setFillMode(FillMode fillMode)
    {
        _fillMode = fillMode;
        emit fillModeChanged(fillMode);
    }

    FillMode fillMode() const
    { return _fillMode; }

    void setFps(int fps)
    {
        _fps = fps;
        emit fpsChanged(fps);
    }

    int fps() const { return _fps; }


    void setLoop(bool loop)
    {
        _loop = loop;
        emit loopChanged(loop);
    }

    bool loop() const { return _loop; }

    QString getMode() { return _mode; }
    void setMode(const QString &mode) { _mode = mode; }

    int getCameraId() { return _cameraId; }
    void setCameraId(const int &cameraId) { _cameraId = cameraId; }

    int getCameraWidth() { return _cameraWidth; }
    void setCameraWidth(const int &cameraWidth) { _cameraWidth = cameraWidth; }
    int getCameraHeight() { return _cameraHeight; }
    void setCameraHeight(const int &cameraHeight) { _cameraHeight = cameraHeight; }

    QString getVideoSource() { return _videoSource; }
    void setVideoSource(const QString &videoSource) { qDebug() << videoSource; _videoSource = videoSource; emit videoSourceChanged(videoSource); }

    void paint(QPainter *painter);

    std::vector<Rect> faces() const
    {
        return m_faces;
    }

    bool faceDetectionFlag() const
    {
        return m_faceDetectionFlag;
    }

    QString haarCascade() const
    {
        return m_haarCascade;
    }

public slots:
    void setFaces(std::vector<Rect> faces)
    {
        if (m_faces == faces)
            return;

        m_faces = faces;
        emit facesChanged(m_faces);
    }

    void setFaceDetectionFlag(bool faceDetectionFlag)
    {
        if (m_faceDetectionFlag == faceDetectionFlag)
            return;

        m_faceDetectionFlag = faceDetectionFlag;
        emit faceDetectionFlagChanged(m_faceDetectionFlag);
    }

    void setHaarCascade(QString haarCascade)
    {
        if (m_haarCascade == haarCascade)
            return;

        m_haarCascade = haarCascade;
        emit haarCascadeChanged(m_haarCascade);
    }

private slots:
    void updateFrame(QImage img);
    void onVideoSourceChanged(QString video);

signals:
    void fillModeChanged(FillMode);
    void loopChanged(bool);
    void videoSourceChanged(QString);
    void fpsChanged(int);

    void facesChanged(std::vector<Rect> faces);

    void faceDetectionFlagChanged(bool faceDetectionFlag);

    void haarCascadeChanged(QString haarCascade);

private:
    bool connectToCamera(bool dropFrameIfBufferFull, int capThreadPrio, int procThreadPrio, /*bool enableFrameProcessing,*/ int width, int height);
    bool connectToVideo(bool dropFrameIfBufferFull, int capThreadPrio, int procThreadPrio, int width = -1, int height = -1);
    void stopCaptureThread();

    FillMode _fillMode;
    bool _loop;
    int _fps;
    QString _mode;

    SharedImageBuffer *_sharedImageBuffer;
    int _cameraId;
    int _cameraWidth;
    int _cameraHeight;
    bool _isCameraConnected;
    bool _isVideoConnected;
    CaptureThread *_captureThread;
    ImageProcessingFlags _imageProcessingFlags;
    Buffer<Mat> *_imageBuffer;

    QString _videoSource;

    QImage _lastFrame;
    std::vector<Rect> m_faces;
    bool m_faceDetectionFlag;
    QString m_haarCascade;
};

#endif // OPENCVTOQML_H
