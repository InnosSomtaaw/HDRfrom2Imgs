#include "imghdr.h"

imgHDR::imgHDR(QObject *parent)
    : QObject{parent}
{
    hasInitialized=false;
    idConst=0.0f;
}

imgHDR::~imgHDR()
{
    coffMat.release();
    coffMatC2.release();
    mat4derivate_r1c3.release();
    mat4derivate_r3c1.release();
    coffMat_cu.release();
    coffMatC2_cu.release();
    fltDX_cu.release();
    fltDY_cu.release();
}

void imgHDR::create_coff_mat()
{
    float lambda = float(-4 * M_PI * M_PI);
    int half_row = coffMat.rows/2,half_col = coffMat.cols/2;

    float* r_ptr;
    r_ptr = coffMat.ptr<float>(0);
    *r_ptr = 0;
    //test min u L2
    *r_ptr = -1;
    r_ptr++;
    for (int j = 1; j <= half_col; j++)
    {
        float tsf = (j * j) * lambda;
        //test min u L2
        tsf+=-1.0;
        tsf = 1/tsf;
        *r_ptr++ = tsf;
    }
    float* r_ptr_backward;
    r_ptr_backward = r_ptr;
    r_ptr_backward--;
    for (int j = half_col + 1; j < coffMat.cols; j++)
    {
        *r_ptr++ = *r_ptr_backward--;
    }
    for (int i = 1; i <= half_row; i++)
    {
        r_ptr = coffMat.ptr<float>(i);
        for (int j = 0; j <= half_col; j++)
        {
            float tsf = (i * i + j * j) * lambda;
            //test min u L2
            tsf+=-1.0;
            tsf = 1/tsf;
            *r_ptr++=tsf;
        }
        r_ptr_backward = r_ptr;
        r_ptr_backward--;
        for (int j = half_col + 1; j < coffMat.cols; j++)
        {
            *r_ptr++ = *r_ptr_backward--;
        }
    }
    for (int i = half_row + 1; i < coffMat.rows; i++)
    {
        r_ptr = coffMat.ptr<float>(i);
        r_ptr_backward = coffMat.ptr<float>(2*half_row-i);
        for (int j=0;j<coffMat.cols;j++)
        {
            *r_ptr++ = *r_ptr_backward++;
        }
    }

    float temp_doubles[3]={-1,0,1};
    mat4derivate_r3c1=Mat(Size(1,3),CV_32FC1,(void*)temp_doubles).clone();
    transpose(mat4derivate_r3c1,mat4derivate_r1c3);
}

void imgHDR::create_coff_mat_cu()
{
    cuda::printCudaDeviceInfo(cuda::getDevice());
    int count = cuda::getCudaEnabledDeviceCount();
    if (count > 0)
        cout<<"GPU Device Count: "<<count<<endl;
    else
        return;

    create_coff_mat();
    coffMat_cu.upload(coffMat);
    coffMatC2=Mat(Size(coffMat.cols,coffMat.rows),CV_32FC2);
    insertChannel(coffMat,coffMatC2,0);
    insertChannel(coffMat,coffMatC2,1);
    coffMatC2_cu.upload(coffMatC2);

    fltDX_cu=cuda::createDerivFilter(CV_32FC1,CV_32FC1,1,0,1);
    fltDY_cu=cuda::createDerivFilter(CV_32FC1,CV_32FC1,0,1,1);
}

Mat imgHDR::hdr2GrayImgs(Mat img_src_bright, Mat img_src_dark)
{
    hdrTimer.start();
    if (img_src_bright.size!=img_src_dark.size)
        return Mat();

    Mat img_hdr = Mat(Size(img_src_bright.cols,img_src_bright.rows),CV_32FC1);

    Mat Max_X,Max_Y;
    if(img_src_bright.channels()>1)
    {
        cvtColor(img_src_bright,Max_X,COLOR_BGR2GRAY);
        cvtColor(img_src_dark,Max_Y,COLOR_BGR2GRAY);
    }
    Max_X.convertTo(Max_X,CV_32F);
    Max_Y.convertTo(Max_Y,CV_32F);

    //获取最大梯度图像
    get_max_derivation(Max_X, Max_Y, Max_X, Max_Y);

    //从最大梯度图像重建原图像
    img_hdr = second_method_solving_Possion(Max_X, Max_Y);
    cout<<"Time of 2 Images HDR(ms) on CPU: "<<
        hdrTimer.nsecsElapsed()/1000000<<endl;
    return img_hdr;
}

Mat imgHDR::hdr1GrayImgs(Mat img)
{
    hdrTimer.start();
    if (img.empty())
        return Mat();

    Mat img_hdr = Mat(Size(img.cols,img.rows),CV_32FC1);

    Mat Max_X = Mat(Size(img.cols,img.rows),CV_32F);
    Mat Max_Y = Mat(Size(img.cols,img.rows),CV_32F);
    if(img.channels()>1)
        cvtColor(img,img,COLOR_BGR2GRAY);
//    equalizeHist(img,img);
    img.convertTo(img,CV_32F);

    //增强最大梯度图像
    enhanceDerivation(img,Max_X,Max_Y);

    //从最大梯度图像重建原图像
    img_hdr = second_method_solving_Possion(Max_X, Max_Y);
    cout<<"Time of 1 Image HDR(ms) on CPU: "<<
        hdrTimer.nsecsElapsed()/1000000<<endl;
    return img_hdr;

}

Mat imgHDR::hdr2GrayImgs_cu(Mat img_src_bright, Mat img_src_dark)
{
    hdrTimer.start();
    if (img_src_bright.size!=img_src_dark.size)
        return Mat();

    Mat img_hdr = Mat(Size(img_src_bright.cols,img_src_bright.rows),CV_32FC1);

    Mat Max_X,Max_Y;
    if(img_src_bright.channels()>1)
    {
        cvtColor(img_src_bright,Max_X,COLOR_BGR2GRAY);
        cvtColor(img_src_dark,Max_Y,COLOR_BGR2GRAY);
    }
    Max_X.convertTo(Max_X,CV_32F);
    Max_Y.convertTo(Max_Y,CV_32F);

    cuda::GpuMat maxX_cu,maxY_cu;
    maxX_cu.upload(Max_X);
    maxY_cu.upload(Max_Y);

    //获取最大梯度图像
    maxDerivation_cu(maxX_cu,maxY_cu,maxX_cu,maxY_cu);

    //从最大梯度图像重建原图像
    img_hdr=solvingPossion_cu(maxX_cu,maxY_cu);
    cout<<"Time of 2 Images HDR(ms) on GPU: "<<
        hdrTimer.nsecsElapsed()/1000000<<endl;
    return img_hdr;
}

void imgHDR::get_max_derivation(Mat bright_img, Mat dark_img, Mat &max_derivativeX, Mat &max_derivativeY)
{
    max_derivativeX = bright_img.clone();
    max_derivativeY = dark_img.clone();
    if (bright_img.size != dark_img.size)
        return;

    Mat derivativeY_bright = bright_img.clone();
    Mat derivativeX_bright = bright_img.clone();
    Mat derivativeY_dark = dark_img.clone();
    Mat derivativeX_dark = dark_img.clone();

    filter2D(bright_img,derivativeY_bright,-1,mat4derivate_r3c1);
    filter2D(dark_img, derivativeY_dark, -1,mat4derivate_r3c1);
    filter2D(bright_img, derivativeX_bright, -1,mat4derivate_r1c3);
    filter2D(dark_img, derivativeX_dark, -1,mat4derivate_r1c3);

    Mat derivativeY_bright_abs = derivativeY_bright.clone();
    Mat derivativeX_bright_abs = derivativeX_bright.clone();
    Mat derivativeY_dark_abs = derivativeY_dark.clone();
    Mat derivativeX_dark_abs = derivativeX_dark.clone();

    convertScaleAbs(derivativeX_bright_abs, derivativeX_bright_abs);
    convertScaleAbs(derivativeY_bright_abs, derivativeY_bright_abs);
    convertScaleAbs(derivativeX_dark_abs, derivativeX_dark_abs);
    convertScaleAbs(derivativeY_dark_abs, derivativeY_dark_abs);

    Mat Max_X = derivativeX_bright_abs.clone(), Max_Y = derivativeY_bright_abs.clone();
    compare(derivativeX_bright_abs, derivativeX_dark_abs, Max_X,CMP_GE);
    compare(derivativeY_bright_abs, derivativeY_dark_abs, Max_Y,CMP_GE);

    Max_X.convertTo(Max_X,CV_32F);
    multiply(derivativeX_bright, Max_X, derivativeX_bright, 1.0f / 255.0f);
    threshold(Max_X, Max_X, 100, 255,THRESH_BINARY_INV);
    multiply(derivativeX_dark, Max_X, derivativeX_dark, 1.0f / 255.0f);
    add(derivativeX_bright, derivativeX_dark, Max_X);

    Max_Y.convertTo(Max_Y, CV_32F);
    multiply(derivativeY_bright, Max_Y, derivativeY_bright, 1.0f / 255.0f);
    threshold(Max_Y, Max_Y, 100, 255, THRESH_BINARY_INV);
    multiply(derivativeY_dark, Max_Y, derivativeY_dark, 1.0f / 255.0f);
    add(derivativeY_bright, derivativeY_dark, Max_Y);

    derivativeX_bright_abs.release();
    derivativeX_dark_abs.release();
    derivativeY_bright_abs.release();
    derivativeY_dark_abs.release();

    derivativeX_bright.release();
    derivativeX_dark.release();
    derivativeY_bright.release();
    derivativeY_dark.release();

    max_derivativeX = Max_X; max_derivativeY = Max_Y;
}

void imgHDR::enhanceDerivation(Mat img, Mat &max_derivativeX, Mat &max_derivativeY)
{
    max_derivativeX = img.clone();
    max_derivativeY = img.clone();

    filter2D(img,max_derivativeY,-1,mat4derivate_r3c1);
    filter2D(img,max_derivativeX, -1,mat4derivate_r1c3);
}

void imgHDR::maxDerivation_cu(cuda::GpuMat bright_img, cuda::GpuMat dark_img,
                              cuda::GpuMat &max_derivativeX, cuda::GpuMat &max_derivativeY)
{
    max_derivativeX = bright_img.clone();
    max_derivativeY = dark_img.clone();
    if (bright_img.size() != dark_img.size())
        return;

    cuda::GpuMat derivativeY_bright = bright_img.clone();
    cuda::GpuMat derivativeX_bright = bright_img.clone();
    cuda::GpuMat derivativeY_dark = dark_img.clone();
    cuda::GpuMat derivativeX_dark = dark_img.clone();

    fltDY_cu->apply(bright_img,derivativeY_bright);
    fltDY_cu->apply(dark_img, derivativeY_dark);
    fltDX_cu->apply(bright_img,derivativeX_bright);
    fltDX_cu->apply(dark_img, derivativeX_dark);

    cuda::GpuMat derivativeY_bright_abs = derivativeY_bright.clone();
    cuda::GpuMat derivativeX_bright_abs = derivativeX_bright.clone();
    cuda::GpuMat derivativeY_dark_abs = derivativeY_dark.clone();
    cuda::GpuMat derivativeX_dark_abs = derivativeX_dark.clone();

    cuda::abs(derivativeX_bright_abs, derivativeX_bright_abs);
    cuda::abs(derivativeY_bright_abs, derivativeY_bright_abs);
    cuda::abs(derivativeX_dark_abs, derivativeX_dark_abs);
    cuda::abs(derivativeY_dark_abs, derivativeY_dark_abs);

    cuda::GpuMat Max_X = derivativeX_bright_abs.clone(), Max_Y = derivativeY_bright_abs.clone();
    cuda::compare(derivativeX_bright_abs, derivativeX_dark_abs, Max_X,CMP_GE);
    cuda::compare(derivativeY_bright_abs, derivativeY_dark_abs, Max_Y,CMP_GE);

    Max_X.convertTo(Max_X,CV_32F);
    cuda::multiply(derivativeX_bright, Max_X, derivativeX_bright, 1.0f / 255.0f);
    cuda::threshold(Max_X, Max_X, 100, 255,THRESH_BINARY_INV);
    cuda::multiply(derivativeX_dark, Max_X, derivativeX_dark, 1.0f / 255.0f);
    cuda::add(derivativeX_bright, derivativeX_dark, Max_X);

    Max_Y.convertTo(Max_Y, CV_32F);
    cuda::multiply(derivativeY_bright, Max_Y, derivativeY_bright, 1.0f / 255.0f);
    cuda::threshold(Max_Y, Max_Y, 100, 255, THRESH_BINARY_INV);
    cuda::multiply(derivativeY_dark, Max_Y, derivativeY_dark, 1.0f / 255.0f);
    cuda::add(derivativeY_bright, derivativeY_dark, Max_Y);

    derivativeX_bright_abs.release();
    derivativeX_dark_abs.release();
    derivativeY_bright_abs.release();
    derivativeY_dark_abs.release();

    derivativeX_bright.release();
    derivativeX_dark.release();
    derivativeY_bright.release();
    derivativeY_dark.release();

    max_derivativeX = Max_X; max_derivativeY = Max_Y;
}

Mat imgHDR::second_method_solving_Possion(Mat derivativeX, Mat derivativeY)
{
    if (derivativeX.size != derivativeY.size
        || derivativeX.depth() != derivativeY.depth()
        || derivativeX.channels() != derivativeY.channels())
        return Mat();

    Mat solution_equation = Mat(Size(derivativeX.cols, derivativeX.rows),
                                CV_32FC1);

    Mat div_v = Mat(Size(derivativeX.cols, derivativeX.rows),
                    CV_32FC2);

    Mat temp_dx = Mat(Size(derivativeX.cols, derivativeX.rows),
                      CV_32FC1);
    Mat temp_dy = Mat(Size(derivativeX.cols, derivativeX.rows),
                      CV_32FC1);

    filter2D(derivativeY, temp_dy,-1,mat4derivate_r3c1);
    filter2D(derivativeX, temp_dx,-1,mat4derivate_r1c3);

    add(temp_dx, temp_dy, temp_dx);
    //test min u L2
    temp_dx+=idConst;
    temp_dy = temp_dy.zeros(Size(temp_dy.cols,temp_dy.rows),temp_dy.depth());

    insertChannel(temp_dx,div_v,0);
    insertChannel(temp_dy,div_v,1);

    dft(div_v, div_v);

    extractChannel(div_v, temp_dx, 0);
    extractChannel(div_v, temp_dy, 1);

    multiply(temp_dx, coffMat, temp_dx);
    multiply(temp_dy, coffMat, temp_dy);

    insertChannel(temp_dx, div_v, 0);
    insertChannel(temp_dy, div_v, 1);

    dft(div_v, div_v, DFT_INVERSE);
    extractChannel(div_v, solution_equation, 0);

    temp_dx.release();
    temp_dy.release();
    div_v.release();

    return solution_equation;
}

Mat imgHDR::solvingPossion_cu(cuda::GpuMat derivativeX, cuda::GpuMat derivativeY)
{
    if (derivativeX.size() != derivativeY.size()
        || derivativeX.depth() != derivativeY.depth()
        || derivativeX.channels() != derivativeY.channels())
        return Mat();

    Mat solution_equation;
    cuda::GpuMat div_v = cuda::GpuMat(coffMat_cu.size(),CV_32FC2);
    cuda::GpuMat temp_dx=cuda::GpuMat(coffMat_cu.size(),CV_32FC1);
    cuda::GpuMat temp_dy=cuda::GpuMat(coffMat_cu.size(),CV_32FC1);

    fltDY_cu->apply(derivativeY,temp_dy);
    fltDX_cu->apply(derivativeX,temp_dx);
    cuda::add(temp_dx, temp_dy, temp_dx);
    temp_dy.setTo(Scalar(0));

    cuda::GpuMat temp_dxdy[2]={temp_dx,temp_dy};
    cuda::merge(temp_dxdy,2,div_v);
    cuda::dft(div_v,div_v,coffMat_cu.size());
    cuda::multiply(div_v,coffMatC2_cu,div_v);
    cuda::dft(div_v,div_v,coffMat_cu.size(),DFT_INVERSE);
    cuda::split(div_v,temp_dxdy);
    temp_dxdy[0].download(solution_equation);

    temp_dx.release();
    temp_dy.release();
    div_v.release();

    return solution_equation;
}
