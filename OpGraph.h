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

	string operatorToken;
	vector<OpNode*> predecessors;
	vector<OpNode*> successors;
	vector<string> inputVars;
	string outputVar;
	int latency;
	int slack;
	bool isAlapScheduled;
	int alapStartTime;
	bool isListRScheduled;
	int listRStartTime;
};

///////////////////////////////

class OpGraph
{
public:
	OpGraph(vector<vector<string>> ops, int latency); //Constructor.

	unordered_map<int, vector<OpNode>> getStartTimeToNodesMap();
	void scheduleOperations();
	vector<OpNode*> getOperationNodes();

	// temp. move to private.
	void scheduleAlap();
	unordered_map<int, vector<OpNode>> getAlapTimesToNodesMap();
	unordered_map<int, vector<OpNode>> getListRTimesToNodesMap();

private:
	int latencyConstraint;
	vector<OpNode*> operationNodes;
	unordered_map<int, vector<OpNode>> startTimeToNodesMap;

};

#endif