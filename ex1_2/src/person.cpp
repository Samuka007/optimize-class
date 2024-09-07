#include "person.h"
#include "impl.hpp"

static int version = 1;

void set_version(int v) {
    version = v;
}

void clear() {
    switch (version) {
        case 1:
            v1::clear();
            break;
        case 2:
            v2::clear();
            break;
        case 3:
            v3::clear();
            break;
        case 4:
            v4::clear();
            break;
        default:
            throw "unsupported version";
    }
}

person *person_search(const char *id) {
    switch (version) {
        case 1:
            return v1::search_interface(id);
        case 2:
            return v2::search_interface(id);
        case 3:
            return v3::search_interface(id);
        case 4:
            return v4::search_interface(id);
        default:
            throw "unsupported version";
    }
}

void person_insert(struct person *p, int N) {
    switch (version) {
        case 1:
            v1::insert_interface(p, N);
            break;
        case 2:
            v2::insert_interface(p, N);
            break;
        case 3:
            v3::insert_interface(p, N);
            break;
        case 4:
            v4::insert_interface(p, N);
            break;
        default:
            throw "unsupported version";
    }
}
