// *********************************************************************************************************************
// *                          Выталкивание самолета (действие, ребро графа состояний самолета)                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 14:22 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_action.h"

namespace xenon {
    
    class AircraftDoesPushBack : public AircraftAbstractAction {
        
        public:
            
            AircraftDoesPushBack(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            ~AircraftDoesPushBack() override = default;
            
        protected:
            
            void _internal_step( float elapsed_since_last_time ) override;
            void _internal_start() override;
        
    }; // class AircraftDoesPushBack
    
}; // namespace xenon
