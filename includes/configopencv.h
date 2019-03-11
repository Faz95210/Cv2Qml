#ifndef CONFIGOPENCV
#define CONFIGOPENCV

// FPS statistics queue lengths
#define PROCESSING_FPS_STAT_QUEUE_LENGTH    32
#define CAPTURE_FPS_STAT_QUEUE_LENGTH       32

// Image buffer size
#define DEFAULT_IMAGE_BUFFER_SIZE           1
// Drop frame if image/frame buffer is full
#define DEFAULT_DROP_FRAMES                 true
// Thread priorities
#define DEFAULT_CAP_THREAD_PRIO             QThread::NormalPriority
#define DEFAULT_PROC_THREAD_PRIO            QThread::HighPriority

// IMAGE PROCESSING
// Smooth
#define DEFAULT_SMOOTH_TYPE                 0 // Options: [BLUR=0,GAUSSIAN=1,MEDIAN=2]
#define DEFAULT_SMOOTH_PARAM_1              3
#define DEFAULT_SMOOTH_PARAM_2              3
#define DEFAULT_SMOOTH_PARAM_3              0
#define DEFAULT_SMOOTH_PARAM_4              0
// Dilate
#define DEFAULT_DILATE_ITERATIONS           1
// Erode
#define DEFAULT_ERODE_ITERATIONS            1
// Flip
#define DEFAULT_FLIP_CODE                   0 // Options: [x-axis=0,y-axis=1,both axes=-1]
// Canny
#define DEFAULT_CANNY_THRESHOLD_1           10
#define DEFAULT_CANNY_THRESHOLD_2           00
#define DEFAULT_CANNY_APERTURE_SIZE         3
#define DEFAULT_CANNY_L2GRADIENT            false

#define NB_CLIPS                            5
#define CALIBRATION_MAX_VALUES              100

#define CLIPS_VALIDATION_TIMEOUT            1500

//#define OPENCV_FACE_DETECTION               1
//#define OPENCV_FACE_DETECTION_HAARCASCADE   "/usr/local/Cellar/opencv/3.4.5/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml"

#endif // CONFIGOPENCV

