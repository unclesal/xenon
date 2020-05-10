// *********************************************************************************************************************
// *                                           Интегрированный виджит управления.                                      *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 sep 2019 at 21:28 *
// *********************************************************************************************************************

#include "AutopilotControlWidget.h"
using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор.                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

AutopilotControlWidget::AutopilotControlWidget( QWidget * parent )
    : QWidget( parent )
{
    setupUi( this );

    // Пока нет соединения с симулятором, виджит запрещен.
    __connected = false;    
    // __wait_hsi_selector = false;

    setAutoFillBackground( true );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    // Виджит установки скорости.
    __speed = new LCDWithButtons( this );
    __speed->lcd->setDigitCount(3);
    __speed->lcd->display("000");
    __speed->button_center->setIconable( true );
    __speed->button_center->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    __speed->button_center->label->setText("MPH");
    __speed->setStep( 1 );

    QObject::connect( __speed->button_down, SIGNAL( signal__fired() ), this, SLOT( slot__speed_down() ) );
    QObject::connect( __speed->button_center, SIGNAL( signal__fired() ), this, SLOT( slot__speed_center() ) );
    QObject::connect( __speed->button_up, SIGNAL( signal__fired() ), this, SLOT( slot__speed_up() ) );

    this->speed_layout->addWidget( __speed );

    // Виджит установки курса.
    __heading = new LCDWithButtons( this );
    __heading->lcd->setDigitCount(3);
    __heading->lcd->display("000");
    __heading->button_center->setIconable( true );
    __heading->button_center->label->setText("HDG");
    __heading->button_center->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    __heading->setStep( 1 );
    QObject::connect( __heading->button_down, SIGNAL( signal__fired() ), this, SLOT(slot__heading_left()) );
    QObject::connect( __heading->button_center, SIGNAL( signal__fired() ), this, SLOT(slot__heading_center()) );
    QObject::connect( __heading->button_up, SIGNAL( signal__fired() ), this, SLOT( slot__heading_right() ) );
    this->heading_layout->addWidget( __heading );

    // Виджит установки вертикальной скорости.
    __vertical_velocity = new LCDWithButtons( this );
    __vertical_velocity->lcd->setDigitCount(5);
    __vertical_velocity->lcd->display("00000");
    __vertical_velocity->button_center->setIconable( true );
    __vertical_velocity->button_center->label->setText("VER");
    __vertical_velocity->button_center->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    __vertical_velocity->setStep( 50 );

    QObject::connect( __vertical_velocity->button_down, SIGNAL( signal__fired() ), this, SLOT(slot__vertical_velocity_down()) );
    QObject::connect( __vertical_velocity->button_center, SIGNAL( signal__fired() ), this, SLOT(slot__vertical_velocity_center()) );
    QObject::connect( __vertical_velocity->button_up, SIGNAL( signal__fired() ), this, SLOT( slot__vertical_velocity_up() ) );

    this->vertical_velocity_layout->addWidget( __vertical_velocity );

    // Виджит установки высоты.
    __altitude = new LCDWithButtons( this );
    __altitude->lcd->display("00000");
    __altitude->button_center->setIconable( true );
    __altitude->button_center->label->setText("ALT");
    __altitude->button_center->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    __altitude->setStep( 100 );

    QObject::connect( __altitude->button_down, SIGNAL( signal__fired() ), this, SLOT( slot__altitude_down() ) );
    QObject::connect( __altitude->button_center, SIGNAL( signal__fired() ), this, SLOT( slot__altitude_center() ) );
    QObject::connect( __altitude->button_up, SIGNAL( signal__fired() ), this, SLOT( slot__altitude_up() ) );
    this->altitude_layout->addWidget( __altitude );

    this->top_layout->setStretch( 0, 3 );
    this->top_layout->setStretch( 1, 4 );

    this->bottom_layout->setStretch( 0, 3 );
    this->bottom_layout->setStretch( 1, 4 );

    // --------------------------------------------------------------------------------------------
    //
    //                                 Кнопки в правой части виджита
    //
    // --------------------------------------------------------------------------------------------

    right_layout->setAlignment( Qt::AlignTop );
    right_layout->setSpacing( 10 );

    // -----------------------------------------------------------
    //       Виджит с тремя кнопками, GPS / NAV1 / NAV2
    // -----------------------------------------------------------

    QWidget * hsi_selector_widget = new QWidget( this );
    QHBoxLayout * hsi_selector_layout = new QHBoxLayout( hsi_selector_widget );
    hsi_selector_layout->setMargin( 0 );
    hsi_selector_layout->setSpacing( 10 );
    hsi_selector_widget->setLayout( hsi_selector_layout );

    // Кнопка навигации по GPS.
    __button_hsi_gps = new RepeatableButton( hsi_selector_widget );
    __button_hsi_gps->setIconable( true );
    __button_hsi_gps->label->setText("GPS");
    QObject::connect( __button_hsi_gps, SIGNAL( signal__fired() ), this, SLOT(slot__button_hsi_gps_fired() ) );
    hsi_selector_layout->addWidget( __button_hsi_gps );

    // Кнопка навигации NAV1
    __button_hsi_nav1 = new RepeatableButton( hsi_selector_widget );
    __button_hsi_nav1->setIconable( true );
    __button_hsi_nav1->label->setText("NAV1");
    QObject::connect( __button_hsi_nav1, SIGNAL( signal__fired()), this, SLOT( slot__button_hsi_nav1_fired() ));
    hsi_selector_layout->addWidget( __button_hsi_nav1 );

    // Кнопка навигации NAV2
    __button_hsi_nav2 = new RepeatableButton( hsi_selector_widget );
    __button_hsi_nav2->setIconable( true );
    __button_hsi_nav2->label->setText("NAV2");
    QObject::connect( __button_hsi_nav2, SIGNAL( signal__fired()), this, SLOT( slot__button_hsi_nav2_fired() ));
    hsi_selector_layout->addWidget( __button_hsi_nav2 );

    right_layout->addWidget( hsi_selector_widget );
    right_layout->setStretch(0, 2);

    // -----------------------------------------------------------
    //              Кнопка автопилота (одна и большая).
    // -----------------------------------------------------------

    __button_autopilot_mode = new RepeatableButton( this );
    __button_autopilot_mode->label->setText("AP");
    __button_autopilot_mode->setIconable( true );
    __button_autopilot_mode->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    QObject::connect( __button_autopilot_mode, SIGNAL( signal__fired() ), this, SLOT( slot__button_autopilot_mode_fired() ));
    right_layout->addWidget( __button_autopilot_mode );
    right_layout->setStretch( 1, 3 );

    // -----------------------------------------------------------
    //  Виджит с двумя кнопками, высота и вертикальная скорость.
    // -----------------------------------------------------------

    QWidget * alt_and_velocity_widget = new QWidget( this );
    QHBoxLayout * alt_and_velocity_layout = new QHBoxLayout( alt_and_velocity_widget );
    alt_and_velocity_layout->setMargin( 0 );
    alt_and_velocity_layout->setSpacing( 10 );
    alt_and_velocity_layout->setAlignment( Qt::AlignCenter );

    __button_altitude = new RepeatableButton( alt_and_velocity_widget );
    __button_altitude->label->setText("ALT");
    __button_altitude->setIconable( true );
    __button_altitude->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    QObject::connect( __button_altitude, SIGNAL( signal__fired() ), this, SLOT(slot__button_altitude_fired()));
    alt_and_velocity_layout->addWidget( __button_altitude );

    __button_vertical_velocity = new RepeatableButton( alt_and_velocity_widget );
    __button_vertical_velocity->label->setText("VER");
    __button_vertical_velocity->setIconable( true );
    __button_vertical_velocity->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    QObject::connect( __button_vertical_velocity, SIGNAL( signal__fired() ), this, SLOT( slot__button_vertical_velocity_fired() ) );
    alt_and_velocity_layout->addWidget( __button_vertical_velocity );

    right_layout->addWidget( alt_and_velocity_widget );
    right_layout->setStretch(2, 2);


    // -------------------------------------------------------------
    //            Виджит с двумя кнопками, скорость и курс.
    // -------------------------------------------------------------

    QWidget * speed_and_heading_widget = new QWidget( this );
    QHBoxLayout * speed_and_heading_layout = new QHBoxLayout( speed_and_heading_widget );
    speed_and_heading_layout->setMargin( 0 );
    speed_and_heading_layout->setSpacing( 10 );
    speed_and_heading_layout->setAlignment( Qt::AlignCenter );
    speed_and_heading_widget->setLayout( speed_and_heading_layout );

    __button_air_speed = new RepeatableButton( speed_and_heading_widget );
    __button_air_speed->setIconable( true );
    __button_air_speed->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    __button_air_speed->label->setText("MPH");
    QObject::connect( __button_air_speed, SIGNAL( signal__fired() ), this, SLOT( slot__button_air_speed_fired() ));
    speed_and_heading_layout->addWidget( __button_air_speed );

    __button_heading = new RepeatableButton( speed_and_heading_widget );
    __button_heading->setIconable( true );
    __button_heading->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    __button_heading->label->setText("HDG");
    QObject::connect( __button_heading, SIGNAL( signal__fired() ), this, SLOT( slot__button_heading_fired() ));
    speed_and_heading_layout->addWidget( __button_heading );

    right_layout->addWidget( speed_and_heading_widget );
    right_layout->setStretch(3, 2);

    // ------------------------------------------------------------
    //   Виджит с двумя кнопками, VNAV и HNAV
    // ------------------------------------------------------------

    QWidget * hnav_vnav_widget = new QWidget( this );
    QHBoxLayout * hnav_vnav_layout = new QHBoxLayout( hnav_vnav_widget );
    hnav_vnav_layout->setMargin( 0 );
    hnav_vnav_layout->setSpacing( 10 );
    hnav_vnav_widget->setLayout( hnav_vnav_layout );

    __button_vnav = new RepeatableButton( hnav_vnav_widget );
    __button_vnav->label->setText("VNAV");
    __button_vnav->setIconable( true );
    __button_vnav->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    hnav_vnav_layout->addWidget( __button_vnav );


//    __button_gps = new RepeatableButton( gps_nav_widget );
//    __button_gps->setIconable( true );
//    __button_gps->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
//    __button_gps->label->setText("GPS");
//    QObject::connect( __button_gps, SIGNAL( signal__fired() ), this, SLOT( slot__button_gps_fired() ));
//    gps_nav_layout->addWidget( __button_gps );

    __button_hnav = new RepeatableButton( hnav_vnav_widget );
    __button_hnav->label->setText("HNAV");
    __button_hnav->setIconable( true );
    __button_hnav->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    QObject::connect( __button_hnav, SIGNAL( signal__fired() ), this, SLOT( slot__button_hnav_fired() ) );
    hnav_vnav_layout->addWidget( __button_hnav );

    right_layout->addWidget( hnav_vnav_widget );
    right_layout->setStretch( 4, 2 );

    // ---------------------------------------------------------
    //         Виджит с двумя кнопками,  и APPROACH
    // ---------------------------------------------------------

    QWidget * vnav_approach_widget = new QWidget( this );
    QHBoxLayout * vnav_approach_layout = new QHBoxLayout( vnav_approach_widget );
    vnav_approach_layout->setMargin( 0 );
    vnav_approach_layout->setSpacing( 10 );
    vnav_approach_widget->setLayout( vnav_approach_layout );

    RepeatableButton * empty = new RepeatableButton( vnav_approach_widget );
    vnav_approach_layout->addWidget( empty );
    empty->disable();

    __button_approach = new RepeatableButton( vnav_approach_widget );
    __button_approach->label->setText("APCH");
    __button_approach->setIconable( true );
    __button_approach->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    QObject::connect( __button_approach, SIGNAL( signal__fired() ), this, SLOT( slot__button_approach_fired() ));
    vnav_approach_layout->addWidget( __button_approach );

    right_layout->addWidget( vnav_approach_widget );
    right_layout->setStretch( 5, 2 );

    slot__xplane_disconnected();

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *               Контролируется ли данный параметр автопилотом? Флаги имеются в виду ARMED, ENGAGED                  *
// *                                                                                                                   *
// *********************************************************************************************************************

bool AutopilotControlWidget::is_controlled(int flag1, int flag2) {
    return (
        ( is_autopilot_on() )
        &&
            (
                ( __ap.autopilot_state() & flag1 )
                || ( __ap.autopilot_state() & flag2 )
            )
    );
}
// *********************************************************************************************************************
// *                                                                                                                   *
// *               Сравнение между собой фактических и желаемых значений, установка цвета LCD-индикаторов              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::__compare_values() {

    // Для начала все индикаторы - в серый цвет.
    __heading->setColor( LCD_ENABLED_GRAY_COLOR );
    __speed->setColor( LCD_ENABLED_GRAY_COLOR );
    __vertical_velocity->setColor( LCD_ENABLED_GRAY_COLOR );
    __altitude->setColor( LCD_ENABLED_GRAY_COLOR );

    // Курс. 1% = 360 / 100 = 3.6 градуса

    float delta = 0.0;
    float ap_heading = __ap.heading();
    float fact_heading = __user_aircraft_state.heading();
    if ( ap_heading > fact_heading ) delta=ap_heading - fact_heading;
    else delta = fact_heading - ap_heading;

    if ( delta < (float) 3.6 ) {
        // Менее 1 %, Cyan.
        if ( is_controlled( AUTOPILOT_STATE_FLAG__HEADING_HOLD_ENGAGE ))
            __heading->setColor( REPEATABLE_BUTTON_ENABLED_CYAN_INDICATOR_COLOR );
        else __heading->setColor( REPEATABLE_BUTTON_ENABLED_DARK_CYAN_INDICATOR_COLOR );

    } else if ( delta < (float) 3.6 * (float)5.0 ) {
        // Менее 5%, желтый цвет.
        if ( is_controlled( AUTOPILOT_STATE_FLAG__HEADING_HOLD_ENGAGE ))
            __heading->setColor( REPEATABLE_BUTTON_ENABLED_YELLOW_INDICATOR_COLOR );
        else
            __heading->setColor( REPEATABLE_BUTTON_ENABLED_DARK_YELLOW_INDICATOR_COLOR );
    }

    // Воздушная скорость.

    // Высота.

    float awanted = __user_aircraft_state.altitude();
    float areal = __ap.altitude();
    if ( ( awanted > 0.0 ) && ( areal > 0.0 ) ) {
        delta = 100.0 * ( areal - awanted ) / awanted;
        if ( delta < 1.0) {
            // Менее 1%, cyan.
            if ( is_controlled( AUTOPILOT_STATE_FLAG__ALTITUDE_HOLD_ARMED, AUTOPILOT_STATE_FLAG__ALTITUDE_HOLD_ENGAGED ))
                __altitude->setColor( REPEATABLE_BUTTON_ENABLED_CYAN_INDICATOR_COLOR );
            else __altitude->setColor( REPEATABLE_BUTTON_ENABLED_DARK_CYAN_INDICATOR_COLOR );
        } else if ( delta < 5.0 ) {
            // Менее 5%, желтый
            if ( is_controlled( AUTOPILOT_STATE_FLAG__ALTITUDE_HOLD_ARMED, AUTOPILOT_STATE_FLAG__ALTITUDE_HOLD_ENGAGED ))
                __altitude->setColor( REPEATABLE_BUTTON_ENABLED_YELLOW_INDICATOR_COLOR );
            else __altitude->setColor( REPEATABLE_BUTTON_ENABLED_DARK_YELLOW_INDICATOR_COLOR );
        }
    }


}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Соединение с X-Plane                                                *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__xplane_connected() {

    __connected = true;

    QPalette widget_palette( palette() );
    widget_palette.setColor( QPalette::Background, ENABLED_BACKGROUND );
    widget_palette.setColor( QPalette::Foreground, ENABLED_COLOR );
    setPalette( widget_palette );

    __speed->enable();
    __heading->enable();
    __vertical_velocity->enable();
    __altitude->enable();

    __button_hsi_gps->enable();
    __button_hsi_nav1->enable();
    __button_hsi_nav2->enable();

    __button_autopilot_mode->enable();
    __button_altitude->enable();
    __button_vertical_velocity->enable();
    __button_air_speed->enable();
    __button_heading->enable();
    // __button_gps->enable();
    __button_hnav->enable();
    __button_vnav->enable();
    __button_approach->enable();

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Рассоединение с X-Plane                                               *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__xplane_disconnected() {

    __connected = false;

    QPalette widget_palette( palette() );
    widget_palette.setColor( QPalette::Background, DISABLED_BACKGROUND );
    widget_palette.setColor( QPalette::Foreground, DISABLED_COLOR );
    setPalette( widget_palette );

    __speed->disable();
    __heading->disable();
    __vertical_velocity->disable();
    __altitude->disable();

    __button_hsi_gps->disable();
    __button_hsi_nav1->disable();
    __button_hsi_nav2->disable();

    __button_autopilot_mode->disable();
    __button_altitude->disable();
    __button_vertical_velocity->disable();
    __button_air_speed->disable();
    __button_heading->disable();
    // __button_gps->disable();
    __button_hnav->disable();
    __button_vnav->disable();
    __button_approach->disable();

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Включен ли автопилот?                                               *
// *                                                                                                                   *
// *********************************************************************************************************************

inline bool AutopilotControlWidget::is_autopilot_on() {
    return ( __ap.autopilot_mode() == CommandAutopilotState::AP_MODE_ON );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Включить автопилот                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

inline void AutopilotControlWidget::set__autopilot_mode_on() {
    if ( __ap.autopilot_mode() != CommandAutopilotState::AP_MODE_ON ) {
        CommandSet cmd( CommandSet::SET_AUTOPILOT_MODE, static_cast<int>( CommandAutopilotState::AP_MODE_ON ), 0.0 );
        emit signal__need_send( cmd );
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Выключить автопилот                                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::set__autopilot_mode_off() {

    if ( __ap.autopilot_mode() != CommandAutopilotState::AP_MODE_OFF ) {

        CommandSet cmd( CommandSet::SET_AUTOPILOT_MODE, static_cast<int>( CommandAutopilotState::AP_MODE_OFF ), 0.0 );
        emit signal__need_send( cmd );

        // Если при этом включена тяга, то ставим ее на ручную
        if ( is_autothrottle_airspeed_hold() ) set__autothrottle_manual();

    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Установлена ли тяга в ручной режим?                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

inline bool AutopilotControlWidget::is_autothrottle_manual() {
    return ( __ap.autothrottle() == CommandAutopilotState::THROTTLE_SERVOS );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                            Установлена ли тяга в режим удержания воздушной скорости?                              *
// *                                                                                                                   *
// *********************************************************************************************************************

inline bool AutopilotControlWidget::is_autothrottle_airspeed_hold() {
    return ( __ap.autothrottle() == CommandAutopilotState::THROTTLE_AIRSPEED_HOLD );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *              Установка одного флага в состоянии автопилота, при условии, что он еще не был установлен             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::set__one_flag( int flag ) {

    // Повторение установки того же самого флага, если он уже установлен,
    // приводит к его сбросу. Фишка автопилота. Поэтому наличие этого флага
    // предварительно - не проверяется, пусть ходит по циклу.

    CommandSet cmd( CommandSet::SET_AUTOPILOT_STATE_FLAGS, flag, 0.0 );
    emit signal__need_send( cmd );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                  Установка режима удержания вертикальной скорости, если он еще не установлен.                     *
// *                                                                                                                   *
// *********************************************************************************************************************

inline void AutopilotControlWidget::set__vvi_climb_engaged() {
    set__one_flag( AUTOPILOT_STATE_FLAG__VVI_CLIMB_ENGAGE );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                         Установка значения задатчика вертикальной скорости автопилота                             *
// *                                                                                                                   *
// *********************************************************************************************************************

inline void AutopilotControlWidget::set__vertical_velocity(float value) {
    CommandSet cmd( CommandSet::SET_AUTOPILOT_VERTICAL_VELOCITY, 0, value );
    emit signal__need_send( cmd );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Установка значения задатчика воздушной скорости                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

inline void AutopilotControlWidget::set__air_speed(float value) {

    CommandSet cmd(CommandSet::SET_AUTOPILOT_AIR_SPEED, 0, value );
    emit signal__need_send( cmd );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                           Установка режима удержания курса, если он еще не установлен.                            *
// *                                                                                                                   *
// *********************************************************************************************************************

inline void AutopilotControlWidget::set__heading_engaged() {
    set__one_flag( AUTOPILOT_STATE_FLAG__HEADING_HOLD_ENGAGE );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                    Установка режима ручного управления тягой                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

inline void AutopilotControlWidget::set__autothrottle_manual() {
    if ( ! is_autothrottle_manual() ) {
        CommandSet cmd(CommandSet::SET_AUTOTHROTTLE, static_cast<int>(CommandAutopilotState::THROTTLE_SERVOS), 0.0 );
        emit signal__need_send( cmd );
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   Установка режима удержания воздушной скорости                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

inline void AutopilotControlWidget::set__autothrottle_airspeed_hold() {
    if ( ! is_autothrottle_airspeed_hold() ) {
        CommandSet cmd(CommandSet::SET_AUTOTHROTTLE, static_cast<int>(CommandAutopilotState::THROTTLE_AIRSPEED_HOLD), 0.0 );
        emit signal__need_send( cmd );
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                           Отображение кнопки автопилота в зависимости от режима его работы.                       *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::__display__autopilot_mode() {

    switch (__ap.autopilot_mode()) {
        case CommandAutopilotState::AP_MODE_NONE:
        case CommandAutopilotState::AP_MODE_OFF:
            __button_autopilot_mode->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
            break;

        case CommandAutopilotState::AP_MODE_FLIGHT_DIR:
            __button_autopilot_mode->setColor( REPEATABLE_BUTTON_ENABLED_YELLOW_INDICATOR_COLOR );
            break;

        case CommandAutopilotState::AP_MODE_ON:
            __button_autopilot_mode->setColor( REPEATABLE_BUTTON_ENABLED_GREEN_INDICATOR_COLOR );
            break;
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                      Отображение кнопок в зависимости от режима удержания воздушной скорости                      *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::__display__air_speed_mode() {

    if ( __ap.autothrottle_on() ) {
        // Зеленый цвет
        if ( is_autopilot_on() ) {
            __button_air_speed->setColor( REPEATABLE_BUTTON_ENABLED_GREEN_INDICATOR_COLOR );
            __speed->button_center->setColor( REPEATABLE_BUTTON_ENABLED_GREEN_INDICATOR_COLOR );
        } else {
            __button_air_speed->setColor( REPEATABLE_BUTTON_ENABLED_DARK_GREEN_INDICATOR_COLOR );
            __speed->button_center->setColor( REPEATABLE_BUTTON_ENABLED_DARK_GREEN_INDICATOR_COLOR );
        }
    } else {
        // Регулировка скорости не активна, серый цвет.
        __button_air_speed->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
        __speed->button_center->setColor(  REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR  );
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                             Отображение кнопок в зависимости от режима удержания курса                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::__display__heading_mode() {

    if ( __ap.autopilot_state() & AUTOPILOT_STATE_FLAG__HEADING_HOLD_ENGAGE ) {
        // Курс - включен. Зеленый индикатор.
        if ( is_autopilot_on() ) {
            // Если автопилот включен, то ярко-зеленый индикатор.
            __button_heading->setColor( REPEATABLE_BUTTON_ENABLED_GREEN_INDICATOR_COLOR );
            __heading->button_center->setColor( REPEATABLE_BUTTON_ENABLED_GREEN_INDICATOR_COLOR );
        } else {
            // Если автопилот не включен, то темно-зеленый индикатор.
            __button_heading->setColor( REPEATABLE_BUTTON_ENABLED_DARK_GREEN_INDICATOR_COLOR );
            __heading->button_center->setColor( REPEATABLE_BUTTON_ENABLED_DARK_GREEN_INDICATOR_COLOR );
        }
    } else {
        // Курс - не в режиме engaged.
        __button_heading->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
        __heading->button_center->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                     Отображение состояния кнопок в зависимости от режима удержания высоты                         *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::__display__altitude_mode() {


    if ( __ap.autopilot_state() & AUTOPILOT_STATE_FLAG__ALTITUDE_HOLD_ARMED ) {
        // Высота - в ARM режиме.
        if ( is_autopilot_on() ) {
            // Если автопилот включен, то ярко-желтый индикатор.
            __altitude->button_center->setColor( REPEATABLE_BUTTON_ENABLED_YELLOW_INDICATOR_COLOR );
            __button_altitude->setColor( REPEATABLE_BUTTON_ENABLED_YELLOW_INDICATOR_COLOR );
        } else {
            // Если автопилот выключен, то темно-желтый индикатор.
            __altitude->button_center->setColor( REPEATABLE_BUTTON_ENABLED_DARK_YELLOW_INDICATOR_COLOR );
            __button_altitude->setColor( REPEATABLE_BUTTON_ENABLED_DARK_YELLOW_INDICATOR_COLOR );
        }

    } else if ( __ap.autopilot_state() & AUTOPILOT_STATE_FLAG__ALTITUDE_HOLD_ENGAGED ) {
        // Удержание высоты в режиме "включено".
        if ( is_autopilot_on() ) {
            // Если автопилот включен, то ярко-зеленый индикатор.
            __altitude->button_center->setColor( REPEATABLE_BUTTON_ENABLED_GREEN_INDICATOR_COLOR );
            __button_altitude->setColor( REPEATABLE_BUTTON_ENABLED_GREEN_INDICATOR_COLOR );
        } else {
            // Если автопилот выключен, то темно-зеленый индикатор.
            __altitude->button_center->setColor( REPEATABLE_BUTTON_ENABLED_DARK_GREEN_INDICATOR_COLOR );
            __button_altitude->setColor( REPEATABLE_BUTTON_ENABLED_DARK_GREEN_INDICATOR_COLOR );
        }
    } else {
        __altitude->button_center->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
        __button_altitude->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    }


}

// *********************************************************************************************************************
// *                                                                                                                   *
// *      Отображение состояния кнопок в зависимости от установленного режима удержания вертикальной скорости          *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::__display__vertical_velocity_mode() {

    if ( __ap.autopilot_state() & AUTOPILOT_STATE_FLAG__VVI_CLIMB_ENGAGE ) {
        // Включено.
        if ( is_autopilot_on() ) {
            __button_vertical_velocity->setColor( REPEATABLE_BUTTON_ENABLED_GREEN_INDICATOR_COLOR );
            __vertical_velocity->button_center->setColor( REPEATABLE_BUTTON_ENABLED_GREEN_INDICATOR_COLOR );
        } else {
            __button_vertical_velocity->setColor( REPEATABLE_BUTTON_ENABLED_DARK_GREEN_INDICATOR_COLOR );
            __vertical_velocity->button_center->setColor( REPEATABLE_BUTTON_ENABLED_DARK_GREEN_INDICATOR_COLOR );
        }
    } else {
        // Выключено.
        __button_vertical_velocity->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
        __vertical_velocity->button_center->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                           Отображение состояния кнопки горизонтальной навигации HNAV                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::__display__hnav_mode() {
    if ( __ap.autopilot_state() & AUTOPILOT_STATE_FLAG__HNAV_ARMED ) {
        // Желтый цвет.
        if ( is_autopilot_on() ) {
            // Ярко-желтый цвет.
            __button_hnav->setColor( REPEATABLE_BUTTON_ENABLED_YELLOW_INDICATOR_COLOR );
        } else {
            // Темно-желтый цвет.
            __button_hnav->setColor( REPEATABLE_BUTTON_ENABLED_DARK_YELLOW_INDICATOR_COLOR );
        }
    } else if ( __ap.autopilot_state() & AUTOPILOT_STATE_FLAG__HNAV_ENGAGED ) {
        // Зеленый цвет.
        if ( is_autopilot_on() ) {
            // Ярко-зеленый цвет
            __button_hnav->setColor( REPEATABLE_BUTTON_ENABLED_GREEN_INDICATOR_COLOR );
        } else {
            // Темно-зеленый цвет.
            __button_hnav->setColor( REPEATABLE_BUTTON_ENABLED_DARK_GREEN_INDICATOR_COLOR );
        }
    } else {
        // Серый цвет.
        __button_hnav->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *            Отображение состояния кнопок горизонтальной навигации в зависимости от установленного режима.          *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::__display__hsi_mode() {

    // Сначала устанавливаем все три кнопки - серого цвета.
    __button_hsi_gps->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    __button_hsi_nav1->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    __button_hsi_nav2->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );

    // Теперь одну из них делаем cyan, в зависимости от установленного режима.

    switch ( __user_aircraft_state.hsi_selector() ) {

        case AircraftState::HSI_UNKNOWN:
        break;

        case AircraftState::HSI_NAV_1:
            __button_hsi_nav1->setColor( REPEATABLE_BUTTON_ENABLED_CYAN_INDICATOR_COLOR );
        break;

        case AircraftState::HSI_NAV_2:
            __button_hsi_nav2->setColor( REPEATABLE_BUTTON_ENABLED_CYAN_INDICATOR_COLOR );
        break;

        case AircraftState::HSI_GPS:
            __button_hsi_gps->setColor( REPEATABLE_BUTTON_ENABLED_CYAN_INDICATOR_COLOR );
        break;
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *           Отображение состояния кнопок в зависимости от установленного режима удержания глиссады.                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::__display__approach_mode() {

    if ( __ap.autopilot_state() & AUTOPILOT_STATE_FLAG__GLIDESLOPE_ARMED ) {
        // "Взведенное" состояние, желтый цвет.
        if ( is_autopilot_on() ) {
            // Ярко-желтый цвет.
            __button_approach->setColor( REPEATABLE_BUTTON_ENABLED_YELLOW_INDICATOR_COLOR );
        } else {
            // Темно-желтый цвет.
            __button_approach->setColor( REPEATABLE_BUTTON_ENABLED_DARK_YELLOW_INDICATOR_COLOR );
        }
    } else if ( __ap.autopilot_state() & AUTOPILOT_STATE_FLAG__GLIDESLOPE_ENGAGED ) {
        // Включенное состояние - зеленый цвет.
        if ( is_autopilot_on() ) {
            // Ярко-зеленый цвет.
            __button_approach->setColor( REPEATABLE_BUTTON_ENABLED_GREEN_INDICATOR_COLOR );
        } else {
            // Темно-зеленый цвет.
            __button_approach->setColor( REPEATABLE_BUTTON_ENABLED_DARK_GREEN_INDICATOR_COLOR );
        }
    } else {
        // Серый цвет.
        __button_approach->setColor( REPEATABLE_BUTTON_ENABLED_GRAY_INDICATOR_COLOR );
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Было получено состояние автопилота.                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__got_autopilot_state( CommandAutopilotState & ap ) {

    __ap = ap;

    // Курс автопилота.
    float heading = ap.heading();
    int i_heading = qRound(heading);
    // Хочу курс - с предшествующими нулями.
    QString s_heading = QString::number(i_heading);
    while ( s_heading.length() < __heading->lcd->digitCount() ) s_heading = "0" + s_heading;
    __heading->lcd->display( s_heading );

    // Установленное в автопилоте значение скорости
    __speed->lcd->display( qRound( ap.airspeed() ) );

    // Установленная в автопилоте высота.
    __altitude->lcd->display( qRound( ap.altitude() ) );

    // Установленная в автопилоте вертикальная скорость
    __vertical_velocity->lcd->display( qRound( ap.vertical_velocity() ) );

    // Режим работы автопилота.    
    __display__autopilot_mode();
    // Флаги автопилота - автоматическое удержание воздушной скорости
    __display__air_speed_mode();
    // Флаги автопилота - курс.
    __display__heading_mode();
    // Флаги автопилота - удержание высоты.
    __display__altitude_mode();
    // Флаги автопилота - вертикальная скорость
    __display__vertical_velocity_mode();
    // Флаги автопилота - HNAV
    __display__hnav_mode();
    // Флаги автопилота - удержание глиссады
    __display__approach_mode();

    // Сравнение установленных и фактических значений с целью
    // установки цветов LCD-индикаторов.
    __compare_values();

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                               Нажатие на кнопку управления режимом автопилота                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__button_autopilot_mode_fired() {

    switch ( __ap.autopilot_mode() ) {

        case CommandAutopilotState::AP_MODE_NONE: break;

        case CommandAutopilotState::AP_MODE_OFF:
        case CommandAutopilotState::AP_MODE_FLIGHT_DIR:
            // Автопилот не в активном состоянии - включаем его.
            set__autopilot_mode_on();
            break;

        case CommandAutopilotState::AP_MODE_ON:
            // Автопилот включен - выключаем его.
            set__autopilot_mode_off();
            break;
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   Нажатие на кнопку удержания высоты автопилотом.                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__button_altitude_fired() {

    if ( ! is_autopilot_on() ) return;
    // Здесь по циклу ходим в ARMED-состояние. Повторная установка флага,
    // если он уже был установлен, приводит к его сбросу.
    set__one_flag(AUTOPILOT_STATE_FLAG__ALTITUDE_HOLD_ARMED);

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                     Нажатие на кнопку удержания курса автопилотом                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__button_heading_fired() {

    if ( ! is_autopilot_on() ) return;
    set__one_flag(AUTOPILOT_STATE_FLAG__HEADING_HOLD_ENGAGE);
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                 Нажатие на кнопку уменьшения значения виджита управления курсом автопилота                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__heading_left() {

    int ival = qRound( __heading->lcd->value() );
    ival -= __heading->step();
    float fval = static_cast<float>( ival );
    CommandSet cmd( CommandSet::SET_AUTOPILOT_HEADING, 0, fval );
    emit signal__need_send( cmd );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                Нажатие на кнопку принятия текущего курса виджита управления курсом автопилота                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__heading_center() {
    int current_heading = qRound( __user_aircraft_state.heading() );
    if ( is_autopilot_on() ) set__heading_engaged();
    float f_heading = static_cast<float>( current_heading );
    CommandSet cmd(CommandSet::SET_AUTOPILOT_HEADING, 0, f_heading);
    emit signal__need_send( cmd );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                   Нажатие на кнопку уменьшения значения виджита управления курсом автопилота                      *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__heading_right() {

    int ival = qRound( __heading->lcd->value() );
    ival += __heading->step();
    float fval = static_cast<float>( ival );
    CommandSet cmd( CommandSet::SET_AUTOPILOT_HEADING, 0, fval );
    emit signal__need_send( cmd );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                      Нажатие на кнопку уменьшения виджита установки высоты автопилота.                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__altitude_down() {

    int ival = qRound( __altitude->lcd->value() );

    // Округление до ближайшего по шагу значения.
    int rest = ( ival % __altitude->step() );
    if ( rest > __altitude->step() / 2.0 ) ival += ( __altitude->step() - rest);
    else ival -= rest;

    ival -= __altitude->step();
    float fval = static_cast<float>( ival );
    CommandSet cmd( CommandSet::SET_AUTOPILOT_ALTITUDE, 0, fval );
    emit signal__need_send( cmd );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                 Нажатие на кнопку фиксации текущей высоты виджита установки высоты автопилота                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__altitude_center() {
    if ( is_autopilot_on() ) {
        // Здесь просто устанавливаем высоту. Она по умолчанию будет "текущей".
        // CommandSet cmd( CommandSet::SET_AUTOPILOT_STATE_FLAGS, AUTOPILOT_STATE_FLAG__ALTITUDE_HOLD_ENGAGED, 0.0 );
        // emit signal__need_send( cmd );
        set__one_flag( AUTOPILOT_STATE_FLAG__ALTITUDE_HOLD_ENGAGED );
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                     Нажатие на кнопку увеличения виджита установки высоты автопилота                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__altitude_up() {

    int ival = qRound( __altitude->lcd->value() );

    // Округление до ближайшего по шагу значения.
    int rest = ( ival % __altitude->step() );
    if ( rest > __altitude->step() / 2.0 ) ival += ( __altitude->step() - rest );
    else ival -= rest;

    ival += __altitude->step();
    float fval = static_cast<float>( ival );
    CommandSet cmd( CommandSet::SET_AUTOPILOT_ALTITUDE, 0, fval );
    emit signal__need_send( cmd );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                    Нажатие на кнопку уменьшения значения виджита воздушной скорости автопилота                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__speed_down() {

    int ival = qRound( __speed->lcd->value() );
    ival -= __speed->step();
    float fval = static_cast<float>( ival );
    set__air_speed( fval );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *               Нажатие на кнопку фиксации текущего значения виджита воздушной скорости автопилота                  *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__speed_center() {
    // float current_speed = __user_aircraft_state.
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *               Нажатие на кнопку увеличения текущего значения виджита воздушной скорости автопилота                *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__speed_up() {

    int ival = qRound( __speed->lcd->value() );
    ival += __speed->step();
    float fval = static_cast<float>( ival );
    set__air_speed( fval );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                         Нажатие на кнопку удержания вертикальной скорости автопилота                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__button_vertical_velocity_fired() {

    if ( is_autopilot_on() ) {

        // Текущее установленное значение вертикальной скорости.
        int i = static_cast<int>( __vertical_velocity->lcd->value() );
        float f = static_cast<float>( i );

        // set__vvi_climb_engaged();
        set__one_flag(AUTOPILOT_STATE_FLAG__VVI_CLIMB_ENGAGE);

        set__vertical_velocity(f);

    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *    Передергиваем флаг навигации. Если уже ARMED или ENGAGED - выключаем. Если еще ничего нет - включаем ARMED.    *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
inline void AutopilotControlWidget::__reswitch_nav_mode() {

    bool armed = __ap.autopilot_state() & AUTOPILOT_STATE_FLAG__HNAV_ARMED;
    bool engaged = __ap.autopilot_state() & AUTOPILOT_STATE_FLAG__HNAV_ENGAGED;

    // Тут такая фишка. Повторно дать ему engaged, если он уже engaged,
    // эффекта не имеет. А вот выдача armed - сбрасывает его.

    if ( armed || engaged ) set__one_flag( AUTOPILOT_STATE_FLAG__HNAV_ARMED );
    else set__one_flag( AUTOPILOT_STATE_FLAG__HNAV_ARMED );
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                     Выключить режим навигации, если он есть.                                      *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
inline void AutopilotControlWidget::__switch_off_nav_mode() {

    bool armed = __ap.autopilot_state() & AUTOPILOT_STATE_FLAG__HNAV_ARMED;
    bool engaged = __ap.autopilot_state() & AUTOPILOT_STATE_FLAG__HNAV_ENGAGED;

    if ( armed || engaged ) set__one_flag( AUTOPILOT_STATE_FLAG__HNAV_ARMED );
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Нажатие на кнопку NAV                                                *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__button_hnav_fired() {

    if ( is_autopilot_on() ) {
        set__one_flag( AUTOPILOT_STATE_FLAG__HNAV_ARMED );
    }
    /*
    switch ( __user_aircraft_state.hsi_selector() ) {
        case AircraftState::HSI_UNKNOWN:
        break;

        case AircraftState::HSI_NAV_1:
        case AircraftState::HSI_NAV_2:
        {
            // Если мы уже находимся в NAV1 и еще раз нажали, то надо сбросить флаг.
            // Но флагов там может быть - два. И если ни один из них не установлен,
            // то ставим в armed.
            __reswitch_nav_mode();
        } break;

        case AircraftState::HSI_GPS:
        {
            __switch_off_nav_mode();
            // Если мы находимся в режиме GPS и нажали кнопку NAV,
            // то надо сначала "передернуть" переключатель на NAV1.
            CommandSet cmd( CommandSet::SET_HSI_SELECTOR, static_cast<int>(AircraftState::HSI_NAV_1), 0.0 );
            emit signal__need_send( cmd );
            // __wait_hsi_selector = true;
            set__one_flag( AUTOPILOT_STATE_FLAG__HNAV_ARMED );

        } break;
    }
    */

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Нажатие на кнопку GPS                                                 *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void AutopilotControlWidget::slot__button_gps_fired() {

    switch ( __user_aircraft_state.hsi_selector() ) {
        case AircraftState::HSI_UNKNOWN:
        break;

        case AircraftState::HSI_NAV_1:
        case AircraftState::HSI_NAV_2:
        {
            __switch_off_nav_mode();
            // Если мы находимся в NAV, а нажали GPS, то сначала нужно переключить на GPS.
            CommandSet cmd( CommandSet::SET_HSI_SELECTOR, static_cast<int>(AircraftState::HSI_GPS), 0.0 );
            emit signal__need_send( cmd );
            // __wait_hsi_selector = true;
            set__one_flag( AUTOPILOT_STATE_FLAG__HNAV_ARMED );
        } break;

        case AircraftState::HSI_GPS:
            // Если уже были в GPS, то смотрим, в каком состоянии флаги.
            __reswitch_nav_mode();
        break;
    }

}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *            Нажатие на кнопку уменьшения текущего значения виджита вертикальной скорости автопилота                *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__vertical_velocity_down() {

    int ival = qRound( __vertical_velocity->lcd->value() );
    ival -= __vertical_velocity->step();
    float fval = static_cast<float>(ival);
    set__vertical_velocity( fval );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *           Нажатие на кнопку фиксации текущего значения виджита вертикальной скорости автопилота                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__vertical_velocity_center() {

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *          Нажатие на кнопку увеличения текущего значения виджита вертикальной скорости автопилота                  *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__vertical_velocity_up() {

    int ival = qRound( __vertical_velocity->lcd->value() );
    ival += __vertical_velocity->step();
    float fval = static_cast<float>(ival);
    set__vertical_velocity(fval);
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Нажатие на кнопку удержания глиссады                                         *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__button_approach_fired() {

    if ( is_autopilot_on() ) {
        set__one_flag( AUTOPILOT_STATE_FLAG__GLIDESLOPE_ARMED );
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                               (Извне) получено состояние пользовательского самолета                               *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__got_user_aircraft_state( AircraftState & acf_state ) {

    /*
    if ( __wait_hsi_selector ) {

        AircraftState::HSI_SELECTOR_T old = __user_aircraft_state.hsi_selector();
        AircraftState::HSI_SELECTOR_T current = acf_state.hsi_selector();
        if ( old != current ) {
            // Если мы ожидали изменения HSI-селектора, то надо
            // подать ему команду на установку ARM-режима навигации.
            set__one_flag( AUTOPILOT_STATE_FLAG__HNAV_ARMED );
            __wait_hsi_selector = false;
        }
    }
    */

    __user_aircraft_state = acf_state;
    __display__hsi_mode();
    __compare_values();

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                            Нажатие на кнопку удержания воздушной скорости автопилота                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__button_air_speed_fired() {

    if ( is_autopilot_on() ) {
        // Переключатель.
        if ( is_autothrottle_manual() ) set__autothrottle_airspeed_hold();
        else if ( is_autothrottle_airspeed_hold() ) set__autothrottle_manual();
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Нажатие на кнопку селектора горизонтальной навигации - по GPS                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__button_hsi_gps_fired() {

    CommandSet cmd( CommandSet::SET_HSI_SELECTOR, AircraftState::HSI_GPS, 0.0 );
    emit signal__need_send( cmd );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                       Нажатие на кнопку селектора горизонтальной навигации - по NAV1                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__button_hsi_nav1_fired() {

    CommandSet cmd( CommandSet::SET_HSI_SELECTOR, AircraftState::HSI_NAV_1, 0.0 );
    emit signal__need_send( cmd );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Нажатие на кнопку селектора горизонтальной навигации - по NAV2                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AutopilotControlWidget::slot__button_hsi_nav2_fired() {

    CommandSet cmd( CommandSet::SET_HSI_SELECTOR, AircraftState::HSI_NAV_2, 0.0 );
    emit signal__need_send( cmd );

}
