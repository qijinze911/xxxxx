#include <iostream>
#include <cstring>
#include <map>
#include <string.h>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <vector>
#include <regex>
#include <iomanip>

using namespace std;

ifstream f;
map<string, string> SymbolTable;
int offset;
int count1;
map<int, string> MemoryMap;
int linenum = 1;
int linebefore;
int lineoffset;
int modulenum = 1;
int size;
int usecounts;
string warning;
map <string, string> rule7;
map <string, string> rule4;
map <string, int> countmodule;

void parseerror(int errcode, int lengthy) {
    static char* errstr[] = {
            "NUM_EXPECTED",             // Number expect
            "SYM_EXPECTED",             // Symbol Expected
            "ADDR_EXPECTED",            // Addressing Expected which is A/E/I/R // Symbol Name is too long
            "SYM_TOO_LONG",             // Symbol Name is too long
            "TOO_MANY_DEF_IN_MODULE",   // > 16
            "TOO_MANY_USE_IN_MODULE",   // > 16
            "TOO_MANY_INSTR",           // total num_instr exceeds memory size (512)
    };
    if (lineoffset == 0) {
        linenum = linenum - 1;
        lineoffset = linebefore;
    }
    
    lineoffset = lineoffset - lengthy + 1;
    if (errcode <=6 && errcode >= 4) {
        lineoffset = 1;
    }
    printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode]);
}


string readSym() {
    char r1 = '\0';
    string r2 = "";
    while (!f.eof()) {
        r1 = f.get();
        //cout << r1;
        regex pattern("[A-Za-z0-9]", regex_constants::extended);
        regex pattern1("[A-Za-z]", regex_constants::extended);
        match_results<string::const_iterator> regResult;
        string s(1, r1);
        //cout << s;
        bool bs = regex_match(s, regResult, pattern);
        //cout << bs;
        if (bs == true ){
            r2 = r2 + r1;
            lineoffset = lineoffset + 1;

            //cout << r2 <<endl;
        }

        else if (r1 == ' ' || r1 == '\t') {
            if (r2 != "") {
                lineoffset = lineoffset + 1;
                //cout << r2 <<endl;
                string S(1, r2[0]);
                if ( regex_match(S, regResult, pattern1) == false) {
                parseerror(1, r2.length());
                exit(1);
                }
                if (r2.length() > 16) {
                    parseerror(3, r2.length());
                    exit(1);
                }
                return r2;
            }
        }
        else if (r1 == '\n') {
            linenum = linenum + 1;
            linebefore = lineoffset;
            lineoffset = 0;
            if (r2 != "") {
                //cout << r2 << endl;
                string S(1, r2[0]);
                if ( regex_match(S, regResult, pattern1) == false) {
                parseerror(1, r2.length());
                exit(1);
                }
                if (r2.length() > 16) {
                    parseerror(3, r2.length());
                    exit(1);
                }
                
                return r2;
            }
        }
        else {
            parseerror(1, r2.length());
            exit(1);
        }
    }
        return "@";
}

string readInt() {
    char r1 = '\0';
    string r2 = "";
    while (!f.eof()) {
        r1 = f.get();
        //cout << r1;
        regex pattern("[A-Za-z0-9]", regex_constants::extended);
        regex pattern1(".", regex_constants::extended);
        match_results<string::const_iterator> regResult;
        string s(1, r1);
        //cout << "this is s" <<s;
        bool bs = regex_match(s, regResult, pattern);
        bool bb = regex_match(s, regResult, pattern1);
        //cout << "this is bs" << bs;
        if (bs == true){
            r2 = r2 + r1;
            lineoffset = lineoffset + 1;

            //cout << r2 <<endl;
        }
        else if (r1 == ' ' || r1 == '\t') {
            if (r2 != "") {
                lineoffset = lineoffset + 1;
                //cout << r2 <<endl;
                return r2;
            }

        }
        else if (r1 == '\n') {
            linenum = linenum + 1;
        	//cout<<"this is r1" << r2;
            linebefore = lineoffset;
            //cout<< "linebefore" << linebefore;
            lineoffset = 0;
            if (r2 != "") {

                //cout << r2 << endl;
                return r2;
            }
        }
		
        
    }
    return "@";
}

string readIARE() {
    char r1 = '\0';
    string r2 = "";
    while (!f.eof()) {
        r1 = f.get();
        //cout << r1;
        string s(1, r1);
        if (s == "I" or s == "A" or s == "R" or s == "E"){
            lineoffset = lineoffset + 1;
            r2 = r1 + r2;
        }
        else if (r1 == ' ' || r1 == '\t') {
            if (r2 != "") {
                lineoffset = lineoffset + 1;
                //cout << r2 <<endl;
                return r2;
            }

        }
        else if (r1 == '\n') {
            linenum = linenum + 1;
            linebefore = lineoffset;
            //cout<< linebefore;
            lineoffset = 0;
            if (r2 != "") {

                //cout << r2 << endl;
                return r2;
            }
        }
        else {
            //cout << r2;
            parseerror(2, r2.length());
            exit(1);
        }
    }
    return "@";
}


void pass1() {
    while(!f.eof()){
        string sdefcount = readInt();
        if (sdefcount == "@"){
        	break;
        }
        //cout << "this is r2" << r2;
        if (sdefcount.find_first_not_of("1234567890") != string::npos) {
            parseerror(0, sdefcount.length());
            exit(1);
        }
        //cout<<"this is sdefcount" << sdefcount;
        //cout << sdefcount;
        int defcount = atoi(sdefcount.c_str());
        //cout << defcount;
        if(defcount > 16) {
            parseerror(4, sdefcount.length());
            exit(1);
            //cout << "too long" << endl;
            //exit(1);
        }

        while (defcount > 0) {
            string symbol = readSym();
            //cout << "this is symbol" << symbol;
            string svalue = readInt();
            //cout << "this is r2" << r2;
            if (svalue.find_first_not_of("1234567890") != string::npos) {
                parseerror(0, svalue.length());
                exit(1);
            }
            //cout << "this is" << svalue;
            int value = atoi(svalue.c_str());
            //cout << value << endl;
            offset = offset + value;
            map<string, string>::iterator iterrule2 = SymbolTable.find(symbol);
            if(iterrule2 != SymbolTable.end()){
                SymbolTable[symbol] = SymbolTable[symbol] + " Error: This variable is multiple times defined; first value used";
            }
            else{
                SymbolTable[symbol] = to_string(offset);
            }

            offset = offset - value;
            defcount --;
        }

        string susecount = readInt();
        //cout << "this is r2" << r2;
        if (susecount.find_first_not_of("1234567890") != string::npos) {
            parseerror(0, susecount.length());
            exit(1);
        }
        //cout << susecount;
        int usecount = atoi(susecount.c_str());

        if(usecount > 16) {
            parseerror(5, susecount.length());
            exit(1);
        }
        //cout << "this is usecount" << usecount;
        while (usecount > 0) {
            string symbol = readSym();
            usecount --;
        }

        string scodecount = readInt();
        //cout << "this is r2" << r2;
        if (scodecount.find_first_not_of("1234567890") != string::npos) {
            parseerror(0, scodecount.length());
            exit(1);
        }
        //cout << scodecount;
        int codecount = atoi(scodecount.c_str());
        offset = offset + codecount;
        if (offset > 512) {
            parseerror(6, susecount.length());
            exit(1);
        }
        map<string, string>::iterator iterrule5 = SymbolTable.begin();
        while(iterrule5 != SymbolTable.end()) {
            if (offset <= atoi((*iterrule5).second.c_str())){
                cout << "Warning: Module " << modulenum << ": " << (*iterrule5).first << " too big " 
                << atoi((*iterrule5).second.c_str()) << " (max=" << (codecount-1) <<") assume zero relative" << endl;
                SymbolTable[(*iterrule5).first] = to_string(offset - codecount);
            }
            iterrule5++;
        }
        while (codecount > 0){
            string IARE = readIARE();
            //cout << IARE;
            string address = readInt();
            if (address.find_first_not_of("1234567890") != string::npos) {
                parseerror(0, address.length());
                exit(1);
            }
            codecount--;
        }


        modulenum++;
    }
    offset = 0;
    modulenum = 1;
}

void pass2(){
    string s;
    while(f>>s){
        int defcount = atoi(s.c_str());
        while (defcount > 0){
            string defsymbol = readSym();
            map<string,string>::iterator rule4iter = rule4.find(defsymbol);
            if(rule4iter == rule4.end()){
                rule4[defsymbol] = "not used";
            }
            map<string,int>::iterator counting = countmodule.find(defsymbol);
            if(counting == countmodule.end()){
                countmodule[defsymbol] = modulenum;
            }
            readInt();

            defcount--;
        }
        int usecount = atoi(readInt().c_str());
        string useSymbol[usecount];
        usecounts = usecount;
        int ucount = 0;
        while (usecount > 0) {
            string symbol = readSym();
            rule4[symbol] = "used";
            cout << "this is symbol "<<symbol << endl;
            useSymbol[ucount] = symbol;
            map<string,string>::iterator rule7iter = rule7.find(symbol);
            if(rule7iter == rule7.end()){
                rule7[symbol] = "not used";
            }
            //cout << useSymbol[0] << endl;
            usecount--;
            ucount++;
        }
        int codecount = atoi(readInt().c_str());
        int tmp = codecount;
        while (codecount > 0){
            string IARE = readIARE();
            string saddress = readInt();
            int address = atoi(saddress.c_str());
            int operand = address % 1000;
            int opcode = address / 1000;
            //cout << IARE << "this is IARE";
            if (IARE == "I") {
                if (address >= 10000){
                    address = 9999;
                    MemoryMap[count1] = to_string(address) + " Error: Illegal immediate value; treated as 9999";
                }
                else{
                	string addr = "0000" + to_string(address);
                	addr = addr.substr(addr.length()-4);
                    MemoryMap[count1] = addr;
                    //cout << MemoryMap[1];
                }

            }

            if (IARE == "A") {
                if (address >= 10000){
                    address = 9999;
                    MemoryMap[count1] = to_string(address) + " Error: Illegal opcode; treated as 9999";
                }
                else {
                    if (operand >512) {
                        MemoryMap[count1] = to_string(opcode) + "000" + " Error: Absolute address exceeds machine size; zero used";
                    }
                    else {
                        MemoryMap[count1] = to_string(address);
                    }
                }

            }

            if (IARE == "R") {
                //cout << "saddress is" << saddress;
                if (address >= 10000){
                    address = 9999;
                    MemoryMap[count1] = to_string(address) + " Error: Illegal opcode; treated as 9999";
                }
                else{
                    if (operand > tmp) {
                        MemoryMap[count1] = to_string(opcode*1000 + offset) + " Error: Relative address exceeds module size; zero used";
                    }
                    else{
                        // cout << "address1 is" << address;
                        //cout << "offset is" << offset;
                        address = offset + address;
                        //cout << "address is" << address;
                        // cout << "count1 is" << count1;
                        string addr = "0000" + to_string(address);
                		addr = addr.substr(addr.length()-4);
                    	MemoryMap[count1] = addr;
                    }
                }


            }

            if(IARE == "E") {
                if (address >= 10000){
                    address = 9999;
                    MemoryMap[count1] = to_string(address) + " Error: Illegal opcode; treated as 9999";
                }
                else{
                    //cout<< "this is usesymbo" << useSymbol[operand];
                    if (operand >= usecounts) {
                    	string addr = "0000" + to_string(address);
                		addr = addr.substr(addr.length()-4);
                    	MemoryMap[count1] = addr;
                        MemoryMap[count1] = addr + " Error: External address exceeds length of uselist; treated as immediate";
                    }
                    else{
                        map<string, string>::iterator iterrule3 = SymbolTable.find(useSymbol[operand]);
                        if (iterrule3 == SymbolTable.end()){
                            saddress = to_string(atoi((to_string(opcode) + "000").c_str()));
                            MemoryMap [count1] = saddress + " Error: " + useSymbol[operand] + " is not defined; zero used" ;
                        }
                        else{
                            saddress = to_string(atoi((to_string(opcode) + "000").c_str()) + atoi(SymbolTable[useSymbol[operand]].c_str()));
                            MemoryMap [count1] = saddress;
                        }
                        rule7[useSymbol[operand]] = "used";
                    }
                }

            }

            count1++;
            codecount--;
        }
        map<string, string>::iterator iterrule7 = rule7.begin();
        while (iterrule7 != rule7.end()) {
            if ((*iterrule7).second == "not used"){
                MemoryMap[count1-1] = MemoryMap[count1-1] + "\nWarning: Module" +  to_string(modulenum) +": "+ 
                (*iterrule7).first + " appeared in the uselist but was not actually used";
                rule7[(*iterrule7).first] = "used";
            }
            iterrule7++;
        }
        offset = offset + tmp;
        //cout<< modulenum;

        modulenum++;
    }
    modulenum = 1;
}

int main(int argc, char* argv[]) {
    string input = argv[1];
    f.open(input.c_str());
    pass1();
    cout << "Symbol Table" << endl;
    map<string, string>::iterator iter1 = SymbolTable.begin();
    while(iter1 != SymbolTable.end()){
        cout << (*iter1).first << "=" << (*iter1).second << endl;
        iter1++;
    }
    cout << endl << "Memory Map" << endl;
    f.close();
    f.open(input.c_str());
    pass2();
    map<int, string>::iterator iter2 = MemoryMap.begin();
    while(iter2 != MemoryMap.end()){
        cout << setfill('0') << setw(3) << (*iter2).first << ": " << (*iter2).second << endl;
        iter2++;
    }
    map<string, string>::iterator iterrule4 = rule4.begin();
    while (iterrule4 != rule4.end()) {
        if ((*iterrule4).second == "not used"){
            warning = warning + "\nWarning: Module" + to_string(countmodule[(*iterrule4).first]) 
            + ": " + (*iterrule4).first + " was defined but never used";
            rule4[(*iterrule4).first] = "used";
        }
            iterrule4++;
    }
    cout<<warning <<endl;
    f.close();

    return 0;
}