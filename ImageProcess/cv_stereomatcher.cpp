#include "cv_stereomatcher.h"
using namespace cv;
using namespace std;

CVStereoMatcher::CVStereoMatcher()
{
    initCamParams();
}

CVStereoMatcher::~CVStereoMatcher()
{
    bm.release();
    sgbm.release();
}

void CVStereoMatcher::initCamParams()
{
    std::string intrinsic_filename, extrinsic_filename, disparity_filename, point_cloud_filename;
//    int alg;
    int SADWindowSize/*, numberOfDisparities*/;
    bool no_display;
    intrinsic_filename = "CalibData\\intrinsics.yml";
    extrinsic_filename = "CalibData\\extrinsics.yml";
    disparity_filename = "CalibData\\disparity.jpg";
    point_cloud_filename = "CalibData\\point_clouds.txt";
    alg = STEREO_SGBM;
    SADWindowSize = 11;
    numberOfDisparities = 112;
    no_display = false;
    scale = 1;

    bm = StereoBM::create(16,9);
    sgbm = StereoSGBM::create(0,16,3);
    Mat temp = imread("CalibData\\srcImg.jpg");
    img_size = temp.size();
    cn = temp.channels();

    if( !intrinsic_filename.empty() )
    {
        // reading intrinsic parameters
        FileStorage fs(intrinsic_filename, FileStorage::READ);
        if(!fs.isOpened())
        {
            printf("Failed to open file %s\n", intrinsic_filename.c_str());
            return;
        }

        Mat M1, D1, M2, D2;
        fs["M1"] >> M1;
        fs["D1"] >> D1;
        fs["M2"] >> M2;
        fs["D2"] >> D2;

        M1 *= scale;
        M2 *= scale;

        fs.open(extrinsic_filename, FileStorage::READ);
        if(!fs.isOpened())
        {
            printf("Failed to open file %s\n", extrinsic_filename.c_str());
            return;
        }

        Mat R, T;
        fs["R"] >> R;
        fs["T"] >> T;

        stereoRectify( M1, D1, M2, D2, img_size, R, T, R1, R2, P1, P2, Q, CALIB_ZERO_DISPARITY, -1, img_size, &roi1, &roi2 );
        initUndistortRectifyMap(M1, D1, R1, P1, img_size, CV_16SC2, map11, map12);
        initUndistortRectifyMap(M2, D2, R2, P2, img_size, CV_16SC2, map21, map22);
//        printf("map11 size: %d, %d", map11.cols, map11.rows);
//        printf("map12 size: %d, %d", map12.cols, map12.rows);
//        printf("map21 size: %d, %d", map21.cols, map21.rows);
//        printf("map22 size: %d, %d", map22.cols, map22.rows);
    }
    numberOfDisparities = numberOfDisparities > 0 ? numberOfDisparities : ((img_size.width/8) + 15) & -16;

    bm->setROI1(roi1);
    bm->setROI2(roi2);
    bm->setPreFilterCap(31);
    bm->setBlockSize(SADWindowSize > 0 ? SADWindowSize : 9);
    bm->setMinDisparity(0);
    bm->setNumDisparities(numberOfDisparities);
    bm->setTextureThreshold(10);
    bm->setUniquenessRatio(15);
    bm->setSpeckleWindowSize(100);
    bm->setSpeckleRange(32);
    bm->setDisp12MaxDiff(1);

    sgbm->setPreFilterCap(63);
    int sgbmWinSize = SADWindowSize > 0 ? SADWindowSize : 3;
    sgbm->setBlockSize(sgbmWinSize);

    sgbm->setP1(8*cn*sgbmWinSize*sgbmWinSize);
    sgbm->setP2(32*cn*sgbmWinSize*sgbmWinSize);
    sgbm->setMinDisparity(0);
    sgbm->setNumDisparities(numberOfDisparities);
    sgbm->setUniquenessRatio(10);
    sgbm->setSpeckleWindowSize(100);
    sgbm->setSpeckleRange(32);
    sgbm->setDisp12MaxDiff(1);
    if(alg==STEREO_HH)
        sgbm->setMode(StereoSGBM::MODE_HH);
    else if(alg==STEREO_SGBM)
        sgbm->setMode(StereoSGBM::MODE_SGBM);
    else if(alg==STEREO_3WAY)
        sgbm->setMode(StereoSGBM::MODE_SGBM_3WAY);
}

void CVStereoMatcher::calcXYZ(const cv::Mat &imgL, const cv::Mat &imgR, const vector<cv::Point> &points, vector<cv::Point3f> &xyzs)
{
    Mat imgLr, imgRr;
    remap(imgL, imgLr, map11, map12, INTER_LINEAR);
    remap(imgR, imgRr, map21, map22, INTER_LINEAR);
    Mat disp, disp8;
    //Mat img1p, img2p, dispp;
    //copyMakeBorder(img1, img1p, 0, 0, numberOfDisparities, 0, IPL_BORDER_REPLICATE);
    //copyMakeBorder(img2, img2p, 0, 0, numberOfDisparities, 0, IPL_BORDER_REPLICATE);

    int64 t = getTickCount();
    float disparity_multiplier = 1.0f;
    if( alg == STEREO_BM )
    {
        bm->compute(imgLr, imgRr, disp);
        if (disp.type() == CV_16S)
            disparity_multiplier = 16.0f;
    }
    else if( alg == STEREO_SGBM || alg == STEREO_HH || alg == STEREO_3WAY )
    {
        sgbm->compute(imgLr, imgRr, disp);
        if (disp.type() == CV_16S)
            disparity_multiplier = 16.0f;
    }
    t = getTickCount() - t;
    printf("Time elapsed: %fms\n", t*1000/getTickFrequency());

    //disp = dispp.colRange(numberOfDisparities, img1p.cols);
    if( alg != STEREO_VAR )
        disp.convertTo(disp8, CV_8U, 255/(numberOfDisparities*16.));
    else
        disp.convertTo(disp8, CV_8U);

    /*if( !no_display )
    {
        namedWindow("left", 1);
        imshow("left", imgLr);
        namedWindow("right", 1);
        imshow("right", imgRr);
        namedWindow("disparity", 0);
        imshow("disparity", disp8);
        printf("press any key to continue...");
        fflush(stdout);
        waitKey();
        printf("\n");
    }*/

    Mat xyzMat;
    Mat floatDisp;
    disp.convertTo(floatDisp, CV_32F, 1.0f / disparity_multiplier);
    reprojectImageTo3D(floatDisp, xyzMat, Q, true);
//    int channels = xyzMat.channels();
//    printf("channels: %d", channels);
    xyzs.clear();
    for(int i = 0; i<points.size(); i++)
    {
        Vec3f p3f = xyzMat.at<Vec3f>(points[i]);
        Point3f xyz;
        xyz.x = p3f[0];
        xyz.y = p3f[1];
        xyz.z = p3f[2];
        xyzs.push_back(xyz);
    }
}
