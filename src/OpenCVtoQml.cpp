#include <QPen>
#include <QPainter>
#include <QDebug>
#include <QPoint>
#include <QRect>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QMultimedia>
#include "includes/OpenCVtoQml.h"
//#include "player.h"

OpenCVtoQml::OpenCVtoQml(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    _fillMode = Stretch;
    _loop     = false;
    _captureThread = nullptr;
    _imageBuffer = nullptr;
    _sharedImageBuffer = nullptr;
    // connect(this, SIGNAL(videoSourceCh   anged(QString)), this, SLOT(onVideoSourceChanged(QString)));
}

void OpenCVtoQml::start()
{
    qDebug() << "Mode: " << _mode << " " << _videoSource << _cameraWidth;

    if (_captureThread != nullptr)
    {
        // Start capturing frames from camera
        _captureThread->start((QThread::Priority)3);
    }
    else if (_mode == "camera")
    {
        qDebug() << "Camera Mode";
        _imageBuffer = new Buffer<Mat>(DEFAULT_IMAGE_BUFFER_SIZE /*cameraConnectDialog->getImageBufferSize()*/);
        _sharedImageBuffer = new SharedImageBuffer();
        _sharedImageBuffer->add(_cameraId, _imageBuffer, false /*ui->actionSynchronizeStreams->isChecked()*/);

        connectToCamera(false /*cameraConnectDialog->getDropFrameCheckBoxState()*/,
                        3 /*cameraConnectDialog->getCaptureThreadPrio()*/,
                        4 /*cameraConnectDialog->getProcessingThreadPrio()*/,
                        //true /*cameraConnectDialog->getEnableFrameProcessingCheckBoxState()*/,
                        _cameraWidth /*cameraConnectDialog->getResolutionWidth()*/,
                        _cameraHeight /*cameraConnectDialog->getResolutionHeight())*/);
    }
    else if (_mode == "video")
    {
        qDebug() << "Video Mode";
        _imageBuffer = new Buffer<Mat>(DEFAULT_IMAGE_BUFFER_SIZE /*cameraConnectDialog->getImageBufferSize()*/);
        _sharedImageBuffer = new SharedImageBuffer();
        _sharedImageBuffer->add("video"/*_videoSource*/, _imageBuffer, false /*ui->actionSynchronizeStreams->isChecked()*/);

        connectToVideo(false, 3, 4);
    }
}


void OpenCVtoQml::stop()
{
    // Stop capture thread
    if ((_captureThread != nullptr) && (_captureThread->isRunning()))
        stopCaptureThread();
}

void OpenCVtoQml::pause()
{
    // Stop capture thread
    if ((_captureThread != nullptr) && (_captureThread->isRunning()))
        stopCaptureThread();
}


void OpenCVtoQml::paint(QPainter *painter)
{
    /*
    QPen pen(_color, 2);
    painter->setPen(pen);
    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->drawPie(boundingRect().adjusted(1, 1, -1, -1), _val * 16, this->height() * 16); */

    //    qDebug() << "Paint: " << this->width() << "x" << this->height() << " | " << _cameraWidth << "x" << _cameraHeight;
    //    qDebug() << "Painter: " << _lastFrame.width() << "x" << _lastFrame.height();
    //    QTransform rotating;
    //    rotating.rotate(90);
    //    _lastFrame = _lastFrame.transformed(rotating); // Works
    switch (_fillMode) {
        case Stretch:
            qDebug("1");
            painter->drawImage(QRect(0, 0, this->width(), this->height()), _lastFrame, QRect(0, 0, _lastFrame.width(), _lastFrame.height()));
            break;
        case PreserveAspectFit:
        {
            qDebug("2");
            double ratioWidth = this->width() / _lastFrame.width();
            double ratioHeight = this->height() / _lastFrame.height();

            if (ratioWidth < ratioHeight)
            {
                int newHeight = _lastFrame.height()*ratioWidth;
                painter->drawImage(QRect(0, (this->height()-newHeight)/2, this->width(), newHeight), _lastFrame, QRect(0, 0, _lastFrame.width(), _lastFrame.height()));
            }
            else
            {
                int newWidth = _lastFrame.width()*ratioHeight;
                painter->drawImage(QRect((this->width()-newWidth)/2, 0, newWidth, this->height()), _lastFrame, QRect(0, 0, _lastFrame.width(), _lastFrame.height()));
            }

            break;
        }
        case PreserveAspectCrop:
        {
            double ratioWidth = this->width() / _lastFrame.width();
            double ratioHeight = this->height() / _lastFrame.height();

            if (ratioWidth < ratioHeight)
            {
                int newWidth = _lastFrame.width()*ratioHeight;
                painter->drawImage(QRect((this->width()-newWidth)/2, 0, newWidth, this->height()), _lastFrame, QRect(0, 0, _lastFrame.width(), _lastFrame.height()));
            }
            else
            {
                int newHeight = _lastFrame.height()*ratioWidth;
                painter->drawImage(QRect(0, (this->height()-newHeight)/2, this->width(), newHeight), _lastFrame, QRect(0, 0, _lastFrame.width(), _lastFrame.height()));
            }
            break;
        }
        default:
            break;
    }

}


void OpenCVtoQml::updateFrame(QImage img)
{
    _lastFrame = img;
    setFaces(_captureThread->getFaces());
    this->update();

}

bool OpenCVtoQml::connectToCamera(bool dropFrameIfBufferFull, int capThreadPrio, int procThreadPrio, /*bool enableFrameProcessing,*/ int width, int height)
{
    // Set frame label text
    if(_sharedImageBuffer->isSyncEnabledForDeviceNumber(_cameraId))
        qDebug() << "Camera connected. Waiting...";
    else
        qDebug() << "Connecting to camera...";

    // Create capture thread
    _captureThread = new CaptureThread(_sharedImageBuffer, _cameraId, dropFrameIfBufferFull, width, height);
    connect(_captureThread, SIGNAL(newFrame(QImage)), this, SLOT(updateFrame(QImage)));
    // Attempt to connect to camera
    if(_captureThread->connectToCamera())
    {
        // Start capturing frames from camera
        _captureThread->start((QThread::Priority)capThreadPrio);

        // Set internal flag and return
        _isCameraConnected=true;
        qDebug() << "Connected to camera";

        return true;
    }
    // Failed to connect to camera
    else{
        qDebug() << "Failed to connect to camera";
        return false;
    }
}



bool OpenCVtoQml::connectToVideo(bool dropFrameIfBufferFull, int capThreadPrio, int procThreadPrio, int width, int height)
{
    // Set frame label text
    if(_sharedImageBuffer->isSyncEnabledForVideoSource("video"/*_videoSource*/))
        qDebug() << "Video connected. Waiting...";
    else
        qDebug() << "Connecting to video...";

    // Create capture thread
    _captureThread = new CaptureThread(_sharedImageBuffer, _videoSource, dropFrameIfBufferFull, width, height);
    connect(this, SIGNAL(loopChanged(bool)), _captureThread, SLOT(onLoopChanged(bool)));
    connect(this, SIGNAL(videoSourceChanged(QString)), _captureThread, SLOT(onVideoSourceChanged(QString)));
    connect(this, SIGNAL(fpsChanged(int)), _captureThread, SLOT(onFpsChanged(int)));
    connect(_captureThread, SIGNAL(newFrame(QImage)), this, SLOT(updateFrame(QImage)));

    qDebug() << "CaptureThread created...";

    emit loopChanged(_loop);
    emit fpsChanged(_fps);

    // Attempt to connect to camera
    if(_captureThread->connectToVideo())
    {
        // Start capturing frames from camera
        _captureThread->start((QThread::Priority)capThreadPrio);

        // Set internal flag and return
        _isVideoConnected=true;
        return true;
    }
    // Failed to connect to camera
    else
        return false;
}

OpenCVtoQml::~OpenCVtoQml()
{
    if(_isCameraConnected)
    {
        // Stop capture thread
        if(_captureThread->isRunning())
            stopCaptureThread();

        // Automatically start frame processing (for other streams)
        if(_sharedImageBuffer->isSyncEnabledForDeviceNumber(_cameraId))
            _sharedImageBuffer->setSyncEnabled(true);

        // Remove from shared buffer
        _sharedImageBuffer->removeByDeviceNumber(_cameraId);
        // Disconnect camera
        if(_captureThread->disconnectCamera())
            qDebug() << "[" << _cameraId << "] Camera successfully disconnected.";
        else
            qDebug() << "[" << _cameraId << "] WARNING: Camera already disconnected.";
    }

    else if(_isVideoConnected)
    {
        // Stop capture thread
        if(_captureThread->isRunning())
            stopCaptureThread();

        // Automatically start frame processing (for other streams)
        if(_sharedImageBuffer->isSyncEnabledForVideoSource("video"/*_videoSource*/))
            _sharedImageBuffer->setSyncEnabled(true);

        // Remove from shared buffer
        _sharedImageBuffer->removeByVideoSource("video"/*_videoSource*/);
        // Disconnect camera
        if(_captureThread->disconnectVideo())
            qDebug() << "[" << _videoSource << "] Video successfully disconnected.";
        else
            qDebug() << "[" << _videoSource << "] WARNING: Video already disconnected.";
    }

    delete (_captureThread);
    delete (_sharedImageBuffer);
    delete (_imageBuffer);
}

void OpenCVtoQml::stopCaptureThread()
{
    qDebug() << "[" << _cameraId << "] About to stop capture thread...";
    _captureThread->stop();
    _sharedImageBuffer->wakeAll(); // This allows the thread to be stopped if it is in a wait-state
    // Take one frame off a FULL queue to allow the capture thread to finish

    if (_isCameraConnected)
    {
        if (_sharedImageBuffer->getByDeviceNumber(_cameraId)->isFull())
            _sharedImageBuffer->getByDeviceNumber(_cameraId)->get();
    }
    else if (_isVideoConnected)
    {
        if (_sharedImageBuffer->getByVideoSource("video"/*_videoSource*/)->isFull())
            _sharedImageBuffer->getByVideoSource("video"/*_videoSource*/)->get();
    }


    _captureThread->wait();
    qDebug() << "[" << _cameraId << "] Capture thread successfully stopped.";
}


void OpenCVtoQml::changeVideoSource(QString videoSource)
{
    stop();

    //_lastFrame = QImage(_lastFrame.width(), _lastFrame.height(), _lastFrame.format());
    videoSourceChanged(videoSource);
    //_captureThread->start((QThread::Priority)capThreadPrio);
    start();
}


#include <QMediaMetaData>
void OpenCVtoQml::onVideoSourceChanged(QString video)
{


    // QString temp = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString(), "Musique (*.mp3 *.wav *.aac *.wav)");
    QMediaPlayer *player = new QMediaPlayer();
    player->setMedia(QUrl::fromLocalFile(video.replace("file://", "")));

    QEventLoop loop;
    connect(player, SIGNAL(metaDataChanged()), &loop, SLOT(quit()));
    loop.exec();

    qDebug() << "Video Metadata:";
    qDebug() << player->availableMetaData();
    qDebug() << player->metaData(QMediaMetaData::Title).toString();
    qDebug() << player->metaData(QMediaMetaData::Orientation).toString();
    qDebug() << player->metaData(QMediaMetaData::Title).toString();
    qDebug() << player->metaData(QMediaMetaData::MediaType).toString();
    qDebug() << player->metaData(QMediaMetaData::Date).toString();
    qDebug() << player->metaData(QMediaMetaData::Duration).toString();
    qDebug() << player->metaData(QMediaMetaData::PixelAspectRatio).toString();
    qDebug() << player->metaData(QMediaMetaData::SampleRate).toString();
    qDebug() << player->metaData(QMediaMetaData::Resolution).toString();
}
