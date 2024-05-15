#include "util.h"

void test() {
    std::cout<<"test"<<std::endl;
}

string toLower(string str) {
    string result = str;
    for(unsigned int i = 0; i < str.length(); i++) {
        result[i] = tolower(str[i]);
    }
    return result;
}

bool IS_All_Letter(string str) {
    for(unsigned int i = 0; i < str.length(); i++) {
        if(!isalpha(str[i])) {
            return false;
        }
    }
    return true;
}

void printToken(string token, int code, int lineNum) {
    FILE *fp = freopen(lexicalTxtPath, "a",stdout);
    string result = "";
    result += token;
    result += '\t';
    result += "<";
    switch(code) {
        case TokenCode::KW_int:
        case TokenCode::KW_void:
        case TokenCode::KW_return:
        case TokenCode::KW_const:
        case TokenCode::KW_main:
        case TokenCode::KW_float:
        case TokenCode::KW_if:
        case TokenCode::KW_else:
                    result += "KW";
                    break;
        case TokenCode::OP_plus:
        case TokenCode::OP_minus:
        case TokenCode::OP_multiply:
        case TokenCode::OP_divide:
        case TokenCode::OP_percent:
        case TokenCode::OP_equal:
        case TokenCode::OP_greater:
        case TokenCode::OP_less:
        case TokenCode::OP_assign:
        case TokenCode::OP_leq:
        case TokenCode::OP_geq:
        case TokenCode::OP_neq:
        case TokenCode::OP_and:
        case TokenCode::OP_or:
                    result += "OP";
                    break;

        case TokenCode::SE_lparent:
        case TokenCode::SE_rparent:
        case TokenCode::SE_lbraces:
        case TokenCode::SE_rbarces:
        case TokenCode::SE_semicolon:
        case TokenCode::SE_comma:
                    result += "SE";
                    break;
        
        case TokenCode::IDN:
                    result += "IDN";
                    break;
        case TokenCode::INT:
                    result += "INT";
                    break;
        case TokenCode::FLOAT:
                    result += "FLOAT";
                    break;
        case TokenCode::UNDIFNIE:
                    result += "UNDIFNIE";
                    break;
    }
    if(code != TokenCode::IDN && code != TokenCode::INT && code != TokenCode::FLOAT) {
        result += "," + to_string(code) + ">";
    } else {
        result += "," + token + ">";
    }
    cout << result << endl;
    fflush(fp);
    fclose(fp);
    freopen("CON","w",stdout);
    if(code == TokenCode::UNDIFNIE) {
        cout << "UNDIFNIED token: " << token << " in line " << lineNum << endl;
    }
}

set<char> getLetterList(char c) {
    set<char> result;
    if(isalpha(c) || c == '_') {
        set<char> letterList = {'_'};
        for (char c = 'a'; c <= 'z'; c++) {
            letterList.insert(c);
        }
        for (char c = 'A'; c <= 'Z'; c++) {
            letterList.insert(c);
        }
        return letterList;
    } else if(c == '0') {
        result = {'0'};
        return result;
    } else if(c >= '1' && c <= '9') {
        set<char> numList;
        for (char c = '1'; c <= '9'; c++) {
            numList.insert(c);
        }
        return numList;
    } else if(c == '.') {
        result = {'.'};
        return result;
    }
    return result;
}