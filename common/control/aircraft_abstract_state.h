// *********************************************************************************************************************
// *                                            Абстрактное состояние самолета                                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 10:52 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_state_graph_definition.h"
#include "abstract_aircraft.h"

namespace xenon {
    
    class AircraftAbstractState {
        
        friend class AircraftStateGraph;
        
        public:
            
            AircraftAbstractState( AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd );
            virtual ~AircraftAbstractState() = default;
            
            aircraft_state_graph::graph_t::vertex_descriptor node_d() {
                return _node_d;
            }
            
        protected:
            
            AbstractAircraft * _ptr_acf;
            aircraft_state_graph::graph_t::vertex_descriptor _node_d;
            
            virtual void _internal_activate() = 0;
            
        private:            
            
            void __activate();
            
    };
    
}; // namespace xenon
