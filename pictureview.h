#ifndef PICTUREVIEWER_H
#define PICTUREVIEWER_H

#include <QWidget>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QtMath>
#include <QContextMenuEvent>
#include <QMenu>
#include <QGraphicsItem>
#include <QDebug>
#include <QGraphicsEffect>
#include <QScrollBar>
#include <QFileDialog>

class PictureView:public QGraphicsView
{
    Q_OBJECT
public:
    explicit PictureView(QWidget *parent = nullptr);
    void Adapte();
public:
    QString img_Name_input;

protected:
    void wheelEvent(QWheelEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    QPointF sceneMousePos;
    double mZoom;

signals:
    void outputImgProperty_request();

private slots:
    void slotAdapte();
    void on_pictureview_OpenImage();
    void on_pictureview_SaveImage();
};

#endif // PICTUREVIEWER_H
