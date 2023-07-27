#include "pictureview.h"


PictureView::PictureView(QWidget *parent): QGraphicsView(parent)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
//    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    mZoom=1;
}

void PictureView::Adapte()
{
    slotAdapte();
}

void PictureView::wheelEvent(QWheelEvent *event)
{
//    if (event->modifiers() & Qt::ControlModifier)
    if (event->orientation()==Qt::Vertical)
    {
        double zoomFactor = qPow(1.0015, event->angleDelta().y());
        mZoom*=zoomFactor;
        scale(zoomFactor, zoomFactor);
        event->accept();
    } else {
        event->ignore();
//        QGraphicsView::wheelEvent(event);
    }

}

void PictureView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu* menu=new QMenu(this);
    QAction* actionFit=new QAction(tr("Adaption"),this);
    menu->addAction(actionFit);
    connect(actionFit,SIGNAL(triggered()),this,SLOT(slotAdapte()));

    QAction *pOpenImage = new QAction("Open Image",this);
    menu->addAction(pOpenImage);
    connect(pOpenImage,&QAction::triggered,this,&PictureView::on_pictureview_OpenImage);

    QAction *pSaveImage = new QAction("Save Image",this);
    menu->addAction(pSaveImage);
    connect(pSaveImage,&QAction::triggered,this,&PictureView::on_pictureview_SaveImage);

    menu->exec(event->globalPos());
    delete menu;
}

void PictureView::slotAdapte()
{
    double h=rect().height()/sceneRect().height();
    double w=rect().width()/sceneRect().width();

    double zoom = h<w ? h:w;
    zoom*=.99;
    scale(zoom/mZoom, zoom/mZoom);
    mZoom=zoom;

    this->viewport()->update();
}

void PictureView::on_pictureview_OpenImage()
{
    img_Name_input = QFileDialog::getOpenFileName(this,tr("Open Image"),".",
                                                  tr("Image File(*.png *.jpg *.jpeg *.bmp)"));

    emit outputImgProperty_request();
}

void PictureView::on_pictureview_SaveImage()
{
    QString save_file = QFileDialog::getSaveFileName(this,tr("Save Image"),".",
                                                     tr("Image File(*.png *.jpg *.jpeg *.bmp)"));

    if (save_file.isEmpty())
        save_file = "test.png";

    QPixmap crnt_img = this->grab();
    crnt_img.save(save_file);
}
