#ifndef VERILOG_PARSER_H
#define VERILOG_PARSER_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "Circuit.h"

class VerilogParser {
public:
    static bool parse(string filename, Circuit& circuit);

private:
    static string trim(string s);
    static string removeComments(string content);
    static vector<string> splitStatements(string content);
    static bool parseGateStatement(string stmt, Circuit& circuit);
    static bool parseModuleDecl(string stmt, Circuit& circuit);
    static bool parseIODecl(string stmt, Circuit& circuit, Net::Type type);
    static bool parseWireDecl(string stmt, Circuit& circuit);
};

#endif
