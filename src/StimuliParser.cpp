#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

#include "StimuliParser.h"

bool StimuliParser::parse(string filename, vector<Event>& events) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open stimuli file: " << filename << endl;
        return false;
    }

    string line;
    int cumulativeTime = 0;

    while (getline(file, line)) {
        //trim leading whitespace
        int start = 0;
        while (start < (int)line.size() && (line[start] == ' ' || line[start] == '\t')) start++;
        line = line.substr(start);

        //skip empty lines and comments
        if (line.empty() || line[0] == '/' || line[0] == '.') continue;

        //format is: #delay netname=value;
        if (line[0] != '#') continue;

        //parse the delay number after #
        int i = 1;
        string delayStr = "";
        while (i < (int)line.size() && isdigit(line[i])) {
            delayStr += line[i];
            i++;
        }
        if (delayStr.empty()) continue;
        int delay = stoi(delayStr);

        //skip spaces
        while (i < (int)line.size() && line[i] == ' ') i++;

        //read net name (up to the = sign)
        string netName = "";
        while (i < (int)line.size() && line[i] != '=' && line[i] != ' ') {
            netName += line[i];
            i++;
        }
        if (netName.empty()) continue;

        //skip spaces and equals sign
        while (i < (int)line.size() && (line[i] == ' ' || line[i] == '=')) i++;

        //read the value digit
        if (i >= (int)line.size() || !isdigit(line[i])) continue;
        int value = line[i] - '0';

        //times are cumulative
        cumulativeTime += delay;

        Event ev;
        ev.time = cumulativeTime;
        ev.netName = netName;
        ev.newValue = value;
        events.push_back(ev);
    }

    file.close();
    return true;
}
