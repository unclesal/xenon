// *********************************************************************************************************************
// *                                 Граф состояний самолета и переходов между ними                                    *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 08:54 *
// *********************************************************************************************************************

#include "aircraft_state_graph.h"

#include "aircraft_state_airborned.h"
#include "aircraft_state_approach.h"
#include "aircraft_state_before_parking.h"
#include "aircraft_state_landed.h"
#include "aircraft_state_motion_started.h"
#include "aircraft_state_on_final.h"
#include "aircraft_state_on_hp.h"
#include "aircraft_state_on_the_fly.h"
#include "aircraft_state_parking.h"
#include "aircraft_state_ready_for_take_off.h"
#include "aircraft_state_ready_for_taxing.h"
#include "aircraft_state_runway_leaved.h"

#include "aircraft_does_becoming.h"
#include "aircraft_does_flying.h"
#include "aircraft_does_gliding.h"
#include "aircraft_does_landing.h"
#include "aircraft_does_lining_up.h"
#include "aircraft_does_nothing.h"
#include "aircraft_does_parking.h"
#include "aircraft_does_push_back.h"
#include "aircraft_does_slow_taxing.h"
#include "aircraft_does_start_motion.h"
#include "aircraft_does_take_off.h"
#include "aircraft_does_taxing.h"
#include "aircraft_does_taxing_stop.h"
#include "aircraft_does_waiting_push_back.h"
#include "aircraft_does_waiting_take_off_approval.h"

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
    
    // Состояние "начал движение" (сдвинулся с парковки)
    auto state_motion_started_d = __create_state< AircraftStateMotionStarted > ( ACF_STATE_MOTION_STARTED, "MotionStarted" );
    
    // Состояние "готов к рулежке".
    auto state_ready_for_taxing_d = __create_state< AircraftStateReadyForTaxing > ( 
        ACF_STATE_READY_FOR_TAXING, "ReadyForTaxing" 
    );
    
    // Состояние "предварительный старт".
    auto state_on_hp_d = __create_state< AircraftStateOnHP >( ACF_STATE_HP, "On HP" );
    
    // Состояние "готов к взлету" (исполнительный старт)
    auto state_ready_for_take_off_d = __create_state< AircraftStateReadyForTakeOff >(
        ACF_STATE_READY_FOR_TAKE_OFF, "ReadyForTakeOff"
    );
    // Состояние "взлетел".
    auto state_airborned_d = __create_state< AircraftStateAirborned > (
        ACF_STATE_AIRBORNED, "Airborned"
    );
    
    // Состояние "в полете"
    auto state_on_the_fly_d = __create_state< AircraftStateOnTheFly > (
        ACF_STATE_ON_THE_FLY, "OnTheFly"
    );
    
    // Состояние "на глиссаде" (подход, зашел в растр ILS)
    auto state_approach_d = __create_state< AircraftStateApproach > (
        ACF_STATE_APPROACH, "Approach"
    );
    
    // Состояние "на финальной прямой" (посадка)
    auto state_on_final_d = __create_state< AircraftStateOnFinal > (
        ACF_STATE_ON_FINAL, "OnFinal"
    );

    // Посадка выполнена (торможение закончено, остановился).
    auto state_landed_d = __create_state< AircraftStateLanded > (
        ACF_STATE_LANDED, "Landed"
    );

    // Освободил ВПП.
    auto state_rwy_leaved_d =  __create_state< AircraftStateRunwayLeaved > (
        ACF_STATE_RUNWAY_LEAVED, "RunwayLeaved"
    );

    // Заходит на парковку (последняя точка осталась, сама парковка)
    auto state_before_parking_d = __create_state< AircraftStateBeforeParking > (
        ACF_STATE_BEFORE_PARKING, "BeforeParking"
    );
    
    // ------------------------------------------------------------------------
    //           Действия, переход из одного состояния в другое
    // ------------------------------------------------------------------------
    
    // На парковке можно находиться бесконечно и ничего не делать. Это неправильно,
    // т.к. может привести к "зависанию" самолета. Но пока я ничего лучшего не придумал.
    __create_action< AircraftDoesNothing >( 
        ACF_DOES_NOTHING, "DoesNothingOnParking", state_parking_d, state_parking_d
    );
    
    // Еще на парковке можно ожидать выталкивания. Ничего не меняется, мы по-прежнему остаемся на парковке.
    __create_action< AircraftDoesWaitingPushBack > (
        ACF_DOES_WAITING_PUSH_BACK, "WaitingPushBack", state_parking_d, state_parking_d
    );
    
    // На парковке -> начал движение.
    __create_action< AircraftDoesStartMotion > (
        ACF_DOES_START_MOTION, "StartMotion", state_parking_d, state_motion_started_d
    );
    
    // Ничегонеделание - из состояния motion started.
    __create_action< AircraftDoesNothing > (
        ACF_DOES_NOTHING, "DoesNothingWhileMotioinStarted", state_motion_started_d, state_motion_started_d
    );

    // Из состояния "начал движение" можно перейти в состояние "готов к
    // рулежке" посредством либо выталкивания, либо собственного руления.
    // Зависит от того, где по курсу расположена начальная точка руления.

    __create_action< AircraftDoesSlowTaxing > (
        ACF_DOES_SLOW_TAXING, "SlowTaxing", state_motion_started_d, state_ready_for_taxing_d
    );
    __create_action< AircraftDoesPushBack >(
        ACF_DOES_PUSH_BACK, "PushBack", state_motion_started_d, state_ready_for_taxing_d
    );

    // Ничегонеделание - из состояния "готов к рулению".
    __create_action< AircraftDoesNothing > (
        ACF_DOES_NOTHING, "DoesNothingWhileReadyForTaxing", state_ready_for_taxing_d, state_ready_for_taxing_d
    );
    
    // Готов к рулению (т.е. происходит рулежка) - может быть остановлено в любой 
    // момент с переходом в то же самое состояние. Т.е. потом можно рулить дальше.
    __create_action<  AircraftDoesTaxingStop >(
        ACF_DOES_TAXING_STOP, "StopTaxing", state_ready_for_taxing_d, state_ready_for_taxing_d
    );
    
    // Из состояния "готов к рулению" в состояние "на
    // предварительном старте" можно перейти рулением.

    __create_action< AircraftDoesTaxing >(
        ACF_DOES_NORMAL_TAXING, "TaxingToHP", state_ready_for_taxing_d, state_on_hp_d
    );
    
    // Ничегонеделание - на предварительном старте.
    __create_action< AircraftDoesNothing > (
        ACF_DOES_NOTHING, "DoesNothingOnHP", state_on_hp_d, state_on_hp_d
    );
    
    // На предварительном старте можно задержаться, ожидая разрешение на взлет.
    __create_action< AircraftDoesWaitingTakeOffApproval > (
        ACF_DOES_WAITING_TAKE_OFF_APPROVAL, "WaitingTakeOffApproval", state_on_hp_d, state_on_hp_d
    );
    
    // Из состояния "на предварительном старте" можно перейти в состояние
    // "готов к взлету" - выравниванием (lining up)
    __create_action< AircraftDoesLiningUp > (
        ACF_DOES_LINING_UP, "LiningUp", state_on_hp_d, state_ready_for_take_off_d
    );
    
    // Ничегонеделание - в состоянии "готов к взлету" (исполнительный старт)
    __create_action< AircraftDoesNothing > (
        ACF_DOES_NOTHING, "DoesNothingWhileReadyForTakeOff", state_ready_for_take_off_d, state_ready_for_take_off_d
    );
    
    // "Готов к взлету" (исполнительный старт) может закольцеваться действием
    // ожидания разрешения на взлет.
    
    __create_action< AircraftDoesWaitingTakeOffApproval > (
        ACF_DOES_WAITING_TAKE_OFF_APPROVAL, "WaitingTakeOffApproval", state_ready_for_take_off_d, state_ready_for_take_off_d
    );
    
    // Из состояния "готов к взлету" можно перейти в
    // состояние airborned путем взлета.
    __create_action< AircraftDoesTakeOff > (
        ACF_DOES_TAKE_OFF, "TakeOff", state_ready_for_take_off_d, state_airborned_d
    );
    
    // Ничегонеделание в состоянии взлетел.
    __create_action< AircraftDoesNothing > (
        ACF_DOES_NOTHING, "DoesNothingWhileAirborned", state_airborned_d, state_airborned_d
    );
    
    // Из состояния airborned в состояние "в полете" переходим становлением.
    __create_action< AircraftDoesBecoming > (
        ACF_DOES_BECOMING, "Becoming", state_airborned_d, state_on_the_fly_d
    );
    
    // Ничегонеделание - в состоянии полета.
    __create_action< AircraftDoesNothing > (
        ACF_DOES_NOTHING, "DoesNothingOnTheFly", state_on_the_fly_d, state_on_the_fly_d
    );
    
    // Из состояния полета происходит действие полета, которое закончится только выходом на глиссаду.
    __create_action< AircraftDoesFlying > (
        ACF_DOES_FLYING, "Flying", state_on_the_fly_d, state_approach_d
    );
    
    // Ничегонеделание - в состоянии подхода.
    __create_action< AircraftDoesNothing> (
        ACF_DOES_NOTHING, "DoesNothingWhileApproach", state_approach_d, state_approach_d
    );
    
    // Из состояния "на глиссаде" в состояние "на финальной прямой" выходим глайдингом.
    __create_action< AircraftDoesGliding > (
        ACF_DOES_GLIDING, "Gliding", state_approach_d, state_on_final_d
    );
    
    // Ничегонеделание - на финальной прямой.
    __create_action< AircraftDoesNothing > (
        ACF_DOES_NOTHING, "DoesNothingOnFinal", state_on_final_d, state_on_final_d
    );
    
    // Из состояния "на финальной прямой" в состояние "приземлился" - выходим посадкой.
    __create_action < AircraftDoesLanding > (
        ACF_DOES_LANDING, "Landing",  state_on_final_d, state_landed_d
    );
    
    // На "приземлился" он может постоять, ожидая прихода полетного плана с рулением от агента.
    __create_action< AircraftDoesNothing > (
        ACF_DOES_NOTHING, "DoesNothingWhileLanded", state_landed_d, state_landed_d 
    );
    
    // Из состояния "приземлился" в состояние "освободил ВПП" выходим рулением.
    __create_action< AircraftDoesTaxing > (
        ACF_DOES_NORMAL_TAXING, "TaxingFromRunway", state_landed_d, state_rwy_leaved_d
    );
    
    // Ничегонеделание в состоянии "освободил ВПП"
    __create_action< AircraftDoesNothing > (
        ACF_DOES_NOTHING, "DoesNothingWhenLeavedRWY", state_rwy_leaved_d, state_rwy_leaved_d
    );

    // Из состояния "освободил ВПП" до "почти запарковался" - все еще руление.
    __create_action< AircraftDoesTaxing > (
        ACF_DOES_NORMAL_TAXING, "TaxingToGate", state_rwy_leaved_d, state_before_parking_d
    );
    
    // Ничегонеделание - при заходе на парковку.
    __create_action < AircraftDoesNothing >(
        ACF_DOES_NOTHING, "DoesNothingBeforeParking", state_before_parking_d, state_before_parking_d
    );

    // Замыкание графа. Из состояния "почти запарковался" в состояние "на парковке".
    // Тут два варианта. Либо заруливает сам, либо его заталкивают.

    __create_action< AircraftDoesParking > (
        ACF_DOES_PARKING, "Parking", state_before_parking_d, state_parking_d
    );

    __create_action< AircraftDoesPushBack > (
        ACF_DOES_PUSH_BACK, "PushBackToParking", state_before_parking_d, state_parking_d
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
        __ptr_acf->vcl_condition.current_state = __graph[ nd ].state;
        __current_state = ( AircraftAbstractState * ) __graph[ nd ].ptr_state_class;
        
        if ( __current_state ) {
            __current_state->__activate();
        }
        
#ifdef DEBUG        
        Logger::log( __ptr_acf->vcl_condition.agent_name + ", set active state " + __graph[ nd ].name );
#endif        
        
#ifdef INSIDE_AGENT            
        if ( __ptr_acf->agent ) __ptr_acf->agent->state_changed( __current_state );
#endif        
    } catch ( const std::range_error & re ) {
        Logger::log("ERROR: AircraftStateGraph::set_active_state called with incorrect vertex descriptor");
    }

};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                    Сделать текущим состояние по его константе                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftStateGraph::set_active_state( const aircraft_state_t & state ) {
    
    if ( ! current_state_is( state ) ) {
        auto state_d = get_node_for( state );
        set_active_state( state_d );
    }
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Сделать текущим действие от его enum'а                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftStateGraph::set_active_action(const xenon::aircraft_action_t & action) {
    
    if ( current_action_is( action ) ) return;
            
    aircraft_state_graph::graph_t::edge_descriptor action_descriptor 
        = get_action_outgoing_from_current_state( action );
        
    aircraft_state_graph::graph_t::edge_descriptor fake;
    if ( action_descriptor == fake ) {
        auto node = get_node_for( __current_state );
        Logger::log(
            __ptr_acf->vcl_condition.agent_name 
            + ", AircraftStateGraph:set_active_action(" + action_to_string( action )
            + "), no outgoing from current state " + node.name + ", try set DOES_NOTHING..."
        );
        
        // Только не себя саму: можно получить замкнутую реентерабельность.
        aircraft_state_graph::graph_t::edge_descriptor action_descriptor 
            = get_action_outgoing_from_current_state( ACF_DOES_NOTHING );
            
        if ( action_descriptor != fake ) {
            set_active_action( action_descriptor );
        };
        
        return;
    }
            
    set_active_action( action_descriptor );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Сделать данное действие - текущим                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftStateGraph::set_active_action( const aircraft_state_graph::graph_t::edge_descriptor & ed ) {
    
    aircraft_state_graph::graph_t::edge_descriptor fake;
    
    if ( ed == fake ) {
        Logger::log(
            __ptr_acf->vcl_condition.agent_name 
            + ", try to set_active_action( nullptr)"
        );
    };
    
    auto descriptor = ed;
    
    if ( __ptr_acf->flight_plan.is_empty() ) {
        // Если полетный план пустой, то нет смысла устанавливать какое-то действие.
        descriptor = get_action_outgoing_from_current_state( ACF_DOES_NOTHING );
        if ( descriptor == fake ) {
            Logger::log(__ptr_acf->vcl_condition.agent_name + ", set_active_action: can not find ACF_DOES_NOTHING" );
            return;
        };
        
    };
    
    clear_actions_activity();
    
    try {

#ifdef DEBUG        
        Logger::log( __ptr_acf->vcl_condition.agent_name + ", set_active_action " + __graph[ descriptor ].name );
#endif        
        __graph[ descriptor ].current_action = true;
        __ptr_acf->vcl_condition.current_action = __graph[ descriptor ].action;
        __current_action = (AircraftAbstractAction * ) __graph[ descriptor ].ptr_does_class;

        // Это единственное место, где должен вызываться старт. Поэтому сам старт сделан приватным.
        if ( __current_action ) __current_action->__start();
#ifdef INSIDE_AGENT        
        if ( __ptr_acf->agent ) __ptr_acf->agent->action_started( __current_action );
#endif        

    } catch ( const std::range_error & re ) {
        Logger::log("ERROR: AircraftStateGraph::set_active_action() called with invalid edge descriptor.");
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
        Logger::log("AircraftStateGraph::place_on_parking, incorrect node_d was found");
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
        Logger::log("ERROR: AircraftStateGraph::current_action_is(), invalid edge descriptor");
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
        Logger::log( "ERROR: AircraftStateGraph::current_state_is(), invalid node descriptor");
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
        Logger::log("ERROR: get_action_outgoing_from_current_state, " + string( re.what() ) );
    };
    
    auto name = __graph[ __current_state->node_d() ].name;
    Logger::log(__ptr_acf->vcl_condition.agent_name + ": no outgoing action was found from current " + name + " with type=" + action_to_string( action ));
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
            Logger::log("ERROR: AircraftStateGraph::get_node_for( AircraftAbstractState * ), invalid descriptor");
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
        Logger::log("ERROR: AircraftStateGraph::get_edge_for() called with null action pointer");
        return result;
    }
    
    try {
        result = __graph[ action->edge_d() ];
    } catch ( const std::range_error & re ) {
        Logger::log("ERROR: AircraftStateGraph::get_edge_for( AircraftAbstractAction * ), invalid edge descriptor");
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
    
    if ( ! ptr_action ) {
        Logger::log("ERROR: AircraftStateGraph::action_finished called with null action pointer.");
        return;
    }
    
    // Переход на следующее состояние, здесь он абсолютно однозначен.
    auto next_state = boost::target( ptr_action->edge_d(), __graph );
    auto next_node = __graph[ next_state ];
    
    if ( ! current_state_is ( next_node.state ) ) {
    
        // Это - не лишнее. Хоть ниже и проводится очистка на момент 
        // установки текущего состояния. Потому что дескриптор
        // состояния может оказаться не валидным.
        clear_states_activity();
        
        try {
            set_active_state( next_state );
        } catch ( const std::range_error & re ) {
            Logger::log("ERROR: AircraftStateGraph::action_finished, invalid descriptor " + to_string( next_state ));
        }
    }

}
