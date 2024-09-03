/**
a simple thread-unsafe implementation of a person database
 */
#pragma once

#include "person.h"

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <span>
#include <optional>

namespace v1 {
    class person_database {
    public:
        void insert(std::span<person> persons);
        person& search(std::string_view id);
    private:
        std::map<std::string, person> persons;
    };
    static person_database instance;

    // conversion functions
    inline void insert_interface(person* p, int N) {
        instance.insert(
            {p, static_cast<std::size_t>(N)}
        );
    }

    inline person* search_interface(const char* id) {
        try {
            return &instance.search(id);
        } catch (...) {
            return nullptr;
        }
    }
}

namespace v2 {
    // 在数量N小于10^9时，可以将所有的个人信息存放在一个数组中。
    // 此时，value不再存放整个数据结构的内容，而是仅存放这个数据
    // 结构在整个数组中的索引（32位无符号整数），因此value的
    // 数据容量从112个字节减小到4个字节。
    class person_database {
    public:
        void insert(std::span<person> persons);
        person& search(std::string_view id);
    private:
        std::vector<person> persons;
        std::map<std::string, std::size_t> index;
    };
    static person_database instance;

    // conversion functions
    inline void insert_interface(person* p, int N) {
        instance.insert(
            {p, static_cast<std::size_t>(N)}
        );
    }

    inline person* search_interface(const char* id) {
        try {
            return &instance.search(id);
        } catch (...) {
            return nullptr;
        }
    }
}

namespace v3 {
    // 实现方案3
	// 在实现方案2的基础上，可以根据身份证号码的构成规则
    // 将含18个字符的字符串id转换为64位整数。此时，
    // key的数据类型从字符串类型转化为64位整数。
    class person_database {
    public:
        void insert(std::span<person> persons);
        person& search(std::string_view id);
    private:
        std::vector<person> persons;
        std::map<std::uint64_t, std::uint32_t> index;
    };
    static person_database instance;

    // conversion functions
    inline void insert_interface(person* p, int N) {
        instance.insert(
            {p, static_cast<std::uint32_t>(N)}
        );
    }

    inline person* search_interface(const char* id) {
        try {
            return &instance.search(id);
        } catch (...) {
            return nullptr;
        }
    }
}

namespace v4 {
    // 实现方案4
	// 在实现方案3的基础上，可以提取身份证号码的日期（共366种可能），
    // 从而形成366个映射。这样能减少映射中元素的数量，并有可能使得key
    // 缩减为32位无符号整数。
    class person_database {
    public:
        void insert(std::span<person> persons);
        person& search(std::string_view id);

        class month_day_table {
            public:
                void insert(std::uint32_t month, std::uint32_t day, std::uint32_t index, std::uint32_t pos) {
                    this->index[month][day].insert_or_assign(index, pos);
                }
                std::optional<std::uint32_t> search(std::uint32_t month, std::uint32_t day, std::uint32_t index) {
                    auto result = this->index[month][day].find(index);
                    if (result == this->index[month][day].end()) {
                        return std::nullopt;
                    }
                    return result->second;
                }
            private:
                std::array<std::array<std::map<std::uint32_t,std::uint32_t>,31>,12> index;
        };
    private:
        std::vector<person> persons;
        // std::array<std::map<std::uint32_t, std::uint32_t>, 366> index;
        std::map<std::uint16_t, month_day_table> index;
    };

    static person_database instance;

    // conversion functions
    inline void insert_interface(person* p, int N) {
        instance.insert(
            {p, static_cast<std::uint32_t>(N)}
        );
    }

    inline person* search_interface(const char* id) {
        try {
            return &instance.search(id);
        } catch (...) {
            return nullptr;
        }
    }
}
