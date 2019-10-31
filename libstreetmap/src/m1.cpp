#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "streetDB.h"
#include"global.h"
#include <algorithm>
#include <iostream>
#include<sstream>
using namespace std;
streetDB* globalBase;

//load_map(map_name) read a map name and try to search for the path;
//If found then load the map file and return true to the upper function;
//If not found then return false to the upper function and do nothing;

bool load_map(std::string map_name) {
    string temp_name=map_name;
    auto iter=temp_name.find("streets.bin");
    temp_name.erase(iter);
    string OSM_path=temp_name+"osm.bin";
    bool success = loadStreetsDatabaseBIN(map_name)&&loadOSMDatabaseBIN(OSM_path);
    if (success) {
        globalBase = new streetDB();
        return true;
    }
    return false;
}

//close_map() clean-up all map related data structures saved in the globalBase class;

void close_map() {
    closeStreetDatabase();
    closeOSMDatabase();
    delete globalBase;
}

//find_street_ids_from_name(street_name) returns street id(s) for the given street name;
//If no street with this name exists, returns a 0-length vector;

std::vector<unsigned> find_street_ids_from_name(std::string street_name) {
    if (street_name == "") {
        vector<unsigned> empty_vector = {};
        return empty_vector;
    } else {
        return globalBase->find_street_ids_from_name_in_DB(street_name);
    }
}

//find_intersection_street_segments(intersection_id) returns the street segment id for the given intersection id;

std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id) {
    return globalBase->find_intersection_street_segments_in_DB(intersection_id);
}

//find_intersection_street_names(intersection_id) returns the street names at the given intersection 
//including duplicate street names in returned vector;

std::vector<std::string> find_intersection_street_names(unsigned intersection_id) {
    return globalBase->find_intersection_street_names_in_DB(intersection_id);
}

//are_directly_connected(inter1, inter2) Returns true if you can get from intersection1 to intersection2 using a single street segment
//corner case: an intersection is considered to be connected to itself

bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2) {
    if (intersection_id1 == intersection_id2) {
        return true;
    }
    return globalBase->are_directly_connected_in_DB(intersection_id1, intersection_id2);
}

//Returns all intersections reachable by traveling down one street segment 
//from given intersection (hint: you can't travel the wrong way on a 1-way street)
//the returned vector should NOT contain duplicate intersections

std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id) {
    return globalBase->find_adjacent_intersections_in_DB(intersection_id);
}

//Returns all street segments for the given street JW

std::vector<unsigned> find_street_street_segments(unsigned street_id) {
    return globalBase->find_street_street_segments_in_DB(street_id);

}

//Returns all intersections along the a given street

std::vector<unsigned> find_all_street_intersections(unsigned street_id) {
    return globalBase->find_all_street_intersections_in_DB(street_id);

}

//Return all intersection ids for two intersecting streets
//This function will typically return one intersection id.
//However street names are not guarenteed to be unique, so more than 1 intersection id may exist

std::vector<unsigned> find_intersection_ids_from_street_names(std::string street_name1, std::string street_name2) {
    return globalBase->find_intersection_ids_from_street_names_in_DB(street_name1, street_name2);
}

//Retruns the distance between two coordinates in meters

double find_distance_between_two_points(LatLon point1, LatLon point2) {
    return globalBase->find_distance_between_two_points(point1, point2);
}

//Returns the length of the given street segment in meters

double find_street_segment_length(unsigned street_segment_id) {

    return globalBase->find_street_segment_length_in_DB(street_segment_id);
}

//Returns the length of the specified street in meters

double find_street_length(unsigned street_id) {

    return globalBase->find_street_length_in_DB(street_id);
}

//Returns the travel time to drive a street segment in seconds (time = distance/speed_limit)

double find_street_segment_travel_time(unsigned street_segment_id) {

    return globalBase->find_street_segment_travel_time_in_DB(street_segment_id);
}

//Returns the nearest point of interest to the given position

unsigned find_closest_point_of_interest(LatLon my_position) {

    return globalBase->find_closest_point_of_interest_in_DB(my_position);
}

//Returns the the nearest intersection to the given position

unsigned find_closest_intersection(LatLon my_position) {

    return globalBase->find_closest_intersection_in_DB(my_position);
}
