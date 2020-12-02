#include "OpGraph.h"
#include <algorithm>
#include <unordered_map>

using namespace std;

OpNode::OpNode(){
    isAlapScheduled = false; 
    isListRScheduled = false;
}

OpGraph::OpGraph(vector<vector<string>> oprs, int latency){
    latencyConst = latencyConst;

    for (unsigned i = 0; i < oprs.size(); i++)
    {
        vector<string> opTokens = oprs[i];

        OpNode* pNewNode = new OpNode();

        pNewNode -> OprToken = opTokens[3];
        pNewNode -> outVars = opTokens[0];

        if (pNewNode -> OprToken == "?")
        {
            pNewNode -> inVars = {opTokens[2], opTokens[4], opTokens[6]};
        }
        else
        {
            pNewNode -> inVars = {opTokens[2], opTokens[4]};
        }

        if (pNewNode -> OprToken == "*")
        {
            pNewNode -> latency = 2;
        }
        else
        {
            pNewNode -> latency = 1;
        }
        
        for (unsigned i = 0; i < OprNodes.size(); i++)
        {
            OpNode& possiblePredecen = *OprNodes[i];
            if ( count(pNewNode->inVars.begin(), pNewNode -> inVars.end(), possiblePredecen.outVars))
            {
                pNewNode -> predecessors.push_back(&possiblePredecen);
                possiblePredecen.successors.push_back(pNewNode);
            }
        }
        OprNodes.push_back(pNewNode);
    }
    
}

unordered_map<int, vector<OpNode>> OpGraph::getStartTimeNodeMap()
{
	return startTimeNodeMap;
}

vector<OpNode*> OpGraph::getOprNodes()
{
	return OprNodes;
}

unordered_map<int, vector<OpNode>> OpGraph::getAlapTimesNodeMap()
{
	unordered_map<int, vector<OpNode>> alapTimesToNodesMap;
	
	for (OpNode* pCurrentNode : OprNodes)
	{
		alapTimesToNodesMap[pCurrentNode->alapStartTime].push_back(*pCurrentNode);
	}
	return alapTimesToNodesMap;
}

unordered_map<int, vector<OpNode>> OpGraph::getListRTimesNodeMap()
{
	unordered_map<int, vector<OpNode>> listRTimesToNodesMap;

	for (OpNode* pCurrentNode : OprNodes)
	{
		listRTimesToNodesMap[pCurrentNode->listRStartTime].push_back(*pCurrentNode);
	}
	return listRTimesToNodesMap;
}

//List_R Schedule
void OpGraph::scheduleOprs()
{
	scheduledAlap();

	unordered_map<string, int> compsAvailableMap = { { "mult", 1 }, { "adder", 1 }, { "logic", 1 } };
	unordered_map<string, string> opTokenToCompTypeMap = { { "+", "adder" }, { "-", "adder" }, { "*", "mult" }, {">","logic"}, 
								{ "<", "logic" }, { "==", "logic" }, { "?", "logic" }, { ">>", "logic" }, { "<<", "logic" } };

	int time = 1;
	vector<OpNode*> unscheduledNodes = OprNodes;

	//Until all nodes are scheduled.
	while (unscheduledNodes.size() > 0)
	{
		//For every resource type...
		for (auto compTypePair : compsAvailableMap)
		{
			string compType = compTypePair.first;

			//...Find the candidate operations. "Operations of type k whose predecessors are completed by time t"
			vector<OpNode*> candidateOps;
			for (OpNode* unschedNode : unscheduledNodes)
			{
				//Does the operation type match?
				string nodesType = opTokenToCompTypeMap[unschedNode->OprToken];
				if (nodesType == compType)
				{
					//Check to see if predecessors are completed by current time. (or no preds)
					bool predsComplete = true;
					for (OpNode* pred : unschedNode->predecessors)
					{
						if (!pred->isListRScheduled || (pred->listRStartTime + pred->latency > time))
						{
							predsComplete = false;
						}
					}
					if (predsComplete)
					{
						// add the node to the candidate operations.
						candidateOps.push_back(unschedNode);
					}
				}
			}//END finding the candidate operations.

			// Calculate the slack of each candidate.
			for (OpNode* candidate : candidateOps)
			{
				int slack = candidate->alapStartTime - time;
				candidate->slack = slack;
			}
 
			unordered_map<string, int> compsUsedThisRound = { { "mult", 0 }, { "adder", 0 }, { "logic", 0 } };

			//Schedule nodes with zero slack and update components used.
			for (int i = 0; i < candidateOps.size(); i++)
			{
				if (candidateOps[i]->slack == 0)
				{
					candidateOps[i]->listRStartTime = time;
					candidateOps[i]->isListRScheduled = true;

					//Remove the newly scheduled op from the list of unscheduled nodes.
					for (int j = 0; j < unscheduledNodes.size(); j++)
					{
						if (candidateOps[i] == unscheduledNodes[j])
						{
							unscheduledNodes.erase(unscheduledNodes.begin() + j);
							j--; // Correct the index.
						}
					}

					//Remove the newly scheduled op from the list of candidates.
					candidateOps.erase(candidateOps.begin() + i);
					i--; // Correct the index.

					//Tally resource usage.
					int& a = compsUsedThisRound[compType];
					a++;
					int& b = compsAvailableMap[compType];
					if (a > b)
					{
						b++;
					}
				}
			}

			//Schedule any remaining resources with candidates.
			for (int i = 0; i < candidateOps.size(); i++)
			{
				int& a = compsUsedThisRound[compType];
				int& b = compsAvailableMap[compType];
				if (b > a)
                {
					candidateOps[i]->listRStartTime = time;
					candidateOps[i]->isListRScheduled = true;

					for (int j = 0; j < unscheduledNodes.size(); j++)
					{
						if (candidateOps[i] == unscheduledNodes[j])
						{
							unscheduledNodes.erase(unscheduledNodes.begin() + j);
							j--; 
						}
					}

					candidateOps.erase(candidateOps.begin() + i);
					i--; 

					int& a = compsUsedThisRound[compType];
					a++;
					int& b = compsAvailableMap[compType];
				}
			}
		}
		time++;
	}
}

void OpGraph::scheduledAlap()
{
	vector<OpNode*> unscheduledNodes = OprNodes;

	while (unscheduledNodes.size() > 0)
	{
		for (int i = 0; i < unscheduledNodes.size(); i++)
		{
			OpNode* pUnscheduledNode = unscheduledNodes[i];

			bool successorsScheduled = true;
			for (OpNode* pSucNode : pUnscheduledNode->successors)
			{
				if (!pSucNode->isAlapScheduled)
				{
					successorsScheduled = false;
				}
			}

			if (successorsScheduled)
			{

				if (pUnscheduledNode->successors.size() == 0)
				{
					pUnscheduledNode->alapStartTime = (latencyConst + 1) - pUnscheduledNode->latency;
					pUnscheduledNode->isAlapScheduled = true;
				}
				else
				{
					//Need to find the earliest time that a successor starts.
					int minStartTime = 10000;
					for (OpNode* pSucNode : pUnscheduledNode->successors)
					{
						if (pSucNode->alapStartTime < minStartTime)
						{
							minStartTime = pSucNode->alapStartTime;
						}
					}

					pUnscheduledNode->alapStartTime = minStartTime - pUnscheduledNode->latency;

					if (pUnscheduledNode->alapStartTime < 1)
					{
						cout << "Error: Operations cannot be scheduled in the given latency constraint." << endl;
						exit(0);
					}

					pUnscheduledNode->isAlapScheduled = true;
				}

				unscheduledNodes.erase(unscheduledNodes.begin() + i);
				i--;

			}
		}
	}
}