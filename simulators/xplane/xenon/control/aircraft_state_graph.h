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

#include "aircraft_state_parking.h"
#include "aircraft_state_ready_for_taxing.h"

#include "aircraft_does_nothing.h"
#include "aircraft_does_slow_taxing.h"
#include "aircraft_does_push_back.h"
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
            
            /**
             * @short Вернуть действие, исходящее из данного текущего состояния и имеющее определенный тип.
             */
            aircraft_state_graph::graph_t::edge_descriptor get_action_outgoing_from_current_state( const aircraft_action_t & action );
            
            void action_finished( AircraftAbstractAction * ptr_action );
            
            aircraft_state_graph::edge_t get_edge_for( AircraftAbstractAction * action );
            
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
            
            aircraft_state_graph::action_parameters_t __previous_action_params;
            
    };

}; // namespace xenon
