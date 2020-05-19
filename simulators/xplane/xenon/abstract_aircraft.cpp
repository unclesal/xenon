// *********************************************************************************************************************
// *                                       An abstract aircraft internal of X-Plane simulator.                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 mar 2019 at 15:47 *
// *********************************************************************************************************************

#include "XPLMGraphics.h"
#include "abstract_aircraft.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   The constructor                                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

AbstractAircraft::AbstractAircraft()
    : AbstractVehicle()
{
    
    // Set all dataref pointers to zero value.
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Конструктор копирования                                                 *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
AbstractAircraft::AbstractAircraft(const AbstractAircraft & aacf)
    : AircraftState(aacf)
    , AbstractObservedObject(aacf)
{
    _x_dataref = aacf._x_dataref;
    _y_dataref = aacf._y_dataref;
    _z_dataref = aacf._z_dataref;

    _psi_dataref = aacf._psi_dataref;
    _theta_dataref = aacf._theta_dataref;
    _phi_dataref = aacf._phi_dataref;

    _heading_dataref = aacf._heading_dataref;
    _vvi_dataref = aacf._vvi_dataref;
    _ias_dataref = aacf._ias_dataref;
    _magnetic_variation_dataref = aacf._magnetic_variation_dataref;
    _hsi_selector_dataref = aacf._hsi_selector_dataref;
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Преобразование объекта в JSON                                           *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void AbstractAircraft::to_json( JSON & json ) {
    AircraftState::to_json( json );
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Преобразование из JSONа в объект.                                        *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void AbstractAircraft::from_json( JSON & json) {
    AircraftState::from_json( json );
}
*/
