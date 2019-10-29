/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m2.h
 * Author: hufeiran
 *
 * Created on February 13, 2017, 10:45 AM
 */

#ifndef M2_H
#define M2_H
#pragma once
#include <string>
#include <vector>
#include "OSMDatabaseAPI.h"
#include "m1.h"
#include "graphics.h"
const double Circle_Deg = 360;

//----------------------struct a new type-------------------
typedef struct {
    std::string type;
    unsigned id;
} Result_Type_And_ID;

//Draw this map. You can assume your load_map (string map_name)
//function is called before this function in the unit tests
//Your main () program should do the same
void draw_map();
void draw_screen();
void search_engine();
double setRoadAngle(t_point pos1, t_point pos2);

//functions and helpers to draw streets
void findFunction(void (*drawscreen_ptr)(void));
void draw_streets(int zoom, float radius);
void draw_segments(unsigned id, float radius);
void draw_highway(int zoom, float radius);
void draw_avenue(int zoom, float radius);
void draw_alleys(int zoom, float radius);
void draw_link(int zoom, float radius);
void draw_cycleways(int zoom, float radius);
void draw_features(int zoom);
void draw_feature_shape(unsigned feature_id);
void draw_street_name(unsigned id);
void draw_all_street_name();
void draw_POI();
float highlight_radius_generator();
void highlight_street(std::vector<unsigned>street_list);
void highlight_intersection(std::vector<unsigned> inter_list);
void highlight_point_of_interest(std::vector<unsigned> poi_list);
void highlight_feature(std::vector<unsigned> features_list);
void highlight_segs(std::vector<unsigned> segs_list);
void act_on_mousebutton(float x, float y, t_event_buttonPressed button_info);
void act_on_keypress(char c,int keysym);
bool mouse_click_in_rec(float x,float y,
        float left,float right,float uper,float bottom);
void draw_POI_name(t_point position, unsigned POI_id);
void processDuplicate(std::vector<unsigned> & temp, unsigned id);
void processForceAdd(std::vector<unsigned>& temp, unsigned id);
void act_on_cycleway_func(void (*drawscreen_ptr) (void));
void act_on_poi_func(void (*drawscreen_ptr) (void));
void act_on_clear_all(void (*drawscreen_ptr) (void));
void act_on_find_path(void (*drawscreen_ptr) (void));
void act_on_change_map(void (*drawscreen_ptr) (void));
void act_on_path_between_inters(void (*drawscreen_ptr) (void));
void search_function(std::string content);
void autoFillSquare(int barSelect);
int barSelect( bool up,bool down,int highlightBar);
void draw_route_planner_label();
void draw_route_planner_label_when_hit();
bool in_screen(t_point point);
int levenshtein_distance(std::string s,std::string t);
void search_closest();
void map_select(std::string selection);
void input_map2();
void draw_route_planner_label();
void draw_route_planner_label_when_hit();
void route_planner_interface();
void draw_street_name_highlight(unsigned highlight_id);
void street_auto_zoom_in(unsigned streetID);
void act_on_route_search_parser(void (*drawscreen_ptr)(void));
void act_on_delivery(void (*drawscreen_ptr)(void));
void route_search_parser();
void actOnReverseFunction();
void detailedInstruction_interface();
Result_Type_And_ID search_function_helper(std::string & name);
std::string fuzzy_search(std::string name);
std::string fuzzy_search_streets(std::string name); 
int travel_time_minutes(double travel_time_in_sec);
double travel_time_hours(double travel_time_in_sec);
void helperSymbol();
void showHelp();
void advancedHelp();
void helperSymbol_hit();
void highlight_segs_helper(std::vector<unsigned> segs_list);
void get_dimension(float left, float bottom, float right, float top);
void act_on_mouse_move(float x, float y) ;
std::vector<unsigned> get_intersection_id(std::string input);
void courier_text_version();
#endif /* M2_H */
