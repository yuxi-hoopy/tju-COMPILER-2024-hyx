# 2024编译原理大作业tju
由C++实现的C--编译器前端，包括词法分析器、语法分析器和中间代码生成一小部分。
姜佳君老师的课，计科专业21级大作业

答辩完成，过程非常完美和顺利，老师很满意，只问了我有什么心得，我把过程中的bug以及解决办法讲了一下就OK了。

# 语法分析-基于SLR

LR语法分析器能识别⼏乎所有能⽤上下⽂⽆关⽂法描述的程序设计语⾔的结构。 LR分析法是已知的最⼀般的⽆回溯移动归约语法分析法，而且可以和其他移动归约分析⼀样被有效地实现。

LR分析的基本思想是，在规范归约过程中，⼀⽅⾯记住已移进和 归约出的整个符号串，即记住“历史”，另⼀⽅⾯根据所⽤的产⽣ 式推测未来可能碰到的输⼊符号，即对未来进⾏“展望。

LR的本质是双栈+DFA，而`SLR`就是LR(0)+FOLLOW集。

![image](https://github.com/yuxi-hoopy/tju-COMPILER-2024-hyx/assets/129140901/4e6471ad-5dc5-4437-ab8e-83089a9cdef9)





## 阶段任务

在做一切之前需要对齐数据结构

### 统一数据结构，三人并行撰写代码

产生式：

```cpp
struct Production {
    string left; // 产生式左部
    vector<string> right; // 产生式右部---（在grammer.txt中多个选项则用空格分隔）
};
```

项目：

```cpp
struct Item {
    int productionIndex; // 产生式的索引
    int dotPos; // 项目点的位置---统一从下标0开始
};
```

项目集规范族：

```cpp
struct State {
    int stateNumber;//状态序号
    vector<Item> items;//状态中包含的所有项目
};
```

集合类：

```cpp
unordered_set<string> nonTerminals;//非终结符
unordered_set<string> terminals;//终结符
unordered_map<string, unordered_set<string>> firstSets;//first集
unordered_map<string, unordered_set<string>> followSets;//follow集
unordered_map<string, unordered_map<string, string>> gotoTable;//goto表
unordered_map<string, unordered_map<string, string>> actionTable;//action表
```

关于goto和action表，为方便理解和使用，举2个例子↓

```cpp
// 示例的 gotoTable
    unordered_map<string, unordered_map<string, int>> gotoTable = {
        {"S0", {{"E", 1}}},
        {"S1", {{"E", 3}, {"T", 2}}},
        {"S2", {{"T", 4}, {"F", 5}}},
    };

    // 示例的 actionTable
    unordered_map<string, unordered_map<string, string>> actionTable = {
        {"S0", {{"id", "shift 3"}, {"(", "shift 4"}}},
        {"S1", {{"+", "shift 6"}, {"$", "accept"}}},
        {"S2", {{"*", "shift 7"}}},
        {"S3", {{"+", "reduce 2"}, {"*", "reduce 2"}, {")", "reduce 2"}, {"$", "reduce 2"}}},
    };
```

### ①将文法简化为A->B形式

代码实现，网上有方法，比较简单

### ②求LR(0)项目集规范族（CLOSURE+GO函数方法），得到识别活前缀的DFA

先得到项目集（对应位置加点）->项目集规范族->得到转换关系，不需要得到DFA图，知道转换关系即可

### ③对每个非终结符求FIRST集、FOLLOW集


### ④根据②③判断文法是否满足SLR文法条件


### ⑤利用②构造ACTION表和GOTO表


### ⑥利用分析表进行移进-规约过程

## 关于①：

老师给的文法是不是LR（0）的？这部分学长的代码并没有做，我看了他们的grammer.txt，70多行，是处理过的版本（，比老师给我们的好用，老师给的文法中还存在`闭包`和`或`,比如

```txt
compUnit -> (decl | funcDef)* EOF;
```

需要进一步处理才能用LR(0) or SLR。

去除闭包和或，将产生式简化为A ->B的形式，得到grammer.txt。注意这里不能直接用往届的txt
ps：已解决，手动修正，去除闭包、直接左递归和或运算符。

## 关于②、③、④：先判断文法是否符合LR(0)规范

1. ### 构造LR(0)项目集规范族

   - 先构造语法G所有的项目：逐个位置加点·

   - 然后用CLOSURE和GO函数构造G'的LR(0)项目集规范族。

     【G'为拓广文法：引进⼀个不出现在G中的⾮终结符S’，并加进⼀个新产⽣式S’→S，⽽这个S’是G’的开始符号。那么我们称G’是G的拓⼴⽂法】


2. ### 判断是否存在规约-规约和规约-移进冲突，不存在则LR(0)；存在则判断FOLLOW集相交情况，大概率是SLR文法了

对于每个项目集I判断。这一步需要计算FIRST集和FOLLOW集


## 填写GOTO表和ACTION表---这2个表组合起来为预测分析表

`ACTION[s ,a]`规定了当前状态s面临输入符号a时应采取什么动作。 

`GOTO[s, X]`规定了状态s面对文法符号X（终结符或⾮终结符）时，下⼀个状态是什么

看看是LR还是SLR，是哪个用哪个。

## 利用预测分析表进行移进-规约过程

LR过程原理如图：如果是SLR也是这个动作，二者的不同之处体现在分析表里，对表的使用方法一样

LR分析器的运行原理和SLR分析器类似，二者的不同之处在于它们使用不同的分析表。然而，它们在使用预测分析表上的方法是相同的。

初步计划，先设2个小ddl，尽量在时段内完成。用gpt应该不会花太多时间，使用下面的数据结构。

ps：注意数据结构一定不要用错哦，不然对接不上会白费力气qwq。

***

2024-05-02-16:11更新：项目集规范族应当包含**状态之间的转移关系**，也就是GO函数的结果；

这部分对三人负责内容都有影响。

**例：**

GO(I~0~，a_string) = I~1~

则0号state内容大致为：

state[0].stateNumber = 1;

state[0].items= {(...,...)，(...,...)，...)};//状态中包含的所有项目

state[0].transitions[a_string] = 1;

新增属性`map<string, int> transitions;`表示当前状态识别一个string转移到的新状态序号。

***

## 代码deadline：2024-05-04 23:59

要求各成员在本地自己生成小测试样例测试通过，最后给出运行结果。可以是单文件，也可以带有头文件，方便后期耦合即可。

对于语法错误的情况，要能给出error信号，老师要求了这一点。（我觉得可能就是`if...;else...; if...;，...，else error`这种形式）

比如：正确生成一个小型文法的LR(0)项目集规范族，推荐从网上找有答案的文法方便对照，遇到问题一定及时在群里发😉

## 代码整合+测试在2024-05-06 23：59之前完成

我来整合，测试文件一人弄2个。共6个，要求包含错误数据

## 数据结构

产生式：

```cpp
struct Production {
    string left; // 产生式左部
    vector<string> right; // 产生式右部---（在grammer.txt中多个选项则用空格分隔）
};
```

项目：

```cpp
struct Item {
    int productionIndex; // 产生式的索引
    int dotPos; // 项目点的位置---统一从下标0开始
};
```

项目集规范族：

```cpp
struct State {
    int stateNumber;//状态序号
    vector<Item> items;//状态中包含的所有项目
    map<string, int> transitions;//状态到其他状态的转移关系
};
```

集合类：

```cpp
unordered_set<string> nonTerminals;//非终结符
unordered_set<string> terminals;//终结符
unordered_map<string, unordered_set<string>> firstSets;//first集
unordered_map<string, unordered_set<string>> followSets;//follow集
unordered_map<string, unordered_map<string, string>> gotoTable;//goto表
unordered_map<string, unordered_map<string, string>> actionTable;//action表
```

关于goto和action表，为方便理解和使用，举2个例子↓

```cpp
// 示例的 gotoTable
    unordered_map<string, unordered_map<string, int>> gotoTable = {
        {"S0", {{"E", 1}}},
        {"S1", {{"E", 3}, {"T", 2}}},
        {"S2", {{"T", 4}, {"F", 5}}},
    };

    // 示例的 actionTable
    unordered_map<string, unordered_map<string, string>> actionTable = {
        {"S0", {{"id", "shift 3"}, {"(", "shift 4"}}},
        {"S1", {{"+", "shift 6"}, {"$", "accept"}}},
        {"S2", {{"*", "shift 7"}}},
        {"S3", {{"+", "reduce 2"}, {"*", "reduce 2"}, {")", "reduce 2"}, {"$", "reduce 2"}}},
    };
```
