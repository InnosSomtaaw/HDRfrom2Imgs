#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QtWidgets>

#include "ImageProcess/image_processing.h"
#include "Camera/videoplayer.h"
#include "ImageProcess/bincularstereo.h"
//#include "Camera/cameraDAHUA.h"
//#include "pictureview.h"
//#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow;}
using namespace cv;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool isDetecting,isCapturing1,isCapturing2;
    QElapsedTimer time1;
    double meanFPS1;

private:
    //初始化相机参数设置
    void initCamSettings();
    //初始化图像输入输出
    void initImageInOuts();
    //初始化图像显示盒子
    void initImageBoxes();
    //初始化AI文件路径
    void initDefaultAIPaths();
    //初始化图像处理进程
    void initImageProcess();
    //初始化视频播放器
    void initVideoPlayers();
    //初始化参数设置
    void initBaseSettings();
    //初始化文本输出
    void initTextBrowsers();
    //初始化工况
    void initWorkCondition();

    //请求图像处理
    void requestProcess(QImage img, int i);
    //前方摄像头图像鼠标事件响应函数
    void onMouseFront(int event, int x, int y, int flags);
    //后方摄像头图像鼠标事件响应函数
    void onMouseBack(int event, int x, int y, int flags);
    //前方摄像头图像鼠标事件友元函数
    friend void on_mouse_front(int event, int x, int y, int flags, void* params);
    //后方摄像头图像鼠标事件友元函数
    friend void on_mouse_back(int event, int x, int y, int flags, void* params);
    //设置基准
    void setBases(int index);

    Ui::MainWindow *ui;
    Mat img_input1,img_output1,img_input2,img_output2,img_output3;

    //网络相机参数
    QString nvrIP,cam1IP,camPort,camChannel,camUserName,camPassWord,camCode;
    vector<QString> camIPs, camPorts, camChannels, camUserNames, camPasswords, camCodes, urls;
    int nvrPort;

    //AI工程相关文件路径
    QString proj_path,img_path;
    QByteArray node_name;
    vector<int> ws, hs;
    vector<int> counts;
    int  maxCount;

    int camCount;
    vector<Point2i> basePoints;
    WorkConditionsEnum workCond;
    QSettings *iniRW;

    //图像处理类
    Image_Processing_Class *imgProcess_main;
    bincular3d *bin3d;

    //数字相机类（rtsp读取）
    VideoPlayer *mPlayer1;                  //播放线程
    VideoPlayer *mPlayer2;
    //数字相机类（硬盘录像机）
//    cameraDAHUA *mNVR;

    //图像处理线程
    QThread *m_imgprocsThread;

    //NVR处理线程
//    QThread *mNvrThread;

    QMetaEnum  qSYSTEM_STATUS,qDOWN_PERMIT,qNO_BOX_ALARM,qSLING_ALARM,qTRUCK_CHECK;
    QMetaEnum  qBOX_TYPE,qPLC_STATUS,qLOCKETED_STATUS;

    QGraphicsScene scene1, scene2, scene3;
    QGraphicsPixmapItem pixmapShow1, pixmapShow2, pixmapShow3;
    QString strOutput1;

    //图像处理类信号-槽连接函数
    void connectIPC();
    //数字相机类信号-槽连接函数
    void connectCamera();

private slots:
    //窗体打开图片
    void on_imageboxOpenImage();
    //窗体保存图片
    void on_imageboxSaveImage();
    //选择AI工程和节点按钮槽
    void on_buttonOpenAIProject_clicked();
//    //单次处理按钮槽
//    void on_buttonProcessOnce_clicked();
    //主窗体状态告知槽
    void on_mainwindowStatus_inform();
    //打开视频按钮槽
//    void on_buttonOpenVideo_clicked();
    //重置按钮槽
     void on_buttonReset_clicked();
    //实时处理按钮槽
    void on_buttonProcess_clicked();
    //确认基准按钮槽
    void on_submitBtn_clicked();
    //可编辑状态改变复选框槽
    void on_editCheckBox_stateChanged(int arg1);
    //图片刷新槽
    void on_imagebox_refresh();
    //多图片刷新槽
    void on_imageboxes_refresh();
    //刷新图片保存参数文本框槽
    void on_textSavingCount_textChanged();
    //开始连续采集按钮槽
    void on_buttonStartCapture_clicked();
    //视频流1获取槽
    void slotGetOneFrame1(QImage img);
    //视频流2获取槽
    void slotGetOneFrame2(QImage img);
    //打开图片序列按钮槽
    void on_buttonOpenImgList_clicked();
    //切换处理算法槽
    void on_condComboBox_activated(int index);

signals:
    //开始单次处理信号
    void startProcessOnceRequest();
    //测试AI多工程信号
    void startProcessOnceRequest1();
    void startProcessOnceRequest2();
    //开始图片组处理信号
    void startPicsProcessRequest();
    //开始单相机处理信号
    void start1CamProcessRequest(QImage);
    //开始多相机处理信号，这种结构不好用，接收方lock不到
    void startMulCamProcessRequest(vector<QImage>);
    //开始多相机处理信号
    void startMulCamProcessRequest(QImage, int);
    //测试仅AI信号
    void testAIProcessRequest();
    //开始多相机临时图像保存信号
    void startMulCamTempRequest(QImage, int);
    //处理参数改变信号
    void changeProcParasRequest(QString,int);

};

void on_mouse_front(int event, int x, int y, int flags, void* params);
void on_mouse_back(int event, int x, int y, int flags, void* params);
#endif // MAINWINDOW_H
