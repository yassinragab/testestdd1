#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

#include "Gate.h"
#include "Net.h"

class Circuit {
public:
    Circuit();

    //get a net by name, or make a new one if it doesnt exist
    Net* getOrCreateNet(string name);
    Net* getNet(string name);

    Gate* addGate(string name, Gate::GateType type, int delay);

    vector<Gate*>& getGates();
    map<string, Net*>& getNets();

    vector<Net*> getInputs();
    vector<Net*> getOutputs();
    vector<Net*> getWires();

    void setModuleName(string name);
    string getModuleName();

private:
    string moduleName_;
    map<string, Net*> nets_;
    vector<Gate*> gates_;
};

#endif
