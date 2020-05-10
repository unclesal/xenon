#include <iostream>
#include <string>
#include "airport.h"

void parsed(string & icao_code ) {
    cout << "Parsed: " << icao_code << endl;
};

using namespace xenon;

int main() {
    Airport::set_has_been_parsed( parsed );
    Airport::read_all();
    Airport usss = Airport::get_by_icao("USSS");
    location_t location;
    location.latitude=56.749406;
    location.longitude=60.797907;
    auto endpoint = usss.get_start_location_for_departure_taxing(location);
    auto way = usss.get_path_for_departure_taxing(endpoint.location);

//    position_t pos_from {
//        .x = -12335.500977,
//        .y = 0.0,
//        .z = -27781.599609
//    };
//    line_descriptor_t line_1 = XPlaneUtilities::line(pos_from, 359.459991);
//
//    // rotation_t rot_from; rot_from.heading = 359.459991;
//
//    position_t pos_to {
//        .x = -12347.484432,
//        .y = 0.0,
//        .z = -27687.530100
//    };
//    // rotation_t rot_to; rot_to.heading = 90.270424;
//    line_descriptor_t line_2 = XPlaneUtilities::line(pos_to, 90.270424);
//    double distance = XPlaneUtilities::distance_2d(pos_from, line_2 );
//    double dis1 = XPlaneUtilities::distance_2d( pos_from, pos_to );

    return 0;
}
