#ifndef _COM_PARAM_H
#define _COM_PARAM_H

#include <QObject>

class _COMM_ENUMS:public QObject
{
    Q_OBJECT
public:
    explicit _COMM_ENUMS();
    ~_COMM_ENUMS();

public:
    //PC输出给PLC信号
    enum SYSTEM_STATUS { Closed = 0, Working = 1, MonitorOnly = 2, SystemErr = 3 };
    Q_ENUM(SYSTEM_STATUS)
    enum DOWN_PERMIT { DisAllow = 0, Allow = 1 };
    Q_ENUM(DOWN_PERMIT)
    enum NO_BOX_ALARM { NoBox = 0, Box = 1 };
    Q_ENUM(NO_BOX_ALARM)
    enum SLING_ALARM { NoAlarm = 1, Alarm = 0 };
    Q_ENUM(SLING_ALARM)
    enum TRUCK_CHECK { InsideTruck = 0, OutsideTruck = 1, NoTruck = 2 };
    Q_ENUM(TRUCK_CHECK)

    //PLC输入给PC信号
    enum BOX_TYPE { Null = 0, Container_20 = 1, Container_40 = 2, Container_45 = 3 };
    Q_ENUM(BOX_TYPE)
    enum PLC_STATUS {Stop = 0, Start = 1 };
    Q_ENUM(PLC_STATUS)
    enum LOCKETED_STATUS { UnlockUnlanding = 0x0101, UnlockLanding = 0x0102, LockUnlanding = 0x0201, LockLanding = 0x0202 };
    Q_ENUM(LOCKETED_STATUS)
};

class _PC_PARAMETER
{
public:
    explicit _PC_PARAMETER();
    ~_PC_PARAMETER();

    _COMM_ENUMS::SYSTEM_STATUS _iSystemStatus;
    _COMM_ENUMS::DOWN_PERMIT _bDownPermit;
    _COMM_ENUMS::NO_BOX_ALARM _bNoBoxAlarm;
    _COMM_ENUMS::SLING_ALARM _bSlingAlarm_20cm;
    _COMM_ENUMS::SLING_ALARM _bSlingAlarm_25cm;
    _COMM_ENUMS::SLING_ALARM _bDragAlarm;
    _COMM_ENUMS::TRUCK_CHECK _bTruckCheck;
};

class _PLC_PARAMETER
{
public:
    explicit _PLC_PARAMETER();
    ~_PLC_PARAMETER();

public:
    _COMM_ENUMS::BOX_TYPE _iBoxType;
    _COMM_ENUMS::PLC_STATUS _bPlcStatus;
    _COMM_ENUMS::LOCKETED_STATUS _bBoxLockedStatus;
    int _iHoistHeight;
    int _iTrolleyPosition;
};

#endif // _COM_PARAM_H
