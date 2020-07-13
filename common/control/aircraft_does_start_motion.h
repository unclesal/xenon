// *********************************************************************************************************************
// *            Начал движение (ушел с парковки). Действие - пустое, просто чтобы привести в другое состояние          *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 13 jul 2020 at 12:59 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_action.h"

namespace xenon {
    
    class AircraftDoesStartMotion : public AircraftAbstractAction {
        
        public:
            
            AircraftDoesStartMotion(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            ) : AircraftAbstractAction( ptr_acf, edge_d ) {
                
            };
            
            ~AircraftDoesStartMotion() override = default;
            
        protected:
            
            void _internal_step( const float & elapsed_since_last_call ) override {
                
            };
            
            void _internal_start() override {
                _finish();
            };
            
        private:
            
    };
    
};
