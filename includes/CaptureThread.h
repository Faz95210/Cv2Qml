/************************************************************************/
/* qt-opencv-multithreaded:                                             */
/* A multithreaded OpenCV application using the Qt framework.           */
/*                                                                      */
/* CaptureThread.h                                                      */
/*                                                                      */
/* Nick D'Ademo <nickdademo@gmail.com>                                  */
/*                                                                      */
/* Copyright (c) 2012-2013 Nick D'Ademo                                 */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files (the "Software"), to deal in the Software without restriction, */
/* including without limitation the rights to use, copy, modify, merge, */
/* publish, distribute, sublicense, and/or sell copies of the Software, */
/* and to permit persons to whom the Software is furnished to do so,    */
/* subject to the following conditions:                                 */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  */
/* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   */
/* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    */
/* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     */
/* SOFTWARE.                                                            */
/*                                                                      */
/************************************************************************/

#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

// Qt
#include <QtCore/QTime>
#include <QtCore/QThread>
#include <QDebug>
#include <QImage>
// OpenCV
#include "opencv/highgui.h"
// Local
#include "SharedImageBuffer.h"
#include "configopencv.h"
#include "Structures.h"
#include "OpenCVtoQml_global.h"

using namespace cv;

class ImageBuffer;

class OPENCVTOQMLSHARED_EXPORT CaptureThread : public QThread
{
    Q_OBJECT

public:
    CaptureThread(SharedImageBuffer *sharedImageBuffer, int deviceNumber, bool dropFrameIfBufferFull, int width, int height);
    CaptureThread(SharedImageBuffer *sharedImageBuffer, QString videoSource, bool dropFrameIfBufferFull, int width, int height);
    void                stop();
    bool                connectToCamera();
    bool                connectToVideo();
    bool                disconnectCamera();
    bool                disconnectVideo();
    bool                isCameraConnected();
    bool                isVideoConnected();
    int                 getInputSourceWidth();
    int                 getInputSourceHeight();
    Mat             getLastFrame() const;
private:
    void                updateFPS(int);
    SharedImageBuffer   *sharedImageBuffer;
    VideoCapture        cap;
    CvCapture           *capVideo;
    Mat                 grabbedFrame;
    IplImage            *frameVideo;
    QTime               t;
    QMutex              doStopMutex;
    QMutex              changeVideoMutex;
    QQueue<int>         fps;
    QImage              frame;
    struct ThreadStatisticsData statsData;
    volatile bool       doStop;
    int                 captureTime;
    int                 sampleNumber;
    int                 fpsSum;
    bool                dropFrameIfBufferFull;
    int                 deviceNumber;
    QString             videoSource;
    int                 width;
    int                 height;

    bool                _loop;
    int                 _fps;
    int                 _delay;

    std::vector<Rect>   faces = std::vector<Rect>();

protected:
    void                run();

public slots:
    void                onLoopChanged(bool loop) { qDebug() << "lopp changed: " << loop; _loop = loop; }
    void                onVideoSourceChanged(QString video) { qDebug() << "videoSourceChanged: " << video; videoSource = video; connectToVideo(); }
    void                onFpsChanged(int fps) { _fps = fps; }

signals:
    void                updateStatisticsInGUI(struct ThreadStatisticsData);
    void                newFrame(const QImage &frame);
    void                facesUpdated();
};

#endif // CAPTURETHREAD_H
