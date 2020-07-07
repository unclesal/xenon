// *********************************************************************************************************************
// *                      Любой объект, который существует внутри X-Plane и может перемещаться                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 11:05 *
// *********************************************************************************************************************

#include "abstract_vehicle.h"

#ifdef INSIDE_XPLANE
#include "xplane.hpp"
#endif

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                  Конструктор.                                                     *
// *                                                                                                                   *
// *********************************************************************************************************************
AbstractVehicle::AbstractVehicle() {

    vcl_condition.is_clamped_to_ground = true; // Пока что - для машинки же, она на плоскости.
    
#ifdef INSIDE_XPLANE    
   __terrain_ref = XPLMCreateProbe( xplm_ProbeY );
#endif    
   
#ifdef INSIDE_AGENT
   agent = nullptr;
#endif
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Локация самоходки                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

location_t AbstractVehicle::get_location() {
#ifdef INSIDE_XPLANE
    
    // Внутри X-Plane
    position_t position = get_position();
    return XPlane::position_to_location(position);

#else
    
    // Не в плагине, отдельно действующая "самоходка" 
    // в рамках, например, внешнего агента.
    return vcl_condition.location;

#endif
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Установить высоту позиции - на земле в данной точке                                *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef INSIDE_XPLANE
void AbstractVehicle::hit_to_ground( position_t & position ) {  
    
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
   
}    
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                               Установить высоту локации - на земле в данной точке                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef INSIDE_XPLANE
void AbstractVehicle::hit_to_ground( location_t & location ) {
   auto position = XPlane::location_to_position( location );
   hit_to_ground( position );
   location = XPlane::position_to_location( position );
}
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                "Прижать к земле"                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef INSIDE_XPLANE
void AbstractVehicle::clamp_to_ground() {

    auto pos = get_position();
    hit_to_ground( pos );
    set_position( pos );

}
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Установка гео-позиции инстанции                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void AbstractVehicle::set_location( const location_t & location ) {
#ifdef INSIDE_XPLANE
    
    // Внутри X-Plane plugin'а.
    position_t position = XPlane::location_to_position( location );
    set_position( position );
#else
    
    // Не в плагине. Например - во внешнем агенте.
    vcl_condition.location = location;
    
#endif
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
