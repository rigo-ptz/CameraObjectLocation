//
// Created by rigo on 15.02.17.
//

#ifndef CAMERAOBJECTLOCATION_BALLFINDER_HPP
#define CAMERAOBJECTLOCATION_BALLFINDER_HPP

#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <string>
#include "../pixels_to_centimeters/Converter.hpp"

/*
 * Try to do with Moments and With FindCircles
 *
 * 1) Motion Flows
 *
 *
 *  H_MIN = 11;
     H_MAX = 170;
     S_MIN = 0;
     S_MAX = 229;
     V_MIN = 0;
     V_MAX = 256;

     H_MIN = 0;
     H_MAX = 20;
     S_MIN = 36;
     S_MAX = 256;
     V_MIN = 36;
     V_MAX = 144;

     H_MIN = 0;
     H_MAX = 13;
     S_MIN = 95;
     S_MAX = 256;
     V_MIN = 57;
     V_MAX = 174;

     H_MIN = 0;
     H_MAX = 8;
     S_MIN = 145;
     S_MAX = 256;
     V_MIN = 99;
     V_MAX = 155;

     H_MIN = 178;
     H_MAX = 236;
     S_MIN = 123;
     S_MAX = 154;
     V_MIN = 112;
     V_MAX = 115;

     H_MIN = 3;
     H_MAX = 6;
     S_MIN = 160;
     S_MAX = 163;
     V_MIN = 119;
     V_MAX = 145;
 */
using namespace std;
using namespace cv;

enum FindMethod {
    HSV_MOMENTS
};

class BallFinder {
private:
    const string PREPROCESS_WINDOW_NAME = "HSV IMAGE";
    const string TRACKBAR_WINDOW_NAME = "Trackbars";
    const string RESULT_WINDOW_NAME = "Result";
    const int MAX_DETECTED_OBJECT_COUNT = 10;
    const double MIN_OBJECT_AREA = 10 * 10;
    const double MAX_OBJECT_AREA = (1920 * 1090) / 1.5;

    // Initiate min and max HSV filter values.
    // We can change them using trackbars
    int H_MIN = 0;
    int H_MAX = 256;
    int S_MIN = 0;
    int S_MAX = 256;
    int V_MIN = 0;
    int V_MAX = 256;

    // Find that this variants are the most suitable.
    int H_MIN_IDEAL = 3;
    int H_MAX_IDEAL = 6;
    int S_MIN_IDEAL = 160;
    int S_MAX_IDEAL = 163;
    int V_MIN_IDEAL = 119;
    int V_MAX_IDEAL = 145;

    enum TrackBarType {
        H_MIN_T, H_MAX_T,
        S_MIN_T, S_MAX_T,
        V_MIN_T, V_MAX_T
    };

    Mat *pRawMat;
    Mat *pHSVMat;
    Mat *threshold;
    void tryHSVMoments(const string &);
    void preprocessFrame();
    void createTrackbars();
    void morph();
    static void onTrackbarChanged(int , void* );
    void findBall();
    void drawObject(int, int, Mat*);


public:
    BallFinder(const string framePath, const FindMethod &refMethod);
    BallFinder(const string videoPath);
    ~BallFinder();
};

#endif //CAMERAOBJECTLOCATION_BALLFINDER_HPP
