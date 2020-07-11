// *********************************************************************************************************************
// *                                 Состояние подхода к ВПП с целью посадки (зашел в растр ILS                        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 11 jul 2020 at 08:51 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_state.h"

namespace xenon {
    
    class AircraftStateApproach : public AircraftAbstractState {
        
        public:
            
            AircraftStateApproach(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd
            );
            
            ~AircraftStateApproach() override = default;
            
        protected:
            
            void _internal_activate() override {};
            
        private:
    };
    
}; // namespace xenon
