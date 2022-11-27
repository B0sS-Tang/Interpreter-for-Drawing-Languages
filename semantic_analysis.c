#define GLOBALS

#include <stdio.h>
#include <math.h>
#include <Windows.h>
#include <wingdi.h>

#include "semantic_analysis.h"


double GetExprValue(ExprNode* root);
static void CalcCoord(
    ExprNode* x_nptr,
	ExprNode* y_nptr,
	double x_val,
	double y_val);
void DrawPixel(unsigned long x, unsigned long y);
void DrawLoop(
    double Start,
	double End,
	double Step,
	ExprNode* HorPtr,
	ExprNode* VerPtr);


double GetExprValue(ExprNode* root)
{
    if (root == NULL) return 0.0;
    switch (root->OpCode)
    {
    case PLUS:
        return GetExprValue(root->Content.CaseOperator.Right)
            + GetExprValue(root->Content.CaseOperator.Left);
    case MINUS:
        return GetExprValue(root->Content.CaseOperator.Left)
            - GetExprValue(root->Content.CaseOperator.Right);
    case MUL:
        return GetExprValue(root->Content.CaseOperator.Right)
            * GetExprValue(root->Content.CaseOperator.Left);
    case DIV:
        return GetExprValue(root->Content.CaseOperator.Left)
            / GetExprValue(root->Content.CaseOperator.Right);
    case POWER:
        return pow(
            GetExprValue(root->Content.CaseOperator.Left),
            GetExprValue(root->Content.CaseOperator.Right));
    case FUNC:
        return (root->Content.CaseFunc.MathFuncPtr)
            (GetExprValue(root->Content.CaseFunc.Child));
    case CONST_ID:
        return root->Content.CaseConst;
    case T:
        return *(root->Content.CaseParmPtr);
    default:
        return 0.0;
    }
}
static void CalcCoord(
    ExprNode* x_nptr,
    ExprNode* y_nptr,
    double* x_val,
    double* y_val)
{
    double local_x, local_y, temp;
    local_x = GetExprValue(x_nptr); 	// 计算点的原始坐标
    local_y = GetExprValue(y_nptr);
    local_x *= Scale_X_Global; 			// 比例变换
    local_y *= Scale_Y_Global;
    temp = local_x * cos(Rot_Ang_Global) + local_y * sin(Rot_Ang_Global);
    local_y = local_y * cos(Rot_Ang_Global) - local_x * sin(Rot_Ang_Global);
    local_x = temp; 			// 旋转变换
    local_x += Origin_X_Global;			// 平移变换
    local_y += Origin_Y_Global;
    *x_val = local_x; 			// 返回变换后点的坐标
    *y_val = local_y;
}

void DrawPixel(unsigned long x, unsigned long y)
{
    SetPixel(hdc, x, y, 1100);
}
void DrawLoop(
    double Start,
    double End,
    double Step,
    ExprNode* x_ptr,
    ExprNode* y_ptr)
{
    //extern double Parameter_Global;	// 参数T的存储空间
    double x = 0.0, y = 0.0;
    for (Parameter_Global = Start; Parameter_Global <= End; Parameter_Global += Step)
    {
        CalcCoord(x_ptr, y_ptr, &x, &y);   // 计算实际坐标
        DrawPixel((unsigned long)x, (unsigned long)y);  // 根据坐标绘制点
    }
}
void SetOrigin(double x, double y)
{
    Origin_X_Global = x;
    Origin_Y_Global = y;
}
void SetRot(double angle)
{
    Rot_Ang_Global = angle;
}
void SetScale(double x, double y)
{
    Scale_X_Global = x;
    Scale_Y_Global = y;
}