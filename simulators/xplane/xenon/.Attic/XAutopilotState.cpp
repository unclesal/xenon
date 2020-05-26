// *********************************************************************************************************************
// *                              Реализация состояния автопилота внутри симулятора X-Plane.                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 sep 2019 at 10:44 *
// *********************************************************************************************************************

#include "XAutopilotState.h"
using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Конструктор.                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

XAutopilotState::XAutopilotState() 
    : CommandAutopilotState()
    , AbstractObservedObject()
{
    
    __vvi_step_ft_dataref = XPlaneUtilities::find_data_ref("sim/aircraft/autopilot/vvi_step_ft");
    __alt_step_ft_dataref = XPlaneUtilities::find_data_ref("sim/aircraft/autopilot/alt_step_ft");
    __radio_altimeter_step_ft_dataref = XPlaneUtilities::find_data_ref("sim/aircraft/autopilot/radio_altimeter_step_ft");
    __preconfigured_ap_type_dataref = XPlaneUtilities::find_data_ref("sim/aircraft/autopilot/preconfigured_ap_type");
    __single_axis_autopilot_dataref = XPlaneUtilities::find_data_ref("sim/aircraft/autopilot/single_axis_autopilot");
    __ah_source_dataref = XPlaneUtilities::find_data_ref("sim/aircraft/autopilot/ah_source");
    __dg_source_dataref = XPlaneUtilities::find_data_ref("sim/aircraft/autopilot/dg_source");
    
    // Режим работы автопилота.
    __autopilot_mode_dataref = XPlaneUtilities::find_data_ref("sim/cockpit/autopilot/autopilot_mode");
    
    // Airspeed mode for the autopilot. DEPRECATED
    __airspeed_mode_dataref = XPlaneUtilities::find_data_ref("sim/cockpit/autopilot/airspeed_mode");
    
    // approach selector
    // __approach_selector_dataref = XUtilities::find_data_ref("sim/cockpit/autopilot/approach_selector");
    
    // Altitude dialed into the AP
    __altitude_dataref = XPlaneUtilities::find_data_ref("sim/cockpit/autopilot/altitude");
    
    // Currently held altitude (remembered until you hit flchg)
    __current_altitude_dataref = XPlaneUtilities::find_data_ref("sim/cockpit/autopilot/current_altitude");
    
    // Vertical speed to hold
    __vertical_velocity_dataref = XPlaneUtilities::find_data_ref("sim/cockpit/autopilot/vertical_velocity");
    
    // Airspeed to hold, this changes from knots to a mach number
    __airspeed_dataref = XPlaneUtilities::find_data_ref("sim/cockpit/autopilot/airspeed");
    
    // The heading to fly (true, legacy)
    __heading_dataref = XPlaneUtilities::find_data_ref("sim/cockpit/autopilot/heading");
    // The heading to fly (magnetic, preferred) pilot
    __heading_mag_dataref = XPlaneUtilities::find_data_ref("sim/cockpit/autopilot/heading_mag");
    
    // Various autopilot engage modes, etc. See docs for flags
    __autopilot_state_dataref = XPlaneUtilities::find_data_ref("sim/cockpit/autopilot/autopilot_state");
    
    // auto throttle
    __autothrottle_dataref = XPlaneUtilities::find_data_ref("sim/cockpit2/autopilot/autothrottle_enabled");
    __autothrottle_on_dataref = XPlaneUtilities::find_data_ref("sim/cockpit2/autopilot/autothrottle_on");
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Процедура наблюдения                                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void XAutopilotState::observe() {
    
    if ( __vvi_step_ft_dataref ) _vvi_step_ft = XPLMGetDataf( __vvi_step_ft_dataref );
    if ( __alt_step_ft_dataref ) _alt_step_ft = XPLMGetDataf( __alt_step_ft_dataref );
    if ( __radio_altimeter_step_ft_dataref ) _radio_altimeter_step_ft = XPLMGetDataf( __radio_altimeter_step_ft_dataref );
    if ( __preconfigured_ap_type_dataref ) _preconfigured_ap_type = static_cast<AP_TYPE_T>( XPLMGetDatai( __preconfigured_ap_type_dataref ) );
    if ( __single_axis_autopilot_dataref ) _single_axis_autopilot = static_cast<bool>( XPLMGetDatai( __single_axis_autopilot_dataref ) );
    if ( __ah_source_dataref ) _ah_source = static_cast <AP_AH_SOURCE_T> ( XPLMGetDatai( __ah_source_dataref ) );
    if ( __dg_source_dataref ) _dg_source = static_cast <AP_DG_SOURCE_T> ( XPLMGetDatai( __dg_source_dataref ) );
    
    // Режим работы автопилота.
    if ( __autopilot_mode_dataref ) _autopilot_mode = static_cast<AP_MODE_T>( XPLMGetDatai( __autopilot_mode_dataref ) );
    
    // Airspeed mode for the autopilot. DEPRECATED
    if ( __airspeed_mode_dataref ) _airspeed_mode = XPLMGetDatai( __airspeed_mode_dataref );
    
    // approach selector
    // if ( __approach_selector_dataref ) _approach_selector = static_cast<AP_APPROACH_SELECTOR_T>( XPLMGetDatai( __approach_selector_dataref ) );
    
    // Altitude dialed into the AP
    if ( __altitude_dataref ) _altitude = XPLMGetDataf( __altitude_dataref );
    
    // Currently held altitude (remembered until you hit flchg)
    if ( __current_altitude_dataref ) _current_altitude = XPLMGetDataf( __current_altitude_dataref );
    
    // Vertical speed to hold
    if ( __vertical_velocity_dataref ) _vertical_velocity = XPLMGetDataf( __vertical_velocity_dataref );
    
    // Airspeed to hold, this changes from knots to a mach number
    if ( __airspeed_dataref ) _airspeed = XPLMGetDataf( __airspeed_dataref );
    
    // The heading to fly (true, legacy)
    if ( __heading_dataref ) _heading = XPLMGetDataf( __heading_dataref );
    // The heading to fly (magnetic, preferred) pilot
    if ( __heading_mag_dataref ) _heading_mag = XPLMGetDataf( __heading_mag_dataref );
    
    if ( __autopilot_state_dataref ) _autopilot_state = XPLMGetDatai( __autopilot_state_dataref );
    
    // Auto throttle
    if ( __autothrottle_dataref ) _autothrottle = static_cast<AP_THROTTLE_T>( XPLMGetDatai( __autothrottle_dataref ) );
    if ( __autothrottle_on_dataref ) _autothrottle_on = XPLMGetDatai( __autothrottle_on_dataref );
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Установка чего-нибудь автопилотного                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

void XAutopilotState::set( CommandSet & cmd ) {
    
    switch ( cmd.set_command() ) {
        
        case CommandSet::SET_AUTOPILOT_MODE:
            // Установка режима работы автопилота.        
            _set_as_int( cmd, __autopilot_mode_dataref ); 
            break;
        
        case CommandSet::SET_AUTOPILOT_STATE_FLAGS:
            _set_as_int( cmd, __autopilot_state_dataref );
            break;
        
        case CommandSet::SET_AUTOPILOT_HEADING:
            _set_as_float( cmd, __heading_dataref );
            break;
        
        case CommandSet::SET_AUTOPILOT_ALTITUDE:
            _set_as_float( cmd, __altitude_dataref );        
            break; 
        
        case CommandSet::SET_AUTOPILOT_AIR_SPEED:
            _set_as_float( cmd, __airspeed_dataref );
            break; 
        
        case CommandSet::SET_AUTOPILOT_VERTICAL_VELOCITY:
            _set_as_float( cmd, __vertical_velocity_dataref );
            break;
            
        case CommandSet::SET_AUTOTHROTTLE:
            // Установка режима работы "газа".
            _set_as_int( cmd, __autothrottle_dataref );
            break;
        
        default: {        
            sprintf( __out, "XAutopilotState::set(), unhandled name %d", static_cast<int>( cmd.set_command() ) );
            XPlaneUtilities::log( __out );
        }
    }
}
