/****************************************************************************
** Meta object code from reading C++ file 'toolspage.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/toolspage.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'toolspage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_ToolsPage_t {
    uint offsetsAndSizes[24];
    char stringdata0[10];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[19];
    char stringdata5[17];
    char stringdata6[14];
    char stringdata7[16];
    char stringdata8[16];
    char stringdata9[12];
    char stringdata10[18];
    char stringdata11[5];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_ToolsPage_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_ToolsPage_t qt_meta_stringdata_ToolsPage = {
    {
        QT_MOC_LITERAL(0, 9),  // "ToolsPage"
        QT_MOC_LITERAL(10, 16),  // "onStartupTabShow"
        QT_MOC_LITERAL(27, 0),  // ""
        QT_MOC_LITERAL(28, 16),  // "onProcessTabShow"
        QT_MOC_LITERAL(45, 18),  // "onUninstallTabShow"
        QT_MOC_LITERAL(64, 16),  // "onProcessRefresh"
        QT_MOC_LITERAL(81, 13),  // "onProcessKill"
        QT_MOC_LITERAL(95, 15),  // "onStartupToggle"
        QT_MOC_LITERAL(111, 15),  // "onStartupRemove"
        QT_MOC_LITERAL(127, 11),  // "onUninstall"
        QT_MOC_LITERAL(139, 17),  // "onUninstallSearch"
        QT_MOC_LITERAL(157, 4)   // "text"
    },
    "ToolsPage",
    "onStartupTabShow",
    "",
    "onProcessTabShow",
    "onUninstallTabShow",
    "onProcessRefresh",
    "onProcessKill",
    "onStartupToggle",
    "onStartupRemove",
    "onUninstall",
    "onUninstallSearch",
    "text"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_ToolsPage[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   68,    2, 0x08,    1 /* Private */,
       3,    0,   69,    2, 0x08,    2 /* Private */,
       4,    0,   70,    2, 0x08,    3 /* Private */,
       5,    0,   71,    2, 0x08,    4 /* Private */,
       6,    0,   72,    2, 0x08,    5 /* Private */,
       7,    0,   73,    2, 0x08,    6 /* Private */,
       8,    0,   74,    2, 0x08,    7 /* Private */,
       9,    0,   75,    2, 0x08,    8 /* Private */,
      10,    1,   76,    2, 0x08,    9 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   11,

       0        // eod
};

Q_CONSTINIT const QMetaObject ToolsPage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ToolsPage.offsetsAndSizes,
    qt_meta_data_ToolsPage,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_ToolsPage_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ToolsPage, std::true_type>,
        // method 'onStartupTabShow'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onProcessTabShow'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onUninstallTabShow'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onProcessRefresh'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onProcessKill'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStartupToggle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStartupRemove'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onUninstall'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onUninstallSearch'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void ToolsPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ToolsPage *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onStartupTabShow(); break;
        case 1: _t->onProcessTabShow(); break;
        case 2: _t->onUninstallTabShow(); break;
        case 3: _t->onProcessRefresh(); break;
        case 4: _t->onProcessKill(); break;
        case 5: _t->onStartupToggle(); break;
        case 6: _t->onStartupRemove(); break;
        case 7: _t->onUninstall(); break;
        case 8: _t->onUninstallSearch((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *ToolsPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ToolsPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ToolsPage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ToolsPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
