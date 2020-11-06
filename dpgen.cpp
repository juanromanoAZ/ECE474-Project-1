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
unordered_map<string, string> GenSizeType(vector<vector<string>>IN, vector<vector<string>>OUT, vector<vector<string>>WIRE, vector<vector<string>>REG);
string GenBitWidth(string Token);
string GenInParamStr(int compBitWidth, int InBitWidth, string InVar, string Token);
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





string Generate_Verilog_String(string VerFileName, vector<vector<string>>IN, vector<vector<string>>OUT, vector<vector<string>>WIRE, vector<vector<string>>REG, vector<vector<string>>OPL, vector<string> InVar, vector<string>OutVar){
    
    
    #pragma region Start of Constructing Header for Verilog File
    //========================================================================================
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
        
        if (WIRE[i][1][0] == 'I') {
            VerilogStr += " signed ";
        }
        
        int num = WIRE[i][1].find_last_of('t');
        string bitWidth = WIRE[i][1].substr(num + 1);
        VerilogStr += "[" + std::to_string(stoi(bitWidth) - 1) + ":0] ";

        for (int j = 2; j < WIRE[i].size(); j++){
            
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
    //========================================================================================
    #pragma endregion

    VerilogStr += "\n";


    unordered_map<string, string> Variable_Size_And_Type_Map = GenSizeType(IN, OUT, WIRE, REG);
    unordered_map<string, int> Num_Components_Used = {  {"REG", 0},  {"MUX2x1", 0},
                                                        {"ADD", 0},  {"SADD", 0},
                                                        {"SUB", 0},  {"SSUB", 0},
                                                        {"MUL", 0},  {"SMUL", 0},
                                                        {"COMP", 0}, {"SCOMP", 0},
                                                        {"SHR", 0},  {"SHL", 0}}      ;

    for (int i = 0; i < OPL.size(); i++)
    {
        vector<string> Temp_Op_Token = OPL[i];

        //Generates Verilog code for Register Component
        if (Temp_Op_Token.size() == 3)
        {   
            //Gathers the output and input.
            string OutV = Temp_Op_Token[0];
            string InV = Temp_Op_Token[2];

            //Gets the size and type of output.
            string Out_Size_Type_Token = Variable_Size_And_Type_Map[OutV];
            string In_Size_Type_Token = Variable_Size_And_Type_Map[InV];

            //Gets Bit Width componenets.
            string Out_BitWidth = GenBitWidth(Out_Size_Type_Token);
            string In_BitWidth = GenBitWidth(In_Size_Type_Token);

            //Determine if the input param widths needs to be zero padded, sign extended, or truncated.
            string InParamStr = GenInParamStr(stoi(Out_BitWidth), stoi(In_BitWidth), InV, In_Size_Type_Token);

            //!!!!!! DOUBLE CHECK THE PARAMETERS ARE IN THE CORRENT ORDER
            //Generate Verilog module instiation.
            VerilogStr += "REG #(.DATAWIDTH(" + Out_BitWidth + ")) reg" + to_string(Num_Components_Used["REG"]) + "(" + InParamStr + ", Clk, Rst, " + OutV + "); \n";
            
            //Increments the number of used for module REG.
            Num_Components_Used["REG"]++;
        }

        //Generates Verilog code for MUX2x1 Component
        else if (Temp_Op_Token.size() == 7)
        {
            //Gathers each individual inputs, output, and selector. 
            string OutV = Temp_Op_Token[0];
            string InVA = Temp_Op_Token[6];
            string InVB = Temp_Op_Token[4];
            string SelVar = Temp_Op_Token[2];

            //gets the size and type due to the inputs.
            string In_A_Size_Type_Token = Variable_Size_And_Type_Map[InVA];
            string In_B_Size_Type_Token = Variable_Size_And_Type_Map[InVB];

            //Gets Bit width of the MUX2x1 component is the width of the ouput variable.
            string Out_BitWidth = GenBitWidth(Variable_Size_And_Type_Map[OutV]);
            string A_BitWidth = GenBitWidth(Variable_Size_And_Type_Map[InVA]);
            string B_BitWidth = GenBitWidth(Variable_Size_And_Type_Map[InVB]);

            //Determine if inputs param widths needs to be zero padded, sign extended, or truncated.
            string In_A_ParamStr = GenInParamStr(stoi(Out_BitWidth), stoi(A_BitWidth), InVA, In_A_Size_Type_Token);
            string In_B_ParamStr = GenInParamStr(stoi(Out_BitWidth), stoi(B_BitWidth), InVB, In_B_Size_Type_Token);

            //Generate Verilog module instiation.
            VerilogStr += "MUX2x1 #(.DATAWIDTH(" + Out_BitWidth + ") " + "mux" + to_string(Num_Components_Used["MUX2x1"]) + "(" + In_A_ParamStr + ", " + In_B_ParamStr + ", " + SelVar + ", " + OutV + ");\n";
            
            //Increments the number of used for module MUX2x1.
            Num_Components_Used["MUX2x1"]++;
        }
        
        //Generates Verilog code for all other operations (ie +, -, *, >, <, ==, >>, <<) Component.
        else if (Temp_Op_Token.size() == 5)
        {        
            string Out = Temp_Op_Token[0];
            string InA = Temp_Op_Token[2];
            string InB = Temp_Op_Token[4];
            string Op_Symbol = Temp_Op_Token[3];

            string In_A_Size_Type_Token = Variable_Size_And_Type_Map[InA];
            string In_B_Size_Type_Token = Variable_Size_And_Type_Map[InB];
            string Out_Size_Type_Token = Variable_Size_And_Type_Map[Out];

            string InA_BitWidth = GenBitWidth(In_A_Size_Type_Token);
            string InB_BitWidth = GenBitWidth(In_B_Size_Type_Token);
            string Out_BitWidth = GenBitWidth(Out_Size_Type_Token);
            string BitWidth, OutParamStr, CompType, lowCompType;

            if (Op_Symbol == ">" || Op_Symbol == "<" || Op_Symbol == "==")
            {
                BitWidth = (stoi(InA_BitWidth) > stoi(InB_BitWidth)) ? InA_BitWidth : InB_BitWidth;

                string InA_ParamStr = GenInParamStr(stoi(BitWidth), stoi(InA_BitWidth), InA, In_A_Size_Type_Token);
                string InB_ParamStr = GenInParamStr(stoi(BitWidth), stoi(InB_BitWidth), InB, In_B_Size_Type_Token);

                if (Op_Symbol == ">")
                {
                    OutParamStr = ".gt(" + Out + "), 0, 0";
                }
                else if (Op_Symbol == "<")
                {
                    OutParamStr = "0, .lt(" + Out + "), 0";
                }
                else
                {
                    OutParamStr = "0, 0, .eq(" + Out + ")";
                }
                
                if (In_A_Size_Type_Token[0] == 'I' || In_B_Size_Type_Token[0] == 'I')
                {
                    CompType = "SCOMP";
                    lowCompType = "scomp";
                }
                else
                {
                    CompType = "COMP";
                    lowCompType = "comp";
                }
                
                VerilogStr += CompType + "#(.DATAWIDTH(" + BitWidth + ")) " + lowCompType + to_string(Num_Components_Used[CompType]) + "(" + InA_ParamStr + ", " + InB_ParamStr + ", " + OutParamStr + ");\n";
                
                Num_Components_Used[(CompType == "SCOMP") ? "SCOMP" : "COMP" ]++;
                

            }
            else
            {
                unordered_map <string, string> TempMap = {{ "+", "ADD" }, { "-", "SUB" },{ "*", "MUL" },{ ">>", "SHR" },{ "<<", "SHL" }};
                unordered_map <string, string> lowTempMap = {{ "+", "add" }, { "-", "sub" },{ "*", "mul" },{ ">>", "shr" },{ "<<", "shl" }};
                BitWidth = Out_BitWidth;
                string InA_ParamStr = GenInParamStr(stoi(BitWidth), stoi(InA_BitWidth), InA, In_A_Size_Type_Token);
				string InB_ParamStr = GenInParamStr(stoi(BitWidth), stoi(InB_BitWidth), InB, In_B_Size_Type_Token);
                
                string OpType = TempMap[Op_Symbol];
                string lowerOpType = lowTempMap[Op_Symbol];
                
                if ((In_A_Size_Type_Token[0] == 'I') || (In_B_Size_Type_Token[0] == 'I'))
                {
                    if (OpType != "<<" && OpType != ">>")
                    {
                        OpType = "S" + OpType;
                        lowerOpType = "s" + lowerOpType;
                    }
                    
                    VerilogStr += OpType + "#(.DATAWIDTH(" + BitWidth + ")) " + lowerOpType + to_string(Num_Components_Used[OpType]) + "(" + InA_ParamStr + ", " + InB_ParamStr + ", " + Out + ");\n";

                    Num_Components_Used[OpType]++;
                }
            }
        }

        else
        {
            cout << "Error: Unkwoned";
        }
        
    }
    
    VerilogStr += "\nendmodule";
    return VerilogStr;
}

#pragma region Functions to Manipulate DATA

bool Single_Check(vector<string> vec, string temp) {
    
    return (std::find(vec.begin(), vec.end(), temp) != vec.end()) ? true : false;
}

bool Group_Check(vector<string> v1, vector<string> v2, vector<string> v3, vector<string> v4, string tempStr){
    
    return ( (Single_Check(v1, tempStr) || Single_Check(v2, tempStr) || Single_Check(v3, tempStr) || Single_Check(v4, tempStr)) == false) ? false : true;
}

unordered_map<string, string> GenSizeType(vector<vector<string>>IN, vector<vector<string>>OUT, vector<vector<string>>WIRE, vector<vector<string>>REG){
    // Creating a temporary Map or Matrix {{"a", "UInt8"}, {"b", "UInt16"}, ....}
    unordered_map<string, string> Temporary_Map;
     
    //Adds every Inputs to the Map
    for (int i = 0; i < IN.size(); i++){
        
        string TAS = IN[i][1];
        
        //Every variable in the current line gets added to the Map.
        for (int j = 2; j < IN[i].size(); j++){
            
            Temporary_Map[IN[i][j]] = TAS;
        }
    }
    
    //Adds every Outputs to the Map
    for (int i = 0; i < OUT.size(); i++){
        
        string TAS = OUT[i][1];
        
        //Every variable in the current line gets added to the Map.
        for (int j = 2; j < OUT[i].size(); j++){
            
            Temporary_Map[OUT[i][j]] = TAS;
        }
    }

    //Adds every Wires to the Map
    for (int i = 0; i < WIRE.size(); i++){
        
        string TAS = WIRE[i][1];
        
        //Every variable in the current line gets added to the Map.
        for (int j = 2; j < WIRE[i].size(); j++){
            
            Temporary_Map[WIRE[i][j]] = TAS;
        }
    }

    //Adds every Registers to the Map
    for (int i = 0; i < REG.size(); i++){
        
        string TAS = REG[i][1];
        
        //Every variable in the current line gets added to the Map.
        for (int j = 2; j < REG[i].size(); j++){
            
            Temporary_Map[REG[i][j]] = TAS;
        }
    }

    return Temporary_Map;
}

string GenBitWidth(string Token){
    
    /* Get the Number after the UInt or Int
        Ex: "UInt16" -> "16"
               "Int8 -> "8"
    */
    return (Token[0] == 'U') ? Token.substr(4) : Token.substr(3);
}

string GenInParamStr(int compBitWidth, int InBitWidth, string InVar, string Token){
    
    string ParamStr;
    
    if (InBitWidth > compBitWidth) {
        
        //Ex: b[7:0]
        ParamStr = InVar + "[" + to_string(compBitWidth - 1) + ":0]";
    }

    //Check if its unsigned then it will receive zero padding to the Bit-Width, otherwise if its signed then extends Bit-Width
    else if (InBitWidth < compBitWidth){
        
        if (Token[0] == 'U'){ //unsigned
            
            ParamStr = "{0," + InVar + "}";
        }
        
        else{ //signed
            
            int Difference_Bit = compBitWidth - InBitWidth;
            ParamStr = "{ {" + to_string(Difference_Bit) + "{" + InVar + "[" + to_string(InBitWidth - 1) + "]}}, " + InVar + "}";
        }
    }

    //InBitWidth == compBitWidth
    else{
        
        ParamStr = InVar;
    }
    
    return ParamStr;
}

#pragma endregion
