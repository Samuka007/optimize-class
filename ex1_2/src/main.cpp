#include <cassert>
#include <iostream>
#include <cstring>
#include <string>
#include <map>
#include "person.h"

namespace test {
    std::string random_id() {
        auto year = 1990 + rand() % 30;
        auto month = 1 + rand() % 12;
        auto day = 1 + rand() % 31;
        auto header /* six num header */ = 100000 + rand() % 900000;
        auto tail = rand() % 1000;
        std::string res;
        
        // header should be six digits
        res += std::to_string(header);
        res += std::to_string(year);
        // month should be two digits
        if (month < 10) {
            res += "0";
        }
        res += std::to_string(month);
        // day should be two digits
        if (day < 10) {
            res += "0";
        }
        res += std::to_string(day);
        // tail should be three digits
        if (tail < 10) {
            res += "00";
        } else if (tail < 100) {
            res += "0";
        }
        res += std::to_string(tail);
        // since the last digit is the check digit, we can just set it to 0
        res += "0";
        return res;
    }

    person new_person(const std::string& id) {
        person p;
        std::strcpy(p.id, id.c_str());
        std::strcpy(p.name, "test");
        std::strcpy(p.address, "test");
        std::strcpy(p.phone_num, "12345678900");
        return p;
    }

    /* test insert and search accuracy: 
    check id only, generate a series of random id, 
    one should be seperatedly stored in a simple 
    key-value map and one should be store in the 
    v1 ~ v4 database, and check the inserted and
    search value whether the same with the simple 
    key-value map */
    void test_accuracy(int version) {
        std::map<std::string, person> simple_map;
        // std::map<std::string, person> database;
        set_version(version);

        // Generate a series of random ids
        for (int i = 0; i < 10; i++) {
            std::string id = random_id();
            person p = new_person(id);

            // Store id and person in simple map
            simple_map[id] = p;

            // Store id and person in database
            person_insert(&p, 1);
        }

        // Check if the inserted and searched values are the same
        for (const auto& [_, p_simple] : simple_map) {

            // Search for the person in the database
            auto it = person_search(p_simple.id);
            assert(it != nullptr);

            const person& p_database = *it;

            // Compare the values
            assert(std::strcmp(p_simple.id, p_database.id) == 0);
        }
    }
}


int main(int argc, char** argv) {
    if (auto id = test::random_id(); id.length() != 18) {
        throw "random_id() should return a string with length 18";
    }
    for (int i = 1; i <= 4; i++) {
        test::test_accuracy(i);
    }
}
