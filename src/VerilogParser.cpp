#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

#include "VerilogParser.h"

//strip leading/trailing whitespace
string VerilogParser::trim(string s) {
    int start = 0;
    while (start < (int)s.size() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n'))
        start++;
    int end = (int)s.size() - 1;
    while (end >= start && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r' || s[end] == '\n'))
        end--;
    if (start > end) return "";
    return s.substr(start, end - start + 1);
}

//remove both // and /* */ style comments
string VerilogParser::removeComments(string content) {
    string result = "";
    int i = 0;
    while (i < (int)content.size()) {
        if (i + 1 < (int)content.size() && content[i] == '/' && content[i+1] == '/') {
            //skip to end of line
            while (i < (int)content.size() && content[i] != '\n') i++;
        } else if (i + 1 < (int)content.size() && content[i] == '/' && content[i+1] == '*') {
            //skip block comment
            i += 2;
            while (i + 1 < (int)content.size() && !(content[i] == '*' && content[i+1] == '/')) i++;
            i += 2;
        } else {
            result += content[i];
            i++;
        }
    }
    return result;
}

//split the file by semicolons into statements
vector<string> VerilogParser::splitStatements(string content) {
    vector<string> stmts;
    string current = "";
    for (int i = 0; i < (int)content.size(); i++) {
        if (content[i] == ';') {
            string t = trim(current);
            if (!t.empty()) stmts.push_back(t);
            current = "";
        } else {
            current += content[i];
        }
    }
    //endmodule has no semicolon so catch it here
    string t = trim(current);
    if (!t.empty()) stmts.push_back(t);
    return stmts;
}

bool VerilogParser::parseModuleDecl(string stmt, Circuit& circuit) {
    //looking for: module name(ports)
    int modPos = stmt.find("module");
    if (modPos == (int)string::npos) return false;

    int nameStart = modPos + 6;
    while (nameStart < (int)stmt.size() && stmt[nameStart] == ' ') nameStart++;

    int nameEnd = nameStart;
    while (nameEnd < (int)stmt.size() && stmt[nameEnd] != ' ' && stmt[nameEnd] != '(') nameEnd++;

    string name = stmt.substr(nameStart, nameEnd - nameStart);
    if (!name.empty()) circuit.setModuleName(name);
    return true;
}

bool VerilogParser::parseIODecl(string stmt, Circuit& circuit, Net::Type type) {
    string keyword = (type == Net::Type::INPUT) ? "input" : "output";
    int pos = stmt.find(keyword);
    if (pos == (int)string::npos) return false;

    string rest = stmt.substr(pos + keyword.size());
    stringstream ss(rest);
    string token;
    while (getline(ss, token, ',')) {
        string name = trim(token);
        if (!name.empty()) {
            Net* net = circuit.getOrCreateNet(name);
            net->setType(type);
        }
    }
    return true;
}

bool VerilogParser::parseWireDecl(string stmt, Circuit& circuit) {
    int pos = stmt.find("wire");
    if (pos == (int)string::npos) return false;

    string rest = stmt.substr(pos + 4);
    stringstream ss(rest);
    string token;
    while (getline(ss, token, ',')) {
        string name = trim(token);
        if (!name.empty()) {
            Net* net = circuit.getOrCreateNet(name);
            //dont overwrite if already declared as input/output
            if (net->getType() != Net::Type::INPUT && net->getType() != Net::Type::OUTPUT)
                net->setType(Net::Type::WIRE);
        }
    }
    return true;
}

bool VerilogParser::parseGateStatement(string stmt, Circuit& circuit) {
    //all the gate keywords we know
    string gateTypes[] = { "and", "or", "xor", "nand", "nor", "xnor", "buf", "not", "bufif1" };
    int numTypes = 9;

    string keyword = "";
    for (int i = 0; i < numTypes; i++) {
        string gt = gateTypes[i];
        if ((int)stmt.size() >= (int)gt.size() && stmt.substr(0, gt.size()) == gt) {
            //make sure its not just the start of a longer word
            if ((int)stmt.size() == (int)gt.size() || !isalnum(stmt[gt.size()])) {
                keyword = gt;
                break;
            }
        }
    }
    if (keyword.empty()) return false;

    string rest = trim(stmt.substr(keyword.size()));

    //check for optional delay like #5
    int delay = 1;
    if (!rest.empty() && rest[0] == '#') {
        int end = 1;
        while (end < (int)rest.size() && isdigit(rest[end])) end++;
        delay = stoi(rest.substr(1, end - 1));
        rest = trim(rest.substr(end));
    }

    //get the instance name (before the opening paren)
    int parenPos = rest.find('(');
    if (parenPos == (int)string::npos) return false;

    string instName = trim(rest.substr(0, parenPos));

    //get everything between the parens
    int closePos = rest.find(')', parenPos);
    if (closePos == (int)string::npos) return false;

    string portStr = rest.substr(parenPos + 1, closePos - parenPos - 1);
    vector<string> ports;
    stringstream ss(portStr);
    string token;
    while (getline(ss, token, ',')) {
        string name = trim(token);
        if (!name.empty()) ports.push_back(name);
    }

    if (ports.empty()) return false;

    Gate::GateType gateType = Gate::stringToType(keyword);
    Gate* gate = circuit.addGate(instName, gateType, delay);

    //first port is always the output
    Net* outputNet = circuit.getOrCreateNet(ports[0]);
    gate->setOutput(outputNet);

    if (gateType == Gate::GateType::BUFIF1) {
        //bufif1 is special: output, input, control
        if ((int)ports.size() >= 2) {
            Net* inputNet = circuit.getOrCreateNet(ports[1]);
            gate->addInput(inputNet);
            inputNet->addFanout(gate);
        }
        if ((int)ports.size() >= 3) {
            Net* controlNet = circuit.getOrCreateNet(ports[2]);
            gate->setControl(controlNet);
            controlNet->addFanout(gate);
        }
    } else {
        //normal gates: output, input1, input2...
        for (int i = 1; i < (int)ports.size(); i++) {
            Net* inputNet = circuit.getOrCreateNet(ports[i]);
            gate->addInput(inputNet);
            inputNet->addFanout(gate);
        }
    }

    return true;
}

bool VerilogParser::parse(string filename, Circuit& circuit) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open Verilog file: " << filename << endl;
        return false;
    }

    //read the whole file into a string
    string content = "";
    string line;
    while (getline(file, line)) {
        content += line + "\n";
    }
    file.close();

    content = removeComments(content);
    vector<string> stmts = splitStatements(content);

    for (int i = 0; i < (int)stmts.size(); i++) {
        string stmt = stmts[i];
        if (stmt == "endmodule") continue;

        if (stmt.substr(0, 6) == "module") {
            parseModuleDecl(stmt, circuit);
        } else if (stmt.substr(0, 5) == "input") {
            parseIODecl(stmt, circuit, Net::Type::INPUT);
        } else if (stmt.substr(0, 6) == "output") {
            parseIODecl(stmt, circuit, Net::Type::OUTPUT);
        } else if (stmt.substr(0, 4) == "wire") {
            parseWireDecl(stmt, circuit);
        } else {
            parseGateStatement(stmt, circuit);
        }
    }

    return true;
}
