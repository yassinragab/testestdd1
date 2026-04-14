#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>
using namespace std;

#include "Circuit.h"
#include "Event.h"

class Simulator {
public:
    Simulator(Circuit& circuit);

    void loadStimuli(vector<Event>& stimuli);

    void run();

    bool writeOutput(string filename);

private:
    Circuit& circuit_;

    //min-heap so we always process earliest event first
    priority_queue<Event, vector<Event>, greater<Event>> eventQueue_;

    //all the changes we recorded during simulation
    vector<Event> outputEvents_;

    void processEvent(Event ev);
};

#endif
