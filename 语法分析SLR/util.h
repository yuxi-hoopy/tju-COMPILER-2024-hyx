#ifndef UTIL_H
#define UTIL_H
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
using namespace std;
// 定义颜色代码
#define RESET "\033[0m"
#define RED "\033[31m"     /* 红色 */
#define GREEN "\033[32m"   /* 绿色 */
#define YELLOW "\033[33m"  /* 黄色 */
#define BLUE "\033[34m"    /* 蓝色 */
#define MAGENTA "\033[35m" /* 品红 */
#define CYAN "\033[36m"    /* 青色 */
#define WHITE "\033[37m"   /* 白色 */
struct Production
{
    string left;          // 产生式左部
    vector<string> right; // 产生式右部---（在grammer.txt中多个选项则用空格分隔）
    bool operator==(const Production &r) const
    {
        bool ri = true;
        for (int i = 0; i < right.size(); i++)
            if (right[i] != r.right[i])
                ri = false;
        return ((left == r.left) && ri);
    }
};

struct Item
{
    int productionIndex; // 产生式的索引
    int dotPos;          // 项目点的位置---统一从下标0开始
    bool operator==(const Item &other) const
    {
        return productionIndex == other.productionIndex && dotPos == other.dotPos;
    }
    bool operator<(const Item &other) const
    {
        if (productionIndex == other.productionIndex)
            return dotPos < other.dotPos;
        else
            return productionIndex < other.productionIndex;
    }
};

struct State
{
    int stateNumber;              // 状态序号
    vector<Item> items;           // 状态中包含的所有项目
    map<string, int> transitions; // 状态到其他状态的转移关系-相当于GO函数表
};
typedef unordered_set<string> nonTerminal;                             // 非终结符
typedef unordered_set<string> Terminal;                                // 终结符
typedef unordered_map<int, unordered_map<string, int>> gotoTable;      // goto表
typedef unordered_map<int, unordered_map<string, string>> actionTable; // action表
typedef unordered_map<string, unordered_set<string>> FollowSets;       // follow集
typedef unordered_map<string, unordered_set<string>> FirstSets;        // first集
// 根据所有产生式生成所有项目
vector<Item> generateAllItems(vector<Production> productions)
{
    vector<Item> items;

    for (int i = 0; i < productions.size(); i++)
    {
        Production production = productions[i];
        for (int j = 0; j < production.right.size(); j++)
        {
            Item item;
            item.productionIndex = i, item.dotPos = j;
            items.push_back(item);
        }
        if (!production.right.size())
        {
            Item item;
            item.productionIndex = i, item.dotPos = 0;
            items.push_back(item);
        }
    }

    return items;
}

Production splitProduction(const string &input, const string &delimiter)
{
    Production result;

    int delimiterPos = input.find(delimiter);
    if (delimiterPos != string::npos)
    {
        result.left = input.substr(0, delimiterPos - 1); // 有个空格
        string rightPart = input.substr(delimiterPos + 1 + delimiter.length());
        istringstream iss(rightPart);
        string token;

        while (getline(iss, token, ' '))
        {
            if (token != "$")
                if (!token.empty())
                {
                    result.right.push_back(token);
                }
                else
                    result.right.push_back("");
        }
    }
    else
    {
        result.left = input;
    }

    return result;
}

vector<string> read_from_lexical()
{
    fstream file("E://compiler//lexical.txt");
    vector<string> input;
    if (!file.is_open())
    {
        cout << "Fail to open file" << endl;
    }
    else
    {
        string line;
        while (getline(file, line))
        {
            std::istringstream iss(line);
            string name, type;

            if (iss >> name >> type)
            {
                string keyword;
                // 数据解析成功，可以进一步处理
                size_t start = type.find("<");      // 查找 "<KW," 的位置，并加上 4，得到关键字的起始位置
                size_t end = type.find(",", start); // 在起始位置后查找 ">" 的位置，得到关键字的结束位置

                if (start != std::string::npos && end != std::string::npos)
                    keyword = type.substr(start + 1, end - start - 1); // 提取关键字
                if (keyword == "INT")
                {
                    input.push_back("IntConst");
                }
                else if (keyword == "IDN")
                {
                    input.push_back("Ident");
                }
                else if (keyword == "FLOAT")
                    input.push_back("floatConst");
                else
                    input.push_back(name);
            }
        }
    }
    input.push_back("#");
    return input;
}

// 从txt文件中读取产生式并存储
vector<Production> parseGrammar(const string &filename)
{
    vector<Production> productions;
    Production x;

    FILE *file = fopen(filename.c_str(), "r");
    if (!file)
    {
        cerr << "Failed to open file: " << filename << endl;
        return productions;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file))
    {
        string line(buffer);
        line.erase(line.find_last_not_of(" \r\n\t") + 1);

        if (line.empty())
            continue;

        string arrow = "->";
        x = splitProduction(line, arrow);
        productions.push_back(x);
    }

    fclose(file);
    return productions;
}

// 提取终结符、非终结符
void getSymbols(const vector<Production> &productions, unordered_set<string> &nonTerminal, unordered_set<string> &Terminal)
{
    for (const Production &production : productions)
    {
        nonTerminal.insert(production.left);
    }
    for (const Production &production : productions)
    {
        for (const string &symbol : production.right)
        {
            if (nonTerminal.find(symbol) == nonTerminal.end())
            {
                Terminal.insert(symbol);
            }
        }
    }
    Terminal.insert("#");
}

// 计算FIRST集
void calculateFIRST(vector<Production> productions, unordered_set<string> &terminal_symbols, unordered_map<string, unordered_set<string>> &FIRST)
{
    terminal_symbols.insert("$");
    for (const auto &production : productions)
    {
        FIRST[production.left] = unordered_set<string>(); // 初始化FIRST集为空
    }
    bool flag = true;
    while (flag)
    {
        flag = false;
        for (const auto &production : productions)
        {
            string left = production.left;
            vector<string> rights = production.right;
            int max_non = 0;
            if (rights.empty())
            {
                if (FIRST[left].find("$") == FIRST[left].end())
                {
                    FIRST[left].insert("$");
                    flag = true;
                }
                continue;
            }
            for (int index = 0; index < rights.size(); ++index)
            {
                string current_symbol = rights[index];
                if (terminal_symbols.find(current_symbol) != terminal_symbols.end())
                {
                    if (FIRST[left].find(current_symbol) == FIRST[left].end())
                    {
                        FIRST[left].insert(current_symbol);
                        flag = true;
                    }
                    max_non = index;
                    break;
                }
                if (FIRST[rights[index]].find("$") == FIRST[rights[index]].end())
                {
                    max_non = index + 1;
                    break;
                }
                if (index == rights.size() - 1)
                {
                    if (FIRST[left].find("$") == FIRST[left].end())
                    {
                        FIRST[left].insert("$");
                        flag = true;
                    }
                    max_non = rights.size();
                }
            }
            for (int index = 0; index < max_non; ++index)
            { 
                for (auto &character : FIRST[rights[index]])
                {
                    if (character != "$" && FIRST[left].find(character) == FIRST[left].end())
                    {
                        FIRST[left].insert(character);
                        flag = true;
                    }
                }
            }
        }
    }
}

// 计算FOLLOW集
void calculateFOLLOW(const vector<Production> productions, unordered_set<string> &terminal_symbols, unordered_map<string, unordered_set<string>> &FIRST, unordered_map<string, unordered_set<string>> &FOLLOW, nonTerminal nonterminals, Terminal terminals)
{
    bool has_changes = true;

    // 初始化FOLLOW集
    for (const auto production : productions)
    {
        FOLLOW[production.left] = unordered_set<string>();
    }

    // 将结束符"#"加入起始符号的FOLLOW集中
    FOLLOW[productions[0].left].insert("#");

    while (has_changes)
    {
        has_changes = false;
        for (const Production production : productions)
        {
            string left = production.left;
            vector<string> right = production.right;
            for (int i = 0; i < right.size(); i++)
            {
                string symbol = right[i];
                if (terminals.find(symbol) != terminals.end())
                    continue;
                // 处理非终结符symbol的FOLLOW集
                if (i != right.size() - 1) // 防止越界
                {
                    string next_symbol = right[i + 1];
                    if (nonterminals.find(next_symbol) != nonterminals.end())
                    {
                        for (const string x : FIRST[next_symbol])
                        {
                            if (x != "$" && !FOLLOW[symbol].count(x))
                            {
                                FOLLOW[symbol].insert(x);
                                has_changes = true;
                            }
                        }
                    }
                    else
                    {
                        if (!FOLLOW[symbol].count(next_symbol))
                        {
                            FOLLOW[symbol].insert(next_symbol);
                            has_changes = true;
                        }
                    }
                }

                // 如果β可以推导出空串或者β是产生式右部的最后一个符号，则将FOLLOW集(left)中的所有元素加入FOLLOW集(symbol)中
                if (i == right.size() - 1 || (i == right.size() - 2 && FIRST[right[i + 1]].count("$")))
                {
                    for (const string t : FOLLOW[left])
                    {
                        if (!FOLLOW[symbol].count(t))
                        {
                            FOLLOW[symbol].insert(t);
                            has_changes = true;
                        }
                    }
                }
            }
        }
    }
}
// 计算闭包----已验证小demo正确---2024-05-05-17:27--hyx
// 修正：要加入非终结符->·xxx----2024-05-07-10：59--hyx
vector<Item> CLOSURE(vector<Item> items, vector<Production> productions, nonTerminal nonterminals)
{
    vector<Item> closure = items; // 初态I0
    vector<Item> closure_copy = closure;
    vector<Item> allitems = generateAllItems(productions);

    bool changed = 0;
    do
    {
        if (changed)
            closure = closure_copy;
        changed = false;
        for (const auto item : closure)
        {
            if (item.dotPos < productions[item.productionIndex].right.size())
            {
                string nextSymbol = productions[item.productionIndex].right[item.dotPos];
                if (nonterminals.find(nextSymbol) != nonterminals.end())
                {
                    for (const auto newItem : allitems)
                    {
                        if (newItem.dotPos == 0 && productions[newItem.productionIndex].left == nextSymbol && find(closure_copy.begin(), closure_copy.end(), newItem) == closure_copy.end())
                        {
                            closure_copy.push_back(newItem);
                            changed = true;
                        }
                    }
                }
            }
        }
    } while (changed);
    return closure;
}

// GO函数前置J已修正:代表从Fromstate中所有项目经过symbol得到的项目集
// 2024-05-06-11：53：解决可能J集合为空--hyx(引用传递+flag)
//
bool GO(vector<Item> &Gotoitems, const vector<Item> Fromstate, const string symbol, vector<Production> productions)
{
    bool ischanged = false;

    for (const auto item : Fromstate)
    {
        if (item.dotPos < productions[item.productionIndex].right.size() &&
            productions[item.productionIndex].right[item.dotPos] == symbol)
        {
            Item newItem;
            newItem.productionIndex = item.productionIndex, newItem.dotPos = item.dotPos + 1;
            Gotoitems.push_back(newItem);
            ischanged = true;
        }
    }
    return ischanged;
}

// 先生成所有项目集的集合C----使用了CLOSURE函数
void generate_all_items_set(vector<vector<Item>> &C, vector<Production> productions, vector<State> states, nonTerminal nonterminals, Terminal terminals)
{
    vector<vector<Item>> copy;

    vector<Item> S0;
    Item a;
    a.dotPos = 0, a.productionIndex = 0; // 在第一个项目集里放第一个项目
    S0.push_back(a);
    vector<Item> items = (CLOSURE(S0, productions, nonterminals)); // 初始项目集为closure({S'→>'S})
    C.push_back(items);
    unordered_set<string> allsymbol;
    allsymbol.insert(nonterminals.begin(), nonterminals.end());
    allsymbol.insert(terminals.begin(), terminals.end());

    int loopnum = 0;
    copy = C;
    while (true)
    {
        loopnum++;
        C = copy;
        bool renew = false;
        for (const auto I : C)
        {
            for (const auto X : allsymbol)
            {
                vector<Item> goItems;
                bool changed = GO(goItems, I, X, productions);
                if (changed)
                {
                    if (!goItems.empty())
                    {
                        goItems = CLOSURE(goItems, productions, nonterminals);
                        if (find(copy.begin(), copy.end(), goItems) == copy.end())
                        {
                            // 确保项目集唯一
                            copy.push_back(goItems);
                            renew = true;
                        }
                    }
                }
            }
        }
        if (!renew)
        {
            break;
        }
    }

    // return C;
}

// 生成转移关系，主要使用GO函数
vector<State> itemsets(vector<vector<Item>> C, vector<Production> productions, vector<State> &states, nonTerminal nonterminals, Terminal terminals)
{
    for (int i = 0; i < C.size(); i++)
    {
        State state;
        state.stateNumber = i;
        state.items = C[i];
        states.push_back(state);
    }
    unordered_set<string> allsymbol;
    allsymbol.insert(nonterminals.begin(), nonterminals.end());
    allsymbol.insert(terminals.begin(), terminals.end());

    // 转移关系初始化为全-1
    for (int i = 0; i < states.size(); i++)
    {
        for (const auto X : allsymbol)
        {
            states[i].transitions[X] = -1;
        }
    }
    // 构建转移关系
    for (int i = 0; i < states.size(); i++)
    {
        const auto &state = states[i];
        for (const auto X : allsymbol)
        {
            vector<Item> goItems;
            bool ischanged = GO(goItems, state.items, X, productions);
            if (ischanged)
            {
                for (int j = 0; j < states.size(); j++)
                {
                    for (const Item item : states[j].items)
                        if (item == goItems[0])
                        {
                            states[i].transitions[X] = j;
                            break;
                        }
                }
            }
        }
    }
    return states;
}

void printStates(vector<Production> productions, vector<State> states)
{
    cout << "states:" << endl;
    for (const auto state : states)
    {
        cout << "State " << state.stateNumber << ":" << endl;
        cout << "Items: ";
        for (const auto &item : state.items)
        {
            cout << productions[item.productionIndex].left << " -> ";
            for (int i = 0; i < productions[item.productionIndex].right.size(); i++)
            {
                if (i == item.dotPos)
                    cout << ". ";
                cout << productions[item.productionIndex].right[i] << " ";
            }
            if (item.dotPos == productions[item.productionIndex].right.size())
                cout << ". ";
            cout << endl;
        }
        cout << "Transitions: ";
        for (const auto &transition : state.transitions)
        {
            cout << transition.first << " -> " << transition.second << " ";
        }
        cout << endl
             << endl;
    }
}

gotoTable fillGotoTable(gotoTable &table, const vector<State> &states, nonTerminal nonterminals, Terminal terminals)
{
    unordered_set<string> allsymbol;
    allsymbol.insert(nonterminals.begin(), nonterminals.end());
    allsymbol.insert(terminals.begin(), terminals.end());
    // 先把所有位置填上"-1"
    for (const State &state : states)
    {
        int state_number = state.stateNumber;
        for (const string &symbol : allsymbol)
        {
            table[state_number][symbol] = -1;
        }
    }
    for (const State &state : states)
    {
        int state_number = state.stateNumber;
        for (const auto goto_item : state.transitions)
        {
            // if(A is Nonterminal): k+A-->j，则加入{k,("A",j)}
            string symbol = goto_item.first;
            if (nonterminals.find(symbol) != nonterminals.end())
                table[state_number][symbol] = goto_item.second;
        }
    }
    return table;
}

// 填写ACTION表
actionTable fillActionTable(vector<Production> productions, actionTable &table, vector<State> states, unordered_map<string, unordered_set<string>> FOLLOW, nonTerminal nonterminals, Terminal terminals)
{
    unordered_set<string> allsymbol;
    allsymbol.insert(nonterminals.begin(), nonterminals.end());
    allsymbol.insert(terminals.begin(), terminals.end());
    // 先把所有位置填上"error"
    for (const State &state : states)
    {
        int state_number = state.stateNumber;
        for (const string &symbol : allsymbol)
        {
            table[state_number][symbol] = "error";
        }
    }
    for (const State &state : states)
    {
        int state_number = state.stateNumber;
        for (const Item &item : state.items)
        {
            int index = item.productionIndex;
            string left = productions[index].left;
            vector<string> right = productions[index].right;
            unordered_set<string> followset;
            followset = FOLLOW[left];

            // 移进项目
            if (item.dotPos < right.size())
            {
                string symbol;
                symbol = right[item.dotPos];

                if (states[state_number].transitions[symbol] != -1) // 终结符
                {
                    if (terminals.find(symbol) != terminals.end())
                    {
                        int j = state.transitions.at(symbol);
                        table[state_number][symbol] = "s" + to_string(j);
                    }
                }
            }
            // 归约项目
            else if (item.dotPos == right.size() && item.productionIndex != 0)
            {
                for (const string symbol : terminals)
                {
                    if (followset.find(symbol) != followset.end())
                    {
                        table[state_number][symbol] = "r" + to_string(index);
                    }
                }
            }
            else if (item.productionIndex == 0 && item.dotPos == right.size())
            {
                table[state_number]["#"] = "acc";
            }
        }
    }
    return table;
}

// 移进-规约过程
void analysis(vector<string> rest_string, vector<Production> &productions, vector<State> states, stack<int> &states_stack, stack<string> &symbol_stack, actionTable actiontable, gotoTable gototable)
{
    int seq = 1;
    states_stack.push(0); // 0号状态入栈
    symbol_stack.push("#");
    int index = 0; // 待识别的字符下标
    string next_symbol;
    next_symbol = rest_string[index]; // 下一个要读的字符
    State curren_state = states[0];   // 当前状态
    int num = 0;                      // 当前状态的序号

    while (actiontable[num][next_symbol] != "acc" && actiontable[num][next_symbol] != "error")
    {
        string act = actiontable[num][next_symbol];   // action表内容，移进规约acc error
        int goto_state = gototable[num][next_symbol]; // GOTO表中的状态序号
        cout << seq << "    " << symbol_stack.top() << " # " << next_symbol << "   ";
        if (act[0] == 's')
        {
            // num = goto_state = curren_state.transitions[next_symbol];
            int state_num = stoi(act.substr(1, act.size() - 1));
            num = goto_state = state_num;
            symbol_stack.push(next_symbol);
            states_stack.push(goto_state);
            // input_stack.pop();

            curren_state = states[goto_state]; // states的index应该是和第一个属性num相等
            next_symbol = rest_string[++index];
            cout << "move" << endl;

            seq++;
        }
        // 规约
        else if (act[0] == 'r')
        {
            int pro_num = stoi(act.substr(1, act.size() - 1)); // 规约使用的产生式序号
            Production pro = productions[pro_num];             // 规约使用的产生式
            int len = pro.right.size();                        // 产生式右部长度
            string left = pro.left;                            // 产生式左部符号
            while (len--)
            {
                symbol_stack.pop();
                states_stack.pop();
            }
            int top_state_num = states[states_stack.top()].stateNumber;
            goto_state = gototable[top_state_num][left]; // S = GoTo[Sm-r,A]
            if (goto_state == -1)
            {
                cout << "error" << endl;
                break;
            }
            states_stack.push(goto_state);
            symbol_stack.push(left);
            num = goto_state;
            curren_state = states[num];
            cout << "reduction" << endl;
            seq++;
        }
    }
    string act = actiontable[num][next_symbol];
    if (act == "acc")
    {
        seq++;
        cout << "accept" << endl;
    }
    else if (act == "error")
    {
        cout << "error" << endl;
        seq++;
    }
}

#endif