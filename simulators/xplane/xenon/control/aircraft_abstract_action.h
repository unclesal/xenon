// *********************************************************************************************************************
// *                          Интерфейс некоего абстрактного действия (ребро графа состояний самолета).                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 14:00 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_state_graph_definition.h"
#include "abstract_aircrafter.h"

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
            
            inline action_parameters_t get_parameters() {
                return _params;
            };
            
            inline void set_parameters( const action_parameters_t & params ) {
                _params = params;
            };
            
        
        protected:
            
            aircraft_state_graph::graph_t::edge_descriptor _edge_d;            
            
            action_parameters_t _params;
            
            
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
