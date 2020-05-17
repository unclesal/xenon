// *********************************************************************************************************************
// *                          Интерфейс некоего абстрактного действия (ребро графа состояний самолета).                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 14:00 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_state_graph_definition.h"
#include "abstract_aircraft.h"

namespace xenon {
    
    class AircraftAbstractAction {
        
        friend class AircraftStateGraph;
        
        public:
            
            AircraftAbstractAction (
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            virtual ~AircraftAbstractAction() = default;
                                    
            bool is_started() {
                return __started;
            };
            
            bool is_finished() {
                return __finished;
            };                        
        
        protected:
            
            aircraft_state_graph::graph_t::edge_descriptor _edge_d;
            AbstractAircraft * _ptr_acf;
            /**
             * @short "Рывок", производная от ускорения.
             */
            double _tug;
            double _acceleration;
            double _target_acceleration;
            double _speed;
            double _target_speed;
            
            
            virtual void _internal_step( float elapsed_since_last_time ) = 0;
            virtual void _internal_start() = 0;
            
            waypoint_t & _get_front_wp();
            void _front_wp_reached();
            void _finish();
            
        private:
            
            bool __started;
            bool __finished;
            
            /** 
             * @short Полное время в секундах, проведенное в данном действии.
             */

            double __total_duration;
            double __total_distance;
            
            void __start();
            void __step( float elapsed_since_last_call );
            void __control_of_speed( float elapsed_since_last_call );
            void __move_straight( float elapsed_since_last_call );

            
    }; // class AircraftAbstractDoes

}; // namespace xenon
