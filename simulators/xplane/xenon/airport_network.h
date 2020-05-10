// *********************************************************************************************************************
// *                                             Граф путей аэропорта.                                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 10 may 2020 at 15:35 *
// *********************************************************************************************************************
#pragma once
#include <string>
#include <lemon/list_graph.h>
#include <lemon/maps.h>

using namespace lemon;
using namespace std;
namespace xenon {

    class AirportNetwork: public ListDigraph {

        public:

            // Типы транспорта, для которых предназначена данная рулежка (ребро графа).
            enum vehicles_t {
                VT_UNKNOWN,
                VT_AIRCRAFTS,
                VT_GROUND_VEHICLES
            };

            enum way_type_t {
                // Node is on a regular taxiway. If on “runway” a clearance is needed from ATC
                // “taxiway” or “runway”
                WAY_NONE = 0,
                WAY_RUNWAY,
                WAY_TAXIWAY
            };

            struct active_zone_t {
                string classification;
                string runways;
            };

            AirportNetwork();
            AirportNetwork & operator = ( const AirportNetwork & anet );
            ~AirportNetwork() = default;

            Digraph::NodeMap<double>                    node_latitude;
            Digraph::NodeMap<double>                    node_longitude;
            // Оригинальный идентификатор узла в X-Plane.
            Digraph::NodeMap<int>                       node_xp_id;
            // Usage of node in network (begin or end a taxi path, or both)
            // “dest”, “init”, “both” or “junc”
            Digraph::NodeMap<string>                    node_usage;
            Digraph::NodeMap<string>                    node_name;

            ListDigraph::NodeIt get_node_by_xp_id( const int & xp_id );

            Digraph::ArcMap<vehicles_t>                 arc_vehicle_type;
            Digraph::ArcMap<way_type_t>                 arc_way_type;
            Digraph::ArcMap< string >                   arc_name;
            Digraph::ArcMap< vector<active_zone_t> >    arc_active_zones;

        private:


    };

};
