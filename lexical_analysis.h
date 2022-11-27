#ifndef _Lexical_Analysis_H
#define _Lexical_Analysis_H

#define BUFFER_SIZE 1024
#define BOOL int
#define TRUE 1
#define FALSE 0

typedef enum
{
    ORIGIN, SCALE, ROT, IS,
    TO, STEP, DRAW, FOR, FROM,
    T,
    SEMICO, L_BRACKET, R_BRACKET, COMMA,
    PLUS, MINUS, MUL, DIV, POWER,
    FUNC,
    CONST_ID,
    COMMENT,
    NONTOKEN,
    ERRTOKEN
} Token_Type;

typedef struct
{
    Token_Type type;
    char* lexeme;
    double value;
    double (*FuncPtr)(double);
} Token;

int InitScanner(char* fp_name);
Token GetToken();
void CloseScanner();

#endif
