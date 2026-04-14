#ifndef STIMULI_PARSER_H
#define STIMULI_PARSER_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "Event.h"

class StimuliParser {
public:
    //read a .stim file and fill the events list
    static bool parse(string filename, vector<Event>& events);
};

#endif
