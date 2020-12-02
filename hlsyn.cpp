#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <math.h>
#include "OpGraph.h"

//Prototypes
void Parsing_Input_File(string File, unordered_map <string, unordered_map <string, vector <string>>> theMap, vector <vector <string>> opLines);
bool exist(vector <string> outVars, string tempVar);
void CmdArgChecker(int argc, char* argv[]);
string generateVerilogString(unordered_map<string, unordered_map<string, vector<string>>> varTypeToLengthToNamesMap, unordered_map<int, vector<OpNode>> startTimeToNodesMap, int latency);


using namespace std;

int main(int argc, char* argv[]){
    //Command Argument Checker
    void CmdArgChecker(int argc, char* argv[]);

    unordered_map <string, unordered_map <string, vector <string>>> Variable_Type_Length_Names_Map;
    vector<vector<string>> OprLines;

    Parsing_Input_File(argv[1], Variable_Type_Length_Names_Map, OprLines);

    OpGraph *OPG = new OpGraph(OprLines, stoi(argv[2]));
    OPG -> scheduledAlap();
    OPG -> scheduleOprs();

    unordered_map<int, vector<OpNode>> startTimeNodeMap = OPG -> getListRTimesNodeMap();

    string Verilog_String;
    Verilog_String += generateVerilogString(Variable_Type_Length_Names_Map, startTimeNodeMap, stoi(argv[2]));

    ofstream outFileStream;
    outFileStream.open(argv[3]);
    outFileStream << Verilog_String;
    outFileStream.close();

    return 0;
}

//Protoype Definitions Below
void CmdArgChecker(int argc, char* argv[]){
    string inFile, outFile;

    if (argc == 4)
    {
        inFile = argv[1];
        outFile = argv[3];

        if (!(inFile.substr(inFile.length() - 2) == ".c") || !(outFile.substr(outFile.length() - 2) == ".v"))
        {
            cout << "Usage: " << argv[0] << "<netlistfile>  <latency>  <verilogfile>" << endl;
            exit(0);
        }
    }

    else
    {
        cout << "Incorrect Num of Cmd arguments entered" << endl;
        cout << "Usage: " << argv[0] << "<netlistfile>  <latency>  <verilogfile>" << endl;
        exit(0);
    }
}

//Checks for Existence 
bool exist(vector<string> outVars, string tempVar){
    return (find(outVars.begin(), outVars.end(), tempVar) != outVars.end()) ? true : false;    
}

//parse through the file
void Parsing_Input_File(string File, unordered_map<string, unordered_map<string, vector<string>>> theMap, vector<vector<string>> opLines){

    string inFile = File;
    string outFile;

    vector<string> tempVec, inVars, outVars, variableVars;
    vector<vector<string>> oprLines;
    unordered_map<string, vector<string>> inputVarsSizeMap, outputVarsSizeMap, variableVarsSizeMap;

    unordered_map<string, unordered_map<string, vector<string>>> varTypeSizeNameMap;

    ifstream file(inFile);
    if (!file)
    {
        cout << "Cannot open file.\n";
        std::exit(0);
    }
    else if (file.peek() == std::ifstream::traits_type::eof())
    {
        cout << "Oh No the file is empty!!!\n";
        std::exit(0);
    }

    string str;
    
    while (std::getline(file, str)) 
    {
        istringstream iss(str);
        int stop = 0;

        do
        {
            string sub;
            iss >> sub;
            if ((sub.size() >= 2) && ((sub[0] == '/') && (sub[1] == '/')))
            {
                stop = 1;
                sub = "";
            }
            else
            {
                if (!sub.empty() && (stop == 0))
                {
                    for (int i = 0; i < sub.length(); i++)
                        if (sub[i] == ',')
                        {
                            sub.resize(i);
                        }
                    tempVec.push_back(sub);
                }
                
            }
            
            
        } while (iss);
        

        if (tempVec.empty())
        {
            /* DO NOTHING, MOVE ON TO NEXT LINE */
        }

        /*input map insert*/
        else if (tempVec[0].compare("input") == 0)
        {
            vector<string> subVec;
            for (int i = 0; i < tempVec.size(); i++)
            {
                subVec.push_back(tempVec[i]);
                inVars.push_back(tempVec[i]);
            }
            inputVarsSizeMap.insert({tempVec[1], subVec});
            subVec.clear();
        }

        /*output map insert*/
        else if (tempVec[0].compare("output") == 0)
        {
            vector<string> subVec;
            for (int i = 0; i < tempVec.size(); i++)
            {
                subVec.push_back(tempVec[i]);
                outVars.push_back(tempVec[i]);
            }
            outputVarsSizeMap.insert({tempVec[1], subVec});
            subVec.clear();
        }

        /*variable map insert*/
        else if (tempVec[0].compare("variable") == 0)
        {
            vector<string> subVec;
            for (int i = 0; i < tempVec.size(); i++)
            {
                subVec.push_back(tempVec[i]);
                variableVars.push_back(tempVec[i]);
            }
            variableVarsSizeMap.insert({tempVec[1], subVec});
            subVec.clear();
        }
        
        else
        {
            oprLines.push_back(tempVec);
        }
        
        tempVec.clear();
    }
    
    varTypeSizeNameMap.insert({"input", inputVarsSizeMap});
    varTypeSizeNameMap.insert({"output", inputVarsSizeMap});
    varTypeSizeNameMap.insert({"variable", inputVarsSizeMap});

    for (int i = 0; i < oprLines.size(); i++)
    {
        if (exist(inVars, oprLines[i][0]) == false)
        {
            cout << "Error 0x64"; //Dont forget to write an actual error message for the user
            std::exit(0);
        }
    }

    for (int i = 0; i < oprLines.size(); i++)
    {
        if (oprLines[i].size() == 3)
        {
            if (!(exist(inVars, oprLines[i][2])))   
            {
                cout << "Error 0x33"; //Dont forget to write an actual error message for the user
                std::exit(0);
            }
        }

        if (oprLines[i].size() == 5)
        {
            if (!(exist(inVars, oprLines[i][2])))
            {
                cout << "Error 0x55"; //Dont forget to write an actual error message for the user
                std::exit(0);
            }
        }

        if (oprLines[i].size() == 7)
        {
            if (!(exist(inVars, oprLines[i][2])))
            {
                cout << "Error 0x77"; //Dont forget to write an actual error message for the user
                std::exit(0);
            }    
        }
    }
    theMap = varTypeSizeNameMap;
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
					verilogString += varNames[i];

					if (!((i == varNames.size() - 1) && (loopCount == varSizeToNameMap.size() - 1)))
					{
						verilogString += ", ";
					}

				}
			}
			loopCount++;

		}
		//---> Listing OTHER outputs ports
		
		for (auto varSizeToNamesMapPair : varSizeToNameMap)
		{
			string varSize = varSizeToNamesMapPair.first;
			vector<string> varNames = varSizeToNamesMapPair.second;

			//---> Listing inputs
			if (varType == "output")
			{
				for (unsigned i = 0; i < varNames.size(); i++)
				{
					verilogString += varNames[i] + ", ";

				}
			}
			
		}
	}

	verilogString += ");\n";

	//---Standard inputs---
	verilogString += "input Clk, Rst, Start;\n";
	
	//---Standard outputs---
	verilogString += "output reg Done;\n\n";

	//---> OTHER inputs, outputs, reg variables
	for (auto varTypeToSubMapPair : varTypeToLengthToNamesMap)
	{
		string varType = varTypeToSubMapPair.first;

		unordered_map<string, vector<string>> varSizeToNameMap = varTypeToSubMapPair.second;

		for (auto varSizeToNamesMapPair : varSizeToNameMap)
		{
			string varSize = varSizeToNamesMapPair.first;
			vector<string> varNames = varSizeToNamesMapPair.second;

			//Checking for input variables
			if (varType == "input")
			{
				//Checking if signed or unsigned (REUSABLE)
				verilogString += "input ";
				if (varSize[0] == 'I')
				{
					verilogString += "signed ";
				}
				//Finding bitwidth of variable (REUSABLE)
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
			//Checking for output variables
			else if (varType == "output")
			{
				//Checking if signed or unsigned (REUSABLE)
				verilogString += "output reg ";
				if (varSize[0] == 'I')
				{
					verilogString += "signed ";
				}
				//Finding bitwidth of variable (REUSABLE)
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
			//Checking for reg variables
			else if (varType == "variable")
			{
				//Checking if signed or unsigned (REUSABLE)
				verilogString += "reg ";
				if (varSize[0] == 'I')
				{
					verilogString += "signed ";
				}
				//Finding bitwidth of variable (REUSABLE)
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
	//--->END DECLARING INPUTS, OUTPUTS, REGS<---//


	//---> Declaring State Parameters <---//

	verilogString += "parameter S_WAIT = 0, ";

	int i = 1;
	while (i <= startTimeToNodesMap.size() + 1)
	{
		verilogString += "S_T" + to_string(i) + " = " + to_string(i) + ",";
		//cout << i << endl;
		i++;
	}
	verilogString += " S_FINAL = " + to_string(startTimeToNodesMap.size() + 2) + ";\n";


	//Determining min number of bits needed for State variable
	int minBits = ceil(log2(startTimeToNodesMap.size() + 2));

	verilogString += "reg [" + to_string(minBits - 1) + ":0] State;\n\n";

	

	//---Standard Always block and initial wait state
	verilogString += "always @(posedge Clk) begin\nif(Rst == 1) begin\nState <= S_WAIT;\nDone <= 0;\nend\nelse begin\n";
	verilogString += "case(State)\n";
	verilogString += "S_WAIT : begin\nDone <= 0;\nif(Start == 1) begin\nState <= S_T1;\nend\nend\n";
	

	//Looping through State CASES for operations
	for (auto q : startTimeToNodesMap) 
	{

		verilogString += "S_T" + to_string(q.first) + " : begin\n";
		
		for (int j = 0; j < q.second.size(); j++) 
		{
			
			//Checking # of input variables
			if (q.second[j].inVars.size() == 3)
			{
				verilogString += q.second[j].outVars + " <= " + q.second[j].inVars[0] + " " + q.second[j].OprToken + " " + q.second[j].inVars[1] + " : " + q.second[j].inVars[2] + ";\n";
			}
			else
			{
				verilogString += q.second[j].outVars + " <= " + q.second[j].inVars[0] + " " + q.second[j].OprToken + " " + q.second[j].inVars[1] + ";\n";


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
	
	//---> Standard Ending module
	verilogString += "S_FINAL : begin\nDone <= 1;\nend\nendcase\nend\nend\nendmodule";



	return verilogString;
}