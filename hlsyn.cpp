#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <math.h>
#include "OpGraph.h"

using namespace std;

#pragma region {FUNCTIONS PROTOTYPES}
void parseInputFile(string fileName, unordered_map<string, unordered_map<string, vector<string>>> & varTypeToLengthToNamesMap, vector<vector<string>> &operationLines);
void CommandArgCheck(int argc, char* argv[]);
void parseInputFile(string fileName, unordered_map<string, unordered_map<string, vector<string>>> & varTypeToLengthToNamesMap, vector<vector<string>> &operationLines);
bool existence(vector <string> outputVars, string tempVars);
string generateVerilogString(unordered_map<string, unordered_map<string, vector<string>>> varTypeToLengthToNamesMap, unordered_map<int, vector<OpNode>> startTimeToNodesMap, int latency);
#pragma endregion

int main(int argc, char* argv[]){
    //Command Argument Checker
    void CommandArgCheck(int argc, char* argv[]);

    unordered_map <string, unordered_map <string, vector <string>>> Variable_Type_Length_Names_Map;
    vector<vector<string>> OprLines;

    parseInputFile(argv[1], Variable_Type_Length_Names_Map, OprLines);

    OpGraph *OPG = new OpGraph(OprLines, stoi(argv[2]));
    OPG -> scheduleAlap();
    OPG -> scheduleOperations();

    unordered_map<int, vector<OpNode>> startTimeNodeMap = OPG -> getListRTimesToNodesMap();

    string Verilog_String;
    Verilog_String += generateVerilogString(Variable_Type_Length_Names_Map, startTimeNodeMap, stoi(argv[2]));

    ofstream outFileStream;
    outFileStream.open(argv[3]);
    outFileStream << Verilog_String;
    outFileStream.close();

    return 0;
}


#pragma region {FUNCTIONS}
void CommandArgCheck(int argc, char* argv[])
{
	//===================> CHECKING COMMAND ARGUMENTS<===================//

	string inFileName;;
	string outFileName;


	if (argc == 4)
	{
		inFileName = argv[1];
		outFileName = argv[3];
		if (!(inFileName.substr(inFileName.length() - 2) == ".c") || !(outFileName.substr(outFileName.length() - 2) == ".v"))
		{
			cout << "Usage: " << argv[0] << " <netlistFile>  <latency>  <verilogFile> " << endl;
			exit(0);
		}
	}
	else //Incorrect # of command arguments
	{
		cout << "Incorrect number of command arguments entered" << endl;
		cout << "Usage: " << argv[0] << " <netlistFile>  <latency>  <verilogFile> " << endl;
		exit(0);
	}
	//=================> END CHECKING COMMAND ARGUMENTS<=================//
}

void parseInputFile(string fileName, unordered_map<string, unordered_map<string, vector<string>>> & varTypeToLengthToNamesMap, vector<vector<string>> &operationLines){
    string inFileName = fileName;
    string outFileName;
    vector<string> tempVector;
    vector<string> inputVars;
    vector<string> outputVars;
    vector<string> variableVars;

    
    //------------UINT16--<a,b,c>
    unordered_map<string, vector<string>> inputvarSizeToNamesMap;
    unordered_map<string, vector<string>> outputvarSizeToNamesMap;
    unordered_map<string, vector<string>> variablesvarSizeToNamesMap;
    
    //------------input-----varSizeToNamesMap
    unordered_map<string, unordered_map<string, vector<string>>> varTypeToSizeToNamesMap;
    
    ifstream file(inFileName);
    if (!file)
    {
        cout << "Unable to open the file.\n";
        std::exit(0);
    }
   else if (file.peek() == std::ifstream::traits_type::eof()) // Check if file is empty.
    {
        cout << "The input file was empty. Uh-oh!!!\n";
        std::exit(0);
    
    }
    
    string str;
    
    while (std::getline(file, str)) {
        //std::cout << str << "\n";
        istringstream iss(str);
        int stop = 0;
        ///seperate individual words
        do
        {
            string subs;
            iss >> subs;
            if ((subs.size() >= 2) && ((subs[0] == '/') && (subs[1] == '/'))){
                
                stop = 1;
                subs = "";
            }
            else{
                
                if (!subs.empty() && (stop == 0)){
                    for (int i = 0; i<subs.length(); i++)
                        if (subs[i] == ','){
                            subs.resize(i);
                        }
                    tempVector.push_back(subs); 
                    //cout << "Substring: " << subs << endl;
                }
            }
        } while (iss);

        if (tempVector.empty()){ // Check if tempVector is null
            //Do Nothing, go to next line.
        }

        else if (tempVector[0].compare("input") == 0){
            vector<string> subVector;
            //cout<< tempVector[0];
            for (int i = 2; i < tempVector.size(); i++){
                subVector.push_back(tempVector[i]);
                inputVars.push_back(tempVector[i]);
            }
            inputvarSizeToNamesMap.insert({tempVector[1], subVector});
            
            subVector.clear();
        }
        //// output map insert
        else if (tempVector[0].compare("output") == 0){
            vector<string> subVector;
            //cout<< tempVector[0];
            for (int i = 2; i < tempVector.size(); i++){
                subVector.push_back(tempVector[i]);
                inputVars.push_back(tempVector[i]);

            }
            outputvarSizeToNamesMap.insert({tempVector[1], subVector});
            
            subVector.clear();
        }
        else if (tempVector[0].compare("variable") == 0){
            vector<string> subVector;
            //cout<< tempVector[0];
            for (int i = 2; i < tempVector.size(); i++){
                subVector.push_back(tempVector[i]);
                inputVars.push_back(tempVector[i]);

            }
            variablesvarSizeToNamesMap.insert({tempVector[1], subVector});
            
            subVector.clear();
        }
        //// variable map insert
        
        else{
            operationLines.push_back(tempVector);
        }
        
        tempVector.clear();
        
    }
    varTypeToSizeToNamesMap.insert({"output", outputvarSizeToNamesMap});
    varTypeToSizeToNamesMap.insert({"variable", variablesvarSizeToNamesMap});
    varTypeToSizeToNamesMap.insert({"input", inputvarSizeToNamesMap});
    /////// START Check if variables exist
    ///// check left side of equal sign if it is an output, wire, register variable
    for (int i = 0; i < operationLines.size(); i++){
        if (existence(inputVars, operationLines[i][0]) == false){
            cout << "Error in input file (0x64)";
            std::exit(0);
        }
    }
    for (int i = 0; i < operationLines.size(); i++){
        
        if (operationLines[i].size() == (3)){
            if (!((existence(inputVars, operationLines[i][2])))){
                cout << "Error found (0x33)" << endl;
                std::exit(0);
                
            }
        }
            if (operationLines[i].size() == (5)){
                if (!((existence(inputVars, operationLines[i][4])))){
                    cout << "Error found (0x55)" << endl;
                    std::exit(0);
                    
                }
                
            }
            if (operationLines[i].size() == (7)){
                if (!((existence(inputVars, operationLines[i][6])))){
                    cout << "Error found (0x77)" << endl;
                    std::exit(0);
                }
        }
    
    }
    varTypeToLengthToNamesMap = varTypeToSizeToNamesMap;       
}

bool existence(vector <string> outputVars, string tempVars){
    if (std::find(outputVars.begin(), outputVars.end(), tempVars) != outputVars.end())
        return true;
    else
        return false;
}

string generateVerilogString(unordered_map<string, unordered_map<string, vector<string>>> varTypeToLengthToNamesMap, unordered_map<int, vector<OpNode>> startTimeToNodesMap, int latency)
{
	string verilogString;
	//---Standard module header---
	verilogString += "`timescale 1ns / 1ps \n\n\n";

	//---Standard module declaration---
	verilogString += "module HLSM(Clk, Rst, Start, Done,  ";

	//---> OTHER inputs and outputs for port declaration
	for (auto varTypeToSubMapPair : varTypeToLengthToNamesMap)
	{
		string varType = varTypeToSubMapPair.first;
		unordered_map<string, vector<string>> varSizeToNameMap = varTypeToSubMapPair.second;

		int loopCount = 0;
		for (auto varSizeToNamesMapPair : varSizeToNameMap)
		{
			string varSize = varSizeToNamesMapPair.first;
			vector<string> varNames = varSizeToNamesMapPair.second;

			//---> Listing OTHER inputs ports
			if (varType == "input")
			{
				for (unsigned i = 0; i < varNames.size(); i++)
				{
					verilogString += varNames[i] + ", ";
				}
			}
			loopCount++;
		}
		
		loopCount = 0;
		for (auto varSizeToNamesMapPair : varSizeToNameMap)
		{
			string varSize = varSizeToNamesMapPair.first;
			vector<string> varNames = varSizeToNamesMapPair.second;

			//---> Listing inputs
			if (varType == "output")
			{
				for (unsigned i = 0; i < varNames.size(); i++)
				{
					verilogString += varNames[i];

					if (!((i == (varNames.size() - 1)) && (loopCount == varSizeToNameMap.size() - 1)))
					{
						verilogString += ", ";
					}
				}
			}
			loopCount++;
		}
	}

	verilogString += ");\n";
	verilogString += "input Clk, Rst, Start;\n";
	verilogString += "output reg Done;\n\n";

	for (auto varTypeToSubMapPair : varTypeToLengthToNamesMap)
	{
		string varType = varTypeToSubMapPair.first;

		unordered_map<string, vector<string>> varSizeToNameMap = varTypeToSubMapPair.second;

		for (auto varSizeToNamesMapPair : varSizeToNameMap)
		{
			string varSize = varSizeToNamesMapPair.first;
			vector<string> varNames = varSizeToNamesMapPair.second;

			if (varType == "input")
			{
				verilogString += "input ";
				if (varSize[0] == 'I')
				{
					verilogString += "signed ";
				}
				size_t found = varSize.find_last_of("t");
				string bitWidth = varSize.substr(found + 1);

				verilogString += "[" + to_string(stoi(bitWidth) - 1) + ":0]";

				for (unsigned i = 0; i < varNames.size(); i++)
				{
					verilogString += varNames[i];

					if (!(i == varNames.size() - 1))
					{
						verilogString += ", ";
					}
					else {
						verilogString += ";\n";
					}
				}
				verilogString += "\n";
			}

			else if (varType == "output")
			{
				verilogString += "output reg ";
				if (varSize[0] == 'I')
				{
					verilogString += "signed ";
				}
				size_t found = varSize.find_last_of("t");
				string bitWidth = varSize.substr(found + 1);

				verilogString += "[" + to_string(stoi(bitWidth) - 1) + ":0]";
				for (unsigned i = 0; i < varNames.size(); i++)
				{
					verilogString += varNames[i];

					if (!(i == varNames.size() - 1))
					{
						verilogString += ", ";
					}
					else {
						verilogString += ";\n";
					}
				}
				verilogString += "\n";
			}

			else if (varType == "variable")
			{
				verilogString += "reg ";
				if (varSize[0] == 'I')
				{
					verilogString += "signed ";
				}
				size_t found = varSize.find_last_of("t");
				string bitWidth = varSize.substr(found + 1);
				verilogString += "[" + to_string(stoi(bitWidth) - 1) + ":0]";
				for (unsigned i = 0; i < varNames.size(); i++)
				{
					verilogString += varNames[i];

					if (!(i == varNames.size() - 1))
					{
						verilogString += ", ";
					}
					else {
						verilogString += ";\n";
					}

				}
				verilogString += "\n";
			}

		}
	}

	verilogString += "\n";
	verilogString += "parameter S_WAIT = 0, ";

	int i = 1;
	while (i <= startTimeToNodesMap.size() + 1)
	{
		verilogString += "S_T" + to_string(i) + " = " + to_string(i) + ",";
		i++;
	}
	verilogString += " S_FINAL = " + to_string(startTimeToNodesMap.size() + 2) + ";\n";

	int minBits = ceil(log2(startTimeToNodesMap.size() + 2));

	verilogString += "reg [" + to_string(minBits - 1) + ":0] State;\n\n";

	verilogString += "always @(posedge Clk) begin\nif(Rst == 1) begin\nState <= S_WAIT;\nDone <= 0;\nend\nelse begin\n";
	verilogString += "case(State)\n";
	verilogString += "S_WAIT : begin\nDone <= 0;\nif(Start == 1) begin\nState <= S_T1;\nend\nend\n";
	

	//Looping through State CASES for operations
	for (auto q : startTimeToNodesMap) 
	{
		verilogString += "S_T" + to_string(q.first) + " : begin\n";	
		for (int j = 0; j < q.second.size(); j++) 
		{
			if (q.second[j].inputVars.size() == 3)
			{
				verilogString += q.second[j].outputVar + " <= " + q.second[j].inputVars[0] + " " + q.second[j].operatorToken + " " + q.second[j].inputVars[1] + " : " + q.second[j].inputVars[2] + ";\n";
			}
			else
			{
				verilogString += q.second[j].outputVar + " <= " + q.second[j].inputVars[0] + " " + q.second[j].operatorToken + " " + q.second[j].inputVars[1] + ";\n";
			}
		}
		if (q.first != latency) 
		{
			int nextState = q.first + 1;
			verilogString += "State <= S_T" + to_string(nextState) + ";\n";
			verilogString += "end\n";
		}
		else 
		{
			verilogString += "State <= S_FINAL;\nend\n";
		}
	}
	verilogString += "S_FINAL : begin\nDone <= 1;\nend\nendcase\nend\nend\nendmodule";

	return verilogString;
}

#pragma endregion
//EOF