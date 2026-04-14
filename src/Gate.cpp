#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "Gate.h"
#include "Net.h"

Gate::Gate(string name, GateType type, int delay) {
    name_ = name;
    type_ = type;
    delay_ = delay;
    output_ = nullptr;
    control_ = nullptr;
}

string Gate::getName() { return name_; }
Gate::GateType Gate::getType() { return type_; }
int Gate::getDelay() { return delay_; }
void Gate::setDelay(int delay) { delay_ = delay; }

void Gate::setOutput(Net* net) { output_ = net; }
Net* Gate::getOutput() { return output_; }

void Gate::addInput(Net* net) { inputs_.push_back(net); }
vector<Net*>& Gate::getInputs() { return inputs_; }

void Gate::setControl(Net* net) { control_ = net; }
Net* Gate::getControl() { return control_; }

int Gate::evaluate() {
    //if any input is unknown, output is unknown too
    for (int i = 0; i < (int)inputs_.size(); i++) {
        if (inputs_[i]->getValue() == -1) return -1;
    }

    if (type_ == GateType::AND) {
        int result = 1;
        for (int i = 0; i < (int)inputs_.size(); i++)
            result &= inputs_[i]->getValue();
        return result;
    }
    if (type_ == GateType::OR) {
        int result = 0;
        for (int i = 0; i < (int)inputs_.size(); i++)
            result |= inputs_[i]->getValue();
        return result;
    }
    if (type_ == GateType::XOR) {
        int result = 0;
        for (int i = 0; i < (int)inputs_.size(); i++)
            result ^= inputs_[i]->getValue();
        return result;
    }
    if (type_ == GateType::NAND) {
        int result = 1;
        for (int i = 0; i < (int)inputs_.size(); i++)
            result &= inputs_[i]->getValue();
        return result ? 0 : 1;
    }
    if (type_ == GateType::NOR) {
        int result = 0;
        for (int i = 0; i < (int)inputs_.size(); i++)
            result |= inputs_[i]->getValue();
        return result ? 0 : 1;
    }
    if (type_ == GateType::XNOR) {
        int result = 0;
        for (int i = 0; i < (int)inputs_.size(); i++)
            result ^= inputs_[i]->getValue();
        return result ? 0 : 1;
    }
    if (type_ == GateType::BUF) {
        if (!inputs_.empty()) return inputs_[0]->getValue();
        return -1;
    }
    if (type_ == GateType::NOT) {
        if (!inputs_.empty()) {
            int v = inputs_[0]->getValue();
            if (v == -1) return -1;
            return v ? 0 : 1;
        }
        return -1;
    }
    //bufif1 - output = input if control is 1, else unknown (high-Z)
    if (type_ == GateType::BUFIF1) {
        if (control_ && control_->getValue() == 1) {
            if (!inputs_.empty()) return inputs_[0]->getValue();
        }
        return -1;
    }
    return -1;
}

Gate::GateType Gate::stringToType(string s) {
    if (s == "and")    return GateType::AND;
    if (s == "or")     return GateType::OR;
    if (s == "xor")    return GateType::XOR;
    if (s == "nand")   return GateType::NAND;
    if (s == "nor")    return GateType::NOR;
    if (s == "xnor")   return GateType::XNOR;
    if (s == "buf")    return GateType::BUF;
    if (s == "not")    return GateType::NOT;
    if (s == "bufif1") return GateType::BUFIF1;
    //shouldnt get here
    return GateType::AND;
}

string Gate::typeToString(GateType t) {
    if (t == GateType::AND)    return "and";
    if (t == GateType::OR)     return "or";
    if (t == GateType::XOR)    return "xor";
    if (t == GateType::NAND)   return "nand";
    if (t == GateType::NOR)    return "nor";
    if (t == GateType::XNOR)   return "xnor";
    if (t == GateType::BUF)    return "buf";
    if (t == GateType::NOT)    return "not";
    if (t == GateType::BUFIF1) return "bufif1";
    return "unknown";
}
