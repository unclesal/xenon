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
            
            aircraft_state_graph::graph_t::edge_descriptor edge_d() {
                return _edge_d;
            };
            
            inline aircraft_state_graph::action_parameters_t get_parameters() {
                return _params;
            };
            
            inline void set_parameters( const aircraft_state_graph::action_parameters_t & params ) {
                _params = params;
            };
            
        
        protected:
            
            aircraft_state_graph::graph_t::edge_descriptor _edge_d;
            AbstractAircraft * _ptr_acf;
            
            aircraft_state_graph::action_parameters_t _params;
            
            
            virtual void _internal_step( const float & elapsed_since_last_time ) = 0;
            virtual void _internal_start() = 0;
            
            /**
             * @short Вернуть нулевую точку полетного плана.
             */
            inline waypoint_t & _get_front_wp() {
                if ( _ptr_acf->_flight_plan.empty() ) {
                    XPlane::log("ERROR: AircraftAbstractAction::_get_front_wp(), but flight plan is empty");
                    return __fake_waypoint;
                }
                return _ptr_acf->_flight_plan.at(0);        
            };
            
            /**
             * @short Вернуть первую (следующую за нулевой) точку полетного плана.
             */
            inline waypoint_t & _get_first_wp() {
                if ( _ptr_acf->_flight_plan.size() >= 2 ) {
                    return _ptr_acf->_flight_plan.at(1);
                };
                XPlane::log("ERROR: AircraftAbstractAction::_get_first_wp(), but flight plan size=" + to_string( _ptr_acf->_flight_plan.size() ) );
                return __fake_waypoint;
            };
            
            inline location_t _get_acf_location() {
                return _ptr_acf->get_location();                
            };
            
            inline rotation_t _get_acf_rotation() {
                return _ptr_acf->get_rotation();
            };
            
            inline void _front_wp_reached() {
                if ( ! _ptr_acf->_flight_plan.empty() ) _ptr_acf->_flight_plan.pop_front();
                else XPlane::log("ERROR: AircraftAbstractAction::_front_wp_reached(), empty flight plan.");
            };
            
            /**
             * @short Нулевая точка плана - удаляется, а не приближается, как оно должно быть в "штатном" режиме.
             */
            
            inline bool _front_wp_recedes() {
                auto wp = _get_front_wp();
                return ( __previous_distance_to_front_wp < (int) _calculate_distance_to_wp( wp ) );
            };
            
            inline void _acf_will_on_ground( bool on_ground ) {
                _ptr_acf->set_will_on_ground( on_ground );
            };
            
            void _finish();
            
            inline double _calculate_distance_to_wp( const waypoint_t & wp ) {
                auto location = _get_acf_location();
                return xenon::distance( location, wp.location );
            };
            
            double _previous_distance_to_front_wp() {
                return __previous_distance_to_front_wp;
            };
            
        private:
            
            bool __started;
            bool __finished;
            
            waypoint_t __fake_waypoint;
            
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
            void __control_of_speed( const float & elapsed_since_last_call );
            void __control_of_angles( const float & elapsed_since_last_call );
            void __move_straight( const float & elapsed_since_last_call );

            
    }; // class AircraftAbstractDoes

}; // namespace xenon
