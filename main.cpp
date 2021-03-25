#include <vector>
#include <deque>
#include <cmath>
#include <iostream>
#include <iterator>
#include <cassert>
#include "deque.h"

struct S {
    int x = 1;

    S() = delete;

    S(int x) : x(x) {}

    void update() {
        std::cout << "upd";
    }

    S(const S& other) {
        throw 1;
    }

};


int main() {
    Deque<int> d(5);
    Deque<int>::const_iterator it = d.begin();
}