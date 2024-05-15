#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include "util.h"

typedef unordered_set<string> nonTerminal;                             // 非终结符
typedef unordered_set<string> Terminal;                                // 终结符
typedef unordered_map<int, unordered_map<string, int>> gotoTable;      // goto表
typedef unordered_map<int, unordered_map<string, string>> actionTable; // action表
typedef unordered_map<string, unordered_set<string>> FollowSets;       // follow集
typedef unordered_map<string, unordered_set<string>> FirstSets;        // first集

stack<string> symbol_stack; // 符号栈
stack<string> input_stack;  // 输入
stack<int> states_stack;    // 状态栈

vector<Production> productions;
vector<State> states;
gotoTable gototable;
actionTable actiontable;
nonTerminal nonterminals;
Terminal terminals;
FirstSets FIRST;
FollowSets FOLLOW;
vector<Item> items;

// 定义颜色代码debug用
#define RESET "\033[0m"
#define RED "\033[31m"     /* 红色 */
#define GREEN "\033[32m"   /* 绿色 */
#define YELLOW "\033[33m"  /* 黄色 */
#define BLUE "\033[34m"    /* 蓝色 */
#define MAGENTA "\033[35m" /* 品红 */
#define CYAN "\033[36m"    /* 青色 */
#define WHITE "\033[37m"   /* 白色 */

int main()
{
    string filename = "grammar.txt";
    vector<string> input = read_from_lexical();
    productions = parseGrammar(filename);
    getSymbols(productions, nonterminals, terminals);
    calculateFIRST(productions, terminals, FIRST);
    calculateFOLLOW(productions, terminals, FIRST, FOLLOW, nonterminals, terminals);
    vector<vector<Item>> C;
    generate_all_items_set(C, productions, states, nonterminals, terminals);
    states = itemsets(C, productions, states, nonterminals, terminals);
    gototable = fillGotoTable(gototable, states, nonterminals, terminals);
    actiontable = fillActionTable(productions, actiontable, states, FOLLOW, nonterminals, terminals);
    analysis(input, productions, states, states_stack, symbol_stack, actiontable, gototable);
    return 0;
}

