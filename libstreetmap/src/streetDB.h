/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   streetDB.h
 * Author: hufeiran
 *
 * Created on January 26, 2017, 5:10 PM
 */

#ifndef STREETDB_H
#define STREETDB_H


#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "LatLon.h"
#include "graphics.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/foreach.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

using namespace std;

class streetDB {
private:
    
 
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    //This section contains all useful index of the map
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    
    //total name number
    unsigned nameNumbers;
    //total street number
    unsigned streetNumbers;

    //total segment number
    unsigned segmentNumbers;

    //total intersection number
    unsigned intersectionNumbers;

    //total POI number
    unsigned pointOfInterestNumbers;

    //total Node number
    unsigned NodeNumbers;

    //total Way number
    unsigned WayNumbers;

    //total feature number
    unsigned featureNumbers;

    //crutial to calculate x and y 
    float average_lat; 
    
    //boundary xy_coordinate of the map
    double max_x, max_y, min_x, min_y;  

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    //This section contains all vectors used to contain information about POI
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    vector<string> POI_id_to_type;
    
    //vector storing POI_name with street_id as index
    vector<string> POI_id_to_name;
    
    //rtree implementation
    //define point(lat,lon)
    typedef bg::model::point<double, 2, bg::cs::cartesian> point;
    
    //values of pair for POI
    typedef std::pair<point, unsigned> POI_value;

    //create rtree for POI
    bgi::rtree< POI_value, bgi::quadratic < 16 >> POI_rtree;

    //vector storing t_point of POI according to their id
    vector<t_point> POI_id_to_t_point;
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    //This section contains all vectors used to contain information about feature
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    
    vector<string> feature_id_to_feature_name;

    vector<TypedOSMID> feature_id_to_feature_osm;

    vector<FeatureType> feature_id_to_feature_type; 
    
    vector<unsigned> feature_id_to_feature_point_count;

    vector<vector<t_point>> feature_id_to_bound_points;

    //vectors containing feature id of different types
    vector<unsigned> Unknown_type_ids;
    vector<unsigned> Park_type_ids;
    vector<unsigned> Beach_type_ids;
    vector<unsigned> Lake_type_ids;
    vector<unsigned> River_type_ids;
    vector<unsigned> Island_type_ids;
    vector<unsigned> Shoreline_type_ids;
    vector<unsigned> Building_type_ids;
    vector<unsigned> Greenspace_type_ids;
    vector<unsigned> Golfcourse_type_ids;
    vector<unsigned> Stream_type_ids;

    //values of pair for buildings
    typedef std::pair<point, unsigned> Buildings_value;
    
    //create box for buildings
    typedef bg::model::box<point> box;
    
    //create rtree for buildings
    bgi::rtree< POI_value, bgi::quadratic < 16 >> Buildings_rtree;
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    //This section contains all vectors used to contain information about streets, segments, and intersections
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    //store each single word of a street name as a 2d-vector (won't be applied right now)
  //vector<vector<string>> streetNameBase;
    
  /*nameBase to store all names within the database: 0-streetNumbers-1: street names
   * streetNumbers-streetNumbers+POINumbers-1: POINames
   *streetNumbers+POINumbers-streetNumbers+POINumbers+FeatureNumbers: FeatureNames
   */
  vector<vector<string>> nameBase;
    //map storing street_name and street_id
    unordered_map<string, vector<unsigned>> street_name_to_street_id;

    //vector storing street_name with street_id as index
    vector<string> street_id_to_name;

    //2d vector with intersecion_id as x and street_segment_id as y
    vector<vector<unsigned> > intersection_id_to_street_segment_id;

    //vector storing segment_info with segment_id as index
    vector<StreetSegmentInfo> segment_id_to_segment_info;

    //2d vector with intersection_id as x and adjacent_intersection_id as y
    vector<vector<unsigned> > intersection_id_to_adjacent_intersection_id;

    //2d vector with street_id as x and segment_id as y
    vector<vector<unsigned> > street_id_to_segment_id;

    //2d vector with street_id as x and intersection_id as y
    vector<vector<unsigned> > street_id_to_intersection_id;

    //vector storing segment_length with segment_id as index
    vector<double> segment_id_to_segment_length;

    //2d vector with segment_id as x and curve_point_pos as y
    vector<vector <LatLon> > segment_id_to_curve_point_pos;

    //2d vector with segment_id as x and curve_point pos(t_point) as y
    vector<vector <t_point> > segment_id_to_curve_point_t_point_pos;

    //vector storing travel_time with street_segment as index
    vector<double> street_segment_id_to_travel_time;

        
    //values of pair for intersection
    typedef std::pair<point, unsigned> intersection_value;
    
    //create rtree for intersection 
    bgi::rtree< intersection_value, bgi::quadratic<16> > intersection_rtree;

    //create vector intersection-id to its latlon
    vector<LatLon> intersection_id_to_latlon;

    //create vector to store intersection's t_points
    vector<t_point> intersection_id_to_t_point;

    //three types of roads
    //store highway street segment index
    vector<unsigned> highway;

    //store avenue street segment index
    vector<unsigned> avenue;

    //store link lines index
    vector<unsigned> link;

    //store small roads (alleys) street segment index
    vector<unsigned> alleys;

    //store cycleways street segment index
    vector<unsigned> cycleways;


    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    //This section contains all vectors used to contain information about OSM id
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    //vector to store OSMNode*
    vector <const OSMNode*> osm_index_to_node;

    //vector to store OSMWay*
    vector<const OSMWay*> osm_index_to_way;

    //vector to store OSMEntity*
    vector <const OSMEntity*> osm_index_to_OSMEntity;

    //unordered map storing OSMNode and vector storing index to its tag
    unordered_map <const OSMEntity*, unordered_map<string, string>> entity_to_tag;

    //unordered_map <OSMID, OSMNode*> osm_id_to_node
    unordered_map <OSMID, const OSMNode*> osm_id_to_node;


    //unordered_map <OSMID, OSMEntity*> osm_id_to_entity
    unordered_map <OSMID, const OSMEntity*> osm_id_to_entity;

    
    
public:
    //constructor
    streetDB();

    //destructor
    ~streetDB();

    //Returns street id(s) for the given street name
    //If no street with this name exists, returns a 0-length vector.
    std::vector<unsigned> find_street_ids_from_name_in_DB(std::string street_name); //tonyfigo

    //find street name from its id
    string find_street_name_from_ids_in_DB(unsigned street_id);

    //Returns the street segments for the given intersection 
    std::vector<unsigned> find_intersection_street_segments_in_DB(unsigned intersection_id); //tonyfigo

    //Returns the street names at the given intersection (includes duplicate street names in returned vector)
    std::vector<std::string> find_intersection_street_names_in_DB(unsigned intersection_id); //tonyfigo

    //Returns true if you can get from intersection1 to intersection2 using a single street segment (hint: check for 1-way streets too)
    //corner case: an intersection is considered to be connected to itself
    bool are_directly_connected_in_DB(unsigned intersection_id1, unsigned intersection_id2); //brianlikestofixbugs

    //Returns all intersections reachable by traveling down one street segment 
    //from given intersection (hint: you can't travel the wrong way on a 1-way street)
    //the returned vector should NOT contain duplicate intersections
    std::vector<unsigned> find_adjacent_intersections_in_DB(unsigned intersection_id); //tonyfigo

    //Returns all street segments for the given street
    std::vector<unsigned> find_street_street_segments_in_DB(unsigned street_id); //tonyfigo

    //Returns all intersections along the a given street
    std::vector<unsigned> find_all_street_intersections_in_DB(unsigned street_id); //tonyfigo

    //Return all intersection ids for two intersecting streets
    //This function will typically return one intersection id.
    //However street names are not guarenteed to be unique, so more than 1 intersection id may exist
    std::vector<unsigned> find_intersection_ids_from_street_names_in_DB(std::string street_name1, std::string street_name2); //brianlikestofixbugs


    //Returns the length of the given street segment in meters
    double find_street_segment_length_in_DB(unsigned street_segment_id); //LY

    //Returns the length of the specified street in meters
    double find_street_length_in_DB(unsigned street_id); //tonyfigo

    //Returns the travel time to drive a street segment in seconds (time = distance/speed_limit)
    double find_street_segment_travel_time_in_DB(unsigned street_segment_id); //tonyfigo

    //Returns the nearest point of interest to the given position
    unsigned find_closest_point_of_interest_in_DB(LatLon my_position); //tonyfigo

    //Returns the the nearest intersection to the given position
    unsigned find_closest_intersection_in_DB(LatLon my_position); //tonyfigo

    //Returns distance between two points
    double find_distance_between_two_points(LatLon point1, LatLon point2); //LY

    unsigned get_segmentNumbers();

    //return streetsegmentinfo
    StreetSegmentInfo get_segment_info(unsigned index);

    //return t_point of the intersection
    t_point get_t_point_intersection_pos(unsigned inter_index);

    vector<t_point> get_curve_points_in_t_point(unsigned index);

    //0---min x; 1---min y; 2----max x; 3-----max y
    vector<double> get_map_limits();

    
    //get highway 
    vector<unsigned> get_highway();

    //get avenue
    vector<unsigned> get_avenue();

    //get cycleways
    vector<unsigned> get_cycleways();

    //get links
    vector<unsigned> get_link();

    //get allies
    vector<unsigned> get_alleys();

    //change a given latlon of a point to the t_point of the point
    t_point latlon_to_t_point(LatLon temp);

    //change a given t_point of a point to the latlon of the point
    LatLon t_point_to_latlon(t_point temp);

    //check whether the given feature is a closed shape
    bool check_closed(vector<t_point> points);

    //feature type vectors, varies by their type
    vector<unsigned> get_Unknown_type_ids();
    vector<unsigned> get_Park_type_ids();
    vector<unsigned> get_Beach_type_ids();
    vector<unsigned> get_Lake_type_ids();
    vector<unsigned> get_River_type_ids();
    vector<unsigned> get_Island_type_ids();
    vector<unsigned> get_Shoreline_type_ids();
    vector<unsigned> get_Building_type_ids();
    vector<unsigned> get_Greenspace_type_ids();
    vector<unsigned> get_Golfcourse_type_ids();
    vector<unsigned> get_Stream_type_ids();

    //get bound points of a feature according to their index
    vector<t_point> get_feature_id_to_bound_points(unsigned index);
    
    //get number of bound points of a feature according to their index
    unsigned get_feature_id_to_feature_point_count(unsigned index);
    
    //get name of a feature according to their index
    string get_feature_id_to_feature_name(unsigned id);
    
    //get total feature number in the map
    unsigned get_feature_number();
    
    //get name of an intersection according to their index
    string get_intersection_name(unsigned id);
    
    //check a street name exist or not
    bool check_street_existing(std::string streetName); 
    
    //get name of a POI according to their index
    string get_POI_name_from_id(unsigned id);
    
    //get id of a POI according to their name
    unsigned get_POI_id_from_name(string name);
    
    //get the xy_coordinate of a POI according to its id
    t_point get_POI_id_to_t_point(unsigned id);
    
    //get total POI number in the map
    unsigned POInumber();
    
    //get the straight distance between two xy_coordinates
    float find_distance_in_t_point(t_point temp1, t_point temp2);
    
    //check whether a POI exist in the map
    bool check_POI_existing(string POI_Name);
    
    //get the certain type of a POI according to its id
    string get_POI_type(unsigned id);
    
    //get the certain type of a feature according to its id
    FeatureType get_feature_id_to_feature_type(unsigned id);
    
    //get id of a feature according to their name
    unsigned get_feature_id_from_name(string name);
    
    //check whether a feature name existing in the map
    bool check_feature_existing(std::string Feature_Name);
    
    //get an average xy_coordinate of a feature according to its id(the pin position on the map)
    t_point get_feature_average_pos(unsigned id);
//    
    //get street_name_list (right now won't be applied)
    vector<vector<string>> get_name_base();
    
    //get total street number 
    unsigned get_streetNumbers();
    //get total name number
    unsigned get_nameNumbers();
    
    unsigned get_segment_between_inters(unsigned inters_id1, unsigned inters_id2);
    
    t_point find_midpoint_of_two_inters(unsigned inters_id1, unsigned inters_id2);
    
    bool check_segment_on_same_street(unsigned seg_id1, unsigned seg_id2);
    
    unsigned get_intersection_number();
    
    string find_name_from_ids_in_nameBase(unsigned id);
    
    std::vector<unsigned> get_all_POI_ids_according_to_name(std::string name);
    
    vector <t_point>  get_segment_id_to_curve_point_t_point_pos(unsigned segment_id);
    
    t_point find_closest_curve_point_to_an_intersection_in_t_point(unsigned inter_id,unsigned segment_id);
    
    LatLon get_intersection_pos_in_latlon(unsigned id);
};

#endif /* STREETDB_H */

