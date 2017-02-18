//
// Created by rigo on 02.02.17.
//

#include "Converter.hpp"
Converter* Converter::pInstance = 0;

Converter::Converter() {
    pHomographyMatrix = new Mat();
}

Converter::~Converter() {
    delete pHomographyMatrix;
    pHomographyMatrix = 0;
}

void Converter::init(vector<cv::Point2f> pixels_origin, vector<cv::Point2f> coords_origin) {
    pts_origin_coords = coords_origin;
    pts_origin_pixels = pixels_origin;
    findHomographyMatrix();
}

//Converter::Converter(vector<cv::Point2f> pixels_origin, vector<cv::Point2f> coords_origin) :
//        pts_origin_pixels(pixels_origin),
//        pts_origin_coords(coords_origin) {
//    pHomographyMatrix = new Mat();
//    findHomographyMatrix();
//}


void Converter::convertFromPixelsToCantimeters(const float &x_pixel, const float &y_pixel,
                                               float &x_coord, float &y_coord) {
    vector<Point2f> from;
    from.push_back(Point2f(x_pixel, y_pixel));

    vector<Point2f> result;

    perspectiveTransform(from, result, *pHomographyMatrix);
    x_coord = result[0].x;
    y_coord = result[0].y;
//    cout << "x: " << floor(result[0].x) << ", " << "y: " << floor(result[0].y);
}

void Converter::findHomographyMatrix() {
    Mat m = findHomography(pts_origin_pixels, pts_origin_coords);
    *pHomographyMatrix = m;
}