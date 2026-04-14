#ifndef EVENT_H
#define EVENT_H

#include <iostream>
#include <string>
using namespace std;

//a signal change that happens at some point in time
struct Event {
    int time;        //when it happens (in ps)
    string netName;  //which net changes
    int newValue;    //0, 1, or -1 for unknown
    int generation;  //0 = stimulus (always valid), >0 = gate output (may be stale)

    Event() {
        time = 0;
        newValue = 0;
        generation = 0;
    }

    //so the priority queue puts earlier events first
    bool operator>(const Event& other) const {
        return time > other.time;
    }
};

#endif
