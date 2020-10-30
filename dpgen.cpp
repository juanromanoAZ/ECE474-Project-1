#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>

#include <bits/stdc++.h> 

using namespace std;

void readLine(string line){
    string verilog_string;
    // string space = " ";
    // string comma_space = ", ";

    if (line.find("+") != std::string::npos) //ADD
    {
        cout << line + "---ADD"<< endl;
    }
    else if (line.find("-") != std::string::npos) //SUB
    {
        cout << line + "---SUB"<< endl;
    }
    else if (line.find("*") != std::string::npos) //MUL
    {
        cout << line + "---MUL"<< endl;
    }

    else if (line.find(">" || "<" || "==") != std::string::npos) //COMP 
    {
        cout << line + "---COMP"<< endl;
    }

    else if (line.find("?") != std::string::npos) //MUX
    {
        cout << line + "---MUX"<< endl;
    }

    else if (line.find(">>") != std::string::npos) //SHR
    {
        cout << line + "---SHR"<< endl;
    }

    else if (line.find("<<") != std::string::npos) //SHL
    {
        cout << line + "---SHL"<< endl;
    }

    else
    {
        cout << line + "---REG"<< endl;
    }
    
}










int main(){


    
    ifstream netlistFile;
    ofstream verilogFile;

    fstream fileName;
    fileName.open("474a_circuit1.txt", ios::in);

    cout << (fileName.is_open() ? "File is opened" : "File can not be opened") << endl;

    
    
    for (size_t i = 0; i < 9; i++)
    {
        string tp;
        getline(fileName, tp);
        if ((tp.find("input") != std::string::npos) || (tp.find("output") != std::string::npos) || (tp.find("wire") != std::string::npos))
        {
            cout << tp + "    " + "contains" << endl;
        }
        else
        {
            cout << "NO!!!" << endl;
        }
        
    }
    
    

    
    // while (!fileName.eof())
    // {
    //     string tp;
    //     getline(fileName, tp);
    //     readLine(tp);
    // }
    
    

    return 0;
}


