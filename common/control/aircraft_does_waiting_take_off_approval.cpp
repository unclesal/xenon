// *********************************************************************************************************************
// *                                        Действие ожидания разрешения на взлет                                      *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 10 jul 2020 at 11:49 *
// *********************************************************************************************************************
#include "aircraft_does_waiting_take_off_approval.h"
using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Конструктор                                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftDoesWaitingTakeOffApproval::AircraftDoesWaitingTakeOffApproval(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d
) : AircraftAbstractAction( ptr_acf, edge_d )
{

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Внутренний старт действия                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesWaitingTakeOffApproval::_internal_start() {
    
    Logger::log( _ptr_acf->vcl_condition.agent_name + " Wait TO approval start");
    
    // Ничего не делаем, просто стоим.
    
    _ptr_acf->vcl_condition.speed = 0;
    _ptr_acf->vcl_condition.acceleration = 0;
    _ptr_acf->vcl_condition.target_speed = 0;
    
    _ptr_acf->vcl_condition.heading_acceleration = 0;
    _ptr_acf->vcl_condition.target_heading = _ptr_acf->vcl_condition.rotation.heading;
        
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                     Внутренний "шаг" (который здесь не нужен)                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesWaitingTakeOffApproval::_internal_step( const float & elapsed_since_last_call ) {
    
}

