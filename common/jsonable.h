// *********************************************************************************************************************
// *                                            Class which can be present in JSON form.                               *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 mar 2019 at 19:40 *
// *********************************************************************************************************************

#pragma once

#include <string>
#include <map>
#include <stdexcept>

#include <stdio.h>

#include "nlohmann/json.hpp"

#define UNKNOWN_COMMAND_NAME "UNKNOWN"

using namespace std;
using JSON=nlohmann::json;

namespace xenon {

    class JSONAble {

        public:

            JSONAble();
            virtual ~JSONAble() = default;

            virtual void to_json(JSON & json);
            virtual void from_json( JSON & json );

            // JSON concat_with_jsonable( JSON & json );
            
            const string & command_name() {
                return _command_name;
            };

        protected:

            /**
             * @short Command name. Value used for recognizing incomming network commands.
             */

            string _command_name;

        private:

    };

};
