#include <cassert>
#include <cmath>
#include <iostream>
#include <chrono>
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

    // 需要测量两个时间。
    // 1. 连续插入$N$条记录所需要的时间$T$。
    // 2. 连续进行$M$次查找所需要的时间$T$。
    // 请注意，$N$和$M$的取值范围是$10^3$到$10^6$。
    auto timer(auto func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return end - start;
    }
    
    // class TestTime
    // 在构造时生成$N$个随机id，形成person，保存在一个std::vector中，
    // 这样方便插入时直接调用person_insert()。
    // 依次从v1到v4的数据库中插入这$N$个id，测量插入时间。
    // 依次从v1到v4的数据库中查找这$N$个id，测量查找时间。
    class TestTime {
    public:
        TestTime(int N) {
            if (N < 1000000) {
                throw "N should be greater than 1000000";
            }
            for (int i = 0; i < N; i++) {
                persons.push_back(new_person(random_id()));
            }
        }

        void insert() { // should only be called once for each version
            person_insert(persons.data(), persons.size());
        }

        void search() {
            for (size_t i = 0; i < std::pow(10, 6); ++i) {
                person_search(persons[i].id);
            }
        }

    private:
        std::vector<person> persons;
    };
}


int main(int argc, char** argv) {
    if (auto id = test::random_id(); id.length() != 18) {
        throw "random_id() should return a string with length 18";
    }
    for (int i = 1; i <= 4; i++) {
        set_version(i);

        // test::test_accuracy(i);
        test::TestTime test_time(1000000);
        auto t = test::timer([&] {
            test_time.insert();
        });
        auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t);
        std::cout << "Insert time for version " << i << ": " 
            << duration_ms << std::endl;
        t = test::timer([&] {
            test_time.search();
        });
        duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t);
        std::cout << "Search time for version " << i << ": " 
            << duration_ms << std::endl;
    }
}
