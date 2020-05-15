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

namespace xenon {

    class AircraftStateGraph {

        public:

            AircraftStateGraph( AbstractAircraft * ptr_acf );
            ~AircraftStateGraph() = default;
                        
            void place_on_parking( const waypoint_t & wp );
            
            void clear_states_activity();
            void clear_actions_activity();
            
            void set_active_state( const aircraft_state_graph::graph_t::vertex_descriptor & nd );
            void set_active_action( const aircraft_state_graph::graph_t::edge_descriptor & ed );
            
            void update( float elapsed_since_last_call );
                        
        private:
            // Граф состояний и переходов между ними (действий).
            aircraft_state_graph::graph_t __graph;
            // Классы состояний. Порождаются один раз и сидят в коллекции.
            vector< AircraftAbstractState * > __states;
            vector< AircraftAbstractAction * > __actions;
                        
            AbstractAircraft * __ptr_acf;
            
            AircraftAbstractState * __current_state;
            AircraftAbstractAction * __current_action;
            
    };

}; // namespace xenon
