// *********************************************************************************************************************
// *                          Интерфейс некоего абстрактного действия (ребро графа состояний самолета).                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 14:00 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_state_graph_definition.h"
#include "abstract_aircrafter.h"
#include "logger.h"

#define PREVIOUS_ARRAY_SIZE 50

namespace xenon {
    
    class AircraftAbstractAction : public AbstractAircrafter {
        
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
            
            aircraft_state_graph::graph_t::edge_descriptor edge_d() {
                return _edge_d;
            };

        
        protected:
            
            aircraft_state_graph::graph_t::edge_descriptor _edge_d;                        
            
            virtual void _internal_step( const float & elapsed_since_last_call ) = 0;
            virtual void _internal_start() = 0;
                        
            inline void _finish() {
                __finished = true;
                _action_has_been_finished( this );
            };
                        
            /**
             * @short Нулевая точка плана - удаляется, а не приближается, как оно должно быть в "штатном" режиме.
             */
            
            inline bool _front_wp_recedes() {
                auto wp = _get_front_wp();
                return ( __previous_distance_to_front_wp < (int) _calculate_distance_to_wp( wp ) );
            };

            
            double _previous_distance_to_front_wp() {
                return __previous_distance_to_front_wp;
            };
            
            
            /** 
             * @short Пропорциональное подруливание на нулевую точку полетного плана по курсу.
             * @param elapsed_since_last_call Квант времени, за который считаем приращение.
             * @param kp пропорциональный коэффициент.
             */

            void _head_steering(float elapsed_since_last_call, double kp);
            
            /**
             * @short Подруливание на курс - в "авиационной" реализации.
             */
            void _head_bearing( const waypoint_t & wp );
            
            void _altitude_adjustment( const float & target_altitude, const float & time_to_achieve );
            void _speed_adjustment( const float & target_speed, const float & time_to_achieve );

            
        private:
            
            bool __started;
            bool __finished;                        
            
            /** 
             * @short Полное время в секундах, проведенное в данном действии.
             */

            double __total_duration;
            double __total_distance;
            
            /**
             * @short Дистанция до следующей точки полетного плана - в предыдущем "шаге" цикла.
             * С точностью до метра. Иначе там набежки бывают, если оно double. 
             */
            int __previous_distance_to_front_wp;
            
            // Массив предыдущих значений для вычисления интегральной составляющей по курсу
            double __previous_heading_delta[ PREVIOUS_ARRAY_SIZE ];
            
            void __start();
            void __step( const float & elapsed_since_last_call );
            void __control_of_speeds( const float & elapsed_since_last_call );
            void __control_of_angles( const float & elapsed_since_last_call );
            void __control_of_one_value( 
                const float & elapsed_since_last_call, float & acceleration, const float & endpoint, float & controlled_value, bool & changed 
            );
            void __move_straight( const float & elapsed_since_last_call );

            
    }; // class AircraftAbstractDoes

}; // namespace xenon
