#include "impl.hpp"
#include <cmath>
#include <cstdint>
#include <string>
#include <string_view>

// bool check(char id[]) {
// 	int index, sum, num;
// 	for (sum = index = 0; index < 17; index++)
//         sum += static_cast<int>(std::pow(2, 17 - index)) % 11 * (id[index] - '0');
// 	num = (12 - (sum % 11)) % 11;
// 	if (num < 10)
// 		return (num == id[17] - '0');
// 	else
// 		return (id[17] == 'X');
// }

// implementation of person_database
void v1::person_database::insert(std::span<person> persons) {
    for (const auto& p : persons) {
        this->persons.insert_or_assign(std::string(p.id), p);
    }
}

person& v1::person_database::search(std::string_view id) {
    return this->persons.at(std::string(id));
}

void v2::person_database::insert(std::span<person> persons) {
    for (const auto& p : persons) {
        auto res = this->index.find(std::string(p.id));
        if (res == this->index.end()) {
            this->index.insert({std::string(p.id), this->persons.size()});
            this->persons.push_back(p);
        } else {
            this->persons[res->second] = p;
        }
    }
}

person& v2::person_database::search(std::string_view id) {
    return this->persons[this->index.at(std::string(id))];
}

// a converter for v3, convert the char* id to a std::uint64_t number
// according to the rules of the ID
// ...最后一位是校验码，这里采用的是ISO 7064:1983, MOD 11-2
// 校验码系统[5]。校验码为一位数，但如果最后采用校验码系统计算的
// 校验码是“10”，碍于身份证号码为18位的规定，则以“X”代替校验码“10”。
// so just ignore the last digit, and convert the rest to a number
std::uint64_t v3_convert_id(std::string_view id) {
    std::uint64_t res = 0;
    for (int i = 0; i < 17; i++) {
        res = res * 10 + (id[i] - '0');
    }
    return res;
}

void v3::person_database::insert(const std::span<person> persons) {
    for (const auto& p : persons) {
        auto id = v3_convert_id(p.id);
        auto res = this->index.find(id);
        if (res == this->index.end()) {
            this->index.insert({id, this->persons.size()});
            this->persons.push_back(p);
        } else {
            this->persons[res->second] = p;
        }
    }
}

person& v3::person_database::search(std::string_view id) {
    return this->persons[this->index.at(v3_convert_id(id))];
}

/**
 * @brief 
 * 0 ~ 5 位：   行政区划代码
 * 6 ~ 9 位：   出生年份
 * 10 ~ 11 位： 出生月份
 * 12 ~ 13 位： 出生日期
 * 14 ~ 16 位： 顺序码
 * 17 位：      校验码
 * @param id 
 * @return std::tuple<std::uint16_t, std::uint32_t> 
 * 返回[出生月日，剩余索引]
 */
std::tuple<int, int, int, std::uint32_t> v4_convert_id(std::string_view id) {
    auto string_view_to_uint = [](std::string_view sv) -> std::uint32_t {
        std::uint32_t result = 0;
        for (auto c : sv) {
            result = result * 10 + (c - '0');
        }
        return result;
    };

    auto year = string_view_to_uint(id.substr(6, 4));
    auto month = string_view_to_uint(id.substr(10, 2));
    auto day = string_view_to_uint(id.substr(12, 2));

    // retain the 0 ~ 9, 14 ~ 16 bits
    std::uint32_t converted_id = string_view_to_uint(id.substr(0, 6));
    converted_id *= std::pow(10, 3);
    converted_id += string_view_to_uint(id.substr(14, 3));

    std::uint32_t md_index {0};

    return {year, month, day, converted_id};
}

void v4::person_database::insert(const std::span<person> persons) {
    for (const auto& p : persons) {
        auto [year, month, day, converted_id] = v4_convert_id(p.id);

        auto map_position = this->index.find(year);
        if (map_position == this->index.end()) {
            auto [map_position, _] = this->index.insert({year, month_day_table()});
            map_position->second.insert(month, day, converted_id, this->persons.size());
            this->persons.push_back(p);
        } else {
            auto result = map_position->second.search(month, day, converted_id);
            if (result.has_value()) {
                this->persons[result.value()] = p;
            } else {
                map_position->second.insert(month, day, converted_id, this->persons.size());
                this->persons.push_back(p);
            }
        }
    }
}

person& v4::person_database::search(std::string_view id) {
    auto [year, month, day, converted_id] = v4_convert_id(id);

    auto map_position = this->index.find(year);
    if (map_position == this->index.end()) {
        throw "not found";
    }

    auto result = map_position->second.search(month, day, converted_id);
    if (result.has_value()) {
        return this->persons[result.value()];
    } else {
        throw "not found";
    }
}