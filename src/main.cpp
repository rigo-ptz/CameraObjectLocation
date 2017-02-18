#include <iostream>
#include <opencv2/opencv.hpp>
#include "pixels_to_centimeters/Converter.hpp"
#include "ball_find/BallFinderTest.hpp"

void initConverter();

using namespace cv;

int main() {
    initConverter();

    BallFinderTest *ballFinder = new BallFinderTest("res/edge.png", HSV_HOUGH_CIRCLES);

    waitKey(0);
    return 0;
}

void initConverter() {
    Converter *pConverter = Converter::getInstance();
    vector<Point2f> pixelsOrigin;
    pixelsOrigin.push_back(Point2f(718, 278));
    pixelsOrigin.push_back(Point2f(1195, 278));
    pixelsOrigin.push_back(Point2f(594, 473));
    pixelsOrigin.push_back(Point2f(1323, 470));

    vector<Point2f> coordsOrigin;
    coordsOrigin.push_back(Point2f(0, 0));
    coordsOrigin.push_back(Point2f(0, 900));
    coordsOrigin.push_back(Point2f(900, 0));
    coordsOrigin.push_back(Point2f(900, 900));

    pConverter->init(pixelsOrigin, coordsOrigin);

    float x_dst, y_dst;
    pConverter->convertFromPixelsToCantimeters(718.0, 278.0, x_dst, y_dst);

//    delete pConverter;
    pConverter = 0;
}




