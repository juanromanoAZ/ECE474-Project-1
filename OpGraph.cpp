#include "OpGraph.h"
#include <algorithm>

using namespace std;

OpNode::OpNode()
{
	isAlapScheduled = false;
	isListRScheduled = false;
}

//////// OpGraph Functions ///////////////////////

//Constructor
OpGraph::OpGraph(vector<vector<string>> ops, int latency)
{
	latencyConstraint = latency;

	//generate the graph of operation nodes from the ops vector.
	for (unsigned i = 0; i < ops.size(); i++) // for every operation in the ops vector...
	{
		vector<string> opTokens = ops[i];

		OpNode* pNewNode = new OpNode();

		//Populate the new node's data members with info from the operation's tokens.
		pNewNode->operatorToken = opTokens[3]; //e.g. "+", "*", ...
		pNewNode->outputVar = opTokens[0];
		//Add input variables. Check for ternary operator, which will have three inputs. 
		if (pNewNode->operatorToken == "?") //e.g. z = g ? d : e
		{
			pNewNode->inputVars = { opTokens[2], opTokens[4], opTokens[6] }; //e.g. {"g", "d", "e"}
		}
		else
		{
			pNewNode->inputVars = { opTokens[2], opTokens[4] };
		}
		//Add latency associated with that operation. MULTs have latency of 2.
		if (pNewNode->operatorToken == "*")
		{
			pNewNode->latency = 2;
		}
		else
		{
			pNewNode->latency = 1;
		}

		//link the node to its predecessors. If the inputs of pNewNode are outputs of other nodes, link them.
		for (unsigned i = 0; i < operationNodes.size(); i++)
		{
			OpNode& possiblePredecessor = *operationNodes[i];
			//if the possiblePredecessor's output variable is in the vector of pNewNode's input variables...
			if (count(pNewNode->inputVars.begin(), pNewNode->inputVars.end(), possiblePredecessor.outputVar))
			{
				//...Link the nodes.
				pNewNode->predecessors.push_back(&possiblePredecessor);
				possiblePredecessor.successors.push_back(pNewNode);
			}
		}

		operationNodes.push_back(pNewNode); //Add the new node to the graph's list of nodes.
	}
}

unordered_map<int, vector<OpNode>> OpGraph::getStartTimeToNodesMap()
{
	return startTimeToNodesMap;
}

vector<OpNode*> OpGraph::getOperationNodes()
{
	return operationNodes;
}


unordered_map<int, vector<OpNode>> OpGraph::getAlapTimesToNodesMap()
{
	unordered_map<int, vector<OpNode>> alapTimesToNodesMap;
	
	for (OpNode* pCurrentNode : operationNodes)
	{
		alapTimesToNodesMap[pCurrentNode->alapStartTime].push_back(*pCurrentNode);
	}
	return alapTimesToNodesMap;
}

unordered_map<int, vector<OpNode>> OpGraph::getListRTimesToNodesMap()
{
	unordered_map<int, vector<OpNode>> listRTimesToNodesMap;

	for (OpNode* pCurrentNode : operationNodes)
	{
		listRTimesToNodesMap[pCurrentNode->listRStartTime].push_back(*pCurrentNode);
	}
	return listRTimesToNodesMap;
}


//List_R Schedule
void OpGraph::scheduleOperations()
{
	scheduleAlap();

	unordered_map<string, int> compsAvailableMap = { { "mult", 1 }, { "adder", 1 }, { "logic", 1 } };
	unordered_map<string, string> opTokenToCompTypeMap = { { "+", "adder" }, { "-", "adder" }, { "*", "mult" }, {">","logic"}, 
								{ "<", "logic" }, { "==", "logic" }, { "?", "logic" }, { ">>", "logic" }, { "<<", "logic" } };

	int time = 1;
	vector<OpNode*> unscheduledNodes = operationNodes;

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
				string nodesType = opTokenToCompTypeMap[unschedNode->operatorToken];
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
					candidateOps[i]->listRStartTime = time; //Scheduling the OpNode.
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
					//schedule it.
					candidateOps[i]->listRStartTime = time; //Scheduling the OpNode.
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
				}
			}//END scheduling remaining resources.


		}//END looping through component types.

		time++;
	}//END while

}


void OpGraph::scheduleAlap()
{
	vector<OpNode*> unscheduledNodes = operationNodes;

	//Schedule the nodes.
	while (unscheduledNodes.size() > 0)
	{
		//Find the nodes whose successors are all scheduled (or don't have successors), then schedule them.
		for (int i = 0; i < unscheduledNodes.size(); i++)
		{
			OpNode* pUnscheduledNode = unscheduledNodes[i];

			//Check if all successors have been scheduled.
			bool successorsScheduled = true;
			for (OpNode* pSucNode : pUnscheduledNode->successors)
			{
				if (!pSucNode->isAlapScheduled)
				{
					successorsScheduled = false;
				}
			}

			//If successors are scheduled, Schedule the node accordingly.
			if (successorsScheduled)
			{
				//If the node doesn't have successors (it's at the end), schedule based off the latency constraint.
				if (pUnscheduledNode->successors.size() == 0)
				{
					pUnscheduledNode->alapStartTime = (latencyConstraint + 1) - pUnscheduledNode->latency;
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

				//Remove the newly scheduled node from the unscheduled list.
				unscheduledNodes.erase(unscheduledNodes.begin() + i);
				i--; // Correct the index.

			} // END if(successorsScheduled)

		}//END for (int i = 0; i < unscheduledNodes.size(); i++)

	}//END while (unscheduledNodes.size() > 0)
}

