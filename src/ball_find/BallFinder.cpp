//
// Created by rigo on 15.02.17.
//

#include "BallFinder.hpp"

BallFinder::BallFinder(const string framePath, const FindMethod &refMethod) {
    switch (refMethod) {
        case HSV_MOMENTS:
            tryHSVMoments(framePath);
            break;
        default:
            break;
    }
}

BallFinder::BallFinder(const string videoPath) {

}

BallFinder::~BallFinder() {

}

void BallFinder::tryHSVMoments(const string &refString) {
    pRawMat = new Mat();
    pHSVMat = new Mat();
    threshold = new Mat();
    *pRawMat = imread(refString, CV_LOAD_IMAGE_COLOR);
    createTrackbars();
    preprocessFrame();
}

void BallFinder::preprocessFrame() {
    cvtColor(*pRawMat, *pHSVMat, COLOR_BGR2HSV);
    namedWindow(PREPROCESS_WINDOW_NAME, CV_WINDOW_AUTOSIZE);
    imshow(PREPROCESS_WINDOW_NAME, *pHSVMat);
}

void BallFinder::createTrackbars() {
    namedWindow(TRACKBAR_WINDOW_NAME,0);

    createTrackbar("H_MIN", TRACKBAR_WINDOW_NAME, &H_MIN, H_MAX, onTrackbarChanged, this);
    createTrackbar("H_MAX", TRACKBAR_WINDOW_NAME, &H_MAX, H_MAX, onTrackbarChanged, this);
    createTrackbar("S_MIN", TRACKBAR_WINDOW_NAME, &S_MIN, S_MAX, onTrackbarChanged, this);
    createTrackbar("S_MAX", TRACKBAR_WINDOW_NAME, &S_MAX, S_MAX, onTrackbarChanged, this);
    createTrackbar("V_MIN", TRACKBAR_WINDOW_NAME, &V_MIN, V_MAX, onTrackbarChanged, this);
    createTrackbar("V_MAX", TRACKBAR_WINDOW_NAME, &V_MAX, V_MAX, onTrackbarChanged, this);
}

void BallFinder::onTrackbarChanged(int value, void *type) {
    BallFinder *pBallFinder = (BallFinder*) type;
    inRange(*(pBallFinder->pHSVMat),
            Scalar(pBallFinder->H_MIN, pBallFinder->S_MIN, pBallFinder->V_MIN),
            Scalar(pBallFinder->H_MAX, pBallFinder->S_MAX, pBallFinder->V_MAX),
            *(pBallFinder->threshold));
    pBallFinder->morph();
//    TrackBarType* trackType = reinterpret_cast<TrackBarType*>(type);
}

void BallFinder::morph(){
    Mat dilateElement = getStructuringElement(MORPH_ELLIPSE, Size(6, 6));
    dilate(*threshold, *threshold, dilateElement);
    dilate(*threshold, *threshold, dilateElement);

    imshow(PREPROCESS_WINDOW_NAME, *threshold);
    findBall();
}

void BallFinder::findBall() {
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

                if(area > MIN_OBJECT_AREA && area < MAX_OBJECT_AREA && area > prevArea){
                    x = (int) (moment.m10 / area);
                    y = (int) (moment.m01 / area);
                    objectFound = true;
                    prevArea = area;
                } else 
                    objectFound = false;
            }

            if (objectFound) {
                drawObject(x, y, pRawMat);
            }
        }
    }
}

void BallFinder::drawObject(int x, int y, Mat *pMat) {
    Mat temp;
    pMat->copyTo(temp);
    circle(temp, Point(x,y), 20, Scalar(0,255,0), 2);

    Converter *pConverter = Converter::getInstance();

    float x_dst, y_dst;
    pConverter->convertFromPixelsToCantimeters(x, y, x_dst, y_dst);

    char res[50];
    sprintf (res, "x: %.2f, y: %.2f", x_dst, y_dst);
    putText(temp,
            res,
            Point(x, y + 30),
            1,
            1,
            Scalar(0, 255, 0),
            2);

    namedWindow(RESULT_WINDOW_NAME, CV_WINDOW_AUTOSIZE);
    imshow(RESULT_WINDOW_NAME, temp);
}