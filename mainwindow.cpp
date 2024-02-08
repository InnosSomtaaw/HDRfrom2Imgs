#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtXml/QDomDocument>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //显示窗体初始化
    initImageBoxes();
    //初始化程序工况
    initWorkCondition();
    //文本输出初始化
    initTextBrowsers();
    //初始化参数设置
    initBaseSettings();
    //图像处理类初始化
    initImageProcess();
    //初始化网络相机设置
    initCamSettings();
    //数字相机类初始化（硬盘录像机读取）
//    mNVR = new cameraDAHUA;
//    nvrIP = "10.11.151.52";
//    nvrPort = 37777;
//    camUserName = "admin";
//    camPassWord = "Astro123";
//    mNvrThread = new QThread();
//    mNVR->moveToThread(mNvrThread);
//    mNvrThread->start();
    //数字相机类初始化（rtsp读取）
    initVideoPlayers();
    //初始化输入输出图像列表
    initImageInOuts();
    //默认AI路径初始化
//    initDefaultAIPaths();

//    mNVR->cameraLogin(nvrIP,nvrPort,camUserName,camPassWord);
//    mNVR->camerasGetConfig();
//    mNVR->imgboxHandle[40]=HWND(ui->imagebox4->winId());
//    mNVR->imgboxHandle[41]=HWND(ui->imagebox2->winId());
//    mNVR->imgboxHandle[42]=HWND(ui->imagebox3->winId());
//    mNVR->imgboxHandle[43]=HWND(ui->imagebox1->winId());
}

MainWindow::~MainWindow()
{
//    if(imgProcess_main->ai_ini_flg)
//        imgProcess_main->this_aue->cppReleaseAI();

    delete ui;
}

void MainWindow::initCamSettings()
{
//    QDomDocument doc;
//    QFile file(_CAMS_SETTING_FILES);
//    if(!file.open(QFile::ReadOnly|QFile::Text))
//    {
//        cout<<"camera settings file can't be found."<<endl;
//        return;
//    }
//    file.close();
//    QString str, name;
//    if(doc.setContent(&file))
//    {
//        int i = 0;
//        QDomElement docElement = doc.documentElement();
//        QDomNode n = docElement.firstChild();
//        while(!n.isNull())
//        {
//            QDomElement e = n.toElement();
//            if(!e.isNull())
//            {
//                str = e.tagName();
////                name = e.attribute("name");
//                if(str == "webCam")
//                {
//                    camUserNames.push_back(e.attribute("userName"));
//                    camPasswords.push_back(e.attribute("password"));
//                    camIPs.push_back(e.attribute("IP"));
//                    camPorts.push_back(e.attribute("port"));
//                    camChannels.push_back(e.attribute("channel"));
//                    camCodes.push_back(e.attribute("code"));
//                    urls.push_back("rtsp://"+camUserNames[i]+":"+camPasswords[i]+"@"+camIPs[i]+":"+camPorts[i]+"/"+
//                            camCodes[i]+"/"+camChannels[i]+"/main/av_stream");
//                    i++;
//                }
//                else if(str == "videoFile")
//                {
//                    urls.push_back(e.attribute("path"));
//                }
//            }
//            n = n.nextSibling();
//        }
//    }
}

void MainWindow::initImageInOuts()
{
//    camCount = 4;
//    for(int i = 0; i<camCount; i++)
//    {
//        Mat imgIn, imgOut;
//        QImage qImg;
//        img_inputs.push_back(imgIn.clone());
//        img_outputs.push_back(imgOut.clone());
//        qimgs.push_back(qImg);
//        counts.push_back(0);
//        isCapturing1s.push_back(false);
//        ws.push_back(0);
//        hs.push_back(0);
//    }
//    maxCount = 10000;
}

void MainWindow::initImageBoxes()
{
    ui->imagebox1->setScene(&scene1);
    scene1.addItem(&pixmapShow1);

    ui->imagebox2->setScene(&scene2);
    scene2.addItem(&pixmapShow2);

    ui->imagebox3->setScene(&scene3);
    scene3.addItem(&pixmapShow3);

    connect(this->ui->imagebox1,&PictureView::loadImgRequest,
            this,&MainWindow::on_imageboxOpenImage);
    connect(this->ui->imagebox2,&PictureView::loadImgRequest,
            this,&MainWindow::on_imageboxOpenImage);
    connect(this->ui->imagebox1,&PictureView::saveImgRequest,
            this,&MainWindow::on_imageboxSaveImage);
    connect(this->ui->imagebox2,&PictureView::saveImgRequest,
            this,&MainWindow::on_imageboxSaveImage);
    connect(this->ui->imagebox3,&PictureView::loadImgRequest,
            this,&MainWindow::on_imageboxOpenImage);
    connect(this->ui->imagebox3,&PictureView::saveImgRequest,
            this,&MainWindow::on_imageboxSaveImage);

    ui->buttonStartCapture->setEnabled(false);
}

void MainWindow::initDefaultAIPaths()
{
    proj_path = ".\\AI_Initial_Data\\TGS_train\\TGS_train.proj";
    img_path = ".\\AI_Initial_Data\\TGS_train\\srcImg.bmp";
    node_name = "hasBoxTGS hasNoBox noBoxTGS ";
}

void MainWindow::initImageProcess()
{
    bin3d=new bincular3d;
    bin3d->workCond=workCond;

    connectIPC();

    m_imgprocsThread = new QThread();
    imgProcess_main->moveToThread(m_imgprocsThread);
    m_imgprocsThread->start();

    imgProcess_main->save_count = 0;
}

void MainWindow::initVideoPlayers()
{
    isCapturing1=false;isCapturing2=false;
    mPlayer1 = new VideoPlayer;
    urls.push_back("rtsp://admin:Lead123456@192.168.137.98:554/h265/ch1/main/av_stream");
    mPlayer1->videoURL = urls[0];
    mPlayer2 = new VideoPlayer;
//    mPlayer2->videoURL = urls[1];
    connectCamera();
}

void MainWindow::initBaseSettings()
{
    isDetecting=false;
    meanFPS1=0;
    QThreadPool::globalInstance()->setMaxThreadCount(8);
}

void MainWindow::initTextBrowsers()
{
    ui->textBrowser1->setStyleSheet("background:transparent;border-width:0;border-style:outset");
    strOutput1="place holder 1...\n";
    ui->textBrowser1->setText(strOutput1);
}

void MainWindow::initWorkCondition()
{
    iniRW = new QSettings("LastSettings.ini",QSettings::IniFormat);
    workCond=WorkConditionsEnum(iniRW->value("WorkCondition/WorkCondition").toInt());
    ui->condComboBox->setCurrentIndex(workCond);
}

void MainWindow::on_buttonOpenAIProject_clicked()
{
    proj_path = QFileDialog::getOpenFileName(this,tr("Open Project"),".\\AI_Initial_Data\\",
                                             tr("Project File(*.proj)"));
    if (proj_path.isEmpty())
        return;
    proj_path = proj_path.replace("/","\\");

    img_path = QFileDialog::getOpenFileName(this,tr("Open Sample"),".\\AI_Initial_Data\\",
                                            tr("Sample Picture(*.bmp)"));
    if (img_path.isEmpty())
        return;

    QStringList node_nameList = QFileDialog::getOpenFileNames(this,tr("Open Nodes"),".\\AI_Initial_Data\\",
                                                              tr("Node Files(*.te)"));
    if(node_nameList.isEmpty())
        return;

    node_name.clear();
    for(int i=0;i<node_nameList.size();i++)
    {
        QFileInfo fi = QFileInfo(node_nameList[i]);
        QByteArray ba;
        ba.append(fi.fileName());
        QList<QByteArray> baList = ba.split('.');
        QString str = QString(baList[0]);
        node_name.append(str+" ");
    }
}

//void MainWindow::on_buttonProcessOnce_clicked()
//{
//    this->isDetecting = false;

////    if(this_mb->isConnected)
////        this_mb->startProcessOnce();

//    QImage qimg1 = pixmapShow1.pixmap().toImage();

//    Mat temp_img;
//    temp_img = Mat(qimg1.height(),qimg1.width(),
//                CV_8UC4,qimg1.bits(),
//                qimg1.bytesPerLine());
//    cvtColor(temp_img,img_input1,COLOR_BGRA2BGR);
////    cvtColor(temp_img,img_input1,CV_BGRA2BGR);
//    temp_img.release();

//    if(img_input1.empty())
//        return;

//    imgProcess_main->img_input1 = img_input1;
////    imgProcess_main->img_input2 = img_input1.clone();

//    //测试AI多工程
////    if(img_input2.empty())
////        imgProcess_main_t->img_input1 = img_input1.clone();
////    else
////        imgProcess_main_t->img_input1 = img_input2;

//    if(!this->m_imgprocsThread->isInterruptionRequested())
//        emit startProcessOnceRequest();
//    //测试AI多工程
////    if(!this->m_imgprocsThread->isInterruptionRequested())
////        emit startProcessOnceRequest1();
////    if(!this->m_imgprocsThread_t->isInterruptionRequested())
////        emit startProcessOnceRequest2();
//}

void MainWindow::on_imagebox_refresh()
{
    imgProcess_main->inputFlags[2]=true;

    int gap2recv=time1.elapsed();
    time1.start();

    //界面状态显示
    QString tempStr;
    double fps;
    imgProcess_main->save_count++;
    tempStr="1st camera refresh time(onGPU "+QString::number(int(imgProcess_main->onGPU))+
            "): "+QString::number(gap2recv)+"ms. In which process consumed: "+
            QString::number(imgProcess_main->onceRunTime)+"ms.\n";
    strOutput1+=tempStr;
    fps=1000/double(gap2recv);
    meanFPS1+=fps;
    fps=meanFPS1/double(imgProcess_main->save_count);
    tempStr="1st camera FPS(processed): "+QString::number(fps,'f',1)+".\n";
    strOutput1+=tempStr;
    imgProcess_main->max_save_count+=imgProcess_main->onceRunTime;
    fps=double(imgProcess_main->max_save_count)/double(imgProcess_main->save_count);
    tempStr="1st camera processed count: "+QString::number(imgProcess_main->save_count)+
            ",mean process time: "+QString::number(fps,'f',1)+".\n";
    strOutput1+=tempStr;

    ui->textBrowser1->setText(strOutput1);
    strOutput1.clear();
    tempStr.clear();

    QImage disImage;
    //    窗体1显示
    if(img_output1.channels()==1)
        disImage = QImage(img_output1.data,img_output1.cols,img_output1.rows,
                          img_output1.cols*img_output1.channels(),QImage::Format_Grayscale8);
    else
        disImage = QImage(img_output1.data,img_output1.cols,img_output1.rows,
                          img_output1.cols*img_output1.channels(),QImage::Format_BGR888);
    pixmapShow1.setPixmap(QPixmap::fromImage(disImage));

}

void MainWindow::on_imageboxes_refresh()
{
    if(!imgProcess_main->ipcMutex.tryLock())
        return;

    QImage disImage;

    img_input1=imgProcess_main->img_input1;
    //窗体1显示
    if(img_input1.channels()==1)
        disImage = QImage(img_input1.data,img_input1.cols,img_input1.rows,
                          img_input1.cols*img_input1.channels(),QImage::Format_Grayscale8);
    else
    {
        cvtColor(img_input1, img_input1, COLOR_BGR2RGB);//图像格式转换
        disImage = QImage(img_input1.data,img_input1.cols,img_input1.rows,
                          img_input1.cols*img_input1.channels(),QImage::Format_RGB888);
    }
    pixmapShow1.setPixmap(QPixmap::fromImage(disImage));

    switch (workCond){
    case ResizeShow:
        if(!isDetecting)
            break;
        img_output1 = imgProcess_main->img_output1;
        //窗体2显示
        if(img_output1.channels()==1)
            disImage = QImage(img_output1.data,img_output1.cols,img_output1.rows,
                              img_output1.cols*img_output1.channels(),QImage::Format_Grayscale8);
        else
        {
            cvtColor(img_output1, img_output1, COLOR_BGR2RGB);//图像格式转换
            disImage = QImage(img_output1.data,img_output1.cols,img_output1.rows,
                              img_output1.cols*img_output1.channels(),QImage::Format_RGB888);
        }
        pixmapShow2.setPixmap(QPixmap::fromImage(disImage));
        break;
    case HDRfrom1ImgGPU:
    case HDRfrom1Img:
        //窗体2显示
        if(imgProcess_main->img_output2.channels()==1)
            disImage = QImage(imgProcess_main->img_output2.data,
                              imgProcess_main->img_output2.cols,
                              imgProcess_main->img_output2.rows,
                              imgProcess_main->img_output2.cols*imgProcess_main->img_output3.channels(),
                              QImage::Format_Grayscale8);
        else
            disImage = QImage(imgProcess_main->img_output2.data,
                              imgProcess_main->img_output2.cols,
                              imgProcess_main->img_output2.rows,
                              imgProcess_main->img_output2.cols*imgProcess_main->img_output3.channels(),
                              QImage::Format_RGB888);
        pixmapShow2.setPixmap(QPixmap::fromImage(disImage));
        break;
    case HDRfrom2ImgGPU:
    case HDRfrom2Img:
        //窗体2显示
        disImage = QImage(img_input2.data,img_input2.cols,img_input2.rows,
                          img_input2.cols*img_input2.channels(),QImage::Format_RGB888);
        pixmapShow2.setPixmap(QPixmap::fromImage(disImage));
        //窗体3显示
        if(imgProcess_main->img_output3.channels()==1)
            disImage = QImage(imgProcess_main->img_output3.data,
                              imgProcess_main->img_output3.cols,
                              imgProcess_main->img_output3.rows,
                              imgProcess_main->img_output3.cols*imgProcess_main->img_output3.channels(),
                              QImage::Format_Grayscale8);
        else
            disImage = QImage(imgProcess_main->img_output3.data,
                              imgProcess_main->img_output3.cols,
                              imgProcess_main->img_output3.rows,
                              imgProcess_main->img_output3.cols*imgProcess_main->img_output3.channels(),
                              QImage::Format_RGB888);

        pixmapShow3.setPixmap(QPixmap::fromImage(disImage));
        ui->imagebox3->Adapte();
    default:
        break;
    }

    imgProcess_main->ipcMutex.unlock();


    //界面状态显示
//    ui->buttonProcessOnce->setEnabled(true);
    ui->textSavingCount->setStyleSheet("background-color: rgb(255,255,255)");

    if (imgProcess_main->isSavingImage)
        ui->textSavingCount->setStyleSheet("background-color: rgb(176,196,222)");
    else
        ui->textSavingCount->setStyleSheet("background-color: rgb(255,255,255)");
    /*QString str;
    str.sprintf("x: %f, y: %f, z: %f", imgProcess_main->pt1.x/1000, imgProcess_main->pt1.y/1000, imgProcess_main->pt1.z/1000);*/

    QString str;
    QString tempStr;
    for(int i = 0; i<2; i++)
    {
        tempStr.sprintf("The %dth camera:\n", i+1);
        str += tempStr;
        for(int k = 0; k<imgProcess_main->ptsVec[i].size(); k++)
        {
            tempStr.sprintf("x: %4d, y: %4d\n", imgProcess_main->ptsVec[i][k].x, imgProcess_main->ptsVec[i][k].y);
            str += tempStr;
        }
    }
    ui->textBrowser1->setText(str);
    str.clear();
}

void MainWindow::on_mainwindowStatus_inform()
{
//    if(ui->buttonOpenImageList->text()=="Next")
//        imgProcess_main->mainwindowIsNext = true;
//    else
//        imgProcess_main->mainwindowIsNext = false;

    if(!isDetecting)
        imgProcess_main->mainwindowIsStopProcess = true;
    else
        imgProcess_main->mainwindowIsStopProcess = false;

    if(imgProcess_main->img_filenames->isEmpty())
        return;

    if(imgProcess_main->img_filenames->count()>0)
        qDebug()<<imgProcess_main->img_filenames->at(0).fileName();
    else if(imgProcess_main->img_filenames->count()==0)
        ui->buttonReset->click();
}

void MainWindow::connectIPC()
{
    switch (workCond){
    case ScheimpflugCalib:
        connect(this,&MainWindow::startPicsProcessRequest,
                bin3d,&bincular3d::startPicsProcess);
        connect(bin3d,&bincular3d::outputImgProcessedRequest,
                this,&MainWindow::on_imagebox_refresh);
        bin3d->inputFlags[0]=true;
        imgProcess_main=bin3d;
        break;
    default:
        imgProcess_main = new Image_Processing_Class;
        imgProcess_main->workCond=workCond;
        connect(this,&MainWindow::startPicsProcessRequest,
                imgProcess_main,&Image_Processing_Class::startPicsProcess);
        connect(this, SIGNAL(startMulCamProcessRequest(QImage, int)),
                imgProcess_main, SLOT(startMulCamProcess(QImage, int)));
        connect(this, SIGNAL(startMulCamTempRequest(QImage, int)),
                imgProcess_main, SLOT(startMulCamTemp(QImage, int)));
        connect(imgProcess_main, &Image_Processing_Class::outputMulImgAIRequest,
                this, &MainWindow::on_imageboxes_refresh);
        connect(imgProcess_main,&Image_Processing_Class::outputImgProcessedRequest,
                this,&MainWindow::on_imageboxes_refresh);
        connect(imgProcess_main,&Image_Processing_Class::mainwindowStatusRequest,
                this,&MainWindow::on_mainwindowStatus_inform);
        connect(this,&MainWindow::changeProcParasRequest,
                imgProcess_main,&Image_Processing_Class::changeProcPara);
        break;
    }
}

void MainWindow::connectCamera()
{
    connect(mPlayer1,SIGNAL(sig_GetOneFrame(QImage)),
            this,SLOT(slotGetOneFrame1(QImage)));
    connect(mPlayer2, SIGNAL(sig_GetOneFrame(QImage)),
            this, SLOT(slotGetOneFrame2(QImage)));

//    connect(mNVR,&cameraDAHUA::sigGetOneFrame1,
//            this,&MainWindow::slotGetOneFrame1);
//    connect(mNVR,&cameraDAHUA::sigGetOneFrame2,
    //            this,&MainWindow::slotGetOneFrame2);
}

void MainWindow::on_imageboxOpenImage()
{
    QString fileName = QFileDialog::getOpenFileName(
                this,tr("Open Image"),".",tr("Image File(*.png *.jpg *.jpeg *.bmp)"));

    if (fileName.isEmpty())
        return;

    QImageReader qir(fileName);
    QImage img=qir.read();

    img=img.convertToFormat(QImage::Format_RGB888);
    Mat srcImage(img.height(), img.width(), CV_8UC3,img.bits(), img.bytesPerLine());

    QObject *ptrSender=sender();
    if(ptrSender==ui->imagebox1)
    {
        img_input1 = srcImage.clone();
        imgProcess_main->img_input1=img_input1;
        //窗体1显示
        pixmapShow1.setPixmap(QPixmap::fromImage(img));
        ui->imagebox1->Adapte();
    }
    else if(ptrSender==ui->imagebox2)
    {
        img_input2 = srcImage.clone();
        imgProcess_main->img_input2=img_input2;
        //窗体2显示
        pixmapShow2.setPixmap(QPixmap::fromImage(img));
        ui->imagebox2->Adapte();
    }
    else
    {
        img_output3 = srcImage.clone();
        //窗体3显示
        pixmapShow3.setPixmap(QPixmap::fromImage(img));
        ui->imagebox3->Adapte();
    }

    srcImage.release();
}

void MainWindow::on_imageboxSaveImage()
{
    ImageWriter *iw=new ImageWriter;
    //保存方式：0-png;1-bmp;2-jpg;
    iw->method=2;

    QObject *ptrSender=sender();
    if(ptrSender==ui->imagebox1)
        iw->qimg=pixmapShow1.pixmap().toImage();
    else if(ptrSender==ui->imagebox2)
        iw->qimg=pixmapShow2.pixmap().toImage();

    QThreadPool::globalInstance()->start(iw);
}

//void MainWindow::on_buttonOpenVideo_clicked()
//{
//    if(ui->buttonOpenVideo->text()=="OpenVideo" && isCapturing1s[0]==false)
//    {
//        QString videoName = QFileDialog::getOpenFileName(this,tr("Open Video"),".",
//                                                      tr("Video File(*.mp4 *.avi *.wmv *dav)"));
//        if (videoName.isEmpty())
//            return;

//        mPlayer1->videoURL = videoName;
//        isCapturing1s[0] = true;
//        mPlayer1->isCapturing1 = isCapturing1s[0];
//        mPlayer1->startPlay();

//        ui->buttonOpenVideo->setText("Stop");
//    }
//    else if(ui->buttonOpenVideo->text()=="Stop" && isCapturing1s[0]==true)
//    {
//        isCapturing1s[0] = false;
//        mPlayer1->isCapturing1 = isCapturing1s[0];
//        ui->buttonOpenVideo->setText("Continue");
//    }
//    else if(ui->buttonOpenVideo->text()=="Continue" && isCapturing1s[0]==false)
//    {
//        isCapturing1s[0] = true;
//        mPlayer1->isCapturing1 = isCapturing1s[0];
//        ui->buttonOpenVideo->setText("Stop");
//    }
//}

void MainWindow::on_buttonProcess_clicked()
{
    if(imgProcess_main->img_filenames->size()==0
        && !isCapturing1 && !isCapturing2)
    {
        imgProcess_main->startProcessOnce();
        return;
    }

    if(ui->buttonProcess->text()=="Process" && isDetecting==false)
    {
        isDetecting = true;
        ui->buttonProcess->setText("StopProcess");
        ui->condComboBox->setEnabled(false);

//        if (ui->buttonOpenImageList->text() == "Next")
//            if(!this->m_imgprocsThread->isInterruptionRequested())
//                emit startPicsProcessRequest();

    }
    else if (ui->buttonProcess->text() == "StopProcess" && isDetecting == true)
    {
        isDetecting = false;
        ui->buttonProcess->setText("Process");
        ui->condComboBox->setEnabled(true);
    }
}

void MainWindow::on_buttonReset_clicked()
{
//    ui->buttonOpenImageList->setText("OpenImageList");
//    ui->buttonOpenVideo->setText("OpenVideo");
    isCapturing1=false;
    isCapturing2=false;
    printf("reset clicked!");
    mPlayer1->isCapturing = isCapturing1;
//    mPlayer1->videoURL = urls[0];
    mPlayer2->isCapturing = isCapturing2;
//    mPlayer2->videoURL = urls[1];

    ui->buttonOpenImgList->setText("OpenImgList");
    isDetecting = false;
    ui->buttonProcess->setText("Process");
    ui->buttonStartCapture->setText("StartCapture");
    imgProcess_main->resetPar();
}

void MainWindow::on_textSavingCount_textChanged()
{
    QString qstr = ui->textSavingCount->toPlainText();
    switch (workCond) {
    case HDRfrom1Img:
    case HDRfrom1ImgGPU:
        emit changeProcParasRequest(qstr,workCond);
        break;
    default:
        imgProcess_main->max_save_count = qstr.toInt();
        break;
    }
}

void MainWindow::on_buttonStartCapture_clicked()
{
    if(ui->buttonStartCapture->text()=="StartCapture" && isCapturing1==false && isCapturing2 == false)
    {
        isCapturing1=true;
        isCapturing2=true;
        ui->buttonStartCapture->setText("StopCapture");

        mPlayer1->isCapturing = isCapturing1;
        mPlayer1->startPlay();
//        mPlayer2->isCapturing1 = isCapturing1s[1];
//        mPlayer2->startPlay();

//        mNVR->isCapturing1=isCapturing1s[0];
//        mNVR->cameraPlay4();

        ui->editCheckBox->setEnabled(false);
    }
    else if(ui->buttonStartCapture->text()=="StopCapture"/* && isCapturing1s[0]&& isCapturing2&& isCapturing13 && isCapturing14*/)
    {
        isCapturing1=false;
        isCapturing2=false;

        ui->buttonStartCapture->setText("StartCapture");

        mPlayer1->isCapturing = isCapturing1;
//        mPlayer2->isCapturing1 = isCapturing1s[1];

//        mNVR->isCapturing1=isCapturing1s[0];
//        for(int i=40;i<44;i++)
//            mNVR->cameraStopPlay(i);

        if((!imgProcess_main->img_inputs[0].empty())&(!imgProcess_main->img_inputs[2].empty()))
        {
            ui->editCheckBox->setEnabled(true);
        }
    }
}

void MainWindow::on_submitBtn_clicked()
{
//    cv::destroyAllWindows();
//    int index = workCond*2;
//    basePoints[index].x = ui->lineEditX1->text().toInt();
//    basePoints[index].y = ui->lineEditY1->text().toInt();
//    index++;
//    basePoints[index].x = ui->lineEditX2->text().toInt();
//    basePoints[index].y = ui->lineEditY2->text().toInt();

//    FileStorage fs(_BASE_SETTING_FILES, FileStorage::WRITE);
//    if(!fs.isOpened())
//        return;
//    fs<<"workCondition"<< (int)workCond;

//    fs<<"baseSettings"<<"[";
//    for(int i = 0; i<basePoints.size(); i++)
//    {
//        fs<<basePoints[i];
//    }
//    fs<<"]";
//    ui->editCheckBox->setCheckState(Qt::Unchecked);

}

void MainWindow::onMouseFront(int event, int x, int y, int flags)
{
    if(event == EVENT_LBUTTONDOWN)
    {
        QString xstr, ystr;
        xstr.sprintf("%d", x);
        ystr.sprintf("%d", y);
        ui->lineEditX1->setText(xstr);
        ui->lineEditY1->setText(ystr);
        Mat temp = imgProcess_main->img_inputs[0].clone();
        drawMarker(temp, Point(x,y), Scalar(0,0,255));
        imshow("frontLeftImg", temp);
    }
}

void MainWindow::onMouseBack(int event, int x, int y, int flags)
{
    if(event == EVENT_LBUTTONDOWN)
    {
        QString xstr, ystr;
        xstr.sprintf("%d", x);
        ystr.sprintf("%d", y);
        ui->lineEditX2->setText(xstr);
        ui->lineEditY2->setText(ystr);
        Mat temp = imgProcess_main->img_inputs[2].clone();
        drawMarker(temp, Point(x,y), Scalar(0,0,255));
        imshow("backLeftImg", temp);
    }
}

void on_mouse_front(int event, int x, int y, int flags, void* params)
{
    MainWindow* win = static_cast<MainWindow*>(params);
    win->onMouseFront(event, x, y, flags);

}

void on_mouse_back(int event, int x, int y, int flags, void* params)
{
    MainWindow* win = static_cast<MainWindow*>(params);
    win->onMouseBack(event, x, y, flags);
}

void MainWindow::on_editCheckBox_stateChanged(int state)
{
    if(state == Qt::Unchecked)
    {
        ui->lineEditX1->setEnabled(false);
        ui->lineEditX2->setEnabled(false);
        ui->lineEditY1->setEnabled(false);
        ui->lineEditY2->setEnabled(false);
        ui->submitBtn->setEnabled(false);
    }
    else if(state == Qt::Checked)
    {
        ui->buttonReset->click();
        ui->lineEditX1->setEnabled(true);
        ui->lineEditX2->setEnabled(true);
        ui->lineEditY1->setEnabled(true);
        ui->lineEditY2->setEnabled(true);
        ui->submitBtn->setEnabled(true);
        imshow("frontLeftImg", imgProcess_main->img_inputs[0]);
        imshow("backLeftImg", imgProcess_main->img_inputs[2]);
        setMouseCallback("frontLeftImg", on_mouse_front, this);
        setMouseCallback("backLeftImg", on_mouse_back, this);
    }
}

void MainWindow::requestProcess(QImage img, int i)
{
    //对应startMulCamProcessRequest(QImage, int)
    if(imgProcess_main->ipcMutex.tryLock())
    {
        switch (workCond) {
        case ScheimpflugCalib:
            bin3d->imgL_filenames=imgProcess_main->img_filenames;
            bin3d->monoCalib(Size(7,7),0.002,true,false);
            break;
        default:
            emit startMulCamProcessRequest(img, i);
            break;
        }
        imgProcess_main->ipcMutex.unlock();
    }

    /*//对应startMulCamProcessRequest(vector<QImage>)
    qimgs[i] = img;
    if(counts[i]<maxCount)
        counts[i]++;
    else
        counts[i] = 0;

    for(int j = 1; j<4; i++)
    {
        if(counts[0] != counts[j])
            return;
    }

    if(imgProcess_main->ipcMutex.tryLock())
    {
        emit startMulCamProcessRequest(qimgs);
        imgProcess_main->ipcMutex.unlock();
    }*/
}

void MainWindow::slotGetOneFrame1(QImage img)
{
//    ws[0]=img.width();hs[0]=img.height();
//    if(mPlayer1->hasFinished)
//    {
//        isCapturing1s[0] = false;
//        mPlayer1->isCapturing1 = isCapturing1s[0];
////        ui->buttonOpenVideo->setText("OpenVideo");
//        return;
//    }

    if(mPlayer1->isCapturing && !isDetecting)
    {
        //窗体1显示
        pixmapShow1.setPixmap(QPixmap::fromImage(img));
//        ui->imagebox1->Adapte();
//        emit startMulCamTempRequest(img, 0);
//        QImage2Mat(img, img_inputs[0]);
    }

    if(mPlayer1->isCapturing && isDetecting)
    {
        requestProcess(img, 0);
    }
}

void MainWindow::slotGetOneFrame2(QImage img)
{
    ws[1]=img.width(); hs[1]=img.height();
//    if(mPlayer2->hasFinished)
//    {
//        isCapturing1s[1] = false;
//        mPlayer2->isCapturing1 = isCapturing1s[1];
////        ui->buttonOpenVideo->setText("OpenVideo");
//        return;
//    }

    if(isCapturing2 && !isDetecting)
    {
        //窗体2显示
//        img = img.scaled(ui->imagebox2->width()-5, ui->imagebox2->height()-5,
//                                           Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        pixmapShow2.setPixmap(QPixmap::fromImage(img));
//        ui->imagebox2->Adapte();
//        QImage2Mat(img, img_inputs[1]);
    }

    if(isCapturing2 && isDetecting)
    {
        requestProcess(img, 1);
    }
}

void MainWindow::setBases(int index)
{
    QString xstr, ystr;
    xstr.sprintf("%d", basePoints[index].x);
    ystr.sprintf("%d", basePoints[index].y);
    ui->lineEditX1->setText(xstr);
    ui->lineEditY1->setText(ystr);
    index++;
    xstr.sprintf("%d",basePoints[index].x);
    ystr.sprintf("%d", basePoints[index].y);
    ui->lineEditX2->setText(xstr);
    ui->lineEditY2->setText(ystr);
}

void MainWindow::on_buttonOpenImgList_clicked()
{
//    if(ui->buttonOpenImgList->text()=="OpenImgList")
//    {
//        QString filename = QFileDialog::getExistingDirectory();
//        QDir *dir=new QDir(filename);
//        QStringList filter;
//        filter<<"*.png"<<"*.jpg"<<"*.jpeg"<<"*.bmp";

//        imgProcess_main->img_filenames =new QList<QFileInfo>(dir->entryInfoList(filter));

//        imgProcess_main->processFilePic();
//        if(imgProcess_main->img_filenames->count()>0)
//            ui->buttonOpenImgList->setText("Next");
//    }
//    else if(ui->buttonOpenImgList->text()=="Next")
//    {
//        if (!imgProcess_main->processFilePic())
//            ui->buttonOpenImgList->setText("OpenImgList");
//    }

    if(ui->buttonOpenImgList->text()=="OpenImgList")
    {
        QStringList filter;
        filter<<"*.png"<<"*.jpg"<<"*.jpeg"<<"*.bmp";
        QString filename = QFileDialog::getExistingDirectory();
        QDir *dir=new QDir(filename);
        imgProcess_main->img_filenames =new QList<QFileInfo>(dir->entryInfoList(filter));
        if(imgProcess_main->img_filenames->count()>0)
            ui->buttonProcess->setEnabled(false);
    }
    else if(ui->buttonOpenImgList->text()=="Next")
    {
        if (imgProcess_main->img_filenames->count()<1)
        {
            img_input1=img_output1.clone();
            ui->buttonOpenImgList->setText("OpenImgList");
            mPlayer1->isCapturing=false;
            mPlayer2->isCapturing=false;
            ui->buttonProcess->setEnabled(true);
            return;
        }
        else
        {
            ui->buttonProcess->setEnabled(false);
        }
    }

    if(imgProcess_main->img_filenames->count()>0)
    {
        isDetecting=true;
        mPlayer1->isCapturing=true;
        QImageReader qir1(imgProcess_main->img_filenames->at(0).filePath());
        QImage img1=qir1.read();

        imgProcess_main->inputFlags[0]=true;
        slotGetOneFrame1(img1);

        if(ui->buttonOpenImgList->text()=="OpenImgList")
            ui->buttonOpenImgList->setText("Next");
        imgProcess_main->img_filenames->removeAt(0);
    }
}

void MainWindow::on_condComboBox_activated(int index)
{
    ui->buttonReset->click();
    workCond = (WorkConditionsEnum)index;
    imgProcess_main->workCond=workCond;
    iniRW = new QSettings("LastSettings.ini",QSettings::IniFormat);
    iniRW->setValue("WorkCondition/WorkCondition",index);
}

