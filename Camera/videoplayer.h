#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QThread>
#include <QImage>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include <stdio.h>
#include<iostream>
using namespace std;

//2017.8.10---lizhen
class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;

class VideoPlayer : public QThread
{
    Q_OBJECT

public:
    explicit VideoPlayer();
    ~VideoPlayer();

    void startPlay();

signals:
    void sig_GetOneFrame(QImage); //每获取到一帧图像 就发送此信号
    void sig_GetRFrame(QImage);   ///2017.8.11---lizhen

protected:
    void run();

public:
    bool mainwindowIsStopProcess,isConnected,isCapturing,isDetecting,hasFinished;
    QString videoURL;
    int width,height,bpl;

private:

    //2017.8.10---lizhen
    VlcInstance *_instance;
    VlcMedia *_media;
    VlcMediaPlayer *_player;
};

#endif // VIDEOPLAYER_H
