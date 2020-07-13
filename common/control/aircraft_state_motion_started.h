// *********************************************************************************************************************
// *         Движение началось (т.е. ушел с парковки). Состояние - пустое, просто чтобы отличалось от парковки.        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 13 jul 2020 at 12:32 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_state.h"

namespace xenon {
    
    class AircraftStateMotionStarted : public AircraftAbstractState {
        
        public:
            
            AircraftStateMotionStarted(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd
            ) : AircraftAbstractState( ptr_acf, nd ) {
                
            };
            
            ~AircraftStateMotionStarted() override = default;
            
        protected:
            
            void _internal_activate() override {
                
            };
            
        private:
            
    };
    
};
