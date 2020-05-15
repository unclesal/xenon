// *********************************************************************************************************************
// *                                 Граф состояний самолета и переходов между ними                                    *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 08:54 *
// *********************************************************************************************************************
#include "aircraft_state_graph.h"

using namespace xenon;
using namespace xenon::aircraft_state_graph;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                 Конструктор                                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftStateGraph::AircraftStateGraph( AbstractAircraft * ptr_acf ) {
    
    __ptr_acf = ptr_acf;

    // ------------------------------------------------------------------------
    //                              Состояния
    // ------------------------------------------------------------------------

    // Состояние "на парковке".
    node_t state_parking;
    state_parking.state = ACF_STATE_PARKING;
    auto state_parking_d = boost::add_vertex( state_parking, __graph );
    
//     AircraftStateParking * ptr_state_parking = new AircraftStateParking( __ptr_acf, state_parking_d );
//     state_parking.ptr_state_class = ptr_state_parking;
//     __states.push_back( ptr_state_parking );
//     __graph[ state_parking_d ] = state_parking;
    

    // Состояние "готов к рулежке".
    node_t state_ready_for_taxing;
    state_ready_for_taxing.state = ACF_STATE_READY_FOR_TAXING;
    auto state_ready_for_taxing_d = boost::add_vertex( state_ready_for_taxing, __graph );
    AircraftStateReadyForTaxing * ptr_state_ready_for_taxing = new AircraftStateReadyForTaxing( 
        __ptr_acf, state_ready_for_taxing_d 
    );
    state_ready_for_taxing.ptr_state_class = ptr_state_ready_for_taxing;
    __states.push_back( ptr_state_ready_for_taxing );
    __graph[ state_ready_for_taxing_d ] = state_ready_for_taxing;

    // Состояние "предварительный старт".
    node_t state_on_hp;
    state_on_hp.state = ACF_STATE_HP;
    auto state_on_hp_d = boost::add_vertex( state_on_hp, __graph );
    

    // ------------------------------------------------------------------------
    //           Действия, переход из одного состояния в другое
    // ------------------------------------------------------------------------

    // Из состояния "на парковке" можно перейти в состояние "готов к
    // рулежке" посредством либо выталкивания, либо собственного руления.
    // Зависит от того, где по курсу расположена начальная точка руления.
    // TODO: тут надо еще иметь промежуточное состояние на разрешение этого дела.

    edge_t e_slow_taxing_to_start_point;
    e_slow_taxing_to_start_point.does = ACF_DOES_SLOW_TAXING;
    auto added_edge = boost::add_edge( state_parking_d, state_ready_for_taxing_d, __graph );
    
    auto ptr_slow_taxing_to_start_point = new AircraftDoesSlowTaxing( __ptr_acf, added_edge.first );
    e_slow_taxing_to_start_point.ptr_does_class = ptr_slow_taxing_to_start_point;
    __graph[ added_edge.first ] = e_slow_taxing_to_start_point;
    __does.push_back( ptr_slow_taxing_to_start_point );
    

    edge_t e_push_back;
    e_push_back.does = ACF_DOES_PUSH_BACK;
    added_edge = boost::add_edge( state_parking_d, state_ready_for_taxing_d, __graph );
    auto ptr_push_back = new AircraftDoesPushBack( __ptr_acf, added_edge.first );
    e_push_back.ptr_does_class = ptr_push_back;
    __graph[ added_edge.first ] = e_push_back;
    __does.push_back( ptr_push_back );

    // Из состояния "готов к рулению" в состояние "на
    // предварительном старте" можно перейти рулением.

    edge_t e_taxing_to_hp;
    e_taxing_to_hp.does = ACF_DOES_NORMAL_TAXING;
    added_edge = boost::add_edge( state_ready_for_taxing_d, state_on_hp_d, __graph );
    __graph[ added_edge.first ] = e_taxing_to_hp;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Расположить самолет на стоянке                                         *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftStateGraph::place_on_parking( const waypoint_t & wp ) {
    XPlane::log("AircraftStateGraph::place_on_parking");
};
