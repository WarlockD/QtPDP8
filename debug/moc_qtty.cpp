/****************************************************************************
** Meta object code from reading C++ file 'qtty.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qtty.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtty.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QTTY_t {
    QByteArrayData data[10];
    char stringdata0[81];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QTTY_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QTTY_t qt_meta_stringdata_QTTY = {
    {
QT_MOC_LITERAL(0, 0, 4), // "QTTY"
QT_MOC_LITERAL(1, 5, 14), // "putDataSignalC"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 1), // "c"
QT_MOC_LITERAL(4, 23, 14), // "putDataSignalS"
QT_MOC_LITERAL(5, 38, 1), // "s"
QT_MOC_LITERAL(6, 40, 12), // "keyboardSend"
QT_MOC_LITERAL(7, 53, 1), // "k"
QT_MOC_LITERAL(8, 55, 12), // "putDataSlotC"
QT_MOC_LITERAL(9, 68, 12) // "putDataSlotS"

    },
    "QTTY\0putDataSignalC\0\0c\0putDataSignalS\0"
    "s\0keyboardSend\0k\0putDataSlotC\0"
    "putDataSlotS"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QTTY[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    1,   42,    2, 0x06 /* Public */,
       6,    1,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   48,    2, 0x08 /* Private */,
       9,    1,   51,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Int,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void QTTY::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QTTY *_t = static_cast<QTTY *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->putDataSignalC((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->putDataSignalS((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->keyboardSend((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->putDataSlotC((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->putDataSlotS((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QTTY::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QTTY::putDataSignalC)) {
                *result = 0;
            }
        }
        {
            typedef void (QTTY::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QTTY::putDataSignalS)) {
                *result = 1;
            }
        }
        {
            typedef void (QTTY::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QTTY::keyboardSend)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject QTTY::staticMetaObject = {
    { &QPlainTextEdit::staticMetaObject, qt_meta_stringdata_QTTY.data,
      qt_meta_data_QTTY,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QTTY::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QTTY::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QTTY.stringdata0))
        return static_cast<void*>(const_cast< QTTY*>(this));
    return QPlainTextEdit::qt_metacast(_clname);
}

int QTTY::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlainTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QTTY::putDataSignalC(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QTTY::putDataSignalS(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QTTY::keyboardSend(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
