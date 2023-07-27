/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>
#include <pictureview.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout;
    QPushButton *buttonStartCapture;
    QPushButton *buttonReset;
    QLabel *label_5;
    QComboBox *condComboBox;
    QPushButton *buttonOpenAIProject;
    QPushButton *buttonOpenImgList;
    QPushButton *buttonProcess;
    QTextEdit *textSavingCount;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEditX1;
    QLabel *label_3;
    QLineEdit *lineEditX2;
    QCheckBox *editCheckBox;
    QLabel *label_2;
    QLineEdit *lineEditY1;
    QLabel *label_4;
    QLineEdit *lineEditY2;
    QPushButton *submitBtn;
    QSpacerItem *horizontalSpacer;
    QGridLayout *gridLayout_2;
    PictureView *imagebox1;
    PictureView *imagebox2;
    PictureView *imagebox3;
    QTextBrowser *textBrowser1;
    QStatusBar *statusbar;
    QMenuBar *menubar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1000, 818);
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        MainWindow->setFont(font);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_3 = new QGridLayout(centralwidget);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        buttonStartCapture = new QPushButton(centralwidget);
        buttonStartCapture->setObjectName(QString::fromUtf8("buttonStartCapture"));

        horizontalLayout->addWidget(buttonStartCapture);

        buttonReset = new QPushButton(centralwidget);
        buttonReset->setObjectName(QString::fromUtf8("buttonReset"));

        horizontalLayout->addWidget(buttonReset);

        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(label_5);

        condComboBox = new QComboBox(centralwidget);
        condComboBox->addItem(QString());
        condComboBox->addItem(QString());
        condComboBox->addItem(QString());
        condComboBox->addItem(QString());
        condComboBox->addItem(QString());
        condComboBox->setObjectName(QString::fromUtf8("condComboBox"));
        condComboBox->setMinimumSize(QSize(125, 0));

        horizontalLayout->addWidget(condComboBox);

        buttonOpenAIProject = new QPushButton(centralwidget);
        buttonOpenAIProject->setObjectName(QString::fromUtf8("buttonOpenAIProject"));

        horizontalLayout->addWidget(buttonOpenAIProject);

        buttonOpenImgList = new QPushButton(centralwidget);
        buttonOpenImgList->setObjectName(QString::fromUtf8("buttonOpenImgList"));

        horizontalLayout->addWidget(buttonOpenImgList);

        buttonProcess = new QPushButton(centralwidget);
        buttonProcess->setObjectName(QString::fromUtf8("buttonProcess"));
        buttonProcess->setEnabled(true);

        horizontalLayout->addWidget(buttonProcess);

        textSavingCount = new QTextEdit(centralwidget);
        textSavingCount->setObjectName(QString::fromUtf8("textSavingCount"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(textSavingCount->sizePolicy().hasHeightForWidth());
        textSavingCount->setSizePolicy(sizePolicy1);
        textSavingCount->setMaximumSize(QSize(16777215, 25));
        textSavingCount->setAutoFillBackground(false);
        textSavingCount->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        horizontalLayout->addWidget(textSavingCount);


        gridLayout_3->addLayout(horizontalLayout, 0, 0, 1, 2);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEditX1 = new QLineEdit(centralwidget);
        lineEditX1->setObjectName(QString::fromUtf8("lineEditX1"));
        lineEditX1->setEnabled(false);

        gridLayout->addWidget(lineEditX1, 0, 1, 1, 1);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 0, 2, 1, 1);

        lineEditX2 = new QLineEdit(centralwidget);
        lineEditX2->setObjectName(QString::fromUtf8("lineEditX2"));
        lineEditX2->setEnabled(false);

        gridLayout->addWidget(lineEditX2, 0, 3, 1, 1);

        editCheckBox = new QCheckBox(centralwidget);
        editCheckBox->setObjectName(QString::fromUtf8("editCheckBox"));
        editCheckBox->setEnabled(false);
        editCheckBox->setCheckable(true);
        editCheckBox->setChecked(false);

        gridLayout->addWidget(editCheckBox, 0, 4, 1, 1);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        lineEditY1 = new QLineEdit(centralwidget);
        lineEditY1->setObjectName(QString::fromUtf8("lineEditY1"));
        lineEditY1->setEnabled(false);

        gridLayout->addWidget(lineEditY1, 1, 1, 1, 1);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 1, 2, 1, 1);

        lineEditY2 = new QLineEdit(centralwidget);
        lineEditY2->setObjectName(QString::fromUtf8("lineEditY2"));
        lineEditY2->setEnabled(false);

        gridLayout->addWidget(lineEditY2, 1, 3, 1, 1);

        submitBtn = new QPushButton(centralwidget);
        submitBtn->setObjectName(QString::fromUtf8("submitBtn"));
        submitBtn->setEnabled(false);

        gridLayout->addWidget(submitBtn, 1, 4, 1, 1);


        gridLayout_3->addLayout(gridLayout, 1, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(403, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer, 1, 1, 1, 1);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        imagebox1 = new PictureView(centralwidget);
        imagebox1->setObjectName(QString::fromUtf8("imagebox1"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(imagebox1->sizePolicy().hasHeightForWidth());
        imagebox1->setSizePolicy(sizePolicy2);
        imagebox1->setMinimumSize(QSize(400, 300));
        imagebox1->setContextMenuPolicy(Qt::DefaultContextMenu);
        imagebox1->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        imagebox1->setDragMode(QGraphicsView::ScrollHandDrag);
        imagebox1->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        imagebox1->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        imagebox1->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
        imagebox1->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);

        gridLayout_2->addWidget(imagebox1, 0, 0, 1, 1);

        imagebox2 = new PictureView(centralwidget);
        imagebox2->setObjectName(QString::fromUtf8("imagebox2"));
        sizePolicy2.setHeightForWidth(imagebox2->sizePolicy().hasHeightForWidth());
        imagebox2->setSizePolicy(sizePolicy2);
        imagebox2->setMinimumSize(QSize(400, 300));
        imagebox2->setContextMenuPolicy(Qt::DefaultContextMenu);
        imagebox2->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        imagebox2->setDragMode(QGraphicsView::ScrollHandDrag);
        imagebox2->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        imagebox2->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        imagebox2->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
        imagebox2->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);

        gridLayout_2->addWidget(imagebox2, 0, 1, 1, 1);

        imagebox3 = new PictureView(centralwidget);
        imagebox3->setObjectName(QString::fromUtf8("imagebox3"));
        sizePolicy2.setHeightForWidth(imagebox3->sizePolicy().hasHeightForWidth());
        imagebox3->setSizePolicy(sizePolicy2);
        imagebox3->setMinimumSize(QSize(400, 300));
        imagebox3->setContextMenuPolicy(Qt::DefaultContextMenu);
        imagebox3->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        imagebox3->setDragMode(QGraphicsView::ScrollHandDrag);
        imagebox3->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        imagebox3->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        imagebox3->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
        imagebox3->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);

        gridLayout_2->addWidget(imagebox3, 1, 0, 1, 1);

        textBrowser1 = new QTextBrowser(centralwidget);
        textBrowser1->setObjectName(QString::fromUtf8("textBrowser1"));
        sizePolicy2.setHeightForWidth(textBrowser1->sizePolicy().hasHeightForWidth());
        textBrowser1->setSizePolicy(sizePolicy2);
        textBrowser1->setMinimumSize(QSize(400, 300));

        gridLayout_2->addWidget(textBrowser1, 1, 1, 1, 1);


        gridLayout_3->addLayout(gridLayout_2, 2, 0, 1, 2);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1000, 27));
        MainWindow->setMenuBar(menubar);

        retranslateUi(MainWindow);

        buttonStartCapture->setDefault(false);
        buttonReset->setDefault(false);
        buttonOpenAIProject->setDefault(false);
        buttonOpenImgList->setDefault(false);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "CV_Test", nullptr));
        buttonStartCapture->setText(QCoreApplication::translate("MainWindow", "StartCapture", nullptr));
        buttonReset->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "ProcessMethold", nullptr));
        condComboBox->setItemText(0, QCoreApplication::translate("MainWindow", "HDRfrom2Img", nullptr));
        condComboBox->setItemText(1, QCoreApplication::translate("MainWindow", "HDRfrom2ImgGPU", nullptr));
        condComboBox->setItemText(2, QCoreApplication::translate("MainWindow", "HDRfrom1img", nullptr));
        condComboBox->setItemText(3, QCoreApplication::translate("MainWindow", "HDRfrom1imgGPU", nullptr));
        condComboBox->setItemText(4, QCoreApplication::translate("MainWindow", "ResizeShow", nullptr));

        buttonOpenAIProject->setText(QCoreApplication::translate("MainWindow", "OpenAIProj", nullptr));
        buttonOpenImgList->setText(QCoreApplication::translate("MainWindow", "OpenImgList", nullptr));
        buttonProcess->setText(QCoreApplication::translate("MainWindow", "Process", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "frontX(pixel)", nullptr));
        lineEditX1->setText(QCoreApplication::translate("MainWindow", "480", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "backX(pixel)", nullptr));
        lineEditX2->setText(QCoreApplication::translate("MainWindow", "480", nullptr));
        editCheckBox->setText(QCoreApplication::translate("MainWindow", "\345\217\257\347\274\226\350\276\221", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "frontY(pixel)", nullptr));
        lineEditY1->setText(QCoreApplication::translate("MainWindow", "288", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "backY(pixel)", nullptr));
        lineEditY2->setText(QCoreApplication::translate("MainWindow", "288", nullptr));
        submitBtn->setText(QCoreApplication::translate("MainWindow", "\347\241\256\350\256\244\345\237\272\345\207\206\345\235\220\346\240\207", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
