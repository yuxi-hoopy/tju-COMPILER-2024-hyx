#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <string>
#include <set>
#include "TokenCodes.h"
using namespace std;

const char lexicalTxtPath[] = "lexical.txt";

void test();

string toLower(string str);

bool IS_All_Letter(string str);

void printToken(string token, int code, int lineNum);

set<char> getLetterList(char ch);

#endif