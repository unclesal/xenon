// *********************************************************************************************************************
// *                      Любой объект, который существует внутри X-Plane и может перемещаться                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 11:05 *
// *********************************************************************************************************************

#include "abstract_vehicle.h"
#include "xplane.hpp"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                  Конструктор.                                                     *
// *                                                                                                                   *
// *********************************************************************************************************************
AbstractVehicle::AbstractVehicle() {

    is_clamped_to_ground = false;
    
#ifdef INSIDE_XPLANE    
   __terrain_ref = XPLMCreateProbe( xplm_ProbeY );
#endif    
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Локация самоходки                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

location_t AbstractVehicle::get_location() {
    position_t position = get_position();
    return XPlane::position_to_location(position);
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Установить высоту позиции - на земле в данной точке                                *
// *                                                                                                                   *
// *********************************************************************************************************************
void AbstractVehicle::hit_to_ground( position_t & position ) {  

#ifdef INSIDE_XPLANE
    
   if ( ! __terrain_ref ) return;
   
   XPLMProbeInfo_t infoProbe = {
       sizeof(XPLMProbeInfo_t),            // structSIze
       0.0f, 0.0f, 0.0f,                   // location
       0.0f, 0.0f, 0.0f,                   // normal vector
       0.0f, 0.0f, 0.0f,                   // velocity vector
       0                                   // is_wet
   };
   
   if (XPLMProbeTerrainXYZ( __terrain_ref, position.x, position.y, position.z, &infoProbe ) == xplm_ProbeHitTerrain ) {
       position.y = infoProbe.locationY;       
   } else {
       XPlane::log(
           "WARNING: position x=" + to_string(position.x) + ", z=" + to_string(position.z) + " does not touch terrain."
       );
   };
   
#endif
    
}    

// *********************************************************************************************************************
// *                                                                                                                   *
// *                               Установить высоту локации - на земле в данной точке                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void AbstractVehicle::hit_to_ground( location_t & location ) {
    
   auto position = XPlane::location_to_position( location );
   hit_to_ground( position );
   location = XPlane::position_to_location( position );
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                "Прижать к земле"                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

void AbstractVehicle::clamp_to_ground() {
    auto pos = get_position();
    hit_to_ground( pos );
    set_position( pos );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Установка гео-позиции инстанции                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void AbstractVehicle::set_location( const location_t & location ) {
    position_t position = XPlane::location_to_position( location );
    set_position( position );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Деструктор.                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

AbstractVehicle::~AbstractVehicle() {

#ifdef INSIDE_XPLANE
    
   if ( __terrain_ref ) XPLMDestroyProbe( __terrain_ref );
   
#endif    

}
