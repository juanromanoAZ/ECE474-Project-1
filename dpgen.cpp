#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <stdio.h>
#include <bits/stdc++.h> 
using namespace std;

//--------------Function(s) Prototypes-----------------

void readLine(string line);

bool Single_Check(vector<string> val1, string val2);
bool Group_Check(vector<string> v1, vector<string> v2, vector<string> v3, vector<string> v4, string tempStr);
bool vect_OutputChecker(vector<vector<string>> vec, string name);
string Generate_Verilog_String(string VerFileName, vector<vector<string>>IN, vector<vector<string>>OUT, vector<vector<string>>WIRE, vector<vector<string>>REG, vector<vector<string>>OPL, vector<string> InVar, vector<string>OutVar);
//-----------------------------------------------------



int main(){


    string netlistFile = "474a_circuit1.txt"; //input file  (CHANGE THESE NAME)
    string verilogFile = "output_file_TEST.v"; //output file (CHANGE THESE NAMES)


    /*Long Term Storage Unallocated Vectors*/
    vector<vector<string>> In_Lines, Out_Lines, Wire_Lines, Reg_Lines, Op_Lines;
    vector<string> InVars, OutVars, WireVars, RegVars;
    vector<string> Operations = {"=", "+", "-", "*", "==", "<<", ">>", "<", ">", "?", ":"}; //List of Valid Operations

    /*Short Term Storage for Manipulation*/
    vector<string> tempVec;
    string tempStr;


    ifstream file(netlistFile);

    if (!file) //checks if file opens correctly
    {
        cout << "Unable to open File." << endl;
        return -1;
    }
    if (file.peek() == ifstream::traits_type::eof()) //check if file is empty
    {
        cout << "The Netlist File is empty." << endl;
        return -1;
    }

    /*Description of While Loop: 
        1.) In this while loop the program will walk through every line until the end of file.
        2.) It will read a line ignoring blank lines or lines with comments.
        3.) Then it will break the line in a vector for manipulation
        4.) Finally it will look at the first element and desipher it where the vector will be store
                (ex: "input" -> [2D-Vector] In_Lines*/
    while (std::getline(file, tempStr)){
        istringstream InSS (tempStr);
        int Skip = 0;

        /*loop walk through the line to seperate words and stare themin a temporary vector*/
        do{
            string sub;
            InSS >> sub;
            if ((sub.size() >= 2) && ((sub[0] == '/') && (sub[1] == '/'))){
                Skip = 1;
                sub = "";
            }
            else{
                if (!sub.empty() && (Skip == 0)){
                    for (int i = 0; i < sub.length(); i++)
                        if (sub[i] == ','){
                            sub.resize(i);
                        }
                    tempVec.push_back(sub);
                }
            }
        } while (InSS);
        
        /*This where it starts to decipher in which longer 2D-Vector is the temporary vector bveing store to*/
        if (tempVec.empty()){
            /* Does Nothing Moves on to Next Line */
        }
        
        else if (tempVec[0].compare("input") == 0){ //Stores to <2D> In_Lines
            In_Lines.push_back(tempVec);
            for (int i = 2; i < tempVec.size(); i++){
                InVars.push_back(tempVec[i]); //Stores input variables in <1D> InVars
            }
        }
        
        else if (tempVec[0].compare("output") == 0){ //Stores to [2D] Out_Lines
            Out_Lines.push_back(tempVec);
            for (int i = 2; i < tempVec.size(); i++){
                OutVars.push_back(tempVec[i]); //Stores output variables in <1D> OutVars
            }
        }
        
        else if (tempVec[0].compare("wire") == 0){ //Stores to [2D] Wire_Lines
            Wire_Lines.push_back(tempVec);
            for (int i = 2; i < tempVec.size(); i++){
                WireVars.push_back(tempVec[i]); //Stores Wires in <1D> WireVars
            }
        }

        else if (tempVec[0].compare("register") == 0){ //Stores to [2D] Reg_Lines
            Reg_Lines.push_back(tempVec);
            for (int i = 2; i < tempVec.size(); i++){
                RegVars.push_back(tempVec[i]); //Stores Registers in <1D>RegVars
            }
        }

        else{
            Op_Lines.push_back(tempVec); //Stores to [2D] Op_Lines
        }

        tempVec.clear();
    }

    /*Description:
        1.) First it would look indicate wether the operation line has 3, 5, or 7 Elements.
        2.) Validates All Operations, Wires, Inputs, Outputs and Registers are correctly named and present*/
    for (int i = 0; i < Op_Lines.size(); i++){

        switch (Op_Lines[i].size()){
            case 3:
                //Validates Operators
                if (!Single_Check(Operations, Op_Lines[i][1])){
                        cout << "Error: An Unidentified Operation has been detected, Double check all operation are croectly shown" << endl;
                        return -1;
                }

                //Detects/Validates Variables
                for (int x = 0; x < 3; x += 2){
                    if (!Group_Check(InVars, OutVars, WireVars, RegVars, Op_Lines[i][x])){
                        cout << "Error: Missing Variable, Double check all your Inputs, Outputs, Wires, and Registers are present with correct spelling" << endl;
                        return -1;
                    }
                }
                break;
            
            
            case 5:
                //Detects/Validates Variables
                for (int x = 0; x < 6; x += 2){
                    if (!Group_Check(InVars, OutVars, WireVars, RegVars, Op_Lines[i][x])){
                        cout << "Error: Missing Variable, Double check all your Inputs, Outputs, Wires, and Registers are present with correct spelling" << endl;
                        return -1;
                    }
                }

                //Validates Operators
                for (int y = 1; y < 4; y += 2){
                    if (!Single_Check(Operations, Op_Lines[i][y])){
                        cout << "Error: An Unidentified Operation has been detected, Double check all operation are croectly shown" << endl;
                        return -1;
                    }
                }
                break;

            
            case 7:
                //Detects/Validates Variables
                for (int x = 0; x < 8; x += 2){
                    if (!Group_Check(InVars, OutVars, WireVars, RegVars, Op_Lines[i][x])){
                        cout << "Error: Missing Variable, Double check all your Inputs, Outputs, Wires, and Registers are present with correct spelling" << endl;
                        return -1;
                    }
                }

                //Validates Operators
                for (int y = 1; y < 6; y += 2){
                    if (!Single_Check(Operations, Op_Lines[i][y])){
                        cout << "Error: An Unidentified Operation has been detected, Double check all operation are croectly shown" << endl;
                        return -1;
                    }
                }
                break;

            default:
                break;
        }
    }
    
    file.close();

    string verilogContent = Generate_Verilog_String(verilogFile, In_Lines, Out_Lines, Wire_Lines, Reg_Lines, Op_Lines, InVars, OutVars);
    cout<< verilogContent << endl;



    // vect_OutputChecker(In_Lines, "In_Lines");
    // vect_OutputChecker(Out_Lines, "Out_Lines");
    // vect_OutputChecker(Wire_Lines, "Wire_Lines");
    // vect_OutputChecker(Reg_Lines, "Reg_Lines");
    // vect_OutputChecker(Op_Lines, "Op_Lines");

    return 0;
}   

bool Single_Check(vector<string> vec, string temp) {
    return (std::find(vec.begin(), vec.end(), temp) != vec.end()) ? true : false;
}

bool Group_Check(vector<string> v1, vector<string> v2, vector<string> v3, vector<string> v4, string tempStr){
    return ( (Single_Check(v1, tempStr) || Single_Check(v2, tempStr) || Single_Check(v3, tempStr) || Single_Check(v4, tempStr)) == false) ? false : true;
}

bool vect_OutputChecker(vector<vector<string>> vec, string name){
    cout << name << " Matrix:" << endl;
    for (int i = 0; i < vec.size(); i++)
    {
        cout << "[";
        for (int j = 0; j < vec[i].size() ; j++)
        {
            std::cout<< vec[i][j] << ", ";
        }
        cout <<"]" << endl;
    }
    cout << "" << endl;
    return true;
}

string Generate_Verilog_String(string VerFileName, vector<vector<string>>IN, vector<vector<string>>OUT, vector<vector<string>>WIRE, 
                                vector<vector<string>>REG, vector<vector<string>>OPL, vector<string> InVar, vector<string>OutVar){
    string VerilogStr = "`timescale 1ns / 1ps \n\n";
    
    int Num = VerFileName.find_last_of("/");
    string circuitName = VerFileName.substr(Num + 1);
    Num = circuitName.find_last_of("\\");
    circuitName = circuitName.substr(Num + 1);

    string moduleName = "module " + circuitName.erase(circuitName.size() - 2) + "("; 
    VerilogStr += moduleName;

    //Checks to see if Rst and Clk is Need (Ex: Register Operation)
    bool Needs_Rst_Clk = false;
    for (int i = 0; i < OPL.size(); i++){
        //As long as one line is a size of 3 (ie Reg Operation) there is a need for Clk, Rst
        if (OPL[i].size() == 3){
            VerilogStr += "Clk, Rst, ";
            Needs_Rst_Clk = true;
            break;
        }
    }

    for (int i = 0; i < InVar.size(); i++){
        VerilogStr += InVar[i] + ", ";
    }
    
    for (int i = 0; i < OutVar.size(); i++){
        VerilogStr += OutVar[i] + ((OutVar[i] == OutVar.back()) ?  + ");\n\n" : ", ");
    }


    /*Declaring Clk and Rst*/
    if (Needs_Rst_Clk){
        VerilogStr += "input Clk, Rst;\n";
    }
    
    /*Declating Input Variables*/
    for (int i = 0; i < IN.size(); i++){
        
        VerilogStr += IN[i][0];
        
        if (IN[i][1][0] == 'I'){
            VerilogStr += " signed ";
        }

        int num = IN[i][1].find_last_of('t');
        string bitWidth = IN[i][1].substr(num + 1);
        VerilogStr += "[" + std::to_string(stoi(bitWidth) - 1) + ":0] ";

        for (int j = 2; j < IN[i].size(); j++){
            
            VerilogStr += IN[i][j];
            
            if (j != (IN[i].size() - 1)){
                VerilogStr += ", ";
            }
        }
        VerilogStr += ";\n";
    }
    VerilogStr += "\n";

    /*Declaring Output Variables*/
    for (int i = 0; i < OUT.size(); i++){

        VerilogStr += OUT[i][0];
        
        if (OUT[i][1][0] == 'I') {
            VerilogStr += " signed ";
        }
        
        int num = OUT[i][1].find_last_of('t');
        string bitWidth = OUT[i][1].substr(num + 1);
        VerilogStr += "[" + std::to_string(stoi(bitWidth) - 1) + ":0] ";

        for (int j = 2; j < OUT[i].size(); j++){
            
            VerilogStr += OUT[i][j];
            
            if (j != (OUT[i].size() - 1)){
                VerilogStr += ", ";
            }
        }
        VerilogStr += ";\n";
        
    }
    VerilogStr += "\n";

    /*Declaring Wire Variables*/
    for (int i = 0; i < WIRE.size(); i++){
        
        VerilogStr += WIRE[i][0];
        
        if (WIRE[i][1][0] == 'I'){
            VerilogStr += " signed ";
        }

        int num = WIRE[i][1].find_last_of('t');
        string bitWidth = WIRE[i][1].substr(num + 1);
        VerilogStr += "[" + std::to_string(stoi(bitWidth) - 1) + ":0] ";

        for (int j = 0; j < WIRE[i].size(); j++){
            
            VerilogStr += WIRE[i][j];
            
            if (j != (WIRE[i].size() - 1)){
                VerilogStr += ", ";
            }
        }
        VerilogStr += ";\n";
    }
    VerilogStr += "\n";

    /*Declaring Registers*/
    for (int i = 0; i < REG.size(); i++){
        
        VerilogStr += REG[i][0];
        
        if (REG[i][1][0] == 'I'){
            VerilogStr += " signed ";
        }

        for (int j = 0; j < REG[i].size(); j++){
            
            VerilogStr += REG[i][j];
            
            if (j != (REG[i].size() - 1)){
                VerilogStr += ", ";
            }
        }
        VerilogStr += ";\n";
    }
    VerilogStr += "\n";
    
    




    return VerilogStr;
}