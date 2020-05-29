// *********************************************************************************************************************
// *                                            Абстрактное состояние самолета                                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 10:52 *
// *********************************************************************************************************************
#pragma once

#include "abstract_aircrafter.h"
#include "aircraft_state_graph_definition.h"

namespace xenon {
    
    class AircraftAbstractState : public AbstractAircrafter {
        
        friend class AircraftStateGraph;
        
        public:
            
            AircraftAbstractState( AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd );
            virtual ~AircraftAbstractState() = default;
            
            aircraft_state_graph::graph_t::vertex_descriptor node_d() {
                return _node_d;
            }
            
        protected:
            
            aircraft_state_graph::graph_t::vertex_descriptor _node_d;
            
            virtual void _internal_activate() = 0;
            
        private:            
            
            void __activate();
            
    };
    
}; // namespace xenon
