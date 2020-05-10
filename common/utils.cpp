// *********************************************************************************************************************
// *                                               Общие мелкие утилитки.                                              *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 19:36 *
// *********************************************************************************************************************
#include <sstream>

#include "constants.h"
#include "utils.h"

using namespace std;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Разбиение строки на подстроки.                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

vector<string> split (const string & s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}
