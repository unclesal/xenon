// *********************************************************************************************************************
// *                                      The common predecessor of all aircraft.                                      *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 mar 2019 at 19:11 *
// *********************************************************************************************************************

#include "aircraft_state.h"

using namespace xenon;

string AircraftState::COMMAND_NAME = "AircraftState";

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   The constructor                                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftState::AircraftState(int acf_index) : JSONAble() {
    
    _command_name = AircraftState::COMMAND_NAME;
    
    _aircraft_index = acf_index;
    
    _latitude = 0.0;
    _longitude = 0.0;
    _altitude = 0.0;

    _heading = 0.0;
    _pitch = 0.0;
    _roll = 0.0;
    _yaw = 0.0;

    _vvi = 0.0;
    _ias = 0.0;
    _magnetic_variation = 0.0;

    // gps / nav1 / nav2
    _hsi_selector = HSI_UNKNOWN;
        
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Конструктор копирования                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftState::AircraftState( const AircraftState & as )
    : AircraftState()
{
    _aircraft_index = as._aircraft_index;

    _latitude = as._latitude;
    _longitude = as._longitude;
    _altitude = as._altitude;

    _heading = as._heading;
    _pitch = as._pitch;
    _roll = as._roll;
    _yaw = as._yaw;

    _vvi = as._vvi;
    _ias = as._ias;
    _magnetic_variation = as._magnetic_variation;

    _hsi_selector = as._hsi_selector;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Present the aircraft in JSON form                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftState::to_json( JSON & json ) {

    JSONAble::to_json( json );
    
    json["aircraft_index"] = _aircraft_index;

    json["latitude"] = _latitude;
    json["longitude"] = _longitude;
    json["altitude"] = _altitude;
        
    json["heading"] = _heading;
    json["pitch"] = _pitch;
    json["roll"] = _roll;
    json["yaw"] = _yaw;

    json["vvi"] = _vvi;
    json["ias"] = _ias;
    json["magnetic_variation"] = _magnetic_variation;

    json["hsi_selector"] = static_cast<int>(_hsi_selector);
        
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Assembling this object from JSON form.                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftState::from_json( JSON & json ) {
    
    JSONAble::from_json( json );
    
    _aircraft_index = json.value( "aircraft_index", -1 );
    _latitude = json.value( "latitude", 0.0 );
    _longitude = json.value( "longitude", 0.0 );
    _altitude = json.value( "altitude", 0.0 );
    
    _heading = static_cast<float>( json.value( "heading", 0.0 ) );
    _pitch = static_cast<float>( json.value( "pitch", 0.0 ) );
    _roll = static_cast<float>( json.value( "roll", 0.0 ) );
    _yaw = static_cast<float>( json.value( "yaw", 0.0 ) );

    _vvi = static_cast<float>( json.value("vvi", 0.0 ));
    _ias = static_cast<float>( json.value("ias", 0.0 ));
    _magnetic_variation = static_cast<float>( json.value("magnetic_variation", 0.0) );

    _hsi_selector = static_cast<HSI_SELECTOR_T>(json.value( "hsi_selector", -1 ));
            
}
