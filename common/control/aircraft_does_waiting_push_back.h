// *********************************************************************************************************************
// *                                             Самолет ожидает выталкивания.                                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 08 jul 2020 at 20:39 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_action.h"

namespace xenon {
    
    class AircraftDoesWaitingPushBack : public AircraftAbstractAction {
        
        public:
            
            AircraftDoesWaitingPushBack(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            ~AircraftDoesWaitingPushBack() override = default;
            
        protected:
            
            // Здесь нечего "шагать". Состояние меняться не будет.
            void _internal_step( const float & elapsed_since_last_call ) override {};
            void _internal_start() override;
            
        private:
        
    };// class AircraftDoesWaitingPushBack
}; // namespace xenon
