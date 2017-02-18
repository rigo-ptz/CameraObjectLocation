//
// Created by rigo on 18.02.17.
//

#ifndef CAMERAOBJECTLOCATION_VIDEOBALLFINDER_HPP
#define CAMERAOBJECTLOCATION_VIDEOBALLFINDER_HPP

#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <string>
#include "../../pixels_to_centimeters/Converter.hpp"


using namespace std;
using namespace cv;


class VideoBallFinder {
private:
    const string MORPHED_WINDOW_NAME = "MORPHED IMAGE";
    const string TRACKBAR_WINDOW_NAME = "Trackbars";
    const string RESULT_WINDOW_NAME = "Result";
    const int MAX_DETECTED_OBJECT_COUNT = 50;
    const double MIN_OBJECT_AREA = 10 * 10;
    const double MAX_OBJECT_AREA = (1920 * 1090) / 1.5;
    const double MIN_DISTANCE_BETWEEN_CIRCLES_CENTERS = 50.0;

    // Initiate min and max HSV filter values.
    // We can change them using trackbars
    int H_MIN = 0;
    int H_MAX = 360;
    int S_MIN = 0;
    int S_MAX = 256;
    int V_MIN = 0;
    int V_MAX = 256;

    // Find that this variants are the most suitable.
    /*int H_MIN_IDEAL = 3;
    int H_MAX_IDEAL = 20;
    int S_MIN_IDEAL = 45;
    int S_MAX_IDEAL = 85;
    int V_MIN_IDEAL = 35;
    int V_MAX_IDEAL = 70;*/

    int H_MIN_IDEAL = 0;
    int H_MAX_IDEAL = 6;
    int S_MIN_IDEAL = 147;
    int S_MAX_IDEAL = 166;
    int V_MIN_IDEAL = 109;
    int V_MAX_IDEAL = 154;

    enum TrackBarType {
        H_MIN_T, H_MAX_T,
        S_MIN_T, S_MAX_T,
        V_MIN_T, V_MAX_T
    };

    Mat *pRawMat;
    Mat *pHSVMat;
    Mat *threshold;

    IplImage* h_plane = 0;
    IplImage* s_plane = 0;
    IplImage* v_plane = 0;

    vector<Point> track;
    void processVideo(const string videoPath);
    void processFrame(Mat &);
    void morph();
    void findBall(Mat &);
    void drawObject(int, int, Mat&);

    void createTrackbars();


public:
    VideoBallFinder(const string videoPath);
    ~VideoBallFinder();
};

#endif //CAMERAOBJECTLOCATION_VIDEOBALLFINDER_HPP
