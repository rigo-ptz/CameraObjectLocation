//
// Created by rigo on 15.02.17.
//

#include "BallFinder.hpp"

BallFinder::BallFinder(const string framePath, bool test) {
    pRawMat = new Mat();
    pHSVMat = new Mat();
    threshold = new Mat();
    *pRawMat = imread(framePath, CV_LOAD_IMAGE_COLOR);
    createTrackbars();
    preprocessFrame();
}

BallFinder::BallFinder(const string videoPath) {

}

BallFinder::~BallFinder() {

}

void BallFinder::preprocessFrame() {
    cvtColor(*pRawMat, *pHSVMat, COLOR_BGR2HSV);
    namedWindow(preprocessedImage, CV_WINDOW_AUTOSIZE);
    imshow(preprocessedImage, *pHSVMat);
}

void BallFinder::createTrackbars() {
    //create window for trackbars
    namedWindow(trackbarWindowName,0);
    //create memory to store trackbar name on window
    char TrackbarName[50];
    sprintf( TrackbarName, "H_MIN", H_MIN);
    sprintf( TrackbarName, "H_MAX", H_MAX);
    sprintf( TrackbarName, "S_MIN", S_MIN);
    sprintf( TrackbarName, "S_MAX", S_MAX);
    sprintf( TrackbarName, "V_MIN", V_MIN);
    sprintf( TrackbarName, "V_MAX", V_MAX);
    //create trackbars and insert them into window
    //3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
    //the max value the trackbar can move (eg. H_HIGH),
    //and the function that is called whenever the trackbar is moved(eg. on_trackbar)
    //                                  ---->    ---->     ---->
    createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, onTrackbarChanged, this);
    createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, onTrackbarChanged, this);
    createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, onTrackbarChanged, this);
    createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, onTrackbarChanged, this);
    createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, onTrackbarChanged, this);
    createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, onTrackbarChanged, this);
}

void BallFinder::onTrackbarChanged(int value, void *type) {
    BallFinder *pBallFinder = (BallFinder*) type;
    inRange(*(pBallFinder->pHSVMat),
            Scalar(pBallFinder->H_MIN, pBallFinder->S_MIN, pBallFinder->V_MIN),
            Scalar(pBallFinder->H_MAX, pBallFinder->S_MAX, pBallFinder->V_MAX),
            *(pBallFinder->threshold));
    pBallFinder->morph();
//    TrackBarType* trackType = reinterpret_cast<TrackBarType*>(type);
//    switch (*trackType) {
//        case H_MIN_T:
//
//            break;
//        case H_MAX_T:
//
//            break;
//        case S_MIN_T:
//
//            break;
//        case S_MAX_T:
//
//            break;
//        case V_MIN_T:
//
//            break;
//        case V_MAX_T:
//
//            break;
//        default:
//            break;
//    }
}

void BallFinder::morph(){
    Mat dilateElement = getStructuringElement(MORPH_ELLIPSE, Size(6, 6));
    dilate(*threshold, *threshold, dilateElement);
    dilate(*threshold, *threshold, dilateElement);

    imshow(preprocessedImage, *threshold);
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
        if (hierarchy.size() < 10) {
            for (int i = 0; i >= 0; i = hierarchy[i][0]) {
                Moments moment = moments(contours[i]);
                double area = moment.m00;

                //if the area is less than 20 px by 20px then it is probably just noise
                //if the area is the same as the 3/2 of the image size, probably just a bad filter
                //we only want the object with the largest area so we safe a reference area each
                //iteration and compare it to the area in the next iteration.
                double MIN_OBJECT_AREA = 10 * 10;
                double MAX_OBJECT_AREA = (1920 * 1090) / 1.5;

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
    namedWindow("1", CV_WINDOW_AUTOSIZE);
    imshow("1", temp);

//    putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(0,255,0),2);

}
