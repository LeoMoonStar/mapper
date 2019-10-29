/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   streetDB.cpp
 * Author: hufeiran
 *
 * Created on January 26, 2017, 5:10 PM
 */
//constexpr double EARTH_RADIUS_IN_METERS = 6372797.560856;
//constexpr double DEG_TO_RAD = 0.017453292519943295769236907684886;

#include "streetDB.h"
#include "m1.h"
#include "time.h"

using namespace std;

streetDB::streetDB() {

    streetNumbers = getNumberOfStreets();
    segmentNumbers = getNumberOfStreetSegments();
    intersectionNumbers = getNumberOfIntersections();
    pointOfInterestNumbers = getNumberOfPointsOfInterest();
    NodeNumbers = getNumberOfNodes();
    WayNumbers = getNumberOfWays();
    featureNumbers = getNumberOfFeatures();
    //insert intersections
    for (unsigned inters_id = 0; inters_id < intersectionNumbers; ++inters_id) {
        LatLon pos = getIntersectionPosition(inters_id);
        intersection_id_to_latlon.push_back(pos);
        point new_ptr;
        new_ptr.set<0>(pos.lat());
        new_ptr.set<1>(pos.lon());
        intersection_rtree.insert(std::make_pair(new_ptr, inters_id));
    }

    //find  average lat of map
    double max = intersection_id_to_latlon[0].lat();
    double min = max;
    double temp_lat;
    for (unsigned i = 0; i < intersectionNumbers; i++) {
        temp_lat = intersection_id_to_latlon[i].lat();
        if (temp_lat > max)
            max = temp_lat;
        if (temp_lat < min)
            min = temp_lat;
    }
    average_lat = (max + min) / 2 * DEG_TO_RAD;
    double cosine_average_lat = cos(average_lat);
    for (unsigned POI_id = 0; POI_id < pointOfInterestNumbers; ++POI_id) {
        LatLon pos = getPointOfInterestPosition(POI_id);
        point new_ptr;
        t_point temp_t = latlon_to_t_point(pos);
        new_ptr.set<0>(pos.lat());
        new_ptr.set<1>(pos.lon());
        string name = getPointOfInterestName(POI_id);
        POI_rtree.insert(std::make_pair(new_ptr, POI_id));
        string type = getPointOfInterestType(POI_id);
        POI_id_to_type.push_back(type);
        POI_id_to_t_point.push_back(temp_t);
        POI_id_to_name.push_back(name);
    }

    //implement method street_name_to_street_id
    //unsigned map key = street name, map to a vector of street ids
    for (unsigned i = 0; i < streetNumbers; ++i) {
        string name = getStreetName(i);
        street_id_to_name.push_back(name);
        auto found = street_name_to_street_id.find(name);
        if (found == street_name_to_street_id.end()) { //not inserted before
            vector<unsigned> temp;
            temp.push_back(i);
            pair<string, vector<unsigned>> ins(name, temp);
            street_name_to_street_id.insert(ins);
        } else {
            found -> second.push_back(i);
        }
    }
    //initialize intersection_id_to_adjacent_intersection_id
    // store vector<vector<unsigned>> intersection_id_to_street_segment_id;
    for (unsigned intersection_index = 0; intersection_index < intersectionNumbers; ++intersection_index) {
        unsigned Nss = getIntersectionStreetSegmentCount(intersection_index);
        vector<unsigned> street_seg;
        vector<unsigned> inters_id;
        for (unsigned segment_at_intersec = 0; segment_at_intersec < Nss; ++segment_at_intersec) {
            unsigned temp = getIntersectionStreetSegment(intersection_index, segment_at_intersec);
            street_seg.push_back(temp);
            if (!getStreetSegmentInfo(temp).oneWay) {
                if (getStreetSegmentInfo(temp).from == intersection_index) {
                    inters_id.push_back(getStreetSegmentInfo(temp).to);
                } else {
                    inters_id.push_back(getStreetSegmentInfo(temp).from);
                }
            } else {
                if (getStreetSegmentInfo(temp).from == intersection_index) {
                    inters_id.push_back(getStreetSegmentInfo(temp).to);
                }
            }
        }
        std::sort(inters_id.begin(), inters_id.end());
        vector<unsigned>::iterator it;
        it = unique(inters_id.begin(), inters_id.end());
        inters_id.resize(distance(inters_id.begin(), it));

        intersection_id_to_street_segment_id.push_back(street_seg);     //dont care oneway
        intersection_id_to_adjacent_intersection_id.push_back(inters_id);   //care one way
    }
    //initialize street_id_to_segment_id as zero vector
    vector<unsigned> zero = {};
    for (unsigned i = 0; i < streetNumbers; ++i) {
        street_id_to_segment_id.push_back(zero);
    }

    //store from street segment id to segment info
    //store segment_id to curve point pos
    for (unsigned i = 0; i < segmentNumbers; ++i) {
        vector<LatLon> temp_to_curve_pos;
        vector<t_point> temp_to_t_point;
        StreetSegmentInfo temp = getStreetSegmentInfo(i);
        segment_id_to_segment_info.push_back(temp);
        unsigned str_id = temp.streetID;
        street_id_to_segment_id[str_id].push_back(i);
        unsigned curvepoint_count = temp.curvePointCount;
        if (curvepoint_count != 0) {
            for (unsigned j = 0; j < curvepoint_count; ++j) {
                LatLon temp_latlon = getStreetSegmentCurvePoint(i, j);
                t_point temp_t;
                temp_t = latlon_to_t_point(temp_latlon);
                temp_to_t_point.push_back(temp_t);
                temp_to_curve_pos.push_back(temp_latlon);
            }
        }
        segment_id_to_curve_point_pos.push_back(temp_to_curve_pos);
        segment_id_to_curve_point_t_point_pos.push_back(temp_to_t_point);
    }
    //store intersection id from street id
    for (unsigned streetcount = 0; streetcount < streetNumbers; ++streetcount) {
        unsigned segments_size = street_id_to_segment_id[streetcount].size();
        vector <unsigned> tempvec;
        for (unsigned segmentscount = 0; segmentscount < segments_size; ++segmentscount) {
            unsigned tempId = street_id_to_segment_id[streetcount][segmentscount];
            StreetSegmentInfo temp = segment_id_to_segment_info[tempId];
            tempvec.push_back(temp.from);
            tempvec.push_back(temp.to);
        }
        std::sort(tempvec.begin(), tempvec.end());
        vector<unsigned>::iterator it;
        it = unique(tempvec.begin(), tempvec.end());
        tempvec.resize(distance(tempvec.begin(), it));
        street_id_to_intersection_id.push_back(tempvec);
    }
    //store vector<double> segment_id_to_segment_length
    //store vector<double> street_segment_id_to_travel_time
    for (unsigned segment_index = 0; segment_index < segmentNumbers; ++segment_index) {
        StreetSegmentInfo temp = segment_id_to_segment_info[segment_index];
        unsigned curvePointCount = temp.curvePointCount;
        double segment_length = 0;
        LatLon l1 = getIntersectionPosition(temp.from);
        LatLon l2 = getIntersectionPosition(temp.to);
        if (curvePointCount == 0)
            segment_length = find_distance_between_two_points(l1, l2);
        else {
            LatLon next_pos;
            vector<LatLon> temp_vec = segment_id_to_curve_point_pos[segment_index];
            LatLon present_pos = l1;
            //fist calculate the length between starting point and first curve point and last curve point and ending point
            //find_distance_between_two_points(l2, temp_vec[curvePointCount-1]); //length between starting point and last curve point
            for (unsigned i = 0; i < curvePointCount; ++i) { //i is the ith curve point and reason for count-1 is that for 3 points only need to count to 2 for distance between 2 and 2+1
                next_pos = temp_vec[i];
                segment_length += find_distance_between_two_points(present_pos, next_pos);
                present_pos = next_pos;
            }
            segment_length += find_distance_between_two_points(l2, temp_vec[curvePointCount - 1]);
        }
        segment_id_to_segment_length.push_back(segment_length);
        double speedLimit = segment_id_to_segment_info[segment_index].speedLimit;
        double travel_time = 60 * 60 * segment_id_to_segment_length[segment_index] / (1000 * speedLimit);
        street_segment_id_to_travel_time.push_back(travel_time);
    }


    //find max x,y and min x,y on the map
    max_x = intersection_id_to_latlon[0].lon() * cosine_average_lat;
    max_y = intersection_id_to_latlon[0].lat();
    min_x = max_x;
    min_y = max_y;
    for (unsigned i = 0; i < intersectionNumbers; ++i) {
        t_point temp;
        temp.x = intersection_id_to_latlon[i].lon() * cosine_average_lat;
        temp.y = intersection_id_to_latlon[i].lat();
        intersection_id_to_t_point.push_back(temp);
        if (temp.x > max_x) {
            max_x = temp.x;
        }
        if (temp.x < min_x) {
            min_x = temp.x;
        }
        if (temp.y > max_y) {
            max_y = temp.y;
        }
        if (temp.y < min_y) {
            min_y = temp.y;
        }
    }

    //vector<cons OSMWay*> osm_index_to_way
    for (unsigned i = 0; i < WayNumbers; i++) {
        const OSMWay* current_way = getWayByIndex(i);
        osm_index_to_way.push_back(current_way);
        unsigned tagcount = getTagCount(current_way);
        unordered_map<string, string> temp_tag;
        for (unsigned j = 0; j < tagcount; j++) {
            temp_tag.insert(getTagPair(current_way, j));
        }
        entity_to_tag.insert(make_pair(current_way, temp_tag));
        OSMID osm_id = current_way->id();
        osm_id_to_entity.insert(make_pair(osm_id, current_way));
    }





    //divide street segments into different categories
    for (unsigned i = 0; i < segmentNumbers; ++i) {
        StreetSegmentInfo current_seg = segment_id_to_segment_info[i];
        OSMID osm_id = current_seg.wayOSMID;
        const OSMEntity * current_osm = osm_id_to_entity.find(osm_id)->second;
        unordered_map<string, string> tag_list;
        tag_list = entity_to_tag.find(current_osm)->second;
        auto iter = tag_list.find("highway");
        auto iter2 = tag_list.find("cycleway");
        if (iter != tag_list.end()) {
            if (iter->second == "motorway") {
                highway.push_back(i);
            } else if (iter->second == "secondary" || iter->second == "trunk" || iter->second == "primary" || iter->second == "tertiary") {
                avenue.push_back(i);
            } else if (iter->second == "secondary_link" || iter->second == "trunk_link" || iter->second == "primary_link" || iter->second == "tertiary_link"
                    || iter->second == "motorway_link") {
                link.push_back(i);
            } else {
                alleys.push_back(i);
                if (iter2 != tag_list.end()) {
                    if (iter2->second != "no") {
                        cycleways.push_back(i);
                    }
                }
            }
        }
    }

    //insert features
    for (unsigned feature_id = 0; feature_id < featureNumbers; feature_id++) {
        string f_name = getFeatureName(feature_id);
        FeatureType f_type = getFeatureType(feature_id);
        TypedOSMID f_osm = getFeatureOSMID(feature_id);
        unsigned pointCount = getFeaturePointCount(feature_id);
        vector<t_point> list_of_t_point;
        for (unsigned count = 0; count < pointCount; count++) {
            LatLon l_temp = getFeaturePoint(feature_id, count);
            t_point t_temp = latlon_to_t_point(l_temp);
            list_of_t_point.push_back(t_temp);
        }
        feature_id_to_feature_name.push_back(f_name);
        feature_id_to_feature_osm.push_back(f_osm);
        feature_id_to_feature_point_count.push_back(pointCount);
        feature_id_to_bound_points.push_back(list_of_t_point);
        feature_id_to_feature_type.push_back(f_type);
        if (f_type == Unknown) {
            Unknown_type_ids.push_back(feature_id);
        } else if (f_type == Park) {
            Park_type_ids.push_back(feature_id);
        } else if (f_type == Beach) {
            Beach_type_ids.push_back(feature_id);
        } else if (f_type == Lake) {
            Lake_type_ids.push_back(feature_id);
        } else if (f_type == River) {
            River_type_ids.push_back(feature_id);
        } else if (f_type == Island) {
            Island_type_ids.push_back(feature_id);
        } else if (f_type == Shoreline) {
            Shoreline_type_ids.push_back(feature_id);
        } else if (f_type == Building) {
            Building_type_ids.push_back(feature_id);
        } else if (f_type == Greenspace) {
            Greenspace_type_ids.push_back(feature_id);
        } else if (f_type == Golfcourse) {
            Golfcourse_type_ids.push_back(feature_id);
        } else {
            Stream_type_ids.push_back(feature_id);
        }
    }


    //    //create name_database
    //    //name_database

    for (unsigned i = 0; i < streetNumbers; i++) {
        string streetname = street_id_to_name[i];
        stringstream ss(streetname);
        vector<string> temp;
        string temp_string;
        ss>>temp_string;
        while (!ss.fail()) {
            for(int j=0;j<temp_string.size();j++){
                temp_string[j]=tolower(temp_string[j]);
            }
            temp.push_back(temp_string);
            ss>>temp_string;
        }
        nameBase.push_back(temp);
    }

    for (unsigned i = 0; i < pointOfInterestNumbers; i++) {
        string poiname = POI_id_to_name[i];
        stringstream ss(poiname);
        vector<string> temp;
        string temp_string;
        ss>>temp_string;
        while (!ss.fail()) {
              for(int j=0;j<temp_string.size();j++){
                temp_string[j]=tolower(temp_string[j]);
            }
            temp.push_back(temp_string);
            ss>>temp_string;
        }
        nameBase.push_back(temp);
    }
      for (unsigned i = 0; i < featureNumbers; i++) {
        string featurename = feature_id_to_feature_name[i];
        stringstream ss(featurename);
        vector<string> temp;
        string temp_string;
        ss>>temp_string;
        while (!ss.fail()) {
              for(int j=0;j<temp_string.size();j++){
                temp_string[j]=tolower(temp_string[j]);
            }
            temp.push_back(temp_string);
            ss>>temp_string;
        }
        nameBase.push_back(temp);
    }
nameNumbers=nameBase.size();

}

//Destructor

streetDB::~streetDB() {
}

//Returns street id(s) for the given street name
//If no street with this name exists, returns a 0-length vector.

vector<unsigned> streetDB::find_street_ids_from_name_in_DB(string street_name) {
    //Returns street id(s) for the given street name
    //If no street with this name exists, returns a 0-length vector.
    auto found = street_name_to_street_id.find(street_name);
    if (found == street_name_to_street_id.end()) {
        vector<unsigned> nothing;
        return nothing;
    } else {
        return found -> second;
    }
}

//Returns the street segments for the given intersection

vector<unsigned> streetDB::find_intersection_street_segments_in_DB(unsigned intersection_id) {
    //Returns the street segments for the given intersection
    vector<unsigned> emp = {};
    if (intersection_id >= intersectionNumbers)
        return emp;
    return intersection_id_to_street_segment_id[intersection_id];
}


//Returns the street names at the given intersection (includes duplicate street names in returned vector)
// intersection id to street segments

vector<string> streetDB::find_intersection_street_names_in_DB(unsigned intersection_id) {

    //street segments id to street name
    vector<string> emp = {};
    if (intersection_id >= intersectionNumbers)
        return emp;
    vector<string> street_name_list;
    vector<unsigned> segment_id;
    unsigned size = intersection_id_to_street_segment_id[intersection_id].size();
    for (unsigned i = 0; i < size; i++) {
        unsigned segment_index = intersection_id_to_street_segment_id[intersection_id].at(i);
        StreetSegmentInfo temp = segment_id_to_segment_info[segment_index];
        unsigned street_id = temp.streetID;
        street_name_list.push_back(street_id_to_name[street_id]);
    }
    return street_name_list;
}

//Returns true if you can get from intersection1 to intersection2 using a single street segment (hint: check for 1-way streets too)
//corner case: an intersection is considered to be connected to itself

bool streetDB::are_directly_connected_in_DB(unsigned intersection_id1, unsigned intersection_id2) {

    unsigned size = (intersection_id_to_adjacent_intersection_id.at(intersection_id1)).size();
    for (unsigned i = 0; i < size; i++) {
        if (intersection_id_to_adjacent_intersection_id.at(intersection_id1).at(i) == intersection_id2)
            return true;
    }
    return false;
}

//Returns all street segments for the given street

vector<unsigned> streetDB::find_adjacent_intersections_in_DB(unsigned intersection_id) {

    return intersection_id_to_adjacent_intersection_id[intersection_id];
}

//Returns all street segments for the given street

vector<unsigned> streetDB::find_street_street_segments_in_DB(unsigned street_id) {
    return street_id_to_segment_id[street_id];
}

//Returns all intersections along the a given street

vector<unsigned> streetDB::find_all_street_intersections_in_DB(unsigned street_id) {

    return street_id_to_intersection_id[street_id];
}

//Returns the length of the given street segment in meters

double streetDB::find_street_segment_length_in_DB(unsigned street_segment_id) {
    if (street_segment_id >= segmentNumbers)
        return 0;
    return segment_id_to_segment_length[street_segment_id];
}

//Returns distance between two points

double streetDB::find_distance_between_two_points(LatLon point1, LatLon point2) {
    double x1, x2, y1, y2;
    y1 = DEG_TO_RAD * point1.lat(); //convert position from degree to rad
    y2 = DEG_TO_RAD * point2.lat();
    double cosine = cos((y1 + y2) / 2);
    x1 = DEG_TO_RAD * point1.lon() * cosine; //convert (lon,lat) to (x,y)
    x2 = DEG_TO_RAD * point2.lon() * cosine;
    return EARTH_RADIUS_IN_METERS * sqrt((y2 - y1)*(y2 - y1)+(x2 - x1)*(x2 - x1));
}

//Returns the nearest point of interest to the given position

unsigned streetDB::find_closest_point_of_interest_in_DB(LatLon my_position) {
    // find 10 nearest value to a point
    // using  rtree structure
    std::vector<POI_value> result_n;
    point good_point;
    good_point.set<0>(my_position.lat());
    good_point.set<1>(my_position.lon());
    POI_rtree.query(bgi::nearest(good_point, 10), std::back_inserter(result_n));
    POI_value temp_value = result_n[0];
    unsigned closest_id = temp_value.second;
    double x = temp_value.first.get<0>();
    double y = temp_value.first.get<1>();
    double closest_distance = find_distance_between_two_points(LatLon(x, y), my_position);
    for (unsigned i = 1; i < result_n.size(); i++) {
        temp_value = result_n[i];
        x = temp_value.first.get<0>();
        y = temp_value.first.get<1>();
        double temp_distance = find_distance_between_two_points(LatLon(x, y), my_position);
        if (temp_distance < closest_distance) {
            closest_id = temp_value.second;
            closest_distance = temp_distance;
        }
    }
    return closest_id;
}

//Returns the the nearest intersection to the given position

unsigned streetDB::find_closest_intersection_in_DB(LatLon my_position) {
    // find 20 nearest value to a point
    // using rtree structure
    std::vector<intersection_value> result_n;
    point good_point;
    good_point.set<0>(my_position.lat());
    good_point.set<1>(my_position.lon());
    intersection_rtree.query(bgi::nearest(good_point, 20), std::back_inserter(result_n));
    intersection_value temp_value = result_n[0];
    unsigned closest_id = temp_value.second;
    double x = temp_value.first.get<0>();
    double y = temp_value.first.get<1>();
    double closest_distance = find_distance_between_two_points(LatLon(x, y), my_position);
    for (unsigned i = 1; i < result_n.size(); i++) {
        temp_value = result_n[i];
        x = temp_value.first.get<0>();
        y = temp_value.first.get<1>();
        double temp_distance = find_distance_between_two_points(LatLon(x, y), my_position);
        if (temp_distance < closest_distance) {
            closest_id = temp_value.second;
            closest_distance = temp_distance;
        }
    }
    return closest_id;
}

//Returns the length of the specified street in meters

double streetDB::find_street_length_in_DB(unsigned street_id) {

    float sum = 0;
    for (unsigned segment_id = 0; segment_id < street_id_to_segment_id[street_id].size(); segment_id++) {
        sum += segment_id_to_segment_length[ street_id_to_segment_id[street_id][segment_id]];
    }
    return sum;
}

//Returns the travel time to drive a street segment in seconds (time = distance/speed_limit)

double streetDB::find_street_segment_travel_time_in_DB(unsigned street_segment_id) {
    //Returns all intersections along the a given street
    return (street_segment_id_to_travel_time[street_segment_id]);
}

//Return all intersection ids for two intersecting streets
//This function will typically return one intersection id.
//However street names are not guarenteed to be unique, so more than 1 intersection id may exist

vector<unsigned> streetDB::find_intersection_ids_from_street_names_in_DB(string street_name1, string street_name2) {
    // street name to id
    // street id to segments id
    //segments id to segments info
    //segments info to intersection id
    vector<unsigned> inters_id;
    vector<unsigned> street_id1;
    vector<unsigned> street_id2;
    auto find1 = street_name_to_street_id.find(street_name1);
    auto find2 = street_name_to_street_id.find(street_name2);
    if (find1 == street_name_to_street_id.end()) {
        return inters_id;
    } else if (find2 == street_name_to_street_id.end()) {
        return inters_id;

    } else {
        for (unsigned i = 0; i < (find1 -> second.size()); i++) {
            street_id1.push_back(find1 -> second.at(i));
        }
        for (unsigned i = 0; i < (find2 -> second.size()); i++) {
            street_id2.push_back(find2 -> second.at(i));
        }
    }
    for (unsigned i = 0; i < street_id1.size(); i++) {
        vector<unsigned> inters1;
        inters1.resize(street_id_to_intersection_id.at(street_id1.at(i)).size());
        inters1 = street_id_to_intersection_id.at(street_id1.at(i));
        vector<unsigned> temp;
        for (unsigned j = 0; j < street_id2.size(); j++) {
            vector<unsigned> inters2;
            inters2.resize(street_id_to_intersection_id.at(street_id2.at(j)).size());
            inters2 = street_id_to_intersection_id.at(street_id2.at(j));
            set_intersection(inters1.begin(), inters1.end(), inters2.begin(), inters2.end(), back_inserter(temp));
            for (unsigned k = 0; k < temp.size(); k++) {
                inters_id.push_back(temp.at(k));
            }
        }
    }
    std::sort(inters_id.begin(), inters_id.end());
    vector<unsigned>::iterator it;
    it = unique(inters_id.begin(), inters_id.end());
    inters_id.resize(distance(inters_id.begin(), it));
    return inters_id;
}
// find all street segments and sum up their length

//get number of all street segments

unsigned streetDB::get_segmentNumbers() {
    return segmentNumbers;
}

//get info of a certain segment according to its id

StreetSegmentInfo streetDB::get_segment_info(unsigned index) {
    return segment_id_to_segment_info[index];
}

//get xy_coordinates of an intersection according to its id

t_point streetDB::get_t_point_intersection_pos(unsigned inter_index) {
    return intersection_id_to_t_point[inter_index];
}

//get xy_coordinates of all curve points according to its id

vector<t_point> streetDB::get_curve_points_in_t_point(unsigned index) {
    return segment_id_to_curve_point_t_point_pos[index];
}

//get four boundaries of the map in a vector in the order of bottom, left, right, top

vector<double> streetDB::get_map_limits() {
    vector<double> temp;
    temp.push_back(min_x);
    temp.push_back(min_y);
    temp.push_back(max_x);
    temp.push_back(max_y);
    return temp;
}

//get highway

vector<unsigned> streetDB::get_highway() {
    return highway;
}

//get avenue

vector<unsigned> streetDB::get_avenue() {
    return avenue;
}

//get link

vector<unsigned> streetDB::get_link() {
    return link;
}

//get allies

vector<unsigned> streetDB::get_alleys() {
    return alleys;
}

//get cycleways

vector<unsigned> streetDB::get_cycleways() {
    return cycleways;
}

//transform latlon into x y coordinates

t_point streetDB::latlon_to_t_point(LatLon temp) {
    double cosine = cos(average_lat);
    t_point current;
    current.x = temp.lon() * cosine;
    current.y = temp.lat();
    return current;
}

//check whether a feature(get xy_coordinates of the feature boundary points) is a closed feature or not

bool streetDB::check_closed(vector<t_point> points) {
    double start_x = points[0].x;
    double start_y = points[0].y;
    double end_x = points[points.size() - 1].x;
    double end_y = points[points.size() - 1].y;
    if (start_x == end_x && start_y == end_y) {
        return true;
    } else {
        return false;
    }
}

//get vector of ids of feature that in the type of unknown

vector<unsigned> streetDB::get_Unknown_type_ids() {
    return Unknown_type_ids;
}

//get vector of ids of feature that in the type of park

vector<unsigned> streetDB::get_Park_type_ids() {
    return Park_type_ids;
}

//get vector of ids of feature that in the type of beach

vector<unsigned> streetDB::get_Beach_type_ids() {
    return Beach_type_ids;
}

//get vector of ids of feature that in the type of lake

vector<unsigned> streetDB::get_Lake_type_ids() {
    return Lake_type_ids;
}

//get vector of ids of feature that in the type of river

vector<unsigned> streetDB::get_River_type_ids() {
    return River_type_ids;
}

//get vector of ids of feature that in the type of island

vector<unsigned> streetDB::get_Island_type_ids() {
    return Island_type_ids;
}

//get vector of ids of feature that in the type of shoreline

vector<unsigned> streetDB::get_Shoreline_type_ids() {
    return Shoreline_type_ids;
}

//get vector of ids of feature that in the type of building

vector<unsigned> streetDB::get_Building_type_ids() {
    return Building_type_ids;
}

//get vector of ids of feature that in the type of greenspace

vector<unsigned> streetDB::get_Greenspace_type_ids() {
    return Greenspace_type_ids;
}

//get vector of ids of feature that in the type of golfcourse

vector<unsigned> streetDB::get_Golfcourse_type_ids() {
    return Golfcourse_type_ids;
}

//get vector of ids of feature that in the type of stream

vector<unsigned> streetDB::get_Stream_type_ids() {
    return Stream_type_ids;
}

//get boundary points of a feature in a vector according to its id

vector<t_point> streetDB::get_feature_id_to_bound_points(unsigned index) {
    return feature_id_to_bound_points.at(index);
}

//get number of boundary points of a feature in a vector according to its id

unsigned streetDB::get_feature_id_to_feature_point_count(unsigned index) {
    return feature_id_to_feature_point_count[index];
}

//get name of a street in a vector according to its id

string streetDB::find_street_name_from_ids_in_DB(unsigned street_id) {
    return street_id_to_name[street_id];
}

//get t_point of a POI according to its id

t_point streetDB::get_POI_id_to_t_point(unsigned id) {
    return POI_id_to_t_point[id];
}

//transfer t_point of a corresponding latlon value

LatLon streetDB::t_point_to_latlon(t_point temp) {
    LatLon current(temp.y, temp.x / (cos(average_lat)));
    return current;
}

//get name of a feature according to its id

string streetDB::get_feature_id_to_feature_name(unsigned id) {
    return feature_id_to_feature_name[id];
}

//get name of a POI point according to its id

string streetDB::get_POI_name_from_id(unsigned id) {
    return POI_id_to_name[id];
}

//get number of total POIs

unsigned streetDB::POInumber() {
    return pointOfInterestNumbers;
}

//check whether a street name exists on the map

bool streetDB::check_street_existing(std::string streetName) {
    bool result = std::find(street_id_to_name.begin(), street_id_to_name.end(), streetName) != street_id_to_name.end();
    return result;
}


//get intersection name from id

string streetDB::get_intersection_name(unsigned id) {
    vector<string> street_names = find_intersection_street_names_in_DB(id);
    string current = street_names[0];
    string intersection_name = current;
    string temp;
    for (unsigned j = 0; j < street_names.size(); ++j) {
        temp = street_names[j];
        auto iter = intersection_name.find(temp);
        if (iter == string::npos) {
            intersection_name = intersection_name + " and " + temp;
        }
    }
    return intersection_name;

}

//get straight distance between two t_points

float streetDB::find_distance_in_t_point(t_point temp1, t_point temp2) {
    float delta_x = temp2.x - temp1.x;
    float delta_y = temp2.y - temp1.y;
    float distance = sqrt(delta_x * delta_x + delta_y * delta_y);
    return distance;
}

//check whether a POI name exists on the map

bool streetDB::check_POI_existing(std::string POI_Name) {
    return std::find(POI_id_to_name.begin(), POI_id_to_name.end(), POI_Name) != POI_id_to_name.end();
}

//check whether a feature name exists on the map

bool streetDB::check_feature_existing(std::string Feature_Name) {
    return std::find(feature_id_to_feature_name.begin(), feature_id_to_feature_name.end(), Feature_Name) != feature_id_to_feature_name.end();
}

//get a POI id according to the name given
//to make this method functional, the name must be a valid name checked before called

unsigned streetDB::get_POI_id_from_name(string name) {
    auto iter = find(POI_id_to_name.begin(), POI_id_to_name.end(), name);
    return std::distance(POI_id_to_name.begin(), iter);
}

//get the number of total features

unsigned streetDB::get_feature_number() {
    return featureNumbers;
}

//get the feature type of a feature according to its id

FeatureType streetDB::get_feature_id_to_feature_type(unsigned id) {
    return feature_id_to_feature_type[id];
}

//get POI type according to its id

string streetDB::get_POI_type(unsigned id) {
    return POI_id_to_type[id];
}

//get feature id according to the name given

unsigned streetDB::get_feature_id_from_name(string name) {
    auto iter = find(feature_id_to_feature_name.begin(), feature_id_to_feature_name.end(), name);
    return std::distance(feature_id_to_feature_name.begin(), iter);
}

//return the position of pin of a feature according to its id

t_point streetDB::get_feature_average_pos(unsigned id) {
    vector<t_point> all_pos = feature_id_to_bound_points.at(id);
    int num_of_pos = all_pos.size();
    double total_x = 0, total_y = 0;
    for (unsigned i = 0; i < num_of_pos; i++) { //add all x together and y together
        total_x += all_pos[i].x;
        total_y += all_pos[i].y;
    }
    t_point ave_pos(total_x / num_of_pos, total_y / num_of_pos); //get an average value of all xs and ys
    return ave_pos;
}

////return name database of street

vector<vector<string>> streetDB::get_name_base() {
    return nameBase;
}

//get total street number

unsigned streetDB::get_streetNumbers() {
    return streetNumbers;
}

unsigned streetDB::get_nameNumbers(){
    return nameNumbers;
}

unsigned streetDB::get_segment_between_inters(unsigned inters_id1, unsigned inters_id2) {
    unsigned i = 0;
    for (i = 0; i < intersection_id_to_street_segment_id[inters_id1].size(); i++) {
        unsigned segment_id = intersection_id_to_street_segment_id[inters_id1][i];
        if (segment_id_to_segment_info[segment_id].from == inters_id1) {
            if (segment_id_to_segment_info[segment_id].to == inters_id2) {
                return segment_id;
            }
        }
        if (segment_id_to_segment_info[segment_id].from == inters_id2) {
            if (segment_id_to_segment_info[segment_id].to == inters_id1) {
                return segment_id;
            }
        }
    }
}

t_point streetDB::find_midpoint_of_two_inters(unsigned inters_id1, unsigned inters_id2) {
    double x1 = intersection_id_to_t_point[inters_id1].x;
    double x2 = intersection_id_to_t_point[inters_id2].x;
    double y1 = intersection_id_to_t_point[inters_id1].y;
    double y2 = intersection_id_to_t_point[inters_id2].y;
    double xavg = (x1 + x2) / 2;
    double yavg = (y1 + y2) / 2;
    t_point ans(xavg, yavg);
    return ans;
}

bool streetDB::check_segment_on_same_street(unsigned seg_id1, unsigned seg_id2){
    unsigned str1 = get_segment_info(seg_id1).streetID;
    unsigned str2 = get_segment_info(seg_id2).streetID;
    if (str1 == str2)
        return true;
    return false;
}

unsigned streetDB::get_intersection_number(){
    return intersectionNumbers;
}


string streetDB::find_name_from_ids_in_nameBase(unsigned id){
  if(id<streetNumbers){
      return street_id_to_name[id];
  }  
  else if(id>=streetNumbers&&id<streetNumbers+pointOfInterestNumbers){
      int new_id=id-streetNumbers;
      return POI_id_to_name[new_id];
  }
  else if(id>=streetNumbers+pointOfInterestNumbers&&id<nameNumbers){
      int new_id=id-streetNumbers-pointOfInterestNumbers;
      return feature_id_to_feature_name[new_id];
  }
}

std::vector<unsigned> streetDB::get_all_POI_ids_according_to_name(std::string name){
    vector<unsigned> seg_ids = {};
    for (unsigned i = 0; i < POI_id_to_name.size(); i++){
        if (POI_id_to_name[i] == name)
            seg_ids.push_back(i);
    }
    return seg_ids;
}


 vector <t_point>  streetDB::get_segment_id_to_curve_point_t_point_pos(unsigned segment_id){
     return segment_id_to_curve_point_t_point_pos[segment_id];
}
 
 
 t_point streetDB::find_closest_curve_point_to_an_intersection_in_t_point(unsigned inter_id,unsigned segment_id){
     vector<t_point> segment_curve_points_pos=get_segment_id_to_curve_point_t_point_pos(segment_id);
     t_point inter_pos=get_t_point_intersection_pos(inter_id);
     float minimum_distance=find_distance_in_t_point(inter_pos,segment_curve_points_pos.at(0));
     t_point current_curve_pos=segment_curve_points_pos.at(0);
     float temp_distance;
     t_point temp_curve_pos;
     for(unsigned i=0;i<segment_curve_points_pos.size();i++){
         temp_curve_pos=segment_curve_points_pos[i];
         temp_distance=find_distance_in_t_point(inter_pos,temp_curve_pos);
         if(temp_distance<minimum_distance){
             minimum_distance=temp_distance;
             current_curve_pos=temp_curve_pos;
         }
         
     }
     return current_curve_pos;
 }
 
 LatLon streetDB::get_intersection_pos_in_latlon(unsigned id){
     return intersection_id_to_latlon[id];
 }