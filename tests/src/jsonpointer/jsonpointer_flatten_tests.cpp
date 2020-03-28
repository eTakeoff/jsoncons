// Copyright 2017 Daniel Parker
// Distributed under Boost license

#if defined(_MSC_VER)
#include "windows.h" // test no inadvertant macro expansions
#endif
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>
#include <catch/catch.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <utility>
#include <ctime>
#include <new>

using namespace jsoncons;

TEST_CASE("flatten test")
{
    /*json input = json::parse(R"(
    {
       "application": "hiking",
       "reputons": [
           {
               "rater": "HikingAsylum",
               "assertion": "advanced",
               "rated": "Marilyn C",
               "rating": 0.90
            },
           {
               "rater": "HikingAsylum",
               "assertion": "intermediate",
               "rated": "Hongmin",
               "rating": 0.75
            }       
        ]
    }
    )");*/
        json input = json::parse(R"(
        {
           "reputons": [
               {
                   "rater": "HikingAsylum"
               }       
            ]
        }
        )");
#if 0
    SECTION("json_ptr")
    {
        jsonpointer::json_ptr ptr("/reputons/0/assertion");
        auto it = ptr.begin();
        auto last = ptr.end();
        CHECK(it != last);
        auto p = it++;
        auto q = it++;
        auto s = it++;


        CHECK(it == last);

        //json j;
        //jsonpointer::mapping mapping("/application", &j);

        //CHECK(mapping.ptr.begin() != mapping.ptr.end());
        //CHECK(mapping.first != mapping.last);
    }
#endif
    SECTION("flatten")
    {
        json result = jsonpointer::flatten(input);

        /*REQUIRE(result.is_object());
        REQUIRE(result.size() == 9);

        CHECK(result["/application"].as<std::string>() == std::string("hiking"));
        CHECK(result["/reputons/0/assertion"].as<std::string>() == std::string("advanced"));
        CHECK(result["/reputons/0/rated"].as<std::string>() == std::string("Marilyn C"));
        CHECK(result["/reputons/0/rater"].as<std::string>() == std::string("HikingAsylum"));
        CHECK(result["/reputons/0/rating"].as<double>() == Approx(0.9).epsilon(0.0000001));
        CHECK(result["/reputons/1/assertion"].as<std::string>() == std::string("intermediate"));
        CHECK(result["/reputons/1/rated"].as<std::string>() == std::string("Hongmin"));
        CHECK(result["/reputons/1/rater"].as<std::string>() == std::string("HikingAsylum"));
        CHECK(result["/reputons/1/rating"].as<double>() == Approx(0.75).epsilon(0.0000001));*/

        //std::cout << pretty_print(result) << "\n";
        json original = jsonpointer::unflatten(result);
        std::cout << pretty_print(original) << "\n";

    }
}

