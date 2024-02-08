#ifndef BINCULARSTEREO_H
#define BINCULARSTEREO_H

#include"ImageProcess/image_processing.h"

class bincular3d : public Image_Processing_Class
{
    Q_OBJECT
public:
    explicit bincular3d(QObject *parent = nullptr);

public:
    QList<QFileInfo> *imgL_filenames,*imgR_filenames;
    Mat img1,img_output1,img2,img_output2,img_output3,imgLmask,imgRmask;
    float scale;
    Mat M1o,M1, D1, M2o,M2, D2;
    Mat R, T, R1, P1, R2, P2, Q;
    Size img_size;
    Mat map11, map12, map21, map22;
    Rect roi1, roi2;

    vector<Mat> Ml;
    vector<Mat> Mr;
    vector<Mat> Dl;
    vector<Mat> Dr;

    vector<Mat> R_l2r;
    vector<Mat> T_l2r;
    vector<Mat> Rl;
    vector<Mat> Pl;
    vector<Mat> Rr;
    vector<Mat> Pr;
    vector<Mat> Q_l2r;
    vector<float> alpharius;
    vector<float> leftShift;
    vector<float> rightShift;

    vector<KeyPoint> keypointsL,keypointsR;
    Mat descriptorsL,descriptorsR;
    vector<DMatch> matchLR;
    vector<Point2f> matchedPtL,matchedPtR;
    Mat FundMat,leftH,rightH;

    std::string disparity_filename;
    std::string point_cloud_filename;

    enum { STEREO_BM=0, STEREO_SGBM=1, STEREO_HH=2, STEREO_VAR=3, STEREO_3WAY=4 };

private:
    void getFileNames(vector<string> &imagelist);

public slots:
    void stereoCalib(Size boardSize, float squareSize,
                     bool displayCorners = false, bool useCalibrated=true, bool showRectified=true);

    void readCalibData();

    void changCalibParm();

    void saveXYZ(const char* filename, const Mat& mat);

    void saveXYZ(string file_name, vector<Mat> &rotations, vector<Mat> &motions,
                 vector<Point3f> &structure, vector<Vec3b> &colors);

    Mat stereoMatch(int alg=1,bool display=false);

    void extractDescriptors(vector<KeyPoint> kpl,vector<KeyPoint> kpr,
                            Mat &dspl,Mat &dspr);

    bool extractLRfeatures(int minNum=8,int siftNum=2000);

    bool matchLRfeatures(int minNum=8);

    vector<DMatch> matchMutifeatures(Mat dspl,Mat dspr);

    void getMatchedPoints();

    void caluFundMat();

    void reconstruct(vector<Point3f> &ptDisparity,vector<Point3f> &structure);

    void monoCalib(Size boardSize, float squareSize,
                     bool displayCorners = false, bool showRectified=true);
    void imagePt2worldPlane(Mat intrinsicMat,Mat distortionCoff,Mat rotationMat,Mat translationMat,
                            Point2d imgPt,Point2d &worldPlanePt,double worldZ=0);
};

#endif // BINCULARSTEREO_H
