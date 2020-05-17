// *********************************************************************************************************************
// *                                            Абстрактное состояние самолета                                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 10:52 *
// *********************************************************************************************************************
#pragma once

#include "abstract_aircraft.h"
#include "aircraft_state_graph_definition.h"

namespace xenon {
    
    class AircraftAbstractState {
        
        public:
            
            AircraftAbstractState( AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd );
            virtual ~AircraftAbstractState() = default;
            
            aircraft_state_graph::graph_t::vertex_descriptor nd() {
                return _node_d;
            }
            
        protected:
            
            aircraft_state_graph::graph_t::vertex_descriptor _node_d;
            AbstractAircraft * _ptr_acf;
            
        private:            
            
            
            
    };
    
}; // namespace xenon
