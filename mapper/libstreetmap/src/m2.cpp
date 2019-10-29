/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "StreetsDatabaseAPI.h"
#include "streetDB.h"
#include"global.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include<string>
#include <cassert>
#include <X11/keysym.h>
#include "m3.h"
#include "m2.h"
#include "m1.h"
#include "graphics.h"
#include "m4.h"
namespace std {
};


////-------------------------initialize name base----------------//
vector<vector < string>> name_database;

////-------------------------highlight vectors-------------------//
vector<unsigned> highlight_inters;
vector<unsigned> highlight_pois;
vector<unsigned> highlight_fea;
vector<unsigned> highlight_str;
vector<unsigned> highlight_path;
vector<unsigned> navigation_highlight_inters;
vector<unsigned> navigation_highlight_street;
//-----------------------------data for traveling courier-------------
vector<unsigned> highlight_depots = {};
vector<unsigned> highlight_pickUps = {};
vector<unsigned> highlight_dropOffs = {};
vector<DeliveryInfo> deliveries_info = {};
//---------------------------------
unsigned closest = 0;
unsigned closest2;
unsigned closest3;
unsigned find_path_inter1;
unsigned find_path_inter2;
unsigned find_path_between_inter_POI_inter; //for the inter
string find_path_between_inter_POI_POI; //for the poi
extern int zoom_level;
extern int real_zoom;
double screen_top = 0;
double screen_bottom = 0;
double screen_left = 0;
double screen_right = 0;
bool mouse_inside_search_box = false;
string content = "click here to search";
string oldcontent = "";
/*the following variables are used in direction graphic*/
string place1 = "";
string place2 = "";
bool type_place1 = false;
bool type_place2 = false;
bool draw_cycle = false;
bool show_auto_fill = false;
//-------------------------these are all click signals-------------------//
int routing_click = 0;
int cycle_button_click = 0;
int poi_button_click = 0;
int return_click = 0;
int search_click = 0;
int find_path_click = 0;
bool draw_poi = false;
bool name_not_found = false;
bool click_on_search = false;
bool start_search;
bool start_routing;
bool up_pressed = false;
bool keep_content = true;
int highlightBar = 0;
bool down_pressed = false;
bool showed_auto_fill = false;
bool click_on_reverse = false;
bool click_on_direction = false;
bool show_route_planner = false;
bool only_show_searchbar = false;
bool showInstruction = false;
bool show_help = false;
bool click_on_help = false;
int help_click_time = 0;
bool click_on_answer1 = false;
bool click_on_answer2 = false;
bool click_on_answer3 = false;
bool click_on_answer4 = false;
int advanced_help_click_time = 0;
//-----------------coordinates for search engine---------------------------//
float left_coordinate2;
float right_coordinate2;
float bottom_coordinate2;
float top_coordinate2;
float left_coordinate;
float top_coordinate;
float bottom_coordinate;
float right_coordinate;

//----------------coordiantes for auto fill ------------------------------//
float left_coordinate_autofill;
float right_coordinate_autofill;
float top1_coordinate_autofill;
float top2_coordinate_autofill;
float top3_coordinate_autofill;
float top4_coordinate_autofill;

//-----------------coordinates for route planner interface-----------------//
float route_left_coordinate;
float route_left_coordinate2;
float route_right_coordinate;
float route_right_coordinate2;
float route_top_coordinate;
float route_bottom_coordinate;

float place_text_bar1_top;
float place_text_bar2_top;
//float place_text_bar3_top;
//-----------------coordinates for detailed instruction interface-----------------//
float instruction_left;
float instruction_right;
float instruction_top;
float instruction_bottom;
float instruction_hint;
float instruction1_dvder;
float instruction2_dvder;
float instruction3_dvder;
float instruction4_dvder;
float instruction_height_offset;
//-----------------coordinates for help symbol-----------------//
float symbol_left;
float symbol_right;
float symbol_top;
float symbol_bottom;
//-----------------coordinates for help windows---------------------------//
float help_windows_left;
float help_windows_right;
float help_windows_top;
float help_windows_bottom;
float left_fram;
float right_fram;
float top_fram;
float bottom_fram;
float dvder1;
float dvder2;
float dvder3;
float dvder4;
//-----------------global variables for detailed instruction interface-----------------//

string instruction[5];
float speedLimit[5];
int directionValue = 0;
string distance_strings[5];
string time_strings[5];
unsigned intersectionids_in_route_planner[5];
//------------------------------------------------------------------------//

bool is_intersection = false;
string contentInBar1;
string contentInBar2;
string contentInBar3;
extern float map_left;
extern float map_bottom;
extern float map_right;
extern float map_top;
//global variable content
//string input into search bar
vector<std::string>string_input;
//string input into route plan S bar
vector<std::string>string_input_routeplan;
//string input into route plan R bar
vector<std::string>string_input_routeplan1;

//----------------------------------------total path travel time (driving) --------------------------------------------------//
int travel_time;

//----------------------------------------direction_instruction_database--------------------------------------------------------------//
seg_direction_handler direction_data;

//----------------------------------------instruction------------------------------------------------------------------------//
int instruction_pointing = 0;

//----------------------------------------path vector------------------------------------------------------------------------//
vector<unsigned> seg_list;
//----------------------------------------------functions implemented----------------------------------------------//

void draw_map() {//draw_map: initialize name_database
    name_database = globalBase->get_name_base();
    init_graphics("JianDan Map", t_color(229, 229, 229, 100));
    vector<double> visible_limits = globalBase->get_map_limits();
    set_visible_world(visible_limits[0], visible_limits[1], visible_limits[2], visible_limits[3]);
    map_left = visible_limits[0];
    map_bottom = visible_limits[1];
    map_right = visible_limits[2];
    map_top = visible_limits[3];
    name_not_found = false;
    set_keypress_input(true);
    set_mouse_move_input(true);
    create_button("Window", "Cycle Paths", act_on_cycleway_func);
    create_button("Cycle Paths", "POI", act_on_poi_func);
    create_button("POI", "Clear All", act_on_clear_all);
    create_button("Clear All", "Inter Path", act_on_path_between_inters);
    create_button("Inter Path", "Change Map", act_on_change_map);
    create_button("Change Map", "Courier Test", act_on_delivery);
    set_keypress_input(true);
    set_mouse_move_input(true);
    initial_zoom_in();
    event_loop(act_on_mousebutton, act_on_mouse_move, act_on_keypress, draw_screen);
    update_message("Loading Map");
}

void draw_screen() {
    //draw_screen
    set_draw_mode(DRAW_NORMAL); // Should set this if your program does any XOR drawing in callbacks.
    clearscreen(); /* Should precede drawing for all screendrawing */

    t_bound_box box = get_visible_screen();
    screen_top = box.top();
    screen_bottom = box.bottom();
    screen_left = box.left();
    screen_right = box.right();
    setfontsize(10);
    setlinestyle(SOLID);
    setlinewidth(1);
    setcolor(PINK);
    double radius = 0.00000001;
    if (real_zoom < 0) {
        zoom_level = 0;
    }

    //draw features
    draw_features(zoom_level);
    //draw streets
    draw_streets(zoom_level, radius);
    //if POI pressed, draw pois
    if (draw_poi) {
        draw_POI();
    }
    //if zoom level>=7 draw all street names
    // if (zoom_level >= 7) {
    draw_all_street_name();
    //}
    //highlight_path
    if (highlight_path.size() != 0) {
        highlight_segs(highlight_path);
    }
    //highlight intersections
    if (highlight_inters.size() != 0) {//highlight all intersection in highlight_inters list
        highlight_intersection(highlight_inters);
    }
    //highlight POIs
    if (highlight_pois.size() != 0) {// highlight all point of interest in highlight point of interest list
        highlight_point_of_interest(highlight_pois);
    }
    //highlight features
    if (highlight_fea.size() != 0) {
        highlight_feature(highlight_fea);
    }
    //highlight street
    if (highlight_str.size() != 0) {
        highlight_street(highlight_str);
    }
    //highlight intersections during navigation
    if (navigation_highlight_inters.size() != 0) {
        highlight_intersection(navigation_highlight_inters);
    }
    if (navigation_highlight_street.size() != 0) {
        highlight_segs_helper(navigation_highlight_street);
    }
    update_message("JianDan Map");
    search_engine();
    helperSymbol();
    if (show_route_planner&&!only_show_searchbar)
        route_planner_interface();
    //show instruction
    if (showInstruction) {
        detailedInstruction_interface();
    }
    if (help_click_time % 2 != 0) {
        showHelp();
    }
    if (advanced_help_click_time % 2 != 0) {
        advancedHelp();
    }
    // get_dimension(left_fram,bottom_fram,right_fram,top_fram);

}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//this function is to draw all streets

void draw_streets(int zoom, float radius) {
    //draw highway
    draw_highway(zoom, radius);
    //zoom level>=1 draw avenue and link
    if (zoom >= 1) {
        draw_avenue(zoom, radius);
        draw_link(zoom, radius);
    }
    //zoom level>=3 draw alleys
    if (zoom >= 3) {
        draw_alleys(zoom, radius / 2);
        //if draw cycle is ture, draw all cycles
        if (draw_cycle) {
            draw_cycleways(zoom, radius / 2);
        }
    }
    update_message("Streets Loaded");
}

void draw_segments(unsigned id, float radius) {
    StreetSegmentInfo segment_info = globalBase->get_segment_info(id);
    unsigned intersection1 = segment_info.from;
    unsigned intersection2 = segment_info.to;
    //get two t_points position between from and to 
    t_point pos_1 = globalBase->get_t_point_intersection_pos(intersection1);
    t_point pos_2 = globalBase->get_t_point_intersection_pos(intersection2);
    fillarc(pos_1, radius, 0, Circle_Deg);
    fillarc(pos_2, radius, 0, Circle_Deg);
    //get segment curve points number
    unsigned curve_points = segment_info.curvePointCount;
    //return a vector containing the XY coordinate of every curve point
    vector <t_point> curve_in_segment = globalBase->get_curve_points_in_t_point(id);
    if (curve_points == 0) {//if straight line, just draw from start to finish
        drawline(pos_1, pos_2);
        //fix segments
        fillarc(pos_1, radius, 0, Circle_Deg);
        fillarc(pos_2, radius, 0, Circle_Deg);
        if (zoom_level >= 9) {
            if (segment_info.oneWay == true) {
                t_bound_box arrow((pos_1.x) - 0.03, (pos_1.y) - 0.03, (pos_2.x) + 0.03, (pos_2.y) + 0.03);
                setcolor(DARKGREY);
                setfontsize(8);
                settextrotation(setRoadAngle(pos_1, pos_2));
                drawtext_in(arrow, "->");

            }
        }
    } else { //non-straight line
        t_point current = pos_1;
        for (unsigned i = 0; i < curve_in_segment.size(); ++i) {
            fillarc(current, radius, 0, Circle_Deg);
            drawline(current, curve_in_segment[i]);
            current = curve_in_segment[i];
        }
        fillarc(current, radius, 0, Circle_Deg);
        fillarc(pos_2, radius, 0, Circle_Deg);
        drawline(current, pos_2);
    }
}

void draw_highway(int zoom, float radius) {
    //draw the highway depending on zoom level and radius
    vector<unsigned> temp = globalBase->get_highway();
    for (unsigned i = 0; i < temp.size(); i++) {
        setlinestyle(SOLID);
        setcolor(t_color(248, 207, 144, 100));
        setlinewidth(zoom + 2);
        draw_segments(temp[i], radius);
    }
}

void draw_avenue(int zoom, float radius) {
    //draw avenue depending on zoom level and radius
    vector<unsigned> temp = globalBase->get_avenue();
    for (unsigned i = 0; i < temp.size(); i++) {
        setlinestyle(SOLID);
        setcolor(WHITE);
        setlinewidth(zoom);
        draw_segments(temp[i], radius * (0.8));
    }
}

void draw_link(int zoom, float radius) {
    //draw highway name first
    //draw links depending on zoom level and radius
    vector<unsigned> temp = globalBase->get_link();
    for (unsigned i = 0; i < temp.size(); i++) {
        setlinestyle(SOLID);
        setcolor(WHITE);
        setlinewidth(zoom - 1);
        draw_segments(temp[i], radius * (0.8));
    }

}

void draw_alleys(int zoom, float radius) {
    //draw alleys depending on zoom level and radius
    vector<unsigned> temp = globalBase->get_alleys();
    for (unsigned i = 0; i < temp.size(); i++) {
        setlinestyle(SOLID);
        setcolor(WHITE);
        setlinewidth(zoom - 3);
        draw_segments(temp[i], radius * (0.6));
    }
}

void draw_cycleways(int zoom, float radius) {
    //draw cycleways depending on zoom level and radius, coulor set as red
    vector<unsigned> temp = globalBase->get_cycleways();
    for (unsigned i = 0; i < temp.size(); i++) {
        setlinestyle(SOLID);
        setcolor(t_color(0, 76, 0, 100));
        setlinewidth(zoom - 3);
        draw_segments(temp[i], radius * (0.4));
    }

}

void draw_features(int zoom) {
    //draw features depending on zoom level and radius, different features have different colour
    setlinewidth(zoom / 5);
    setlinestyle(SOLID);
    for (unsigned i = 0; i < globalBase -> get_feature_number(); i++) {
        FeatureType f_type = globalBase -> get_feature_id_to_feature_type(i);
        if (f_type == Unknown) {
            //all colours are selected from google map, though it looks kind of kind of different from it
            setcolor(WHITE);
        } else if (f_type == Park) {
            setcolor(t_color(199, 220, 148, 100));
        } else if (f_type == Beach) {
            setcolor(t_color(248, 207, 144, 100));
        } else if (f_type == Lake) {
            setcolor(t_color(152, 193, 252, 100));
        } else if (f_type == River) {
            setcolor(t_color(152, 193, 252, 100));
        } else if (f_type == Island) {
            setcolor(t_color(248, 207, 144, 100));
        } else if (f_type == Shoreline) {
            setcolor(t_color(248, 207, 144, 100));
        } else if (f_type == Building) {
            if (zoom >= 8) {
                setcolor(t_color(117, 117, 117, 100));
            } else {
                continue;
            }
        } else if (f_type == Greenspace) {
            setcolor(t_color(199, 220, 148, 100));
        } else if (f_type == Golfcourse) {
            setcolor(t_color(199, 220, 148, 100));
        } else {
            setcolor(t_color(152, 193, 252, 100));
        }
        draw_feature_shape(i);
    }

}

void draw_feature_shape(unsigned feature_id) {
    //draw feature shape, getting data from features list
    bool closed = globalBase -> check_closed(globalBase -> get_feature_id_to_bound_points(feature_id));
    unsigned number_of_points = globalBase -> get_feature_id_to_feature_point_count(feature_id);
    vector<t_point> points = globalBase -> get_feature_id_to_bound_points(feature_id);
    if (closed) {// determining if the shape is closed
        t_point array_of_points[number_of_points];
        for (unsigned i = 0; i < number_of_points - 1; i++) {
            array_of_points[i].x = points[i].x;
            array_of_points[i].y = points[i].y;
            t_point next = points[i + 1];
            drawline(points[i], next);
        }
        array_of_points[number_of_points - 1].x = points[number_of_points - 1].x;
        array_of_points[number_of_points - 1].y = points[number_of_points - 1].y;
        fillpoly(array_of_points, number_of_points);

    } else {
        for (unsigned i = 0; i < number_of_points - 1; i++) {
            t_point next = points[i + 1];
            drawline(points[i], next);
        }
    }
}

double setRoadAngle(t_point pos1, t_point pos2) {
    // set the road angle 
    //depending on the start point and end point of street segments
    //mainly used for draw street names
    double angle;

    if (pos1.x == pos2.x) {
        return 0;
    }
    angle = atan((pos1.y - pos2.y) / (pos1.x - pos2.x)) * Circle_Deg / (2 * PI); //calculate arctan 
    for (int i = 0; angle < 0; i++) {
        angle += Circle_Deg;
    }
    return angle;
}

//have over-flapping 

void draw_street_name(unsigned id) {
    //draw street name in their street segments depending on the street rotate degree
    StreetSegmentInfo segment_info = globalBase->get_segment_info(id);
    unsigned intersection1 = segment_info.from;
    unsigned intersection2 = segment_info.to;
    //get two t_points position between from and to 
    t_point pos_1 = globalBase->get_t_point_intersection_pos(intersection1);
    t_point pos_2 = globalBase->get_t_point_intersection_pos(intersection2);
    t_point mid_pos;
    double segment_length = globalBase->find_street_segment_length_in_DB(id);
    double length = segment_length / 100000;
    vector<t_point> curve_points;
    double angle;
    string streetName = globalBase->find_street_name_from_ids_in_DB(segment_info.streetID);
    if (segment_info.curvePointCount == 0) {
        mid_pos.x = (pos_1.x + pos_2.x) / 2;
        mid_pos.y = (pos_1.y + pos_2.y) / 2;
        setcolor(t_color(117, 117, 117, 200));
        settextrotation(setRoadAngle(pos_1, pos_2));
        if (streetName != "<unknown>") {
            setfontsize(zoom_level);
            drawtext(mid_pos, streetName, length, length);
        }
    } else if (segment_info.curvePointCount == 1) {
        // pending if the segment only has one curve point
        // one curve point segments shall be drawn in different ways
        curve_points = globalBase->get_curve_points_in_t_point(id);
        t_point temp_1, temp_2;
        temp_1.x = (pos_1.x + curve_points[0].x) / 2;
        temp_1.y = (pos_1.y + curve_points[0].y) / 2;
        temp_2.x = (pos_2.x + curve_points[0].x) / 2;
        temp_2.y = (pos_2.y + curve_points[0].y) / 2;
        double angle1, angle2;
        angle1 = setRoadAngle(pos_1, temp_1);
        angle2 = setRoadAngle(temp_2, pos_2);
        if (streetName != "<unknown>") {
            //situation that this street has name, then draw the name
            setcolor(t_color(117, 117, 117, 200));
            setfontsize(zoom_level);
            settextrotation(angle1);
            drawtext(temp_1, streetName, length / 2, length / 2);
            settextrotation(angle2);
            drawtext(temp_2, streetName, length / 2, length / 2);
        }
    } else {
        //situation that this street has no name
        curve_points = globalBase->get_curve_points_in_t_point(id);
        mid_pos.x = (pos_1.x + curve_points[0].x) / 2;
        mid_pos.y = (pos_1.y + curve_points[0].y) / 2;
        for (unsigned i = 0; i < curve_points.size(); ++i) {
            angle = setRoadAngle(mid_pos, curve_points[i]);
            mid_pos.x = (mid_pos.x + curve_points[i].x) / 2;
            mid_pos.y = (mid_pos.y + curve_points[i].y) / 2;
        }
        angle = setRoadAngle(mid_pos, pos_2);
        mid_pos.x = (mid_pos.x + pos_2.x) / 2;
        mid_pos.y = (mid_pos.y + pos_2.y) / 2;
        setcolor(t_color(117, 117, 117, 200));
        settextrotation(angle);
        if (streetName != "<unknown>") {
            setfontsize(zoom_level);
            drawtext(mid_pos, streetName, length, length);
        }
    }

}
//draw name of all streets

void draw_all_street_name() {// draw all the streets with a loop
    unsigned i;
    if (zoom_level >= 6) {
        vector<unsigned> highway = globalBase->get_highway();
        for (i = 0; i < highway.size(); i++) {
            draw_street_name(highway[i]);
        }
        vector<unsigned> avenue = globalBase->get_avenue();
        for (i = 0; i < avenue.size(); i++) {
            draw_street_name(avenue[i]);
        }
    }
    if (zoom_level > 7) {
        vector<unsigned>link = globalBase->get_link();
        for (i = 0; i < link.size(); i++) {
            draw_street_name(link[i]);
        }
        vector<unsigned>alley = globalBase->get_alleys();
        for (i = 0; i < alley.size(); i++) {
            draw_street_name(alley[i]);
        }

    }
}

void draw_POI() {// only draw POI once the zoom level is greater than 7
    t_bound_box offset_wold = get_visible_world();
    float left_offset = offset_wold.get_width()* 0.007;
    float top_offset = offset_wold.get_height()*0.02;
    if (zoom_level > 7) {
        Surface hospital("HOSPITAL.png"); //different types are signed with icons
        Surface bank("BANK.png");
        Surface restaurant("RESTURANT.png");
        Surface hotel("HOTEL.png");
        Surface parking("PARKING.png");
        Surface oil("OIL.png");
        Surface other("POI.png");
        unsigned POI_size = globalBase -> POInumber();
        for (unsigned i = 0; i < POI_size; i++) {
            t_point POI_point = globalBase -> get_POI_id_to_t_point(i);
            string type = globalBase->get_POI_type(i);
            if (type == "clinic" || type == "pharmacy" || type == "doctors" || type == "hospital" || type == "dentist") {
                draw_surface(hospital, POI_point.x - left_offset, POI_point.y + top_offset);
            } else if (type == "bank" || type == "atm" || type == "financial" || type == "vending_machine") {
                draw_surface(bank, POI_point.x - left_offset, POI_point.y + top_offset);
            } else if (type == "bakery" || type == "cafe" || type == "fast_food" || type == "food_court" || type == "ice_cream" || type == "internet_cafe" || type == "restaurant"
                    || type == "sweets" || type == "pub") {
                draw_surface(restaurant, POI_point.x - left_offset, POI_point.y + top_offset);
            } else if (type == "hotel") {
                draw_surface(hotel, POI_point.x - left_offset, POI_point.y + top_offset);
            } else if (type == "parking") {
                draw_surface(parking, POI_point.x - left_offset, POI_point.y + top_offset);
            } else if (type == "fuel") {
                draw_surface(oil, POI_point.x - left_offset, POI_point.y + top_offset);
            } else {
                draw_surface(other, POI_point.x - left_offset, POI_point.y + top_offset);
            }
            if (zoom_level >= 8)//once the zoom level is greater than 8, keep drawing 
                draw_POI_name(POI_point, i);

        }
    }
}

float highlight_radius_generator() {
    //want to achieve the effect that size of highlight cycle  on the map remains the same
    float radius = 0.02;
    for (int i = 0; i < zoom_level; i++) {
        radius = radius * 0.5;
    }
    return radius;
}

void highlight_intersection(vector<unsigned> inter_list) {
    //receive inter_list, which contains all intersections that need to be highlighted
    t_bound_box offset_wold = get_visible_world();
    float left_offset = offset_wold.get_width()* 0.007;
    float top_offset = offset_wold.get_height()*0.02;
    top_coordinate = offset_wold.top() - top_offset;

    for (unsigned i = 0; i < inter_list.size(); i++) {
        unsigned id = inter_list[i];
        t_point inter_pos = globalBase->get_t_point_intersection_pos(id);
        Surface hi_inters("PIN.png");
        draw_surface(hi_inters, inter_pos.x - left_offset, inter_pos.y + top_offset);
        //0.00002*zoom_level
        setfontsize(12);
        setcolor(BLACK);
        settextrotation(0);
        string name = globalBase->get_intersection_name(id);
        drawtext(inter_pos.x, inter_pos.y - top_offset * 0.5, name, 200);
        update_message("Selected Intersection Name: " + name); //show the name of selected intersection at the bottom of the windows
    }
}

void highlight_street(vector<unsigned>street_list) {
    //get street list that needs to be highlighted
    //then get its segments list
    // then get its curve point list
    setlinewidth(zoom_level + 2);
    setcolor(t_color(0, 179, 253, 100));
    for (unsigned i = 0; i < street_list.size(); i++) {//for each street, find its street segments
        std::vector<unsigned>segment_list = globalBase->find_street_street_segments_in_DB(street_list[i]);
        for (unsigned j = 0; j < segment_list.size(); j++) {//for each segments, get their segments info
            StreetSegmentInfo segment_info = globalBase->get_segment_info(segment_list[j]);
            unsigned intersection1 = segment_info.from; //set from and to as two point on maps
            unsigned intersection2 = segment_info.to;
            //get two t_points position between from and to 
            t_point pos_1 = globalBase->get_t_point_intersection_pos(intersection1); //concert latlons to t_point on map
            t_point pos_2 = globalBase->get_t_point_intersection_pos(intersection2);

            unsigned curvePointCount = segment_info.curvePointCount; //get the number of curve point of this segments

            if (curvePointCount == 0) {//if straight line, just draw from start to finish
                drawline(pos_1, pos_2);
                //fix segments
            } else if (curvePointCount == 1) {
                vector <t_point> curve_in_segment = globalBase->get_curve_points_in_t_point(segment_list[j]);
                t_point mid_point = curve_in_segment[0];
                drawline(pos_1, mid_point);
                drawline(mid_point, pos_2);
            } else { //non-straight line
                vector <t_point> curve_in_segment = globalBase->get_curve_points_in_t_point(segment_list[j]);
                t_point current = pos_1;
                for (unsigned k = 0; k < curve_in_segment.size(); k++) {

                    drawline(current, curve_in_segment[i]);
                    current = curve_in_segment[i];
                }
                drawline(current, pos_2);
            }
        }
        draw_street_name_highlight(street_list[i]);
        setcolor(t_color(0, 179, 253, 100));
    }
}

//function to highlight street segments

void highlight_segs(vector<unsigned> segs_list) {
    setlinewidth(zoom_level + 2);
    double radius = 0.00000001;
    for (unsigned i = 0; i < segs_list.size(); i++) {
        setcolor(t_color(0, 179, 253, 100));
        draw_segments(segs_list[i], radius);
    }
}

//helper function to highlight street segments, but with color t_color(255,158,0,400)

void highlight_segs_helper(vector<unsigned> segs_list) {
    setlinewidth(zoom_level + 2);
    double radius = 0.00000001;
    for (unsigned i = 0; i < segs_list.size(); i++) {
        setcolor(t_color(255, 158, 0, 400));
        draw_segments(segs_list[i], radius);
    }
}

//function to highlight point_of_interest

void highlight_point_of_interest(vector<unsigned> poi_list) {
    t_bound_box offset_wold = get_visible_world();
    float left_offset = offset_wold.get_width()* 0.007;
    float top_offset = offset_wold.get_height()*0.03;

    // highlight all point of interest, this is mainly used for search engine 
    for (unsigned i = 0; i < poi_list.size(); i++) {
        unsigned id = poi_list[i];
        t_point poi_pos = globalBase->get_POI_id_to_t_point(id);
        Surface hi_inters("PIN.png");
        draw_surface(hi_inters, poi_pos.x - left_offset, poi_pos.y + top_offset);
        string poi_name = globalBase->get_POI_name_from_id(id);
        setfontsize(12);
        setcolor(BLACK);
        settextrotation(0);
        drawtext(poi_pos.x, poi_pos.y - top_offset * 0.5, poi_name, 200);
        update_message("Selected Point of Interest Name: " + poi_name);
    }
}

void highlight_feature(vector<unsigned> features_list) {
    // highlight features, this is mainly used for search engine 
    for (unsigned i = 0; i < features_list.size(); i++) {
        unsigned id = features_list[i];
        t_point feature_pos = globalBase->get_feature_average_pos(id);
        Surface hi_inters("PIN.png");
        float offset_X = screen_right - screen_left;
        float offset_y = screen_top - screen_bottom;
        draw_surface(hi_inters, feature_pos.x - offset_X * 0.0000001, feature_pos.y - offset_y * 0.0000004);
        string feature_name = globalBase->get_feature_id_to_feature_name(id);
        setfontsize(12);
        setcolor(BLACK);
        settextrotation(0);
        drawtext(feature_pos.x, feature_pos.y, feature_name, 200);
        update_message("Selected Feature Name: " + feature_name);
    }
}

void draw_POI_name(t_point position, unsigned POI_id) {
    t_bound_box offset_wold = get_visible_world();
    float top_offset = offset_wold.get_height()*0.012;
    //draw the name of point of interest, once it is clicked
    setcolor(t_color(117, 117, 117, 200));
    t_point textposition;
    textposition.x = position.x;
    textposition.y = position.y - top_offset;
    string name = globalBase -> get_POI_name_from_id(POI_id);
    setfontsize(zoom_level - 2);
    if (name == "<noname>")
        return;
    drawtext(textposition, name, 10, 10);
}
//

void act_on_mousebutton(float x, float y, t_event_buttonPressed button_info) {
    // mouse_inside_search_box=mouse_click_in_rec(x,y,button_info,search_box_left,search_box_right,search_box_upper,search_box_bottom);
    click_on_search = mouse_click_in_rec(x, y, left_coordinate, right_coordinate, top_coordinate2, bottom_coordinate2);
    bool click_on_help2 = mouse_click_in_rec(x, y, symbol_left, symbol_right, symbol_top, symbol_bottom);
    start_search = mouse_click_in_rec(x, y, right_coordinate, right_coordinate2, top_coordinate2, bottom_coordinate2);
    show_auto_fill = string_input.size() > 2 && content[0] != ' ';
    bool click_on_auto = show_auto_fill && (mouse_click_in_rec(x, y, left_coordinate_autofill, right_coordinate_autofill, top1_coordinate_autofill, top2_coordinate_autofill)
            || mouse_click_in_rec(x, y, left_coordinate_autofill, right_coordinate_autofill, top2_coordinate_autofill, top3_coordinate_autofill) ||
            mouse_click_in_rec(x, y, left_coordinate_autofill, right_coordinate_autofill, top3_coordinate_autofill, top4_coordinate_autofill) || mouse_click_in_rec(x, y, left_coordinate_autofill, right_coordinate_autofill, top3_coordinate_autofill, top4_coordinate_autofill));
    if (!click_on_help)
        click_on_help = mouse_click_in_rec(x, y, symbol_left, symbol_right, symbol_top, symbol_bottom);
    if (!start_routing)// Once the program has reached the route step, program does not need to determine the value of signal start_routing
        start_routing = mouse_click_in_rec(x, y, left_coordinate2, left_coordinate, top_coordinate2, bottom_coordinate2);

    t_point current_pos;
    current_pos.x = x;
    current_pos.y = y;
    //now transfer back into latlon coords
    //left click
    //if(mouse_inside_search_box())
    //if click on help button
    if (click_on_help) {
        //add click time
        if (mouse_click_in_rec(x, y, symbol_left, symbol_right, symbol_top, symbol_bottom))
            help_click_time++;
        if (help_click_time % 2 != 0) {
            showHelp();
            click_on_help = true;
            click_on_answer1 = mouse_click_in_rec(x, y, left_fram, right_fram, dvder1, dvder2);
            click_on_answer2 = mouse_click_in_rec(x, y, left_fram, right_fram, dvder2, dvder3);
            click_on_answer3 = mouse_click_in_rec(x, y, left_fram, right_fram, dvder3, dvder4);
            click_on_answer4 = mouse_click_in_rec(x, y, left_fram, right_fram, dvder4, bottom_fram);
            if (click_on_answer1 || click_on_answer2 || click_on_answer3 || click_on_answer4) {
                advanced_help_click_time++;
                if (advanced_help_click_time % 2 != 0) {
                    advancedHelp();
                } else {
                    showHelp();
                    //disable all detailed instruction
                    click_on_answer1 = false;
                    click_on_answer2 = false;
                    click_on_answer3 = false;
                    click_on_answer4 = false;
                }
            }
        } else {
            //disable all detailed instruction
            click_on_answer1 = false;
            click_on_answer2 = false;
            click_on_answer3 = false;
            click_on_answer4 = false;
            click_on_help = false;
            show_help = false;
            draw_screen();
        }
    }
    //if users click on neither search bar nor starting search button nor help button, highlight intersecton or POI
    if (!click_on_search&&!start_search&&!start_routing&&!click_on_help&&!click_on_auto&&!click_on_help2) {
        show_auto_fill = string_input.size() > 2 && content[0] != ' ';
        show_route_planner = false;

        LatLon current = globalBase->t_point_to_latlon(current_pos);
        unsigned highlight_inters_id = globalBase->find_closest_intersection_in_DB(current);
        unsigned hightlight_poi_id = globalBase->find_closest_point_of_interest_in_DB(current);
        float distance1 = globalBase->find_distance_in_t_point(current_pos, globalBase->get_t_point_intersection_pos(highlight_inters_id));
        float distance2 = globalBase->find_distance_in_t_point(current_pos, globalBase->get_POI_id_to_t_point(hightlight_poi_id));
        bool poi_marked = false;
        auto iter = find(highlight_pois.begin(), highlight_pois.end(), hightlight_poi_id);
        if (iter != highlight_pois.end()) {
            poi_marked = true;
        }
        if (distance1 > distance2 && poi_marked) {
            processDuplicate(highlight_pois, hightlight_poi_id);
        } else if (distance1 > distance2 && draw_poi) {
            processDuplicate(highlight_pois, hightlight_poi_id);
        } else {
            processDuplicate(highlight_inters, highlight_inters_id);
        }
        keep_content = true;
        draw_screen();

    } else if (click_on_search&&!start_search) {
        search_click++;
        name_not_found = false;
        if (search_click % 2 == 0) {
            only_show_searchbar = false;
            show_route_planner = false;
            click_on_search = false;
            showed_auto_fill = false;

            if (string_input.size() < 2) {
                search_engine();
            } else {
                draw_screen();
            }
        } else {
            only_show_searchbar = true;
        }
        if (show_route_planner) {
            keep_content = true;
        }
        search_engine();

        keep_content = true;
        if (show_auto_fill) {
            showed_auto_fill = true;
            autoFillSquare(highlightBar);

        }

    } else if (click_on_auto) {
        //if users click on auto fill result, the system automatically searchs the result and auto_zoom_in
        if (mouse_click_in_rec(x, y, left_coordinate_autofill, right_coordinate_autofill, top1_coordinate_autofill, top2_coordinate_autofill)) {
            content = contentInBar1;
            keep_content = false;
            search_function(content); //do the search function
            highlightBar = 0;
            content = "";
            string_input = {};
            click_on_search = false;
            string_input.resize(0);
            search_click++;
            search_engine();
        } else if (mouse_click_in_rec(x, y, left_coordinate_autofill, right_coordinate_autofill, top2_coordinate_autofill, top3_coordinate_autofill)) {
            content = contentInBar2;
            keep_content = false;
            search_function(content); //do the search function
            highlightBar = 0;
            content = "";
            string_input = {};
            click_on_search = false;
            string_input.resize(0);
            search_click++;
            search_engine();
        } else if (mouse_click_in_rec(x, y, left_coordinate_autofill, right_coordinate_autofill, top3_coordinate_autofill, top4_coordinate_autofill)) {
            content = contentInBar3;
            keep_content = false;
            search_function(content); //do the search function
            highlightBar = 0;
            content = "";
            string_input = {};
            click_on_search = false;
            string_input.resize(0);
            search_click++;
            search_engine();
        }
    } else if (start_search) {
        show_route_planner = false;
        if (highlightBar == 1)//determine which result are selected depending on the value of highlightBar
            content = contentInBar1; //return contentInBar1 to content
        if (highlightBar == 2)
            content = contentInBar2; //return contentInBar2 to content
        if (highlightBar == 3)
            content = contentInBar3; //return contentInBar3 to content
        keep_content = false;
        search_function(content); //do the search function
        highlightBar = 0;
        content = "";
        string_input = {};
        click_on_search = false;
        string_input.resize(0);
        search_click++;
        search_engine();

    } else if (start_routing) {
        //if enables route planner, following actions will be conducted
        click_on_direction = mouse_click_in_rec(x, y, route_right_coordinate, route_right_coordinate2, route_bottom_coordinate, place_text_bar1_top);
        click_on_reverse = mouse_click_in_rec(x, y, route_right_coordinate, route_right_coordinate2, place_text_bar1_top, place_text_bar2_top);
        type_place1 = mouse_click_in_rec(x, y, route_left_coordinate, route_right_coordinate, route_bottom_coordinate, place_text_bar1_top);
        type_place2 = mouse_click_in_rec(x, y, route_left_coordinate, route_right_coordinate, place_text_bar1_top, place_text_bar2_top);
        if (mouse_click_in_rec(x, y, left_coordinate2, left_coordinate, top_coordinate2, bottom_coordinate2))
            routing_click++;
        if (routing_click % 2 != 0) {
            only_show_searchbar = false;
            show_route_planner = true;
            keep_content = false;
            if (string_input.size() > 1) {
                draw_screen();
            } else {
                search_engine();
            }
            content = "";
            string_input = {};
            string_input.resize(0);
            if (showed_auto_fill) {
                showed_auto_fill = false;
                click_on_search = false;
                draw_screen();
            }
            if (click_on_reverse)
                actOnReverseFunction();
            if (click_on_direction) {
                string_input_routeplan = {};
                string_input_routeplan.resize(0);
                string_input_routeplan1 = {};
                string_input_routeplan1.resize(0);
                route_search_parser();
            }
            route_planner_interface();

        } else {
            // Signal change in routing processing
            // All signals go back default value
            show_route_planner = false;
            place1 = "";
            place2 = "";
            string_input_routeplan = {};
            string_input_routeplan1 = {};
            string_input_routeplan.resize(0);
            string_input_routeplan1.resize(0);
            start_routing = false;
            type_place1 = false;
            type_place2 = false;
            showInstruction = false;
            draw_screen();
        }


    }
}

int barSelect(bool up, bool down, int highlightBar) {
    //This function is used to determine the value changed on highlightBar, which would be affected by up and down 
    if (highlightBar == 0) {//no bar is selected when the value of highlightBar is zero
        if (up == true)
            return 3;
        if (down == true)
            return 1;
    }
    if (highlightBar == 1) {
        if (up == true)
            return 0;
        if (down == true)
            return 2;
    }
    if (highlightBar == 2) {
        if (up == true)
            return 1;
        if (down == true)
            return 3;
    }
    if (highlightBar == 3) {
        if (up == true)
            return 2;
        if (down == true)
            return 1;
    }

}

void autoFillSquare(int highlightBar) {
    //This function is used for autoFill function , which shows three most close searching results below the search bar
    // this part is mostly about graphic operation
    t_bound_box autoFillBox = get_visible_world();
    if (only_show_searchbar && content != "" && string_input.size() >= 2) {
        bool highlight1 = false;

        float left_offset = autoFillBox.get_width()*(float) 0.75;
        float left_coordinate = autoFillBox.left() + 0.8 * left_offset;
        float right_offset = autoFillBox.get_width()*(float) 0.1;
        float right_coordinate = autoFillBox.right() - right_offset;

        float top1_offset = autoFillBox.get_height()*(float) 0.93;
        float top1_coordinate = autoFillBox.bottom() + top1_offset;

        float hight_offset = autoFillBox.get_height()*(float) 0.05;
        float top2_coordinate = top1_coordinate - hight_offset;
        float top3_coordinate = top2_coordinate - hight_offset;
        float top4_coordinate = top3_coordinate - hight_offset;
        left_coordinate_autofill = left_coordinate;
        right_coordinate_autofill = right_coordinate;
        top1_coordinate_autofill = top1_coordinate;
        top2_coordinate_autofill = top2_coordinate;
        top3_coordinate_autofill = top3_coordinate;
        top4_coordinate_autofill = top4_coordinate;
        contentInBar1 = globalBase->find_name_from_ids_in_nameBase(closest);
        contentInBar2 = globalBase->find_name_from_ids_in_nameBase(closest2);
        contentInBar3 = globalBase->find_name_from_ids_in_nameBase(closest3);
        t_bound_box autoResult1(left_coordinate, top2_coordinate, right_coordinate, top1_coordinate); //three rec are created here
        t_bound_box autoResult2(left_coordinate, top3_coordinate, right_coordinate, top2_coordinate);
        t_bound_box autoResult3(left_coordinate, top4_coordinate, right_coordinate, top3_coordinate);
        if (highlightBar == 0) {//depending on the value of highlightBar, determine colors of each bar
            setcolor(WHITE);
            fillrect(autoResult1);
            fillrect(autoResult2);
            fillrect(autoResult3);
            setcolor(BLACK);
            setfontsize(15);
            drawtext_in(autoResult1, contentInBar1);
            drawtext_in(autoResult2, contentInBar2);
            drawtext_in(autoResult3, contentInBar3);


        } else if (highlightBar == 1) {
            setcolor(LIGHTGREY);
            fillrect(autoResult1);
            setcolor(WHITE);
            fillrect(autoResult2);
            fillrect(autoResult3);
            setcolor(BLACK);
            setfontsize(15);
            drawtext_in(autoResult1, contentInBar1);
            drawtext_in(autoResult2, contentInBar2);
            drawtext_in(autoResult3, contentInBar3);
        } else if (highlightBar == 2) {
            setcolor(LIGHTGREY);
            fillrect(autoResult2);
            setcolor(WHITE);
            fillrect(autoResult1);
            fillrect(autoResult3);
            setcolor(BLACK);
            setfontsize(15);
            drawtext_in(autoResult1, contentInBar1);
            drawtext_in(autoResult2, contentInBar2);
            drawtext_in(autoResult3, contentInBar3);
        } else if (highlightBar == 3) {
            setcolor(LIGHTGREY);
            fillrect(autoResult3);
            setcolor(WHITE);
            fillrect(autoResult1);
            fillrect(autoResult2);
            setcolor(BLACK);
            setfontsize(15);
            drawtext_in(autoResult1, contentInBar1);
            drawtext_in(autoResult2, contentInBar2);
            drawtext_in(autoResult3, contentInBar3);
        }

        t_point left1(left_coordinate, top1_coordinate);
        t_point left2(left_coordinate, top2_coordinate);
        t_point left3(left_coordinate, top3_coordinate);
        t_point left4(left_coordinate, top4_coordinate);

        t_point right1(right_coordinate, top1_coordinate);
        t_point right2(right_coordinate, top2_coordinate);
        t_point right3(right_coordinate, top3_coordinate);
        t_point right4(right_coordinate, top4_coordinate);

        setcolor(BLACK);
        drawline(left1, right1);
        drawline(left2, right2);
        drawline(left3, right3);
        drawline(left4, right4);
    }

}

void helperSymbol() {

    t_bound_box helper_symbol = get_visible_world();

    float left_offset = helper_symbol.get_width()* 0.92;
    symbol_left = helper_symbol.left() + left_offset;

    float right_offset = helper_symbol.get_width()* 0.03;
    symbol_right = helper_symbol.right() - right_offset;

    float top_offset = helper_symbol.get_height()* 0.92;
    symbol_top = helper_symbol.top() - top_offset;

    symbol_bottom = symbol_top - (symbol_right - symbol_left);

    t_bound_box symbol(symbol_left, symbol_bottom, symbol_right, symbol_top);
    //setcolor(BLACK);
    //fillrect(symbol);
    float x_offset = helper_symbol.get_width()*0.0245;
    float y_offset = helper_symbol.get_width()*0.0247;

    float orignal_x = (symbol_left + symbol_right) / 2;
    float orignal_y = (symbol_top + symbol_bottom) / 2;
    Surface help_symbol("help_icon copy.png");
    draw_surface(help_symbol, orignal_x - x_offset, orignal_y + y_offset);
}

void search_engine() {
    //content add mouse
    string temp_content;
    temp_content = content + "|";

    // all the graphic operation of search engine is put here
    t_bound_box search_box = get_visible_world();
    // inner bound
    float left_offset = search_box.get_width()* 0.75;
    left_coordinate = search_box.left() + 0.8 * left_offset;
    float right_offset = search_box.get_width()* 0.1;
    right_coordinate = search_box.right() - right_offset;
    float top_offset = search_box.get_height()* 0.02;
    top_coordinate = search_box.top() - top_offset;
    float bottom_offset = search_box.get_height()* 0.93;
    bottom_coordinate = search_box.bottom() + bottom_offset;

    //outer border
    float left_offset2 = search_box.get_width()* 0.745;
    left_coordinate2 = search_box.left() + 0.76 * left_offset2;
    float right_offset2 = search_box.get_width()* 0.015;
    right_coordinate2 = search_box.right() - right_offset2;
    float top_offset2 = search_box.get_height()* 0.015;
    top_coordinate2 = search_box.top() - top_offset;
    float bottom_offset2 = search_box.get_height()* 0.925;
    bottom_coordinate2 = search_box.bottom() + bottom_offset;

    if (name_not_found == false) {//show the default text content
        if (only_show_searchbar && keep_content) {
            if (content == "click here to search") {
                temp_content = "|";
            }
            setcolor(BLUE);
        } else if (!only_show_searchbar && content != "" && keep_content) {
            setcolor(DARKGREY);
            temp_content = content;
        } else if (!keep_content) {
            setcolor(DARKGREY);
            content = "";
            string_input = {};
            string_input.resize(0);
            search_click++;
            click_on_search = false;
            showed_auto_fill = false;
            setcolor(DARKGREY);
            content = "click here to search";
            temp_content = content;

        } else {
            setcolor(DARKGREY);
            content = "click here to search";
            temp_content = content;
        }//two rec box
        fillrect(left_coordinate2, bottom_coordinate2, right_coordinate2, top_coordinate2);
    } else {
        string_input = {};
        string_input.resize(0);
        setcolor(RED); //once the content can not be founded in street or point of interest or intersections
        fillrect(left_coordinate2, bottom_coordinate2, right_coordinate2, top_coordinate2);
        content = "Name not found";
        temp_content = content;

    }
    setcolor(WHITE); // All the draw setting about search bar
    fillrect(left_coordinate, bottom_coordinate, right_coordinate, top_coordinate);
    t_bound_box search_button(right_coordinate, bottom_coordinate, right_coordinate2, top_coordinate);
    t_bound_box input_center(left_coordinate, bottom_coordinate, right_coordinate, top_coordinate);
    setcolor(LIGHTGREY);
    setfontsize(15);
    settextrotation(0);
    drawtext_in(input_center, temp_content);
    setcolor(WHITE);
    setfontsize(12);
    settextrotation(0);
    drawtext_in(search_button, "SEARCH");
    if (!show_route_planner) {
        draw_route_planner_label();
    } else {
        draw_route_planner_label_when_hit();
    }

}

void act_on_keypress(char c, int keysym) {
    //Processing all the keyboard activities
    if (only_show_searchbar) {
        // When search bar is activated
        name_not_found = false;
        string s(1, c);
        string_input.push_back(s);
        bool show_auto_fill = string_input.size() >= 2;
        if (content != "" && string_input.size() >= 2 && keysym == 65288) {
            //if the pressed key is backspace
            string_input.pop_back();
            string_input.pop_back(); //pop back backspace and the last input in string input
            if (string_input.size() <= 1) {
                draw_screen();
            }
        }
        if (keysym == 65293) {
            //if receive key return, auto fill the suggestion below and do the search function
            string_input.pop_back(); //first pop back the return
            if (highlightBar == 1)//determine which result are selected depending on the value of highlightBar
                content = contentInBar1; //return contentInBar1 to content
            if (highlightBar == 2)
                content = contentInBar2; //return contentInBar2 to content
            if (highlightBar == 3)
                content = contentInBar3; //return contentInBar3 to content
            keep_content = false;
            search_function(content); //do the search function
            highlightBar = 0;
            content = "";
            string_input = {};
        }
        if (keysym == 65362 && show_auto_fill) {//receive up
            string_input.pop_back(); //pop back the up
            up_pressed = true;
            highlightBar = barSelect(up_pressed, down_pressed, highlightBar);
            autoFillSquare(highlightBar);
            up_pressed = false; // back to false
        }
        if (keysym == 65364 && show_auto_fill) {//receive down
            string_input.pop_back(); //pop back the down
            down_pressed = true;
            highlightBar = barSelect(up_pressed, down_pressed, highlightBar);
            autoFillSquare(highlightBar);
            down_pressed = false; //back to false
        }
        if (keysym == 65509 || keysym == 65507 || keysym == 65508 || keysym == 65505 || keysym == 65506)
            string_input.pop_back();
        for (unsigned int i = 0; i < string_input.size(); i++) {//get value from string_input and put it in content
            if (i == 0) {
                content = "";
            } else {
                content += string_input[i];
                auto iter = content.find("And");
                if (iter == string::npos) {
                    search_closest();
                }

            }
        }
        if (string_input.size() >= 2) {
            autoFillSquare(highlightBar);
        }
        content[0] = toupper(content[0]); // automatically make the first letter or the one following space
        for (unsigned int j = 0; j < content.size(); j++) {//make the first letter of each word capital 
            if (content[j] == ' ') {
                content[j + 1] = toupper(content[j + 1]);
            }
        }

        search_engine(); //do the search engine



    } else if (show_route_planner) {
        //When route planner is activated
        string s(1, c);
        if (type_place1&&!type_place2) {
            string_input_routeplan.push_back(s);
            if (place1 != "" && string_input_routeplan.size() >= 2 && keysym == 65288) {
                //if the pressed key is backspace
                string_input_routeplan.pop_back();
                string_input_routeplan.pop_back(); //pop back backspace and the last input in string input
            }
            if (keysym == 65364 || keysym == 65289) {
                //if receive key return or down or tab, jump to the next place
                string_input_routeplan.pop_back(); //first pop back the return
                type_place1 = false;
                type_place2 = true;
            }
            //if recieve key return search path
            if (keysym == 65293) {
                string_input_routeplan.pop_back();
                if (place1 != "" && place2 != "") {
                    string_input_routeplan = {};
                    string_input_routeplan.resize(0);
                    string_input_routeplan1 = {};
                    string_input_routeplan1.resize(0);
                    type_place1 = false;
                    type_place2 = false;
                    route_search_parser();
                }
            }
            /* the following key should be ignored*/
            if (keysym == 65509 || keysym == 65507 || keysym == 65508 || keysym == 65505 || keysym == 65506 || keysym == 65362)
                string_input_routeplan.pop_back();

            for (unsigned int i = 0; i < string_input_routeplan.size(); i++) {//get value from string_input and put it in content
                if (i == 0) {
                    place1 = "";
                } else {
                    place1 += string_input_routeplan[i];

                }
            }


            place1[0] = toupper(place1[0]); // automatically make the first letter or the one following space
            for (unsigned int j = 0; j < place1.size(); j++) {//make the first letter of each word capital 
                if (place1[j] == ' ') {
                    place1[j + 1] = toupper(place1[j + 1]);
                }
            }
            route_planner_interface(); //do the search engine
        } else if (type_place2&&!type_place1) {
            string_input_routeplan1.push_back(s);
            if (place2 != "" && string_input_routeplan1.size() >= 2 && keysym == 65288) {
                //if the pressed key is backspace
                string_input_routeplan1.pop_back();
                string_input_routeplan1.pop_back(); //pop back backspace and the last input in string input
            }
            if (keysym == 65362 || keysym == 65289) {
                //if receive key up or tab, jump to the next place
                string_input_routeplan1.pop_back(); //first pop back the return
                type_place1 = true;
                type_place2 = false;
            }
            //if recieve key return search path
            if (keysym == 65293) {
                string_input_routeplan1.pop_back();
                if (place1 != "" && place2 != "") {
                    string_input_routeplan = {};
                    string_input_routeplan.resize(0);
                    string_input_routeplan1 = {};
                    string_input_routeplan1.resize(0);
                    type_place1 = false;
                    type_place2 = false;
                    route_search_parser();
                }
            }

            /* the following key should be ignored*/
            if (keysym == 65509 || keysym == 65507 || keysym == 65508 || keysym == 65505 || keysym == 65506 || keysym == 65364)
                string_input_routeplan1.pop_back();

            for (unsigned int i = 0; i < string_input_routeplan1.size(); i++) {//get value from string_input and put it in content
                if (i == 0) {
                    place2 = "";
                } else {
                    place2 += string_input_routeplan1[i];
                }
            }

            place2[0] = toupper(place2[0]); // automatically make the first letter or the one following space
            for (unsigned int j = 0; j < place2.size(); j++) {//make the first letter of each word capital 
                if (place2[j] == ' ') {
                    place2[j + 1] = toupper(place2[j + 1]);
                }
            }


            route_planner_interface();

        } else if (showInstruction == true) {
            //receive key return
            if (keysym == 65293 && distance_strings[instruction_pointing] == " ") {
                navigation_highlight_street = {};
                navigation_highlight_inters = {};
                unsigned inter_id = intersectionids_in_route_planner[instruction_pointing];
                t_point last_inter_pos = globalBase->get_t_point_intersection_pos(inter_id);
                auto_zoom_in2(last_inter_pos);
                navigation_highlight_inters.push_back(inter_id);
                draw_screen();
            } else if (keysym == 65293 && distance_strings[instruction_pointing] != " ") {
                navigation_highlight_street = {};
                navigation_highlight_inters = {};
                unsigned inter1 = intersectionids_in_route_planner[instruction_pointing];
                unsigned inter2;
                navigation_highlight_street = direction_data.street_segments_ids[instruction_pointing + directionValue];
                int direction_size = direction_data.instruction.size();
                if (instruction_pointing < direction_size - directionValue - 2 && instruction_pointing < direction_size) {
                    inter2 = intersectionids_in_route_planner[instruction_pointing + 1];
                    if (instruction_pointing == 0 && directionValue == 0) {
                        vector<unsigned> temp_ids = direction_data.intersection_id;
                        t_point starting_point = globalBase->get_t_point_intersection_pos(temp_ids[0]);
                        auto_zoom_in2(starting_point);
                        draw_screen();
                    } else {
                        t_point mid_point_pos = globalBase->find_midpoint_of_two_inters(inter1, inter2);
                        auto_zoom_in2(mid_point_pos);
                        draw_screen();
                    }

                } else if (instruction_pointing == direction_size - directionValue - 1) {
                    vector<unsigned> temp_ids = direction_data.intersection_id;
                    t_point destination = globalBase->get_t_point_intersection_pos(temp_ids[temp_ids.size() - 1]);
                    auto_zoom_in2(destination);
                    draw_screen();
                }
            } else if (keysym == 65362) {//receive up
                if (directionValue > 0 && instruction_pointing == 0) {
                    directionValue--;
                }
                if (instruction_pointing == 0) {
                    instruction_pointing = 0;
                } else if (instruction_pointing > 0) {
                    instruction_pointing--;
                }
                detailedInstruction_interface();
            } else if (keysym == 65364) {//receive down
                int instructionSize = direction_data.instruction.size();
                if (directionValue + 5 < instructionSize) {
                    directionValue++;
                    instruction_pointing = 0;
                } else if (instruction_pointing < instructionSize - directionValue - 1) {
                    instruction_pointing++;
                }
                detailedInstruction_interface();
            }

        }

    }
}

bool mouse_click_in_rec(float x, float y,
        float left, float right, float upper, float bottom) {
    // determine whether the mouse is inside a rec
    // that is given the position
    if (x <= right && x >= left && y <= upper && y >= bottom) {

        return true;
    } else
        return false;
}

void findFunction(void (*drawscreen_ptr)(void)) {
    // function of find button
    string streetName1, streetName2;
    string POI_name;
    cout << "Please select from following:" << endl; // three modes for choose
    cout << "1 -- Find Intersection of Two Streets" << endl;
    cout << "2 -- Find Point of Interest" << endl;
    cout << "3 -- Find a Feature Location" << endl;
    cout << "Enter the number of your selection: ";
    int selection;
    cin>>selection;
    cin.ignore();
    if (selection == 1) {//find intersection of two street
        cout << "Start to find intersection of two streets." << endl;
        cout << "Enter the name of the first street:" << endl;
        getline(cin, streetName1);
        bool street1Existing = globalBase->check_street_existing(streetName1);
        while (!street1Existing) {//check the validity of street 1, if false, asking for a new one
            cout << "Street name " << streetName1 << " does not exist" << endl;
            cout << "Enter the name of the first street: ";
            getline(cin, streetName1);
            street1Existing = globalBase->check_street_existing(streetName1);
        }
        cout << "Enter the name of the second street:" << endl;
        getline(cin, streetName2);
        bool street2Existing = globalBase->check_street_existing(streetName2);
        while (!street2Existing) {// check the validity of street 2, if false, asking for a new one
            cout << "Street name " << streetName2 << " does not exist" << endl;
            cout << "Enter the name of the second street: ";
            getline(cin, streetName2);
            street2Existing = globalBase->check_street_existing(streetName2);
        }
        // street 1 and street 2 both exist at this point
        std::vector<unsigned> intersectionBetweenTwoStreet = globalBase->find_intersection_ids_from_street_names_in_DB(streetName1, streetName2);
        if (intersectionBetweenTwoStreet.size() == 0)
            cout << streetName1 << " and " << streetName2 << " have no intersections" << endl;
        else {
            for (unsigned i = 0; i < intersectionBetweenTwoStreet.size(); i++) {
                unsigned interId = intersectionBetweenTwoStreet.at(i);
                highlight_inters.push_back(interId);
                auto_zoom_in(globalBase -> get_t_point_intersection_pos(interId));
                draw_screen();
            }
        }
    } else if (selection == 2) {//find point of interest
        cout << "Start to find point of interest" << endl;
        cout << "Enter the name of the POI:" << endl;
        getline(cin, POI_name);
        while (!globalBase->check_POI_existing(POI_name)) {
            cout << "POI name " << POI_name << " does not exist " << endl;
            cout << "Enter the name of the POI:";
            getline(cin, POI_name);
        }
        unsigned poi_id = globalBase->get_POI_id_from_name(POI_name);
        highlight_pois.push_back(poi_id);
        auto_zoom_in(globalBase -> get_POI_id_to_t_point(poi_id));
        draw_screen();
    } else if (selection == 3) {//find a feature location
        cout << "Start to find feature location" << endl;
        cout << "Enter the name of the POI:";
    }

}

//this function is to check whether there's double click for POI or intersection 

void processDuplicate(vector<unsigned>& temp, unsigned id) {
    //avoid that two same result 
    auto iter = find(temp.begin(), temp.end(), id);
    if (iter != temp.end()) {
        temp.erase(iter);
    } else {
        temp.push_back(id);
    }
}

void processForceAdd(vector<unsigned>& temp, unsigned id) {
    //avoid that two same result 
    auto iter = find(temp.begin(), temp.end(), id);
    if (iter != temp.end()) {
        return;
    } else {
        temp.push_back(id);
    }
}

void act_on_cycleway_func(void (*drawscreen_ptr) (void)) {
    // show all the cycle path in map
    cycle_button_click++;
    if (cycle_button_click % 2 != 0) {
        draw_cycle = true;
    } else {
        draw_cycle = false;
    }
    draw_screen();
}

void act_on_clear_all(void (*drawscreen_ptr) (void)) {
    // Clear all highlighted database, make them void
    highlight_inters = {};
    highlight_pois = {};
    highlight_fea = {};
    highlight_str = {};
    highlight_path = {};
    navigation_highlight_inters = {};
    navigation_highlight_street = {};
    show_route_planner = false;
    showInstruction = false;
    draw_screen();
}

void act_on_find_path(void (*drawscreen_ptr) (void)) {
    // Processing when click find path
    find_path_click++;
    int option;
    cout << "Please choose from one of the following options:" << endl;
    cout << "  1. find shortest distance between intersection and POI" << endl;
    cout << "  2. find shortest distance between intersection and intersection" << endl;
    //Show the instruction for this function
    cin >> option;
    if (option == 1) {
        // Shortest distance between intersection and POI
        unsigned start;
        cout << "insert the intersection id:" << endl;
        //only accept the intersection id
        cin >> find_path_between_inter_POI_inter;
        cin.ignore();
        cout << "now the POI name:" << endl;
        getline(cin, find_path_between_inter_POI_POI);
        double penalty = 1;
        seg_list = find_path_to_point_of_interest(find_path_between_inter_POI_inter, find_path_between_inter_POI_POI, penalty);
        unsigned POI_id = get_the_best_POI_id(find_path_between_inter_POI_inter, find_path_between_inter_POI_POI, penalty);
        if (!seg_list.empty()) {
            highlight_path = {};
            highlight_path = seg_list;
            processForceAdd(highlight_inters, find_path_between_inter_POI_inter);
            processForceAdd(highlight_pois, POI_id);
            t_point midpoint = globalBase -> get_t_point_intersection_pos(find_path_between_inter_POI_inter);
            auto_zoom_in(midpoint);
            draw_screen();
        } else {
            cout << "wrong!!" << endl;
        }
    } else if (option == 2) {
        // Shortest distance between intersection and intersection
        unsigned start, end;
        cout << "insert first intersection id:" << endl;
        cin >> find_path_inter1;
        cin.ignore();
        cout << "now second:" << endl;
        cin >> find_path_inter2;
        cin.ignore();

        double penalty = 1;
        seg_list = find_path_between_intersections(find_path_inter1, find_path_inter2, penalty);
        if (!seg_list.empty()) {
            highlight_path = {};
            highlight_path = seg_list;
            processForceAdd(highlight_inters, find_path_inter1);
            processForceAdd(highlight_inters, find_path_inter2);
            t_point midpoint = globalBase -> get_t_point_intersection_pos(find_path_inter1);
            auto_zoom_in(midpoint);
            draw_screen();
        } else {
            cout << "wrong!!" << endl;
            //Once the search information is wrong
        }

    } else {
        cout << "Function not yet implemented!" << endl;
    }
}

void search_function(string content) {//this function only deals with street and point of interest
    auto iter = content.find("And"); //find the And in input
    auto iter2 = content.find("&");
    string lower_case_content;
    for (unsigned i = 0; i < content.size(); i++) {
        lower_case_content.push_back(tolower(content[i]));
    }
    if (iter == string::npos && iter2 == string::npos) {
        // the input has no And or & symbol
        bool existingInStreet = globalBase->check_street_existing(content);
        bool existingInPOI = globalBase->check_POI_existing(content);
        bool existingInFeature = globalBase->check_feature_existing(content);
        if (existingInStreet) {//if the result exists in street
            name_not_found = false;
            if (highlight_str.size() != 0) {
                for (unsigned i = 0; i < highlight_str.size(); i++)
                    highlight_str.pop_back();
            }
            string a = content;
            vector<unsigned> streetID = globalBase->find_street_ids_from_name_in_DB(content);
            street_auto_zoom_in(streetID[0]);
            for (unsigned i = 0; i < streetID.size(); i++) {
                auto iter2 = find(highlight_str.begin(), highlight_str.end(), streetID[i]);
                if (iter2 != highlight_str.end()) {
                    highlight_str = {};
                    break;
                } else {
                    highlight_str.push_back(streetID[i]);
                }
            }
            cout << globalBase->find_street_name_from_ids_in_DB(streetID[0]) << endl;
            content = "";
            draw_screen();

        } else if (existingInPOI) {//if it exists in POI
            name_not_found = false;
            vector<unsigned> poi_ids = globalBase->get_all_POI_ids_according_to_name(content);
            for (unsigned i = 0; i < poi_ids.size(); i++) {
                processDuplicate(highlight_pois, poi_ids[i]);
            }
            if (poi_ids.size() == 1) {
                auto_zoom_in(globalBase -> get_POI_id_to_t_point(poi_ids[0]));
            } else {
                zoom_fit_helper();
            }

            draw_screen();
        } else if (existingInFeature) {//if it exists in features
            name_not_found = false;
            unsigned feature_id = globalBase -> get_feature_id_from_name(content);
            processDuplicate(highlight_fea, feature_id);
            content = "";
            auto_zoom_in(globalBase -> get_feature_average_pos(feature_id));
            draw_screen();
        } else if (lower_case_content == "toronto map" || lower_case_content == "cairo map" || lower_case_content == "beijing map" || lower_case_content == "tehran map" || lower_case_content == "iceland map"
                || lower_case_content == "rio-de-janeiro map" || lower_case_content == "golden-horseshoe map" || lower_case_content == "new-dehli map" || lower_case_content == "new-york map"
                || lower_case_content == "tokyo map" || lower_case_content == "london map") {
            auto it = lower_case_content.find(" map");
            lower_case_content.erase(it);
            close_map();
            map_select(lower_case_content);
            highlight_inters = {};
            highlight_pois = {};
            highlight_fea = {};
            highlight_str = {};
            highlight_path = {};
            place1 = "";
            place2 = "";
            bool mouse_inside_search_box = false;
            content = "click here to search";
            type_place1 = false;
            type_place2 = false;
            draw_cycle = false;
            //-------------------------these are all click signals-------------------//
            routing_click = 0;
            cycle_button_click = 0;
            return_click = 0;
            search_click = 0;
            find_path_click = 0;
            draw_poi = false;
            name_not_found = false;
            click_on_search = false;
            start_search = false;
            start_routing = false;
            ;
            up_pressed = false;
            keep_content = true;
            highlightBar = 0;
            down_pressed = false;
            showed_auto_fill = false;
            click_on_reverse = false;
            click_on_direction = false;
            show_route_planner = false;
            only_show_searchbar = false;
            showInstruction = false;

            directionValue = 0;

            //------------------------------------------------------------------------//

            is_intersection = false;

            string_input = {};
            //string input into route plan S bar
            string_input_routeplan = {};
            //string input into route plan R bar
            string_input_routeplan1 = {};
            destroy_button("Cycle Paths");
            destroy_button("POI");
            destroy_button("Clear All");
            destroy_button("Change Map");
            draw_map();

        } else {//does not exist at all
            content = contentInBar1;
            search_function(content);
        }

    } else if (iter != string::npos && iter2 == string::npos) {// if it is intended to highlight the intersection of two street
        string temp_content = content;
        temp_content.erase(iter);
        string streetname1 = temp_content;
        streetname1.pop_back();
        temp_content = content;
        string reversed(temp_content.rbegin(), temp_content.rend());
        auto iter2 = reversed.find("dnA");
        temp_content = reversed.erase(iter2);
        if (temp_content != "") {
            temp_content.pop_back();
            string streetname2(temp_content.rbegin(), temp_content.rend());
            if (globalBase->check_street_existing(streetname1) && globalBase->check_street_existing(streetname2)) {
                name_not_found = false;
                std::vector<unsigned> intersectionBetweenTwoStreet = globalBase->find_intersection_ids_from_street_names_in_DB(streetname1, streetname2);
                if (intersectionBetweenTwoStreet.size() == 0) {//if the intersection is not found
                    name_not_found = true;
                    search_engine();
                } else {
                    for (unsigned i = 0; i < intersectionBetweenTwoStreet.size(); i++) {
                        unsigned interId = intersectionBetweenTwoStreet.at(i);
                        processDuplicate(highlight_inters, interId);
                        content = "";
                        auto_zoom_in(globalBase -> get_t_point_intersection_pos(interId));
                        draw_screen();
                    }
                }
            } else {
                streetname1 = fuzzy_search_streets(streetname1);
                streetname2 = fuzzy_search_streets(streetname2);
                std::vector<unsigned> intersectionBetweenTwoStreet = globalBase->find_intersection_ids_from_street_names_in_DB(streetname1, streetname2);
                if (intersectionBetweenTwoStreet.size() == 0) {//if the intersection is not found
                    name_not_found = true;
                    search_engine();
                } else {
                    for (unsigned i = 0; i < intersectionBetweenTwoStreet.size(); i++) {
                        unsigned interId = intersectionBetweenTwoStreet.at(i);
                        processDuplicate(highlight_inters, interId);
                        content = "";
                        auto_zoom_in(globalBase -> get_t_point_intersection_pos(interId));
                        draw_screen();
                    }
                }
            }
        } else {
            name_not_found = true;
        }

    } else if (iter == string::npos && iter2 != string::npos) { //if the name contains "&", means search for intersection
        string temp_content = content;
        temp_content.erase(iter2);
        string streetname1 = temp_content;
        streetname1.pop_back();
        temp_content = content;
        string reversed(temp_content.rbegin(), temp_content.rend());
        auto iter3 = reversed.find("&");
        temp_content = reversed.erase(iter3);
        if (temp_content != "") {
            temp_content.pop_back();
            string streetname2(temp_content.rbegin(), temp_content.rend());

            if (globalBase->check_street_existing(streetname1) && globalBase->check_street_existing(streetname2)) {
                name_not_found = false;
                std::vector<unsigned> intersectionBetweenTwoStreet = globalBase->find_intersection_ids_from_street_names_in_DB(streetname1, streetname2);
                if (intersectionBetweenTwoStreet.size() == 0) {//if the intersection is not found
                    name_not_found = true;
                    search_engine();
                } else {
                    for (unsigned i = 0; i < intersectionBetweenTwoStreet.size(); i++) {
                        unsigned interId = intersectionBetweenTwoStreet.at(i);
                        processDuplicate(highlight_inters, interId);
                        content = "";
                        auto_zoom_in(globalBase -> get_t_point_intersection_pos(interId));
                        draw_screen();
                    }
                }
            } else {
                streetname1 = fuzzy_search_streets(streetname1);
                streetname2 = fuzzy_search_streets(streetname2);
                std::vector<unsigned> intersectionBetweenTwoStreet = globalBase->find_intersection_ids_from_street_names_in_DB(streetname1, streetname2);
                if (intersectionBetweenTwoStreet.size() == 0) {//if the intersection is not found
                    name_not_found = true;
                    search_engine();
                } else {
                    for (unsigned i = 0; i < intersectionBetweenTwoStreet.size(); i++) {
                        unsigned interId = intersectionBetweenTwoStreet.at(i);
                        processDuplicate(highlight_inters, interId);
                        content = "";
                        auto_zoom_in(globalBase -> get_t_point_intersection_pos(interId));
                        draw_screen();
                    }
                }
            }
        } else {
            name_not_found = true;
        }

    } else {
        name_not_found = true;
    }
}

void act_on_poi_func(void (*drawscreen_ptr) (void)) {
    //once the poi button is clicked
    poi_button_click++;
    if (poi_button_click % 2 != 0) {
        draw_poi = true;
    } else {
        draw_poi = false;
    }
    draw_screen();
}

bool in_screen(t_point point) {
    //check if the mouse in screen
    if (point.x < screen_right && point.x > screen_left && point.y < screen_top && point.y > screen_bottom) {
        return true;
    }
    return false;
}

void search_closest() {
    vector<int> content_distance;
    vector<string> content_keyword;
    //    vector<vector<string>>street_name_lists=globalBase->get_street_name_list();
    /*   This part will be used for future function, which can not be finished by the deadline of m2. They are not old codes    */
    int distance;
    auto it = content.find("And");
    bool generate_auto = (it == string::npos);
    if (generate_auto) {
        stringstream temp_ss(content);
        string temp_content;
        temp_ss>>temp_content;
        while (!temp_ss.fail()) {
            content_keyword.push_back(temp_content);
            temp_ss>>temp_content;
        }
        //only find the closest result to first word
        //structure should be loop through every keyword in street name data base and add distance togather
        for (unsigned i = 0; i < globalBase->get_nameNumbers(); i++) {
            if (globalBase->find_name_from_ids_in_nameBase(i) == "<unknown>" || toupper(content.at(0)) != globalBase->find_name_from_ids_in_nameBase(i).at(0)) {
                distance = 1000;
                content_distance.push_back(distance);
            } else {
                distance = 0;
                int individual_distance = 0;
                vector<string> name_keyword = name_database[i];
                for (unsigned j = 0; j < content_keyword.size(); j++) {
                    individual_distance = levenshtein_distance(content_keyword[j], name_keyword[0]);
                    for (unsigned k = 0; k < name_keyword.size(); k++) {
                        int temp_distance = levenshtein_distance(content_keyword[j], name_keyword[k]);
                        //find the word with closest distance
                        if (temp_distance < individual_distance) {
                            individual_distance = temp_distance;
                        }
                    }
                    distance += individual_distance;
                }
                content_distance.push_back(distance);
            }
        }
        //distance has been all stored, now check the three minimum
        vector<int> temp = content_distance;
        closest = 0;
        int minimum_distance = content_distance[0];
        for (unsigned i = 0; i < globalBase->get_nameNumbers(); i++) {
            if (content_distance[i] < minimum_distance && i != 70 && i != 71 && i != 129 && i != 489 && i != 67 && i != 50 && i != 30006 && i != 30856) {
                minimum_distance = content_distance[i];
                closest = i;

            }


        }
        //to get the second closest result
        for (unsigned i = 0; i < globalBase->get_nameNumbers(); i++) {
            if (content_distance[i] - minimum_distance <= 1 && i != closest && i != 70 && i != 67 && i != 489 && i != 50 && i != 30006 && i != 30856
                    && i != 129 && i != 71 && globalBase->find_name_from_ids_in_nameBase(closest) != globalBase->find_name_from_ids_in_nameBase(i)
                    && globalBase->find_name_from_ids_in_nameBase(i) != "") {
                closest2 = i;
                minimum_distance = content_distance[i];
                break;
            }
        }
        //to get the third closest result
        for (unsigned i = 0; i < globalBase->get_nameNumbers(); i++) {
            if (content_distance[i] - minimum_distance <= 1 && i != closest2 && i != 70 && i != 489 && i != 50 && i != 30006 && i != 30856
                    && i != 67 && i != 129 && i != 71 && i != closest && globalBase->find_name_from_ids_in_nameBase(i) != globalBase->find_name_from_ids_in_nameBase
                    (closest2) && globalBase->find_name_from_ids_in_nameBase(i) != globalBase->find_name_from_ids_in_nameBase(closest)
                    && globalBase->find_name_from_ids_in_nameBase(i) != "") {
                closest3 = i;
                break;
            }
        }

    }

}

//string distance comparison function

int levenshtein_distance(string s, string t) {
    int n = s.length();
    int m = t.length();
    int d[n + 1][m + 1];
    if (n == 0) {
        return m;
    }
    if (m == 0) {
        return n;
    }
    for (int i = 0; i <= n; i++) {
        d[i][0] = i;
    }
    for (int j = 0; j <= m; j++) {
        d[0][j] = j;
    }
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            int cost = (t[j - 1] == s[i - 1]) ? 0 : 1;
            d[i][j] = min(min(d[i - 1][j] + 1, d[i][j - 1] + 1), d[i - 1][j - 1] + cost);
        }
    }
    return d[n][m];
}

//action when pressing change_map button

void act_on_change_map(void (*drawscreen_ptr) (void)) {
    close_map();
    highlight_inters = {};
    highlight_pois = {};
    highlight_fea = {};
    highlight_str = {};
    highlight_path = {};
    input_map2();
    place1 = "";
    place2 = "";
    bool mouse_inside_search_box = false;
    content = "click here to search";
    type_place1 = false;
    type_place2 = false;
    draw_cycle = false;
    //-------------------------these are all click signals-------------------//
    routing_click = 0;
    cycle_button_click = 0;
    return_click = 0;
    search_click = 0;
    find_path_click = 0;
    draw_poi = false;
    name_not_found = false;
    click_on_search = false;
    start_search = false;
    start_routing = false;
    ;
    up_pressed = false;
    keep_content = true;
    highlightBar = 0;
    down_pressed = false;
    showed_auto_fill = false;
    click_on_reverse = false;
    click_on_direction = false;
    show_route_planner = false;
    only_show_searchbar = false;
    showInstruction = false;

    directionValue = 0;

    //------------------------------------------------------------------------//

    is_intersection = false;

    string_input = {};
    //string input into route plan S bar
    string_input_routeplan = {};
    //string input into route plan R bar
    string_input_routeplan1 = {};
    destroy_button("Cycle Paths");
    destroy_button("POI");
    destroy_button("Clear All");
    destroy_button("Change Map");
    draw_map();

}

//select a map after first drawing

void act_on_path_between_inters(void (*drawscreen_ptr) (void)) {
    unsigned size = highlight_inters.size();
    if (size == 0 || size == 1) {
        update_message("System cannot find two valid intersections.");
    } else {
        size = highlight_inters.size();
        unsigned id_place1 = highlight_inters[size - 2];
        unsigned id_place2 = highlight_inters[size - 1];
        double penalty = 1;

        seg_list = find_path_between_intersections(id_place1, id_place2, penalty);
        if (!seg_list.empty()) {
            direction_data = direction_process(seg_list);
            travel_time = compute_path_travel_time(seg_list, penalty);
            if (travel_time >= 60) {
                cout << "total travel  time is: " << travel_time_minutes(travel_time) << " minutes" << endl;
            } else if (travel_time >= 3600) {
                cout << "total travel  time is: " << travel_time_hours(travel_time) << " hours" << endl;
            } else {
                cout << "total travel  time is: " << int(travel_time) << " seconds" << endl;
            }
            cout << endl;
            highlight_path = {};
            highlight_path = {};
            highlight_inters = {};
            highlight_pois = {};
            highlight_path = seg_list;
            processForceAdd(highlight_inters, id_place1);
            processForceAdd(highlight_inters, id_place2);
            t_point midpoint = globalBase -> find_midpoint_of_two_inters(id_place1, id_place2);
            t_point start = globalBase -> get_t_point_intersection_pos(id_place1);
            t_point end = globalBase -> get_t_point_intersection_pos(id_place2);
            auto_zoom_in_smart(start, end, midpoint);
            draw_screen();
            show_route_planner = true;
            route_planner_interface();
            showInstruction = true;
            instruction_pointing = 0;
            directionValue = 0;
            detailedInstruction_interface();
        } else {
            place1 = "WRONG";
            place2 = "WRONG";
            showInstruction = false;
        }
    }

}

void map_select(string selection) {
    //Provide map selection on the first step when launching the program 
    string map_name;
    string map_path;
    stringstream ss(selection);
    string temp;
    ss>>temp;
    if (temp == "1" || temp == "toronto") {
        map_name = "toronto_canada";
    } else if (temp == "2" || temp == "cairo") {
        map_name = "cairo_egypt";
    } else if (temp == "3" || temp == "beijing") {
        map_name = "beijing_china";
    } else if (temp == "4" || temp == "tehran") {
        map_name = "tehran_iran";

    } else if (temp == "5" || temp == "iceland") {
        map_name = "iceland";
    } else if (temp == "6" || temp == "rio-de-janeiro") {
        map_name = "rio-de-janeiro_brazil";
    } else if (temp == "7" || temp == "golden-horseshoe") {
        map_name = "golden-horseshoe_canada";
    } else if (temp == "8" || temp == "new-dehli") {
        map_name = "new-delhi_india";
    } else if (temp == "9" || temp == "new-york") {
        map_name = "new-york_usa";
    } else if (temp == "10" || temp == "tokyo") {
        map_name = "tokyo_japan";
    } else if (temp == "11" || temp == "london") {
        map_name = "london_england";
    }
    map_path = "/cad2/ece297s/public/maps/" + map_name + ".streets.bin";
    bool load_success = load_map(map_path);
    if (!load_success) {
        std::cerr << "Failed to load map '" << map_path << "'\n";
        input_map2();
    }
    std::cout << "Successfully loaded map '" << map_path << "'\n";
}

//input map after first drawing in the terminal 

void input_map2() {
    //show the map selection instruction
    bool load_success = true;
    string map_path;
    string selection;
    string map_name;
    cout << "List of Maps: " << endl;
    cout << "1 toronto_canada" << endl << "2 cairo_egypt" << endl << "3 beijing china" << endl << "4 tehran_iran" << endl <<
            "5 iceland" << endl << "6 rio-de-janeiro_brazil" << endl << "7 golden-horseshoe_canada" << endl <<
            "8 new-delhi_india" << endl << "9 new-york_usa" << endl << "10 tokyo_japan" << endl << "11 london_england" << endl;
    cout << "Please Enter the Number of Map: ";
    getline(cin, selection);
    map_select(selection);
}

void detailedInstruction_interface() {

    int i;
    int instruction_size = direction_data.instruction.size();
    // Most graphic operation on instruction demonstrating
    // and instruction content processing
    t_bound_box iBox = get_visible_world();
    float bottom_offset = iBox.get_height()*0.4;
    float left_offset = iBox.get_width()*0.07;
    float distance_offset = iBox.get_height()*0.03;

    instruction_left = (route_left_coordinate + route_right_coordinate) / 2;
    instruction_right = route_right_coordinate2;
    instruction_top = place_text_bar2_top;
    instruction_bottom = iBox.bottom() + bottom_offset;
    instruction_height_offset = (instruction_top - instruction_bottom) / 11;

    instruction_hint = instruction_top - instruction_height_offset;
    instruction1_dvder = instruction_hint - instruction_height_offset * 2;
    instruction2_dvder = instruction1_dvder - instruction_height_offset * 2;
    instruction3_dvder = instruction2_dvder - instruction_height_offset * 2;
    instruction4_dvder = instruction3_dvder - instruction_height_offset * 2;

    t_bound_box instructionBox(instruction_left - left_offset, instruction_bottom, instruction_right, instruction_top);
    t_bound_box instructionBox1(instruction_left - left_offset, instruction1_dvder, instruction_right, instruction_top);
    t_bound_box instructionBox2(instruction_left - left_offset, instruction2_dvder, instruction_right, instruction_top);
    t_bound_box instructionBox3(instruction_left - left_offset, instruction3_dvder, instruction_right, instruction_top);
    t_bound_box instructionBox4(instruction_left - left_offset, instruction4_dvder, instruction_right, instruction_top);
    setcolor(t_color(255, 255, 255, 500));

    if (instruction_size >= 5) {
        fillrect(instructionBox);
    } else if (instruction_size < 5) {
        if (instruction_size == 1) {
            fillrect(instructionBox1);
        } else if (instruction_size == 2) {
            fillrect(instructionBox2);
        } else if (instruction_size == 3) {
            fillrect(instructionBox3);
        } else if (instruction_size == 4) {
            fillrect(instructionBox4);
        }
    }
    t_bound_box title(instruction_left - left_offset, instruction_hint, instruction_right, instruction_top);
    setcolor(BLUE);
    fillrect(title);
    string instructionWord = "Please follow the instruction below";
    setfontsize(10);
    setcolor(WHITE);
    drawtext_in(title, instructionWord);

    /*the following point may be used for further purpose*/
    /*t_point lt(instruction_left, instruction_top);
    t_point lb(instruction_left, instruction_bottom);
    t_point rt(instruction_right, instruction_top);
    t_point rb(instruction_right, instruction_bottom);
     */

    t_point dvd1l(instruction_left - left_offset, instruction1_dvder);
    t_point dvd1r(instruction_right, instruction1_dvder);
    t_bound_box bar1(instruction_left - left_offset * 1.1, instruction1_dvder, instruction_right + left_offset * 0.1, instruction_hint);

    t_point dvd2l(instruction_left - left_offset, instruction2_dvder);
    t_point dvd2r(instruction_right, instruction2_dvder);
    t_bound_box bar2(instruction_left - left_offset * 1.1, instruction2_dvder, instruction_right + left_offset * 0.1, instruction1_dvder);

    t_point dvd3l(instruction_left - left_offset, instruction3_dvder);
    t_point dvd3r(instruction_right, instruction3_dvder);
    t_bound_box bar3(instruction_left - left_offset * 1.1, instruction3_dvder, instruction_right + left_offset * 0.1, instruction2_dvder);

    t_point dvd4l(instruction_left - left_offset, instruction4_dvder);
    t_point dvd4r(instruction_right, instruction4_dvder);
    t_bound_box bar4(instruction_left - left_offset * 1.1, instruction4_dvder, instruction_right + left_offset * 0.1, instruction3_dvder);
    t_bound_box bar5(instruction_left - left_offset * 1.1, instruction_bottom, instruction_right + left_offset * 0.1, instruction4_dvder);


    t_bound_box distance_interface1(instruction_right - 0.5 * left_offset, instruction1_dvder, instruction_right, instruction_hint - distance_offset);
    t_bound_box distance_interface2(instruction_right - 0.5 * left_offset, instruction2_dvder, instruction_right, instruction1_dvder - distance_offset);
    t_bound_box distance_interface3(instruction_right - 0.5 * left_offset, instruction3_dvder, instruction_right, instruction2_dvder - distance_offset);
    t_bound_box distance_interface4(instruction_right - 0.5 * left_offset, instruction4_dvder, instruction_right, instruction3_dvder - distance_offset);
    t_bound_box distance_interface5(instruction_right - 0.5 * left_offset, instruction_bottom, instruction_right, instruction4_dvder - distance_offset);


    t_bound_box time_interface1(instruction_right - 0.5 * left_offset, instruction1_dvder, instruction_right, instruction_hint + distance_offset);
    t_bound_box time_interface2(instruction_right - 0.5 * left_offset, instruction2_dvder, instruction_right, instruction1_dvder + distance_offset);
    t_bound_box time_interface3(instruction_right - 0.5 * left_offset, instruction3_dvder, instruction_right, instruction2_dvder + distance_offset);
    t_bound_box time_interface4(instruction_right - 0.5 * left_offset, instruction4_dvder, instruction_right, instruction3_dvder + distance_offset);
    t_bound_box time_interface5(instruction_right - 0.5 * left_offset, instruction_bottom, instruction_right, instruction4_dvder + distance_offset);
    //t_bound_box distance_time_1()
    setcolor(LIGHTGREY);
    setlinewidth(1);
    drawline(dvd1l, dvd1r);
    drawline(dvd2l, dvd2r);
    drawline(dvd3l, dvd3r);
    drawline(dvd4l, dvd4r);
    string distance_string;
    if (instruction_size >= 5) {
        for (i = 0; i < 5; i++) {
            instruction[i] = direction_data.instruction[i + directionValue];
            speedLimit[i] = direction_data.speedlimit[i + directionValue];
            if (direction_data.street_distance[i + directionValue] < 1000 && direction_data.street_distance[i + directionValue] > 0) {
                distance_strings[i] = int_to_string(distance_optimizer(direction_data.street_distance[i + directionValue])) + " m ";
            } else if (direction_data.street_distance[i + directionValue] >= 1000) {
                distance_strings[i] = double_to_string(distance_optimizer_kilometer(direction_data.street_distance[i + directionValue])) + " km";
            } else if (direction_data.street_distance[i + directionValue] == -1) {
                distance_strings[i] = " ";
            }
            if (direction_data.travel_time[i + directionValue] < 60 && direction_data.travel_time[i + directionValue] > 0) {
                time_strings[i] = int_to_string(int(direction_data.travel_time[i + directionValue])) + " sec";
            } else if (direction_data.travel_time[i + directionValue] >= 60 && direction_data.travel_time[i + directionValue] < 3600) {
                time_strings[i] = int_to_string(travel_time_minutes(direction_data.travel_time[i + directionValue])) + " min";
            } else if (direction_data.travel_time[i + directionValue] >= 3600) {
                time_strings[i] = double_to_string(travel_time_hours(direction_data.travel_time[i + directionValue])) + " hr";
            } else if (direction_data.travel_time[i + directionValue] == -1) {
                time_strings[i] = " ";
            }
            intersectionids_in_route_planner[i] = direction_data.intersection_id[i + directionValue];
            if (speedLimit[i] > 0) {
                ostringstream tem;
                tem << speedLimit[i];
                string convert = tem.str();
                instruction[i] = instruction[i];
            }
        }
    } else if (instruction_size < 5) {
        for (i = 0; i < instruction_size; i++) {

            instruction[i] = direction_data.instruction[i];
            speedLimit[i] = direction_data.speedlimit[i];
            if (direction_data.street_distance[i] < 1000 && direction_data.street_distance[i ] > 0) {
                distance_strings[i] = int_to_string(distance_optimizer(direction_data.street_distance[i])) + " m ";
            } else if (direction_data.street_distance[i] >= 1000) {
                distance_strings[i] = double_to_string(distance_optimizer_kilometer(direction_data.street_distance[i])) + " km";
            } else if (direction_data.street_distance[i] == -1) {
                distance_strings[i] = " ";
            }
            if (direction_data.travel_time[i] < 60 && direction_data.travel_time[i] > 0) {
                time_strings[i] = int_to_string(int(direction_data.travel_time[i])) + " sec";
            } else if (direction_data.travel_time[i] >= 60 && direction_data.travel_time[i] < 3600) {

                time_strings[i] = int_to_string(travel_time_minutes(direction_data.travel_time[i])) + " min";
            } else if (direction_data.travel_time[i] >= 3600) {
                time_strings[i] = double_to_string(travel_time_hours(direction_data.travel_time[i])) + " hr";
            } else if (direction_data.travel_time[i] == -1) {
                time_strings[i] = " ";
            }
            intersectionids_in_route_planner[i] = direction_data.intersection_id[i];
            if (speedLimit[i] > 0) {
                ostringstream tem;
                tem << speedLimit[i];
                string convert = tem.str();
                instruction[i] = instruction[i]; //+ "\n" + "  SL:  " + convert
            }
        }
        for (i = instruction_size; i < 5; i++) {
            instruction[i] = " ";
            speedLimit[i] = -1;
            distance_strings[i] = " ";
            time_strings[i] = " ";
        }
    }

    if (showInstruction)
        setcolor(BLACK);
    setfontsize(10);
    drawtext_in(bar1, instruction[0]);
    drawtext_in(bar2, instruction[1]);
    drawtext_in(bar3, instruction[2]);
    drawtext_in(bar4, instruction[3]);
    drawtext_in(bar5, instruction[4]);
    setcolor(t_color(113, 113, 113, 500));
    setfontsize(9);
    drawtext_in(distance_interface1, distance_strings[0]);
    drawtext_in(distance_interface2, distance_strings[1]);
    drawtext_in(distance_interface3, distance_strings[2]);
    drawtext_in(distance_interface4, distance_strings[3]);
    drawtext_in(distance_interface5, distance_strings[4]);
    setcolor(BLACK);
    setfontsize(9);
    drawtext_in(time_interface1, time_strings[0]);
    drawtext_in(time_interface2, time_strings[1]);
    drawtext_in(time_interface3, time_strings[2]);
    drawtext_in(time_interface4, time_strings[3]);
    drawtext_in(time_interface5, time_strings[4]);
    t_bound_box selectBox_final;
    /*The following show the bar motion when up and down are hit*/
    if (instruction_pointing == 0) {
        t_bound_box selectBox(instruction_left - left_offset, instruction1_dvder, instruction_left - 0.8 * left_offset, instruction_hint);
        selectBox_final = selectBox;
    } else if (instruction_pointing == 1) {
        t_bound_box selectBox(instruction_left - left_offset, instruction2_dvder, instruction_left - 0.8 * left_offset, instruction1_dvder);
        selectBox_final = selectBox;
    } else if (instruction_pointing == 2) {
        t_bound_box selectBox(instruction_left - left_offset, instruction3_dvder, instruction_left - 0.8 * left_offset, instruction2_dvder);
        selectBox_final = selectBox;
    } else if (instruction_pointing == 3) {
        t_bound_box selectBox(instruction_left - left_offset, instruction4_dvder, instruction_left - 0.8 * left_offset, instruction3_dvder);
        selectBox_final = selectBox;
    } else if (instruction_pointing == 4) {
        t_bound_box selectBox(instruction_left - left_offset, instruction_bottom, instruction_left - 0.8 * left_offset, instruction4_dvder);
        selectBox_final = selectBox;
    }
    setcolor(t_color(174, 193, 229, 500));
    int order = directionValue + 1 + instruction_pointing;
    string order_string = int_to_string(order);
    fillrect(selectBox_final);
    setcolor(WHITE);
    setfontsize(11);
    drawtext_in(selectBox_final, order_string);

}

void route_planner_interface() {
    string temp_from = place1;
    string temp_to = place2;
    t_bound_box route_planner = get_visible_world();
    // inner bound
    route_left_coordinate = left_coordinate;
    route_left_coordinate2 = left_coordinate2;
    route_right_coordinate = right_coordinate;
    route_right_coordinate2 = right_coordinate2;
    route_top_coordinate = top_coordinate;
    route_bottom_coordinate = bottom_coordinate;

    /*route planer has the same width with search bar
     * which means the left coordinate and right coordinate are the same
     when it is activated, there should be square drawn in the same place with search bar
     */

    /*Background color should be 67, 122, 248 in rgb*/


    float bar_offset = route_planner.get_height()*0.05;
    place_text_bar1_top = route_bottom_coordinate - bar_offset;
    place_text_bar2_top = place_text_bar1_top - bar_offset;

    t_color bagblue(67, 122, 248, 1000);
    // This color was picked following Google map
    setcolor(bagblue);
    t_bound_box startPoint(route_left_coordinate2, place_text_bar1_top, route_left_coordinate, route_bottom_coordinate);
    fillrect(startPoint);
    setfontsize(12);
    setcolor(WHITE);
    drawtext_in(startPoint, "From");
    //this t_bound_box is used for the D symbol in front of the first line
    t_bound_box destination(route_left_coordinate2, place_text_bar2_top, route_left_coordinate, place_text_bar1_top);
    setcolor(bagblue);
    fillrect(destination);
    setfontsize(12);
    setcolor(WHITE);
    drawtext_in(destination, "To"); // this t_bound_box is used for the S symbol in front of the second line
    /*The next two box is used to received the content for route find function*/

    t_bound_box firstPlace(route_left_coordinate, place_text_bar1_top, route_right_coordinate, route_bottom_coordinate); //this two is used to received the input
    t_bound_box secondPlace(route_left_coordinate, place_text_bar2_top, route_right_coordinate, place_text_bar1_top);
    setcolor(bagblue);
    fillrect(firstPlace);
    fillrect(secondPlace);

    t_point left1(route_left_coordinate, place_text_bar1_top);
    t_point left2(route_left_coordinate, place_text_bar2_top);

    t_point right1(route_right_coordinate, place_text_bar1_top);
    t_point right2(route_right_coordinate, place_text_bar2_top);

    setcolor(WHITE);
    setlinewidth(1);
    drawline(left1, right1);
    drawline(left2, right2);

    setcolor(WHITE);
    setfontsize(13);
    if (type_place1) {
        temp_from = place1 + "|";
    }
    if (type_place2) {
        temp_to = place2 + "|";
    }
    if (place1 == "WRONG") {
        setcolor(RED);
    }
    drawtext_in(firstPlace, temp_from);
    if (place2 == "WRONG") {
        setcolor(RED);
    }
    drawtext_in(secondPlace, temp_to);

    /*Until here, two input bar has been displayed*/
    /*The following part is used for the direction function button*/

    t_bound_box directionButton(route_right_coordinate, place_text_bar1_top, route_right_coordinate2, route_bottom_coordinate);
    t_bound_box reverseButton(route_right_coordinate, place_text_bar2_top, route_right_coordinate2, place_text_bar1_top);

    t_point rright1(route_right_coordinate2, place_text_bar1_top);
    t_point leftUp(route_right_coordinate, route_bottom_coordinate);
    t_point leftDown(route_right_coordinate, place_text_bar2_top);
    setcolor(BLACK);
    setlinewidth(1);
    drawline(right1, rright1);
    drawline(leftDown, leftUp);

    setcolor(bagblue);
    fillrect(directionButton);
    fillrect(reverseButton);
    setfontsize(12);
    setcolor(WHITE);
    drawtext_in(directionButton, "Find Path");
    drawtext_in(reverseButton, "Reverse");

}

void draw_route_planner_label() {//this function draws route planner label-----
    t_bound_box offset_wold = get_visible_world();
    float original_x = (left_coordinate + left_coordinate2) / 2;
    float original_y = (top_coordinate + bottom_coordinate) / 2;
    float left_offset = offset_wold.get_width()* 0.017;
    float top_offset = offset_wold.get_height()*0.026;
    Surface route_planner("direction.png");
    float pos_x = original_x - left_offset;
    float pos_y = original_y + top_offset;
    draw_surface(route_planner, pos_x, pos_y);
}

void draw_route_planner_label_when_hit() {
    t_bound_box offset_wold = get_visible_world();
    float original_x = (left_coordinate + left_coordinate2) / 2;
    float original_y = (top_coordinate + bottom_coordinate) / 2;
    float left_offset = offset_wold.get_width()* 0.017;
    float top_offset = offset_wold.get_height()*0.026;
    Surface route_planner("direction_hit.png");
    float pos_x = original_x - left_offset;
    float pos_y = original_y + top_offset;
    draw_surface(route_planner, pos_x, pos_y);
}

//auto_zoom_in street function

void street_auto_zoom_in(unsigned streetID) {
    // Auto zoomin function 
    vector<unsigned> segments_on_street = globalBase->find_street_street_segments_in_DB(streetID);
    unsigned number_of_seg = segments_on_street.size();
    //find the medium segment
    bool even = (number_of_seg % 2 == 0);
    unsigned index_in_street;
    unsigned medium_segment_id;
    if (even) {
        index_in_street = (number_of_seg / 2) - 1;
    } else {
        index_in_street = ((number_of_seg + 1) / 2) - 1;
    }
    medium_segment_id = segments_on_street[index_in_street];
    StreetSegmentInfo segment_info = globalBase->get_segment_info(medium_segment_id);
    unsigned medium_from_id = segment_info.from;
    unsigned medium_to_id = segment_info.to;
    t_point medium_from = globalBase->get_t_point_intersection_pos(medium_from_id);
    t_point medium_to = globalBase->get_t_point_intersection_pos(medium_to_id);
    if (number_of_seg < 2) {
        t_point medium_pos;
        medium_pos.x = (medium_from.x + medium_to.x) / 2;
        medium_pos.y = (medium_from.y + medium_to.y) / 2;
        auto_zoom_in_street(medium_pos);
    } else {
        auto_zoom_in_street(medium_from);
    }
}
//draw highlighted street name 

void draw_street_name_highlight(unsigned highlight_id) {
    vector<unsigned> segments_on_street = globalBase->find_street_street_segments_in_DB(highlight_id);
    unsigned number_of_seg = segments_on_street.size();
    double angle;
    vector<t_point> curve_points;
    //find the medium segment
    bool even = (number_of_seg % 2 == 0);
    unsigned index_in_street;
    unsigned medium_segment_id;
    if (even) {
        index_in_street = (number_of_seg / 2) - 1;
    } else {
        index_in_street = ((number_of_seg + 1) / 2) - 1;
    }
    medium_segment_id = segments_on_street[index_in_street];
    StreetSegmentInfo segment_info = globalBase->get_segment_info(medium_segment_id);
    unsigned medium_from_id = segment_info.from;
    unsigned medium_to_id = segment_info.to;
    t_point mid_pos;
    t_point medium_from = globalBase->get_t_point_intersection_pos(medium_from_id);
    t_point medium_to = globalBase->get_t_point_intersection_pos(medium_to_id);
    string streetName = globalBase->find_street_name_from_ids_in_DB(segment_info.streetID);
    double segment_length = globalBase->find_street_segment_length_in_DB(medium_segment_id);
    double length = segment_length / 100000;
    if (segment_info.curvePointCount == 0) {
        mid_pos.x = (medium_from.x + medium_to.x) / 2;
        mid_pos.y = (medium_from.y + medium_to.y) / 2;
        setcolor(BLACK);
        settextrotation(setRoadAngle(medium_from, medium_from));
        if (streetName != "<unknown>") {
            setfontsize(10);
            drawtext(mid_pos.x, mid_pos.y, streetName, 200);
        }
    } else {
        //situation that this street has no name
        curve_points = globalBase->get_curve_points_in_t_point(medium_segment_id);
        mid_pos.x = (medium_from.x + curve_points[0].x) / 2;
        mid_pos.y = (medium_from.y + curve_points[0].y) / 2;
        for (unsigned i = 0; i < curve_points.size(); ++i) {
            angle = setRoadAngle(mid_pos, curve_points[i]);
            mid_pos.x = (mid_pos.x + curve_points[i].x) / 2;
            mid_pos.y = (mid_pos.y + curve_points[i].y) / 2;
        }
        angle = setRoadAngle(mid_pos, medium_to);
        mid_pos.x = (mid_pos.x + medium_to.x) / 2;
        mid_pos.y = (mid_pos.y + medium_to.y) / 2;
        setcolor(BLACK);
        settextrotation(angle);
        if (streetName != "<unknown>") {
            setfontsize(10);
            drawtext(mid_pos.x, mid_pos.y, streetName, 200);
        }
    }
}

void act_on_route_search_parser(void (*drawscreen_ptr)(void)) {
    route_search_parser();
}

//parser for route_searcch interface

void route_search_parser() {
    //Processing the input in route planner and search for the route 
    //Result_Type_And_ID is defined in m3.h
    Result_Type_And_ID place1_result = search_function_helper(place1);
    Result_Type_And_ID place2_result = search_function_helper(place2);
    //get the type of place1 and place2 respectively
    string type_place1 = place1_result.type;
    unsigned id_place1 = place1_result.id;
    string type_place2 = place2_result.type;
    unsigned id_place2 = place2_result.id;
    //if the type of place is unknown, do fuzzy search
    if (type_place1 == "unknown") {
        string temp = fuzzy_search(place1);
        place1_result = search_function_helper(temp);
        type_place1 = place1_result.type;
        id_place1 = place1_result.id;
        if (type_place1 == "unknown") {
            place1 = "WRONG";
            showInstruction = false;
            // return;
        } else {
            place1 = temp;
            route_search_parser();
        }
    } else if (type_place2 == "unknown") {
        string temp2 = fuzzy_search(place2);
        place2_result = search_function_helper(temp2);
        type_place2 = place2_result.type;
        id_place2 = place2_result.id;
        if (type_place2 == "unknown") {
            place2 = "WRONG";
            showInstruction = false;
        } else {
            place2 = temp2;
            route_search_parser();
        }
    } else {
        //if types are intersection and intersection
        if (type_place1 == "intersection" && type_place2 == "intersection") {
            double penalty = 1;

            seg_list = find_path_between_intersections(id_place1, id_place2, penalty);
            if (!seg_list.empty()) {
                direction_data = direction_process(seg_list);
                travel_time = compute_path_travel_time(seg_list, penalty);
                if (travel_time >= 60) {
                    cout << "total travel  time is: " << travel_time_minutes(travel_time) << " minutes" << endl;
                } else if (travel_time >= 3600) {
                    cout << "total travel  time is: " << travel_time_hours(travel_time) << " hours" << endl;
                } else {
                    cout << "total travel  time is: " << int(travel_time) << " seconds" << endl;
                }
                cout << endl;
                highlight_path = {};
                highlight_path = {};
                highlight_inters = {};
                highlight_pois = {};
                highlight_path = seg_list;
                processForceAdd(highlight_inters, id_place1);
                processForceAdd(highlight_inters, id_place2);
                t_point midpoint = globalBase -> find_midpoint_of_two_inters(id_place1, id_place2);
                t_point start = globalBase -> get_t_point_intersection_pos(id_place1);
                t_point end = globalBase -> get_t_point_intersection_pos(id_place2);
                auto_zoom_in_smart(start, end, midpoint);
                draw_screen();
                show_route_planner = true;
                route_planner_interface();
                showInstruction = true;
                instruction_pointing = 0;
                directionValue = 0;
                detailedInstruction_interface();
            } else {
                place1 = "WRONG";
                place2 = "WRONG";
                showInstruction = false;
            }
        }//if types are poi and intersection 
        else if (type_place1 == "poi" && type_place2 == "intersection") {
            double penalty = 1;
            string poi_name = globalBase->get_POI_name_from_id(id_place1);
            seg_list = find_path_to_point_of_interest(id_place2, poi_name, penalty);
            unsigned POI_id = get_the_best_POI_id(id_place2, poi_name, penalty);
            if (!seg_list.empty()) {
                std::reverse(seg_list.begin(), seg_list.end());
                direction_data = direction_process(seg_list);
                travel_time = compute_path_travel_time(seg_list, penalty);
                if (travel_time >= 60) {
                    cout << "total travel  time is: " << travel_time_minutes(travel_time) << " minutes" << endl;
                } else if (travel_time >= 3600) {
                    cout << "total travel  time is: " << travel_time_hours(travel_time) << " hours" << endl;
                } else {
                    cout << "total travel  time is: " << int(travel_time) << " seconds" << endl;
                }
                cout << endl;
                highlight_path = {};
                highlight_inters = {};
                highlight_pois = {};
                highlight_path = seg_list;
                processForceAdd(highlight_inters, id_place2);
                processForceAdd(highlight_pois, POI_id);
                t_point midpoint = globalBase ->get_POI_id_to_t_point(POI_id);
                auto_zoom_in(midpoint);
                draw_screen();
                show_route_planner = true;
                route_planner_interface();
                showInstruction = true;
                directionValue = 0;
                instruction_pointing = 0;
                detailedInstruction_interface();
            } else {
                place1 = "WRONG";
                place2 = "WRONG";
                showInstruction = false;
            }

        }//if types are intersection and poi
        else if (type_place1 == "intersection" && type_place2 == "poi") {
            double penalty = 1;
            string poi_name = globalBase->get_POI_name_from_id(id_place2);
            seg_list = find_path_to_point_of_interest(id_place1, poi_name, penalty);
            unsigned POI_id = get_the_best_POI_id(id_place1, poi_name, penalty);
            if (!seg_list.empty()) {
                direction_data = direction_process(seg_list);
                travel_time = compute_path_travel_time(seg_list, penalty);
                if (travel_time >= 60) {
                    cout << "total travel  time is: " << travel_time_minutes(travel_time) << " minutes" << endl;
                } else if (travel_time >= 3600) {
                    cout << "total travel  time is: " << travel_time_hours(travel_time) << " hours" << endl;
                } else {
                    cout << "total travel  time is: " << int(travel_time) << " seconds" << endl;
                }
                cout << endl;
                highlight_path = {};
                highlight_inters = {};
                highlight_pois = {};
                highlight_path = seg_list;
                processForceAdd(highlight_inters, id_place1);
                processForceAdd(highlight_pois, POI_id);
                t_point midpoint = globalBase -> get_t_point_intersection_pos(id_place1);
                auto_zoom_in(midpoint);
                draw_screen();
                show_route_planner = true;
                route_planner_interface();
                showInstruction = true;
                directionValue = 0;
                instruction_pointing = 0;
                detailedInstruction_interface();
            } else {
                place1 = "WRONG";
                place2 = "WRONG";
                showInstruction = false;
            }
        }//if types are poi and poi 
        else if (type_place1 == "poi" && type_place2 == "poi") {
            double penalty = 1;
            string poi_end_name = globalBase->get_POI_name_from_id(id_place2);
            seg_list = find_path_between_pois(id_place1, poi_end_name, penalty);
            unsigned POI_id = get_the_best_POI_id_pois(id_place1, poi_end_name, penalty);
            if (!seg_list.empty()) {
                direction_data = direction_process(seg_list);
                travel_time = compute_path_travel_time(seg_list, penalty);
                if (travel_time >= 60) {
                    cout << "total travel  time is: " << travel_time_minutes(travel_time) << " minutes" << endl;
                } else if (travel_time >= 3600) {
                    cout << "total travel  time is: " << travel_time_hours(travel_time) << " hours" << endl;
                } else {
                    cout << "total travel  time is: " << int(travel_time) << " seconds" << endl;
                }
                cout << endl;
                t_point poi_start_t_point = globalBase->get_POI_id_to_t_point(id_place1);
                LatLon poi_start_latlon = globalBase -> t_point_to_latlon(poi_start_t_point);
                unsigned closest_inter1 = globalBase -> find_closest_intersection_in_DB(poi_start_latlon);
                highlight_path = {};
                highlight_inters = {};
                highlight_pois = {};
                highlight_path = seg_list;
                processForceAdd(highlight_pois, id_place1);
                processForceAdd(highlight_pois, POI_id);
                t_point midpoint = globalBase -> get_t_point_intersection_pos(closest_inter1);
                auto_zoom_in(midpoint);
                draw_screen();
                show_route_planner = true;
                route_planner_interface();
                showInstruction = true;
                directionValue = 0;
                instruction_pointing = 0;
                detailedInstruction_interface();
            } else {
                place1 = "WRONG";
                place2 = "WRONG";
                showInstruction = false;
            }
        }
    }

}

//search function helper to get result type and its id 

Result_Type_And_ID search_function_helper(string& name) {
    auto iter = name.find("And"); //find the And in input
    auto iter2 = name.find("&"); //find the "&" symbol
    string temp_type;
    Result_Type_And_ID temp_result;
    if (iter == string::npos && iter2 == string::npos) {
        bool existingInPOI = globalBase->check_POI_existing(name);
        bool existingInFeature = globalBase->check_feature_existing(name);
        if (existingInPOI) {//if it exists in POI
            temp_type = "poi";
            unsigned poi_id = globalBase->get_POI_id_from_name(name);
            temp_result.type = temp_type;
            temp_result.id = poi_id;
            return temp_result;
        } else if (existingInFeature) {//if it exists in features
            temp_type = "feature";
            unsigned feature_id = globalBase -> get_feature_id_from_name(name);
            temp_result.type = temp_type;
            temp_result.id = feature_id;
        } else {
            temp_result.type = "unknown";
            temp_result.id = 0;
        }
    } else if (iter != string::npos && iter2 == string::npos) {// if it is intended to get the intersection of two street
        temp_result.type = "intersection";
        string temp_content = name;
        temp_content.erase(iter);
        string streetname1 = temp_content;
        streetname1.pop_back();
        //cout << streetname1 << endl;
        temp_content = name;
        string reversed(temp_content.rbegin(), temp_content.rend());
        auto iter2 = reversed.find("dnA");
        temp_content = reversed.erase(iter2);
        if (temp_content != "") {
            temp_content.pop_back();
            string streetname2(temp_content.rbegin(), temp_content.rend());
            //cout << streetname2 << endl;
            if (globalBase->check_street_existing(streetname1) && globalBase->check_street_existing(streetname2)) {
                std::vector<unsigned> intersectionBetweenTwoStreet = globalBase->find_intersection_ids_from_street_names_in_DB(streetname1, streetname2);
                if (intersectionBetweenTwoStreet.size() == 0) {//if the intersection is not found
                    temp_result.type = "unknown";
                    temp_result.id = 0;
                } else {
                    name = streetname1 + " & " + streetname2;
                    temp_result.id = intersectionBetweenTwoStreet.at(0);
                    return temp_result;
                }
            } else {
                streetname1 = fuzzy_search_streets(streetname1);
                streetname2 = fuzzy_search_streets(streetname2);
                std::vector<unsigned> intersectionBetweenTwoStreet = globalBase->find_intersection_ids_from_street_names_in_DB(streetname1, streetname2);
                if (intersectionBetweenTwoStreet.size() == 0) {//if the intersection is not found
                    temp_result.type = "unknown";
                    temp_result.id = 0;
                } else {
                    name = streetname1 + " & " + streetname2;
                    temp_result.id = intersectionBetweenTwoStreet.at(0);

                    return temp_result;
                }
            }
        } else {
            temp_result.type = "unknown";
            temp_result.id = 0;
        }

    } else if (iter == string::npos && iter2 != string::npos) {//find "&" intersection
        temp_result.type = "intersection";
        temp_result.type = "intersection";
        string temp_content = name;
        temp_content.erase(iter2);
        string streetname1 = temp_content;
        streetname1.pop_back();

        temp_content = name;
        string reversed(temp_content.rbegin(), temp_content.rend());
        auto iter3 = reversed.find("&");
        temp_content = reversed.erase(iter3);
        if (temp_content != "") {
            temp_content.pop_back();
            string streetname2(temp_content.rbegin(), temp_content.rend());

            if (globalBase->check_street_existing(streetname1) && globalBase->check_street_existing(streetname2)) {
                std::vector<unsigned> intersectionBetweenTwoStreet = globalBase->find_intersection_ids_from_street_names_in_DB(streetname1, streetname2);
                if (intersectionBetweenTwoStreet.size() == 0) {//if the intersection is not found
                    temp_result.type = "unknown";
                    temp_result.id = 0;
                } else {
                    temp_result.id = intersectionBetweenTwoStreet.at(0);
                    return temp_result;
                }
            } else {
                streetname1 = fuzzy_search_streets(streetname1);
                streetname2 = fuzzy_search_streets(streetname2);
                std::vector<unsigned> intersectionBetweenTwoStreet = globalBase->find_intersection_ids_from_street_names_in_DB(streetname1, streetname2);
                if (intersectionBetweenTwoStreet.size() == 0) {//if the intersection is not found
                    temp_result.type = "unknown";
                    temp_result.id = 0;
                } else {
                    temp_result.id = intersectionBetweenTwoStreet.at(0);
                    return temp_result;
                }
            }
        } else {
            temp_result.type = "unknown";
            temp_result.id = 0;
        }

    } else {
        temp_result.type = "unknown";
        temp_result.id = 0;
    }

}

//only fuzzy search poi 
//only returns a fuzzypoi name

string fuzzy_search(string name) {
    vector<int> content_distance;
    vector<string> content_keyword;
    unsigned closest_name_id;
    //    vector<vector<string>>street_name_lists=globalBase->get_street_name_list();
    /*   This part will be used for future function, which can not be finished by the deadline of m2. They are not old codes    */
    int distance;
    stringstream temp_ss(name);
    string temp_content;
    temp_ss>>temp_content;
    while (!temp_ss.fail()) {
        content_keyword.push_back(temp_content);
        temp_ss>>temp_content;
    }
    //only find the closest result to first word
    //structure should be loop through every keyword in street name data base and add distance togather
    for (unsigned i = globalBase->get_streetNumbers(); i < globalBase->get_streetNumbers() + globalBase->POInumber(); i++) {
        if (globalBase->find_name_from_ids_in_nameBase(i) == "<unknown>" || toupper(name.at(0)) != globalBase->find_name_from_ids_in_nameBase(i).at(0)) {
            distance = 1000;
            content_distance.push_back(distance);
        } else {
            distance = 0;
            int individual_distance = 0;
            vector<string> name_keyword = name_database[i];
            for (unsigned j = 0; j < content_keyword.size(); j++) {
                individual_distance = levenshtein_distance(content_keyword[j], name_keyword[0]);
                for (unsigned k = 0; k < name_keyword.size(); k++) {
                    int temp_distance = levenshtein_distance(content_keyword[j], name_keyword[k]);
                    //find the word with closest distance
                    if (temp_distance < individual_distance) {
                        individual_distance = temp_distance;
                    }
                }
                distance += individual_distance;
            }
            content_distance.push_back(distance);
        }
    }
    //distance has been all stored, now check the three minimum
    vector<int> temp = content_distance;
    closest_name_id = 0;
    int minimum_distance = content_distance[0];
    for (unsigned i = 0; i < globalBase->POInumber(); i++) {
        if (content_distance[i] < minimum_distance) {
            minimum_distance = content_distance[i];
            closest_name_id = i;
        }
    }
    return globalBase->get_POI_name_from_id(closest_name_id);
}

void actOnReverseFunction() {
    //reverse function
    string medium;
    medium = place1;
    place1 = place2;
    place2 = medium;
    route_planner_interface();
}

//fuzzy search for street name

string fuzzy_search_streets(string name) {
    vector<int> content_distance;
    vector<string> content_keyword;
    unsigned closest_name_id;
    /*   This part will be used for future function, which can not be finished by the deadline of m2. They are not old codes    */
    int distance;
    stringstream temp_ss(name);
    string temp_content;
    temp_ss>>temp_content;
    while (!temp_ss.fail()) {
        content_keyword.push_back(temp_content);
        temp_ss>>temp_content;
    }
    //only find the closest result to first word
    //structure should be loop through every keyword in street name data base and add distance togather
    for (unsigned i = 0; i < globalBase->get_streetNumbers(); i++) {
        if (globalBase->find_name_from_ids_in_nameBase(i) == "<unknown>" || toupper(name.at(0)) != globalBase->find_name_from_ids_in_nameBase(i).at(0)) {
            distance = 1000;
            content_distance.push_back(distance);
        } else {
            distance = 0;
            int individual_distance = 0;
            vector<string> name_keyword = name_database[i];
            for (unsigned j = 0; j < content_keyword.size(); j++) {
                individual_distance = levenshtein_distance(content_keyword[j], name_keyword[0]);
                for (unsigned k = 0; k < name_keyword.size(); k++) {
                    int temp_distance = levenshtein_distance(content_keyword[j], name_keyword[k]);
                    //find the word with closest distance
                    if (temp_distance < individual_distance) {
                        individual_distance = temp_distance;
                    }
                }
                distance += individual_distance;
            }
            content_distance.push_back(distance);
        }
    }
    //distance has been all stored, now check the three minimum
    vector<int> temp = content_distance;
    closest_name_id = 0;
    int minimum_distance = content_distance[0];
    for (unsigned i = 0; i < globalBase->get_streetNumbers(); i++) {
        if (content_distance[i] < minimum_distance) {
            minimum_distance = content_distance[i];
            closest_name_id = i;
        }
    }
    return globalBase->find_street_name_from_ids_in_DB(closest_name_id);
}

int travel_time_minutes(double travel_time_in_sec) {
    // convert seconds to minutes
    return int(travel_time_in_sec / 60);
}

double travel_time_hours(double travel_time_in_sec) {
    //convert minutes to hours
    double miniutes = travel_time_in_sec / 60;
    int multiple = 30;
    int minutes_temp = miniutes;
    int minutes_temp2;
    minutes_temp2 = minutes_temp + multiple / 2;
    minutes_temp2 -= minutes_temp2 % minutes_temp;
    return double(minutes_temp2) / double(60);
}

void showHelp() {
    t_bound_box helper_box = get_visible_world();

    float left_offset = helper_box.get_width()* 0.02;
    help_windows_left = helper_box.left() + left_offset;
    left_fram = help_windows_left + 0.39 * left_offset;

    float right_offset = helper_box.get_width()* 0.21;
    help_windows_right = helper_box.left() + right_offset;
    right_fram = help_windows_right - 0.15 * left_offset;

    float top_offset = helper_box.get_height()* 0.2;
    help_windows_top = helper_box.top() - top_offset;
    top_fram = help_windows_top - 0.1 * top_offset;

    float bottom_offset = helper_box.get_height()*0.6;
    help_windows_bottom = helper_box.top() - bottom_offset;
    bottom_fram = help_windows_bottom + 0.1 * top_offset;

    t_bound_box hWindows(help_windows_left, help_windows_bottom, help_windows_right, help_windows_top);
    t_bound_box innerFram(left_fram, bottom_fram, right_fram, top_fram);

    t_color bgGREY(117, 117, 117, 1000);
    t_color dvdGREY(200, 200, 200, 1000);

    setcolor(bgGREY); //base fram of help windows
    fillrect(hWindows);

    setcolor(WHITE); // inner fram of helo windows
    fillrect(innerFram);

    dvder1 = top_fram - (top_fram - bottom_fram) / 10; // bottom line for "USER GUID"
    float dvdoffset = (dvder1 - bottom_fram) / 4;

    dvder2 = dvder1 - dvdoffset;
    dvder3 = dvder2 - dvdoffset;
    dvder4 = dvder3 - dvdoffset;

    t_bound_box userGuid(left_fram, dvder1, right_fram, top_fram);
    t_bound_box answer1(left_fram, dvder2, right_fram, dvder1);
    t_bound_box answer2(left_fram, dvder3, right_fram, dvder2);
    t_bound_box answer3(left_fram, dvder4, right_fram, dvder3);
    t_bound_box answer4(left_fram, bottom_fram, right_fram, dvder4);

    setcolor(BLACK);

    string ug = "USER GUIDANCE(once to enter, twice to return)";
    setfontsize(8);
    string a1 = "1, Explanation of right side button";
    string a2 = "2, Guidance on Search Bar";
    string a4 = "4, Guidance on Direction function";
    string a3 = "3, Guidance on route planner";

    drawtext_in(userGuid, ug);
    setfontsize(9);
    drawtext_in(answer1, a1);
    drawtext_in(answer2, a2);
    drawtext_in(answer3, a3);
    drawtext_in(answer4, a4);

    t_point dvd1l(left_fram, dvder1);
    t_point dvd1r(right_fram, dvder1);

    t_point dvd2l(0.75 * left_fram + 0.25 * right_fram, dvder2);
    t_point dvd2r(0.75 * right_fram + 0.25 * left_fram, dvder2);
    t_point dvd3l(0.75 * left_fram + 0.25 * right_fram, dvder3);
    t_point dvd3r(0.75 * right_fram + 0.25 * left_fram, dvder3);
    t_point dvd4l(0.75 * left_fram + 0.25 * right_fram, dvder4);
    t_point dvd4r(0.75 * right_fram + 0.25 * left_fram, dvder4);

    setcolor(dvdGREY);
    setlinewidth(1);
    drawline(dvd1l, dvd1r);
    drawline(dvd2l, dvd2r);
    drawline(dvd3l, dvd3r);
    drawline(dvd4l, dvd4r);
}

void advancedHelp() {
    t_bound_box a = get_visible_world();
    float x_coordinate = (left_fram + right_fram) / 2;
    float y_coordinate = (top_fram + bottom_fram) / 2;

    float x_offset = a.get_width()*0.09;
    float y_offset = a.get_height()*0.2018;
    if (click_on_answer1) {
        Surface t("4.png");
        draw_surface(t, x_coordinate - x_offset, y_coordinate + y_offset);
    } else if (click_on_answer2) {
        Surface y("3.png");
        draw_surface(y, x_coordinate - x_offset, y_coordinate + y_offset);
    } else if (click_on_answer3) {
        Surface rt("direction copy2.png");
        draw_surface(rt, x_coordinate - x_offset, y_coordinate + y_offset);
    } else if (click_on_answer4) {
        Surface ins("instruction2.png");
        draw_surface(ins, x_coordinate - x_offset, y_coordinate + y_offset);
    }
}

void act_on_mouse_move(float x, float y) {
    // function to handle mouse move event, the current mouse position in the current world coordinate
    //    // system (as defined in your call to init_world) is returned
    show_auto_fill = string_input.size() > 2 && content[0] != ' ';
    if (show_auto_fill) {
        if (mouse_click_in_rec(x, y, left_coordinate_autofill, right_coordinate_autofill, top1_coordinate_autofill, top2_coordinate_autofill)) {
            autoFillSquare(1);
        } else if (mouse_click_in_rec(x, y, left_coordinate_autofill, right_coordinate_autofill, top2_coordinate_autofill, top3_coordinate_autofill)) {
            autoFillSquare(2);
        } else if (mouse_click_in_rec(x, y, left_coordinate_autofill, right_coordinate_autofill, top3_coordinate_autofill, top4_coordinate_autofill)) {
            autoFillSquare(3);
        }
    }
    if (mouse_click_in_rec(x, y, left_coordinate2, left_coordinate, top_coordinate, bottom_coordinate)) {
        draw_route_planner_label_when_hit();
    } else if (!mouse_click_in_rec(x, y, left_coordinate, left_coordinate2, top_coordinate, bottom_coordinate)&&!show_route_planner) {
        draw_route_planner_label();
    }
    if (mouse_click_in_rec(x, y, symbol_left, symbol_right, symbol_top, symbol_bottom)) {
        helperSymbol_hit();
    } else {
        helperSymbol();
    }
}

void helperSymbol_hit() {

    t_bound_box helper_symbol = get_visible_world();

    float left_offset = helper_symbol.get_width()* 0.92;
    symbol_left = helper_symbol.left() + left_offset;

    float right_offset = helper_symbol.get_width()* 0.03;
    symbol_right = helper_symbol.right() - right_offset;

    float top_offset = helper_symbol.get_height()* 0.92;
    symbol_top = helper_symbol.top() - top_offset;

    symbol_bottom = symbol_top - (symbol_right - symbol_left);

    t_bound_box symbol(symbol_left, symbol_bottom, symbol_right, symbol_top);
    //setcolor(BLACK);
    //fillrect(symbol);
    float x_offset = helper_symbol.get_width()*0.0245;
    float y_offset = helper_symbol.get_width()*0.0247;

    float orignal_x = (symbol_left + symbol_right) / 2;
    float orignal_y = (symbol_top + symbol_bottom) / 2;
    Surface help_symbol("help_hit.png");
    draw_surface(help_symbol, orignal_x - x_offset, orignal_y + y_offset);
}

void act_on_delivery(void (*drawscreen_ptr)(void)) {
    courier_text_version();
}

void courier_interface() {

}

vector<unsigned> get_intersection_id(string input) {
    string lower_case_input;
    for (unsigned i = 0; i < input.size(); i++) {
        lower_case_input.push_back(tolower(input[i]));
    }
    input = lower_case_input;
    vector<unsigned> interid;
    auto iter = input.find("and"); //find the And in input
    auto iter2 = input.find("&");
    if (iter != string::npos && iter2 == string::npos) {
        string temp_input = input;
        temp_input.erase(iter);
        string streetname1 = temp_input;
        streetname1.pop_back();
        temp_input = input;
        string reversed(temp_input.rbegin(), temp_input.rend());
        auto iter3 = reversed.find("dna");
        temp_input = reversed.erase(iter3);
        if (temp_input != "") {
            temp_input.pop_back();
            string streetname2(temp_input.rbegin(), temp_input.rend());
            if (globalBase->check_street_existing(streetname1) && globalBase->check_street_existing(streetname2)) {
                std::vector<unsigned> intersectionBetweenTwoStreet = globalBase->find_intersection_ids_from_street_names_in_DB(streetname1, streetname2);
                if (intersectionBetweenTwoStreet.size() == 0) {//if the intersection is not found
                    interid = {};
                } else {
                    interid = intersectionBetweenTwoStreet;
                }

            } else {
                streetname1 = fuzzy_search_streets(streetname1);
                streetname2 = fuzzy_search_streets(streetname2);
                std::vector<unsigned> intersectionBetweenTwoStreet = globalBase->find_intersection_ids_from_street_names_in_DB(streetname1, streetname2);
                if (intersectionBetweenTwoStreet.size() == 0) {//if the intersection is not found
                    interid = {};
                } else {
                    interid = intersectionBetweenTwoStreet;
                }
            }
        } else {
            interid = {};
        }
    } else if (iter == string::npos && iter2 != string::npos) {
        string temp_input = input;
        temp_input.erase(iter2);
        string streetname1 = temp_input;
        streetname1.pop_back();
        temp_input = input;
        string reversed(temp_input.rbegin(), temp_input.rend());
        auto iter3 = reversed.find("&");
        temp_input = reversed.erase(iter3);
        if (temp_input != "") {
            temp_input.pop_back();
            string streetname2(temp_input.rbegin(), temp_input.rend());

            if (globalBase->check_street_existing(streetname1) && globalBase->check_street_existing(streetname2)) {
                std::vector<unsigned> intersectionBetweenTwoStreet = globalBase->find_intersection_ids_from_street_names_in_DB(streetname1, streetname2);
                if (intersectionBetweenTwoStreet.size() == 0) {//if the intersection is not found
                    interid = {};
                } else {
                    interid = intersectionBetweenTwoStreet;
                }
            } else {
                streetname1 = fuzzy_search_streets(streetname1);
                streetname2 = fuzzy_search_streets(streetname2);
                std::vector<unsigned> intersectionBetweenTwoStreet = globalBase->find_intersection_ids_from_street_names_in_DB(streetname1, streetname2);
                if (intersectionBetweenTwoStreet.size() == 0) {//if the intersection is not found
                    interid = {};
                } else {
                    interid = intersectionBetweenTwoStreet;
                }
            }
        } else {
            interid = {};
        }
    } else {
        interid = {};
    }

    return interid;
}

void courier_text_version() {
    cout << "Commands" << endl;
    cout << "1. enter depots" << endl;
    cout << "2. enter pickups & dropoffs" << endl;
    cout << "3. enter existing information" << endl;
    cout << "4. find courier route" << endl;
    cout << "5. clear all"<<endl;
    cout << "6. exit"<<endl;
    cout << "Please select a command (using number) : ";
    string selection;
    getline(cin, selection);
    stringstream ss(selection);
    string input_content;
    vector<unsigned> temp_id;
    int selectnumber;
    ss>>selectnumber;
    if (selectnumber == 1) {
        cout << "Please enter the name of the intersection: " << endl;
        getline(cin, input_content);
        temp_id = get_intersection_id(input_content);
        while (temp_id.size() == 0) {
            cout << "Can't find the intersection. Please re-enter: " << endl;
            getline(cin, input_content);
            temp_id = get_intersection_id(input_content);
        }
        highlight_depots.insert(highlight_depots.end(), temp_id.begin(), temp_id.end());
        cout << "Success!" <<endl<< "Depot at " << globalBase->get_intersection_name(temp_id[0]) << endl;
        courier_text_version();
    } else if (selectnumber == 2) {
        cout << "Please enter the name of the pick-up intersection: " << endl;
        getline(cin, input_content);
        vector<unsigned> pickup_temp;
        vector<unsigned> dropoff_temp;
        pickup_temp = get_intersection_id(input_content);
        while (pickup_temp.size() == 0) {
            cout << "Can't find the intersection. Please re-enter: " << endl;
            getline(cin, input_content);
            pickup_temp = get_intersection_id(input_content);
        }
        highlight_pickUps.insert(highlight_pickUps.end(), pickup_temp.begin(), pickup_temp.end());
        cout << "Success!" << endl << "Pick up at " << globalBase->get_intersection_name(pickup_temp[0]) << endl;
        cout << "Please enter the name of the drop-off intersection: " << endl;
        getline(cin, input_content);
        dropoff_temp = get_intersection_id(input_content);
        while (dropoff_temp.size() == 0) {
            cout << "Can't find the intersection. Please re-enter: " << endl;
            getline(cin, input_content);
            dropoff_temp = get_intersection_id(input_content);
        }
        highlight_dropOffs.insert(highlight_dropOffs.end(), dropoff_temp.begin(), dropoff_temp.end());
        cout << "Success!" << endl << "Drop off at " << globalBase->get_intersection_name(dropoff_temp[0]) << endl;
        DeliveryInfo temp_info(pickup_temp[0], dropoff_temp[0]);
        deliveries_info.push_back(temp_info);
        courier_text_version();
    } else if (selectnumber == 3) {
        cout << endl;
        cout << "Depots: " << endl;
        for (unsigned i = 0; i < highlight_depots.size(); i++) {
            cout << globalBase->get_intersection_name(highlight_depots[i]) << endl;
        }
        cout << "Pick Up & Drop Offs:" << endl;
        for (unsigned i = 0; i < deliveries_info.size(); i++) {
            cout << "Pick up at: " << globalBase->get_intersection_name(deliveries_info[i].pickUp) << endl;
            cout << "Drop off at: " << globalBase->get_intersection_name(deliveries_info[i].dropOff) << endl;
        }
        cout << endl;
        courier_text_version();
    } else if (selectnumber == 4) {
        if (highlight_pickUps.size() != 0 && highlight_dropOffs.size() != 0 && highlight_depots.size() != 0) {
            vector<unsigned> path;
            path = traveling_courier(deliveries_info, highlight_depots, -1);
            highlight_path = {};
            highlight_path = {};
            highlight_inters = {};
            highlight_pois = {};
            highlight_path = path;
            highlight_inters.insert(highlight_inters.end(), highlight_pickUps.begin(), highlight_pickUps.end());
            highlight_inters.insert(highlight_inters.end(), highlight_dropOffs.begin(), highlight_dropOffs.end());
            cout << "Path found :)" << endl;
        }
        else {
            cout << "Not enough information provided" << endl;
            courier_text_version();
        }
    }else if(selectnumber==5){
        highlight_pickUps={};
        highlight_dropOffs={};
        highlight_depots={};
        cout<<"Success!"<<endl<<"All information has been cleared"<<endl;
        courier_text_version();
    } else if(selectnumber==6){
        cout<<"Now exit"<<endl;
    }
    else {
        cout << "Invalid selection!" << endl;
        courier_text_version();
    }
}