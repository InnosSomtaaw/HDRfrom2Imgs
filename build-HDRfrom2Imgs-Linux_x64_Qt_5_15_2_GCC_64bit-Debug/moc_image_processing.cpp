/****************************************************************************
** Meta object code from reading C++ file 'image_processing.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../ImageProcess/image_processing.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'image_processing.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Image_Processing_Class_t {
    QByteArrayData data[17];
    char stringdata0[248];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Image_Processing_Class_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Image_Processing_Class_t qt_meta_stringdata_Image_Processing_Class = {
    {
QT_MOC_LITERAL(0, 0, 22), // "Image_Processing_Class"
QT_MOC_LITERAL(1, 23, 25), // "outputImgProcessedRequest"
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 21), // "outputMulImgAIRequest"
QT_MOC_LITERAL(4, 72, 23), // "mainwindowStatusRequest"
QT_MOC_LITERAL(5, 96, 16), // "startProcessOnce"
QT_MOC_LITERAL(6, 113, 16), // "startPicsProcess"
QT_MOC_LITERAL(7, 130, 15), // "startMulCamTemp"
QT_MOC_LITERAL(8, 146, 7), // "recvImg"
QT_MOC_LITERAL(9, 154, 1), // "i"
QT_MOC_LITERAL(10, 156, 16), // "start1CamProcess"
QT_MOC_LITERAL(11, 173, 11), // "receivedImg"
QT_MOC_LITERAL(12, 185, 18), // "startMulCamProcess"
QT_MOC_LITERAL(13, 204, 20), // "testMulStereoMatcher"
QT_MOC_LITERAL(14, 225, 14), // "changeProcPara"
QT_MOC_LITERAL(15, 240, 4), // "qstr"
QT_MOC_LITERAL(16, 245, 2) // "wc"

    },
    "Image_Processing_Class\0outputImgProcessedRequest\0"
    "\0outputMulImgAIRequest\0mainwindowStatusRequest\0"
    "startProcessOnce\0startPicsProcess\0"
    "startMulCamTemp\0recvImg\0i\0start1CamProcess\0"
    "receivedImg\0startMulCamProcess\0"
    "testMulStereoMatcher\0changeProcPara\0"
    "qstr\0wc"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Image_Processing_Class[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    0,   65,    2, 0x06 /* Public */,
       4,    0,   66,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   67,    2, 0x0a /* Public */,
       6,    0,   68,    2, 0x0a /* Public */,
       7,    2,   69,    2, 0x0a /* Public */,
      10,    1,   74,    2, 0x0a /* Public */,
      12,    2,   77,    2, 0x0a /* Public */,
      13,    0,   82,    2, 0x0a /* Public */,
      14,    2,   83,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QImage, QMetaType::Int,    8,    9,
    QMetaType::Void, QMetaType::QImage,   11,
    QMetaType::Void, QMetaType::QImage, QMetaType::Int,    8,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   15,   16,

       0        // eod
};

void Image_Processing_Class::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Image_Processing_Class *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->outputImgProcessedRequest(); break;
        case 1: _t->outputMulImgAIRequest(); break;
        case 2: _t->mainwindowStatusRequest(); break;
        case 3: _t->startProcessOnce(); break;
        case 4: _t->startPicsProcess(); break;
        case 5: _t->startMulCamTemp((*reinterpret_cast< QImage(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->start1CamProcess((*reinterpret_cast< QImage(*)>(_a[1]))); break;
        case 7: _t->startMulCamProcess((*reinterpret_cast< QImage(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 8: _t->testMulStereoMatcher(); break;
        case 9: _t->changeProcPara((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Image_Processing_Class::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Image_Processing_Class::outputImgProcessedRequest)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Image_Processing_Class::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Image_Processing_Class::outputMulImgAIRequest)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Image_Processing_Class::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Image_Processing_Class::mainwindowStatusRequest)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Image_Processing_Class::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Image_Processing_Class.data,
    qt_meta_data_Image_Processing_Class,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Image_Processing_Class::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Image_Processing_Class::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Image_Processing_Class.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Image_Processing_Class::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void Image_Processing_Class::outputImgProcessedRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Image_Processing_Class::outputMulImgAIRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Image_Processing_Class::mainwindowStatusRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
