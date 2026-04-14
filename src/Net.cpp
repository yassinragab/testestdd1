#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "Net.h"

Net::Net(string name, Type type) {
    name_ = name;
    type_ = type;
    value_ = -1;  //start as unknown
    eventGeneration_ = 0;
}

string Net::getName() { return name_; }
Net::Type Net::getType() { return type_; }
void Net::setType(Type type) { type_ = type; }

int Net::getValue() { return value_; }
void Net::setValue(int val) { value_ = val; }

void Net::incrementGeneration() { eventGeneration_++; }
int Net::getGeneration() { return eventGeneration_; }

void Net::addFanout(Gate* gate) { fanouts_.push_back(gate); }
vector<Gate*>& Net::getFanouts() { return fanouts_; }
