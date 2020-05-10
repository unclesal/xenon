// *********************************************************************************************************************
// *                                           Интегрированный виджит управления.                                      *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 sep 2019 at 21:26 *
// *********************************************************************************************************************
#pragma once

#include <QWidget>
#include <QPalette>
#include <QColor>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "UserInterface.h"
#include "ui_control_widget.h"
#include "LCDWithButtons.h"
#include "RepeatableButton.h"

#include "CommandAutopilotState.h"
#include "CommandSet.h"
#include "AircraftState.h"

using namespace xenon;
class AutopilotControlWidget: public QWidget, public Ui::ControlWidget {

    Q_OBJECT
    signals:

        void signal__need_send(CommandSet & cmd);

    public:

        AutopilotControlWidget( QWidget * parent = nullptr );
        virtual ~AutopilotControlWidget() override = default;

        bool is_autopilot_on();
        bool is_controlled( int flag1, int flag2=0 );
        bool is_autothrottle_manual();
        bool is_autothrottle_airspeed_hold();

        void set__one_flag( int flag );
        void set__vvi_climb_engaged();
        void set__autopilot_mode_on();
        void set__autopilot_mode_off();
        void set__air_speed( float value );
        void set__vertical_velocity( float value );
        void set__heading_engaged();
        void set__autothrottle_manual();
        void set__autothrottle_airspeed_hold();

    public slots:

        void slot__xplane_connected();
        void slot__xplane_disconnected();
        void slot__got_autopilot_state( CommandAutopilotState & );
        void slot__got_user_aircraft_state( AircraftState & );

    protected:
    private:

        LCDWithButtons * __heading;
        LCDWithButtons * __speed;
        LCDWithButtons * __vertical_velocity;
        LCDWithButtons * __altitude;

        RepeatableButton * __button_hsi_gps;
        RepeatableButton * __button_hsi_nav1;
        RepeatableButton * __button_hsi_nav2;

        RepeatableButton * __button_autopilot_mode;

        RepeatableButton * __button_air_speed;
        RepeatableButton * __button_heading;

        RepeatableButton * __button_altitude;
        RepeatableButton * __button_vertical_velocity;

        // RepeatableButton * __button_gps;
        RepeatableButton * __button_hnav;

        RepeatableButton * __button_vnav;
        RepeatableButton * __button_approach;

        bool __connected;

        CommandAutopilotState __ap;
        AircraftState __user_aircraft_state;

        // bool __wait_hsi_selector;

        // Отображение всяких-разных состояний - по данным автопилота.
        void __display__autopilot_mode();
        void __display__air_speed_mode();
        void __display__heading_mode();
        void __display__altitude_mode();
        void __display__vertical_velocity_mode();
        void __display__hnav_mode();
        void __display__approach_mode();

        // Отображение всяких-разных состояний - по данным пользовательского самолета.
        void __display__hsi_mode();

        // void __reswitch_nav_mode();
        // void __switch_off_nav_mode();

        // Сравнение между собой желаемых данных и фактических
        // с целью установки цветов LCD индикаторов.
        void __compare_values();

    private slots:

        void slot__button_hsi_gps_fired();
        void slot__button_hsi_nav1_fired();
        void slot__button_hsi_nav2_fired();

        void slot__button_autopilot_mode_fired();
        void slot__button_altitude_fired();
        void slot__button_heading_fired();
        void slot__button_vertical_velocity_fired();
        void slot__button_approach_fired();
        void slot__button_air_speed_fired();
        // void slot__button_gps_fired();
        void slot__button_hnav_fired();

        // Реакция на кнопки установки значения скорости автопилота.
        void slot__speed_down();
        void slot__speed_center();
        void slot__speed_up();

        // Реакция на кнопки виджита установки курса автопилота.
        void slot__heading_left();
        void slot__heading_right();
        void slot__heading_center();

        // Реакция на кнопки виджита установки высоты автопилота.
        void slot__altitude_down();
        void slot__altitude_center();
        void slot__altitude_up();

        // Реакция на кнопки виджита установки вертикальной скорости автопилота.
        void slot__vertical_velocity_down();
        void slot__vertical_velocity_center();
        void slot__vertical_velocity_up();

};
