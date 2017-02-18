//
// Created by rigo on 15.02.17.
//

#include "BallFinderTest.hpp"

BallFinderTest::BallFinderTest(const string framePath, const FindMethod &refMethod) {
    pRawMat = new Mat();
    pHSVMat = new Mat();
    threshold = new Mat();

    switch (refMethod) {
        case HSV_MOMENTS:
            tryHSVMoments(framePath);
            break;
        case HSV_HOUGH_CIRCLES:
            tryHSVHoughCircles(framePath);
            break;
        default:
            break;
    }
}

BallFinderTest::BallFinderTest(const string videoPath) {

}

BallFinderTest::~BallFinderTest() {

}

void BallFinderTest::tryHSVMoments(const string &refString) {
    *pRawMat = imread(refString, CV_LOAD_IMAGE_COLOR);
    createTrackbars();
    preprocessFrame();
}

void BallFinderTest::preprocessFrame() {
    cvtColor(*pRawMat, *pHSVMat, COLOR_BGR2HSV);
    namedWindow(PREPROCESS_WINDOW_NAME, CV_WINDOW_AUTOSIZE);
    imshow(PREPROCESS_WINDOW_NAME, *pHSVMat);
}

void BallFinderTest::createTrackbars() {
    namedWindow(TRACKBAR_WINDOW_NAME,0);

    createTrackbar("H_MIN", TRACKBAR_WINDOW_NAME, &H_MIN, H_MAX, onTrackbarChanged, this);
    createTrackbar("H_MAX", TRACKBAR_WINDOW_NAME, &H_MAX, H_MAX, onTrackbarChanged, this);
    createTrackbar("S_MIN", TRACKBAR_WINDOW_NAME, &S_MIN, S_MAX, onTrackbarChanged, this);
    createTrackbar("S_MAX", TRACKBAR_WINDOW_NAME, &S_MAX, S_MAX, onTrackbarChanged, this);
    createTrackbar("V_MIN", TRACKBAR_WINDOW_NAME, &V_MIN, V_MAX, onTrackbarChanged, this);
    createTrackbar("V_MAX", TRACKBAR_WINDOW_NAME, &V_MAX, V_MAX, onTrackbarChanged, this);
}

void BallFinderTest::onTrackbarChanged(int value, void *type) {
    BallFinderTest *pBallFinder = (BallFinderTest*) type;
    inRange(*(pBallFinder->pHSVMat),
            Scalar(pBallFinder->H_MIN, pBallFinder->S_MIN, pBallFinder->V_MIN),
            Scalar(pBallFinder->H_MAX, pBallFinder->S_MAX, pBallFinder->V_MAX),
            *(pBallFinder->threshold));
    pBallFinder->morph();
//    TrackBarType* trackType = reinterpret_cast<TrackBarType*>(type);
}

void BallFinderTest::morph(){
    Mat dilateElement = getStructuringElement(MORPH_ELLIPSE, Size(6, 6));
    dilate(*threshold, *threshold, dilateElement);
    dilate(*threshold, *threshold, dilateElement);

    imshow(PREPROCESS_WINDOW_NAME, *threshold);
    findBall();
}

void BallFinderTest::findBall() {
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

void BallFinderTest::drawObject(int x, int y, Mat *pMat) {
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

void BallFinderTest::tryHSVHoughCircles(const string &refString) {
    *pRawMat = imread(refString, CV_LOAD_IMAGE_COLOR);

    cvtColor(*pRawMat, *pHSVMat, COLOR_BGR2HSV);

    inRange(*pHSVMat,
            Scalar(H_MIN_IDEAL, S_MIN_IDEAL, V_MIN_IDEAL),
            Scalar(H_MAX_IDEAL, S_MAX_IDEAL, V_MAX_IDEAL),
            *threshold);

    Mat dilateElement = getStructuringElement(MORPH_ELLIPSE, Size(6, 6));
    dilate(*threshold, *threshold, dilateElement);
    dilate(*threshold, *threshold, dilateElement);

    detectCircles();
}

void BallFinderTest::detectCircles() {
    CvMemStorage *pStorage = cvCreateMemStorage(0);
    IplImage*  thresholded = new IplImage(*threshold);

    cvSmooth(thresholded, thresholded, CV_GAUSSIAN, 9, 9);

    namedWindow("f", CV_WINDOW_AUTOSIZE);
    imshow("f", *threshold);

    CvSeq *pCirclesSeq = cvHoughCircles(thresholded, pStorage, CV_HOUGH_GRADIENT, 2,
                                        MIN_DISTANCE_BETWEEN_CIRCLES_CENTERS,
                                        100, 50, 20, 80);

    Mat temp;
    pRawMat->copyTo(temp);

    for (int i = 0; i < pCirclesSeq->total; i++) {
        float *p = (float*) cvGetSeqElem(pCirclesSeq, i);
        printf("Ball! x=%f y=%f r=%f\n\r", p[0], p[1], p[2]);

        circle(temp,
               Point((int) p[0], (int) p[1]),
               (int) p[2],
               Scalar(0, 255, 0),
               2);

        char res[50];
        sprintf (res, "x: %.2f, y: %.2f", p[0], p[1]);
        putText(temp,
                res,
                Point((int) p[0], (int) (p[1] + 30)),
                1,
                1,
                Scalar(0, 255, 0),
                2);
    }

    namedWindow(RESULT_WINDOW_NAME, CV_WINDOW_AUTOSIZE);
    imshow(RESULT_WINDOW_NAME, temp);

}
