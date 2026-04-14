#ifndef NET_H
#define NET_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Gate;

//represents a wire/signal in the circuit
class Net {
public:
    enum class Type { INPUT, OUTPUT, WIRE };

    Net(string name, Type type = Type::WIRE);

    string getName();
    Type getType();
    void setType(Type type);

    int getValue();
    void setValue(int val);

    //generation counter - used to throw away stale events
    void incrementGeneration();
    int getGeneration();

    //gates that use this net as an input
    void addFanout(Gate* gate);
    vector<Gate*>& getFanouts();

private:
    string name_;
    Type type_;
    int value_;           //-1 means unknown/X
    int eventGeneration_;
    vector<Gate*> fanouts_;
};

#endif
