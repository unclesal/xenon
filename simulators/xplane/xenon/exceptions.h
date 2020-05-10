// *********************************************************************************************************************
// *                                              Генерируемые исключения                                              *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 19:52 *
// *********************************************************************************************************************
#pragma once
#include <string>
#include <exception>

using namespace std;

namespace xenon {

    class base_exception : public exception {

        public:

            base_exception(string msg)
                    : exception() { _msg_ = msg; }
            virtual char const * what() { return _msg_.c_str(); };

        private:
            string _msg_;

    };

    class bad_format_exception: public base_exception {
        public:
            bad_format_exception(string msg) : base_exception(msg) {};
    };

}; // namespace xenon
