/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "StreetsDatabaseAPI.h"
#include "streetDB.h"
#include "global.h"
#include <algorithm>
#include <math.h>
#include <iostream>
#include <sstream>
#include<string>
#include <cassert>
#include <X11/keysym.h>
#include "m3.h"
#include "m2.h"
#include "m1.h"
#include "graphics.h"
using namespace std;
vector<unsigned> paths;
unsigned from_inter_id = 0;
unsigned best_poi_id = 0;

double compute_path_travel_time(const std::vector<unsigned>& path, const double turn_penalty) {
    unsigned penalty_count = 0;
    double total_time = 0;
    for (unsigned i = 0; i < path.size(); i++) {
        total_time += find_street_segment_travel_time(path[i]); //add travel time of each segments to total_time
        if (!globalBase -> check_segment_on_same_street(path[i], path[i + 1])) //if change street then plus penalty time
            penalty_count++;
        if (i + 2 == path.size()) { //if next segment is the last segment in the path
            i += 1;
            total_time += find_street_segment_travel_time(path[i]);
            break;
        }

    }
    double total_penalty_time = penalty_count * turn_penalty; //total time is the total travel time plus punish time
    total_time += total_penalty_time;
    return total_time;
}

vector<unsigned> find_path_between_intersections(const unsigned intersect_id_start, const unsigned intersect_id_end, const float turn_penalty) {

    from_inter_id = intersect_id_start;

    unsigned tot_inters_number = globalBase ->get_intersection_number(); //save total number of intersections
    if (intersect_id_start >= tot_inters_number || intersect_id_end >= tot_inters_number) { //if value of an inter id out_of_range
        vector<unsigned> completely_empty = {};
        return completely_empty;
    }
    vector<unsigned> pre_inter(tot_inters_number, tot_inters_number); //the previous intersection of any intersection
    vector<unsigned> pre_seg(tot_inters_number, tot_inters_number); //the previous segment of any intersection
    vector<double> time_cost(tot_inters_number, tot_inters_number); //the shortest total time cost to any intersection
    vector<unsigned> inter_visited(tot_inters_number, tot_inters_number); //check whether the intersection has been visited or not
    map<double, unsigned> final_time_used; //save weighed score to a point

    time_cost[intersect_id_start] = 0;

    t_point destination = globalBase ->get_t_point_intersection_pos(intersect_id_end);
    t_point start_point = globalBase ->get_t_point_intersection_pos(intersect_id_start);
    double initial_distance = (start_point.x - destination.x)*(start_point.x - destination.x) +(start_point.y - destination.y)*(start_point.y - destination.y);

    final_time_used[initial_distance] = intersect_id_start; //set the initial distance to the start point (no time yet)

    while (!final_time_used.empty()) {
        auto fastest = final_time_used.begin(); //start from the fastest point for now (highest weighed score)
        unsigned cur_inter = fastest->second; //get the intersection id
        if (cur_inter == intersect_id_end) { //if found
            vector<unsigned> segs;
            while (cur_inter != intersect_id_start) {
                segs.push_back(pre_seg[cur_inter]);
                cur_inter = pre_inter[cur_inter];
            } //get the segs in the reverse order
            reverse(segs.begin(), segs.end());
            return segs;
        }

        inter_visited[cur_inter] = cur_inter; //set the cur_inter to cur_inter so that it is visited
        final_time_used.erase(fastest);
        vector<unsigned> segs = globalBase ->find_intersection_street_segments_in_DB(cur_inter);
        for (auto iter = segs.begin(); iter != segs.end(); iter++) { //for all the segments connected to the fastest inter
            StreetSegmentInfo cur_seg_info = globalBase ->get_segment_info(*iter);
            bool reachable = true;
            unsigned target = cur_seg_info.from;

            if (target == cur_inter)
                target = cur_seg_info.to;
            else if (cur_seg_info.oneWay)
                reachable = false; //check one way information

            if (reachable && inter_visited[target] != target) { //if reachable and has not been visited
                t_point id_pos = globalBase ->get_t_point_intersection_pos(target);
                double distance = (id_pos.x - destination.x)*(id_pos.x - destination.x) +(id_pos.y - destination.y)*(id_pos.y - destination.y);

                double time = find_street_segment_travel_time(*iter); //get the travel time

                if (cur_inter != intersect_id_start) {
                    unsigned prev_seg = pre_seg[cur_inter];
                    unsigned prev_str = globalBase ->get_segment_info(prev_seg).streetID;
                    unsigned cur_str = cur_seg_info.streetID;

                    if (prev_str != cur_str)
                        time = time + turn_penalty; //get the street turning penalty
                }

                double total_cost = time_cost[cur_inter] + time; //total time cost
                double tot_time = total_cost + distance; //total score got

                if (time_cost[target] == tot_inters_number || total_cost < time_cost[target]) { //if not yet recorded or faster than last time visited
                    pre_inter[target] = cur_inter;
                    pre_seg[target] = *iter;
                    time_cost[target] = total_cost;
                    final_time_used[tot_time] = target;

                }
            }
        }
    }
    vector<unsigned> completely_empty = {};
    return completely_empty;
}

std::vector<unsigned> find_path_to_point_of_interest(const unsigned intersect_id_start, const std::string point_of_interest_name, const double turn_penalty) {
    from_inter_id = intersect_id_start;
    vector<unsigned> poi_ids = globalBase -> get_all_POI_ids_according_to_name(point_of_interest_name);
    if (poi_ids.empty()) {
        return poi_ids;
    } else {
        vector<unsigned> list_of_inters;
        for (unsigned i = 0; i < poi_ids.size(); i++) {
            t_point poi_t_point = globalBase -> get_POI_id_to_t_point(poi_ids[i]);
            LatLon poi_latlon = globalBase -> t_point_to_latlon(poi_t_point);
            unsigned closest_inter = globalBase -> find_closest_intersection_in_DB(poi_latlon);
            list_of_inters.push_back(closest_inter);
        } //difference between this function and the last one is 
        unsigned tot_inters_number = globalBase ->get_intersection_number(); //instead of find one specific inter, find whether the inter is in the
        vector<unsigned> pre_inter(tot_inters_number, tot_inters_number); //list_of_inter. if in, then the result is found and valid
        vector<unsigned> pre_seg(tot_inters_number, tot_inters_number);
        vector<double> time_cost(tot_inters_number, tot_inters_number);     //just set a random number
        vector<unsigned> inter_visited(tot_inters_number, tot_inters_number);
        map<double, unsigned> final_time_used;
        double zero = 0;
        time_cost[intersect_id_start] = 0;
        final_time_used[zero] = intersect_id_start;
        while (!final_time_used.empty()) {
            auto fastest = final_time_used.begin();
            unsigned cur_inter = fastest->second;
            if (find(list_of_inters.begin(), list_of_inters.end(), cur_inter) != list_of_inters.end()) { //found the name in the list
                vector<unsigned> segs;
                while (cur_inter != intersect_id_start) {
                    segs.push_back(pre_seg[cur_inter]);
                    cur_inter = pre_inter[cur_inter];
                }
                reverse(segs.begin(), segs.end());
                return segs;
            }
            inter_visited[cur_inter] = cur_inter;
            final_time_used.erase(fastest);
            vector<unsigned> segs = globalBase ->find_intersection_street_segments_in_DB(cur_inter);
            for (auto iter = segs.begin(); iter != segs.end(); iter++) {
                StreetSegmentInfo cur_seg_info = globalBase ->get_segment_info(*iter);
                bool reachable = true;
                unsigned target = cur_seg_info.from;
                if (target == cur_inter)
                    target = cur_seg_info.to;
                else if (cur_seg_info.oneWay)
                    reachable = false;
                if (reachable && inter_visited[target] != target) {
                    double time = find_street_segment_travel_time(*iter);
                    if (cur_inter != intersect_id_start) { //in this if statement, check whether change street or not
                        unsigned prev_seg = pre_seg[cur_inter];
                        unsigned prev_str = globalBase ->get_segment_info(prev_seg).streetID;
                        unsigned cur_str = cur_seg_info.streetID;
                        if (prev_str != cur_str)
                            time = time + turn_penalty;
                    }
                    double total_cost = time_cost[cur_inter] + time;
                    if (time_cost[target] == tot_inters_number || total_cost < time_cost[target]) { //no distance means only time will be considered in weighed score
                        pre_inter[target] = cur_inter;
                        pre_seg[target] = *iter;
                        time_cost[target] = total_cost;
                        final_time_used[total_cost] = target;
                    }
                }
            }
        }
        vector<unsigned> emp = {};
        return emp;

    }
    vector<unsigned> emp = {};
    return emp;
}
//find path between two pois
vector<unsigned> find_path_between_pois(const unsigned poi_start_id, const std::string poi_end_name, const double turn_penalty) {
    vector<unsigned> poi_end_ids = globalBase -> get_all_POI_ids_according_to_name(poi_end_name);
    t_point poi_start_t_point = globalBase->get_POI_id_to_t_point(poi_start_id);
    LatLon poi_start_latlon = globalBase -> t_point_to_latlon(poi_start_t_point);
    unsigned closest_inter1 = globalBase -> find_closest_intersection_in_DB(poi_start_latlon);
    from_inter_id = closest_inter1;
    unsigned closest_inter2;
    if (poi_end_ids.empty()) {
        return poi_end_ids;
    } else if (poi_end_ids.size() == 1) {
        unsigned poi_end_id = poi_end_ids[0];
        t_point poi_end_t_point = globalBase -> get_POI_id_to_t_point(poi_end_id);
        LatLon poi_end_latlon = globalBase -> t_point_to_latlon(poi_end_t_point);
        closest_inter2 = globalBase -> find_closest_intersection_in_DB(poi_end_latlon);
        vector<unsigned> seg_ids = find_path_between_intersections(closest_inter1, closest_inter2, turn_penalty);
        return seg_ids;
    } else {
        vector<vector<unsigned>> list(poi_end_ids.size()); //store all the segs to all the pois
        for (unsigned i = 0; i < poi_end_ids.size(); i++) {
            unsigned poi_end_id = poi_end_ids[i];
            t_point poi_end_t_point = globalBase -> get_POI_id_to_t_point(poi_end_id);
            LatLon poi_end_latlon = globalBase -> t_point_to_latlon(poi_end_t_point);
            closest_inter2 = globalBase -> find_closest_intersection_in_DB(poi_end_latlon);
            vector<unsigned> seg_ids = find_path_between_intersections(closest_inter1, closest_inter2, turn_penalty);
            list[i] = seg_ids;
        }
        vector<bool> valid(poi_end_ids.size());
        vector<double> travel_times(poi_end_ids.size());
        for (unsigned i = 0; i < poi_end_ids.size(); i++) {
            if (list[i].empty()) {
                travel_times[i] = -1;
                valid[i] = false;
            } else {
                double time_used = compute_path_travel_time(list[i], turn_penalty);
                travel_times[i] = time_used;
                valid[i] = true;
            }
        }
        unsigned id = 0;
        double final_time = -1;
        for (unsigned i = 0; i < poi_end_ids.size(); i++) {
            if (valid[i]) {
                if (travel_times[i] < final_time || final_time == -1) {
                    final_time = travel_times[i];
                    id = i;
                }
            }
        }
        if (final_time == -1) {
            vector<unsigned> emp = {};
            return emp;
        } else {
            return list[id];
        }
    }
    vector<unsigned> emp = {};
    return emp;



}

unsigned get_the_best_POI_id(const unsigned intersect_id_start, const std::string point_of_interest_name, const double turn_penalty) {
    from_inter_id = intersect_id_start;
    vector<unsigned> poi_ids = globalBase -> get_all_POI_ids_according_to_name(point_of_interest_name);
    if (poi_ids.empty()) {
        return 0;
    } else {
        vector<unsigned> list_of_inters;
        for (unsigned i = 0; i < poi_ids.size(); i++) {
            t_point poi_t_point = globalBase -> get_POI_id_to_t_point(poi_ids[i]);
            LatLon poi_latlon = globalBase -> t_point_to_latlon(poi_t_point);
            unsigned closest_inter = globalBase -> find_closest_intersection_in_DB(poi_latlon);
            list_of_inters.push_back(closest_inter);
        }
        unsigned tot_inters_number = globalBase ->get_intersection_number();
        vector<unsigned> pre_inter(tot_inters_number, tot_inters_number);
        vector<unsigned> pre_seg(tot_inters_number, tot_inters_number);
        vector<double> time_cost(tot_inters_number, tot_inters_number);
        vector<unsigned> inter_visited(tot_inters_number, tot_inters_number);
        map<double, unsigned> final_time_used;
        double zero = 0;
        time_cost[intersect_id_start] = 0;
        final_time_used[zero] = intersect_id_start;

        while (!final_time_used.empty()) {
            auto fastest = final_time_used.begin();
            unsigned cur_inter = fastest->second;
            if (find(list_of_inters.begin(), list_of_inters.end(), cur_inter) != list_of_inters.end()) {
                for (unsigned i = 0; i < list_of_inters.size(); i++) {
                    if (list_of_inters[i] == cur_inter)
                        return poi_ids[i];
                }
            }

            inter_visited[cur_inter] = cur_inter;
            final_time_used.erase(fastest);
            vector<unsigned> segs = globalBase ->find_intersection_street_segments_in_DB(cur_inter);
            for (auto iter = segs.begin(); iter != segs.end(); iter++) {
                StreetSegmentInfo cur_seg_info = globalBase ->get_segment_info(*iter);
                bool reachable = true;
                unsigned target = cur_seg_info.from;

                if (target == cur_inter)
                    target = cur_seg_info.to;
                else if (cur_seg_info.oneWay)
                    reachable = false;

                if (reachable && inter_visited[target] != target) {
                    t_point id_pos = globalBase ->get_t_point_intersection_pos(target);

                    double time = find_street_segment_travel_time(*iter);

                    if (cur_inter != intersect_id_start) {
                        unsigned prev_seg = pre_seg[cur_inter];
                        unsigned prev_str = globalBase ->get_segment_info(prev_seg).streetID;
                        unsigned cur_str = cur_seg_info.streetID;

                        if (prev_str != cur_str)
                            time = time + turn_penalty;
                    }

                    double total_cost = time_cost[cur_inter] + time;

                    if (time_cost[target] == tot_inters_number || total_cost < time_cost[target]) {
                        pre_inter[target] = cur_inter;
                        pre_seg[target] = *iter;
                        time_cost[target] = total_cost;
                        final_time_used[total_cost] = target;

                    }
                }
            }
        }
        return 0;

    }
    return 0;
}

unsigned get_the_best_POI_id_pois(const unsigned poi_start_id, const std::string point_of_interest_name, const double turn_penalty) {
    vector<unsigned> poi_ids = globalBase -> get_all_POI_ids_according_to_name(point_of_interest_name);
    t_point poi_start_t_point = globalBase->get_POI_id_to_t_point(poi_start_id);
    LatLon poi_start_latlon = globalBase -> t_point_to_latlon(poi_start_t_point);
    unsigned closest_inter1 = globalBase -> find_closest_intersection_in_DB(poi_start_latlon);
    if (poi_ids.empty()) {
        return 0;
    } else if (poi_ids.size() == 1) {
        unsigned poi_id = poi_ids[0];
        t_point poi_t_point = globalBase -> get_POI_id_to_t_point(poi_id);
        LatLon poi_latlon = globalBase -> t_point_to_latlon(poi_t_point);
        unsigned closest_inter2 = globalBase -> find_closest_intersection_in_DB(poi_latlon);
        vector<unsigned> seg_ids = find_path_between_intersections(closest_inter1, closest_inter2, turn_penalty);
        if (seg_ids.empty()) {
            return 0;
        } else {
            return poi_ids[0];
        }
    } else {
        vector<vector<unsigned>> list(poi_ids.size()); //store all the segs to all the pois
        for (unsigned i = 0; i < poi_ids.size(); i++) {
            unsigned poi_id = poi_ids[i];
            t_point poi_t_point = globalBase -> get_POI_id_to_t_point(poi_id);
            LatLon poi_latlon = globalBase -> t_point_to_latlon(poi_t_point);
            unsigned closest_inter2 = globalBase -> find_closest_intersection_in_DB(poi_latlon);
            vector<unsigned> seg_ids = find_path_between_intersections(closest_inter1, closest_inter2, turn_penalty);
            list[i] = seg_ids;
        }
        vector<bool> valid(poi_ids.size());
        vector<double> travel_times(poi_ids.size());
        for (unsigned i = 0; i < poi_ids.size(); i++) {
            if (list[i].empty()) {
                travel_times[i] = -1;
                valid[i] = false;
            } else {
                double time_used = compute_path_travel_time(list[i], turn_penalty);
                travel_times[i] = time_used;
                valid[i] = true;
            }
        }
        unsigned id = 0;
        double final_time = -1;
        for (unsigned i = 0; i < poi_ids.size(); i++) {
            if (valid[i]) {
                if (travel_times[i] < final_time || final_time == -1) {
                    final_time = travel_times[i];
                    id = i;
                }
            }
        }
        if (final_time == -1) {
            return 0;
        } else {
            return poi_ids[id];
        }

    }
    return 0;
}
//process direction 
seg_direction_handler direction_process(vector<unsigned> path_segments) {
    seg_direction_handler result;
    unsigned seg_id = path_segments.at(0);
    vector<string> instructions;
    vector<float> speedlimits;
    vector<double> travelTime;
    vector<double> streetDistance;
    vector<unsigned> intersection_id;
    vector<vector<unsigned>> streetSegmentsIds;
    vector<unsigned> temp_segment_ids;
    unsigned temp_intersection_id;
    double temp_travel_time;
    string instruction;
    double distance_temp;
    float speedlimit_temp;
    vector<path_street_catogariser> street_categorized = categorize_street_segments(path_segments);
    int additional_instruction_number = street_categorized.size() - 1;
    path_street_catogariser previous_street = street_categorized[0];
    int total_instruction_number = street_categorized.size() + additional_instruction_number;
    //loop through street categorized
    for (unsigned i = 0; i < street_categorized.size(); i++) {
        path_street_catogariser current_street = street_categorized[i];
        unsigned startringSegment_id = current_street.starting_segment_id;
        unsigned endingSegment_id = current_street.ending_segment_id;
        if (i != 0) {
            //instruction for left and right
            string direction = direction_process_helper(previous_street.endInter, previous_street.ending_segment_id, current_street.starting_segment_id);
            string instruction_temp = direction;
            instructions.push_back(instruction_temp);
            speedlimit_temp = -1;
            temp_travel_time = -1;
            distance_temp = -1;
            temp_intersection_id = previous_street.endInter;
            intersection_id.push_back(temp_intersection_id);
            streetDistance.push_back(distance_temp);
            speedlimits.push_back(speedlimit_temp);
            travelTime.push_back(temp_travel_time);
            temp_segment_ids = {};
            streetSegmentsIds.push_back(temp_segment_ids);
        }
        double distance_temp = current_street.distance_on_street;
        auto it = find(path_segments.begin(), path_segments.end(), startringSegment_id);
        auto it1 = find(path_segments.begin(), path_segments.end(), endingSegment_id);
        unsigned index1 = distance(path_segments.begin(), it);
        unsigned index2 = distance(path_segments.begin(), it1);
        unsigned distance_between_two_index = distance(it, it1);
        for (unsigned i = 0; i <= distance_between_two_index; i++) {
            temp_segment_ids.push_back(path_segments[index1 + i]);
        }
        //instruction for "stay on"
        string instruction_temp;
        instruction_temp = "Stay On " + globalBase->find_street_name_from_ids_in_DB(current_street.streetID);
        speedlimit_temp = current_street.speedlimit;
        temp_travel_time = (current_street.distance_on_street * 60 * 60) / (1000 * current_street.speedlimit);
        temp_intersection_id = current_street.beginInter;
        intersection_id.push_back(temp_intersection_id);
        travelTime.push_back(temp_travel_time);
        speedlimits.push_back(speedlimit_temp);
        instructions.push_back(instruction_temp);
        streetDistance.push_back(distance_temp);
        if (i == street_categorized.size() - 1) {
            intersection_id.push_back(current_street.endInter);
        }
        streetSegmentsIds.push_back(temp_segment_ids);
        //cout << "Stay On " << globalBase->find_street_name_from_ids_in_DB(current_street.streetID) << " For " << distance_optimizer(current_street.distance_on_street) << " Meters" << end
        previous_street = current_street;
    }
    result.instruction = instructions;
    result.speedlimit = speedlimits;
    result.street_distance = streetDistance;
    result.travel_time = travelTime;
    result.intersection_id = intersection_id;
    result.street_segments_ids = streetSegmentsIds;
    return result;
}

//categorize path into street 
vector<path_street_catogariser> categorize_street_segments(vector<unsigned> segment_ids) {
    vector<path_street_catogariser> street_categorised;
    float speedlimits;
    unsigned segment_id = segment_ids[0];
    unsigned current_street_id;
    unsigned next_street_id;
    unsigned ending_segment = segment_ids[0];
    double distance_on_street;
    StreetSegmentInfo next_info;
    StreetSegmentInfo current_info = globalBase->get_segment_info(segment_id);
    distance_on_street = globalBase->find_street_segment_length_in_DB(segment_id);
    current_street_id = current_info.streetID;
    speedlimits = current_info.speedLimit;
    path_street_catogariser temp;
    unsigned starting_segment = segment_ids[0];
    temp.streetID = current_street_id;
    temp.ending_segment_id = ending_segment;
    temp.distance_on_street = distance_on_street;
    temp.beginInter = from_inter_id;
    temp.speedlimit = speedlimits;
    temp.starting_segment_id = starting_segment;
    if (current_info.from == from_inter_id) {
        temp.endInter = current_info.to;
    } else {
        temp.endInter = current_info.from;
    }
    //start from segment 0
    for (unsigned i = 0; i < segment_ids.size(); i++) {
        unsigned temp_segment_id = segment_ids[i];
        next_street_id = globalBase->get_segment_info(temp_segment_id).streetID;
        if (current_street_id != next_street_id) {
            unsigned last_end_inter = temp.endInter;
            //push_back last street_categorised
            street_categorised.push_back(temp);
            current_street_id = next_street_id;
            starting_segment = temp_segment_id;
            ending_segment = temp_segment_id;
            temp.streetID = current_street_id;
            temp.ending_segment_id = ending_segment;
            temp.starting_segment_id = starting_segment;
            temp.distance_on_street = globalBase->find_street_segment_length_in_DB(temp_segment_id);
            temp.beginInter = last_end_inter;
            current_info = globalBase->get_segment_info(temp_segment_id);
            temp.speedlimit = current_info.speedLimit;
            if (current_info.from == last_end_inter) {
                temp.endInter = current_info.to;
            } else {
                temp.endInter = current_info.from;
            }
            if (i == segment_ids.size() - 1) {
                street_categorised.push_back(temp);
            }
        } else {
            if (i != 0) {
                //add distance
                temp.distance_on_street += globalBase->find_street_segment_length_in_DB(temp_segment_id);
                //begin ID doesn't change
                temp.beginInter = temp.beginInter;
                temp.speedlimit = temp.speedlimit;
                temp.starting_segment_id = temp.starting_segment_id;
                temp.ending_segment_id = temp_segment_id;
                unsigned last_end_inter = temp.endInter;
                current_info = globalBase->get_segment_info(temp_segment_id);
                if (current_info.from == last_end_inter) {
                    temp.endInter = current_info.to;
                } else {
                    temp.endInter = current_info.from;
                }
                if (i == segment_ids.size() - 1) {
                    street_categorised.push_back(temp);
                }
            }
        }
    }
    return street_categorised;
}
//helper function for left and right
string direction_process_helper(unsigned inter_id, unsigned from_segment_id, unsigned to_segment_id) {
    vector<unsigned> street_segments_at_intersection = globalBase->find_intersection_street_segments_in_DB(inter_id);
    unsigned crossroads_number = street_segments_at_intersection.size();
    //now we have to determine street order from left to right
    vector<angle_and_street_id> angle;
    double temp_angle;
    angle_and_street_id temp;
    StreetSegmentInfo temp_info;
    float inter_pos_x;
    string instruction;
    float inter_pos_y;
    unsigned previous_street_index;
    unsigned next_street_index;
    //get angle of every segment at intersection
    for (unsigned i = 0; i < crossroads_number; i++) {
        unsigned segment_id = street_segments_at_intersection[i];
        if (segment_id == from_segment_id) {
            previous_street_index = i;
        } else if (segment_id == to_segment_id) {
            next_street_index = i;
        }
        temp_info = globalBase->get_segment_info(segment_id);
        if (temp_info.from == inter_id) {
            t_point inter_pos = globalBase->get_t_point_intersection_pos(inter_id);
            inter_pos_x = inter_pos.x;
            inter_pos_y = inter_pos.y;
            if (temp_info.curvePointCount != 0) {
                t_point curve_pos = globalBase->find_closest_curve_point_to_an_intersection_in_t_point(inter_id, segment_id);
                float curve_pos_x = curve_pos.x;
                float curve_pos_y = curve_pos.y;
                temp_angle = atan((curve_pos_y - inter_pos_y) / (curve_pos_x - inter_pos_x));
                if (curve_pos_y >= inter_pos_y && curve_pos_x >= inter_pos_x) {
                    temp_angle = temp_angle;
                } else if (curve_pos_y <= inter_pos_y && curve_pos_x >= inter_pos_x) {
                    temp_angle = temp_angle;
                } else if (curve_pos_y >= inter_pos_y && curve_pos_x <= inter_pos_x) {
                    temp_angle = temp_angle + 180;
                } else if (curve_pos_y <= inter_pos_y && curve_pos_x <= inter_pos_x) {
                    temp_angle = temp_angle - 180;
                }
                temp.angle = temp_angle;
                temp.streetid = temp_info.streetID;
                temp.segment_id = segment_id;
                angle.push_back(temp);
            } else {
                t_point inter_pos2 = globalBase->get_t_point_intersection_pos(temp_info.to);
                float inter_pos2_x = inter_pos2.x;
                float inter_pos2_y = inter_pos2.y;
                temp_angle = atan((inter_pos2_y - inter_pos_y) / (inter_pos2_x - inter_pos_x));
                if (inter_pos2_y >= inter_pos_y && inter_pos2_x >= inter_pos_x) {
                    temp_angle = temp_angle;
                } else if (inter_pos2_y <= inter_pos_y && inter_pos2_x >= inter_pos_x) {
                    temp_angle = temp_angle;
                } else if (inter_pos2_y >= inter_pos_y && inter_pos2_x <= inter_pos_x) {
                    temp_angle = temp_angle + 180;
                } else if (inter_pos2_y <= inter_pos_y && inter_pos2_x <= inter_pos_x) {
                    temp_angle = temp_angle - 180;
                }
                temp.angle = temp_angle;
                temp.streetid = temp_info.streetID;
                temp.segment_id = segment_id;
                angle.push_back(temp);
            }
        }
        //compare angle
        if (temp_info.to == inter_id) {
            t_point inter_pos = globalBase->get_t_point_intersection_pos(inter_id);
            inter_pos_x = inter_pos.x;
            inter_pos_y = inter_pos.y;
            if (temp_info.curvePointCount != 0) {
                t_point curve_pos = globalBase->find_closest_curve_point_to_an_intersection_in_t_point(inter_id, segment_id);
                float curve_pos_x = curve_pos.x;
                float curve_pos_y = curve_pos.y;
                temp_angle = atan((curve_pos_y - inter_pos_y) / (curve_pos_x - inter_pos_x));
               //compare angle situation 1
                if (curve_pos_y >= inter_pos_y && curve_pos_x >= inter_pos_x) {
                    temp_angle = temp_angle;
                } else if (curve_pos_y <= inter_pos_y && curve_pos_x >= inter_pos_x) {
                     //compare angle situation 2
                    temp_angle = temp_angle;
                } else if (curve_pos_y >= inter_pos_y && curve_pos_x <= inter_pos_x) {
                     //compare angle situation 3
                    temp_angle = temp_angle + 180;
                } else if (curve_pos_y <= inter_pos_y && curve_pos_x <= inter_pos_x) {
                     //compare angle situation 4
                    temp_angle = temp_angle - 180;
                }
                temp.angle = temp_angle;
                temp.streetid = temp_info.streetID;
                temp.segment_id = segment_id;
                angle.push_back(temp);
            } else {
                t_point inter_pos2 = globalBase->get_t_point_intersection_pos(temp_info.from);
                float inter_pos2_x = inter_pos2.x;
                float inter_pos2_y = inter_pos2.y;
                temp_angle = atan((inter_pos2_y - inter_pos_y) / (inter_pos2_x - inter_pos_x));
                if (inter_pos2_y >= inter_pos_y && inter_pos2_x >= inter_pos_x) {
                    temp_angle = temp_angle;
                } else if (inter_pos2_y <= inter_pos_y && inter_pos2_x >= inter_pos_x) {
                    temp_angle = temp_angle;
                } else if (inter_pos2_y >= inter_pos_y && inter_pos2_x <= inter_pos_x) {
                    temp_angle = temp_angle + 180;
                } else if (inter_pos2_y <= inter_pos_y && inter_pos2_x <= inter_pos_x) {
                    temp_angle = temp_angle - 180;
                }
                temp.angle = temp_angle;
                temp.streetid = temp_info.streetID;
                temp.segment_id = segment_id;
                angle.push_back(temp);
            }
        }
    }

    float anglenow = abs((angle[next_street_index].angle - angle[previous_street_index].angle) - 180);
    bool facingdirection = (anglenow <= 15);
    bool enterlane = false;
    if (angle.size() == 3) {
        for (unsigned i = 0; i < 3; i++) {
            StreetSegmentInfo temp1_segment = globalBase->get_segment_info(to_segment_id);
            StreetSegmentInfo temp2_info = globalBase->get_segment_info(angle[i].segment_id);
            if (i != previous_street_index && i != previous_street_index) {
                if (angle[i].streetid == temp1_segment.streetID && temp2_info.to == inter_id && temp2_info.oneWay == true) {
                    facingdirection = false;
                    enterlane = true;
                } else {
                    //if(angle[i].streetid==temp1_segment.streetID&&temp2_info.to==inter_id&&temp2_info.oneWay==true){
                    enterlane = false;
                }
            }
        }
    }
    //different instructions
    string next_street_name = globalBase->find_street_name_from_ids_in_DB(globalBase->get_segment_info(to_segment_id).streetID);
    //negative y axis
    if (angle[previous_street_index].angle >= -180 && angle[previous_street_index].angle <= 0) {
        if (facingdirection) {
            instruction = "Go Straight Onto " + next_street_name;
        } else if (enterlane) {
            instruction = "Merge onto " + next_street_name;
        } else if (angle[next_street_index].angle < angle[previous_street_index].angle || angle[next_street_index].angle - angle[previous_street_index].angle > 180) {
            instruction = "Turn Left Onto " + next_street_name;
        } else if (angle[next_street_index].angle > angle[previous_street_index].angle && angle[next_street_index].angle - angle[previous_street_index].angle < 180) {
            instruction = "Turn Right Onto " + next_street_name;
        }
    }//positive y axis
    else if (angle[previous_street_index].angle >= 0 && angle[previous_street_index].angle <= 180) {
        if (facingdirection) {
            instruction = "Go Straight Onto " + next_street_name;
        } else if (enterlane) { //instruction for merging
            instruction = "Merge onto  " + next_street_name;
        }
        if (angle[next_street_index].angle > angle[previous_street_index].angle || angle[previous_street_index].angle - angle[next_street_index].angle > 180) {
            instruction = "Turn Right Onto " + next_street_name;
        } else if (angle[next_street_index].angle < angle[previous_street_index].angle && angle[previous_street_index].angle - angle[next_street_index].angle < 180) {
            instruction = "Turn Left Onto " + next_street_name;
        }
    }
    //    //find the most nearest to 180
    //    float temp_angle;
    //    float anglenow=abs((angle[next_street_index].angle - angle[previous_street_index].angle)-180);
    //    bool facing_next_segment=false;;
    //    if(angle.size()==4){
    //        for(unsigned i=0;i<4;i++){
    //            if(i!=previous_street_index&&i!=next_street_index){
    //             temp_angle=abs((angle[i].angle-angle[previous_street_index].angle)-180);
    //             if(temp_angle<anglenow){
    //                 facing_next_segment=true;
    //                 break;
    //             }
    //            }
    //        }
    //    }


    return instruction;

}



//optimize distance<1000

int distance_optimizer(double distance) {
    int result;
    int multiple = 10;
    int distance_temp = int(distance);
    result = distance_temp + multiple / 2;
    result -= result % multiple;
    return result;
}

//optimize distance>1000

double distance_optimizer_kilometer(double distance) {
    int result;
    int multiple = 100;
    int distance_temp = int(distance);
    result = distance_temp + multiple / 2;
    result -= result % multiple;
    double return_result = double(result) / double(1000);
    return return_result;
}

string double_to_string(double const& value) {
    stringstream sstr;
    sstr << value;
    return sstr.str();
}

string int_to_string(int const& value) {
    stringstream sstr;
    sstr << value;
    return sstr.str();
}
