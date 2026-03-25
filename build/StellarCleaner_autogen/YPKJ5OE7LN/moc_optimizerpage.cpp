/****************************************************************************
** Meta object code from reading C++ file 'optimizerpage.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/optimizerpage.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'optimizerpage.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_OptimizerPage_t {
    uint offsetsAndSizes[32];
    char stringdata0[14];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[10];
    char stringdata5[8];
    char stringdata6[18];
    char stringdata7[16];
    char stringdata8[12];
    char stringdata9[13];
    char stringdata10[14];
    char stringdata11[17];
    char stringdata12[5];
    char stringdata13[4];
    char stringdata14[10];
    char stringdata15[4];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_OptimizerPage_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_OptimizerPage_t qt_meta_stringdata_OptimizerPage = {
    {
        QT_MOC_LITERAL(0, 13),  // "OptimizerPage"
        QT_MOC_LITERAL(14, 17),  // "totalFreedChanged"
        QT_MOC_LITERAL(32, 0),  // ""
        QT_MOC_LITERAL(33, 5),  // "bytes"
        QT_MOC_LITERAL(39, 9),  // "onAnalyze"
        QT_MOC_LITERAL(49, 7),  // "onClean"
        QT_MOC_LITERAL(57, 17),  // "onAnalyzeFinished"
        QT_MOC_LITERAL(75, 15),  // "onCleanFinished"
        QT_MOC_LITERAL(91, 11),  // "onSelectAll"
        QT_MOC_LITERAL(103, 12),  // "onSelectNone"
        QT_MOC_LITERAL(116, 13),  // "onItemChanged"
        QT_MOC_LITERAL(130, 16),  // "QTreeWidgetItem*"
        QT_MOC_LITERAL(147, 4),  // "item"
        QT_MOC_LITERAL(152, 3),  // "col"
        QT_MOC_LITERAL(156, 9),  // "appendLog"
        QT_MOC_LITERAL(166, 3)   // "msg"
    },
    "OptimizerPage",
    "totalFreedChanged",
    "",
    "bytes",
    "onAnalyze",
    "onClean",
    "onAnalyzeFinished",
    "onCleanFinished",
    "onSelectAll",
    "onSelectNone",
    "onItemChanged",
    "QTreeWidgetItem*",
    "item",
    "col",
    "appendLog",
    "msg"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_OptimizerPage[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   68,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,   71,    2, 0x08,    3 /* Private */,
       5,    0,   72,    2, 0x08,    4 /* Private */,
       6,    0,   73,    2, 0x08,    5 /* Private */,
       7,    0,   74,    2, 0x08,    6 /* Private */,
       8,    0,   75,    2, 0x08,    7 /* Private */,
       9,    0,   76,    2, 0x08,    8 /* Private */,
      10,    2,   77,    2, 0x08,    9 /* Private */,
      14,    1,   82,    2, 0x08,   12 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::LongLong,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11, QMetaType::Int,   12,   13,
    QMetaType::Void, QMetaType::QString,   15,

       0        // eod
};

Q_CONSTINIT const QMetaObject OptimizerPage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_OptimizerPage.offsetsAndSizes,
    qt_meta_data_OptimizerPage,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_OptimizerPage_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<OptimizerPage, std::true_type>,
        // method 'totalFreedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        // method 'onAnalyze'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onClean'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAnalyzeFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onCleanFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSelectAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSelectNone'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onItemChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidgetItem *, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'appendLog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void OptimizerPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<OptimizerPage *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->totalFreedChanged((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1]))); break;
        case 1: _t->onAnalyze(); break;
        case 2: _t->onClean(); break;
        case 3: _t->onAnalyzeFinished(); break;
        case 4: _t->onCleanFinished(); break;
        case 5: _t->onSelectAll(); break;
        case 6: _t->onSelectNone(); break;
        case 7: _t->onItemChanged((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 8: _t->appendLog((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (OptimizerPage::*)(qint64 );
            if (_t _q_method = &OptimizerPage::totalFreedChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *OptimizerPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OptimizerPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_OptimizerPage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int OptimizerPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void OptimizerPage::totalFreedChanged(qint64 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
