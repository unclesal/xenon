/****************************************************************************
** Meta object code from reading C++ file 'LCDWithButtons.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../common/widgets/LCDWithButtons.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LCDWithButtons.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LCDWithButtons_t {
    QByteArrayData data[1];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LCDWithButtons_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LCDWithButtons_t qt_meta_stringdata_LCDWithButtons = {
    {
QT_MOC_LITERAL(0, 0, 14) // "LCDWithButtons"

    },
    "LCDWithButtons"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LCDWithButtons[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void LCDWithButtons::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject LCDWithButtons::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_LCDWithButtons.data,
    qt_meta_data_LCDWithButtons,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *LCDWithButtons::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LCDWithButtons::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LCDWithButtons.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui::LCDWithButtons"))
        return static_cast< Ui::LCDWithButtons*>(this);
    return QWidget::qt_metacast(_clname);
}

int LCDWithButtons::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
