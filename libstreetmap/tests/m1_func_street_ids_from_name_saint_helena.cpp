/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <algorithm>
#include <set>
#include <unittest++/UnitTest++.h>
#include "m1.h"
#include "unit_test_util.h"
#include "StreetsDatabaseAPI.h"

using ece297test::relative_error;

struct MapFixture {

    MapFixture() {
        //Load the map
        load_map("/cad2/ece297s/public/maps/saint-helena.streets.bin");
    }

    ~MapFixture() {
        //Clean-up
        close_map();
    }
};

SUITE(street_queries_public_saint_helena) {

    TEST_FIXTURE(MapFixture, street_ids_from_name) {
        std::vector<unsigned> expected;
        std::vector<unsigned> actual;

        expected = {};
        actual = find_street_ids_from_name("");
        CHECK_EQUAL(expected, actual);

        expected = {};
        actual = find_street_ids_from_name(" ");
        CHECK_EQUAL(expected, actual);

        expected = {};
        actual = actual = find_street_ids_from_name("!");
        CHECK_EQUAL(expected, actual);

        expected = {2};
        actual = find_street_ids_from_name("Castle Terrace");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {11};
        actual = find_street_ids_from_name("Side Path");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {3};
        actual = find_street_ids_from_name("Church Lane");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {6};
        actual = find_street_ids_from_name("Casons");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        //"Market Street", "The Run"

        expected = {8};
        actual = find_street_ids_from_name("Market Street");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {10};
        actual = find_street_ids_from_name("The Run");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {13};
        actual = find_street_ids_from_name("Napoleon Street");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {5};
        actual = find_street_ids_from_name("Constitution Hill");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {12};
        actual = find_street_ids_from_name("Seafront");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {15};
        actual = find_street_ids_from_name("Shy Road");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {1};
        actual = find_street_ids_from_name("Field Road");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {11};
        actual = find_street_ids_from_name("Side Path");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {9};
        actual = find_street_ids_from_name("China Lane");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {8};
        actual = find_street_ids_from_name("Market Street");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {16};
        actual = find_street_ids_from_name("Seales Corner");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {0};
        actual = find_street_ids_from_name("<unknown>");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        //TIME TEST
        ECE297_TIME_CONSTRAINT(250);
        std::vector<unsigned> result;
        unsigned i = 0;
        for (size_t j = 0; j < 1000000; j++) {
            if (i == 17) {
                i = 0;
            }
            result = find_street_ids_from_name(getStreetName(i));
            i++;
        }
    }
}
