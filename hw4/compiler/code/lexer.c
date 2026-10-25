#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "string.c"

typedef enum Token_Kind
{
    TOKEN_KEYWORD,
    TOKEN_TYPE,
    TOKEN_IDENT,
    TOKEN_INTEGER,
    TOKEN_STRING,

    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_OPEN_BRACE,
    TOKEN_CLOSE_BRACE,
    TOKEN_OPEN_BRACKET,
    TOKEN_CLOSE_BRACKET,

    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_ARROW,
    TOKEN_ASSIGN,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_PERCENT,

    TOKEN_AMPERSAND,
    TOKEN_PIPE,
    TOKEN_CARET,
    TOKEN_TILDE,

    TOKEN_EQUAL_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS,
    TOKEN_GREATER,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_EQUAL,

    TOKEN_AND_AND,
    TOKEN_OR_OR,
    TOKEN_BANG,
    
    TOKEN_EOF,
    //+ - * / % & | ^ == != < > <= >= && || ~ ! ( ) [ ] { } , :
}Token_Kind;

typedef enum Keyword
{
    KEYWORD_FN,
    KEYWORD_LET,
    KEYWORD_SET,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_CALL,
    KEYWORD_RETURN,
    KEYWORD_TRUE,
    KEYWORD_FALSE,
    //fn let set if while call return true false ELSE
}Keyword;

typedef enum Type
{
    TYPE_INTEGER,
    TYPE_STRING,
    TYPE_BOOL,
    //int str bool
}Type;

typedef struct Loc
{
    long   line;
    long   col;
    const char *file_name;
}Loc;

typedef struct Token
{
    Token_Kind kind;
    String     source;
    Loc        loc;

    union
    {
        Keyword keyword;
        Type    type;
        long    long_value;
    };

}Token;

typedef struct Token_Array
{
    Token *items;
    long count;
    long capacity;
}Token_Array;

typedef struct Lexer
{
    String source;
    long   index;
    Loc loc;
    
}Lexer;

String read_file(const char *file_name)
{
    String result = {0};
    FILE *file = fopen(file_name, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "ERROR: could not open file %s\n", file_name);
        exit(1);
    }

    fseek(file, 0 , SEEK_END);//fseek(哪个文件, 偏移多少字节, 从哪里算起);
    long size = ftell(file);
    fseek(file, 0 ,SEEK_SET);
    result.data = malloc(size + 1);
    fread(result.data, 1 , size, file); //fread(放到哪里, 每块多大, 读几块, 从哪个文件读);
    result.data[size] = '\0';
    result.count = size;
    fclose(file);
    return result;
    /**
    fopen           打开文件，光标在 0
    fseek END       光标移到末尾 37
    ftell           问位置 → 37，就是文件大小
    fseek SET       光标移回 0
    malloc 38       申请 38 字节内存
    fread 37        从光标位置读 37 字节进内存
    data[37]='\0'   补结尾
    count = 37      记录长度
    fclose          关闭文件
    */
}



