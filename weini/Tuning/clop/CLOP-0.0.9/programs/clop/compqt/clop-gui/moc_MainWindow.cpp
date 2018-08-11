/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/qt-gui/MainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[23];
    char stringdata0[389];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 24), // "on_action_Open_triggered"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 25), // "on_action_Close_triggered"
QT_MOC_LITERAL(4, 63, 25), // "on_action_Start_triggered"
QT_MOC_LITERAL(5, 89, 28), // "on_action_SoftStop_triggered"
QT_MOC_LITERAL(6, 118, 28), // "on_action_HardStop_triggered"
QT_MOC_LITERAL(7, 147, 28), // "on_action_About_Qt_triggered"
QT_MOC_LITERAL(8, 176, 25), // "on_action_About_triggered"
QT_MOC_LITERAL(9, 202, 16), // "onOpenRecentFile"
QT_MOC_LITERAL(10, 219, 11), // "onRexpReady"
QT_MOC_LITERAL(11, 231, 13), // "onRexpRunning"
QT_MOC_LITERAL(12, 245, 13), // "onRexpMessage"
QT_MOC_LITERAL(13, 259, 11), // "std::string"
QT_MOC_LITERAL(14, 271, 8), // "sMessage"
QT_MOC_LITERAL(15, 280, 17), // "updateConnections"
QT_MOC_LITERAL(16, 298, 9), // "updateAll"
QT_MOC_LITERAL(17, 308, 13), // "updateCounter"
QT_MOC_LITERAL(18, 322, 16), // "updateStatistics"
QT_MOC_LITERAL(19, 339, 10), // "updatePlot"
QT_MOC_LITERAL(20, 350, 14), // "displayHessian"
QT_MOC_LITERAL(21, 365, 12), // "displayEigen"
QT_MOC_LITERAL(22, 378, 10) // "displayMax"

    },
    "MainWindow\0on_action_Open_triggered\0"
    "\0on_action_Close_triggered\0"
    "on_action_Start_triggered\0"
    "on_action_SoftStop_triggered\0"
    "on_action_HardStop_triggered\0"
    "on_action_About_Qt_triggered\0"
    "on_action_About_triggered\0onOpenRecentFile\0"
    "onRexpReady\0onRexpRunning\0onRexpMessage\0"
    "std::string\0sMessage\0updateConnections\0"
    "updateAll\0updateCounter\0updateStatistics\0"
    "updatePlot\0displayHessian\0displayEigen\0"
    "displayMax"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  109,    2, 0x08 /* Private */,
       3,    0,  110,    2, 0x08 /* Private */,
       4,    0,  111,    2, 0x08 /* Private */,
       5,    0,  112,    2, 0x08 /* Private */,
       6,    0,  113,    2, 0x08 /* Private */,
       7,    0,  114,    2, 0x08 /* Private */,
       8,    0,  115,    2, 0x08 /* Private */,
       9,    0,  116,    2, 0x08 /* Private */,
      10,    0,  117,    2, 0x08 /* Private */,
      11,    0,  118,    2, 0x08 /* Private */,
      12,    1,  119,    2, 0x08 /* Private */,
      15,    0,  122,    2, 0x08 /* Private */,
      16,    0,  123,    2, 0x08 /* Private */,
      17,    0,  124,    2, 0x08 /* Private */,
      18,    0,  125,    2, 0x08 /* Private */,
      19,    0,  126,    2, 0x08 /* Private */,
      20,    0,  127,    2, 0x08 /* Private */,
      21,    0,  128,    2, 0x08 /* Private */,
      22,    0,  129,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_action_Open_triggered(); break;
        case 1: _t->on_action_Close_triggered(); break;
        case 2: _t->on_action_Start_triggered(); break;
        case 3: _t->on_action_SoftStop_triggered(); break;
        case 4: _t->on_action_HardStop_triggered(); break;
        case 5: _t->on_action_About_Qt_triggered(); break;
        case 6: _t->on_action_About_triggered(); break;
        case 7: _t->onOpenRecentFile(); break;
        case 8: _t->onRexpReady(); break;
        case 9: _t->onRexpRunning(); break;
        case 10: _t->onRexpMessage((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 11: _t->updateConnections(); break;
        case 12: _t->updateAll(); break;
        case 13: _t->updateCounter(); break;
        case 14: _t->updateStatistics(); break;
        case 15: _t->updatePlot(); break;
        case 16: _t->displayHessian(); break;
        case 17: _t->displayEigen(); break;
        case 18: _t->displayMax(); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 19;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
