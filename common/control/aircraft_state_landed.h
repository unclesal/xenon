// *********************************************************************************************************************
// *                                    Приземлился ( закончил фазу торможения на ВПП )                                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 03 jun 2020 at 10:56 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_state.h"
#include "airport.h"

namespace xenon {
    
    class AircraftStateLanded : public AircraftAbstractState {
        
        public:
            
            AircraftStateLanded(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd
            );
            
            ~AircraftStateLanded() override = default;
            
        protected:
            
            void _internal_activate() override;

        private:
        
    }; // class AircraftStateLanded
    
}; // namespace xenon
