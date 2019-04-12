#ifndef OPENCVTOQML_H
#define OPENCVTOQML_H

#include <QImage>
#include <QObject>
#include <QQuickPaintedItem>

#include "Structures.h"
#include "SharedImageBuffer.h"
#include "CaptureThread.h"
#include "OpenCVtoQml_global.h"


class OPENCVTOQMLSHARED_EXPORT OpenCVtoQml : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(Mat frame READ frame WRITE setFrame)
    Q_PROPERTY(QString mode READ getMode WRITE setMode)
    Q_PROPERTY(int cameraId READ getCameraId WRITE setCameraId)
    Q_PROPERTY(int cameraWidth READ getCameraWidth WRITE setCameraWidth)
    Q_PROPERTY(int cameraHeight READ getCameraHeight WRITE setCameraHeight)
    Q_PROPERTY(bool loop READ loop WRITE setLoop NOTIFY loopChanged)
    Q_PROPERTY(QString videoSource READ getVideoSource WRITE setVideoSource NOTIFY videoSourceChanged)
    Q_PROPERTY(int fps READ fps WRITE setFps NOTIFY fpsChanged)

    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_ENUMS(FillMode)


public:
    OpenCVtoQml(QQuickItem *parent = nullptr);
    ~OpenCVtoQml();

    Q_INVOKABLE void    start();
    Q_INVOKABLE void    stop();
    Q_INVOKABLE void    pause();
    Q_INVOKABLE void    changeVideoSource(QString videoSource);
//    Q_INVOKABLE std::vector<cv::Rect> getFaces(){
//        qDebug() << m_faces.size() << " Faces found";
//        return m_faces;
//    }

    enum FillMode { Stretch, PreserveAspectFit, PreserveAspectCrop };

    void                setFillMode(FillMode fillMode);

    FillMode            fillMode() const;

    void                setFps(int fps);

    int                 fps() const;

    void                setLoop(bool loop);

    bool                loop() const { return _loop; }

    QString             getMode() { return _mode; }
    void                setMode(const QString &mode) { _mode = mode; }

    int                 getCameraId() { return _cameraId; }
    void                setCameraId(const int &cameraId) { _cameraId = cameraId; }

    int                 getCameraWidth() { return _cameraWidth; }
    void                setCameraWidth(const int &cameraWidth) { _cameraWidth = cameraWidth; }
    int                 getCameraHeight() { return _cameraHeight; }
    void                setCameraHeight(const int &cameraHeight) { _cameraHeight = cameraHeight; }

    QString             getVideoSource() { return _videoSource; }
    void                setVideoSource(const QString &videoSource) { qDebug() << videoSource; _videoSource = videoSource; emit videoSourceChanged(videoSource); }

    void                paint(QPainter *painter);

    Mat             frame() const;

public slots:

    void                setFrame(Mat frame);

private slots:
    void                updateFrame(QImage img);
    void                onVideoSourceChanged(QString video);

signals:
    void                fillModeChanged(FillMode);
    void                loopChanged(bool);
    void                videoSourceChanged(QString);
    void                fpsChanged(int);
    void                haarCascadeChanged(QString haarCascade);

private:
    bool                connectToCamera(bool dropFrameIfBufferFull, int capThreadPrio, int procThreadPrio, /*bool enableFrameProcessing,*/ int width, int height);
    bool                connectToVideo(bool dropFrameIfBufferFull, int capThreadPrio, int procThreadPrio, int width = -1, int height = -1);
    void                stopCaptureThread();

    FillMode            _fillMode;
    bool                _loop;
    int                 _fps;
    QString             _mode;

    SharedImageBuffer   *_sharedImageBuffer;
    int                 _cameraId;
    int                 _cameraWidth;
    int                 _cameraHeight;
    bool                _isCameraConnected;
    bool                _isVideoConnected;
    CaptureThread       *_captureThread;
    ImageProcessingFlags _imageProcessingFlags;
    Buffer<Mat>         *_imageBuffer;

    QString             _videoSource;

    QImage              _lastFrame;
    Mat                 m_frame;
};

#endif // OPENCVTOQML_H
