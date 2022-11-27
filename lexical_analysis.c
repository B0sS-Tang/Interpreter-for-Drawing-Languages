#include <stdio.h>
#include <math.h>
#include <string.h>

#include "lexical_analysis.h"
#pragma warning(disable:4996)

static FILE* fp;
static char Buffer[BUFFER_SIZE];
static char* c_ptr = Buffer;
static char* f_ptr = Buffer;
static BOOL if_file_finished = FALSE;   //文件是否读完标志，FALSE表示文件中还有内容未被读取，TRUE表示文件中所有信息均以读完

static Token TokenTab[] =
{
	{CONST_ID,    "PI",     3.1415926,    NULL},
	{CONST_ID,    "E",      2.71828,      NULL},
	{T,           "T",      0.0,          NULL},
	{FUNC,        "SIN",    0.0,          sin},
	{FUNC,        "COS",    0.0,          cos},
	{FUNC,        "TAN",    0.0,          tan},
	{FUNC,        "LN",     0.0,          log},
	{FUNC,        "EXP",    0.0,          exp},
	{FUNC,        "SQRT",   0.0,          sqrt},
	{ORIGIN,      "ORIGIN", 0.0,          NULL},
	{SCALE,       "SCALE",  0.0,          NULL},
	{ROT,         "ROT",    0.0,          NULL},
	{IS,          "IS",     0.0,          NULL},
	{FOR,         "FOR",    0.0,          NULL},
	{FROM,        "FROM",   0.0,          NULL},
	{TO,          "TO",     0.0,          NULL},
	{STEP,        "STEP",   0.0,          NULL},
	{DRAW,        "DRAW",   0.0,          NULL}
};

int isalpha(char ch);
int isdigit(char ch);
char* char_ptr_copy(char* sou, char* des, int len);
Token Match(Token token);
int GetNextBuffer(char* buffer, char* start, char* end);
char* LexemeRedirect(char* start, char* end);
void LexemeRedirectAfterComment();

int isalpha(char ch)
{
	if (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'))) return 1;
	else return 0;
}
int isdigit(char ch)
{
	if ((ch >= '0') && (ch <= '9')) return 1;
	else return 0;
}
char* char_ptr_copy(char* sou, char* des, int len)  //字符串复制，将sou处的长度为len的字符串复制到des处，返回值为des
{
	char* temp;
	des = (char*)malloc(1);
	temp = des;
	for (int i = 0; i < len; i++) {
		*temp = *sou;
		temp++;
		sou++;
	}
	*temp = 0;  //结尾加上'\0',使它成为一个完整的字符串
	return des;
}
Token Match(Token token)    //token.lexeme进行匹配，返回匹配后的token
{                           //若未匹配成功，则证明输入有错，返回ERRTOKEN的token
	Token* temp;
	temp = TokenTab;
	for (int i = 0; i < 18; i++)
	{
		if (stricmp(token.lexeme, (temp + i)->lexeme) == 0)
		{
			token.type = (temp + i)->type;
			token.value = (temp + i)->value;
			token.FuncPtr = (temp + i)->FuncPtr;
			token.lexeme = strcpy(token.lexeme, (temp + i)->lexeme);
			return token;
		}
	}
	token.type = ERRTOKEN;
	token.lexeme = "";
	token.value = 0.0;
	token.FuncPtr = NULL;
	return token;
}
int GetNextBuffer(char* buffer, char* start, char* end)  //在检测到Buffer终结符'\0'时，将从文件中继续读取新的内容刷新Buffer的内容。
{
	if (!if_file_finished)  //判断是否可读
	{
		int size_of_update, len;
		len = end - start;
		for (int i = 0; i < len; i++) //将start处的字符移动到Buffer首部
		{
			Buffer[i] = *(start + i);
		}
		start = Buffer;
		end = start + len;
		size_of_update = fread(end, 1, BUFFER_SIZE - len - 1, fp);  //从文件中读取新的字符串填充到Buffer中
		Buffer[size_of_update + len] = 0;

		if (size_of_update < (BUFFER_SIZE - len - 1))   //若实际读取到的数据个数小于应该读取的个数，则证明文件已经读完，将标志位置为TRUE
		{
			if_file_finished = TRUE;
		}
		return size_of_update;
	}
	else return -1;
}
char* LexemeRedirect(char* start, char* end)    //c_ptr与f_ptr在Buffer中进行重定位，定位到下一个词素开始的位置，以便确定下一个词素
{
	while ((*end == ' ') || (*end == '\n') || (*end == '\t'))   //搜索下一个有意义的字符，即为下一个词素的开始
	{
		end++;
	}
	start = end;
	return start;   //返回该指针，在调用函数中令c_ptr指向该位置
}
void LexemeRedirectAfterComment()   //在检测到注释符号后进行的c_ptr重定位，我们以回车符作为注释的结束
{
	while (*f_ptr != '\n')  //搜索Buffer中的回车符
	{
		if ((*f_ptr == '\0') && (if_file_finished == FALSE))    //搜索到Buffer末尾，注释依然没有结束，此时文件还有未读信息，则读取新的内容到Buffer中
		{
			GetNextBuffer(Buffer, f_ptr - 1, f_ptr);
			c_ptr = Buffer;
			f_ptr = Buffer;
		}
		else if ((*f_ptr == '\0') && (if_file_finished == TRUE))    //搜索到Buffer末尾，并且也是文件末尾（文件中无未读信息），则将c_ptr指向'\0'
		{                                                           //在GetToken函数中会返回NONTOKEN，即文件结束标志
			c_ptr = f_ptr;
			return;
		}
		else
		{
			f_ptr++;
		}
	}
	c_ptr = f_ptr + 1;  //回车符之后的那个字符即为下一个词素的开始
	f_ptr = c_ptr;
}

Token GetToken()
{
	Token token = { ERRTOKEN, "", 0.0, NULL };
	int offset;

S:  while ((*c_ptr == ' ') || (*c_ptr == '\n') || (*c_ptr == '\t')) //过滤无用字符
{
	c_ptr++;
	f_ptr++;
}
if (isalpha(*c_ptr))         //  识别ID
{
	while (isalpha(*f_ptr)) f_ptr++;
	if ((*f_ptr == '\0') && (if_file_finished == FALSE))
	{
		offset = f_ptr - c_ptr;
		GetNextBuffer(Buffer, c_ptr, f_ptr);
		c_ptr = Buffer;
		f_ptr = c_ptr + offset;
	}
	while (isalpha(*f_ptr)) f_ptr++;

	token.lexeme = char_ptr_copy(c_ptr, token.lexeme, f_ptr - c_ptr);
	token = Match(token);
	c_ptr = LexemeRedirect(c_ptr, f_ptr);
	f_ptr = c_ptr;
	return token;
}
else if (isdigit(*c_ptr))    // 识别数字常量
{
	while (isdigit(*f_ptr)) f_ptr++;
	if ((*f_ptr == '\0') && (if_file_finished == FALSE))
	{
		offset = f_ptr - c_ptr;
		GetNextBuffer(Buffer, c_ptr, f_ptr);
		c_ptr = Buffer;
		f_ptr = c_ptr + offset;
	}
	while (isdigit(*f_ptr)) f_ptr++;

	if (*f_ptr == '.') f_ptr++;

	while (isdigit(*f_ptr)) f_ptr++;
	if ((*f_ptr == '\0') && (if_file_finished == FALSE))
	{
		offset = f_ptr - c_ptr;
		GetNextBuffer(Buffer, c_ptr, f_ptr);
		c_ptr = Buffer;
		f_ptr = c_ptr + offset;
	}
	while (isdigit(*f_ptr)) f_ptr++;

	token.type = CONST_ID;
	token.value = atof(c_ptr);
	token.lexeme = char_ptr_copy(c_ptr, token.lexeme, f_ptr - c_ptr);
	c_ptr = LexemeRedirect(c_ptr, f_ptr);
	f_ptr = c_ptr;
	return token;
}
else
{
	switch (*c_ptr)
	{
	case ';':
		token.type = SEMICO;
		token.lexeme = ";";
		f_ptr++;
		c_ptr = LexemeRedirect(c_ptr, f_ptr);
		f_ptr = c_ptr;
		return token;
	case '+':
		token.type = PLUS;
		token.lexeme = "+";
		f_ptr++;
		c_ptr = LexemeRedirect(c_ptr, f_ptr);
		f_ptr = c_ptr;
		return token;
	case ',':
		token.type = COMMA;
		token.lexeme = ",";
		f_ptr++;
		c_ptr = LexemeRedirect(c_ptr, f_ptr);
		f_ptr = c_ptr;
		return token;
	case '(':
		token.type = L_BRACKET;
		token.lexeme = "(";
		f_ptr++;
		c_ptr = LexemeRedirect(c_ptr, f_ptr);
		f_ptr = c_ptr;
		return token;
	case ')':
		token.type = R_BRACKET;
		token.lexeme = ")";
		f_ptr++;
		c_ptr = LexemeRedirect(c_ptr, f_ptr);
		f_ptr = c_ptr;
		return token;
	case '*':
		f_ptr++;
		if (*f_ptr == '*')
		{
			token.type = POWER;
			token.lexeme = "**";
			f_ptr++;
			c_ptr = LexemeRedirect(c_ptr, f_ptr);
			f_ptr = c_ptr;
			return token;
		}
		else
		{
			token.type = MUL;
			token.lexeme = "*";
			c_ptr = LexemeRedirect(c_ptr, f_ptr);
			f_ptr = c_ptr;
			return token;
		}
	case '/':
		f_ptr++;
		if (*f_ptr == '/')
		{
			token.type = COMMENT;
			token.lexeme = "//";
			f_ptr++;
			//c_ptr = LexemeRedirect(c_ptr, f_ptr);
			//f_ptr = c_ptr;
			LexemeRedirectAfterComment();
			token = GetToken();
			return token;
		}
		else
		{
			token.type = DIV;
			token.lexeme = "/";
			c_ptr = LexemeRedirect(c_ptr, f_ptr);
			f_ptr = c_ptr;
			return token;
		}
	case '-':
		f_ptr++;
		if (*f_ptr == '-')
		{
			token.type = COMMENT;
			token.lexeme = "--";
			f_ptr++;
			//c_ptr = LexemeRedirect(c_ptr, f_ptr);
			//f_ptr = c_ptr;
			LexemeRedirectAfterComment();
			token = GetToken();
			return token;
		}
		else
		{
			token.type = MINUS;
			token.lexeme = "-";
			c_ptr = LexemeRedirect(c_ptr, f_ptr);
			f_ptr = c_ptr;
			return token;
		}
	case '\0':
		if (if_file_finished == TRUE)   //读取到Buffer结尾，同时也是文件结尾，即证明文件结束
		{
			token.type = NONTOKEN;
			return token;
		}
		else                            //读取到Buffer结尾但不是文件结尾，则从文件中读取新的字符到Buffer中，并goto S重新开始识别
		{
			offset = f_ptr - c_ptr;
			GetNextBuffer(Buffer, c_ptr, f_ptr);
			c_ptr = Buffer;
			f_ptr = c_ptr + offset;
			goto S;
		}
	default:
		return token;
	}
}
}

int InitScanner(char* f_name)
{
	fp = fopen(f_name, "r");
	if (fp == NULL) return 0;
	else
	{
		GetNextBuffer(Buffer, c_ptr, f_ptr);
		return 1;
	}
}

void CloseScanner()
{
	fclose(fp);
}