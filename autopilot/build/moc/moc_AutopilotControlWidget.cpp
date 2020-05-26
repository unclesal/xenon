/****************************************************************************
** Meta object code from reading C++ file 'AutopilotControlWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../common/widgets/AutopilotControlWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AutopilotControlWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AutopilotControlWidget_t {
    QByteArrayData data[33];
    char stringdata0[749];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AutopilotControlWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AutopilotControlWidget_t qt_meta_stringdata_AutopilotControlWidget = {
    {
QT_MOC_LITERAL(0, 0, 22), // "AutopilotControlWidget"
QT_MOC_LITERAL(1, 23, 17), // "signal__need_send"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 11), // "CommandSet&"
QT_MOC_LITERAL(4, 54, 3), // "cmd"
QT_MOC_LITERAL(5, 58, 22), // "slot__xplane_connected"
QT_MOC_LITERAL(6, 81, 25), // "slot__xplane_disconnected"
QT_MOC_LITERAL(7, 107, 25), // "slot__got_autopilot_state"
QT_MOC_LITERAL(8, 133, 22), // "CommandAutopilotState&"
QT_MOC_LITERAL(9, 156, 29), // "slot__got_user_aircraft_state"
QT_MOC_LITERAL(10, 186, 14), // "AircraftState&"
QT_MOC_LITERAL(11, 201, 26), // "slot__button_hsi_gps_fired"
QT_MOC_LITERAL(12, 228, 27), // "slot__button_hsi_nav1_fired"
QT_MOC_LITERAL(13, 256, 27), // "slot__button_hsi_nav2_fired"
QT_MOC_LITERAL(14, 284, 33), // "slot__button_autopilot_mode_f..."
QT_MOC_LITERAL(15, 318, 27), // "slot__button_altitude_fired"
QT_MOC_LITERAL(16, 346, 26), // "slot__button_heading_fired"
QT_MOC_LITERAL(17, 373, 36), // "slot__button_vertical_velocit..."
QT_MOC_LITERAL(18, 410, 27), // "slot__button_approach_fired"
QT_MOC_LITERAL(19, 438, 28), // "slot__button_air_speed_fired"
QT_MOC_LITERAL(20, 467, 23), // "slot__button_hnav_fired"
QT_MOC_LITERAL(21, 491, 16), // "slot__speed_down"
QT_MOC_LITERAL(22, 508, 18), // "slot__speed_center"
QT_MOC_LITERAL(23, 527, 14), // "slot__speed_up"
QT_MOC_LITERAL(24, 542, 18), // "slot__heading_left"
QT_MOC_LITERAL(25, 561, 19), // "slot__heading_right"
QT_MOC_LITERAL(26, 581, 20), // "slot__heading_center"
QT_MOC_LITERAL(27, 602, 19), // "slot__altitude_down"
QT_MOC_LITERAL(28, 622, 21), // "slot__altitude_center"
QT_MOC_LITERAL(29, 644, 17), // "slot__altitude_up"
QT_MOC_LITERAL(30, 662, 28), // "slot__vertical_velocity_down"
QT_MOC_LITERAL(31, 691, 30), // "slot__vertical_velocity_center"
QT_MOC_LITERAL(32, 722, 26) // "slot__vertical_velocity_up"

    },
    "AutopilotControlWidget\0signal__need_send\0"
    "\0CommandSet&\0cmd\0slot__xplane_connected\0"
    "slot__xplane_disconnected\0"
    "slot__got_autopilot_state\0"
    "CommandAutopilotState&\0"
    "slot__got_user_aircraft_state\0"
    "AircraftState&\0slot__button_hsi_gps_fired\0"
    "slot__button_hsi_nav1_fired\0"
    "slot__button_hsi_nav2_fired\0"
    "slot__button_autopilot_mode_fired\0"
    "slot__button_altitude_fired\0"
    "slot__button_heading_fired\0"
    "slot__button_vertical_velocity_fired\0"
    "slot__button_approach_fired\0"
    "slot__button_air_speed_fired\0"
    "slot__button_hnav_fired\0slot__speed_down\0"
    "slot__speed_center\0slot__speed_up\0"
    "slot__heading_left\0slot__heading_right\0"
    "slot__heading_center\0slot__altitude_down\0"
    "slot__altitude_center\0slot__altitude_up\0"
    "slot__vertical_velocity_down\0"
    "slot__vertical_velocity_center\0"
    "slot__vertical_velocity_up"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AutopilotControlWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  149,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,  152,    2, 0x0a /* Public */,
       6,    0,  153,    2, 0x0a /* Public */,
       7,    1,  154,    2, 0x0a /* Public */,
       9,    1,  157,    2, 0x0a /* Public */,
      11,    0,  160,    2, 0x08 /* Private */,
      12,    0,  161,    2, 0x08 /* Private */,
      13,    0,  162,    2, 0x08 /* Private */,
      14,    0,  163,    2, 0x08 /* Private */,
      15,    0,  164,    2, 0x08 /* Private */,
      16,    0,  165,    2, 0x08 /* Private */,
      17,    0,  166,    2, 0x08 /* Private */,
      18,    0,  167,    2, 0x08 /* Private */,
      19,    0,  168,    2, 0x08 /* Private */,
      20,    0,  169,    2, 0x08 /* Private */,
      21,    0,  170,    2, 0x08 /* Private */,
      22,    0,  171,    2, 0x08 /* Private */,
      23,    0,  172,    2, 0x08 /* Private */,
      24,    0,  173,    2, 0x08 /* Private */,
      25,    0,  174,    2, 0x08 /* Private */,
      26,    0,  175,    2, 0x08 /* Private */,
      27,    0,  176,    2, 0x08 /* Private */,
      28,    0,  177,    2, 0x08 /* Private */,
      29,    0,  178,    2, 0x08 /* Private */,
      30,    0,  179,    2, 0x08 /* Private */,
      31,    0,  180,    2, 0x08 /* Private */,
      32,    0,  181,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    2,
    QMetaType::Void, 0x80000000 | 10,    2,
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
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void AutopilotControlWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AutopilotControlWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signal__need_send((*reinterpret_cast< CommandSet(*)>(_a[1]))); break;
        case 1: _t->slot__xplane_connected(); break;
        case 2: _t->slot__xplane_disconnected(); break;
        case 3: _t->slot__got_autopilot_state((*reinterpret_cast< CommandAutopilotState(*)>(_a[1]))); break;
        case 4: _t->slot__got_user_aircraft_state((*reinterpret_cast< AircraftState(*)>(_a[1]))); break;
        case 5: _t->slot__button_hsi_gps_fired(); break;
        case 6: _t->slot__button_hsi_nav1_fired(); break;
        case 7: _t->slot__button_hsi_nav2_fired(); break;
        case 8: _t->slot__button_autopilot_mode_fired(); break;
        case 9: _t->slot__button_altitude_fired(); break;
        case 10: _t->slot__button_heading_fired(); break;
        case 11: _t->slot__button_vertical_velocity_fired(); break;
        case 12: _t->slot__button_approach_fired(); break;
        case 13: _t->slot__button_air_speed_fired(); break;
        case 14: _t->slot__button_hnav_fired(); break;
        case 15: _t->slot__speed_down(); break;
        case 16: _t->slot__speed_center(); break;
        case 17: _t->slot__speed_up(); break;
        case 18: _t->slot__heading_left(); break;
        case 19: _t->slot__heading_right(); break;
        case 20: _t->slot__heading_center(); break;
        case 21: _t->slot__altitude_down(); break;
        case 22: _t->slot__altitude_center(); break;
        case 23: _t->slot__altitude_up(); break;
        case 24: _t->slot__vertical_velocity_down(); break;
        case 25: _t->slot__vertical_velocity_center(); break;
        case 26: _t->slot__vertical_velocity_up(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AutopilotControlWidget::*)(CommandSet & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AutopilotControlWidget::signal__need_send)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject AutopilotControlWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_AutopilotControlWidget.data,
    qt_meta_data_AutopilotControlWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AutopilotControlWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AutopilotControlWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AutopilotControlWidget.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui::ControlWidget"))
        return static_cast< Ui::ControlWidget*>(this);
    return QWidget::qt_metacast(_clname);
}

int AutopilotControlWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 27)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 27;
    }
    return _id;
}

// SIGNAL 0
void AutopilotControlWidget::signal__need_send(CommandSet & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
