// *********************************************************************************************************************
// *                                           Окружающая действительность.                                            *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 14 sep 2019 at 18:55 *
// *********************************************************************************************************************

#pragma once

// my own include files.
#include "jsonable.h"

namespace xenon {

    class Ambient : public JSONAble {

        public:

            static string COMMAND_NAME;

            Ambient();
            virtual ~Ambient();

            virtual void to_json( JSON & json ) override;
            virtual void from_json( JSON & json ) override;

        protected:

            // The ambient temperature, taking into account the altitude of the aircraft position.
            float _ambient_temperature;

            // Высота над уровнем подстилающей поверхности, метров.
            float _agl;


        private:

    };

};

