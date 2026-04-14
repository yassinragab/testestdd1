#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
using namespace std;

#include "Simulator.h"

Simulator::Simulator(Circuit& circuit) : circuit_(circuit) {}

void Simulator::loadStimuli(vector<Event>& stimuli) {
    //push all stimulus events into the queue
    //generation = 0 means they are always valid (never stale)
    for (int i = 0; i < (int)stimuli.size(); i++) {
        eventQueue_.push(stimuli[i]);
    }
}

void Simulator::processEvent(Event ev) {
    Net* net = circuit_.getNet(ev.netName);
    if (!net) {
        cerr << "Warning: Unknown net '" << ev.netName << "' in event." << endl;
        return;
    }

    //stale check - gate output events might be outdated if inputs changed again
    //stimulus events have generation == 0 so they skip this check
    if (ev.generation != 0 && ev.generation != net->getGeneration()) {
        return;  //this event is old, throw it away
    }

    //if value didnt actually change, nothing to do
    if (net->getValue() == ev.newValue) {
        return;
    }

    //apply the change and record it
    net->setValue(ev.newValue);
    outputEvents_.push_back(ev);

    //propagate to all gates that have this net as input
    vector<Gate*>& fanouts = net->getFanouts();
    for (int i = 0; i < (int)fanouts.size(); i++) {
        Gate* gate = fanouts[i];
        int newOutput = gate->evaluate();
        Net* gateOut = gate->getOutput();
        if (!gateOut) continue;

        //incrementing generation cancels all previously scheduled events for this net
        gateOut->incrementGeneration();

        if (gateOut->getValue() != newOutput) {
            Event newEv;
            newEv.time = ev.time + gate->getDelay();
            newEv.netName = gateOut->getName();
            newEv.newValue = newOutput;
            newEv.generation = gateOut->getGeneration();
            eventQueue_.push(newEv);
        }
    }
}

void Simulator::run() {
    int maxEvents = 100000;
    int count = 0;

    while (!eventQueue_.empty()) {
        if (count >= maxEvents) {
            cerr << "ERROR: Simulation exceeded " << maxEvents << " events -- possible infinite loop." << endl;
            break;
        }
        Event ev = eventQueue_.top();
        eventQueue_.pop();
        processEvent(ev);
        count++;
    }
}

bool Simulator::writeOutput(string filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open output file: " << filename << endl;
        return false;
    }

    for (int i = 0; i < (int)outputEvents_.size(); i++) {
        file << outputEvents_[i].time << ", " << outputEvents_[i].netName << ", " << outputEvents_[i].newValue << endl;
    }

    file.close();
    cout << "Simulation output written to: " << filename << endl;
    return true;
}
