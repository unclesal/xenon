// *********************************************************************************************************************
// *                                             Состояние автопилота.                                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 14 sep 2019 at 21:45 *
// *********************************************************************************************************************

#include "CommandAutopilotState.h"
using namespace xenon;
using namespace std;

string CommandAutopilotState::COMMAND_NAME = "CommandAutopilotState";

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Конструктор.                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

CommandAutopilotState::CommandAutopilotState()
    : JSONAble()
{
    _command_name = CommandAutopilotState::COMMAND_NAME;
    _vvi_step_ft = 0.0;
    _alt_step_ft = 0.0;
    _radio_altimeter_step_ft = 0.0;
    _preconfigured_ap_type = AP_NONE;
    _single_axis_autopilot = false;
    _ah_source = AH_NONE;
    _dg_source = DG_NONE;
    
    _autopilot_mode = AP_MODE_NONE;
    
    // Airspeed mode for the autopilot. DEPRECATED
    _airspeed_mode = -1;
    
    // _approach_selector = AP_APPROACH_SELECTOR_NONE;
    
    // Altitude dialed into the AP
    _altitude = 0.0;
    
    // Currently held altitude (remembered until you hit flchg)
    _current_altitude = 0.0;
    
    // Vertical speed to hold
    _vertical_velocity = 0.0;
    
    // Airspeed to hold, this changes from knots to a mach number
    _airspeed = 0.0;
    
    // The heading to fly (true, legacy)
    _heading = 0.0;
    _heading_mag = 0.0;
    
    // Various autopilot engage modes, etc. See docs for flags
    _autopilot_state = 0;

    // autothrottle
    _autothrottle = THROTTLE_UNKNOWN;
    _autothrottle_on = 0;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Преобразование из объекта в JSON                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void CommandAutopilotState::to_json( JSON & json ) {
    
    JSONAble::to_json( json );
    json["vvi_step_ft"] = _vvi_step_ft;
    json["alt_step_ft"] = _alt_step_ft;
    json["radio_altimeter_step_ft"] = _radio_altimeter_step_ft;
    json["preconfigured_ap_type"] = static_cast<int>( _preconfigured_ap_type );
    json["single_axis_autopilot"] = _single_axis_autopilot;
    json["ah_source"] = static_cast<int>( _ah_source );
    json["dg_source"] = static_cast<int>( _dg_source );
    
    // Режим работы автопилота.
    json["autopilot_mode"] = static_cast<int>( _autopilot_mode );
    
    // Airspeed mode for the autopilot. DEPRECATED
    json["airspeed_mode"] = _airspeed_mode;
    
    // approach selector
    // json["approach_selector"] = static_cast<int>( _approach_selector );
    
    // Altitude dialed into the AP
    json["altitude"] = _altitude;
    
    // Currently held altitude (remembered until you hit flchg)
    json["current_altitude"] = _current_altitude;
    
    // Vertical speed to hold
    json["vertical_velocity"] = _vertical_velocity;
    
    // Airspeed to hold, this changes from knots to a mach number
    json["airspeed"] = _airspeed;
    
    // The heading to fly (true, legacy)
    json["heading"] = _heading;
    // The heading to fly (magnetic, preferred) pilot
    json["heading_mag"] = _heading_mag;
    
    // Various autopilot engage modes, etc. See docs for flags
    json["autopilot_state"] = _autopilot_state;

    // autothrottle
    json["autothrottle"] = static_cast<int>(_autothrottle);
    json["autothrottle_on"] = _autothrottle_on;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Преобразование из JSONа в объект                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void CommandAutopilotState::from_json( JSON & json ) {
    
    JSONAble::from_json( json );
    
    _vvi_step_ft = static_cast<float>( json.value( "vvi_step_ft", 0.0 ) );
    _alt_step_ft = static_cast<float>( json.value( "alt_step_ft", 0.0 ) );
    _radio_altimeter_step_ft = static_cast<float>( json.value( "radio_altimeter_step_ft", 0.0 ) );
    
    int i_preconfigured_ap_type = json.value( "preconfigured_ap_type", -1 );
    _preconfigured_ap_type = static_cast<AP_TYPE_T>(i_preconfigured_ap_type);
    
    _single_axis_autopilot = json.value( "single_axis_autopilot", false );
    
    int i_ah_source = json.value( "ah_source", -1 );
    _ah_source = static_cast<AP_AH_SOURCE_T>( i_ah_source );
    
    int i_dg_source = json.value( "dg_source", -1 );
    _dg_source = static_cast<AP_DG_SOURCE_T>( i_dg_source );
    
    // Режим работы автопилота.
    int i_autopilot_mode = json.value( "autopilot_mode", -1 );
    _autopilot_mode = static_cast<AP_MODE_T>( i_autopilot_mode );
    
    // Airspeed mode for the autopilot. DEPRECATED
    _airspeed_mode = json.value( "airspeed_mode", -1 );
    
    // approach selector
    // int i_approach_selector = json.value( "approach_selector", -1 );
    // _approach_selector = static_cast<AP_APPROACH_SELECTOR_T>( i_approach_selector );
    
    // altitude
    _altitude = static_cast<float>(json.value( "altitude", 0.0 ));
    
    // Currently held altitude (remembered until you hit flchg)
    _current_altitude = static_cast<float>(json.value( "current_altitude", 0.0 ));
    
    // Vertical speed to hold
    _vertical_velocity = static_cast<float>(json.value( "vertical_velocity", 0.0 ));
    
    // Airspeed to hold, this changes from knots to a mach number
    _airspeed = static_cast<float>(json.value( "airspeed", 0.0 ));
    
    // The heading to fly (true, legacy)
    _heading = static_cast<float>(json.value( "heading", 0.0 ));
    // The heading to fly (magnetic, preferred) pilot
    _heading_mag = static_cast<float>(json.value("heading_mag", 0.0 ));

    // Various autopilot engage modes, etc. See docs for flags
    _autopilot_state = json.value("autopilot_state", 0);

    // autothrottle
    int at = json.value("autothrottle", -1);
    _autothrottle = static_cast<AP_THROTTLE_T>( at );
    _autothrottle_on = json.value("autothrottle_on", 0 );

}

