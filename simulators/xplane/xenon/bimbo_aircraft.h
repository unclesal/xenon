// *********************************************************************************************************************
// *                                        "Ботовый" (multiplayer) самолет.                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 01 may 2020 at 12:06 *
// *********************************************************************************************************************
#pragma once

#include <string>
#include <deque>

// X-Plane includes
#include "XPLMScenery.h"
#include "XPLMInstance.h"

// XPMP2 includes
#include "XPMPAircraft.h"

// My own includes
#include "structures.h"
#include "constants.h"
#include "aircraft_state_graph.h"
#include "abstract_aircraft.h"
#include "xplane.hpp"

using namespace XPMP2;

namespace xenon {

    class BimboAircraft : public AbstractAircraft, public XPMP2::Aircraft {

        public:

            struct actuator_motion_t {

                // Полное время прохождения от верхней конечной
                // точки до нижней, берется из констант.
                float full_time = 1.0;
                // Затребовано движение или нет.
                bool requested = false;
                // Конечная точка, до которой хотим дойти в текущем цикле.
                float endpoint = 0.0;

            };

            BimboAircraft(
                const std::string & icao_type,
                const std::string & icao_airline,
                const std::string & livery
            );

            ~BimboAircraft() override = default;

            position_t get_position() override;
            position_with_angles_t get_position_with_angles();
            void set_position( const position_t & position ) override;
            rotation_t get_rotation() override;
            void set_rotation( const rotation_t & rotation ) override;

            // Перекрытая функция XPMP2::Aircraft
            void UpdatePosition(float elapsed_since_last_call, int fl_counter) override ;

            // Освещение вкл-выкл
            void set_taxi_lites(bool on) override {
                on ? v[ V_CONTROLS_TAXI_LITES_ON ] = 1.0 : v[ V_CONTROLS_TAXI_LITES_ON ] = 0.0;
            };
            
            void set_landing_lites(bool on) override {
                on ? v[ V_CONTROLS_LANDING_LITES_ON ] = 1.0 : v[ V_CONTROLS_LANDING_LITES_ON ] = 0.0;
            };
            
            void set_beacon_lites(bool on) override {
                on ? v[ V_CONTROLS_BEACON_LITES_ON ] = 1.0 : v[ V_CONTROLS_BEACON_LITES_ON ] = 0.0;    
            };
            
            void set_strobe_lites(bool on) override {
                on ? v[ V_CONTROLS_STROBE_LITES_ON ] = 1.0 : v[ V_CONTROLS_STROBE_LITES_ON ] = 0.0;    
            };
            void set_nav_lites(bool on) override {
                on ? v[ V_CONTROLS_NAV_LITES_ON ] = 1.0 : v[ V_CONTROLS_NAV_LITES_ON ] = 0.0;
            };
            
            void set_gear_down( bool down ) override {
                down ? __actuators[ V_CONTROLS_GEAR_RATIO ].endpoint = 1.0 : __actuators[ V_CONTROLS_GEAR_RATIO ].endpoint = 0.0;
                __actuators[  V_CONTROLS_GEAR_RATIO ].requested = true;
            };
            
            void set_reverse_on( bool on ) override { 
                on ? __actuators[ V_CONTROLS_THRUST_REVERS ].endpoint = 1.0 : __actuators[ V_CONTROLS_THRUST_REVERS ].endpoint = 0.0;
                __actuators[ V_CONTROLS_THRUST_REVERS ].requested = true;
            };
            
            void set_flaps_position( const float & position ) override {
                __actuators[ V_CONTROLS_FLAP_RATIO ].endpoint = position;
                __actuators[ V_CONTROLS_FLAP_RATIO ].requested = true;
            };
            
            void set_speed_brake_position( const float & position ) override {
                __actuators[ V_CONTROLS_SPEED_BRAKE_RATIO ].endpoint = position;
                __actuators[ V_CONTROLS_SPEED_BRAKE_RATIO ].requested = true;
            };
            
            void set_thrust_value( const float & value ) override {                
                __actuators[ V_CONTROLS_THRUST_RATIO ].endpoint = value;
                __actuators[ V_CONTROLS_THRUST_RATIO ].requested = true;
            };
            
            void set_will_on_ground( bool on_ground ) override {
                bClampToGround = on_ground;
            };
            
            bool will_on_ground() override { return bClampToGround; };

            // Расположить самолет на данной стоянке.
            void place_on_ground( const startup_location_t & ramp );
            void place_on_ground(
                const position_t & position, const rotation_t & rotation, bool clamp_to_ground = true
            );

            // Переместить самолет на определенное количество
            // метров согласно его курсу (heading)
            void move( float meters ) override;

            // Смещение в метрах относительно координат стоянки.
            float shift_from_ramp();

            /**
             * @short Подготовить маршрут к взлету.
             * От аэропорта был получен путь руления к ВПП и сама рабочая ВПП.
             * Определяем действия (ребра графа) и складываем эти точки в план полета.
             */

            void prepare_for_take_off( const deque<waypoint_t> & taxi_way );
            
            /**
             * @param cruise_altitude высота в футах. Она не сильно необходима в подготовке полетного
             * плана, но просто чтобы не забывать, что ее нужно устанавливать на полетный план.
             */
            void prepare_flight_plan( deque<waypoint_t> & fp, const float & cruise_altitude );
            
            void choose_next_action();
            
            void test__place_on_hp();
            void test__place_on_rwy_end();
            void test__fly();
            
            /**
             * @short Перекрытая функция "касания земли" с учетом высоты самолета.
             */
            void hit_to_ground( position_t & position ) override;


        protected:

            void _on_ground_correction();
            
            /**
             * @short Текущее действие было завершено.
             */
            void _action_finished( void * action ) override;
            
        private:

            actuator_motion_t __actuators[ XPMP2::V_COUNT ];
            
            // Граф состояний самолета.
            AircraftStateGraph * __graph;
            
            bool __taxing_prepared;

            void __acf_parameters_correction();
            
            void __update_actuators( float elapsedSinceLastCall ); // NOLINT(bugprone-reserved-identifier)
            
            void __start_fp0_action();                 
            
            
            
            
    };  // class BimboAircraft

}; // namespace xenon
