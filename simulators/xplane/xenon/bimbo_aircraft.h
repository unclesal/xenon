// *********************************************************************************************************************
// *                                        "Ботовый" (multiplayer) самолет.                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 01 may 2020 at 12:06 *
// *********************************************************************************************************************
#pragma once

#include <string>

// X-Plane includes
#include "XPLMScenery.h"
#include "XPLMInstance.h"

// XPMP2 includes
#include "XPMPAircraft.h"

// My own includes
#include "structures.h"
#include "constants.h"
#include "abstract_aircraft.h"
#include "xplane_utilities.h"

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

            // virtual void observe() override;
            // virtual void control( float elapsed_since_last_call ) override;

            position_t get_position() override;
            position_with_angles_t get_position_with_angles();
            void set_position( const position_t & position ) override;
            void set_rotation( const rotation_t & rotation ) override;

            // Перекрытая функция XPMP2::Aircraft
            void UpdatePosition(float elapsed_since_last_call, int fl_counter) override ;

            // Освещение вкл-выкл
            void set_taxi_lites(bool on);
            void set_landing_lites(bool on);
            void set_beacon_lites(bool on);
            void set_strobe_lites(bool on);
            void set_nav_lites(bool on);

            // Расположить самолет на данной стоянке.
            void place_on_ground( const startup_location_t & ramp );
            void place_on_ground(
                const position_t & position, const rotation_t & rotation, bool clamp_to_ground = true
            );

            // Переместить самолет на определенное количество
            // метров согласно его курсу (heading)
            void move( float meters );

            // Смещение в метрах относительно координат стоянки.
            float shift_from_ramp();

            // Перекрытая функция управления движением самолета.
            // void control( float elapse_since_last_call ) override;

            void add_condition( const aircraft_condition_t & condition ) {
                _conditions.push_back( condition );
            };

            void apply_next_condition();

            /**
             * @short Подготовить маршрут либо к выталкиванию, либо к рулежке.
             * Самолет стоит на стоянке и ему дают некую начальную точку рулежки к ВПП для вылета.
             * Два варианта. Либо нужно выталкивание, либо самолет сам сможет вырулить на эту точку.
             * Самолет решает это самостоятельно в зависимости от направления на эту самую конечную
             * точку. В результате он должен встать где-то близко к заданной точке с заданным
             * курсом, тоже "где-то близко".
             * @param target
             */
            void prepare_for_push_back_or_taxing( const location_with_angles_t & target );

            void prepare_for_taxing( const vector<location_with_angles_t> & taxi_way );

        protected:

            void _on_ground_correction();

        private:

            aircraft_condition_t _current_condition;
            vector<aircraft_condition_t> _conditions;

            actuator_motion_t __actuators[ XPMP2::V_COUNT ];

            static aircraft_condition_t _make_condition_full_taxing_stop(const float & from_speed);
            static aircraft_condition_t _make_condition_straight_push_back( const location_with_angles_t & target );
            static aircraft_condition_t _make_condition_rotated_push_back( const location_with_angles_t & target );

            void _prepare_for_taxing( const location_with_angles_t & target );
            void _prepare_for_push_back( const location_with_angles_t & target );
            // void _control_check_request_lites_finished( const unsigned short int & request );
            // void _control_request_lites( const unsigned int & request, const float & elapsed_since_last_call );
            // static void _control_one_light( float & value, const float & dv );

            void __update_actuators( float elapsedSinceLastCall ); // NOLINT(bugprone-reserved-identifier)

    };  // class BimboAircraft

}; // namespace xenon
