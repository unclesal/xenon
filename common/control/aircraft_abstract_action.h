// *********************************************************************************************************************
// *                          Интерфейс некоего абстрактного действия (ребро графа состояний самолета).                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 14:00 *
// *********************************************************************************************************************
#pragma once

#include <string>

#include "aircraft_state_graph_definition.h"
#include "logger.h"
#include "abstract_aircraft.h"

#define PREVIOUS_ARRAY_SIZE 50

namespace xenon {
    
    class AircraftAbstractAction {
        
        friend class AircraftStateGraph;
        
        public:
            
            AircraftAbstractAction (
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            virtual ~AircraftAbstractAction() = default;
                                                            
            aircraft_state_graph::graph_t::edge_descriptor edge_d() {
                return _edge_d;
            };

        
        protected:
            
            AbstractAircraft * _ptr_acf;
            
            aircraft_state_graph::graph_t::edge_descriptor _edge_d;                        
            
            virtual void _internal_step( const float & elapsed_since_last_call ) = 0;
            virtual void _internal_start() = 0;
                        
            inline void _finish() {   
                __finished = true;
                _ptr_acf->action_finished( this );                
            };
                        
                        
            /**
             * @short Нулевая точка плана - удаляется, а не приближается, как оно должно быть в "штатном" режиме.
             */
            
//             inline bool _front_wp_recedes() {
//                 auto wp = _get_front_wp();
//                 return ( _previous_distance_to_front_wp() < _calculate_distance_to_wp( wp ) );
//             };

            
//             float _previous_distance_to_front_wp() {
//                 float result;
//                 for ( int i=0; i<PREVIOUS_ARRAY_SIZE; i++ ) {
//                     result += __previous_distance_to_front_wp[i];
//                 };
//                 result /= (float) PREVIOUS_ARRAY_SIZE;
//                 return result;
//             };
            
            
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
            
            /**
             * @short Получить (не нормированную, как есть) разницу между текущим курсом и курсом точки привода.
             */
            double _get_delta_to_target_heading( const waypoint_t & wp );
            
            /**
             * @short Получить (не нормируя, как оно есть) разницу между текущим курсом и азимутом на указанную точку.
             */
            double _get_delta_bearing( const waypoint_t & wp );
            
            /**
             * @short Вычисление точки начала поворота при рулении.
             * И если точка достигнута, то установка параметров этого самого разворота.
             */
            bool _taxi_turn_started( const waypoint_t & destination );
            
            /**
             * @short Применить торможение при рулении.
             * @param to_speed целевая скорость, до которой надо затормозить.
             * @param seconds за сколько секунд планируется затормозить.
             */
            
            void _taxi_breaking( const float & to_speed, const float & for_seconds );

            
            /** 
             * @short Полное время в секундах, проведенное в данном действии.
             */

            double _total_duration;
            double _total_distance;
            
            void _control_of_flaps();
            
        private:
                        
            /**
             * @short Дистанция до следующей точки полетного плана - в предыдущем "шаге" цикла.
             * С точностью до метра. Иначе там набежки бывают, если оно double. 
             */
            // int __previous_distance_to_front_wp[ PREVIOUS_ARRAY_SIZE ];
            
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
            bool __finished;

            
    }; // class AircraftAbstractDoes

}; // namespace xenon
