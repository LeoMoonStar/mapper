/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "StreetsDatabaseAPI.h"
#include <algorithm>
#include <math.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include<string>
#include <cassert>
#include "global.h"
#include "streetDB.h"
#include "m4.h"
#include <type_traits>
#include <chrono>

using namespace std;
#define TIME_LIMIT 30// m4 time limit

//create rtree for POI
boost::geometry::index::rtree< pickup_value, boost::geometry::index::quadratic < 16 >> pickup_rtree;
boost::geometry::index::rtree< depot_value, boost::geometry::index::quadratic < 16 >> depot_rtree;
vector<t_point> pick_ups_t_point;
vector<t_point> drop_offs_t_point;
vector<t_point> t_point_of_current_inters;
//*********************this page has a lot of red marks but ignore them, this is the result of using rtree***************************//
//very important!!!!!//



//vector<unsigned> traveling_courier(const vector<DeliveryInfo> & deliveries, const vector<unsigned> & depots, const float turn_penalty) {
//    unsigned drop_off_number=deliveries.size();
//    vector<unsigned> traveling_route;
//    //create a backup deliveres
//    vector<DeliveryInfo> temp_deliveryinfo=deliveries;
//    //first insert pick up rtree rtree implementation
//        for (unsigned i = 0; i < deliveries.size(); ++i) {
//        LatLon pos = globalBase->get_intersection_pos_in_latlon(deliveries[i].pickUp);
//        coordinate new_ptr;
//        new_ptr.set<0>(pos.lat());
//        new_ptr.set<1>(pos.lon());
//        pickup_rtree.insert(std::make_pair(new_ptr, deliveries[i].pickUp));
//    }
//    //insert depot rtree rtree implementation
//    for(unsigned i=0;i<depots.size();++i){
//        LatLon pos=globalBase->get_intersection_pos_in_latlon(depots[i]);
//         coordinate new_ptr;
//        new_ptr.set<0>(pos.lat());
//        new_ptr.set<1>(pos.lon());
//        depot_rtree.insert(std::make_pair(new_ptr, depots[i]));
//    }
//    //now here begins the algorithm
//    //go from the zero (or any depot you like) depot
//    srand (time(NULL));
//    int depot_index=rand()%(deliveries.size());
//       unsigned depot=depots[depot_index];
//      DeliveryInfo closest_pickup= find_nearest_pickup(depot,temp_deliveryinfo);
//       //find path_from_depot_to_nearest_pickup
//      traveling_route=find_path_between_intersections(depot,closest_pickup.pickUp, turn_penalty);
//      //while there still exists deliveries
//      while(drop_off_number!=0){
//      //now go to drop off
//      vector<unsigned> drop_off_route;
//       drop_off_route=find_path_between_intersections(closest_pickup.pickUp,closest_pickup.dropOff,turn_penalty);
//       //insert drop_off route into traveling route
//       traveling_route.insert(traveling_route.end(),drop_off_route.begin(),drop_off_route.end());
//       //reduce drop_off number
//       drop_off_number--;
//       //make sure still have pick up
//       if(drop_off_number!=0){
//       //now go to the nearest pick up
//       DeliveryInfo next_closest_pickup=find_nearest_pickup(closest_pickup.dropOff,temp_deliveryinfo);
//       //path from last drop off to nearest pick up
//       vector<unsigned> temp_path;
//       temp_path=find_path_between_intersections(closest_pickup.dropOff,next_closest_pickup.pickUp,turn_penalty);
//       //now closest_pick is current_pick_up
//       closest_pickup=next_closest_pickup;
//       //insert route
//       traveling_route.insert(traveling_route.end(),temp_path.begin(),temp_path.end());
//      }
//      }
//      //now go to nearest depot (end))
//      unsigned closest_depot=find_nearest_depot(depots,closest_pickup.dropOff);
//      //path to nearest depot
//      vector<unsigned> off_work=find_path_between_intersections(closest_depot,closest_pickup.dropOff,turn_penalty);
//      traveling_route.insert(traveling_route.end(),off_work.begin(),off_work.end());
//      return traveling_route;
//}
//
//DeliveryInfo find_nearest_pickup(unsigned depot,vector<DeliveryInfo> & DeliveryInfos){
//   
//  LatLon depot_location=globalBase->get_intersection_pos_in_latlon(depot);
//  //LatLon pickups_location=globalBase->get_intersection_pos_in_latlon(DeliveryInfos[0].pickUp);
// //   float min_distance=globalBase->find_distance_between_two_points(depot_location,pickups_location);
//    float current_distance;
//    unsigned closest_index=0;
//    DeliveryInfo closest_deli=DeliveryInfos[0];
//    //find closest pickup using rtree
//     std::vector<pickup_value> result_n;
//    coordinate good_point;
//    good_point.set<0>(depot_location.lat());
//    good_point.set<1>(depot_location.lon());
//    pickup_rtree.query(bgi::nearest(good_point, 20), std::back_inserter(result_n));
//    while(result_n.size()!=0){
//    pickup_value temp_value = result_n[0];
//    unsigned closest_id = temp_value.second;
//    double x = temp_value.first.get<0>();
//    double y = temp_value.first.get<1>();
//    double closest_distance = find_distance_between_two_points(LatLon(x, y), depot_location);
//    for (unsigned i = 1; i < result_n.size(); i++) {
//        temp_value = result_n[i];
//        x = temp_value.first.get<0>();
//        y = temp_value.first.get<1>();
//        double temp_distance = find_distance_between_two_points(LatLon(x, y), depot_location);
//        if (temp_distance < closest_distance) {
//            closest_id = temp_value.second;
//            closest_distance = temp_distance;
//        }
//    }
//    //closest id is the closest pick up id
//    //remove closest id from rtree
//    LatLon closest_pos=globalBase->get_intersection_pos_in_latlon(closest_id);
//    good_point.set<0>(closest_pos.lat());
//    good_point.set<1>(closest_pos.lon());
//    pickup_rtree.remove(std::make_pair(good_point, closest_id));
//    //then delete the pickup location
//    //reorganize deliveries (delete the first item with the same pick_up)
//    int find_time=0;
//    vector<DeliveryInfo> temp_DeliveryInfos;
//    for(unsigned i=0;i<DeliveryInfos.size();i++){
//        if(DeliveryInfos[i].pickUp==closest_id&find_time==0){
//            find_time++;
//            closest_deli=DeliveryInfos[i];           
//        }else if(DeliveryInfos[i].pickUp==closest_id&&find_time==1){
//            temp_DeliveryInfos.push_back(DeliveryInfos[i]);
//        }else{
//            temp_DeliveryInfos.push_back(DeliveryInfos[i]);
//        }
//    }
//    DeliveryInfos=temp_DeliveryInfos;
//    cout<<"Pick up at "<<globalBase->get_intersection_name(closest_deli.pickUp)<<endl;
//    cout<<"Drop off at "<<globalBase->get_intersection_name(closest_deli.dropOff)<<endl;
//    return closest_deli;
//    }
//}
//
////rtree implementation to find nearest_depot
//unsigned find_nearest_depot(vector<unsigned> depots, unsigned last_drop){
//    unsigned nearest_depot=depots[0];
//  LatLon depot_location=globalBase->get_intersection_pos_in_latlon(depots[0]);
//  LatLon pickups_location=globalBase->get_intersection_pos_in_latlon(last_drop);
//  std::vector<depot_value> result_n;
//    coordinate good_point;
//    good_point.set<0>(pickups_location.lat());
//    good_point.set<1>(pickups_location.lon());
//    depot_rtree.query(bgi::nearest(good_point, 20), std::back_inserter(result_n));
//    while(result_n.size()!=0){
//  depot_value temp_value = result_n[0];
//    unsigned closest_id = temp_value.second;
//    double x = temp_value.first.get<0>();
//    double y = temp_value.first.get<1>();
//    double closest_distance = find_distance_between_two_points(LatLon(x, y), pickups_location);
//    for (unsigned i = 1; i < result_n.size(); i++) {
//        temp_value = result_n[i];
//        x = temp_value.first.get<0>();
//        y = temp_value.first.get<1>();
//        double temp_distance = find_distance_between_two_points(LatLon(x, y), pickups_location);
//        if (temp_distance < closest_distance) {
//            closest_id = temp_value.second;
//            closest_distance = temp_distance;
//        }
//    }
//    return closest_id;
//    }
//}
//algorithm for brian 
vector<unsigned> traveling_courier(const vector<DeliveryInfo> & deliveries,
        const vector<unsigned> & depots, const float turn_penalty) {
    vector<unsigned> points_in_order; //final order of all points
    vector<unsigned> all_pick_ups;
    vector<unsigned> all_drop_offs;
    vector<unsigned> possible_next_points; //save possible next points
    vector<bool> have_picked(deliveries.size(), false); //save whether the position has been picked up
    unsigned points_left = deliveries.size(); //save number of deliveries that has not yet finished
    pick_ups_t_point = {};
    drop_offs_t_point = {};
    t_point_of_current_inters = {};
    for (unsigned i = 0; i < deliveries.size(); i++) {
        all_pick_ups.push_back(deliveries[i].pickUp);
        pick_ups_t_point.push_back(globalBase -> get_t_point_intersection_pos(deliveries[i].pickUp));
        all_drop_offs.push_back(deliveries[i].dropOff);
        drop_offs_t_point.push_back(globalBase -> get_t_point_intersection_pos(deliveries[i].dropOff));
    }
    t_point_of_current_inters = pick_ups_t_point;
    possible_next_points = all_pick_ups;
    srand(time(NULL));
    unsigned start_order_in_list = rand() % (deliveries.size());
    unsigned start_id = all_pick_ups[start_order_in_list];
    points_in_order.push_back(start_id);
    possible_next_points[start_order_in_list] = all_drop_offs[start_order_in_list]; //change the possible pick_up to possible drop_off
    have_picked[start_order_in_list] = true;
    while (points_left != 0) {
        unsigned next_id_in_order = find_nearest_valid_point(possible_next_points, start_id);
        if (have_picked[next_id_in_order]) { //the point is a drop_off
            start_id = all_drop_offs[next_id_in_order];
            all_pick_ups.erase(all_pick_ups.begin() + next_id_in_order);
            all_drop_offs.erase(all_drop_offs.begin() + next_id_in_order);
            pick_ups_t_point.erase(pick_ups_t_point.begin() + next_id_in_order);
            drop_offs_t_point.erase(drop_offs_t_point.begin() + next_id_in_order);
            possible_next_points.erase(possible_next_points.begin() + next_id_in_order);
            t_point_of_current_inters.erase(t_point_of_current_inters.begin() + next_id_in_order);
            have_picked.erase(have_picked.begin() + next_id_in_order);
            points_left--;
        } else { //the point is a pick_up
            start_id = all_pick_ups[next_id_in_order];
            possible_next_points[next_id_in_order] = all_drop_offs[next_id_in_order];
            t_point_of_current_inters[next_id_in_order] = drop_offs_t_point[next_id_in_order];
            have_picked[next_id_in_order] = true;
        }
        points_in_order.push_back(start_id);
    }
    //till now the system is able to return a list of inters in points_in_order
    //now consider depot
    vector<unsigned> final_path = {}; //save the final path
    unsigned first_dep_in_order = find_nearest_valid_point(depots, points_in_order[0]);
    unsigned last_dep_in_order = find_nearest_valid_point(depots, points_in_order[points_in_order.size()-1]);
    unsigned first_dep = depots[first_dep_in_order];
    unsigned last_dep = depots[last_dep_in_order];
    vector<unsigned> first_segs = find_path_between_intersections(first_dep, points_in_order[0], turn_penalty);
    vector<unsigned> last_segs = find_path_between_intersections(points_in_order[points_in_order.size()-1], last_dep, turn_penalty);
    while (last_segs.empty()){
        last_dep = depots[rand()%(depots.size())];
        last_segs = find_path_between_intersections(points_in_order[points_in_order.size()-1], last_dep, turn_penalty);
    }
    final_path.insert(final_path.end(), first_segs.begin(), first_segs.end());
    for (unsigned i = 0; i < points_in_order.size() - 1; i++) {
        if (points_in_order[i] == points_in_order[i+1]) {
            continue;
        } else {
            vector<unsigned> segs = find_path_between_intersections(points_in_order[i], points_in_order[i + 1], turn_penalty);
            if (segs.empty()) {
                return segs;
            }
            final_path.insert(final_path.end(), segs.begin(), segs.end());
        }
    }
    final_path.insert(final_path.end(), last_segs.begin(), last_segs.end());
    return final_path;
}

//returns the ORDER in the list!!!!! Its ORDER not INTER_ID
//start from 0
//could return duplicate values

unsigned find_nearest_valid_point(vector<unsigned> valid_inters, unsigned cur_inter) {
    for (unsigned i = 0; i < valid_inters.size(); i++){
        if (valid_inters[i] == cur_inter)
            return i;
    }   //loop used to check whether duplicate inter
    t_point cur_pos = globalBase -> get_t_point_intersection_pos(cur_inter);
    double x = cur_pos.x;
    double y = cur_pos.y;
    double distance = 1000000;              //random big number
    unsigned id;
    for (unsigned i = 0; i < valid_inters.size(); i++){
        t_point comp_pos = globalBase -> get_t_point_intersection_pos(valid_inters[i]);
        double x_diff = x-comp_pos.x;
        double y_diff = y-comp_pos.y;
        double cur_dis = x_diff*x_diff+y_diff*y_diff;
        if (cur_dis < distance){
            distance = cur_dis;
            id = i;
        }
    }
    return id;
    
}


/*vector<unsigned> traveling_courier(const vector<DeliveryInfo> & deliveries,
        const vector<unsigned> & depots, const float turn_penalty) {
    
    vector <place_pluse_times> pick_up_list={};
    vector <place_pluse_times> drop_off_list={};
    for (unsigned i = 0; i < deliveries.size(); i++) {
        if(idExistingInVector(deliveries[i].pickUp,pick_up_list)){//once exists
            unsigned pname=deliveries[i].pickUp;
            for(unsigned j=0;j<pick_up_list.size();j++){
                if(pick_up_list[j].id==pname)
                    pick_up_list[j].appear_time++;//
            }
        }
        else{
            place_pluse_times temp;
            temp.id=deliveries[i].pickUp;
            temp.appear_time=1;
            pick_up_list.push_back(temp);
        }
        if(idExistingInVector(deliveries[i].dropOff,drop_off_list)){//once exists
            unsigned dname=deliveries[i].dropOff;
            for(unsigned j=0;j<drop_off_list.size();j++){
                if(drop_off_list[j].id==dname)
                    drop_off_list[j].appear_time++;
            }
        }
        else{
            place_pluse_times temp;
            temp.id=deliveries[i].dropOff;
            temp.appear_time=1;
            drop_off_list.push_back(temp);
        }
      }
        // no need to sort 
        //vector<place_pluse_times> temp;
        //temp=sortVectorBasedOnTime(pick_up_list);
        //pick_up_list=temp;
        //temp=sortVectorBasedOnTime(drop_off_list);
        //drop_off_list=temp;
        
        double timeConsumed[depots.size()];
        for(unsigned i=0;i<depots.size();i++){
          timeConsumed[i]=0;
          for(unsigned m=0;m<pick_up_list.size();m++){
            vector <unsigned> path=find_path_between_intersections(depots[i],pick_up_list[m].id,turn_penalty);
            double t=compute_path_travel_time(path,turn_penalty);
            timeConsumed[i]=timeConsumed[i]+t*pick_up_list[m].appear_time;
          }
          for(unsigned n=0;n<drop_off_list.size();n++){
            vector <unsigned> path=find_path_between_intersections(depots[i],drop_off_list[n].id,turn_penalty);
            double t=compute_path_travel_time(path,turn_penalty);
            timeConsumed[i]=timeConsumed[i]+t*drop_off_list[n].appear_time;
          }
      }
       int least_index=0;
       double min=timeConsumed[0];
       for(unsigned i=0;i<depots.size();i++){
         if(timeConsumed[i]<min){
           min=timeConsumed[i];
           least_index=i;
         }
        
       }
       
       unsigned depots_id=depots[least_index];
       vector<unsigned> final_path;
       final_path.push_back(depots_id);
       vector<place_pluse_times> available_site=pick_up_list;
       while(available_site.size()!=0){

         double least_time=0;
         unsigned decided_point;
         int decided_position;

         for(unsigned i=0;i<available_site.size();i++){
           vector <unsigned> path = find_path_between_intersections(depots_id,available_site[i].id,turn_penalty);
           double t=compute_path_travel_time(path,turn_penalty)*available_site[i].appear_time;
           if(least_time==0)
           least_time=t;
           else{
             if(least_time>t)
             least_time=t;
             decided_point=available_site[i].id;
             decided_position=i;
           }
         }
         final_path.push_back(decided_point);
         available_site.erase(available_site.begin()+decided_position);
         if(idExistingInVector(decided_point,pick_up_list)){
           for (unsigned i=0;i<deliveries.size();i++){
             if(deliveries[i].pickUp==decided_point){
               place_pluse_times tempp;
               tempp.id=deliveries[i].dropOff;
               tempp.appear_time=1;
               available_site.push_back(tempp);
             }

           }

         }         
}
    return final_path;
}*/



bool vectorInOrderOrNot(const vector<place_pluse_times> inputList){
  for(unsigned i=1;i<inputList.size();i++) {
    if(inputList[i-1].appear_time<inputList[i].appear_time)
    return false;
  }
  return true;
}

bool idExistingInVector(unsigned id,const vector<place_pluse_times>v){
  for(unsigned i=0;i<v.size();i++){
    if(v[i].id==id)
    return true;
  }
  return false;
}



vector<place_pluse_times> sortVectorBasedOnTime( const vector<place_pluse_times> inputList){
    /*
     * logic is demonstrated here:
     * find the components with largest appear_time
     * make the temp one equal to that one,
     * erase that one, and push back it at end;
     * then do it until the appear_time of first components is equal to the largest appear time
     *
     */
    vector<place_pluse_times> resultList=inputList;
    int max_appear_time=0;

    while (!vectorInOrderOrNot(inputList)) {
      place_pluse_times max_since_now;
      int max_index=0;
      for(unsigned i=0;i<resultList.size();i++){
        if(resultList[i].appear_time>max_appear_time){
          max_appear_time=resultList[i].appear_time;
          max_since_now=resultList[i];
          max_index=i;
        }
      }
      resultList.erase(resultList.begin()+max_index);
      resultList.push_back(max_since_now);
    }
return resultList;
}
