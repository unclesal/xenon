// *********************************************************************************************************************
// *                                 Граф состояний самолета и переходов между ними                                    *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 08:52 *
// *********************************************************************************************************************

#pragma once

#include <vector>

#include "abstract_aircraft.h"
#include "aircraft_state_graph_definition.h"
#include "aircraft_abstract_state.h"

#include "aircraft_state_airborned.h"
#include "aircraft_state_on_final.h"
#include "aircraft_state_on_hp.h"
#include "aircraft_state_parking.h"
#include "aircraft_state_ready_for_take_off.h"
#include "aircraft_state_ready_for_taxing.h"
#include "aircraft_state_runway_leaved.h"

#include "aircraft_does_flying.h"
#include "aircraft_does_landing.h"
#include "aircraft_does_lining_up.h"
#include "aircraft_does_nothing.h"
#include "aircraft_does_push_back.h"
#include "aircraft_does_runway_leaving.h"
#include "aircraft_does_slow_taxing.h"
#include "aircraft_does_take_off.h"
#include "aircraft_does_taxing.h"

namespace xenon {

    class AircraftStateGraph {

        public:

            AircraftStateGraph( AbstractAircraft * ptr_acf );
            ~AircraftStateGraph() = default;
                        
            void place_on_parking();
            
            void clear_states_activity();
            void clear_actions_activity();
            
            void set_active_state( const aircraft_state_graph::graph_t::vertex_descriptor & nd );
            void set_active_state( const aircraft_state_t & state );
            void set_active_action( const aircraft_state_graph::graph_t::edge_descriptor & ed );
            
            void update( float elapsed_since_last_call );
            
            bool current_state_is( const aircraft_state_t & state );
            bool current_action_is( const aircraft_action_t & action );
            
            aircraft_state_graph::graph_t::vertex_descriptor get_node_for( const aircraft_state_t & state );
            aircraft_state_graph::node_t get_node_for( AircraftAbstractState * ptr_state );
            
            /**
             * @short Получить действие по заданным состояниям и определителю действия.
             * @param from_state состояние из которого
             * @param to_state состояние, в которое 
             * @param action сам определитель действия.
             */
            aircraft_state_graph::graph_t::edge_descriptor get_action_for( 
                const aircraft_state_t & from_state, 
                const aircraft_state_t & to_state, 
                const aircraft_action_t & action 
            );
            
            aircraft_state_graph::graph_t::edge_descriptor get_action_for(
                const aircraft_state_graph::graph_t::vertex_descriptor & v_from,
                const aircraft_state_graph::graph_t::vertex_descriptor & v_to,
                const aircraft_action_t & action 
            );
            
            aircraft_state_graph::edge_t get_edge_for( AircraftAbstractAction * action );

            inline aircraft_state_graph::edge_t get_edge_for( const aircraft_state_graph::graph_t::edge_descriptor & ed ) {
                try {
                    aircraft_state_graph::edge_t edge = __graph[ ed ];
                    return edge;
                } catch ( const std::range_error & re ) {
                    Logger::log("ERROR: AircraftStateGraph::get_edge_for( edge_descriptor & ), invalid descriptor");
                    return aircraft_state_graph::edge_t();
                };
            };

            
            /**
             * @short Вернуть действие, исходящее из данного текущего состояния и имеющее определенный тип.
             */
            aircraft_state_graph::graph_t::edge_descriptor get_action_outgoing_from_current_state( const aircraft_action_t & action );
            
            void action_finished( AircraftAbstractAction * ptr_action );
                        
            AircraftAbstractState * get_current_state() {
                return __current_state;
            };
            
            AircraftAbstractAction * get_current_action() {
                return __current_action;
            };
                        
        private:
            
            // Граф состояний и переходов между ними (действий).
            aircraft_state_graph::graph_t __graph;
            // Классы состояний. Порождаются один раз и сидят в коллекции.
            vector< AircraftAbstractState * > __states;
            vector< AircraftAbstractAction * > __actions;
                        
            AbstractAircraft * __ptr_acf;
            
            AircraftAbstractState * __current_state;
            AircraftAbstractAction * __current_action;
            
            // action_parameters_t __previous_action_params;
            
            // Добавить узел графа (состояние) и класс реализации этого состояния.
            template < class T >
            aircraft_state_graph::graph_t::vertex_descriptor __create_state(
                const aircraft_state_t & state, std::string name
            ) {
                // Сам "узел", он же "vertex".
                aircraft_state_graph::node_t node;
                node.state = state;
                node.name = name;
                // Его дескриптор.
                auto vd = boost::add_vertex( node, __graph );
                // Класс состояния
                AircraftAbstractState * ptr_class = new T( __ptr_acf, vd );
                node.ptr_state_class = ptr_class;
                __states.push_back( ptr_class );
                __graph[ vd ] = node;
                // Возвращаем - дескриптор созданного узла.
                return vd;
            };
            
            // Добавить дугу графа (действие) и класс реализации этого действия.
            template < class T >
            aircraft_state_graph::graph_t::edge_descriptor __create_action(
                const aircraft_action_t & action, std::string name,
                aircraft_state_graph::graph_t::vertex_descriptor v_from,
                aircraft_state_graph::graph_t::vertex_descriptor v_to
            ) {
                aircraft_state_graph::edge_t edge;
                edge.action = action;
                edge.name = name;
                auto added_edge = boost::add_edge( v_from, v_to, __graph );
                AircraftAbstractAction * ptr_class = new T( __ptr_acf, added_edge.first );
                edge.ptr_does_class = ptr_class;
                __graph[ added_edge.first ] = edge;
                __actions.push_back( ptr_class );
                return added_edge.first;
            };
            
    };

}; // namespace xenon
