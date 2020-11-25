#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

//Prototypes
void Parsing_Input_File(string File, unordered_map<string, unordered_map<string, vector<string>>> theMap, vector<vector<string>> opLines);
bool exist(vector<string> outVars, string tempVar);

using namespace std;

int main(){
    //Command Argument Checker
    unordered_map<string, unordered_map<string, vector<string>>> Variable_Type_Length_Names_Map;
    vector<vector<string>> OperationLines;


    


}

//Protoype Definitions Below

//Checks for Existence 
bool exist(vector<string> outVars, string tempVar){
    return (find(outVars.begin(), outVars.end(), tempVar) != outVars.end()) ? true : false;    
}

void Parsing_Input_File(string File, unordered_map<string, unordered_map<string, vector<string>>> theMap, vector<vector<string>> opLines){
    string inFile = File;
    string outFile;
    vector<string> tempVec;
    vector<string> inVars, outVars, variableVars;

    unordered_map<string, vector<string>> inputVarsSizeMap, outputVarsSizeMap, variableVarsSizeMap;

    unordered_map<string, unordered_map<string, vector<string>>> varTypeSizeNameMap;

}