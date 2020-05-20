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
    auto state_parking_d = __create_state< AircraftStateParking > (ACF_STATE_PARKING,  "Parking");            
    // Состояние "готов к рулежке".
    auto state_ready_for_taxing_d = __create_state< AircraftStateReadyForTaxing > ( 
        ACF_STATE_READY_FOR_TAXING, "Ready for taxing" 
    );
    // Состояние "предварительный старт".
    auto state_on_hp_d = __create_state< AircraftStateOnHP >( ACF_STATE_HP, "On HP" );
    // Состояние "готов к взлету"
    auto state_ready_for_take_off_d = __create_state< AircraftStateReadyForTakeOff >(
        ACF_STATE_READY_FOR_TAKE_OFF, "Ready for take off"
    );
    // Состояние "взлетел".
    auto state_airborned_d = __create_state< AircraftStateAirborned > (
        ACF_STATE_AIRBORNED, "Airborned"
    );
    

    // ------------------------------------------------------------------------
    //           Действия, переход из одного состояния в другое
    // ------------------------------------------------------------------------
    
    // На парковке можно находиться бесконечно и ничего не делать. Это неправильно,
    // т.к. может привести к "зависанию" самолета. Но пока я ничего лучшего не придумал.
    __create_action< AircraftDoesNothing >( 
        ACF_DOES_NOTHING, "Nothing to do", state_parking_d, state_parking_d
    );

    // Из состояния "на парковке" можно перейти в состояние "готов к
    // рулежке" посредством либо выталкивания, либо собственного руления.
    // Зависит от того, где по курсу расположена начальная точка руления.
    // TODO: тут надо еще иметь промежуточное состояние на разрешение этого дела.

    __create_action< AircraftDoesSlowTaxing > (
        ACF_DOES_SLOW_TAXING, "Taxing (slowly)", state_parking_d, state_ready_for_taxing_d
    );
    __create_action< AircraftDoesPushBack >(
        ACF_DOES_PUSH_BACK, "Push back", state_parking_d, state_ready_for_taxing_d
    );

    // Из состояния "готов к рулению" в состояние "на
    // предварительном старте" можно перейти рулением.

    __create_action< AircraftDoesTaxing >(
        ACF_DOES_NORMAL_TAXING, "Taxing (normal)", state_ready_for_taxing_d, state_on_hp_d
    );
    
    // Из состояния "на предварительном старте" можно перейти в состояние
    // "готов к взлету" - выравниванием (lining up)
    __create_action< AircraftDoesLiningUp > (
        ACF_DOES_LINING_UP, "Lining up", state_on_hp_d, state_ready_for_take_off_d
    );
    
    // Из состояния "готов к взлету" можно перейти в
    // состояние airborned путем взлета.
    __create_action< AircraftDoesTakeOff >(
        ACF_DOES_TAKE_OFF, "Take off", state_ready_for_take_off_d, state_airborned_d
    );
    
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
    
    clear_states_activity();
    
    try {
        __graph[ nd ].current_state = true;
        __current_state = ( AircraftAbstractState * ) __graph[ nd ].ptr_state_class;    
    } catch ( const std::range_error & re ) {
        XPlane::log("ERROR: AircraftStateGraph::set_active_state called with incorrect vertex descriptor");
    }
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Сделать данное действие - текущим                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftStateGraph::set_active_action( const aircraft_state_graph::graph_t::edge_descriptor & ed ) {
    
    XPlane::log("In graph::set_active_action");
    clear_actions_activity();
    XPlane::log("clear actions activity done");
    
    try {

#ifdef DEBUG        
        XPlane::log("AircraftStateGraph::set_active_action " + __graph[ ed ].name );
#endif        
        __graph[ ed ].current_action = true;
        __current_action = (AircraftAbstractAction * ) __graph[ ed ].ptr_does_class;

        __current_action->set_parameters( __previous_action_params );
        // Это единственное место, где должен вызываться старт. Поэтому сам старт сделан приватным.
        if ( __current_action ) __current_action->__start();

    } catch ( const std::range_error & re ) {
        XPlane::log("ERROR: AircraftStateGraph::set_active_action() called with invalid edge descriptor.");
    }

};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Расположить самолет на стоянке                                         *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftStateGraph::place_on_parking() {
        
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

    clear_states_activity();
    clear_actions_activity();
    
    // Находим узел "на стоянке" и делаем его текущим.
    aircraft_state_graph::graph_t::vertex_descriptor node_d = get_node_for( ACF_STATE_PARKING );
            
    // Стоим на стоянке - ничего не делаем. Если нашли неправильный node_t, то здесь будет исключение индекса массива.
    try {
        set_active_state( node_d );
        auto action = get_action_for( node_d, node_d, ACF_DOES_NOTHING );
        set_active_action( action );
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

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Является ли текущее действие указанным ?                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

bool AircraftStateGraph::current_action_is( const xenon::aircraft_action_t & action ) {
    
    if ( ! __current_action ) return false;
    
    try {
        auto edge = __graph[ __current_action->edge_d() ];
        return ( edge.action == action );
    } catch ( const std::range_error & re ) {
        XPlane::log("ERROR: AircraftStateGraph::current_action_is(), invalid edge descriptor");
    }
    
    return false;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                             Действительно ли граф находится в данном текущем состоянии ?                          *
// *                                                                                                                   *
// *********************************************************************************************************************

bool AircraftStateGraph::current_state_is( const xenon::aircraft_state_t & state ) {
        
    if ( ! __current_state ) return false;
    
    try {
        auto node = __graph[ __current_state->node_d() ];
        return node.state == state;
    } catch ( const std::range_error & e ) {
        XPlane::log( "ERROR: AircraftStateGraph::current_state_is(), invalid node descriptor");
    }
    
    return false;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Получить дескриптор узла для указанного состояния.                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

aircraft_state_graph::graph_t::vertex_descriptor AircraftStateGraph::get_node_for( const aircraft_state_t & state ) {
    
    aircraft_state_graph::graph_t::vertex_descriptor node_d = aircraft_state_graph::graph_t::null_vertex();
    aircraft_state_graph::graph_t::vertex_iterator vi, vi_end, next;
    for ( tie(vi, vi_end) = vertices( __graph ); vi != vi_end; ++vi) {
        node_d = * vi;
        if ( __graph[ node_d ].state == state ) {
            return node_d;
        }
    }
    return node_d;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                           Получить указанное действие - из дескрипторов состояний                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

aircraft_state_graph::graph_t::edge_descriptor AircraftStateGraph::get_action_for(
    const aircraft_state_graph::graph_t::vertex_descriptor & v_from,
    const aircraft_state_graph::graph_t::vertex_descriptor & v_to,
    const aircraft_action_t & action 
) {
    
    aircraft_state_graph::graph_t::out_edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::out_edges( v_from, __graph); ei != ei_end; ++ei) {
        auto target = boost::target ( * ei, __graph );
        if ( ( target == v_to ) && ( __graph[ * ei ].action == action ) ) {
            return * ei;
            break;
        }
    }                
    return aircraft_state_graph::graph_t::edge_descriptor();

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                             Получить указанное действие - из типов состояний                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

aircraft_state_graph::graph_t::edge_descriptor AircraftStateGraph::get_action_for( 
    const aircraft_state_t & from_state, 
    const aircraft_state_t & to_state, 
    const aircraft_action_t & action 
) {
    auto node_d_from = get_node_for( from_state );
    auto node_d_to = get_node_for( to_state );
    return get_action_for( node_d_from, node_d_to, action );    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *               Вернуть действие, исходящее из данного текущего состояния и имеющее определенный тип                *
// *                                                                                                                   *
// *********************************************************************************************************************

aircraft_state_graph::graph_t::edge_descriptor AircraftStateGraph::get_action_outgoing_from_current_state(
    const aircraft_action_t & action 
) {
    aircraft_state_graph::graph_t::edge_descriptor fake;
    
    try {
        
        if ( ! __current_state ) return fake;
        aircraft_state_graph::graph_t::vertex_descriptor current_state_d = __current_state->node_d();
        
        aircraft_state_graph::graph_t::out_edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = boost::out_edges( current_state_d, __graph); ei != ei_end; ++ei) {
            
            // auto target = boost::target ( * ei, __graph );
            
            if ( __graph[ * ei ].action == action ) {
                return * ei;
                break;
            }
        }    
    } catch ( const std::range_error & re ) {
        XPlane::log("ERROR: get_action_outgoing_from_current_state, " + string( re.what() ) );
    };
    
    XPlane::log("ERROR: no outgoing action was found from current state with type=" + to_string( action ));
    return fake;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                         Вернуть структуру node_t в зависимости от указателя на состояние                          *
// *                                                                                                                   *
// *********************************************************************************************************************

aircraft_state_graph::node_t AircraftStateGraph::get_node_for( AircraftAbstractState * state ) {
    aircraft_state_graph::node_t result;
    if ( state ) {
        try {
            result = __graph[ state->node_d() ];
        } catch ( const std::range_error & re ) {
            XPlane::log("ERROR: AircraftStateGraph::get_node_for( AircraftAbstractState * ), invalid descriptor");
        }
    }
    return result;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Вернуть структуру edge_t в зависимости от указателя на действие                            *
// *                                                                                                                   *
// *********************************************************************************************************************

aircraft_state_graph::edge_t AircraftStateGraph::get_edge_for( AircraftAbstractAction * action ) {

    aircraft_state_graph::edge_t result;
    if ( ! action ) {
        XPlane::log("ERROR: AircraftStateGraph::get_edge_for() called with null action pointer");
        return result;
    }
    
    try {
        result = __graph[ action->edge_d() ];
    } catch ( const std::range_error & re ) {
        XPlane::log("ERROR: AircraftStateGraph::get_edge_for( AircraftAbstractAction * ), invalid edge descriptor");
    }
    
    return result;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *            Действие было завершено, текущим состоянием становится то, куда приводит данное действие               *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftStateGraph::action_finished( xenon::AircraftAbstractAction * ptr_action ) {
    
    clear_actions_activity();
    
    // Это - не лишнее. Хоть ниже и проводится очистка на момент 
    // установки текущего состояния. Потому что дескриптор
    // состояния может оказаться не валидным.
    clear_states_activity();
    
    if ( ! ptr_action ) {
        XPlane::log("ERROR: AircraftStateGraph::action_finished called with null action pointer.");
        return;
    }
    
    // Запоминаем параметры для обеспечения плавности перехода между действиями.
    __previous_action_params = ptr_action->get_parameters();
    // Переход на следующее состояние, здесь он абсолютно однозначен.
    auto next_state = boost::target( ptr_action->edge_d(), __graph );
    
    try {
        set_active_state( next_state );
    } catch ( const std::range_error & re ) {
        XPlane::log("ERROR: AircraftStateGraph::action_finished, invalid descriptor " + to_string( next_state ));
    }
}
