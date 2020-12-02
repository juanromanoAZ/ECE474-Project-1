#ifndef OPGRAPH_H_USED_
#define OPGRAPH_H_USED_

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

class OpNode
{
public:
    OpNode();
    ~OpNode();

    string OprToken, outVars;
    vector<OpNode*> predecessors;
    vector<OpNode*> successors;
    vector<string> inVars;

    int latency, slack, alapStartTime, listRStartTime;
    bool isAlapScheduled, isListRScheduled;
};


class OpGraph
{
private:
    int latencyConst;
    vector<OpNode*> OprNodes;
    unordered_map<int, vector<OpNode>> startTimeNodeMap;

public:
    OpGraph(vector<vector<string>> oprs, int latency);
    unordered_map<int, vector<OpNode>> getStartTimeNodeMap();
    void scheduleOprs();
    
    vector<OpNode*> getOprNodes();

    void scheduledAlap();
    unordered_map<int, vector<OpNode>> getAlapTimesNodeMap();
    unordered_map<int, vector<OpNode>> getListRTimesNodeMap();
};

#endif
