/****************************************************************************
** Meta object code from reading C++ file 'netclientthread.h'
**
** Created: Mon Aug 6 13:31:43 2018
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../ads8568_dsp/host/app/netclientthread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'netclientthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetClientThread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      46,   34,   16,   16, 0x0a,
      77,   16,   16,   16, 0x0a,
      94,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NetClientThread[] = {
    "NetClientThread\0\0close_activity()\0"
    "data,length\0on_get_ready_datas(float*,int)\0"
    "on_new_connect()\0on_read_message()\0"
};

void NetClientThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NetClientThread *_t = static_cast<NetClientThread *>(_o);
        switch (_id) {
        case 0: _t->close_activity(); break;
        case 1: _t->on_get_ready_datas((*reinterpret_cast< float*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->on_new_connect(); break;
        case 3: _t->on_read_message(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NetClientThread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NetClientThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_NetClientThread,
      qt_meta_data_NetClientThread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetClientThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetClientThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetClientThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetClientThread))
        return static_cast<void*>(const_cast< NetClientThread*>(this));
    return QThread::qt_metacast(_clname);
}

int NetClientThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void NetClientThread::close_activity()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
