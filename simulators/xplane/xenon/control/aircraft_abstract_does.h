// *********************************************************************************************************************
// *                          Интерфейс некоего абстрактного действия (ребро графа состояний самолета).                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 14:00 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_state_graph_definition.h"
#include "abstract_aircraft.h"

namespace xenon {
    
    class AircraftAbstractDoes {
        
        public:
            
            AircraftAbstractDoes (
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            virtual ~AircraftAbstractDoes() = default;
            
            void start();
            void step( float elapsed_since_last_time );
            
            bool is_started() {
                return __started;
            };
            
            bool is_finished() {
                return __finished;
            };                        
        
        protected:
            
            aircraft_state_graph::graph_t::edge_descriptor _edge_d;
            AbstractAircraft * _ptr_acf;
            
            virtual void _internal_step( float elapsed_since_last_time ) = 0;
            virtual void _internal_start() = 0;
            
            void _finish();
            
        private:
            
            bool __started;
            bool __finished;
            
            /** 
             * @short Полное время в секундах, проведенное в данном действии.
             */

            double __total_duration;
            
    }; // class AircraftAbstractDoes

}; // namespace xenon
