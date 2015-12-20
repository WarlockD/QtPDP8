/****************************************************************************
** Meta object code from reading C++ file 'placabletextedit.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../placabletextedit.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'placabletextedit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PlacableTextEdit_t {
    QByteArrayData data[9];
    char stringdata0[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlacableTextEdit_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlacableTextEdit_t qt_meta_stringdata_PlacableTextEdit = {
    {
QT_MOC_LITERAL(0, 0, 16), // "PlacableTextEdit"
QT_MOC_LITERAL(1, 17, 13), // "putCharSignal"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 1), // "c"
QT_MOC_LITERAL(4, 34, 12), // "keyboardSend"
QT_MOC_LITERAL(5, 47, 1), // "k"
QT_MOC_LITERAL(6, 49, 7), // "putChar"
QT_MOC_LITERAL(7, 57, 7), // "blinker"
QT_MOC_LITERAL(8, 65, 11) // "putCharSlot"

    },
    "PlacableTextEdit\0putCharSignal\0\0c\0"
    "keyboardSend\0k\0putChar\0blinker\0"
    "putCharSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlacableTextEdit[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    1,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   45,    2, 0x0a /* Public */,
       7,    0,   48,    2, 0x0a /* Public */,
       8,    1,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void PlacableTextEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PlacableTextEdit *_t = static_cast<PlacableTextEdit *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->putCharSignal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->keyboardSend((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->putChar((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->blinker(); break;
        case 4: _t->putCharSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PlacableTextEdit::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlacableTextEdit::putCharSignal)) {
                *result = 0;
            }
        }
        {
            typedef void (PlacableTextEdit::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlacableTextEdit::keyboardSend)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject PlacableTextEdit::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_PlacableTextEdit.data,
      qt_meta_data_PlacableTextEdit,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PlacableTextEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlacableTextEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PlacableTextEdit.stringdata0))
        return static_cast<void*>(const_cast< PlacableTextEdit*>(this));
    return QWidget::qt_metacast(_clname);
}

int PlacableTextEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void PlacableTextEdit::putCharSignal(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PlacableTextEdit::keyboardSend(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
