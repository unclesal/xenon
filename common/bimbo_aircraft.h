// *********************************************************************************************************************
// *                                        "Ботовый" (multiplayer) самолет.                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 01 may 2020 at 12:06 *
// *********************************************************************************************************************
#pragma once

#include <string>
#include <deque>

#ifdef INSIDE_XPLANE

// X-Plane includes
#include "XPLMScenery.h"
#include "XPLMInstance.h"

// XPMP2 includes
#include "XPMPAircraft.h"

#include "xplane.hpp"

using namespace XPMP2;

#endif // INSIDE_XPLANE

// My own includes
#include "structures.h"
#include "constants.h"
#include "aircraft_state_graph.h"
#include "abstract_aircraft.h"

namespace xenon {

    class BimboAircraft : public AbstractAircraft
#ifdef INSIDE_XPLANE
        , public XPMP2::Aircraft
#endif
    {
        
#ifdef INSIDE_AGENT
        friend class AgentAircraft;
#endif        

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

#ifdef INSIDE_XPLANE
            // Мы находимся внутри плагина X-Plane.
            position_t get_position() override;
            position_with_angles_t get_position_with_angles();
            void set_position( const position_t & position ) override;
            void set_location( const location_t & location ) override;
            void set_vcl_coordinates();
            void store_vcl_coordinates();
#else
            // Мы находимся - не внутри X-Plane. Перекрытие имен 
            // и функций таким образом, чтобы оно тоже работало.
            std::string acIcaoType;
            std::string acIcaoAirline;
            std::string acLivery;
            
            // Имена сохранены таким образом, чтобы их можно было использовать везде.
            float GetPitch () const         { return vcl_condition.rotation.pitch;   };
            void  SetPitch (float _deg)     { vcl_condition.rotation.pitch = _deg;   };
            float GetHeading () const       { return vcl_condition.rotation.heading; };
            void  SetHeading (float _deg)   { vcl_condition.rotation.heading = _deg; };
            float GetRoll () const          { return vcl_condition.rotation.roll;    };
            void  SetRoll (float _deg)      { vcl_condition.rotation.roll = _deg;    };
            
#endif // INSIDE_XPLANE

            rotation_t get_rotation() override;
            void set_rotation( const rotation_t & rotation ) override;

            /**
             * @short Перекрытая функция XPMP2::Aircraft / ExternalAircraft
             */
            void UpdatePosition(float elapsed_since_last_call, int fl_counter) 
#ifdef INSIDE_XPLANE
            // Если внутри X-Plane, то она перекрывает XPMP2::Aircraft. Если 
            // снаружи - то ничего не перекрывает. Но должна быть все равно.
            override 
#endif
            ;

            void update_from( vehicle_condition_t & vc, aircraft_condition_t & ac ) override;

            // Освещение вкл-выкл
            void set_taxi_lites(bool on) override {
#ifdef INSIDE_XPLANE
                on ? v[ V_CONTROLS_TAXI_LITES_ON ] = 1.0 : v[ V_CONTROLS_TAXI_LITES_ON ] = 0.0;
#endif
                acf_condition.is_taxi_lites_on = on;

            };
            
            void set_landing_lites(bool on) override {
#ifdef INSIDE_XPLANE                
                on ? v[ V_CONTROLS_LANDING_LITES_ON ] = 1.0 : v[ V_CONTROLS_LANDING_LITES_ON ] = 0.0;
#endif
                acf_condition.is_landing_lites_on = on;
            };
            
            void set_beacon_lites(bool on) override {
#ifdef INSIDE_XPLANE
                on ? v[ V_CONTROLS_BEACON_LITES_ON ] = 1.0 : v[ V_CONTROLS_BEACON_LITES_ON ] = 0.0;    
#endif
                acf_condition.is_beacon_lites_on = on;
            };
            
            void set_strobe_lites(bool on) override {
#ifdef INSIDE_XPLANE
                on ? v[ V_CONTROLS_STROBE_LITES_ON ] = 1.0 : v[ V_CONTROLS_STROBE_LITES_ON ] = 0.0;
#endif
                acf_condition.is_strobe_lites_on = on;
            };

            void set_nav_lites(bool on) override {
#ifdef INSIDE_XPLANE
                on ? v[ V_CONTROLS_NAV_LITES_ON ] = 1.0 : v[ V_CONTROLS_NAV_LITES_ON ] = 0.0;
#endif
                acf_condition.is_nav_lites_on = on;
            };
            
            void set_gear_down( bool down ) override {
#ifdef INSIDE_XPLANE                
                down ? __actuators[ V_CONTROLS_GEAR_RATIO ].endpoint = 1.0 : __actuators[ V_CONTROLS_GEAR_RATIO ].endpoint = 0.0;
                __actuators[  V_CONTROLS_GEAR_RATIO ].requested = true;
#endif
                acf_condition.is_gear_down = down;
            };
            
            void set_reverse_on( bool on ) override { 
#ifdef INSIDE_XPLANE
                on ? __actuators[ V_CONTROLS_THRUST_REVERS ].endpoint = 1.0 : __actuators[ V_CONTROLS_THRUST_REVERS ].endpoint = 0.0;
                __actuators[ V_CONTROLS_THRUST_REVERS ].requested = true;
#endif
                acf_condition.is_reverse_on = on;
            };
            
            void set_flaps_position( const float & position ) override {
#ifdef INSIDE_XPLANE
                __actuators[ V_CONTROLS_FLAP_RATIO ].endpoint = position;
                __actuators[ V_CONTROLS_FLAP_RATIO ].requested = true;
#endif
                acf_condition.flaps_position = position;
            };
                        
            void set_speed_brake_position( const float & position ) override {
#ifdef INSIDE_XPLANE
                __actuators[ V_CONTROLS_SPEED_BRAKE_RATIO ].endpoint = position;
                __actuators[ V_CONTROLS_SPEED_BRAKE_RATIO ].requested = true;
#endif
                acf_condition.speed_brake_position = position;
            };
            
            void set_thrust_position( const float & position ) override {
#ifdef INSIDE_XPLANE
                __actuators[ V_CONTROLS_THRUST_RATIO ].endpoint = position;
                __actuators[ V_CONTROLS_THRUST_RATIO ].requested = true;
#endif
                acf_condition.thrust_position = position;
            };
                        
            // Расположить самолет на данной стоянке.
            void place_on_ground( const startup_location_t & ramp );

#ifdef INSIDE_XPLANE
            void place_on_ground(
                const position_t & position, rotation_t & rotation, bool clamp_to_ground = true
            );
#endif

            // Переместить самолет на определенное количество
            // метров согласно его курсу (heading)
            void move( float meters ) override;

            /**
             * @short Подготовить маршрут к взлету.
             * От аэропорта был получен путь руления к ВПП и сама рабочая ВПП.
             * Определяем действия (ребра графа) и складываем эти точки в план полета.
             */

            void prepare_for_take_off( const deque<waypoint_t> & taxi_way );
            
            void prepare_for_taxing( const deque< waypoint_t > & taxi_way ) override;
                        
            void test__fly();
            void test__fp_landing();
            // void test__taxing();

#ifdef INSIDE_XPLANE
            void test__place_on_hp();
            void test__place_on_rwy_end();            
#endif
            
#ifdef INSIDE_XPLANE
            /**
             * @short Перекрытая функция "касания земли" с учетом высоты самолета.
             */
            void hit_to_ground( position_t & position ) override;
#endif
            
            // Граф состояний самолета.
            AircraftStateGraph * graph;

            /**
             * @short Текущее действие было завершено.
             */
            void action_finished( void * action ) override;
            
#ifdef INSIDE_AGENT
            void set_agent(AgentInterface * a ) override {
                AbstractAircraft::set_agent( a );
                flight_plan.set_agent( a );
            };
#endif            
            
        protected:
            
        private:
                        
            
            bool __taxing_prepared;

            void __acf_parameters_correction();

#ifdef INSIDE_XPLANE            
            actuator_motion_t __actuators[ V_COUNT ];            
            void __update_actuators( float elapsedSinceLastCall ); // NOLINT(bugprone-reserved-identifier)
#endif                                                                        
            
    };  // class BimboAircraft

}; // namespace xenon
