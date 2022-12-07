// Copyright 2013 Daniel Parker
// Distributed under Boost license

#include <jsoncons/json.hpp>
#include <jsoncons/source.hpp>
#include <catch/catch.hpp>
#include <sstream>
#include <vector>
#include <utility>
#include <ctime>
#include <new>

TEST_CASE("utf8_string_reader tests")
{
    SECTION("test 1")
    {
        std::string str = "[0,1,2,3,4,5,6,7,8,9]";

        jsoncons::utf8_string_reader reader(str);
    }
}

