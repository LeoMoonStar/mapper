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
        load_map("/cad2/ece297s/public/maps/toronto_canada.streets.bin");
    }

    ~MapFixture() {
        //Clean-up
        close_map();
    }
};

SUITE(street_queries_public_toronto_canada) {

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

        expected = {35};
        actual = find_street_ids_from_name("Yonge Street");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {126};
        actual = find_street_ids_from_name("University Avenue");
        std::sort(actual.begin(), actual.end());
        CHECK_EQUAL(expected, actual);

        expected = {8537};
        actual = find_street_ids_from_name("Airdrie Drive");
        CHECK_EQUAL(expected, actual);

        expected = {15641};
        actual = find_street_ids_from_name("Siberry Road");
        CHECK_EQUAL(expected, actual);

        expected = {11545};
        actual = find_street_ids_from_name("Belinda Square");
        CHECK_EQUAL(expected, actual);

        expected = {18040};
        actual = find_street_ids_from_name("Vescovo Road");
        CHECK_EQUAL(expected, actual);


        expected = {11357};
        actual = find_street_ids_from_name("Highways 27, 401, 427");
        CHECK_EQUAL(expected, actual);


        expected = {7839};
        actual = find_street_ids_from_name("Northgate Boulevard");
        CHECK_EQUAL(expected, actual);


        expected = {7831};
        actual = find_street_ids_from_name("Northampton Street");
        CHECK_EQUAL(expected, actual);

        expected = {15649};
        actual = find_street_ids_from_name("Duncairn Drive");
        CHECK_EQUAL(expected, actual);

        expected = {4625};
        actual = find_street_ids_from_name("Spears Street");
        CHECK_EQUAL(expected, actual);

        expected = {14236};
        actual = find_street_ids_from_name("Aymarn Court");
        CHECK_EQUAL(expected, actual);

        expected = {11111};
        actual = find_street_ids_from_name("Cunard Street");
        CHECK_EQUAL(expected, actual);

        expected = {10436};
        actual = find_street_ids_from_name("Welsh Street");
        CHECK_EQUAL(expected, actual);

        expected = {6904};
        actual = find_street_ids_from_name("Gleda Crescent");
        CHECK_EQUAL(expected, actual);

        expected = {12196};
        actual = find_street_ids_from_name("Bellvare Crescent");
        CHECK_EQUAL(expected, actual);

        expected = {19638};
        actual = find_street_ids_from_name("Little Hannah Lane");
        CHECK_EQUAL(expected, actual);

        expected = {254, 16537};
        actual = find_street_ids_from_name("Bay Street");
        CHECK_EQUAL(expected, actual);

        expected = {578, 2028};
        actual = find_street_ids_from_name("Waterloo Avenue");
        CHECK_EQUAL(expected, actual);


        expected = {255, 5752};
        actual = find_street_ids_from_name("Grosvenor Street");
        CHECK_EQUAL(expected, actual);

        expected = {197, 13044};
        actual = find_street_ids_from_name("College Street");
        CHECK_EQUAL(expected, actual);

        expected = {12181, 12193, 12211};
        actual = find_street_ids_from_name("Townley Avenue");
        CHECK_EQUAL(expected, actual);

        expected = {260, 273, 925, 4590};
        actual = find_street_ids_from_name("Spadina Avenue");
        CHECK_EQUAL(expected, actual);

        expected = {195, 7349, 16531, 16534, 16568};
        actual = find_street_ids_from_name("Queen Street West");
        CHECK_EQUAL(expected, actual);

        expected = {1313, 5191, 6621, 7411, 12215};
        actual = find_street_ids_from_name("Fourteenth Avenue");
        CHECK_EQUAL(expected, actual);

        expected = {19003, 19004, 19068, 19340, 19343, 19871};
        actual = find_street_ids_from_name("Dundas Street");
        CHECK_EQUAL(expected, actual);

        expected = {426, 1833, 6464, 6685, 7015, 7689, 17723, 18843};
        actual = find_street_ids_from_name("Queen Street East");
        CHECK_EQUAL(expected, actual);

        //TIME TEST
        ECE297_TIME_CONSTRAINT(250);
        std::vector<unsigned> result;
        unsigned i = 0;
        for (size_t j = 0; j < 1000000; j++) {
            if (i == 21222) {
                i = 0;
            }
            result = find_street_ids_from_name(getStreetName(i));
            i++;
        }

    }
}