
// *********************************************************************************************************************
// *                                Представление окружающей действительности - внутри X-Plane.                        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 14 sep 2019 at 19:43 *
// *********************************************************************************************************************

#include "XAmbient.h"

using namespace xenon;
using namespace std;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор.                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

XAmbient::XAmbient() 
    : Ambient()
{
    // The temperature of ambient air.
    __ambient_temperature_dataref = XPlane::find_data_ref("sim/weather/temperature_ambient_c");
    
    // The altitude relative to the underlying surface of the earth, meters.
    // Value about sea level is "altitude", this is already exists in global coordinates.
    __y_agl_dataref = XPlane::find_data_ref("sim/flightmodel/position/y_agl");

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                       Реализация метода наблюдения за состоянием окружающей действительности.                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void XAmbient::observe() {

    if ( __ambient_temperature_dataref ) _ambient_temperature = XPLMGetDataf( __ambient_temperature_dataref );
    if ( __y_agl_dataref ) _agl = XPLMGetDataf( __y_agl_dataref );
    
    // sprintf( _out, "AGL=%0.3f", _y_agl );
    // XUtilities::log( _out );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Деструктор                                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

XAmbient::~XAmbient() {
}


