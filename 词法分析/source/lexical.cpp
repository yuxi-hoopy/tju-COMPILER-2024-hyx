#include <iostream>
#include <string>
#include <fstream>
#include <set>
#include <map>
#include "util.h"
#include "SymbolTable.h"
#include "FSM.h"
using namespace std;

set<string> keyword = {"int", "void", "return", "const", "main", "float", "if", "else"}; //关键字
set<char> operation = {'+', '-', '*', '/', '%', '=', '>', '<'};
set<char> operationBeginChar = {'=', '<', '>', '!', '&', '|'};
set<string> operationOf2Char = {"==", "<=", ">=", "!=", "&&", "||"};
set<char> boundary = {'(', ')', '{', '}', ';', ','}; //界符

map<std::string, int> tokenCodeMap = {
    {"int", TokenCode::KW_int},
    {"void", TokenCode::KW_void},
    {"return", TokenCode::KW_return},
    {"const", TokenCode::KW_const},
    {"main", TokenCode::KW_main},
    {"float", TokenCode::KW_float},
    {"if", TokenCode::KW_if},
    {"else", TokenCode::KW_else},
    {"+", TokenCode::OP_plus},
    {"-", TokenCode::OP_minus},
    {"*", TokenCode::OP_multiply},
    {"/", TokenCode::OP_divide},
    {"%", TokenCode::OP_percent},
    {"=", TokenCode::OP_assign},
    {">", TokenCode::OP_greater},
    {"<", TokenCode::OP_less},
    {"==", TokenCode::OP_equal},
    {"<=", TokenCode::OP_leq},
    {">=", TokenCode::OP_geq},
    {"!=", TokenCode::OP_neq},
    {"&&", TokenCode::OP_and},
    {"||", TokenCode::OP_or},
    {"(", TokenCode::SE_lparent},
    {")", TokenCode::SE_rparent},
    {"{", TokenCode::SE_lbraces},
    {"}", TokenCode::SE_rbarces},
    {";", TokenCode::SE_semicolon},
    {",", TokenCode::SE_comma}
};

SymbolTable symbolTable = SymbolTable();
FSM NFA = CREATE_NFA(); //非确定自动机
FSM DFA = NFA_TO_DFA(NFA); //有限自动机的确定化
FSM miniDFA = MIN_DFA(DFA); //有限确定机的最小化
int lineNum = 0; //行数

// 使用自动机分析token
void analyseToken(string token) 
{
    if(token.length() <= 0) 
    {
        return;
    }
    // 先判断好是不是关键字
    if(IS_All_Letter(token) && keyword.count(toLower(token))) 
    {
        printToken(token, tokenCodeMap[token], lineNum);
        return;
    }
    /**
     * 思路：
     * 1. 先用正则表达式构造NFA
     * 2. NFA转成DFA
     * 3. DFA最小化
     * 4. 把token扔进最小化的DFA看看返回什么状态---> 标识符!或变量!，以及错误状态，只有这三种可能
     * 注意：
     * 1. 这里只需要返回TokenCode即可
    */
    
    int tokenCode = IDENTITY(miniDFA, token);
    printToken(token, tokenCode, lineNum);
    if(tokenCode == TokenCode::IDN) 
    {
        symbolTable.addSymbol(token);
    }
}

// 词法分析
void lexicalAnalysis(string fileName) 
{
    FILE* fp;
    fp =fopen(lexicalTxtPath,"w");
    fwrite("", 0, 1, fp);
    ifstream file;
    file.open(fileName, std::ios::in);
    if (!file.is_open()) 
    {
        cout << "Error: " << fileName << " could not open!" << endl;
    }

    char c;
    string token = "";
    /**
     * 算法概括：
     * 除了分割符和已经定义好的界符和运算符之外，其余所有!所有！token序列都应交给自动机判断
     * 所以这里就是要利用分隔符、界符和运算符将程序分开，因为带有运算符的字符串不应该进入自动机
    */
    while((c = file.get()) != EOF) 
    {
        if (c == ' ' || c == '\t' || c == '\n') 
        {
            analyseToken(token);
            token = "";
            if(c == '\n') 
            {
                lineNum++;
            }
            continue;
        } 
        else if (boundary.count(c)) 
        { // 界符
            analyseToken(token);
            token = string(1, c);
            printToken(token, tokenCodeMap[token], lineNum);
            token = "";
            continue;
        } 
        else if (operation.count(c) || operationBeginChar.count(c)) 
        { // 运算符
            analyseToken(token);
            if(operationBeginChar.count(c)) 
            { // 有可能是两个字符组成的运算符
                char nextChar = file.get();
                string tryOp = "";
                tryOp += c;
                tryOp += nextChar;
                if(operationOf2Char.count(tryOp)) 
                { // 真的是两个字符组成的运算符
                    token = tryOp;
                    printToken(token, tokenCodeMap[token], lineNum);
                } 
                else if(operation.count(c)) 
                { // 普通的由一个字符组成的运算符
                    token = string(1, c);
                    printToken(token, tokenCodeMap[token], lineNum);
                    file.putback(nextChar);
                } 
                else 
                {
                    //出错了，错误token
                    token = string(1, c);
                    printToken(token, TokenCode::UNDIFNIE, lineNum);
                    file.putback(nextChar);
                }
            } 
            else 
            { // 这就是一个字符组成的运算符
                token = string(1, c);
                printToken(token, tokenCodeMap[token], lineNum);
            }
            token = "";
        } 
        else 
        {
            token += c;
        }
    }
    symbolTable.printTable();
}

int main(int argc, char **argv)
{
    string fileName;
    if(argc == 1) 
    {
        fileName = "test.sy"; 
    } 
    else 
    {
        fileName = argv[1];
    }
    lexicalAnalysis(fileName);
    return 0;
}