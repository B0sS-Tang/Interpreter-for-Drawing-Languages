#ifndef _Semantic_Anylysis_H
#define _Semantic_Anylysis_H

#include "lexical_analysis.h"

#ifdef GLOBALS
double Parameter_Global = 0;		    // 为参数T分配的变量
double Origin_X_Global = 0.0, Origin_Y_Global = 0.0;// 用于记录平移距离
double Rot_Ang_Global = 0.0;		    // 用于记录旋转角度
double Scale_X_Global = 1.0, Scale_Y_Global = 1.0;	    // 用于记录比例因子
HDC         hdc;  //设备环境句柄
PAINTSTRUCT ps;
RECT        rect;
#else
extern double Parameter_Global;		    // 为参数T分配的变量
extern double Origin_X_Global, Origin_Y_Global;// 用于记录平移距离
extern double Rot_Ang_Global;		    // 用于记录旋转角度
extern double Scale_X_Global, Scale_Y_Global;	    // 用于记录比例因子
extern HDC         hdc;  //设备环境句柄
extern PAINTSTRUCT ps;
extern RECT        rect;
#endif // GLOBALS


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

double GetExprValue(ExprNode* root);
void CalcCoord(
    ExprNode* x_nptr,
    ExprNode* y_nptr,
    double* x_val,
    double* y_val);
void DrawLoop(
    double Start,
    double End,
    double Step,
    ExprNode* HorPtr,
    ExprNode* VerPtr);
void SetOrigin(double x, double y);
void SetRot(double angle);
void SetScale(double x, double y);

#endif // !_Semantic_Anylysis_H
