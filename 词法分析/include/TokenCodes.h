#ifndef __TOKENCODES_H__
#define __TOKENCODES_H__

enum TokenCode{
    UNDIFNIE = 0,   //未定义

    /*KW 关键字*/
    KW_int,         // int
    KW_void,        // void
    KW_return,      // return
    KW_const,       // const
    KW_main,        // main
    KW_float,      // float
    KW_if,       // if
    KW_else,        // else

    /*OP 运算符*/
    OP_plus,        // +
    OP_minus,       // -
    OP_multiply,    // *
    OP_divide,      // /
    OP_percent,     // %
    OP_assign,      // =
    OP_greater,          // >
    OP_less,          // <
    OP_equal,       // ==
    OP_leq,         // <=
    OP_geq,         // >=
    OP_neq,         // !=
    OP_and,         // &&
    OP_or,          // ||

    /*SE 界符*/
    SE_lparent,     // (
    SE_rparent,     // )
    SE_lbraces,     // { 
    SE_rbarces,     // }
    SE_semicolon,   // ;
    SE_comma,       // ,

    IDN,            // 标识符: (letter|_)(letter|digit|_)*
    INT,            // 整数: digit digit*
    FLOAT           // 浮点数
};

#endif