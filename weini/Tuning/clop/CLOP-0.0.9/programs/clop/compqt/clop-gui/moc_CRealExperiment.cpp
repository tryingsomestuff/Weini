/****************************************************************************
** Meta object code from reading C++ file 'CRealExperiment.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/real/CRealExperiment.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CRealExperiment.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CRealExperiment_t {
    QByteArrayData data[20];
    char stringdata0[175];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CRealExperiment_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CRealExperiment_t qt_meta_stringdata_CRealExperiment = {
    {
QT_MOC_LITERAL(0, 0, 15), // "CRealExperiment"
QT_MOC_LITERAL(1, 16, 12), // "StartLoading"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 4), // "size"
QT_MOC_LITERAL(4, 35, 15), // "LoadingProgress"
QT_MOC_LITERAL(5, 51, 3), // "pos"
QT_MOC_LITERAL(6, 55, 7), // "Started"
QT_MOC_LITERAL(7, 63, 6), // "Result"
QT_MOC_LITERAL(8, 70, 6), // "Sample"
QT_MOC_LITERAL(9, 77, 8), // "Finished"
QT_MOC_LITERAL(10, 86, 7), // "Message"
QT_MOC_LITERAL(11, 94, 11), // "std::string"
QT_MOC_LITERAL(12, 106, 8), // "sMessage"
QT_MOC_LITERAL(13, 115, 11), // "OnPROutcome"
QT_MOC_LITERAL(14, 127, 2), // "id"
QT_MOC_LITERAL(15, 130, 4), // "Seed"
QT_MOC_LITERAL(16, 135, 8), // "COutcome"
QT_MOC_LITERAL(17, 144, 7), // "outcome"
QT_MOC_LITERAL(18, 152, 9), // "OnPRError"
QT_MOC_LITERAL(19, 162, 12) // "AbortLoading"

    },
    "CRealExperiment\0StartLoading\0\0size\0"
    "LoadingProgress\0pos\0Started\0Result\0"
    "Sample\0Finished\0Message\0std::string\0"
    "sMessage\0OnPROutcome\0id\0Seed\0COutcome\0"
    "outcome\0OnPRError\0AbortLoading"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CRealExperiment[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       4,    1,   67,    2, 0x06 /* Public */,
       6,    0,   70,    2, 0x06 /* Public */,
       7,    0,   71,    2, 0x06 /* Public */,
       8,    0,   72,    2, 0x06 /* Public */,
       9,    0,   73,    2, 0x06 /* Public */,
      10,    1,   74,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    3,   77,    2, 0x08 /* Private */,
      18,    2,   84,    2, 0x08 /* Private */,
      19,    0,   89,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 16,   14,   15,   17,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   14,   15,
    QMetaType::Void,

       0        // eod
};

void CRealExperiment::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CRealExperiment *_t = static_cast<CRealExperiment *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->StartLoading((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->LoadingProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->Started(); break;
        case 3: _t->Result(); break;
        case 4: _t->Sample(); break;
        case 5: _t->Finished(); break;
        case 6: _t->Message((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 7: _t->OnPROutcome((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< COutcome(*)>(_a[3]))); break;
        case 8: _t->OnPRError((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: _t->AbortLoading(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CRealExperiment::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CRealExperiment::StartLoading)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (CRealExperiment::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CRealExperiment::LoadingProgress)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (CRealExperiment::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CRealExperiment::Started)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (CRealExperiment::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CRealExperiment::Result)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (CRealExperiment::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CRealExperiment::Sample)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (CRealExperiment::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CRealExperiment::Finished)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (CRealExperiment::*_t)(std::string );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CRealExperiment::Message)) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject CRealExperiment::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CRealExperiment.data,
      qt_meta_data_CRealExperiment,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CRealExperiment::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CRealExperiment::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CRealExperiment.stringdata0))
        return static_cast<void*>(const_cast< CRealExperiment*>(this));
    return QObject::qt_metacast(_clname);
}

int CRealExperiment::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void CRealExperiment::StartLoading(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CRealExperiment::LoadingProgress(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CRealExperiment::Started()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void CRealExperiment::Result()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void CRealExperiment::Sample()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void CRealExperiment::Finished()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void CRealExperiment::Message(std::string _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
