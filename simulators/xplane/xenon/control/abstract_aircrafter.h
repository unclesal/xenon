// *********************************************************************************************************************
// *                 Предок действий и состояний, который позволяет работать с абстрактным самолетом                   *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 23 may 2020 at 12:41 *
// *********************************************************************************************************************
#pragma once

#include "abstract_aircraft.h"
#include "xplane.hpp"

namespace xenon {
    
    class AbstractAircrafter {
        
        public:
            
            AbstractAircrafter( AbstractAircraft * ptr_acf ) {
                _ptr_acf = ptr_acf;
            };
            
            virtual ~AbstractAircrafter() = default;
            
        protected:
            
            AbstractAircraft * _ptr_acf;
            
            /**
             * @short Вернуть нулевую точку полетного плана.
             */
            inline waypoint_t & _get_front_wp() {
                if ( _ptr_acf->_flight_plan.empty() ) {
                    // XPlane::log("ERROR: AbstractAircrafter::_get_front_wp(), but flight plan is empty");
                    return __fake_waypoint;
                }
                return _ptr_acf->_flight_plan.at(0);        
            };
            
            /**
             * @short Заместить, либо добавить нулевую точку полетного плана.
             */
            
            inline void _set_front_wp( const waypoint_t & wp ) {
                if ( !_ptr_acf->_flight_plan.empty() ) _ptr_acf->_flight_plan[0] = wp;
                else _ptr_acf->_flight_plan.push_front( wp );
            };
            
            /**
             * @short Вернуть первую (следующую за нулевой) точку полетного плана.
             */
            inline waypoint_t & _get_first_wp() {
                if ( _ptr_acf->_flight_plan.size() >= 2 ) {
                    return _ptr_acf->_flight_plan.at(1);
                };
                XPlane::log("ERROR: AbstractAircrafter::_get_first_wp(), but flight plan size=" + to_string( _ptr_acf->_flight_plan.size() ) );
                return __fake_waypoint;
            };
            
            inline location_t _get_acf_location() {
                return _ptr_acf->get_location();                
            };
            
            inline position_t _get_acf_position() {
                auto location = _get_acf_location();
                return XPlane::location_to_position(location);
            };
            
            inline rotation_t _get_acf_rotation() {
                return _ptr_acf->get_rotation();
            };
                        
            inline aircraft_parameters_t & _get_acf_parameters() {
                return _ptr_acf->_params;
            };
            
            inline void _front_wp_reached() {
                if ( ! _ptr_acf->_flight_plan.empty() ) _ptr_acf->_flight_plan.pop_front();
                else XPlane::log("ERROR: AbstractAircrafter::_front_wp_reached(), empty flight plan.");                
            };
                        
            inline double _calculate_distance_to_wp( const waypoint_t & wp ) {
                auto location = _get_acf_location();
                return xenon::distance( location, wp.location );
            };
            
            /**
             * @short Посчитать расстояние до "достаточно крутого поворота"
             */

            inline double _calculate_distance_to_turn() {
    
                auto location = _get_acf_location();
                for ( int i=0; i<_ptr_acf->_flight_plan.size(); i++ ) {
                    auto wp = _ptr_acf->_flight_plan.at(i);
                    double change = wp.incomming_heading - wp.outgoing_heading;
                    if ( change >= 20.0 ) {
                        return xenon::distance(location, wp.location);
                    }
                }
                return 0.0;
            };

            /**
             * @short Посчитать расстояние до точки, которая принадлежит ВПП
             */

            inline double _calculate_distance_to_runway() {
                auto location = _get_acf_location();
                for ( int i=0; i<_ptr_acf->_flight_plan.size(); i++ ) {
                    auto wp = _ptr_acf->_flight_plan.at(i);
                    if ( wp.type == WAYPOINT_RUNWAY ) {
                        return xenon::distance( location, wp.location );
                    }
                };
                return 0.0;
            };
            
            /**
             * @short Действие было завершено.
             */
            
            inline void _action_has_been_finished( void * ptr_action ) {
                _ptr_acf->_action_finished( ptr_action );
            };
            
            inline bool _is_flight_plan_empty() {
                return _ptr_acf->_flight_plan.empty();
            };

            
        private:
            
            waypoint_t __fake_waypoint;

    };
    
}; // namespace xenon
