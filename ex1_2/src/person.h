/**
Defined the struct person and the database interface
 */
#pragma once

struct person {
    char id[18];        // 18位身份证号码
    char name[20];      // 姓名
    char address[60];   // 地址
    char phone_num[14]; // 电话号码
};

extern "C" {
    void set_version(int version);                 // 设置数据库版本
    void person_insert(struct person *p, int N);   // 插入N条个人信息
    struct person *person_search(const char *id);  // 根据id查找个人信息
}
