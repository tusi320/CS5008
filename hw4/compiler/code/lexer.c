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

void token_array_append(Token_Array *arr, Token token)
{
    if (arr->count == arr->capacity)
    {
        if (arr->capacity == 0)
        {
            arr->capacity = 16;
        } else
        {
            arr->capacity = arr->capacity * 2;
        }

        arr->items = realloc(arr->items, arr->capacity * sizeof(Token));
        if (!arr->items){
            fprintf(stderr, "Error: Failed to allocate memory for tokens\n");
			exit(1);
        }
    }
    arr->items[arr->count] = token;
    arr->count++;
}


bool is_at_end(Lexer *lx)
{
    return lx->index >= lx->source.count;
}

char peek(Lexer *lx)
{
    if (is_at_end(lx))
    {
        return '\0';
    }
    return lx->source.data[lx->index];
}

char advance(Lexer *lx)
{
    char c = lx->source.data[lx->index];
    lx->index++;

    if (c == '\n')
    {
        lx->loc.line++;
        lx->loc.col = 1;
    }
    else
    {
        lx->loc.col++;
    }
    return c;
}


char peek_next(Lexer *lx)
{
    if (lx->index + 1 >= lx->source.count)
    {
        return '\0';
    }
    return lx->source.data[lx->index + 1];
}

void skip_whitespace_and_comment(Lexer *lx)
{
    for(;;)
    {
        char c = peek(lx);
        if (isspace((unsigned char)c))
        {
            advance(lx);
        }
        else if(c == '/' && peek_next(lx) == '/')
        {
            while(!is_at_end(lx) && peek(lx) != '\n')
            {
                advance(lx);
            }
        }
        else
        {
            return;
        }
    }
        
}

Token lex_identifier_or_keyword(Lexer *lx)
{
    Token token = {0};
    long start = lx->index;
    while (isalnum((unsigned char)peek(lx)) || peek(lx) == '_')
    {
        advance(lx);
    }
    
    String word = {lx->source.data + start, lx->index - start};
    
     //fn let set if while call return true false ELSE
    if (strings_match(word, str_lit("fn"))) {
        token.kind = TOKEN_KEYWORD;
        token.keyword = KEYWORD_FN;
    }
    else if (strings_match(word, str_lit("let"))){
        token.kind = TOKEN_KEYWORD;
        token.keyword = KEYWORD_LET;
    }
    else if(strings_match(word, str_lit("set"))){
        token.kind = TOKEN_KEYWORD;
        token.keyword = KEYWORD_SET;
    }
    else if (strings_match(word, str_lit("if"))){
        token.kind = TOKEN_KEYWORD;
        token.keyword = KEYWORD_IF;
    }
    else if (strings_match(word, str_lit("else"))){
        token.kind = TOKEN_KEYWORD; 
        token.keyword = KEYWORD_ELSE; 
    }
    else if (strings_match(word, str_lit("while"))){
        token.kind = TOKEN_KEYWORD; 
        token.keyword = KEYWORD_WHILE; 
    }
    else if (strings_match(word, str_lit("call"))){
        token.kind = TOKEN_KEYWORD; 
        token.keyword = KEYWORD_CALL; 
    }
    else if (strings_match(word, str_lit("return"))){
        token.kind = TOKEN_KEYWORD; 
        token.keyword = KEYWORD_RETURN; 
    }
    else if (strings_match(word, str_lit("true"))){
        token.kind = TOKEN_KEYWORD; 
        token.keyword = KEYWORD_TRUE; 
    }
    else if (strings_match(word, str_lit("false"))){
        token.kind = TOKEN_KEYWORD; 
        token.keyword = KEYWORD_FALSE; 
    }
    else if (strings_match(word, str_lit("int"))){
        token.kind = TOKEN_TYPE;
        token.type = TYPE_INTEGER; 
    }
    else if (strings_match(word, str_lit("str"))){
        token.kind = TOKEN_TYPE;    
        token.type = TYPE_STRING; 
    }
    else if (strings_match(word, str_lit("bool"))){
        token.kind = TOKEN_TYPE;
        token.type = TYPE_BOOL; 
    }
    else{
        token.kind = TOKEN_IDENT; 
    }
    return token;
}

Token lex_number(Lexer *lx)
{
    Token token = {0};
    long value = 0;

    while(isdigit((unsigned char)peek(lx)))
    {
        char c = advance(lx);
        value = value * 10 + (c - '0');
    }

    token.kind = TOKEN_INTEGER;
    token.long_value = value;
    return token;
}


bool match(Lexer *lx, char expected)
{
    if (peek(lx) == expected)
    {
        advance(lx);
        return true;
    }
    return false;
}

Token lex_symbol(Lexer *lx)
{
    Token token = {0};
    Loc loc = lx->loc;
    char c = advance(lx);
    
    switch(c)
    {
    case '(': token.kind = TOKEN_OPEN_PAREN;    break;
    case ')': token.kind = TOKEN_CLOSE_PAREN;   break;
    case '{': token.kind = TOKEN_OPEN_BRACE;    break;
    case '}': token.kind = TOKEN_CLOSE_BRACE;   break;
    case '[': token.kind = TOKEN_OPEN_BRACKET;  break;
    case ']': token.kind = TOKEN_CLOSE_BRACKET; break;
    case ',': token.kind = TOKEN_COMMA;         break;
    case ':': token.kind = TOKEN_COLON;         break;
    case '+': token.kind = TOKEN_PLUS;          break;
    case '*': token.kind = TOKEN_STAR;          break;
    case '/': token.kind = TOKEN_SLASH;         break;
    case '%': token.kind = TOKEN_PERCENT;       break;
    case '^': token.kind = TOKEN_CARET;         break;
    case '~': token.kind = TOKEN_TILDE;         break;
    
    case '-': token.kind = match(lx, '>') ? TOKEN_ARROW         : TOKEN_MINUS;     break;
    case '=': token.kind = match(lx, '=') ? TOKEN_EQUAL_EQUAL   : TOKEN_ASSIGN;    break;
    case '!': token.kind = match(lx, '=') ? TOKEN_NOT_EQUAL     : TOKEN_BANG;      break;
    case '<': token.kind = match(lx, '=') ? TOKEN_LESS_EQUAL    : TOKEN_LESS;      break;
    case '>': token.kind = match(lx, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER;   break;
    case '&': token.kind = match(lx, '&') ? TOKEN_AND_AND       : TOKEN_AMPERSAND; break;
    case '|': token.kind = match(lx, '|') ? TOKEN_OR_OR         : TOKEN_PIPE;      break;
    
    default:
        fprintf(stderr, "%s:%ld:%ld: ERROR: unexpected character '%c'\n",
                loc.file_name, loc.line, loc.col, c);
        exit(1);
    }

    return token;
}


Token next_token(Lexer *lx)
{
    skip_whitespace_and_comment(lx);
    Loc loc = lx->loc;
    long start = lx->index;

    Token token = {0};

    char c = peek(lx);
    if(is_at_end(lx))
    {
        token.kind = TOKEN_EOF;
    }
    else if (isalpha((unsigned char)c) || c == '_')
    {
        token = lex_identifier_or_keyword(lx);
    }
    else if (isdigit((unsigned char)c))
    {
        token = lex_number(lx);
    }
    else
    {
        token = lex_symbol(lx);
    }

    token.loc    = loc;
    token.source = (String){ lx->source.data + start, lx->index - start };
    return token;
}

void print_token(Token token)
{
    printf("%s:%ld:%ld\t", token.loc.file_name, token.loc.line, token.loc.col);

    switch(token.kind)
    {
    case TOKEN_KEYWORD:
        printf("KEYWORD\t\t%.*s\n", PRINT_STRING(token.source));
        break;
    case TOKEN_TYPE:
        printf("TYPE\t\t%.*s\n", PRINT_STRING(token.source));
        break;
    case TOKEN_IDENT:
        printf("IDENTIFIER\t%.*s\n", PRINT_STRING(token.source));
        break;
    case TOKEN_INTEGER:
        printf("INTEGER\t\t%ld\n", token.long_value);
        break;
    case TOKEN_EOF:
        printf("EOF\n");
        break;
    default:
        printf("%.*s\n", PRINT_STRING(token.source));
        break;
    }

}

void print_token_array(Token_Array tokens)
{
    for (long i = 0; i < tokens.count; i++)
    {
        print_token(tokens.items[i]);
    }
}

Token_Array lex_file(const char *file_name)
{
    Token_Array result = {0};

    Lexer lx = {0};
    lx.source = read_file(file_name);
    lx.index = 0;
    lx.loc.line = 1;
    lx.loc.col = 1;
    lx.loc.file_name = file_name;

    for( ; ;)
    {
        Token token = next_token(&lx);
        token_array_append(&result, token);
        if(token.kind == TOKEN_EOF)
        {
            break;
        }
    }
    return result;
}


