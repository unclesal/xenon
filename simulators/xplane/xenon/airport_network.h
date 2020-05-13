// *********************************************************************************************************************
// *                                             Граф путей аэропорта.                                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 10 may 2020 at 15:35 *
// *********************************************************************************************************************
#pragma once
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>

#include "structures.h"

using namespace std;

namespace xenon {

//    point1.set<0>(1.0); 2
//    point1.set<1>(2.0);
//
//    double x = point1.get<0>(); 3
//    double y = point1.get<1>();

    class AirportNetwork {

        public:

            struct node_t {
                // Latitude of node in decimal degrees Eight decimal places supported
                // double latitude = 0.0;
                // Longitude of node in decimal degrees Eight decimal places supported
                // double longitude = 0.0;
                location_t location;

                // Usage of node in network (begin or end a taxi path, or both)
                // “dest”, “init”, “both” or “junc”
                string usage;
                // Node identifier (defined in 0 based sequence, ascending)
                // Integer. Must be unique within scope of an airport.
                // Минус 1 означает, что реально узла - нет.
                int xp_id = -1;
                // Node name. Not currently used. String (max 16 characters)
                string name = "";
            };

            struct active_zone_t {
                string classification;
                string runways;
            };

            // typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_descriptor_t;

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
                WAY_TAXIWAY,
                WAY_ANY
            };

            struct edge_t {
                vehicles_t vehicles = VT_UNKNOWN;
                // Node identifier for start of edge Integer. Must refer to valid node (row code ‘1201’)
                int xp_start_id = -1;
                // Node identifier for end of edge Integer. Must refer to valid node (row code ‘1201’)
                int xp_end_id = -1;
                // Edge can be used in both directions “twoway” or “oneway”
                // string directions = "";
                // Node is on a regular taxiway. If on “runway” a clearance is needed from ATC
                // “taxiway” or “runway”
                // string type = "";
                way_type_t way_type = WAY_NONE;
                // Taxiway identifier. Used to build ATC taxi clearances (eg. “.. .taxi via A, T, Q”)
                // String. Taxiway or runway identifier (eg. “A” or “16L/34R”)
                string name = "";
                double distance = 1.0;
                double weight = 1.0;
                vector< active_zone_t > active_zones;
            };

            // Описание собственно графа.
            typedef boost::adjacency_list<
                // selects the STL list container to store the OutEdge list
                boost::listS,
                // selects the STL vector container to store the vertices
                boost::vecS,
                boost::directedS,
                node_t, edge_t> graph_t;

            // С целью писать поменьше.
            using node_descriptor_t = graph_t::vertex_descriptor;
            using edge_descriptor_t = graph_t::edge_descriptor;

            // Перекрытый класс Дейкстра-обхода графа (dijkstra visitor)
            
            struct dij_visitor_t : boost::default_dijkstra_visitor {
                    using base = boost::default_dijkstra_visitor;
                    struct done{};

                    dij_visitor_t(node_descriptor_t vd, size_t & visited)
                        : destination(vd), visited(visited) {}

                    void finish_vertex(node_descriptor_t v, graph_t const & g) {
                        ++visited;

                        if (v == destination)
                            throw done{};

                        base::finish_vertex(v, g);
                    }

                private:
                    graph_t::vertex_descriptor destination;
                    size_t & visited;
            };

            AirportNetwork();
            AirportNetwork & operator = ( const AirportNetwork & anet ) = default;
            ~AirportNetwork() = default;

            const xenon::AirportNetwork::graph_t & graph() {
                return __graph;
            }

            void add_apt_dat_node( const string & line, const vector<string> & contents );
            void add_apt_dat_edge( int i_type, const string & line, const vector<string> & contents );
            void add_apt_dat_active_zone( const string & line, const vector<string> & contents );

            graph_t::vertex_descriptor get_node_by_xp_id( const int & xp_id );

            graph_t::vertex_descriptor get_nearest_node( const location_t & from, const way_type_t way_type );
            // edge_t get_nearest_node( const location_t & from, const vector<graph_t::vertex_descriptor> & input_set );

            // @todo наверняка здесь может быть правильнее.
            vector<edge_t> get_edges_for( const graph_t::vertex_descriptor & node_descriptor );

            void dijkstra_shortest_paths(
                const graph_t::vertex_descriptor & start_node_descriptor, const location_t & to_location
            );

        private:

            graph_t __graph;
            graph_t::edge_descriptor __last_added_direct_edge;
            graph_t::edge_descriptor __last_added_reverse_edge;
    };

};
