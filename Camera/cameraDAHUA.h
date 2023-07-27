#ifndef CAMERADAHUA_H
#define CAMERADAHUA_H

#include <QObject>
#include <QImage>
#include <QMutex>

#include <DAHUA/avglobal.h>
#include <DAHUA/dhconfigsdk.h>
#include <DAHUA/dhnetsdk.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}
#include <opencv2/opencv.hpp>

#include<iostream>

QT_BEGIN_NAMESPACE
using namespace std;
using namespace cv;
QT_END_NAMESPACE

class cameraDAHUA : public QObject
{
    Q_OBJECT
public:
    explicit cameraDAHUA(QObject *parent = nullptr);
    ~cameraDAHUA();

signals:
    void sig_GetOneFrame(QImage,int); //每获取到一帧图像 就发送此信号
    void sigGetOneFrame1(QImage);
    void sigGetOneFrame2(QImage);
    void sigGetOneFrame3(QImage);
    void sigGetOneFrame4(QImage);

public slots:
    //设备登录
    void cameraLogin(QString IP,int Port,QString UserName,QString PassWord);
    //获取所有挂载相机信息
    void camerasGetConfig();
    //单次抓图
    void cameraSnap(int Channel);
    //实时监视
    void cameraRealPlay(int Channel);
    //停止监视
    void cameraStopPlay(int Channel);
    //接收实时数据
    void ReceiveRealData(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LLONG lParam);
    //接收相机1实时数据
    void ReceiveRealData1(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LLONG lParam);
    //接收相机2实时数据
    void ReceiveRealData2(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LLONG lParam);
    //接收相机3实时数据
    void ReceiveRealData3(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LLONG lParam);
    //接收相机4实时数据
    void ReceiveRealData4(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LLONG lParam);
    //接收所有相机数据
    void ReceiveAllChannels(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LLONG lParam);
    //接收单帧数据
    void ReceiveOneData(BYTE *pBuf, UINT RevLen, UINT EncodeType, DWORD CmdSerial);
    //YUV图片快速转BGR
    bool YV12ToBGR24_FFmpeg(unsigned char* pYUV,unsigned char* pBGR24,int width,int height);
    //FFMPEG码流解码
    void RawToRGB(unsigned char* data, unsigned int dataSize, unsigned char* outBuffer,CFG_VIDEO_COMPRESSION compType=VIDEO_FORMAT_H264);
    //本地抓图（需要实时监视启动）
    void cameraCapture(int Channel);
    //开始4相机采集
    void cameraPlay4();
    //开始所有相机采集
    void cameraPlayAll();
    //所有相机遍历截图
    void cameraSnapAll();

public:
    QString camIP,camUserName,camPassWord;
    int camPort;
    int allCh,curCh,expCh;
    long long camLoginID;
    vector<long long> chanPlayHandle;
    vector<CFG_VIDEO_FORMAT> vdInfo;
    vector<HWND> imgboxHandle;
    vector<BYTE*> pImgBufs;
    bool isCapturing;
    QMutex camMutex;

private:


};

////////////////////////////////回调函数//////////////////////////////////////////
 //掉线通知回调函数
 void CALLBACK DisConnectFunc(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser);
 //掉线重连回调函数
 void CALLBACK ReConnectFunc(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser);
 //实时监视码流获取回调函数
 void CALLBACK RealDataCallBackEx(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,
     DWORD dwBufSize, LONG lParam, LDWORD dwUser);
 //实时监视码流获取回调函数-相机1
 void CALLBACK RealDataCallBackEx1(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,
     DWORD dwBufSize, LONG lParam, LDWORD dwUser);
 //实时监视码流获取回调函数-相机2
 void CALLBACK RealDataCallBackEx2(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,
     DWORD dwBufSize, LONG lParam, LDWORD dwUser);
 //实时监视码流获取回调函数-相机3
 void CALLBACK RealDataCallBackEx3(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,
     DWORD dwBufSize, LONG lParam, LDWORD dwUser);
 //实时监视码流获取回调函数-相机4
 void CALLBACK RealDataCallBackEx4(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,
     DWORD dwBufSize, LONG lParam, LDWORD dwUser);
 //所有相机的回调函数（实验）
 void CALLBACK AllChannelsCallBackEx(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,
     DWORD dwBufSize, LONG lParam, LDWORD dwUser);
 //抓图回调函数
 void CALLBACK SnapPicRet(LLONG ILoginID, BYTE *pBuf, UINT RevLen, UINT EncodeType, DWORD CmdSerial, LDWORD dwUser);

#endif // CAMERADAHUA_H
