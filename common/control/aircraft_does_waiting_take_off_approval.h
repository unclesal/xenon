// *********************************************************************************************************************
// *                                        Действие ожидания разрешения на взлет                                      *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 10 jul 2020 at 11:34 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_action.h"

namespace xenon {
    
    class AircraftDoesWaitingTakeOffApproval : public AircraftAbstractAction {
    
        public:
            
            AircraftDoesWaitingTakeOffApproval(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            ~AircraftDoesWaitingTakeOffApproval() override = default;
            
        protected:
            
            void _internal_step( const float & elapsed_since_last_call ) override;
            void _internal_start() override;
            
        private:                        
        
    };
    
}; // namespace xenon

