#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

#include "Circuit.h"

Circuit::Circuit() {}

Net* Circuit::getOrCreateNet(string name) {
    //return existing net if we already have it
    if (nets_.count(name)) {
        return nets_[name];
    }
    Net* net = new Net(name);
    nets_[name] = net;
    return net;
}

Net* Circuit::getNet(string name) {
    if (nets_.count(name)) return nets_[name];
    return nullptr;
}

Gate* Circuit::addGate(string name, Gate::GateType type, int delay) {
    Gate* gate = new Gate(name, type, delay);
    gates_.push_back(gate);
    return gate;
}

vector<Gate*>& Circuit::getGates() { return gates_; }
map<string, Net*>& Circuit::getNets() { return nets_; }

vector<Net*> Circuit::getInputs() {
    vector<Net*> result;
    for (auto it = nets_.begin(); it != nets_.end(); it++) {
        if (it->second->getType() == Net::Type::INPUT)
            result.push_back(it->second);
    }
    return result;
}

vector<Net*> Circuit::getOutputs() {
    vector<Net*> result;
    for (auto it = nets_.begin(); it != nets_.end(); it++) {
        if (it->second->getType() == Net::Type::OUTPUT)
            result.push_back(it->second);
    }
    return result;
}

vector<Net*> Circuit::getWires() {
    vector<Net*> result;
    for (auto it = nets_.begin(); it != nets_.end(); it++) {
        if (it->second->getType() == Net::Type::WIRE)
            result.push_back(it->second);
    }
    return result;
}

void Circuit::setModuleName(string name) { moduleName_ = name; }
string Circuit::getModuleName() { return moduleName_; }
