/****************************************************************************
** Meta object code from reading C++ file 'pdp8_silly.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../pdp8_silly.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pdp8_silly.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PDP8__PDP8_Silly_t {
    QByteArrayData data[18];
    char stringdata0[163];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PDP8__PDP8_Silly_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PDP8__PDP8_Silly_t qt_meta_stringdata_PDP8__PDP8_Silly = {
    {
QT_MOC_LITERAL(0, 0, 16), // "PDP8::PDP8_Silly"
QT_MOC_LITERAL(1, 17, 10), // "consoleOut"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 1), // "c"
QT_MOC_LITERAL(4, 31, 15), // "updateRegesters"
QT_MOC_LITERAL(5, 47, 3), // "LAC"
QT_MOC_LITERAL(6, 51, 2), // "PC"
QT_MOC_LITERAL(7, 54, 2), // "MQ"
QT_MOC_LITERAL(8, 57, 13), // "updateDisplay"
QT_MOC_LITERAL(9, 71, 16), // "DisplayRegesters"
QT_MOC_LITERAL(10, 88, 3), // "reg"
QT_MOC_LITERAL(11, 92, 13), // "keyPressEvent"
QT_MOC_LITERAL(12, 106, 10), // "QKeyEvent*"
QT_MOC_LITERAL(13, 117, 1), // "a"
QT_MOC_LITERAL(14, 119, 8), // "keyPress"
QT_MOC_LITERAL(15, 128, 15), // "setRunningState"
QT_MOC_LITERAL(16, 144, 12), // "RunningState"
QT_MOC_LITERAL(17, 157, 5) // "state"

    },
    "PDP8::PDP8_Silly\0consoleOut\0\0c\0"
    "updateRegesters\0LAC\0PC\0MQ\0updateDisplay\0"
    "DisplayRegesters\0reg\0keyPressEvent\0"
    "QKeyEvent*\0a\0keyPress\0setRunningState\0"
    "RunningState\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PDP8__PDP8_Silly[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    3,   47,    2, 0x06 /* Public */,
       8,    1,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    1,   57,    2, 0x0a /* Public */,
      14,    1,   60,    2, 0x0a /* Public */,
      15,    1,   63,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    5,    6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, 0x80000000 | 16,   17,

       0        // eod
};

void PDP8::PDP8_Silly::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PDP8_Silly *_t = static_cast<PDP8_Silly *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->consoleOut((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->updateRegesters((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->updateDisplay((*reinterpret_cast< const DisplayRegesters(*)>(_a[1]))); break;
        case 3: _t->keyPressEvent((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 4: _t->keyPress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->setRunningState((*reinterpret_cast< RunningState(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PDP8_Silly::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PDP8_Silly::consoleOut)) {
                *result = 0;
            }
        }
        {
            typedef void (PDP8_Silly::*_t)(int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PDP8_Silly::updateRegesters)) {
                *result = 1;
            }
        }
        {
            typedef void (PDP8_Silly::*_t)(const DisplayRegesters & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PDP8_Silly::updateDisplay)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject PDP8::PDP8_Silly::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PDP8__PDP8_Silly.data,
      qt_meta_data_PDP8__PDP8_Silly,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PDP8::PDP8_Silly::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PDP8::PDP8_Silly::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PDP8__PDP8_Silly.stringdata0))
        return static_cast<void*>(const_cast< PDP8_Silly*>(this));
    return QObject::qt_metacast(_clname);
}

int PDP8::PDP8_Silly::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void PDP8::PDP8_Silly::consoleOut(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PDP8::PDP8_Silly::updateRegesters(int _t1, int _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PDP8::PDP8_Silly::updateDisplay(const DisplayRegesters & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
