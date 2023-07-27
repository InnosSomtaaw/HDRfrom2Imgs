#ifndef IMGHDR_H
#define IMGHDR_H

#include <QObject>
#include <QElapsedTimer>

#include <opencv2/opencv.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaoptflow.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudafeatures2d.hpp>
#include <opencv2/cudaobjdetect.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudafilters.hpp>

QT_BEGIN_NAMESPACE
using namespace cv;
using namespace std;
QT_END_NAMESPACE

class imgHDR : public QObject
{
    Q_OBJECT
public:
    explicit imgHDR(QObject *parent = nullptr);
    ~imgHDR();

signals:

public:
    bool hasInitialized;
    Mat coffMat,coffMatC2,mat4derivate_r3c1,mat4derivate_r1c3;
    cuda::GpuMat coffMat_cu,coffMatC2_cu;
    Ptr<cuda::Filter> fltDX_cu,fltDY_cu;
    float idConst;

    //计时器
    QElapsedTimer hdrTimer;

    void create_coff_mat();
    void create_coff_mat_cu();
    //双灰度图片HDR合成
    Mat hdr2GrayImgs(Mat img_src_bright, Mat img_src_dark);
    //单灰度图片HDR合成
    Mat hdr1GrayImgs(Mat img);
    //双灰度图片HDR合成GPU版
    Mat hdr2GrayImgs_cu(Mat img_src_bright, Mat img_src_dark);

private:
    //获取最大梯度图像
    void get_max_derivation(Mat bright_img, Mat dark_img,
                            Mat &max_derivativeX, Mat &max_derivativeY);
    //增强梯度图像
    void enhanceDerivation(Mat img,Mat &max_derivativeX, Mat &max_derivativeY);
    //获取最大梯度图像GPU版
    void maxDerivation_cu(cuda::GpuMat bright_img, cuda::GpuMat dark_img,
                          cuda::GpuMat &max_derivativeX, cuda::GpuMat &max_derivativeY);
    //傅里叶变换数值求解泊松方程
    Mat second_method_solving_Possion(Mat derivativeX, Mat derivativeY);
    //傅里叶变换数值求解泊松方程GPU版
    Mat solvingPossion_cu(cuda::GpuMat derivativeX, cuda::GpuMat derivativeY);

};

#endif // IMGHDR_H
