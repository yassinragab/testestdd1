#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "Circuit.h"
#include "VerilogParser.h"
#include "StimuliParser.h"
#include "Simulator.h"

void printUsage(char* programName) {
    cout << "Event-Driven Logic Circuit Simulator" << endl;
    cout << "Usage: " << programName << " <circuit.v> <stimuli.stim> [output.sim]" << endl;
    cout << endl;
    cout << "Arguments:" << endl;
    cout << "  circuit.v     - Verilog file describing the circuit (structural)" << endl;
    cout << "  stimuli.stim  - Stimuli file with input events" << endl;
    cout << "  output.sim    - Output simulation file (default: output.sim)" << endl;
    cout << endl;
    cout << "Supported Verilog primitives:" << endl;
    cout << "  and, or, xor, nand, nor, xnor, buf, not, bufif1" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    string verilogFile = argv[1];
    string stimFile = argv[2];
    string outputFile = "output.sim";
    if (argc >= 4) outputFile = argv[3];

    //step 1: parse the verilog circuit
    cout << "Parsing circuit: " << verilogFile << endl;
    Circuit circuit;
    if (!VerilogParser::parse(verilogFile, circuit)) {
        cerr << "Error: Failed to parse Verilog file." << endl;
        return 1;
    }
    cout << "Module: " << circuit.getModuleName() << endl;
    cout << "  Inputs:  " << circuit.getInputs().size() << endl;
    cout << "  Outputs: " << circuit.getOutputs().size() << endl;
    cout << "  Wires:   " << circuit.getWires().size() << endl;
    cout << "  Gates:   " << circuit.getGates().size() << endl;

    //step 2: parse the stimuli file
    cout << "Parsing stimuli: " << stimFile << endl;
    vector<Event> stimuli;
    if (!StimuliParser::parse(stimFile, stimuli)) {
        cerr << "Error: Failed to parse stimuli file." << endl;
        return 1;
    }
    cout << "  Events loaded: " << stimuli.size() << endl;

    //step 3: run the simulation
    cout << "Running simulation..." << endl;
    Simulator simulator(circuit);
    simulator.loadStimuli(stimuli);
    simulator.run();

    //step 4: write the output
    simulator.writeOutput(outputFile);

    return 0;
}
