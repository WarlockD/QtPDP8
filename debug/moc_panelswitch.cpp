/****************************************************************************
** Meta object code from reading C++ file 'panelswitch.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../panelswitch.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'panelswitch.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PanelSwitch_t {
    QByteArrayData data[11];
    char stringdata0[95];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PanelSwitch_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PanelSwitch_t qt_meta_stringdata_PanelSwitch = {
    {
QT_MOC_LITERAL(0, 0, 11), // "PanelSwitch"
QT_MOC_LITERAL(1, 12, 13), // "switchChanged"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 5), // "state"
QT_MOC_LITERAL(4, 33, 13), // "switchPressed"
QT_MOC_LITERAL(5, 47, 3), // "tag"
QT_MOC_LITERAL(6, 51, 7), // "setDown"
QT_MOC_LITERAL(7, 59, 4), // "down"
QT_MOC_LITERAL(8, 64, 9), // "momantary"
QT_MOC_LITERAL(9, 74, 10), // "lightShade"
QT_MOC_LITERAL(10, 85, 9) // "darkShade"

    },
    "PanelSwitch\0switchChanged\0\0state\0"
    "switchPressed\0tag\0setDown\0down\0momantary\0"
    "lightShade\0darkShade"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PanelSwitch[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       5,   40, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    2,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   37,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int,    3,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    7,

 // properties: name, type, flags
       7, QMetaType::Bool, 0x00495103,
       5, QMetaType::Int, 0x00095103,
       8, QMetaType::Bool, 0x00095003,
       9, QMetaType::QColor, 0x00095003,
      10, QMetaType::QColor, 0x00095003,

 // properties: notify_signal_id
       0,
       0,
       0,
       0,
       0,

       0        // eod
};

void PanelSwitch::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PanelSwitch *_t = static_cast<PanelSwitch *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->switchChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->switchPressed((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->setDown((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PanelSwitch::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PanelSwitch::switchChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (PanelSwitch::*_t)(bool , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PanelSwitch::switchPressed)) {
                *result = 1;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        PanelSwitch *_t = static_cast<PanelSwitch *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->isDown(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->getTag(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->m_momantary; break;
        case 3: *reinterpret_cast< QColor*>(_v) = _t->m_lightShade; break;
        case 4: *reinterpret_cast< QColor*>(_v) = _t->m_darkShade; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        PanelSwitch *_t = static_cast<PanelSwitch *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setDown(*reinterpret_cast< bool*>(_v)); break;
        case 1: _t->setTag(*reinterpret_cast< int*>(_v)); break;
        case 2:
            if (_t->m_momantary != *reinterpret_cast< bool*>(_v)) {
                _t->m_momantary = *reinterpret_cast< bool*>(_v);
            }
            break;
        case 3:
            if (_t->m_lightShade != *reinterpret_cast< QColor*>(_v)) {
                _t->m_lightShade = *reinterpret_cast< QColor*>(_v);
            }
            break;
        case 4:
            if (_t->m_darkShade != *reinterpret_cast< QColor*>(_v)) {
                _t->m_darkShade = *reinterpret_cast< QColor*>(_v);
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject PanelSwitch::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_PanelSwitch.data,
      qt_meta_data_PanelSwitch,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PanelSwitch::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PanelSwitch::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PanelSwitch.stringdata0))
        return static_cast<void*>(const_cast< PanelSwitch*>(this));
    return QWidget::qt_metacast(_clname);
}

int PanelSwitch::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void PanelSwitch::switchChanged(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PanelSwitch::switchPressed(bool _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
