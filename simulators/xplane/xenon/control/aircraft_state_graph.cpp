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
    
    __current_state = nullptr;
    __current_action = nullptr;

    // ------------------------------------------------------------------------
    //                              Состояния
    // ------------------------------------------------------------------------

    // Состояние "на парковке".
    node_t state_parking;
    state_parking.state = ACF_STATE_PARKING;
    state_parking.name = "Parking";
    auto state_parking_d = boost::add_vertex( state_parking, __graph );    
    AircraftStateParking * ptr_state_parking = new AircraftStateParking( __ptr_acf, state_parking_d );
    state_parking.ptr_state_class = ptr_state_parking;
    __states.push_back( ptr_state_parking );
    __graph[ state_parking_d ] = state_parking;
    

    // Состояние "готов к рулежке".
    node_t state_ready_for_taxing;
    state_ready_for_taxing.state = ACF_STATE_READY_FOR_TAXING;
    state_ready_for_taxing.name = "Ready for taxing";
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
    
    // На парковке можно находиться бесконечно и ничего не делать. Это неправильно,
    // т.к. может привести к "зависанию" самолета. Но пока я ничего лучшего не придумал.
    edge_t e_does_nothing_on_parking;
    e_does_nothing_on_parking.action = ACF_DOES_NOTHING;
    e_does_nothing_on_parking.name = "Nothing to do";
    auto added_edge = boost::add_edge( state_parking_d, state_parking_d, __graph );
    auto ptr_does_nothing_on_parking = new AircraftDoesNothing( __ptr_acf, added_edge.first );
    e_does_nothing_on_parking.ptr_does_class = ptr_does_nothing_on_parking;
    __graph[ added_edge.first ] = e_does_nothing_on_parking;
    __actions.push_back( ptr_does_nothing_on_parking );

    // Из состояния "на парковке" можно перейти в состояние "готов к
    // рулежке" посредством либо выталкивания, либо собственного руления.
    // Зависит от того, где по курсу расположена начальная точка руления.
    // TODO: тут надо еще иметь промежуточное состояние на разрешение этого дела.

    edge_t e_slow_taxing_to_start_point;
    e_slow_taxing_to_start_point.action = ACF_DOES_SLOW_TAXING;
    e_slow_taxing_to_start_point.name = "Taxing (slowly)";
    added_edge = boost::add_edge( state_parking_d, state_ready_for_taxing_d, __graph );    
    auto ptr_slow_taxing_to_start_point = new AircraftDoesSlowTaxing( __ptr_acf, added_edge.first );
    e_slow_taxing_to_start_point.ptr_does_class = ptr_slow_taxing_to_start_point;
    __graph[ added_edge.first ] = e_slow_taxing_to_start_point;
    __actions.push_back( ptr_slow_taxing_to_start_point );
    

    edge_t e_push_back;
    e_push_back.action = ACF_DOES_PUSH_BACK;
    e_push_back.name = "Taxing";
    added_edge = boost::add_edge( state_parking_d, state_ready_for_taxing_d, __graph );
    auto ptr_push_back = new AircraftDoesPushBack( __ptr_acf, added_edge.first );
    e_push_back.ptr_does_class = ptr_push_back;
    __graph[ added_edge.first ] = e_push_back;
    __actions.push_back( ptr_push_back );

    // Из состояния "готов к рулению" в состояние "на
    // предварительном старте" можно перейти рулением.

    edge_t e_taxing_to_hp;
    e_taxing_to_hp.action = ACF_DOES_NORMAL_TAXING;
    added_edge = boost::add_edge( state_ready_for_taxing_d, state_on_hp_d, __graph );
    __graph[ added_edge.first ] = e_taxing_to_hp;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                Установка current_state = false во всех состояниях, обнуление указателя текущего состояния         *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftStateGraph::clear_states_activity() {
    __current_state = nullptr;
    graph_t::vertex_iterator vi, vi_end, next;
    for ( tie(vi, vi_end) = boost::vertices( __graph ); vi != vi_end; ++vi) {
        __graph[ * vi ].current_state = false;
    }
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Обнуление указателей на текущую "деятельность"                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftStateGraph::clear_actions_activity() {
    __current_action = nullptr;
    graph_t::edge_iterator vi, vi_end, next;
    for ( tie(vi, vi_end) = boost::edges( __graph ); vi != vi_end; ++vi) {
        __graph[ * vi ].current_action = false;
    }
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Сделать данное состояние - текущим                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftStateGraph::set_active_state( const aircraft_state_graph::graph_t::vertex_descriptor & nd ) {
    try {
        __graph[ nd ].current_state = true;
        __current_state = ( AircraftAbstractState * ) __graph[ nd ].ptr_state_class;    
    } catch ( const std::range_error & re ) {
        XPlane::log("AircraftStateGraph::set_active_state called with incorrect vertex descriptor");
    }
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Сделать данное действие - текущим                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftStateGraph::set_active_action( const aircraft_state_graph::graph_t::edge_descriptor & ed ) {
    try {
        __graph[ ed ].current_action = true;
        __current_action = (AircraftAbstractAction * ) __graph[ ed ].ptr_does_class;
        // Это единственное место, где должен вызываться старт. Поэтому сам старт сделан приватным.
        if ( __current_action ) __current_action->__start();        
    } catch ( const std::range_error & re ) {
        XPlane::log("AircraftStateGraph::set_active_action() called with invalid edge descriptor." );
    }
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Расположить самолет на стоянке                                         *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftStateGraph::place_on_parking( const waypoint_t & wp ) {
        
    clear_states_activity();
    clear_actions_activity();
    
    // Находим узел "на стоянке" и делаем его текущим.
    graph_t::vertex_descriptor node_d = aircraft_state_graph::graph_t::null_vertex();
    graph_t::vertex_iterator vi, vi_end, next;
    for ( tie(vi, vi_end) = vertices( __graph ); vi != vi_end; ++vi) {
        node_d = * vi;
        if ( __graph[ node_d ].state == ACF_STATE_PARKING ) {
            set_active_state( node_d );
            break;
        }
    }
    
// ------------ in_edges --------------
//     boost::graph_traits<Graph>::vertex_iterator i, end;
//     boost::graph_traits<Graph>::in_edge_iterator ei, edge_end;
// 
//     for(boost::tie(i,end) = vertices(g); i != end; ++i) {
//         cout << *i << " <-- ";
//         for (boost::tie(ei,edge_end) = in_edges(*i, g); ei != edge_end; ++ei)
//         cout << source(*ei, g) << "  ";
//         cout << endl;
//     }
// ------------------------------------
    
    // Стоим на стоянке - ничего не делаем. Если нашли неправильный node_t, то здесь будет исключение индекса массива.
    try {
        aircraft_state_graph::graph_t::out_edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = out_edges( node_d, __graph); ei != ei_end; ++ei) {
            auto source = boost::source ( *ei, __graph );
            auto target = boost::target ( *ei, __graph );
            if ( ( source == target ) && ( __graph[ * ei ].action == ACF_DOES_NOTHING ) ) {
                // Выходит из состояния парковки, входит в него же и при
                // этом ничего не делает. Ура. Нашли то, что нам надо.
                set_active_action( * ei );
                break;
            }
        }                
    } catch ( const std::range_error & re ) {
        XPlane::log("AircraftStateGraph::place_on_parking, incorrect node_d was found");
        return;
    }
    
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Изменение состояния текущего действия                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftStateGraph::update( float elapsed_since_last_call ) {
    if ( __current_action ) __current_action->__step( elapsed_since_last_call );
}
