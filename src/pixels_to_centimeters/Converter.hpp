//
// Created by rigo on 02.02.17.
//

#ifndef CAMERAOBJECTLOCATION_CONVERTER_HPP
#define CAMERAOBJECTLOCATION_CONVERTER_HPP

#include <opencv2/opencv.hpp>
#include <cmath>

using namespace cv;
using namespace std;

class Converter {
private:
    static Converter * pInstance;
    Converter();
    vector<cv::Point2f> pts_origin_pixels;
    vector<cv::Point2f> pts_origin_coords;
    Mat *pHomographyMatrix;

    void findHomographyMatrix();

public:
    static Converter * getInstance() {
        if (!pInstance) {
            pInstance = new Converter();
        }
        return pInstance;
    }
    ~Converter();
    void init(vector<cv::Point2f> pixels_origin,
              vector<cv::Point2f> coords_origin);

    void convertFromPixelsToCantimeters(const float & x_pixel, const float & y_pixel,
                                        float & x_coord, float & y_coord);
};

#endif //CAMERAOBJECTLOCATION_CONVERTER_HPP
