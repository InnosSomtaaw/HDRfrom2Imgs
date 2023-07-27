#include "cameraDAHUA.h"

cameraDAHUA::cameraDAHUA(QObject *parent) : QObject(parent)
{
    isCapturing = false;curCh=0;expCh=0;
    BOOL ret = CLIENT_Init(DisConnectFunc, (LDWORD)this);
    if(ret)
    {
        CLIENT_SetSnapRevCallBack(SnapPicRet,(LDWORD)this);
        CLIENT_SetAutoReconnect(ReConnectFunc,(LDWORD)this);
    }
}

cameraDAHUA::~cameraDAHUA()
{
//    bool fg=CLIENT_ControlDeviceEx(camLoginID,DH_CTRL_SHUTDOWN,NULL,NULL,3000);
//    if (!fg)
//    {
//        int nErrorNum = CLIENT_GetLastError()&(0x7fffffff);
//        cout<<"CLIENT_ControlDeviceEx failed! Last Error code: "<<nErrorNum<<endl;
//    }
    if(camLoginID!=0)
        CLIENT_Logout(camLoginID);
    CLIENT_Cleanup();
}

void cameraDAHUA::cameraLogin(QString IP, int Port, QString UserName, QString PassWord)
{
    camIP = IP;
    camPort = Port;
    camUserName = UserName;
    camPassWord = PassWord;

    NET_DEVICEINFO deviceInfo ={0};
    NET_IN_LOGIN_WITH_HIGHLEVEL_SECURITY stInparam;
    memset(&stInparam, 0, sizeof(stInparam));
    stInparam.dwSize = sizeof(stInparam);

    QByteArray baIP = IP.toUtf8();
    const char *strIP=baIP.data();
    QByteArray baUN = UserName.toUtf8();
    const char *strUN=baUN.data();
    QByteArray baPW = PassWord.toUtf8();
    const char *strPW=baPW.data();

    strncpy(stInparam.szIP, strIP, sizeof(stInparam.szIP) - 1);
    strncpy(stInparam.szUserName, strUN, sizeof(stInparam.szUserName) - 1);
    strncpy(stInparam.szPassword, strPW, sizeof(stInparam.szPassword) - 1);
    stInparam.nPort = Port;
    stInparam.emSpecCap = EM_LOGIN_SPEC_CAP_TCP;
    NET_OUT_LOGIN_WITH_HIGHLEVEL_SECURITY stOutparam;
    memset(&stOutparam, 0, sizeof(stOutparam));
    stOutparam.dwSize = sizeof(stOutparam);

    camLoginID = CLIENT_LoginWithHighLevelSecurity(&stInparam, &stOutparam);
    if(camLoginID == 0)
    {
        cout<<"Login device failed"<<endl;
    }
    else
    {
//        allCh = stOutparam.stuDeviceInfo.nChanNum;
        allCh = 44;
        chanPlayHandle = vector<long long>(allCh);
        vdInfo = vector<CFG_VIDEO_FORMAT>(allCh);
        pImgBufs = vector<BYTE*>(allCh);
        imgboxHandle = vector<HWND>(allCh);
        cout<<"Network fault 1 minute if no recovery, the SDK will think break line, please be patient"<< endl;
        cout<<"Login equipment success, press any key exit procedure"<<endl;  
    }
}

void cameraDAHUA::camerasGetConfig()
{
    CFG_ENCODE_INFO *pEncodeInfo = new CFG_ENCODE_INFO();
    char *szBuffer = new char[512*1024];
    QString qCmd = "Encode";
    QByteArray baCmd = qCmd.toUtf8();
    char *szCmd = baCmd.data();
    int nerror = 0;
    for(int ch=0;ch<allCh;ch++)
    {
        BOOL bRet = CLIENT_GetNewDevConfig(camLoginID,szCmd,ch,szBuffer,512*1024,&nerror,3000);
        if (!bRet)
        {
            int nErrorNum = CLIENT_GetLastError()&(0x7fffffff);
            cout<<"Camera "<<ch<<" CLIENT_GetNewDevConfig failed! Last Error code: "<<nErrorNum<<endl;
        }
        else
        {
            int nRetLen = 0;
            bRet = CLIENT_ParseData(szCmd, szBuffer, (char *)pEncodeInfo, sizeof(CFG_ENCODE_INFO), &nRetLen);
            if (!bRet)
            {
                int nErrorNum = CLIENT_GetLastError()&(0x7fffffff);
                cout<<"CLIENT_ParseData failed! Last Error code: "<<nErrorNum<<endl;
            }
            else
            {
                vdInfo[ch] = pEncodeInfo->stuMainStream[0].stuVideoFormat;
            }
        }
    }
    delete  pEncodeInfo;
}

// 网络抓图
void cameraDAHUA::cameraSnap(int Channel)
{
    if(!camMutex.tryLock())
        return;

    NET_IN_MANUAL_SNAP stuInParam = {sizeof(stuInParam)};
    NET_OUT_MANUAL_SNAP stuOutParam = {sizeof(stuOutParam)};
    stuInParam.nChannel = Channel;
//    QString FN = "test"+QString::number(Channel)+".jpg";
//    QByteArray baFN = FN.toUtf8();
//    const char *strFN=baFN.data();
//    strncpy(stuInParam.szFilePath, strFN, sizeof(stuInParam.szFilePath) - 1);
    long nBufferLen = 3*2048*2048;
    char* pBuffer = new char[nBufferLen]; // 图片缓存
    memset(pBuffer, 0, nBufferLen);
    stuOutParam.pRcvBuf = pBuffer;
    stuOutParam.nMaxBufLen = nBufferLen;
    if (!CLIENT_ManualSnap(camLoginID,&stuInParam,&stuOutParam,1000))
    {
        int nErrorNum = CLIENT_GetLastError()&(0x7fffffff);
        cout<<"CLIENT_SnapPictureEx Failed!Last Error code: "<<nErrorNum<<endl;
        return;
    }
    Mat rgbImg(vdInfo[curCh].nHeight, vdInfo[curCh].nWidth,CV_8UC3);
    RawToRGB((BYTE*)stuOutParam.pRcvBuf,stuOutParam.nRetBufLen,rgbImg.data,VIDEO_FORMAT_MJPG);
//    imshow("test",rgbImg);
//    waitKey(0);
    QImage tmpImg = QImage(rgbImg.data,rgbImg.cols,rgbImg.rows,rgbImg.cols*rgbImg.channels(),
                           QImage::Format_RGB888);
    QImage image = tmpImg.copy();
    curCh = Channel;
    emit sig_GetOneFrame(image,curCh);
    camMutex.unlock();
    rgbImg.release();

//    // 网络抓图示例
//    NET_IN_SNAP_PIC_TO_FILE_PARAM stuInParam = {sizeof(stuInParam)};
//    NET_OUT_SNAP_PIC_TO_FILE_PARAM stuOutParam = {sizeof(stuOutParam)};
//    SNAP_PARAMS stuSnapParams = {0};
//    stuSnapParams.Channel = Channel;
//    stuInParam.stuParam = stuSnapParams;
//    int nBufferLen = 2*1024*1024;
//    char* pBuffer = new char[nBufferLen]; // 图片缓存
//    memset(pBuffer, 0, nBufferLen);
//    stuOutParam.szPicBuf = pBuffer;
//    stuOutParam.dwPicBufLen = nBufferLen;
//    if (FALSE == CLIENT_SnapPictureToFile(camLoginID,&stuInParam,&stuOutParam,3000))
//    {
//        int nErrorNum = CLIENT_GetLastError()&(0x7fffffff);
//        cout<<"CLIENT_SnapPictureToFile Failed!Last Error code: "<<nErrorNum<<endl;
//    }
//    Mat rgbImg(vdInfo[curCh].nHeight, vdInfo[curCh].nWidth,CV_8UC3);
//    RawToRGB((BYTE*)stuOutParam.szPicBuf,stuOutParam.dwPicBufRetLen,rgbImg.data,VIDEO_FORMAT_MJPG);
////    imshow("test",rgbImg);
////    waitKey(0);
//    QImage tmpImg = QImage(rgbImg.data,rgbImg.cols,rgbImg.rows,
//                           QImage::Format_RGB888);
//    QImage image = tmpImg.copy();
//    emit sig_GetOneFrame(image);
//    rgbImg.release();
}

void cameraDAHUA::cameraRealPlay(int Channel)
{
    chanPlayHandle[Channel] = CLIENT_RealPlayEx(camLoginID,Channel, imgboxHandle[Channel+1], DH_RType_Realplay);
    if (NULL == chanPlayHandle[Channel])
    {
        int nErrorNum = CLIENT_GetLastError()&(0x7fffffff);
        cout<<"CLIENT_RealPlayEx: failed! Error code: "<<nErrorNum<<endl;
    }
    else
    {
        curCh = Channel;
        DWORD dwFlag = REALDATA_FLAG_RAW_DATA; //原始数据标志
        dwFlag = REALDATA_FLAG_YUV_DATA;//YUV数据标志
        CLIENT_SetRealDataCallBackEx(chanPlayHandle[Channel], RealDataCallBackEx, (LDWORD)this, dwFlag);
    }
}

// 关闭预览
void cameraDAHUA::cameraStopPlay(int Channel)
{
    if (0 != chanPlayHandle[Channel])
        CLIENT_StopRealPlayEx(chanPlayHandle[Channel]);
}

void cameraDAHUA::ReceiveRealData(long long lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, long long lParam)
{
    Mat rgbImg(vdInfo[curCh].nHeight, vdInfo[curCh].nWidth,CV_8UC3);
    switch (dwDataType) {
    case 0:
        RawToRGB(pBuffer,dwBufSize,rgbImg.data,vdInfo[curCh].emCompression);
        break;
    case 2:
        YV12ToBGR24_FFmpeg(pBuffer,rgbImg.data,vdInfo[curCh].nWidth,vdInfo[curCh].nHeight);
        break;
    }

//    imshow("test",rgbImg);
//    waitKey(0);

    QImage tmpImg = QImage(rgbImg.data,rgbImg.cols,rgbImg.rows,
                             QImage::Format_RGB888);
    QImage image = tmpImg.copy();
    emit sig_GetOneFrame(image,curCh);

    rgbImg.release();
}

void cameraDAHUA::ReceiveRealData1(long long lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, long long lParam)
{
    size_t ch=40;
    Mat rgbImg(vdInfo[ch].nHeight, vdInfo[ch].nWidth,CV_8UC3);
    switch (dwDataType) {
    case 0:
        RawToRGB(pBuffer,dwBufSize,rgbImg.data,vdInfo[ch].emCompression);
        break;
    case 2:
        YV12ToBGR24_FFmpeg(pBuffer,rgbImg.data,vdInfo[ch].nWidth,vdInfo[ch].nHeight);
        break;
    }
    QImage tmpImg = QImage(rgbImg.data,rgbImg.cols,rgbImg.rows,
                             QImage::Format_RGB888);
    QImage image = tmpImg.copy();
    emit sigGetOneFrame4(image);
    rgbImg.release();
    Sleep(10);
}

void cameraDAHUA::ReceiveRealData2(long long lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, long long lParam)
{
    size_t ch=41;
    Mat rgbImg(vdInfo[ch].nHeight, vdInfo[ch].nWidth,CV_8UC3);
    switch (dwDataType) {
    case 0:
        RawToRGB(pBuffer,dwBufSize,rgbImg.data,vdInfo[ch].emCompression);
        break;
    case 2:
        YV12ToBGR24_FFmpeg(pBuffer,rgbImg.data,vdInfo[ch].nWidth,vdInfo[ch].nHeight);
        break;
    }
    QImage tmpImg = QImage(rgbImg.data,rgbImg.cols,rgbImg.rows,
                             QImage::Format_RGB888);
    QImage image = tmpImg.copy();
    emit sigGetOneFrame2(image);
    rgbImg.release();
    Sleep(10);
}

void cameraDAHUA::ReceiveRealData3(long long lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, long long lParam)
{
    size_t ch=42;
    Mat rgbImg(vdInfo[ch].nHeight, vdInfo[ch].nWidth,CV_8UC3);
    switch (dwDataType) {
    case 0:
        RawToRGB(pBuffer,dwBufSize,rgbImg.data,vdInfo[ch].emCompression);
        break;
    case 2:
        YV12ToBGR24_FFmpeg(pBuffer,rgbImg.data,vdInfo[ch].nWidth,vdInfo[ch].nHeight);
        break;
    }
    QImage tmpImg = QImage(rgbImg.data,rgbImg.cols,rgbImg.rows,
                             QImage::Format_RGB888);
    QImage image = tmpImg.copy();
    emit sigGetOneFrame3(image);
    rgbImg.release();
    Sleep(10);
}

void cameraDAHUA::ReceiveRealData4(long long lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, long long lParam)
{
    size_t ch=43;
    Mat rgbImg(vdInfo[ch].nHeight, vdInfo[ch].nWidth,CV_8UC3);
    switch (dwDataType) {
    case 0:
        RawToRGB(pBuffer,dwBufSize,rgbImg.data,vdInfo[ch].emCompression);
        break;
    case 2:
        YV12ToBGR24_FFmpeg(pBuffer,rgbImg.data,vdInfo[ch].nWidth,vdInfo[ch].nHeight);
        break;
    }
    QImage tmpImg = QImage(rgbImg.data,rgbImg.cols,rgbImg.rows,
                             QImage::Format_RGB888);
    QImage image = tmpImg.copy();
    emit sigGetOneFrame1(image);
    rgbImg.release();
    Sleep(10);
}

void cameraDAHUA::ReceiveAllChannels(long long lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, long long lParam)
{
    curCh=0;
    for(int i=0;i<allCh;i++)
    {
        if(lRealHandle==chanPlayHandle[i])
        {
            curCh=i;
            break;
        }
    }

    if(expCh>0 && (expCh-1)!=curCh)
        return;

//    if(!camMutex.tryLock())
//        return;

    Mat rgbImg(vdInfo[curCh].nHeight, vdInfo[curCh].nWidth,CV_8UC3);
    switch (dwDataType) {
    case 0:
        RawToRGB(pBuffer,dwBufSize,rgbImg.data,vdInfo[curCh].emCompression);
        break;
    case 2:
        YV12ToBGR24_FFmpeg(pBuffer,rgbImg.data,vdInfo[curCh].nWidth,vdInfo[curCh].nHeight);
        break;
    }

//    imshow("test",rgbImg);
//    waitKey(0);

    if(expCh==0)
        Sleep(200);
    QImage tmpImg = QImage(rgbImg.data,rgbImg.cols,rgbImg.rows,
                             QImage::Format_RGB888);
    QImage image = tmpImg.copy();
    emit sig_GetOneFrame(image,curCh);
//    camMutex.unlock();
//    rgbImg.release();

}

void cameraDAHUA::ReceiveOneData(BYTE *pBuf, UINT RevLen, UINT EncodeType, DWORD CmdSerial)
{
    if(!camMutex.tryLock())
        return;
    if(RevLen<1000)
    {
        Sleep(100);
        //Fill in request structure
        SNAP_PARAMS snapparams = {0};
        snapparams.Channel = CmdSerial;
        snapparams.Quality = 6;
        snapparams.mode = 0;
        snapparams.CmdSerial = CmdSerial;
        //Send out request.
        CLIENT_SnapPictureEx(camLoginID, &snapparams);
        return;
    }
    curCh=CmdSerial;
    Mat rgbImg(vdInfo[curCh].nHeight, vdInfo[curCh].nWidth,CV_8UC3);
    RawToRGB(pBuf,RevLen,rgbImg.data,VIDEO_FORMAT_MJPG);
    QImage tmpImg = QImage(rgbImg.data,rgbImg.cols,rgbImg.rows,rgbImg.cols*rgbImg.channels(),
                           QImage::Format_RGB888);
    QImage image = tmpImg.copy();
    emit sig_GetOneFrame(image,CmdSerial);
    rgbImg.release();
    if(curCh<22)
        curCh=curCh+2;
    camMutex.unlock();
}

//YUV转RGB
bool cameraDAHUA::YV12ToBGR24_FFmpeg(unsigned char* pYUV,unsigned char* pBGR24,int width,int height)
{
    if (width < 1 || height < 1 || pYUV == NULL || pBGR24 == NULL)
        return false;
    //int srcNumBytes,dstNumBytes;
    //uint8_t *pSrc,*pDst;
    AVPicture pFrameYUV,pFrameBGR;

    //pFrameYUV = avpicture_alloc();
    //srcNumBytes = avpicture_get_size(PIX_FMT_YUV420P,width,height);
    //pSrc = (uint8_t *)malloc(sizeof(uint8_t) * srcNumBytes);
     avpicture_fill(&pFrameYUV,pYUV,AV_PIX_FMT_YUV420P,width,height);

    //U,V互换
    uint8_t * ptmp=pFrameYUV.data[1];
    pFrameYUV.data[1]=pFrameYUV.data[2];
    pFrameYUV.data[2]=ptmp;

    //pFrameBGR = avcodec_alloc_frame();
    //dstNumBytes = avpicture_get_size(PIX_FMT_BGR24,width,height);
    //pDst = (uint8_t *)malloc(sizeof(uint8_t) * dstNumBytes);
    avpicture_fill(&pFrameBGR,pBGR24,AV_PIX_FMT_BGR24,width,height);

    struct SwsContext* imgCtx = NULL;
    imgCtx = sws_getContext(width,height,AV_PIX_FMT_YUV420P,width,height,AV_PIX_FMT_BGR24,SWS_BILINEAR,0,0,0);

    if (imgCtx != NULL){
        sws_scale(imgCtx,pFrameYUV.data,pFrameYUV.linesize,0,height,pFrameBGR.data,pFrameBGR.linesize);
        if(imgCtx){
            sws_freeContext(imgCtx);
            imgCtx = NULL;
        }
        return true;
    }
    else{
        sws_freeContext(imgCtx);
        imgCtx = NULL;
        return false;
    }
}

//FFMPEG各种转RGB
void cameraDAHUA::RawToRGB(unsigned char *data, unsigned int dataSize, unsigned char *outBuffer, CFG_VIDEO_COMPRESSION compType)
{
    // 1. 将元数据装填到packet
        AVPacket* avPkt = av_packet_alloc();
        avPkt->size = dataSize;
        avPkt->data = data;

        static AVCodecContext* codecCtx = nullptr;
        AVCodec* avCodec;
        if (codecCtx == nullptr) {
            // 2. 创建并配置codecContext
            switch(compType){
            case VIDEO_FORMAT_H264:
                avCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
                break;
            case VIDEO_FORMAT_H265:
                avCodec = avcodec_find_decoder(AV_CODEC_ID_H265);
                break;
            case VIDEO_FORMAT_MJPG:
                avCodec = avcodec_find_decoder(AV_CODEC_ID_MJPEG);
                break;
            }

            codecCtx = avcodec_alloc_context3(avCodec);
            avcodec_get_context_defaults3(codecCtx, avCodec);
            avcodec_open2(codecCtx, avCodec, nullptr);
        }

        // 3. 解码
        //avcodec_decode_video2(codecCtx, &outFrame, &lineLength, &avPkt);  // 接口被弃用，使用下边接口代替
        auto ret = avcodec_send_packet(codecCtx, avPkt);
        if (ret >= 0) {
            AVFrame* YUVFrame = av_frame_alloc();
            ret = avcodec_receive_frame(codecCtx, YUVFrame);
            if (ret >= 0) {

                // 4.YUV转RGB24
                AVFrame* RGB24Frame = av_frame_alloc();
                struct SwsContext* convertCxt = sws_getContext(
                    YUVFrame->width, YUVFrame->height, AV_PIX_FMT_YUV420P,
                    YUVFrame->width, YUVFrame->height, AV_PIX_FMT_RGB24,
                    SWS_POINT, NULL, NULL, NULL
                );

                // outBuffer将会分配给RGB24Frame->data,AV_PIX_FMT_RGB24格式只分配到RGB24Frame->data[0]
                av_image_fill_arrays(
                    RGB24Frame->data, RGB24Frame->linesize, outBuffer,
                    AV_PIX_FMT_RGB24, YUVFrame->width, YUVFrame->height,
                    1
                );
                sws_scale(convertCxt, YUVFrame->data, YUVFrame->linesize, 0, YUVFrame->height, RGB24Frame->data, RGB24Frame->linesize);

                // 5.清除各对象/context -> 释放内存
                // free context and avFrame
                sws_freeContext(convertCxt);
                av_frame_free(&RGB24Frame);
                // RGB24Frame.
            }
            // free context and avFrame
            av_frame_free(&YUVFrame);
        }
        // free context and avFrame
        av_packet_unref(avPkt);
        av_packet_free(&avPkt);
        // avcodec_free_context(&codecCtx);
}

// 本地抓图示例， hPlayHandle 为打开监视获取的句柄
void cameraDAHUA::cameraCapture(int Channel)
{
    if (FALSE == CLIENT_CapturePictureEx(chanPlayHandle[Channel],"cam4.bmp", NET_CAPTURE_BMP))
    {
        int nErrorNum = CLIENT_GetLastError()&(0x7fffffff);
        cout<<"CLIENT_CapturePictureEx: failed! Error code: "<<nErrorNum<<endl;
    }
}

void cameraDAHUA::cameraPlay4()
{
    DWORD dwFlag = REALDATA_FLAG_YUV_DATA;
    chanPlayHandle[40] = CLIENT_RealPlayEx(camLoginID,40, imgboxHandle[40], DH_RType_Realplay);
    bool flg1 = CLIENT_SetRealDataCallBackEx(chanPlayHandle[40], RealDataCallBackEx1, (LDWORD)this, dwFlag);
    chanPlayHandle[41] = CLIENT_RealPlayEx(camLoginID,41, imgboxHandle[41], DH_RType_Realplay);
    bool flg2 = CLIENT_SetRealDataCallBackEx(chanPlayHandle[41], RealDataCallBackEx2, (LDWORD)this, dwFlag);
    chanPlayHandle[42] = CLIENT_RealPlayEx(camLoginID,42, imgboxHandle[42], DH_RType_Realplay);
    bool flg3 = CLIENT_SetRealDataCallBackEx(chanPlayHandle[42], RealDataCallBackEx3, (LDWORD)this, dwFlag);
    chanPlayHandle[43] = CLIENT_RealPlayEx(camLoginID,43, imgboxHandle[43], DH_RType_Realplay);
    bool flg4 = CLIENT_SetRealDataCallBackEx(chanPlayHandle[43], RealDataCallBackEx4, (LDWORD)this, dwFlag);
}

void cameraDAHUA::cameraPlayAll()
{
    DWORD dwFlag = REALDATA_FLAG_YUV_DATA;
    for(size_t ch=0;ch<24;ch=ch+2)
    {
        chanPlayHandle[ch] = CLIENT_RealPlayEx(camLoginID,ch, imgboxHandle[ch], DH_RType_Realplay);
        bool flg = CLIENT_SetRealDataCallBackEx(chanPlayHandle[ch], AllChannelsCallBackEx, (LDWORD)this, dwFlag);
    }
}

void cameraDAHUA::cameraSnapAll()
{
    int ch=0;
    int stuckCount=0;
    do
    {
        Sleep(100);
        if(curCh<ch && stuckCount<100)
        {
            stuckCount++;
            continue;
        }
        stuckCount=0;
        //Fill in request structure
        SNAP_PARAMS snapparams = {0};
        snapparams.Channel = ch;
        snapparams.Quality = 6;
        snapparams.mode = 0;
        snapparams.CmdSerial = ch;
        //Send out request.
        BOOL b = CLIENT_SnapPictureEx(camLoginID, &snapparams);
        if (!b)
        {
            int nErrorNum = CLIENT_GetLastError()&(0x7fffffff);
            cout<<"CLIENT_SnapPicture: failed! Error code: "<<nErrorNum<<endl;
//            camMutex.unlock();
            continue;
        }
        if(ch<22)
            ch=ch+2;
        else
            ch=0;
//        camMutex.unlock();
    }while(isCapturing);
}

//断线回调函数，通知用户当前断开网络的设备
void CALLBACK DisConnectFunc(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort,LDWORD dwUser)
{
    cout<<lLoginID<<" Disconenct."<<endl;
}

//断线重连成功的回调函数，通知用户当前断线重连成功的设备
void CALLBACK ReConnectFunc(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser)
{
    cout<<lLoginID<<" Reconnect success."<<endl;
}

void CALLBACK RealDataCallBackEx(long long lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG lParam, long long dwUser)
{
    if(dwUser == 0)
        return;

    cameraDAHUA *cam = (cameraDAHUA *)dwUser;
    cam->ReceiveRealData(lRealHandle,dwDataType, pBuffer, dwBufSize, lParam);
}

void CALLBACK SnapPicRet(long long ILoginID, BYTE *pBuf, UINT RevLen, UINT EncodeType, DWORD CmdSerial, long long dwUser)
{
//    if(dwUser == 0)
//        return;

    cameraDAHUA *cam = (cameraDAHUA *)dwUser;
    cam->ReceiveOneData(pBuf,RevLen,EncodeType,CmdSerial);
}

void CALLBACK AllChannelsCallBackEx(long long lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG lParam, long long dwUser)
{
    if(dwUser == 0)
        return;

    cameraDAHUA *cam = (cameraDAHUA *)dwUser;
    cam->ReceiveAllChannels(lRealHandle,dwDataType, pBuffer, dwBufSize, lParam);
}

void RealDataCallBackEx1(long long lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG lParam, long long dwUser)
{
    if(dwUser == 0)
        return;
    cameraDAHUA *cam = (cameraDAHUA *)dwUser;
    cam->ReceiveRealData1(lRealHandle,dwDataType, pBuffer, dwBufSize, lParam);
}

void RealDataCallBackEx2(long long lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG lParam, long long dwUser)
{
    if(dwUser == 0)
        return;
    cameraDAHUA *cam = (cameraDAHUA *)dwUser;
    cam->ReceiveRealData2(lRealHandle,dwDataType, pBuffer, dwBufSize, lParam);
}

void RealDataCallBackEx3(long long lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG lParam, long long dwUser)
{
    if(dwUser == 0)
        return;
    cameraDAHUA *cam = (cameraDAHUA *)dwUser;
    cam->ReceiveRealData3(lRealHandle,dwDataType, pBuffer, dwBufSize, lParam);
}

void RealDataCallBackEx4(long long lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LONG lParam, long long dwUser)
{
    if(dwUser == 0)
        return;
    cameraDAHUA *cam = (cameraDAHUA *)dwUser;
    cam->ReceiveRealData4(lRealHandle,dwDataType, pBuffer, dwBufSize, lParam);
}
