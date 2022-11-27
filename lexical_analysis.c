#include <stdio.h>
#include <math.h>
#include <string.h>

#include "lexical_analysis.h"
#pragma warning(disable:4996)

static FILE* fp;
static char Buffer[BUFFER_SIZE];
static char* c_ptr = Buffer;
static char* f_ptr = Buffer;
static BOOL if_file_finished = FALSE;   //�ļ��Ƿ�����־��FALSE��ʾ�ļ��л�������δ����ȡ��TRUE��ʾ�ļ���������Ϣ���Զ���

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
char* char_ptr_copy(char* sou, char* des, int len)  //�ַ������ƣ���sou���ĳ���Ϊlen���ַ������Ƶ�des��������ֵΪdes
{
	char* temp;
	des = (char*)malloc(1);
	temp = des;
	for (int i = 0; i < len; i++) {
		*temp = *sou;
		temp++;
		sou++;
	}
	*temp = 0;  //��β����'\0',ʹ����Ϊһ���������ַ���
	return des;
}
Token Match(Token token)    //token.lexeme����ƥ�䣬����ƥ����token
{                           //��δƥ��ɹ�����֤�������д�����ERRTOKEN��token
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
int GetNextBuffer(char* buffer, char* start, char* end)  //�ڼ�⵽Buffer�ս��'\0'ʱ�������ļ��м�����ȡ�µ�����ˢ��Buffer�����ݡ�
{
	if (!if_file_finished)  //�ж��Ƿ�ɶ�
	{
		int size_of_update, len;
		len = end - start;
		for (int i = 0; i < len; i++) //��start�����ַ��ƶ���Buffer�ײ�
		{
			Buffer[i] = *(start + i);
		}
		start = Buffer;
		end = start + len;
		size_of_update = fread(end, 1, BUFFER_SIZE - len - 1, fp);  //���ļ��ж�ȡ�µ��ַ�����䵽Buffer��
		Buffer[size_of_update + len] = 0;

		if (size_of_update < (BUFFER_SIZE - len - 1))   //��ʵ�ʶ�ȡ�������ݸ���С��Ӧ�ö�ȡ�ĸ�������֤���ļ��Ѿ����꣬����־λ��ΪTRUE
		{
			if_file_finished = TRUE;
		}
		return size_of_update;
	}
	else return -1;
}
char* LexemeRedirect(char* start, char* end)    //c_ptr��f_ptr��Buffer�н����ض�λ����λ����һ�����ؿ�ʼ��λ�ã��Ա�ȷ����һ������
{
	while ((*end == ' ') || (*end == '\n') || (*end == '\t'))   //������һ����������ַ�����Ϊ��һ�����صĿ�ʼ
	{
		end++;
	}
	start = end;
	return start;   //���ظ�ָ�룬�ڵ��ú�������c_ptrָ���λ��
}
void LexemeRedirectAfterComment()   //�ڼ�⵽ע�ͷ��ź���е�c_ptr�ض�λ�������Իس�����Ϊע�͵Ľ���
{
	while (*f_ptr != '\n')  //����Buffer�еĻس���
	{
		if ((*f_ptr == '\0') && (if_file_finished == FALSE))    //������Bufferĩβ��ע����Ȼû�н�������ʱ�ļ�����δ����Ϣ�����ȡ�µ����ݵ�Buffer��
		{
			GetNextBuffer(Buffer, f_ptr - 1, f_ptr);
			c_ptr = Buffer;
			f_ptr = Buffer;
		}
		else if ((*f_ptr == '\0') && (if_file_finished == TRUE))    //������Bufferĩβ������Ҳ���ļ�ĩβ���ļ�����δ����Ϣ������c_ptrָ��'\0'
		{                                                           //��GetToken�����л᷵��NONTOKEN�����ļ�������־
			c_ptr = f_ptr;
			return;
		}
		else
		{
			f_ptr++;
		}
	}
	c_ptr = f_ptr + 1;  //�س���֮����Ǹ��ַ���Ϊ��һ�����صĿ�ʼ
	f_ptr = c_ptr;
}

Token GetToken()
{
	Token token = { ERRTOKEN, "", 0.0, NULL };
	int offset;

S:  while ((*c_ptr == ' ') || (*c_ptr == '\n') || (*c_ptr == '\t')) //���������ַ�
{
	c_ptr++;
	f_ptr++;
}
if (isalpha(*c_ptr))         //  ʶ��ID
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
else if (isdigit(*c_ptr))    // ʶ�����ֳ���
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
		if (if_file_finished == TRUE)   //��ȡ��Buffer��β��ͬʱҲ���ļ���β����֤���ļ�����
		{
			token.type = NONTOKEN;
			return token;
		}
		else                            //��ȡ��Buffer��β�������ļ���β������ļ��ж�ȡ�µ��ַ���Buffer�У���goto S���¿�ʼʶ��
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