/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m3.h
 * Author: weiliche
 *
 * Created on March 14, 2017, 2:45 PM
 */

#ifndef M3_H
#define M3_H
#pragma once
#include <string>
#include <vector>
#include "OSMDatabaseAPI.h"
#include "m1.h"
#include "m2.h"
#include "graphics.h"
typedef struct{
    vector<string> instruction;
    vector<float> speedlimit;
    vector<double> travel_time;
    vector<double> street_distance;
    vector<unsigned> intersection_id;
    vector<vector<unsigned>> street_segments_ids;//segment ids for each instruction on street
}seg_direction_handler;

//categorize different street segments into different street
typedef struct{
    unsigned streetID;
    unsigned beginInter;
    unsigned endInter;
    double distance_on_street;
    float speedlimit;
    unsigned starting_segment_id;
    unsigned ending_segment_id;
}path_street_catogariser;


typedef struct{
    float angle;
    unsigned streetid;
    unsigned segment_id;
}angle_and_street_id;

double compute_path_travel_time(const std::vector<unsigned>& path, const double turn_penalty);

std::vector<unsigned> find_path_between_intersections(const unsigned intersect_id_start, const unsigned intersect_id_end, const float turn_penalty);

vector<unsigned> find_path_to_point_of_interest (const unsigned intersect_id_start, const std::string point_of_interest_name, const double turn_penalty);

vector<unsigned> find_path_between_pois(const unsigned poi_start, const std::string poi_end_name, const double turn_penalty);

unsigned get_the_best_POI_id(const unsigned intersect_id_start, const std::string point_of_interest_name, const double turn_penalty);
//overload this function
unsigned get_the_best_POI_id_pois(const unsigned poi_id, const std::string point_of_interest_name, const double turn_penalty);

seg_direction_handler direction_process(vector<unsigned> path_segments);
string direction_process_helper(unsigned inter_id,unsigned from_segment_id,unsigned to_segment_id);

vector<path_street_catogariser> categorize_street_segments(vector<unsigned> segment_ids);

int distance_optimizer(double distance);

double distance_optimizer_kilometer(double distance);

string double_to_string(double const& value);

string int_to_string(int const& value) ;

#endif /* M3_H */

