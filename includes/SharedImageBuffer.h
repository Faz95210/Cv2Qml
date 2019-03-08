/************************************************************************/
/* qt-opencv-multithreaded:                                             */
/* A multithreaded OpenCV application using the Qt framework.           */
/*                                                                      */
/* SharedImageBuffer.h                                                  */
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

#ifndef SHAREDIMAGEBUFFER_H
#define SHAREDIMAGEBUFFER_H

// Qt
#include <QHash>
#include <QSet>
#include <QWaitCondition>
#include <QMutex>
// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
// Local
#include "Buffer.h"
#include "OpenCVtoQml_global.h"

using namespace cv;

class OPENCVTOQMLSHARED_EXPORT SharedImageBuffer
{
    public:
        SharedImageBuffer();

        // Camera accessor
        void add(int deviceNumber, Buffer<Mat> *imageBuffer, bool sync=false);
        Buffer<Mat>* getByDeviceNumber(int deviceNumber);
        void removeByDeviceNumber(int deviceNumber);

        // Video accessor
        void add(QString videoSource, Buffer<Mat> *imageBuffer, bool sync=false);
        Buffer<Mat>* getByVideoSource(QString videoSource);
        void removeByVideoSource(QString videoSource);

        void sync(int deviceNumber);
        void sync(QString videoSource);
        void wakeAll();
        void setSyncEnabled(bool enable);
        bool isSyncEnabledForDeviceNumber(int deviceNumber);
        bool isSyncEnabledForVideoSource(QString videoSource);
        bool getSyncEnabled();
        bool containsImageBufferForDeviceNumber(int deviceNumber);
        bool containsImageBufferForVideoSource(QString videoSource);

    private:
        QHash<int, Buffer<Mat>*> imageBufferMap;
        QHash<QString, Buffer<Mat>*> videoBufferMap;
        QSet<int> syncSet;
        QSet<QString> syncSetVideo;
        QWaitCondition wc;
        QMutex mutex;
        int nArrived;
        bool doSync;
};

#endif // SHAREDIMAGEBUFFER_H
