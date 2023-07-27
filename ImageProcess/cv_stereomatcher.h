#ifndef STEREOMATCHER_H
#define STEREOMATCHER_H

#include <QString>
#include <string>
#include <opencv2/opencv.hpp>
#include <vector>

class CVStereoMatcher
{
public:
    CVStereoMatcher();
    ~CVStereoMatcher();

    enum { STEREO_BM=0, STEREO_SGBM=1, STEREO_HH=2, STEREO_VAR=3, STEREO_3WAY=4 };

private:
//    std::string intrinsic_filename, extrinsic_filename, disparity_filename, point_cloud_filename;
    int alg;
    int /*SADWindowSize,*/ numberOfDisparities;
//    bool no_display;
    float scale;
    cv::Size img_size;
    int cn;
    cv::Rect roi1, roi2;
    cv::Mat R1, P1, R2, P2, Q;
    cv::Mat map11, map12, map21, map22;

    cv::Ptr<cv::StereoBM> bm;
    cv::Ptr<cv::StereoSGBM> sgbm;

private:
    void initCamParams();

public:
    void calcXYZ(const cv::Mat& imgL, const cv::Mat& imgR, const std::vector<cv::Point>& points, std::vector<cv::Point3f>& xyzs);
};

#endif // STEREOMATCHER_H
