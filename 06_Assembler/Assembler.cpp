#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>

std::unordered_map<std::string, int> TABLE;
std::unordered_map<std::string, std::string> JUMP_TBL;
std::unordered_map<std::string, std::string> COMP_TBL;
std::unordered_map<std::string, std::string> DEST_TBL;

int POSITION_COUNT = 16;

void initTable();
void initCompTable();
void initJumpTable();
void initDestTable();
void firstPass(std::vector<std::string>& lines);
void printLines(std::vector<std::pair<int, std::string>>& lines);
void firstPassForLabels(std::vector<std::pair<int, std::string>>& lines);
void printTable(); 
std::vector<std::pair<int, std::string>> getInsFromFile(std::ifstream& inStream);
std::string getLabelFromBrackets(const std::string& s);
std::string toBinary(int number);
std::vector<std::string> convertLinesToBinary(std::vector<std::pair<int, std::string>> & lines);
std::string handleAInstruction(std::string line);
bool is_digits(const std::string &str);
std::string handleCInstruction(std::string line);
std::string getDestToBin(std::string line);
std::string getCompToBin(std::string line);
std::string getJmpToBin(std::string line);


int main(int argv, char** argc)
{
    if (argv < 2){
        std::cout << "Error, no file is inputed!\n";
        return -1;
    }
    std::string inputFileName = argc[1];

    std::ifstream inputFile {inputFileName};

    if (!inputFile){
        std::cout << "Can't opet file with name '" << inputFileName << "'!\n";
        return -1;
    }

    initTable();
    std::vector<std::pair<int, std::string>> lines = getInsFromFile(inputFile);
    //std::cout << "===============LINES==================\n";
    //printLines(lines);

    firstPassForLabels(lines);
    //std::cout << "================TABLE=================================\n";
    //printTable();

    std::vector<std::string> binaryData = convertLinesToBinary(lines);

    std::string outFileName = inputFileName.substr(0, inputFileName.find('.')) + ".hack";
    std::ofstream outFile {outFileName};
    
    for (auto& binline: binaryData){
        //std::cout << binline << '\n';
        outFile << binline << '\n';
    }

    return 0;
}

void initTable()
{
    TABLE["R0"] = 0;
    TABLE["R1"] = 1;
    TABLE["R2"] = 2;
    TABLE["R3"] = 3;
    TABLE["R4"] = 4;
    TABLE["R5"] = 5;
    TABLE["R6"] = 6;
    TABLE["R7"] = 7;
    TABLE["R8"] = 8;
    TABLE["R9"] = 9;
    TABLE["R10"] = 10;
    TABLE["R11"] = 11;
    TABLE["R12"] = 12;
    TABLE["R13"] = 13;
    TABLE["R14"] = 14;
    TABLE["R15"] = 15;
    TABLE["SCREEN"] = 16384;
    TABLE["KBD"] = 24576;
    TABLE["SP"] = 0;
    TABLE["LCL"] = 1;
    TABLE["ARG"] = 2;
    TABLE["THIS"] = 3;
    TABLE["THAT"] = 4;
    initCompTable();
    initJumpTable();
    initDestTable();
    
}
void initCompTable()
{   
    COMP_TBL["0"] = "101010";
    COMP_TBL["1"] = "111111";
    COMP_TBL["-1"] = "111010";
    COMP_TBL["D"] = "001100";
    COMP_TBL["A"] = "110000";
    COMP_TBL["M"] = "110000";
    COMP_TBL["!D"] = "001101";
    COMP_TBL["!A"] = "110001";
    COMP_TBL["!M"] = "110001";
    COMP_TBL["-D"] = "001111";
    COMP_TBL["-A"] = "110011";
    COMP_TBL["-M"] = "110011";
    COMP_TBL["D+1"] = "011111";
    COMP_TBL["A+1"] = "110111";
    COMP_TBL["M+1"] = "110111";
    COMP_TBL["D-1"] = "001110";
    COMP_TBL["A-1"] = "110010";
    COMP_TBL["M-1"] = "110010";
    COMP_TBL["D+A"] = "000010";
    COMP_TBL["D+M"] = "000010";
    COMP_TBL["D-A"] = "010011";
    COMP_TBL["D-M"] = "010011";
    COMP_TBL["A-D"] = "000111";
    COMP_TBL["M-D"] = "000111";
    COMP_TBL["D&A"] = "000000";
    COMP_TBL["D&M"] = "000000";
    COMP_TBL["D|A"] = "010101";
    COMP_TBL["D|M"] = "010101";
}
void initJumpTable()
{
    JUMP_TBL["null"] = "000";
    JUMP_TBL["JGT"] = "001";
    JUMP_TBL["JEG"] = "010";
    JUMP_TBL["JGE"] = "011";
    JUMP_TBL["JLT"] = "100";
    JUMP_TBL["JNE"] = "101";
    JUMP_TBL["JLE"] = "110";
    JUMP_TBL["JMP"] = "111";
}
void initDestTable()
{
    DEST_TBL["null"] = "000";
    DEST_TBL["M"] = "001";
    DEST_TBL["D"] = "010";
    DEST_TBL["MD"] = "011";
    DEST_TBL["A"] = "100";
    DEST_TBL["AM"] = "101";
    DEST_TBL["AD"] = "110";
    DEST_TBL["AMD"] = "111";
}

std::vector<std::string> convertLinesToBinary(std::vector<std::pair<int, std::string>> & lines){
    std::vector<std::string> retBinary;
    for (auto& line: lines)
    {   
        if (line.second[0] == '(')
            continue;
        if (line.second[0] == '@')
        {
            retBinary.push_back(handleAInstruction(line.second));
        }
        else
        {   
            retBinary.push_back(handleCInstruction(line.second));
        }
    }
    return retBinary;
}


std::string getDestToBin(std::string line)
{
    if (line.find('=') == std::string::npos){
        return "000";
    }
    return DEST_TBL[line.substr(0, line.find('='))];
}

std::string getCompToBin(std::string line)
{
    auto first = line.find('=');
    auto last = line.find(';');
    std::string comp = line.substr(first+1, last - first - 1);
    if (comp.find('M') == std::string::npos){
        return "0" + COMP_TBL[comp];
    }
    return "1" + COMP_TBL[comp];
}
std::string getJmpToBin(std::string line)
{
    if (line.find(';') == std::string::npos){
        return "000";
    }
    auto first = line.find(';');
    return JUMP_TBL[line.substr(first + 1, line.size() - first)];
}


std::string handleCInstruction(std::string line)
{   
    auto first = line.find("//");
    if (first != std::string::npos){
        line = line.substr(0, first);
    }
    std::string retVal = "111";
    return retVal + getCompToBin(line) + getDestToBin(line) + getJmpToBin(line);
}


std::string handleAInstruction(std::string line)
{   
    std::string retVal = "0";
    std::string code = line.substr(1, line.size() - 1);

    if (is_digits(code))
    {   
        std::string binaryPart = toBinary(std::stoi(code));
        for (int i = 1; i <= 16 - 1 - binaryPart.size(); ++i){
            retVal += "0";
        }
        retVal += binaryPart;
        return retVal;
    }

    auto it = TABLE.find(code);
    if (it == TABLE.end()){
        TABLE[code] = POSITION_COUNT;
        ++POSITION_COUNT;
    }
    std::string binaryPart = toBinary(TABLE[code]);
    for (int i = 1; i <= 16 - 1 - binaryPart.size(); ++i){
        retVal += "0";
    }
    retVal += binaryPart;
    return retVal;
}


bool is_digits(const std::string &str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}

void printLines(std::vector<std::pair<int, std::string>>& lines)
{
    for (auto& v: lines) std::cout << v.first << " " << v.second << '\n';
}

std::string getLabelFromBrackets(const std::string& s)
{   
    std::string retVal = "";
    for(int i = 1; i < s.size() - 1; ++i)
        retVal += s[i];
    return retVal;
}

void firstPassForLabels(std::vector<std::pair<int, std::string>>& lines)
{   
    for (auto& line: lines)
    {
        if (line.second[0] == '('){
            TABLE[getLabelFromBrackets(line.second)] = line.first;
        }
    }
}

void printTable()
{       
    for (auto& it: TABLE) std::cout << it.first << " " << it.second << '\n';
    std::cout << '\n';
}

std::vector<std::pair<int, std::string>> getInsFromFile(std::ifstream& inStream)
{
    std::string line;
    std::vector<std::pair<int, std::string>> lines;
    int counter = -1;
    while (!inStream.eof()){
        std::getline(inStream, line);
        line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
        if (line != "" && line[0] != '/'){
            if (line[0] != '('){
                ++counter;
                lines.push_back({counter, line});
            }else{
                lines.push_back({counter + 1, line});    
            }
        }
    }
    return lines;    
}

std::string toBinary(int number)
{   
    if (number == 0)
        return "0";
    std::string retVal = "";
    while(number > 0){
        retVal += std::to_string(number % 2);
        number /= 2;
    }
    std::reverse(retVal.begin(), retVal.end());
    return retVal;
}