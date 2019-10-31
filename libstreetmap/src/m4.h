/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   m4.h
 * Author: hufeiran
 *
 * Created on March 27, 2017, 10:18 AM
 */

#ifndef M4_H
#define M4_H
#pragma once
#include <vector>
#include "m3.h"
#include "streetDB.h"
#include <assert.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/foreach.hpp>
//we use rtree to quickly find nearest point
using namespace std;

struct DeliveryInfo {

    DeliveryInfo(unsigned pick_up, unsigned drop_off)
    : pickUp(pick_up), dropOff(drop_off) {
    }

    //intersection id where item-to-be-delivered is picked up
    unsigned pickUp;

    //intersection id where item-to-be-delivered is dropped off
    unsigned dropOff;
};
struct place_pluse_times{
        unsigned id;
        int appear_time;
    };
//rtree implementation
//define point(lat,lon)
    typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> coordinate;
//values of pair for POI
typedef std::pair<coordinate, unsigned> pickup_value;
typedef std::pair<coordinate,unsigned> depot_value;
//first implement heuristic 1 algorithm
//all locations are intersections
vector<unsigned> traveling_courier(const vector<DeliveryInfo> & deliveries,
        const vector<unsigned> & depots, const float turn_penalty);

DeliveryInfo find_nearest_pickup(unsigned depot, vector<DeliveryInfo> & DeliveryInfos);
unsigned find_nearest_depot(vector<unsigned> depots, unsigned last_drop);
bool vectorInOrderOrNot(const vector<place_pluse_times> a);
bool idExistingInVector(unsigned id,const vector<place_pluse_times>v);
unsigned find_nearest_valid_point(vector<unsigned> valid_inters, unsigned cur_inter);
vector<place_pluse_times> sortVectorBasedOnTime( const vector<place_pluse_times> inputList);
#endif /* M4_H */
