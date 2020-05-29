// *********************************************************************************************************************
// *                                        Состояние самолета "готов к рулежке"                                       *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 12:20 *
// *********************************************************************************************************************

#pragma once
#include "aircraft_abstract_state.h"

namespace xenon {
    class AircraftStateReadyForTaxing : public AircraftAbstractState {
        
        public:
            
            AircraftStateReadyForTaxing(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd 
            );
            virtual ~AircraftStateReadyForTaxing() = default;
            
    protected:
        
        void _internal_activate() override {};
            
    };
}; // namespace xenon
