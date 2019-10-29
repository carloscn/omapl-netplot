/****************************************************************************
** Meta object code from reading C++ file 'netsocket.h'
**
** Created: Tue Oct 29 10:36:58 2019
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../app/netsocket.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'netsocket.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_netsocket[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   11,   10,   10, 0x05,
      51,   10,   10,   10, 0x05,
      70,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      88,   10,   10,   10, 0x0a,
     105,   10,   10,   10, 0x0a,
     154,  142,   10,   10, 0x0a,
     185,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_netsocket[] = {
    "netsocket\0\0,\0change_sample_para(uint32_t,uint32_t)\0"
    "cmd_start_sample()\0cmd_stop_sample()\0"
    "on_new_connect()\0on_dsp_data_recv(std::vector<float>)\0"
    "data,length\0on_get_ready_datas(float*,int)\0"
    "on_read_message()\0"
};

void netsocket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        netsocket *_t = static_cast<netsocket *>(_o);
        switch (_id) {
        case 0: _t->change_sample_para((*reinterpret_cast< uint32_t(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2]))); break;
        case 1: _t->cmd_start_sample(); break;
        case 2: _t->cmd_stop_sample(); break;
        case 3: _t->on_new_connect(); break;
        case 4: _t->on_dsp_data_recv((*reinterpret_cast< std::vector<float>(*)>(_a[1]))); break;
        case 5: _t->on_get_ready_datas((*reinterpret_cast< float*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->on_read_message(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData netsocket::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject netsocket::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_netsocket,
      qt_meta_data_netsocket, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &netsocket::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *netsocket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *netsocket::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_netsocket))
        return static_cast<void*>(const_cast< netsocket*>(this));
    return QThread::qt_metacast(_clname);
}

int netsocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void netsocket::change_sample_para(uint32_t _t1, uint32_t _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void netsocket::cmd_start_sample()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void netsocket::cmd_stop_sample()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
