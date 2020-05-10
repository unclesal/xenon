// *********************************************************************************************************************
// *                                             Команда установки чего-нибудь.                                        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 21 sep 2019 at 14:48 *
// *********************************************************************************************************************
#pragma once

#include "jsonable.h"
using namespace std;

#ifdef EXTERNAL_SETTER
#include <QDebug>
#endif

namespace xenon {

    class CommandSet : public JSONAble {

        public:

            enum SET_COMMANDS_T {
                SET_UNKNOWN = -1,
                SET_AUTOPILOT_MODE,                 // Установить режим работы автопилота. ivalue - новый режим работы
                SET_AUTOPILOT_HEADING,              // Установить задатчик курса. fvalue = курс.
                SET_AUTOPILOT_ALTITUDE,             // Установить задатчик высоты. fvalue = задаваемая высота.
                SET_AUTOPILOT_AIR_SPEED,            // Установить задатчик воздушной скорости автопилота. fvalue = задаваемая скорость
                SET_AUTOPILOT_VERTICAL_VELOCITY,    // Установить задатчик вертикальной скорости автопилота. fvalue = задаваемая скорость.
                SET_AUTOPILOT_STATE_FLAGS,          // Установить флаги автопилота. ivalue = флаг, который надо установить.
                SET_AUTOTHROTTLE,                   // Установка режима работы привода дросселей двигателей
                SET_HSI_SELECTOR                    // Установка селектора gps / nav1 / nav2 (это в пользовательском самолете)
            };

            static string COMMAND_NAME;
            static string SECTION_AUTOPILOT;
            static string SECTION_USER_AIRCRAFT;

            CommandSet( SET_COMMANDS_T set_command, int ivalue, float fvalue );
            virtual ~CommandSet() override = default;

            virtual void to_json(JSON & json) override;
            virtual void from_json( JSON & json ) override;

            string section() { return _section; }
            SET_COMMANDS_T set_command() { return _set_command; }
            int i_value() { return _i_value; }
            float f_value() { return _f_value; }
            // bool b_value() { return _b_value; }

        protected:

            string _section;
            SET_COMMANDS_T _set_command;
            int _i_value;
            float _f_value;
            // bool _b_value;

        private:

            void __set_section();

    };

};
