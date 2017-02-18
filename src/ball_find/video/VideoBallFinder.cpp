//
// Created by rigo on 18.02.17.
//
#include "VideoBallFinder.hpp"

VideoBallFinder::VideoBallFinder(const string videoPath) {
    pRawMat = new Mat();
    pHSVMat = new Mat();
    threshold = new Mat();

    createTrackbars();
    processVideo(videoPath);
}

VideoBallFinder::~VideoBallFinder() {
    delete pRawMat;
    pRawMat = 0;
    delete pHSVMat;
    pHSVMat = 0;
    delete threshold;
    threshold = 0;
}

void VideoBallFinder::processVideo(const string videoPath) {
    VideoCapture capture(videoPath);
    Mat frame;
    int frameCounter = 0;

    for( ; ; ) {
        capture >> frame;
        if(frame.empty())
            break;

        frameCounter += 1;

        if (frameCounter == 1 || frameCounter % 50 == 0) {
            processFrame(frame);
            findBall(frame);
        }

        if (track.size() > 0) {
            drawObject(track.back().x, track.back().y, frame);
            for (int i = 0; i < track.size(); ++i) {
                if (i + 1 <=  track.size() - 1)
                    line(frame, track[i], track[i + 1], Scalar(0,255,0), 2);
            }
        }

        namedWindow(RESULT_WINDOW_NAME, CV_WINDOW_AUTOSIZE);
        imshow(RESULT_WINDOW_NAME, frame);

        waitKey(20);
    }

}

void VideoBallFinder::processFrame(Mat &refFrame) {
    cvtColor(refFrame, *pHSVMat, COLOR_BGR2HSV);

   /* IplImage hsvImage(*pHSVMat);
    if (h_plane == NULL && s_plane == NULL && v_plane == NULL) {
        h_plane = cvCreateImage(cvGetSize(&hsvImage), IPL_DEPTH_8U, 1);
        s_plane = cvCreateImage(cvGetSize(&hsvImage), IPL_DEPTH_8U, 1);
        v_plane = cvCreateImage(cvGetSize(&hsvImage), IPL_DEPTH_8U, 1);
    }
    cvSplit(&hsvImage, h_plane, s_plane, v_plane, 0);

    double min = 0, max = 0;
    cvMinMaxLoc(h_plane, &min, &max);
    printf("[H] %f x %f\n", min, max);

    cvMinMaxLoc(s_plane, &min, &max);
    printf("[S] %f x %f\n", min, max);

    cvMinMaxLoc(v_plane, &min, &max);
    printf("[V] %f x %f\n", min, max);*/

    inRange(*pHSVMat,
            Scalar(H_MIN_IDEAL, S_MIN_IDEAL, V_MIN_IDEAL),
            Scalar(H_MAX_IDEAL, S_MAX_IDEAL, V_MAX_IDEAL),
            *threshold);
    morph();
}

void VideoBallFinder::morph() {
    Mat dilateElement = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    dilate(*threshold, *threshold, dilateElement);
    dilate(*threshold, *threshold, dilateElement);
    dilate(*threshold, *threshold, dilateElement);

    namedWindow(MORPHED_WINDOW_NAME, CV_WINDOW_AUTOSIZE);
    imshow(MORPHED_WINDOW_NAME, *threshold);
}

void VideoBallFinder::findBall(Mat &refFrame) {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(*threshold, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    int x = 0, y = 0;
    double prevArea = 0;
    bool objectFound = false;

    if (hierarchy.size() > 0) {
        if (hierarchy.size() < MAX_DETECTED_OBJECT_COUNT) {
            for (int i = 0; i >= 0; i = hierarchy[i][0]) {
                Moments moment = moments(contours[i]);
                double area = moment.m00;

                if(area > MIN_OBJECT_AREA && area < MIN_OBJECT_AREA * 2 && area > prevArea){
                    x = (int) (moment.m10 / area);
                    y = (int) (moment.m01 / area);
                    objectFound = true;
                    prevArea = area;
                    track.push_back(Point(x, y));
                } else
                    objectFound = false;
            }

//            if (objectFound) {
//                drawObject(x, y, refFrame);
//            }
        }
    }
}

void VideoBallFinder::drawObject(int x, int y, Mat &refFrame) {

    circle(refFrame, Point(x,y), 20, Scalar(0,255,0), 2);

    Converter *pConverter = Converter::getInstance();

    float x_dst, y_dst;
    pConverter->convertFromPixelsToCantimeters(x, y, x_dst, y_dst);

    char res[50];
    sprintf (res, "x: %.2f, y: %.2f", x_dst, y_dst);
    putText(refFrame,
            res,
            Point(x, y + 30),
            1,
            1,
            Scalar(0, 255, 0),
            2);
}

void VideoBallFinder::createTrackbars() {
    namedWindow(TRACKBAR_WINDOW_NAME,0);

    createTrackbar("H_MIN", TRACKBAR_WINDOW_NAME, &H_MIN, H_MAX, NULL, this);
    createTrackbar("H_MAX", TRACKBAR_WINDOW_NAME, &H_MAX, H_MAX, NULL, this);
    createTrackbar("S_MIN", TRACKBAR_WINDOW_NAME, &S_MIN, S_MAX, NULL, this);
    createTrackbar("S_MAX", TRACKBAR_WINDOW_NAME, &S_MAX, S_MAX, NULL, this);
    createTrackbar("V_MIN", TRACKBAR_WINDOW_NAME, &V_MIN, V_MAX, NULL, this);
    createTrackbar("V_MAX", TRACKBAR_WINDOW_NAME, &V_MAX, V_MAX, NULL, this);
}