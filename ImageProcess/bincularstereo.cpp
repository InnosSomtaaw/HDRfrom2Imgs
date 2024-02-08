#include "bincularstereo.h"

bincular3d::bincular3d(QObject *parent)
{
//    readCalibData();
    scale = 1.f;
    img_size = Size(1920,1080);
    disparity_filename = "";
    point_cloud_filename = "";
    alpharius={0.0f,0.0f,0.0f,0.0f,0.4f,0.0f,0.0f,0.0f,0.0f,0.3f};
//    alpharius={0.0f,0.15f,0.35f,0.2f,0.25f,0.0f,0.0f,0.0f,0.0f,0.3f};
//    alpharius={0.0f,0.0f,0.4f,0.15f,0.2f,0.0f,0.0f,0.0f,0.0f,0.0f};
    leftShift = {0.25f,0.25f,0.25f,0.25f,0.35f,0.25f,0.35f,0.35f,0.25f,0.25f};
    rightShift = {0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f};

    img1=Mat();img2=Mat();
    imgLmask = Mat();imgRmask = Mat();
    M1o=Mat::zeros(3,3,CV_64F);M2o=Mat::zeros(3,3,CV_64F);M1=Mat::zeros(3,3,CV_64F);M2=Mat::zeros(3,3,CV_64F);
    D1=Mat::zeros(1,5,CV_64F);D2=Mat::zeros(1,5,CV_64F);R=Mat::zeros(3,3,CV_64F);T=Mat::zeros(3,1,CV_64F);
    R1=Mat::zeros(3,3,CV_64F);R2=Mat::zeros(3,3,CV_64F);P1=Mat::zeros(3,4,CV_64F);P2=Mat::zeros(3,4,CV_64F);
    Q=Mat::zeros(4,4,CV_64F);roi1=Rect();roi2=Rect();
    map11=Mat();map12=Mat();map21=Mat();map22=Mat();
}

void bincular3d::getFileNames(vector<string> &names)
{
    names.clear();
    if(imgL_filenames->count()<2 || imgR_filenames->count()<2 ||
            imgL_filenames->count()!=imgR_filenames->count())
        return;

    for(int i=0;i<imgL_filenames->count();i++)
    {
        if(i%10==0)
        {
        names.push_back(imgL_filenames->at(i).filePath().toLocal8Bit().toStdString());
        names.push_back(imgR_filenames->at(i).filePath().toLocal8Bit().toStdString());
        }
    }
}

void bincular3d::stereoCalib(Size boardSize, float squareSize,
                                 bool displayCorners, bool useCalibrated, bool showRectified)
{
    vector<string> imagelist;
    getFileNames(imagelist);
    if( imagelist.size() % 2 != 0 )
    {
        cout << "Error: the image list contains odd (non-even) number of elements\n";
        return;
    }

    const int maxScale = 1;
    // ARRAY AND VECTOR STORAGE:

    vector<vector<Point2f> > imagePoints[2];
    vector<vector<Point3f> > objectPoints;
    Size imageSize;

    int i, j, k, nimages = (int)imagelist.size()/2;

    imagePoints[0].resize(nimages);
    imagePoints[1].resize(nimages);
    vector<string> goodImageList;

    for( i = j = 0; i < nimages; i++ )
    {
        for( k = 0; k < 2; k++ )
        {
            const string& filename = imagelist[i*2+k];
            Mat img = imread(filename, 0);
            if(img.empty())
                break;
            if( imageSize == Size() )
                imageSize = img.size();
            else if( img.size() != imageSize )
            {
                cout << "The image " << filename << " has the size different from the first image size. Skipping the pair\n";
                break;
            }
            bool found = false;
            vector<Point2f>& corners = imagePoints[k][j];
            for( int scale = 1; scale <= maxScale; scale++ )
            {
                Mat timg;
                if( scale == 1 )
                    timg = img;
                else
                    resize(img, timg, Size(), scale, scale, INTER_LINEAR_EXACT);
//                found = findChessboardCorners(timg, boardSize, corners,
//                    CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);

                found = findCirclesGrid(timg,boardSize,corners,CALIB_CB_SYMMETRIC_GRID);
                if( found )
                {
                    if( scale > 1 )
                    {
                        Mat cornersMat(corners);
                        cornersMat *= 1./scale;
                    }
                    break;
                }
            }
            if( displayCorners )
            {
                cout << filename << endl;
                Mat cimg, cimg1;
                cvtColor(img, cimg, COLOR_GRAY2BGR);
                drawChessboardCorners(cimg, boardSize, corners, found);
                double sf = 640./MAX(img.rows, img.cols);
                resize(cimg, cimg1, Size(), sf, sf, INTER_LINEAR_EXACT);
                imshow("corners", cimg1);
                char c = (char)waitKey(500);
                if( c == 27 || c == 'q' || c == 'Q' ) //Allow ESC to quit
                    exit(-1);
            }
            else
                putchar('.');
            if( !found )
                break;
            cornerSubPix(img, corners, Size(11,11), Size(-1,-1),
                         TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,
                                      30, 0.01));
        }
        if( k == 2 )
        {
            goodImageList.push_back(imagelist[i*2]);
            goodImageList.push_back(imagelist[i*2+1]);
            j++;
        }
    }
    cout << j << " pairs have been successfully detected.\n";
    nimages = j;
    if( nimages < 2 )
    {
        cout << "Error: too little pairs to run the calibration\n";
        return;
    }

    imagePoints[0].resize(nimages);
    imagePoints[1].resize(nimages);
    objectPoints.resize(nimages);

    for( i = 0; i < nimages; i++ )
    {
        for( j = 0; j < boardSize.height; j++ )
            for( k = 0; k < boardSize.width; k++ )
                objectPoints[i].push_back(Point3f(k*squareSize, j*squareSize, 0));
    }

    cout << "Running stereo calibration ...\n";

    Mat cameraMatrix[2], distCoeffs[2];
    cameraMatrix[0] = initCameraMatrix2D(objectPoints,imagePoints[0],imageSize,0);
    cameraMatrix[1] = initCameraMatrix2D(objectPoints,imagePoints[1],imageSize,0);

    //reading intrinsic parameters
    FileStorage fsIni(".\\AI_Initial_Data\\LS_test.yml", FileStorage::READ);
    if(!fsIni.isOpened())
    {
        cout<<"Failed to open intrinsic parameters!\n";
        return;
    }
    fsIni["cameraMatrix"] >> cameraMatrix[0];
    fsIni["distCoeffs"] >> distCoeffs[0];
    fsIni["cameraMatrix"] >> cameraMatrix[1];
    fsIni["distCoeffs"] >> distCoeffs[1];

    Mat R, T, E, F;

    double rms = stereoCalibrate(objectPoints, imagePoints[0], imagePoints[1],
                    cameraMatrix[0], distCoeffs[0],
                    cameraMatrix[1], distCoeffs[1],
                    imageSize, R, T, E, F,
//            CALIB_FIX_ASPECT_RATIO+
//            CALIB_ZERO_TANGENT_DIST+
            CALIB_USE_INTRINSIC_GUESS+CALIB_FIX_FOCAL_LENGTH,
//            CALIB_FIX_K1+CALIB_FIX_K2+CALIB_FIX_K3,
            TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 400, 1e-3) );
    cout << "done with RMS error=" << rms << endl;

// CALIBRATION QUALITY CHECK
// because the output fundamental matrix implicitly
// includes all the output information,
// we can check the quality of calibration using the
// epipolar geometry constraint: m2^t*F*m1=0
    double err = 0;
    int npoints = 0;
    vector<Vec3f> lines[2];
    for( i = 0; i < nimages; i++ )
    {
        int npt = (int)imagePoints[0][i].size();
        Mat imgpt[2];
        for( k = 0; k < 2; k++ )
        {
            imgpt[k] = Mat(imagePoints[k][i]);
            undistortPoints(imgpt[k], imgpt[k], cameraMatrix[k], distCoeffs[k], Mat(), cameraMatrix[k]);
            computeCorrespondEpilines(imgpt[k], k+1, F, lines[k]);
        }
        for( j = 0; j < npt; j++ )
        {
            double errij = fabs(imagePoints[0][i][j].x*lines[1][j][0] +
                                imagePoints[0][i][j].y*lines[1][j][1] + lines[1][j][2]) +
                           fabs(imagePoints[1][i][j].x*lines[0][j][0] +
                                imagePoints[1][i][j].y*lines[0][j][1] + lines[0][j][2]);
            err += errij;
        }
        npoints += npt;
    }
    cout << "average epipolar err = " <<  err/npoints << endl;

    // save intrinsic parameters
    FileStorage fs(".\\AI_Initial_Data\\intrinsics.yml", FileStorage::WRITE);
    if( fs.isOpened() )
    {
        fs << "M1" << cameraMatrix[0] << "D1" << distCoeffs[0] <<
            "M2" << cameraMatrix[1] << "D2" << distCoeffs[1];
        fs.release();
    }
    else
        cout << "Error: can not save the intrinsic parameters\n";

    Mat R1, R2, P1, P2, Q;
    Rect validRoi[2];

    stereoRectify(cameraMatrix[0], distCoeffs[0],
                  cameraMatrix[1], distCoeffs[1],
                  imageSize, R, T, R1, R2, P1, P2, Q,
                  CALIB_ZERO_DISPARITY, 1, imageSize, &validRoi[0], &validRoi[1]);

    fs.open(".\\AI_Initial_Data\\extrinsics.yml", FileStorage::WRITE);
    if( fs.isOpened() )
    {
        fs << "R" << R << "T" << T << "R1" << R1 << "R2" << R2 << "P1" << P1 << "P2" << P2 << "Q" << Q;
        fs.release();
    }
    else
        cout << "Error: can not save the extrinsic parameters\n";

    // OpenCV can handle left-right
    // or up-down camera arrangements
    bool isVerticalStereo = fabs(P2.at<double>(1, 3)) > fabs(P2.at<double>(0, 3));

// COMPUTE AND DISPLAY RECTIFICATION
    if( !showRectified )
        return;

    Mat rmap[2][2];
// IF BY CALIBRATED (BOUGUET'S METHOD)
    if( useCalibrated )
    {
        // we already computed everything
    }
// OR ELSE HARTLEY'S METHOD
    else
 // use intrinsic parameters of each camera, but
 // compute the rectification transformation directly
 // from the fundamental matrix
    {
        vector<Point2f> allimgpt[2];
        for( k = 0; k < 2; k++ )
        {
            for( i = 0; i < nimages; i++ )
                std::copy(imagePoints[k][i].begin(), imagePoints[k][i].end(), back_inserter(allimgpt[k]));
        }
        F = findFundamentalMat(Mat(allimgpt[0]), Mat(allimgpt[1]), FM_8POINT, 0, 0);
        Mat H1, H2;
        stereoRectifyUncalibrated(Mat(allimgpt[0]), Mat(allimgpt[1]), F, imageSize, H1, H2, 3);

        R1 = cameraMatrix[0].inv()*H1*cameraMatrix[0];
        R2 = cameraMatrix[1].inv()*H2*cameraMatrix[1];
        P1 = cameraMatrix[0];
        P2 = cameraMatrix[1];
    }

    //Precompute maps for cv::remap()
    initUndistortRectifyMap(cameraMatrix[0], distCoeffs[0], R1, P1, imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
    initUndistortRectifyMap(cameraMatrix[1], distCoeffs[1], R2, P2, imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);

    Mat canvas;
    double sf;
    int w, h;
    if( !isVerticalStereo )
    {
        sf = 600./MAX(imageSize.width, imageSize.height);
        w = cvRound(imageSize.width*sf);
        h = cvRound(imageSize.height*sf);
        canvas.create(h, w*2, CV_8UC3);
    }
    else
    {
        sf = 300./MAX(imageSize.width, imageSize.height);
        w = cvRound(imageSize.width*sf);
        h = cvRound(imageSize.height*sf);
        canvas.create(h*2, w, CV_8UC3);
    }

    for( i = 0; i < nimages; i++ )
    {
        for( k = 0; k < 2; k++ )
        {
            Mat img = imread(goodImageList[i*2+k], 0), rimg, cimg;
            remap(img, rimg, rmap[k][0], rmap[k][1], INTER_LINEAR);
            cvtColor(rimg, cimg, COLOR_GRAY2BGR);
            Mat canvasPart = !isVerticalStereo ? canvas(Rect(w*k, 0, w, h)) : canvas(Rect(0, h*k, w, h));
            resize(cimg, canvasPart, canvasPart.size(), 0, 0, INTER_AREA);
            if( useCalibrated )
            {
                Rect vroi(cvRound(validRoi[k].x*sf), cvRound(validRoi[k].y*sf),
                          cvRound(validRoi[k].width*sf), cvRound(validRoi[k].height*sf));
                rectangle(canvasPart, vroi, Scalar(0,0,255), 3, 8);
            }
        }

        if( !isVerticalStereo )
            for( j = 0; j < canvas.rows; j += 16 )
                line(canvas, Point(0, j), Point(canvas.cols, j), Scalar(0, 255, 0), 1, 8);
        else
            for( j = 0; j < canvas.cols; j += 16 )
                line(canvas, Point(j, 0), Point(j, canvas.rows), Scalar(0, 255, 0), 1, 8);
        imshow("rectified", canvas);
        char c = (char)waitKey();
        if( c == 27 || c == 'q' || c == 'Q' )
            break;
    }

}

void bincular3d::monoCalib(Size boardSize, float squareSize, bool displayCorners, bool showRectified)
{
    vector<string> imagelist;
    if(imgL_filenames->count()<3)
    {
        cout << "Error: too little pairs to run the calibration\n";
        return;
    }
    for(int i=0;i<imgL_filenames->count();i++)
        imagelist.push_back(imgL_filenames->at(i).filePath().toLocal8Bit().toStdString());

    const double maxScale = 0.25;
    // ARRAY AND VECTOR STORAGE:

    vector<vector<Point2f> > imagePoints;
    vector<vector<Point3f> > objectPoints;
    Size imageSize;

    int i, j, k, nimages = imagelist.size();

    imagePoints.resize(nimages);
    vector<string> goodImageList;

    for( i = j = 0; i < nimages; i++ )
    {
        const string& filename = imagelist[i];
        Mat img = imread(filename, 0);
        if(img.empty())
            continue;
        if( imageSize == Size() )
            imageSize = img.size();
        else if( img.size() != imageSize )
        {
            cout << "The image " << filename << " has the size different from the first image size. Skipping the pair\n";
            continue;
        }
        bool found = false;
        vector<Point2f>& corners = imagePoints[j];
        for( double scale = 1.0; scale >= maxScale; scale=scale/2.0 )
        {
            Mat timg;
            if( scale == 1 )
                timg = img;
            else
                resize(img, timg, Size(), scale, scale, INTER_LINEAR_EXACT);
            //                found = findChessboardCorners(timg, boardSize, corners,
            //                    CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);

            found = findCirclesGrid(timg,boardSize,corners,CALIB_CB_SYMMETRIC_GRID);
            if( found )
            {
                if( scale < 1 )
                {
                    Mat cornersMat(corners);
                    cornersMat *= 1./scale;
                }
                continue;
            }
        }
        if( displayCorners )
        {
            cout << filename << endl;
            Mat cimg, cimg1;
            cvtColor(img, cimg, COLOR_GRAY2BGR);
            drawChessboardCorners(cimg, boardSize, corners, found);
            double sf = 640./MAX(img.rows, img.cols);
            resize(cimg, cimg1, Size(), sf, sf, INTER_LINEAR_EXACT);
            imshow("corners", cimg1);
            char c = (char)waitKey(50);
            if( c == 27 || c == 'q' || c == 'Q' ) //Allow ESC to quit
                exit(-1);
        }
        else
            putchar('.');
        if( !found )
            continue;
        cornerSubPix(img, corners, Size(11,11), Size(-1,-1),
                     TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,
                                  30, 0.01));
        goodImageList.push_back(imagelist[i]);
        j++;
    }
    cout << j << " images have been successfully detected.\n";
    nimages = j;

    imagePoints.resize(nimages);
    objectPoints.resize(nimages);

    for( i = 0; i < nimages; i++ )
    {
        for( j = 0; j < boardSize.height; j++ )
            for( k = 0; k < boardSize.width; k++ )
                objectPoints[i].push_back(Point3f(k*squareSize, j*squareSize, 0));
    }

    cout << "Running mono calibration ...\n";

    Mat cameraMatrix, distCoeffs;
    cameraMatrix = initCameraMatrix2D(objectPoints,imagePoints,imageSize);
    distCoeffs=Mat::zeros(1,14,CV_64F);
//    distCoeffs.at<double>(0,12)=8.0/180.0*CV_PI;

//    //reading intrinsic parameters
//    FileStorage fsIni("./LS_test.yml", FileStorage::READ);
//    if(!fsIni.isOpened())
//    {
//        cout<<"Failed to open intrinsic parameters!\n";
//        return;
//    }
//    fsIni["cameraMatrix"] >> cameraMatrix;
//    fsIni["distCoeffs"] >> distCoeffs;

//    Mat R, T;
    vector<Mat> R,T;
//    cout<<distCoeffs<<endl;
    double rms=calibrateCamera(objectPoints,imagePoints,imageSize,cameraMatrix,distCoeffs,R,T,
                               CALIB_USE_INTRINSIC_GUESS+CALIB_TILTED_MODEL+CALIB_FIX_K3,
                               TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 5000, 1e-6));

    cout << "done with RMS error=" << rms << endl;

    // save intrinsic parameters
    FileStorage fs("./intrinsics.yml", FileStorage::WRITE);
    if( fs.isOpened() )
    {
        fs << "M" << cameraMatrix << "D" << distCoeffs;
        fs.release();
    }
    else
        cout << "Error: can not save the intrinsic parameters\n";

    fs.open("./extrinsics.yml", FileStorage::WRITE);
    if( fs.isOpened() )
    {
        fs << "R" << R << "T" << T;
        fs.release();
    }
    else
        cout << "Error: can not save the extrinsic parameters\n";

    Mat rotationMat;
    vector<vector<Point2d>> objPt_Backproj;
    objPt_Backproj.resize(nimages);
    double maxError=0;
    double rmsError=0;
    for(int i=0;i<nimages;i++)
    {
        Rodrigues(R[i],rotationMat);
        for(int j=0;j<imagePoints[i].size();j++)
        {
          Point2d pt2d=Point2d(double(imagePoints[i][j].x),
                             double(imagePoints[i][j].y));
          imagePt2worldPlane(cameraMatrix,distCoeffs,rotationMat,T[i],
                             pt2d,pt2d);
          objPt_Backproj[i].push_back(pt2d);
          pt2d.x-=objectPoints[i][j].x;
          pt2d.y-=objectPoints[i][j].y;
          double cntError=pt2d.ddot(pt2d);
          rmsError+=cntError;
          if(cntError>maxError)
              maxError=cntError;
        }
    }
    rmsError/=double(nimages*boardSize.width*boardSize.height);
    rmsError=sqrt(rmsError);
    cout<<"MAX error in mm: "<<sqrt(maxError)*1000.0<<endl;
    cout<<"RMS error in mm: "<<rmsError*1000.0<<endl;
    Point2d pt2d=Point2d(double(imagePoints[0][0].x-imagePoints[0][1].x),
            double(imagePoints[0][0].y-imagePoints[0][1].y));
    double va=pt2d.ddot(pt2d);
    va=(squareSize/sqrt(va))*double((imageSize.width+imageSize.height)/2);
    cout<<"Ratio error in %: "<<(rmsError/va)*100.0<<endl;

//    double rmsDist=0;
//    for(int i=0;i<nimages;i++)
//    {
//        for(int j=0;j<objPt_Backproj[i].size()-1;j++)
//            if(j%boardSize.width<boardSize.width-1)
//            {
//                Point2d pt2d=Point2d(objPt_Backproj[i][j].x-objPt_Backproj[i][j+1].x,
//                        objPt_Backproj[i][j].y-objPt_Backproj[i][j+1].y);
//                double cntDist=pt2d.ddot(pt2d);
//                cntDist=sqrt(cntDist);
////                cout<<cntDist*1000.0<<endl;
//                cntDist-=squareSize;
//                cntDist=pow(cntDist,2);
//                rmsDist+=cntDist;
//            }
//    }
//    rmsDist/=double(nimages*(boardSize.width-1)*boardSize.height);
//    rmsDist=sqrt(rmsDist);
//    cout<<"RMS Dist in mm: "<<rmsDist*1000.0<<endl;

//    undistortImagePoints(imgpt,imgpt,cameraMatrix,distCoeffs);

// COMPUTE AND DISPLAY RECTIFICATION
    if( !showRectified )
        return;

    Mat rmap[2];
    //Precompute maps for cv::remap()
    initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(), cameraMatrix, imageSize, CV_16SC2, rmap[0], rmap[1]);

    for( i = 0; i < nimages; i++ )
    {
        Mat img = imread(goodImageList[i], 0), rimg, cimg;
        remap(img, rimg, rmap[0], rmap[1], INTER_LINEAR);
        cvtColor(rimg, cimg, COLOR_GRAY2BGR);

        double sf = 640./MAX(img.rows, img.cols);
        resize(cimg, cimg, Size(), sf, sf, INTER_LINEAR_EXACT);
        imshow("rectified", cimg);
        char c = (char)waitKey();
        if( c == 27 || c == 'q' || c == 'Q' ) //Allow ESC to quit
            break;
    }

}

void bincular3d::imagePt2worldPlane(Mat intrinsicMat, Mat distortionCoff, Mat rotationMat, Mat translationMat,
                                    Point2d imgPt, Point2d &worldPlanePt, double worldZ)
{
    Mat ptMat = Mat(1,1,CV_64FC2);
    ptMat.at<Vec2d>(0,0)[0]=imgPt.x;
    ptMat.at<Vec2d>(0,0)[1]=imgPt.y;
    undistortPoints(ptMat,ptMat,intrinsicMat,distortionCoff);

    Mat A=Mat::eye(3,3,CV_64F);
    Mat B=Mat::zeros(3,1,CV_64F);

    rotationMat(Range::all(),Range(0,2)).copyTo(
                A(Range::all(),Range(0,2)));
    A.at<double>(0,2)=-1.0*ptMat.at<Vec2d>(0,0)[0];
    A.at<double>(1,2)=-1.0*ptMat.at<Vec2d>(0,0)[1];
    A.at<double>(2,2)=-1.0;

    rotationMat.col(2).copyTo(B);
    B*=worldZ;
    B+=translationMat;
    B*=-1.0;

    Mat X=Mat(3,1,CV_64F);
    solve(A,B,X);
    worldPlanePt.x=X.at<double>(0,0);
    worldPlanePt.y=X.at<double>(1,0);
}

void bincular3d::readCalibData()
{
    //reading intrinsic parameters
    FileStorage fs(".\\AI_Initial_Data\\intrinsics.yml", FileStorage::READ);
    if(!fs.isOpened())
    {
        cout<<"Failed to open intrinsic parameters!\n";
        return;
    }

    fs["M1"] >> M1o;
    fs["D1"] >> D1;
    fs["M2"] >> M2o;
    fs["D2"] >> D2;

    // reading extrinsics parameters
    fs.open(".\\AI_Initial_Data\\extrinsics.yml", FileStorage::READ);
    if(!fs.isOpened())
    {
        cout<<"Failed to open extrinsic parameters!\n";
        return;
    }

    fs["R"] >> R;
    fs["T"] >> T;
    fs["R1"] >> R1;
    fs["R2"] >> R2;
    fs["P1"] >> P1;
    fs["P2"] >> P2;
    fs["Q"] >> Q;
}

void bincular3d::changCalibParm()
{
    M1 = M1o*double(scale);
    M2 = M2o*double(scale);

    if(!img_size.empty())
    {
        stereoRectify(M1, D1, M2, D2, img_size, R, T, R1, R2, P1, P2, Q, CALIB_ZERO_DISPARITY, -1, img_size, &roi1, &roi2 );
        initUndistortRectifyMap(M1, D1, R1, P1, img_size, CV_16SC2, map11, map12);
        initUndistortRectifyMap(M2, D2, R2, P2, img_size, CV_16SC2, map21, map22);
    }
}

void bincular3d::saveXYZ(const char *filename, const Mat &mat)
{
    const double max_z = 1.0e4;
    FILE* fp = fopen(filename, "wt");
    for(int y = 0; y < mat.rows; y++)
    {
        for(int x = 0; x < mat.cols; x++)
        {
            Vec3f point = mat.at<Vec3f>(y, x);
            if(fabs(point[2] - max_z) < FLT_EPSILON || fabs(point[2]) > max_z) continue;
            fprintf(fp, "%f %f %f\n", point[0], point[1], point[2]);
        }
    }
    fclose(fp);
}

void bincular3d::saveXYZ(string file_name,vector<Mat> &rotations, vector<Mat> &motions,
                         vector<Point3f> &structure, vector<Vec3b> &colors)
{
    int n = (int)rotations.size();

    FileStorage fs(file_name, FileStorage::WRITE);
    fs << "Camera Count" << n;
    fs << "Point Count" << (int)structure.size();

    fs << "Rotations" << "[";
    for (size_t i = 0; i < n; ++i)
    {
        fs << rotations[i];
    }
    fs << "]";

    fs << "Motions" << "[";
    for (size_t i = 0; i < n; ++i)
    {
        fs << motions[i];
    }
    fs << "]";

    fs << "Points" << "[";
    for (size_t i = 0; i < structure.size(); ++i)
    {
        fs << structure[i];
    }
    fs << "]";

    fs << "Colors" << "[";
    for (size_t i = 0; i < colors.size(); ++i)
    {
        fs << colors[i];
    }
    fs << "]";

    fs.release();
}

Mat bincular3d::stereoMatch(int alg,bool display)
{
    Mat disp, disp8;
    disp=Mat();disp8=Mat();
    if (img1.empty() || img2.empty())
        return disp;

//    std::string disparity_filename = "disparity.bmp";
//    std::string point_cloud_filename = "3dstruct.yml";

//    disparity_filename = "";
//    point_cloud_filename = "3dstruct.yml";



    Ptr<StereoSGBM> sgbm = StereoSGBM::create();
//    int color_mode = alg == STEREO_BM ? 0 : -1;

//    if(!map11.empty() && !map12.empty() &&
//            !map21.empty() && !map22.empty())
//    {
//        remap(img1, imgLmask, map11, map12, INTER_LINEAR);
//        remap(img2, imgRmask, map21, map22, INTER_LINEAR);
//        imgLmask.copyTo(img1);
//        imgRmask.copyTo(img2);
//    }

//    namedWindow("L",WINDOW_FREERATIO);
//    namedWindow("R",WINDOW_FREERATIO);
//    imshow("L",img1);
//    waitKey(50);
//    imshow("R",img2);
//    waitKey(50);

//    numberOfDisparities = 8;

    int SADWindowSize =3, numberOfDisparities = img1.cols/3;
    int cn = img1.channels();
    sgbm->setPreFilterCap(35);//预处理滤波器截断值
    sgbm->setBlockSize(SADWindowSize);//SAD窗口大小
    sgbm->setP1(8*cn*SADWindowSize*SADWindowSize);//控制视差平滑度第一参数
    sgbm->setP2(32*cn*SADWindowSize*SADWindowSize);//控制视差平滑度第二参数
    sgbm->setMinDisparity(-10);//最小视差
    sgbm->setNumDisparities(numberOfDisparities);//视差搜索范围
    sgbm->setUniquenessRatio(5);//视差唯一性百分比
    sgbm->setSpeckleWindowSize(20);//检查视差连通区域变化度的窗口大小
    sgbm->setSpeckleRange(10);//视差变化阈值
    sgbm->setDisp12MaxDiff(-1);//
    switch (alg) {
    case STEREO_HH:
        sgbm->setMode(StereoSGBM::MODE_HH);
        break;
    case STEREO_SGBM:
        sgbm->setMode(StereoSGBM::MODE_SGBM);//采用全尺寸双通道动态编程算法
        break;
    case STEREO_3WAY:
        sgbm->setMode(StereoSGBM::MODE_SGBM_3WAY);
        break;
    }

    //Mat img1p, img2p, dispp;
    //copyMakeBorder(img1, img1p, 0, 0, numberOfDisparities, 0, IPL_BORDER_REPLICATE);
    //copyMakeBorder(img2, img2p, 0, 0, numberOfDisparities, 0, IPL_BORDER_REPLICATE);

    int64 t = getTickCount();
    float disparity_multiplier = 1.0f;
    sgbm->compute(img1, img2, disp);
    if (disp.type() == CV_16S)
        disparity_multiplier = 16.0f;
    t = getTickCount() - t;
    cout<<"Time elapsed: "<<t*1000/getTickFrequency()<<"ms\n";

    //disp = dispp.colRange(numberOfDisparities, img1p.cols);
    if( alg != STEREO_VAR )
        disp.convertTo(disp8, CV_8U, 255/(numberOfDisparities*16.));
    else
        disp.convertTo(disp8, CV_8U);

    Mat floatDisp;
    disp.convertTo(floatDisp, CV_32F, 1.0f / disparity_multiplier);

    applyColorMap(disp8,img_output3,COLORMAP_JET);
    if(display)
    {
        namedWindow("left", 1);
        imshow("left", img1);
        namedWindow("right", 1);
        imshow("right", img2);
        namedWindow("disparity", 0);
        imshow("disparity", img_output3);
        fflush(stdout);
        waitKey();
    }

    if(!disparity_filename.empty())
        imwrite(disparity_filename, img_output3);

    if(!point_cloud_filename.empty())
    {
        printf("storing the point cloud...");
        fflush(stdout);
        Mat xyz;
        reprojectImageTo3D(floatDisp, xyz, Q, false);
        saveXYZ(point_cloud_filename.c_str(), xyz);
        printf("\n");
    }

    disp8.release();disp.release();
    return floatDisp;
}

void bincular3d::extractDescriptors(vector<KeyPoint> kpl,vector<KeyPoint> kpr,
                                    Mat &dspl,Mat &dspr)
{
    // 1. 初始化
//    Ptr<ORB> feature = ORB::create();
    Ptr<SiftFeatureDetector> feature = SiftFeatureDetector::create();

    dspl=Mat();dspr=Mat();

    // 2. 提取特征点

    // 3. 计算特征描述符
    feature->compute(img1,kpl,dspl);
    feature->compute(img2,kpr,dspr);
}

bool bincular3d::extractLRfeatures(int minNum,int siftNum)
{
    // 1. 初始化
    Ptr<SiftFeatureDetector> featureSIFT = SiftFeatureDetector::create(siftNum);

    // 2. 提取特征点
    featureSIFT->detect(img1, keypointsL,imgLmask);
    featureSIFT->detect(img2, keypointsR,imgRmask);

//    // 绘制关键点
//    Mat keypoint_img = img1.clone();
//    drawKeypoints(img1, keypointsL, keypoint_img, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
//    namedWindow("KeyPoints ImageL",WINDOW_FREERATIO);
//    imshow("KeyPoints ImageL", keypoint_img);
//    waitKey(0);
//    keypoint_img = img2.clone();
//    drawKeypoints(img2, keypointsR, keypoint_img, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
//    namedWindow("KeyPoints ImageR",WINDOW_FREERATIO);
//    imshow("KeyPoints ImageR", keypoint_img);
//    waitKey(0);
//    keypoint_img.release();

    if(keypointsL.size()<minNum || keypointsR.size()<minNum)
    {
        keypointsL.clear();
        keypointsR.clear();
        descriptorsL.release();
        descriptorsR.release();
        return false;
    }

    // 3. 计算特征描述符
    featureSIFT->compute(img1,keypointsL,descriptorsL);
    featureSIFT->compute(img2,keypointsR,descriptorsR);

//    vector<Vec3b> colors(keypoints.size());
//    for (int i = 0; i < keypoints.size(); ++i)
//    {
//        Point2f& p = keypoints[i].pt;
//        colors[i] = imgInput.at<Vec3b>(p.y, p.x);
//    }

    return true;
}

bool bincular3d::matchLRfeatures(int minNum)
{
    vector<DMatch> knn_matches;
    BFMatcher matcher(NORM_L2);
    matcher.match(descriptorsL,descriptorsR,knn_matches);

    matchLR.clear();
    //     匹配对筛选
    float min_dist = 5000, max_dist = 0;
    // 找出所有匹配之间的最大值和最小值
    for (size_t r = 0; r < knn_matches.size(); r++)
    {
        float dist = knn_matches[r].distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }

    for (size_t r = 0; r < knn_matches.size(); ++r)
    {
        if (knn_matches[r].distance < max(2.0f * min_dist, 500.0f))
            matchLR.push_back(knn_matches[r]);
    }
    knn_matches.clear();

    if(matchLR.size()<minNum)
        return false;

    return  true;
}

vector<DMatch> bincular3d::matchMutifeatures(Mat dspl,Mat dspr)
{
    vector<DMatch> knn_matches;
    BFMatcher matcher(NORM_L2);
    matcher.match(dspl,dspr,knn_matches);

    vector<DMatch> result;
    for (size_t r = 0; r < knn_matches.size(); ++r)
        result.push_back(knn_matches[r]);
    knn_matches.clear();

    return result;
}

void bincular3d::getMatchedPoints()
{
    matchedPtL.clear();
    matchedPtR.clear();
    for (size_t i = 0; i < matchLR.size(); ++i)
    {
        matchedPtL.push_back(keypointsL[matchLR[i].queryIdx].pt);
        matchedPtR.push_back(keypointsR[matchLR[i].trainIdx].pt);
    }
}

void bincular3d::caluFundMat()
{
    Mat status;

    FundMat=findFundamentalMat(matchedPtL,matchedPtR,FM_RANSAC,3,0.999,status);
//    FundMat=findFundamentalMat(matchedPtL,matchedPtR,FM_LMEDS,3,0.99,status);
    cout<<"======Fundamental Matrix: ======"<<endl;
    cout<<FundMat.at<double>(0,0)<<" "<<FundMat.at<double>(0,1)<<" "<<FundMat.at<double>(0,2)<<" "<<endl;
    cout<<FundMat.at<double>(1,0)<<" "<<FundMat.at<double>(1,1)<<" "<<FundMat.at<double>(1,2)<<" "<<endl;
    cout<<FundMat.at<double>(2,0)<<" "<<FundMat.at<double>(2,1)<<" "<<FundMat.at<double>(2,2)<<" "<<endl;

    if(status.rows==matchLR.size())
    {
        //优化匹配结果
        vector<KeyPoint> leftInlier;
        vector<KeyPoint> rightInlier;
        vector<DMatch> inlierMatch;
        vector<Point2f> inlierPtL;
        vector<Point2f> inlierPtR;

        int index = 0;
        for (size_t i = 0; i < matchLR.size(); i++)
        {
            if (status.at<uchar>(i,0) != 0)
            {
                leftInlier.push_back(keypointsL[i]);
                rightInlier.push_back(keypointsR[i]);
                matchLR[i].trainIdx = index;
                matchLR[i].queryIdx = index;
                inlierMatch.push_back(matchLR[i]);
                inlierPtL.push_back(matchedPtL[i]);
                inlierPtR.push_back(matchedPtR[i]);
                index++;
            }
        }
        keypointsL.clear();
        keypointsR.clear();
        matchLR.clear();
        matchedPtL.clear();
        matchedPtR.clear();
        keypointsL = leftInlier;
        keypointsR = rightInlier;
        matchLR = inlierMatch;
        matchedPtL=inlierPtL;
        matchedPtR=inlierPtR;
    }

    //计算图像矫正的单应变换矩阵
    stereoRectifyUncalibrated(matchedPtL,matchedPtR,FundMat,img_size,leftH,rightH);
}

void bincular3d::reconstruct(vector<Point3f> &ptDisparity, vector<Point3f> &structure)
{

}
