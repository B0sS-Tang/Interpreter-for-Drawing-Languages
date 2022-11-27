#ifndef _Syntax_Analysis_H
#define _Syntax_Analysis_H

#include "semantic_analysis.h"


typedef double (*FuncPtr)(double);
typedef struct node
{
    Token_Type OpCode;	// 记号种类
    union
    {
        struct {
            struct node* Left, * Right;
        } CaseOperator;	// 二元运算
        struct {
            struct node* Child;
            FuncPtr MathFuncPtr;
        } CaseFunc;	// 函数调用
        double CaseConst; 	// 常数，绑定右值
        double* CaseParmPtr; // 参数T，绑定左值
    }Content;
}ExprNode;


void Parser(char* SrcFilePtr);

#endif