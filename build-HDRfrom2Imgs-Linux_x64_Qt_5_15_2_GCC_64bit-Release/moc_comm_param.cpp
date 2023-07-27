/****************************************************************************
** Meta object code from reading C++ file 'comm_param.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../RadarDetection/comm_param.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'comm_param.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata__COMM_ENUMS_t {
    QByteArrayData data[32];
    char stringdata0[331];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata__COMM_ENUMS_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata__COMM_ENUMS_t qt_meta_stringdata__COMM_ENUMS = {
    {
QT_MOC_LITERAL(0, 0, 11), // "_COMM_ENUMS"
QT_MOC_LITERAL(1, 12, 13), // "SYSTEM_STATUS"
QT_MOC_LITERAL(2, 26, 6), // "Closed"
QT_MOC_LITERAL(3, 33, 7), // "Working"
QT_MOC_LITERAL(4, 41, 11), // "MonitorOnly"
QT_MOC_LITERAL(5, 53, 9), // "SystemErr"
QT_MOC_LITERAL(6, 63, 11), // "DOWN_PERMIT"
QT_MOC_LITERAL(7, 75, 8), // "DisAllow"
QT_MOC_LITERAL(8, 84, 5), // "Allow"
QT_MOC_LITERAL(9, 90, 12), // "NO_BOX_ALARM"
QT_MOC_LITERAL(10, 103, 5), // "NoBox"
QT_MOC_LITERAL(11, 109, 3), // "Box"
QT_MOC_LITERAL(12, 113, 11), // "SLING_ALARM"
QT_MOC_LITERAL(13, 125, 7), // "NoAlarm"
QT_MOC_LITERAL(14, 133, 5), // "Alarm"
QT_MOC_LITERAL(15, 139, 11), // "TRUCK_CHECK"
QT_MOC_LITERAL(16, 151, 11), // "InsideTruck"
QT_MOC_LITERAL(17, 163, 12), // "OutsideTruck"
QT_MOC_LITERAL(18, 176, 7), // "NoTruck"
QT_MOC_LITERAL(19, 184, 8), // "BOX_TYPE"
QT_MOC_LITERAL(20, 193, 4), // "Null"
QT_MOC_LITERAL(21, 198, 12), // "Container_20"
QT_MOC_LITERAL(22, 211, 12), // "Container_40"
QT_MOC_LITERAL(23, 224, 12), // "Container_45"
QT_MOC_LITERAL(24, 237, 10), // "PLC_STATUS"
QT_MOC_LITERAL(25, 248, 4), // "Stop"
QT_MOC_LITERAL(26, 253, 5), // "Start"
QT_MOC_LITERAL(27, 259, 15), // "LOCKETED_STATUS"
QT_MOC_LITERAL(28, 275, 15), // "UnlockUnlanding"
QT_MOC_LITERAL(29, 291, 13), // "UnlockLanding"
QT_MOC_LITERAL(30, 305, 13), // "LockUnlanding"
QT_MOC_LITERAL(31, 319, 11) // "LockLanding"

    },
    "_COMM_ENUMS\0SYSTEM_STATUS\0Closed\0"
    "Working\0MonitorOnly\0SystemErr\0DOWN_PERMIT\0"
    "DisAllow\0Allow\0NO_BOX_ALARM\0NoBox\0Box\0"
    "SLING_ALARM\0NoAlarm\0Alarm\0TRUCK_CHECK\0"
    "InsideTruck\0OutsideTruck\0NoTruck\0"
    "BOX_TYPE\0Null\0Container_20\0Container_40\0"
    "Container_45\0PLC_STATUS\0Stop\0Start\0"
    "LOCKETED_STATUS\0UnlockUnlanding\0"
    "UnlockLanding\0LockUnlanding\0LockLanding"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data__COMM_ENUMS[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       8,   14, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // enums: name, alias, flags, count, data
       1,    1, 0x0,    4,   54,
       6,    6, 0x0,    2,   62,
       9,    9, 0x0,    2,   66,
      12,   12, 0x0,    2,   70,
      15,   15, 0x0,    3,   74,
      19,   19, 0x0,    4,   80,
      24,   24, 0x0,    2,   88,
      27,   27, 0x0,    4,   92,

 // enum data: key, value
       2, uint(_COMM_ENUMS::Closed),
       3, uint(_COMM_ENUMS::Working),
       4, uint(_COMM_ENUMS::MonitorOnly),
       5, uint(_COMM_ENUMS::SystemErr),
       7, uint(_COMM_ENUMS::DisAllow),
       8, uint(_COMM_ENUMS::Allow),
      10, uint(_COMM_ENUMS::NoBox),
      11, uint(_COMM_ENUMS::Box),
      13, uint(_COMM_ENUMS::NoAlarm),
      14, uint(_COMM_ENUMS::Alarm),
      16, uint(_COMM_ENUMS::InsideTruck),
      17, uint(_COMM_ENUMS::OutsideTruck),
      18, uint(_COMM_ENUMS::NoTruck),
      20, uint(_COMM_ENUMS::Null),
      21, uint(_COMM_ENUMS::Container_20),
      22, uint(_COMM_ENUMS::Container_40),
      23, uint(_COMM_ENUMS::Container_45),
      25, uint(_COMM_ENUMS::Stop),
      26, uint(_COMM_ENUMS::Start),
      28, uint(_COMM_ENUMS::UnlockUnlanding),
      29, uint(_COMM_ENUMS::UnlockLanding),
      30, uint(_COMM_ENUMS::LockUnlanding),
      31, uint(_COMM_ENUMS::LockLanding),

       0        // eod
};

void _COMM_ENUMS::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject _COMM_ENUMS::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata__COMM_ENUMS.data,
    qt_meta_data__COMM_ENUMS,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *_COMM_ENUMS::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *_COMM_ENUMS::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata__COMM_ENUMS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int _COMM_ENUMS::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
