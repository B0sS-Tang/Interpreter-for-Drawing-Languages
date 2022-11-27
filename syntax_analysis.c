
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <Windows.h>

#include "syntax_analysis.h"
//#include "semantic_analysis.h"

Token Token_Global;	//记号的全局变量

ExprNode* MakeExprNode(Token_Type opcode, ...);
//分析器所需的辅助子程序 
void FetchToken();
void MatchToken(Token_Type AToken);
void SyntaxError(int case_of);
//主要产生式的递归子程序
void Parser(char* SrcFilePtr);
void Program();
void Statement();
void OriginStatement();
void RotStatement();
void ScaleStatement();
void ForStatement();
ExprNode* Expression();
ExprNode* Term();
ExprNode* Factor();
ExprNode* Component();
ExprNode* Atom();


ExprNode* MakeExprNode(Token_Type opcode, ...)
{
	ExprNode* ExprPtr = (ExprNode*)malloc(sizeof(ExprNode));
	va_list ArgPtr;
	ExprPtr->OpCode = opcode;
	va_start(ArgPtr, opcode);
	switch (opcode)
	{
	case CONST_ID:	// 常数节点
		ExprPtr->Content.CaseConst = (double)va_arg(ArgPtr, double);
		break;
	case T:		// 参数节点
		ExprPtr->Content.CaseParmPtr = (double*)va_arg(ArgPtr, double*);
		break;
	case FUNC:	       // 函数调用节点
		ExprPtr->Content.CaseFunc.MathFuncPtr = (FuncPtr)va_arg(ArgPtr, FuncPtr);
		ExprPtr->Content.CaseFunc.Child = (ExprNode*)va_arg(ArgPtr, ExprNode*);
		break;
	default:	       // 二元运算节点
		ExprPtr->Content.CaseOperator.Left = (ExprNode*)va_arg(ArgPtr, ExprNode*);
		ExprPtr->Content.CaseOperator.Right = (ExprNode*)va_arg(ArgPtr, ExprNode*);
		break;
	}
	va_end(ArgPtr);
	return ExprPtr;
}

static void FetchToken()
{
	Token_Global = GetToken();
	if (Token_Global.type == ERRTOKEN) SyntaxError(1);
}
void MatchToken(Token_Type AToken)
{
	if (Token_Global.type == AToken) FetchToken();
	else SyntaxError(2);
}
void SyntaxError(int case_of)
{
	switch (case_of)
	{
	case 1:
		printf("lexical error!\n");
		exit(1);
	case 2:
		printf("syntax error!\n");
		exit(2);
	default:
		break;
	}
	printf("syntax error!\n");
}
void Parser(char* SrcFilePtr)	//分析器函数
{
	if (!InitScanner(SrcFilePtr))	//初始化词法分析器
	{
		printf("Open Source File Error ! \n");
		return;
	}
	FetchToken();	// 获取第一个记号
	Program();		// 递归下降分析
	CloseScanner();	// 关闭词法分析器
}
static void Program()
{
	while (Token_Global.type != NONTOKEN)
	{
		Statement();
		MatchToken(SEMICO);
	}
}
static void Statement()
{
	switch (Token_Global.type)
	{
	case ORIGIN:
		OriginStatement();
		break;
	case ROT:
		RotStatement();
		break;
	case SCALE:
		ScaleStatement();
		break;
	case FOR:
		ForStatement();
		break;
	default:
		SyntaxError(1);
		break;
	}
}
static void OriginStatement()
{
	double x, y;

	ExprNode* x_ptr, * y_ptr;
	MatchToken(ORIGIN);
	MatchToken(IS);
	MatchToken(L_BRACKET);
	x_ptr = Expression();	x = GetExprValue(x_ptr);
	MatchToken(COMMA);
	y_ptr = Expression();	y = GetExprValue(y_ptr);
	MatchToken(R_BRACKET);	SetOrigin(x, y);
}
static void RotStatement()
{
	double angle;

	ExprNode* angle_ptr;
	MatchToken(ROT);
	MatchToken(IS);
	angle_ptr = Expression();	angle = GetExprValue(angle_ptr);	SetRot(angle);
}
static void ScaleStatement()
{
	double x, y;

	ExprNode* x_ptr, * y_ptr;
	MatchToken(SCALE);
	MatchToken(IS);
	MatchToken(L_BRACKET);
	x_ptr = Expression();	x = GetExprValue(x_ptr);
	MatchToken(COMMA);
	y_ptr = Expression();	y = GetExprValue(y_ptr);
	MatchToken(R_BRACKET);	SetScale(x, y);
}
static void ForStatement()
{
	double Start, End, Step;
	ExprNode* start_ptr, * end_ptr, * step_ptr,
		* x_ptr, * y_ptr;
	MatchToken(FOR);
	MatchToken(T);
	MatchToken(FROM);
	start_ptr = Expression();	Start = GetExprValue(start_ptr);
	MatchToken(TO);
	end_ptr = Expression();		End = GetExprValue(end_ptr);
	MatchToken(STEP);
	step_ptr = Expression();	Step = GetExprValue(step_ptr);
	MatchToken(DRAW);
	MatchToken(L_BRACKET);
	x_ptr = Expression();
	MatchToken(COMMA);
	y_ptr = Expression();
	MatchToken(R_BRACKET);		DrawLoop(Start, End, Step, x_ptr, y_ptr);
}

static ExprNode* Expression()
{
	ExprNode* left, * right;
	Token_Type token_type;
	left = Term();
	while (Token_Global.type == PLUS || Token_Global.type == MINUS)
	{
		token_type = Token_Global.type;
		MatchToken(token_type);
		right = Term();
		left = MakeExprNode(token_type, left, right);
	}
	return left;
}
static ExprNode* Term()
{
	ExprNode* left, * right;
	Token_Type token_type;
	left = Factor();
	while (Token_Global.type == MUL || Token_Global.type == DIV)
	{
		token_type = Token_Global.type;
		MatchToken(token_type);
		right = Factor();
		left = MakeExprNode(token_type, left, right);
	}
	return left;
}
static ExprNode* Factor()
{
	ExprNode* left, * zero;
	Token_Type token_type;
	if ((Token_Global.type == PLUS) || (Token_Global.type == MINUS))
	{
		if (Token_Global.type == MINUS)
		{
			zero = (ExprNode*)malloc(sizeof(ExprNode));
			zero->OpCode = CONST_ID;
			zero->Content.CaseConst = 0.0;
			token_type = Token_Global.type;
			MatchToken(token_type);
			left = Factor();
			left = MakeExprNode(token_type, zero, left);
		}
		else
		{
			token_type = Token_Global.type;
			MatchToken(token_type);
			left = Factor();
			left = MakeExprNode(token_type, left);
		}
	}
	else
	{

		left = Component();
	}
	return left;
}
static ExprNode* Component()
{
	ExprNode* left, * right;
	Token_Type token_type;
	left = Atom();
	if (Token_Global.type == POWER)
	{
		token_type = Token_Global.type;
		MatchToken(token_type);
		right = Component();
		left = MakeExprNode(token_type, left, right);
	}
	return left;
}
static ExprNode* Atom()
{
	ExprNode* child;
	Token_Type token_type;
	FuncPtr fun_ptr;
	double const_id;
	double* ptr;
	
	switch (Token_Global.type)
	{
	case CONST_ID:
		token_type = Token_Global.type;
		const_id = Token_Global.value;
		MatchToken(token_type);
		child = MakeExprNode(token_type, const_id);
		return child;
	case T:
		token_type = Token_Global.type;
		MatchToken(token_type);
		ptr = (double*)malloc(sizeof(double));
		ptr = &Parameter_Global;
		child = MakeExprNode(token_type, ptr);
		return child;
	case FUNC:
		token_type = Token_Global.type;
		fun_ptr = Token_Global.FuncPtr;
		MatchToken(token_type);
		MatchToken(L_BRACKET);
		child = Expression();
		MatchToken(R_BRACKET);
		child = MakeExprNode(token_type, fun_ptr, child);
		return child;
	case L_BRACKET:
		MatchToken(L_BRACKET);
		child = Expression();
		MatchToken(R_BRACKET);
		return child;
	default:
		SyntaxError(1);
		break;
	}
}
