#ifndef _Syntax_Analysis_H
#define _Syntax_Analysis_H

#include "semantic_analysis.h"


typedef double (*FuncPtr)(double);
typedef struct node
{
    Token_Type OpCode;	// �Ǻ�����
    union
    {
        struct {
            struct node* Left, * Right;
        } CaseOperator;	// ��Ԫ����
        struct {
            struct node* Child;
            FuncPtr MathFuncPtr;
        } CaseFunc;	// ��������
        double CaseConst; 	// ����������ֵ
        double* CaseParmPtr; // ����T������ֵ
    }Content;
}ExprNode;


void Parser(char* SrcFilePtr);

#endif