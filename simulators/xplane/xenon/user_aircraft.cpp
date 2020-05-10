// *********************************************************************************************************************
// *       User aircraft. The one on which the user itself (the human, not automate) is flying in the simulator.       *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 mar 2019 at 17:07 *
// *********************************************************************************************************************

#include "user_aircraft.h"
#include "xplane_utilities.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    The constructor                                                *
// *                                                                                                                   *
// *********************************************************************************************************************

UserAircraft::UserAircraft() : AbstractAircraft() {


    // For user aircraft the aircraft index is always 0.
    // _aircraft_index = 0;

    // _pitch_dataref = nullptr;
    // _roll_dataref = nullptr;
    // _yaw_dataref = nullptr;

    // position variables of the aircraft in the air (in the space).
    __dr_x = XPlaneUtilities::find_data_ref("sim/flightmodel/position/local_x");
    __dr_y = XPlaneUtilities::find_data_ref("sim/flightmodel/position/local_y");
    __dr_z = XPlaneUtilities::find_data_ref("sim/flightmodel/position/local_z");
    __dr_psi = XPlaneUtilities::find_data_ref("sim/flightmodel/position/psi");
    __dr_theta = XPlaneUtilities::find_data_ref("sim/flightmodel/position/theta");
    __dr_phi = XPlaneUtilities::find_data_ref("sim/flightmodel/position/phi");

    // The true magnetic course of the aircraft.
    __dr_heading = XPlaneUtilities::find_data_ref("sim/flightmodel/position/mag_psi");

    // VVI, vertical velocity, feet per second, float.
    __dr_vvi = XPlaneUtilities::find_data_ref("sim/flightmodel/position/vh_ind_fpm");

    // Instrumental speed (relative to air)
    __dr_ias = XPlaneUtilities::find_data_ref("sim/flightmodel/position/indicated_airspeed");

    // The local magnetic variation
    __dr_magnetic_variation = XPlaneUtilities::find_data_ref("sim/flightmodel/position/magnetic_variation");

    // GPS / NAV1 / NAV2
    __dr_hsi_selector = XPlaneUtilities::find_data_ref("sim/cockpit/switches/HSI_selector");

    /*

    // The speed relative to ground.
    _groundspeed_dataref = xplugin__find_dataref("sim/flightmodel/position/groundspeed");

    // Скорости самолета.
    // acf->vh_ind_dataref = xplugin__find_dataref("sim/flightmodel/position/vh_ind");

    // The acceleration along the X axis in local coordinates.
    _ax_dataref = xplugin__find_dataref("sim/flightmodel/position/local_ax");
    // The acceleration along the Y axis in local coordinates.
    _ay_dataref = xplugin__find_dataref("sim/flightmodel/position/local_ay");
    // The acceleration along the Y axis in local coordinates.
    _az_dataref = xplugin__find_dataref("sim/flightmodel/position/local_az");

    // Угловой момент по крену M
    // acf->M_dataref = xplugin__find_dataref("sim/flightmodel/position/M");
    // Угловой момент по тангажу N
    // acf->N_dataref = xplugin__find_dataref("sim/flightmodel/position/N");
    // Угловой момент по рысканию L
    // acf->L_dataref = xplugin__find_dataref("sim/flightmodel/position/L");

    // Скорость вращения по крену.
    acf->roll_rate_dataref =  xplugin__find_dataref("sim/flightmodel/position/P");
    // Скорость вращения по тангажу.
    acf->pitch_rate_dataref = xplugin__find_dataref("sim/flightmodel/position/Q");
    // Скорость вращения по рысканию
    acf->yaw_rate_dataref = xplugin__find_dataref("sim/flightmodel/position/R");

    // Ускорение вращения по крену.
    // acf->roll_a_rotation_dataref = xplugin__find_dataref("sim/flightmodel/position/P_dot");
    // Ускорение вращения по тангажу.
    // acf->pitch_a_rotation_dataref = xplugin__find_dataref("sim/flightmodel/position/Q_dot");
    // Ускорение вращения по рысканию.
    // acf->yaw_a_rotation_dataref = xplugin__find_dataref("sim/flightmodel/position/R_dot");

    // Угол отклонения левого элерона в градусах.
    // sim/flightmodel/controls/lail1def
    acf->left_aileron_dataref = xplugin__find_dataref("sim/flightmodel/controls/wing1l_ail1def");
        // Угол отклонения правого элерона в градусах.
//     acf->right_aileron_dataref = xplugin__find_dataref("sim/flightmodel/controls/wing1r_ail1def");

    acf->left_flaperon_dataref = xplugin__find_dataref("sim/flightmodel/controls/wing1l_fla1def");
    // Угол отклонения правого флаеерона.
    acf->right_flaperon_dataref = xplugin__find_dataref("sim/flightmodel/controls/wing1r_fla1def");


    // Угол отклонения элеронов как массива (для новой моделькии).
    acf->ail1_array_dataref = xplugin__find_dataref("sim/flightmodel/controls/ail1_def");
    // Угол отклонения флаперонов как массива (для новой модельки).
    acf->fla1_array_dataref = xplugin__find_dataref("sim/flightmodel/controls/fla1_def");


    // Массив положения "Руль высоты 1".
    acf->elevator_1_dataref = xplugin__find_dataref("sim/flightmodel/controls/elv1_def");

    // Массив положения "Руль высоты 2"
    acf->elevator_2_dataref = xplugin__find_dataref("sim/flightmodel/controls/elv2_def");

    // Руль направления левый.
    acf->left_rudder_dataref = xplugin__find_dataref("sim/flightmodel/controls/vstab1_rud1def");
    // Руль направления правый.
    acf->right_rudder_dataref = xplugin__find_dataref("sim/flightmodel/controls/vstab2_rud1def");
    // Руль направления как массив (для новой модельки).
    acf->rudder_array_dataref = xplugin__find_dataref("sim/flightmodel/controls/rudd_def");

    // Массив из всех возможных шасси для любой модификации самолетов. 10 элементов float.
    // acf->wheel_steer_dataref = xplane__find_dataref("sim/flightmodel/parts/tire_steer_act");

    // Переменная изменения положения "виртуального джойстика" по курсу (для руления передним колесом).
    acf->yoke_heading_dataref = xplugin__find_dataref( "sim/joystick/yoke_heading_ratio" );

    // Углы отклонения рулевого колеса на малой и большой скорости движения самолета.
    acf->acf_nw_steerdeg_1speed = xplugin__find_dataref("sim/aircraft/gear/acf_nw_steerdeg1");
    acf->acf_nw_steerdeg_hspeed = xplugin__find_dataref("sim/aircraft/gear/acf_nw_steerdeg2");

    acf->throttle_dataref = xplugin__find_dataref("sim/flightmodel/engine/ENGN_thro_use");

    */
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Observing aircraft position and state.                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

void UserAircraft::observe() {
    /*
    // Getting aircraft's global position.

    _latitude = 0.0;
    _longitude = 0.0;
    _altitude = 0.0;

    if ( ( __dr_x ) && ( __dr_y ) && ( __dr_z ) ) {

        // Get local position of aircraft.

        double x = XPLMGetDatad( __dr_x );
        double y = XPLMGetDatad( __dr_y );
        double z = XPLMGetDatad( __dr_z );

        // Translate local aircraft's position to global coordinates.

        XPLMLocalToWorld( x, y, z, & _latitude, & _longitude, & _altitude );
    }

    if ( __dr_heading ) _heading = XPLMGetDataf( __dr_heading );
    if ( __dr_theta ) _pitch = XPLMGetDataf( __dr_theta );
    if ( __dr_phi ) _roll = XPLMGetDataf( __dr_phi );
    if ( __dr_psi ) _yaw = XPLMGetDataf( __dr_psi );

    // Вертикальная скорость.
    if ( __dr_vvi ) _vvi = XPLMGetDataf( __dr_vvi );

    // Инструментальная воздушная скорость.
    if ( __dr_ias ) _ias = XPLMGetDataf( __dr_ias );

    if ( _dr_magnetic_variation ) _magnetic_variation = XPLMGetDataf( __dr_magnetic_variation );
    if ( __dr_hsi_selector ) _hsi_selector = static_cast<AircraftState::HSI_SELECTOR_T>(
            XPLMGetDatai( __dr_hsi_selector )
    );

    */
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Преобразование в JSON форму.                                             *
// *                                                                                                                   *
// *********************************************************************************************************************
//
//void UserAircraft::to_json( JSON & json ) {
//    AbstractAircraft::to_json( json );
//}
//
// ********************************************************************************************************************
// *                                                                                                                  *
// *                                         Преобразование из JSON формы                                             *
// *                                                                                                                  *
// ********************************************************************************************************************
//
//void UserAircraft::from_json( JSON & json ) {
//    // Вызов базового метода.
//    AbstractAircraft::from_json( json );
//}
// *********************************************************************************************************************
// *                                                                                                                   *
// *                          Получение внутриигровой позиции пользовательского самолета.                              *
// *                                                                                                                   *
// *********************************************************************************************************************

position_t UserAircraft::get_position() {
    position_t result;
    result.x = result.y = result.z = 0.0;
    if ( ( __dr_x ) && ( __dr_y ) && ( __dr_z ) ) {
        result.x = XPLMGetDatad( __dr_x );
        result.y = XPLMGetDatad( __dr_y );
        result.z = XPLMGetDatad( __dr_z );
    }
    return result;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                           Установка внутриигровой позиции пользовательского самолета.                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void UserAircraft::set_position( const position_t & position ) {

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Установка чего-нибудь в пользовательском самолете                                  *
// *                                                                                                                   *
// *********************************************************************************************************************
//
//void UserAircraft::set( CommandSet & cmd ) {
//
//    switch ( cmd.set_command() ) {
//
//        case CommandSet::SET_HSI_SELECTOR:
//            _set_as_int( cmd, _hsi_selector_dataref );
//        break;
//
//        default:
//            char out[512];
//            sprintf( out, "UserAircraft::set(), unhandled command %d", static_cast<int>( cmd.set_command() ) );
//            XPlaneUtilities::log( out );
//    }
//
//}
