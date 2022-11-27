#ifndef _Semantic_Anylysis_H
#define _Semantic_Anylysis_H

#include "lexical_analysis.h"

#ifdef GLOBALS
double Parameter_Global = 0;		    // Ϊ����T����ı���
double Origin_X_Global = 0.0, Origin_Y_Global = 0.0;// ���ڼ�¼ƽ�ƾ���
double Rot_Ang_Global = 0.0;		    // ���ڼ�¼��ת�Ƕ�
double Scale_X_Global = 1.0, Scale_Y_Global = 1.0;	    // ���ڼ�¼��������
HDC         hdc;  //�豸�������
PAINTSTRUCT ps;
RECT        rect;
#else
extern double Parameter_Global;		    // Ϊ����T����ı���
extern double Origin_X_Global, Origin_Y_Global;// ���ڼ�¼ƽ�ƾ���
extern double Rot_Ang_Global;		    // ���ڼ�¼��ת�Ƕ�
extern double Scale_X_Global, Scale_Y_Global;	    // ���ڼ�¼��������
extern HDC         hdc;  //�豸�������
extern PAINTSTRUCT ps;
extern RECT        rect;
#endif // GLOBALS


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
