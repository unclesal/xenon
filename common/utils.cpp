// *********************************************************************************************************************
// *                                               Общие мелкие утилитки.                                              *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 19:36 *
// *********************************************************************************************************************
#include <sstream>

#include "utils.hpp"

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Разбиение строки на подстроки.                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

vector<std::string> xenon::split (const string & s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}
