/************************************************************************/
/* qt-opencv-multithreaded:                                             */
/* A multithreaded OpenCV application using the Qt framework.           */
/*                                                                      */
/* CaptureThread.cpp                                                    */
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

#include <QMediaResource>
#include <QMediaPlayer>
#include "includes/CaptureThread.h"
#include "includes/MatToQImage.h"

CaptureThread::CaptureThread(SharedImageBuffer *sharedImageBuffer, int deviceNumber, bool dropFrameIfBufferFull, int width, int height)
    : QThread(), sharedImageBuffer(sharedImageBuffer)
{
    // Save passed parameters
    this->dropFrameIfBufferFull=dropFrameIfBufferFull;
    this->deviceNumber=deviceNumber;
    this->videoSource = "";
    this->width = width;
    this->height = height;
    // Initialize variables(s)
    _delay                      = 0;
    _fps                        = 30;
    _loop                       = false;
    doStop                      =false;
    sampleNumber                =0;
    fpsSum                      =0;
    fps.clear();
    statsData.averageFPS        =0;
    statsData.nFramesProcessed  =0;
}

CaptureThread::CaptureThread(SharedImageBuffer *sharedImageBuffer, QString videoSource, bool dropFrameIfBufferFull, int width, int height)
    : QThread(), sharedImageBuffer(sharedImageBuffer)
{
    // Save passed parameters
    this->dropFrameIfBufferFull     = dropFrameIfBufferFull;
    this->videoSource               = videoSource;
    this->deviceNumber              = -1;
    this->width                     = width;
    this->height                    = height;
    // Initialize variables(s)
    _delay                          = 0;
    _fps                            = 30;
    capVideo                        = nullptr;
    _loop                           = false;
    doStop                          = false;
    sampleNumber                    = 0;
    fpsSum                          = 0;
    fps.clear();
    statsData.averageFPS            = 0;
    statsData.nFramesProcessed      = 0;
}

void CaptureThread::run()
{
    while(1)
    {
        ////////////////////////////////
        // Stop thread if doStop=TRUE //
        ////////////////////////////////
        doStopMutex.lock();
        if(doStop)
        {
            doStop=false;
            doStopMutex.unlock();
            break;
        }
        doStopMutex.unlock();
        /////////////////////////////////
        /////////////////////////////////

        // Save capture time
        captureTime=t.elapsed();
        // Start timer (used to calculate capture rate)
        t.start();

        // Synchronize with other streams (if enabled for this stream)
        if (deviceNumber != -1)
        {
            sharedImageBuffer->sync(deviceNumber);
        }
        else
        {
            sharedImageBuffer->sync("video"/*videoSource*/);
        }

        // Capture frame (if available)
        if (deviceNumber != -1)
        {
            if (!cap.isOpened()) {
                qDebug("can't grab frame");
                continue;
            }
            // Retrieve frame

            cap >> grabbedFrame;
            //            qDebug() << "Grab image from camera...";
            Buffer<Mat> *localDeviceBuffer = sharedImageBuffer->getByDeviceNumber(deviceNumber);
            localDeviceBuffer->add(grabbedFrame, true);
        }
        else
        {
            qDebug() << "grab frame from video...";
            changeVideoMutex.lock();
            if (capVideo)
            { frameVideo = cvQueryFrame( capVideo ); }
            else
            { frameVideo = nullptr; }


            if (frameVideo)
            {
                grabbedFrame = cvarrToMat(frameVideo);

                qDebug() << "add video frame...";

                //sharedImageBuffer->getByVideoSource("video"/*videoSource*/)->add(grabbedFrame, dropFrameIfBufferFull);
            }
            changeVideoMutex.unlock();




            if (!frameVideo)
            {
                if (_loop)
                {
                    //cvSetCaptureProperty(capVideo, CV_CAP_PROP_POS_AVI_RATIO , 0);
                    //changeVideoMutex.lock();
                    qDebug() << "call connectToVideo...";
                    connectToVideo();
                    //changeVideoMutex.unlock();
                    continue;
                }
                qDebug() << "no frames...";
                continue;
            }
        }

//        if (faceDetectionEnabled && _faceDetection != nullptr){
//            _faceDetection->update(grabbedFrame);
//            if (faces.size() > 0) {
//                emit facesUpdated();
//            }
//        }
        frame=MatToQImage(grabbedFrame);
        emit newFrame(frame);
        msleep(_delay);

        // Update statistics
        updateFPS(captureTime);
        statsData.nFramesProcessed++;
        // Inform GUI of updated statistics
        emit updateStatisticsInGUI(statsData);
    }
    qDebug() << "Stopping capture thread...";

}

bool CaptureThread::connectToCamera()
{
    // Open camera
    bool camOpenResult = cap.open(deviceNumber);
    // Set resolution
    if(width != -1)
        cap.set(CV_CAP_PROP_FRAME_WIDTH, width);
    if(height != -1)
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);

    qDebug() << "Camera : " << width << height;

    // Return result
    return camOpenResult;
}


bool CaptureThread::connectToVideo()
{
    videoSource.replace("file://", "");
    qDebug() << "COnnectToVideo: " << videoSource.toLatin1();

    // Open video
    //qDebug() << "COnnectToVideo: " << videoSource.toLatin1();
    disconnectVideo();

    QMutexLocker locker(&changeVideoMutex);
    capVideo = cvCreateFileCapture(videoSource.toLatin1());
    bool videoOpenResult = (capVideo != nullptr);
    /*
    QMediaResource media = QMediaResource(QUrl::fromLocalFile(videoSource));
    qDebug() << "Media: resolution: " << media.resolution().width() << "x" << media.resolution().height();
    qDebug() << "Media: sampleRate: " << media.sampleRate();
    qDebug() << "Media: videoBitRate: " << media.videoBitRate();
    qDebug() << "Media: codec: " << media.videoCodec();

    QMediaPlayer player;
    player.setMedia(QMediaContent(QUrl::fromLocalFile(videoSource)));
    qDebug() << "Player: data: " << player.metaData("VideoFrameRate");
    qDebug() << "Player: data: " << player.metaData("VideoBitRate");
    qDebug() << "Player: data: " << player.metaData("VideoCodec");
    qDebug() << "Player: data: " << player.playbackRate();

*/

    if (!videoOpenResult)
    { stop(); qDebug() << "Error opening video: " << videoSource; }
    else
    {


        /*double fps = (double)cvGetCaptureProperty(capVideo, CV_CAP_PROP_FPS);
        qDebug() << "Video FPS: " << fps;

        double fpc = (double)cvGetCaptureProperty(capVideo, CV_CAP_PROP_FRAME_COUNT);
        qDebug() << "Video FPC: " << fpc;

*/
        //frameVideo = cvQueryFrame(capVideo);
        //if (!frameVideo)
        //{ qDebug() << "Error while grabbing 1st frame..."; }
    }
    /*
    // Set resolution
    if(width != -1)
        cap.set(CV_CAP_PROP_FRAME_WIDTH, width);
    if(height != -1)
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);
*/
    qDebug() << "videoSource : " << width << height;

    // Return result
    return videoOpenResult;
}

bool CaptureThread::disconnectCamera()
{
    // Camera is connected
    if(cap.isOpened())
    {
        // Disconnect camera
        cap.release();
        return true;
    }
    // Camera is NOT connected
    else
        return false;
}


bool CaptureThread::disconnectVideo()
{
    QMutexLocker locker(&changeVideoMutex);
    // Camera is connected
    if(capVideo)
    {
        // Disconnect camera
        cvReleaseCapture(&capVideo);
        capVideo = nullptr;
        return true;
    }
    // Camera is NOT connected
    else
        return false;
}


void CaptureThread::updateFPS(int timeElapsed)
{
    // Add instantaneous FPS value to queue
    if(timeElapsed>0)
    {
        fps.enqueue((int)1000/timeElapsed);
        // Increment sample number
        sampleNumber++;
    }
    // Maximum size of queue is DEFAULT_CAPTURE_FPS_STAT_QUEUE_LENGTH
    if(fps.size()>CAPTURE_FPS_STAT_QUEUE_LENGTH)
        fps.dequeue();
    // Update FPS value every DEFAULT_CAPTURE_FPS_STAT_QUEUE_LENGTH samples
    if((fps.size()==CAPTURE_FPS_STAT_QUEUE_LENGTH)&&(sampleNumber==CAPTURE_FPS_STAT_QUEUE_LENGTH))
    {
        // Empty queue and store sum
        while(!fps.empty())
            fpsSum+=fps.dequeue();
        // Calculate average FPS
        statsData.averageFPS=fpsSum/CAPTURE_FPS_STAT_QUEUE_LENGTH;
        // Reset sum
        fpsSum=0;
        // Reset sample number
        sampleNumber=0;

        double newDelay = (double)(1000/_fps)-(double)(1000/statsData.averageFPS) + 4;

        if (newDelay > 0)
        { _delay = newDelay; }
        else
        { _delay = 0; qDebug() << "Need to drop some frames..."; }

        qDebug() << "Fps: " << statsData.averageFPS << " / " << _fps << " : " << newDelay;
    }
}

void CaptureThread::stop()
{
    QMutexLocker locker(&doStopMutex);
    doStop=true;
}

bool CaptureThread::isCameraConnected()
{
    return cap.isOpened();
}

bool CaptureThread::isVideoConnected()
{
    return (capVideo != nullptr);
}


int CaptureThread::getInputSourceWidth()
{
    return cap.get(CV_CAP_PROP_FRAME_WIDTH);
}

int CaptureThread::getInputSourceHeight()
{
    return cap.get(CV_CAP_PROP_FRAME_HEIGHT);
}

Mat CaptureThread::getLastFrame() const
{
    return grabbedFrame;
}

//std::vector<Rect> &CaptureThread::getFaces()
//{
//    return faces;
//}
