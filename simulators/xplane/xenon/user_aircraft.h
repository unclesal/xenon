// *********************************************************************************************************************
// *       User aircraft. The one on which the user itself (the human, not automate) is flying in the simulator.       *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 mar 2019 at 17:07 *
// *********************************************************************************************************************

#pragma once

// System include files.
#include <string>

#include "XPLMDataAccess.h"

// My own includes.
#include "abstract_aircraft.h"

using namespace std;

namespace xenon {
    
    class UserAircraft : public AbstractAircraft {
        
        public:
                        
            UserAircraft();
            virtual ~UserAircraft() override = default;
                        
            void observe() override;

            position_t get_position() override;
            void set_position( const position_t & position ) override;
            rotation_t get_rotation() override;

            // Перекрытые функции вызова преобразований в json и обратно.
            // virtual void to_json( JSON & json ) override;
            // virtual void from_json( JSON & json ) override;
            
            // Установка чего-нибудь в пользовательском самолете.
            // virtual void set( CommandSet & cmd ) override;                        
            
            /**
             * @short Обновить состояние структур, отображающих пользовательский самолет.
             * Сами структуры-то оно конечно есть как наследники, и они даже публичные.
             * Только вот в пользовательском самолете в них ровным счетом ничего нет,
             * потому что здесь с точностью до наоборот: это не структуры определяют
             * положение самолета, а сам самолет, управляемый живым человеком, должен
             * заполнять эти структуры.
             */
            void update_conditions();
            
        protected:
            
            void _action_finished( void * action ) override {
            };                        
            
        private:

            /**
             * @short Pointer to local OGL X coordinate of aircraft position.
             */

            XPLMDataRef __dr_x;

            /**
             * @short Pointer to local OGL Y-coordinate of aircraft position.
             */

            XPLMDataRef __dr_y;

            /**
             * @short Pointer to local OGL Z-coordinate of aircraft position.
             */

            XPLMDataRef __dr_z;


            /**
             * @short A pointer to the local OGL value of the angle of rotation of the aircraft in the vertical axis.
             *
             * This is not the same thing as the "aircraft course", it is the angle in the local QGL-coordinates
             * of the simulator  between the axis of the aircraft and the axis Z.
             */

            XPLMDataRef __dr_psi;

            /**
             * @short A pointer to the local OGL value of the angle of rotation of the aircraft in the transverse axis.
             *
             * This is not the same as the pitch of the aircraft, this is the angle in the local OGL-coordinates of
             * the simulator. This is the angle between the aircraft axis and the Y axis in the local OpenGL
             * coordinate system.
             */

            XPLMDataRef __dr_theta;

            /**
             * @short A pointer to the local OGL value of the angle of rotation of the aircraft in the longitudinal axis.
             *
             * This is not the same as the roll of the aircraft, this is the angle in the local OGL-coordinates of the simulator.
             */

            XPLMDataRef __dr_phi;

            /**
             * @short A pointer to the course variable of the aircraft.
             *
             * It is calculated differently and comes from different variables, but the concept
             * of "course" is applicable for any aircraft, both the main (user) and auxiliary
             * (fake, bimbo, soother). Accordingly, the "course" is the variable of an abstract airplane.
             */

            XPLMDataRef __dr_heading;

            // VVI, vertical velocity, feet per second, float.
            XPLMDataRef __dr_vvi;

            // Instrumental speed (relative to air)
            XPLMDataRef __dr_ias;

            // Magnetic variation
            XPLMDataRef __dr_magnetic_variation;

            // GPS / NAV1 / NAV2
            XPLMDataRef __dr_hsi_selector;




    }; // class UserAircraft
    
}; // namespace xenon

