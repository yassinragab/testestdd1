#ifndef GATE_H
#define GATE_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Net;

class Gate {
public:
    //all the gate types we support
    enum class GateType {
        AND, OR, XOR, NAND, NOR, XNOR, BUF, NOT, BUFIF1
    };

    Gate(string name, GateType type, int delay = 1);

    string getName();
    GateType getType();
    int getDelay();
    void setDelay(int delay);

    void setOutput(Net* net);
    Net* getOutput();

    void addInput(Net* net);
    vector<Net*>& getInputs();

    //bufif1 needs a special control pin
    void setControl(Net* net);
    Net* getControl();

    //compute what the gate outputs right now
    int evaluate();

    static GateType stringToType(string s);
    static string typeToString(GateType t);

private:
    string name_;
    GateType type_;
    int delay_;
    Net* output_;
    vector<Net*> inputs_;
    Net* control_;
};

#endif
