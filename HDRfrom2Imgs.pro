QT       += core gui network xml
qtHaveModule(printsupport): QT += printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Camera/videoplayer.cpp \
    ImageProcess/bincularstereo.cpp \
    ImageProcess/cv_stereomatcher.cpp \
    ImageProcess/image_processing.cpp \
    ImageProcess/imghdr.cpp \
    RadarDetection/comm_param.cpp \
    main.cpp \
    mainwindow.cpp \
    pictureview.cpp

HEADERS += \
    Camera/videoplayer.h \
    ImageProcess/bincularstereo.h \
    ImageProcess/cv_stereomatcher.h \
    ImageProcess/image_processing.h \
    ImageProcess/imghdr.h \
    RadarDetection/comm_param.h \
    mainwindow.h \
    pictureview.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QMAKE_LFLAGS += -no-pie

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += opencv4 \
                   libavcodec\
                   libavformat \
                   libavutil \
                   libswresample \
                   libswscale

#unix:!macx: LIBS += -lavcodec \
#                    -lavformat \
#                    -lavutil \
#                    -lswresample \
#                    -lswscale \
