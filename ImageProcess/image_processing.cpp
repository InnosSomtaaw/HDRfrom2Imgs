#include "image_processing.h"

Image_Processing_Class::Image_Processing_Class(QObject *parent)
{
//    img_input1,img_output1, img_input2, img_output2,img_output3 = NULL;
    initImgInOut();
    ai_ini_flg=false;hasInited=false;cam1Refreshed=false;cam2Refreshed = false;isDetecting=false;onGPU=false;
    img_filenames = new QList<QFileInfo>();
    save_count = 1,max_save_count =100,onceRunTime=0;
    mainwindowIsNext=false;mainwindowIsStopProcess=false;isSavingImage =false;
    inputFlags.insert(inputFlags.begin(),4,false);

    plcSignals = new QQueue<_PLC_PARAMETER>();
    basePTs = new QQueue<Point>();
    islifting = new QQueue<bool>();

    //默认输出：
    {
    _mPcParameter._iSystemStatus = _COMM_ENUMS::SYSTEM_STATUS::Closed;
    _mPcParameter._bDownPermit = _COMM_ENUMS::DOWN_PERMIT::Allow;
    _mPcParameter._bNoBoxAlarm = _COMM_ENUMS::NO_BOX_ALARM::NoBox;
    _mPcParameter._bSlingAlarm_20cm = _COMM_ENUMS::SLING_ALARM::NoAlarm;
    _mPcParameter._bSlingAlarm_25cm = _COMM_ENUMS::SLING_ALARM::NoAlarm;
    _mPcParameter._bTruckCheck = _COMM_ENUMS::TRUCK_CHECK::NoTruck;
    _mPcParameter._bDragAlarm = _COMM_ENUMS::SLING_ALARM::NoAlarm;
    }

    hdrProc=new imgHDR();

}

Image_Processing_Class::~Image_Processing_Class()
{
    delete hdrProc;
}

void Image_Processing_Class::initImgInOut()
{
    for(int i = 0; i<4; i++)
    {
        Mat imgIn, imgOut;
        img_inputs.push_back(imgIn.clone());
        img_outputs.push_back(imgOut.clone());
        inputFlags.push_back(false);
        vector<Point> pts;
        ptsVec.push_back(pts);
    }
}

void Image_Processing_Class::resetPar()
{
    mainwindowIsNext=false;mainwindowIsStopProcess=false;isSavingImage =false;
    save_count=0;
    ai_ini_flg=false;cam1Refreshed=false;cam2Refreshed = false;
    //默认输出：
    {
        _mPcParameter._iSystemStatus = _COMM_ENUMS::SYSTEM_STATUS::Closed;
        _mPcParameter._bDownPermit = _COMM_ENUMS::DOWN_PERMIT::Allow;
        _mPcParameter._bNoBoxAlarm = _COMM_ENUMS::NO_BOX_ALARM::NoBox;
        _mPcParameter._bSlingAlarm_20cm = _COMM_ENUMS::SLING_ALARM::NoAlarm;
        _mPcParameter._bSlingAlarm_25cm = _COMM_ENUMS::SLING_ALARM::NoAlarm;
        _mPcParameter._bTruckCheck = _COMM_ENUMS::TRUCK_CHECK::NoTruck;
        _mPcParameter._bDragAlarm = _COMM_ENUMS::SLING_ALARM::NoAlarm;
    }
    img_filenames->clear();
    plcSignals->clear();
    basePTs->clear();
    islifting->clear();
    hdrProc->hasInitialized=false;
}

void Image_Processing_Class::hdr2Imgs(bool onGPU)
{
    if (img_input1.empty() || img_input2.empty()
        || (img_input1.size!=img_input2.size))
        return;

    ipcMutex.lock();

    img_output1 = img_input1.clone();
    img_output2 = img_input2.clone();

    ipcMutex.unlock();

    if(!hdrProc->hasInitialized ||
        hdrProc->coffMat.size!=img_input1.size)
    {
        ustimer.start();
        hdrProc->coffMat = Mat(Size(img_input1.cols,img_input1.rows),
                               CV_32FC1);
        if(!onGPU)
            hdrProc->create_coff_mat();
        else
            hdrProc->create_coff_mat_cu();
        hdrProc->hasInitialized=true;
        cout<<"Time of coefficient initialization(ms): "<<
            ustimer.nsecsElapsed()/1000000<<endl;
    }

    if(!onGPU)
        img_output3=hdrProc->hdr2GrayImgs(img_output1,img_output2);
    else
        img_output3=hdrProc->hdr2GrayImgs_cu(img_output1,img_output2);

    double minGray,maxGray;
    minMaxLoc(img_output3,&minGray,&maxGray);
    if(maxGray!=minGray)
        img_output3.convertTo(img_output3,CV_32F,
                              1/(maxGray-minGray),minGray/(minGray-maxGray));
    img_output3.convertTo(img_output3,CV_8U,255);


//    imshow("result_raw: ",img_output3);
//    waitKey();

//    if (save_count % max_save_count == 0)
//    {
//        imwrite("test1.jpg",img_input1);
//        imwrite("test2.jpg",img_input2);

//        isSavingImage = true;
//    }
//    else
//        isSavingImage = false;

//    save_count++;
//    if (save_count > max_save_count)
//        save_count = 1;


    emit outputMulImgAIRequest();
    return;
}

void Image_Processing_Class::hdrImg(bool onGPU)
{
    if (img_input1.empty())
        return;

    ipcMutex.lock();

    img_output1 = img_input1.clone();

    ipcMutex.unlock();

    if(!hdrProc->hasInitialized ||
        hdrProc->coffMat.size!=img_input1.size)
    {
        ustimer.start();
        hdrProc->coffMat = Mat(Size(img_input1.cols,img_input1.rows),
                               CV_32FC1);
        if(!onGPU)
            hdrProc->create_coff_mat();
        else
            hdrProc->create_coff_mat_cu();
        hdrProc->hasInitialized=true;
        cout<<"Time of coefficient initialization(ms): "<<
            ustimer.nsecsElapsed()/1000000<<endl;
    }

    if(!onGPU)
        img_output2=hdrProc->hdr1GrayImgs(img_output1);
    else
        img_output2=hdrProc->hdr1GrayImgs(img_output1);;

    double minGray,maxGray;
    minMaxLoc(img_output2,&minGray,&maxGray);
    if(maxGray!=minGray)
        img_output2.convertTo(img_output2,CV_32F,
                              1/(maxGray-minGray),minGray/(minGray-maxGray));
    img_output2.convertTo(img_output2,CV_8U,255);
    Scalar meanSca=mean(img_output2);
    cout<<"mean of output: "<<meanSca(0)<<endl;

    //    if (save_count % max_save_count == 0)
    //    {
    //        imwrite("test1.jpg",img_input1);
    //        imwrite("test2.jpg",img_input2);

    //        isSavingImage = true;
    //    }
    //    else
    //        isSavingImage = false;

    //    save_count++;
    //    if (save_count > max_save_count)
    //        save_count = 1;


    emit outputImgProcessedRequest();
    return;

}

void Image_Processing_Class::resizeCompare()
{
    if (img_input1.empty())
        return;

    ipcMutex.lock();
    //测试线程：
    {
        img_output1 = img_input1.clone();
//        Laplacian(img_output1,img_output1,img_output1.depth());
        resize(img_output1,img_output1,Size(),0.5,0.5);

        ipcMutex.unlock();

//        if (save_count % max_save_count == 0)
//        {
//            imwrite("test1.jpg",img_output1);
//            isSavingImage = true;
//        }
//        else
//            isSavingImage = false;

//        save_count++;
//        if (save_count > max_save_count)
//            save_count = 1;

        emit outputImgProcessedRequest();
        return;
    }

}

void Image_Processing_Class::startProcessOnce()
{
    switch (workCond) {
    case HDRfrom2Img:
        hdr2Imgs();
        break;
    case HDRfrom2ImgGPU:
        hdr2Imgs(true);
        break;
    case ResizeShow:
        resizeCompare();
        break;
    case HDRfrom1Img:
        hdrImg();
        break;
    default:
        break;
    }
}

void Image_Processing_Class::startPicsProcess()
{
    emit mainwindowStatusRequest();
    do
    {
        bool flg = processFilePic();
        if(mainwindowIsStopProcess || !flg)
            break;
        QThread::sleep(10);
        emit mainwindowStatusRequest();
    }while(mainwindowIsNext);
}

void Image_Processing_Class::startMulCamTemp(QImage recvImg, int i)
{
    QImage2Mat(recvImg, img_inputs[i]);
}

void Image_Processing_Class::start1CamProcess(QImage receivedImg)
{
    if(receivedImg.isNull())
        return;
    Mat temp_img(receivedImg.height(),receivedImg.width(),
                CV_8UC4,receivedImg.bits(),
                size_t(receivedImg.bytesPerLine()));
    cvtColor(temp_img,img_input1,COLOR_BGRA2BGR);
//    cvtColor(temp_img,img_input1,CV_BGRA2BGR);
    temp_img.release();
}


void QImage2Mat(QImage img, Mat& imgMat)
{
//    printf("QImage height: %d, width: %d", img.height(), img.width());

//    Mat temp_img(img.height(),img.width(),
//                CV_8UC4,img.bits(),
//                img.bytesPerLine());
//    printf("cvtColor into imgMat");
//    cvtColor(temp_img,imgMat,COLOR_BGRA2BGR);
    Mat temp_img(img.height(), img.width(), CV_8UC3, img.bits(), img.bytesPerLine());
    cvtColor(temp_img, imgMat, COLOR_RGB2BGR);
    temp_img.release();
//    printf("QImage2Mat ended.");
}


void Image_Processing_Class::startMulCamProcess(QImage recvImg, int i)
{
//    if(inputFlags[i])
//    {
//        cout<<i<<" unsync happend."<<endl;
//        return;
//    }
    QImage2Mat(recvImg, img_inputs[i]);
    inputFlags[i] = true;
    for(size_t j = 0; j<4; j++)
    {
        if(!inputFlags[j])
            return;
    }
//    testMulCamAIProcess();
//    testMulStereoMatcher();
}


void Image_Processing_Class::testMulStereoMatcher()
{
//    printf(" testMulStereoMatcher start");
    if(!ipcMutex.tryLock())
    {
        printf("no lock!");
        return;
    }
    if(img_inputs[2].empty() | img_inputs[3].empty())
    {
        printf("image inputs are empty.");
        return;
    }
    vector<Point> points;
    points.push_back(Point(276, 288));
    points.push_back(Point(278, 288));
    vector<Point3f> pts3;
    stereoMatcher.calcXYZ(img_inputs[2], img_inputs[3], points, pts3);
    pt1 = pts3[0];
    pt2 = pts3[1];
    printf("%f, %f, %f\n", pt1.x, pt1.y, pt1.z);
    for(int k = 0; k<4; k++)
    {
//        resize(img_inputs[k], img_inputs[k], Size(this_aue->SrcImage.width, this_aue->SrcImage.height));
        img_outputs[k] = img_inputs[k].clone();
    }
    for(int k = 0; k<4; k++)
        inputFlags[k] = false;

    ipcMutex.unlock();
    emit outputMulImgAIRequest();
}

void Image_Processing_Class::changeProcPara(QString qstr, int wc)
{
    switch (wc) {
    case WorkConditionsEnum::HDRfrom1Img:
    case WorkConditionsEnum::HDRfrom1ImgGPU:
        hdrProc->idConst=qstr.toFloat();
        break;
    default:
        break;
    }

}

bool Image_Processing_Class::processFilePic()
{
    if (img_filenames->count() > 0)
    {
        img_input1 = imread(img_filenames->at(0).filePath().toLocal8Bit().toStdString());

        if(img_input1.empty())
            return false;

        startProcessOnce();

        img_filenames->removeAt(0);
        return true;
    }
    else
    {
        return false;
    }
}

void ImageWriter::run()
{
    usrtimer.start();

    //按需保存图片：
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy_MM_dd");
    QString current_time =current_date_time.toString("hh_mm_ss_zzz");
    QString dir_str = "./SaveFile/"+current_date+"/";
    QDir dir;
    if (!dir.exists(dir_str))
        dir.mkpath(dir_str);
    QString fn;
    switch (method) {
    case 1:
        fn=dir_str+current_time+"_mannulsaved.bmp";
        break;
    case 2:
        fn=dir_str+current_time+"_mannulsaved.jpg";
        break;
    default:
        fn=dir_str+current_time+"_mannulsaved.png";
        break;
    }

    qimg.save(fn);
    cout<<"Save time(width:"<<qimg.width()<<",height:"<<qimg.height()<<
          "): "<<usrtimer.elapsed()<<" ms."<<endl;

}
