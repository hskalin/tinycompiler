#include "lexer.h"
#include<iostream>
#include<string>
#include<unordered_map>

Token::Token (std::string tokenText, TokenType tokenKind)
    : text {tokenText}
    , kind {tokenKind}
    {}

Token::Token ()
    : text {""}
    , kind {TokenType::NAN}
    {}

// see https://www.learncpp.com/cpp-tutorial/constructor-member-initializer-lists/ for member init syntax
Lexer::Lexer(const std::string& src)
    : source {src + '\n'}
    , curChar {'\0'}
    , curPos {-1}
    {
        std::cout << "lexer initialized\n";
        nextChar();   
    }

void Lexer::nextChar(){
    curPos++;
    if ((unsigned) curPos >= source.size())
        curChar = '\0'; // EOF
    else
        curChar = source[curPos];
}

char Lexer::peek(){
    if ((unsigned) curPos+1 >= source.size())
        return '\0';
    return source[curPos+1];
}

void Lexer::abort(const std::string& message){
    std::cerr << "Lexing Error: " << message << '\n';
    exit(1);
}

void Lexer::skipWhitespace(){
    while (curChar==' ' || curChar=='\t' || curChar=='\r')
        nextChar();
}

void Lexer::skipComment(){
    if (curChar == '#')
        while (curChar != '\n')
            nextChar();
}

Token Lexer::getToken(){
    Token token;
    skipWhitespace();
    skipComment();

    // other things that work. in the first two the 1 is how many times to repeat curChar
    // std::string curString = std::string(1,curChar);
    // std::string curString(1, curChar);
    std::string curString{curChar};

    switch (curChar) {
    case '+':  
        token = Token(curString, TokenType::PLUS);
        break;
    case '-':
        token = Token(curString, TokenType::MINUS);
        break;
    case '*':
        token = Token(curString, TokenType::ASTERISK);
        break;
    case '/':
        token = Token(curString, TokenType::SLASH);
        break;
    case '=':
        if (peek() == '='){
            nextChar();
            curString += curChar;
            token = Token(curString, TokenType::EQEQ);
        }
        else
            token = Token(curString, TokenType::EQ);
        break;
    case '>':
        if (peek() == '='){
            nextChar();
            curString += curChar;
            token = Token(curString, TokenType::GTEQ);
        }
        else
            token = Token(curString, TokenType::GT);
        break;
    case '<':
        if (peek() == '='){
            nextChar();
            curString += curChar;
            token = Token(curString, TokenType::LTEQ);
        }
        else
            token = Token(curString, TokenType::LT);
        break;
    case '!':
        if (peek() == '='){
            nextChar();
            curString += curChar;
            token = Token(curString, TokenType::NOTEQ);
        }
        else
            abort("Expected !=, got !" + std::string(1,peek()));
        break;
    case '\"': {
        nextChar();
        unsigned long startPos = (unsigned) curPos;
        while (curChar != '\"'){
            // Don't allow special characters in the string. No escape characters, newlines, tabs, or %.
            // We will be using C's printf on this string.
            if (curChar=='\r' || curChar=='\n' || curChar=='\t' || curChar=='\\' || curChar=='%')
                abort("Illegal character in string.");
            nextChar();
        }
        curString = source.substr(startPos, (unsigned)curPos - startPos);
        token = Token(curString, TokenType::STRING);
    }
        break;
    case '\n':
        token = Token(curString, TokenType::NEWLINE);
        break;
    case '\0':
        token = Token(curString, TokenType::T_EOF);
        break;
    default: // unknow token or more processing required
        {
            if (isdigit(curChar)){
                // Leading character is a digit, so this must be a number.
                // Get all consecutive digits and decimal if there is one.
                unsigned long startPos = (unsigned) curPos;
                while (isdigit(peek()))
                    nextChar();
                if (isalpha(peek()))
                    abort("IDENT can't start with numbers");
                if (peek() == '.'){
                    nextChar();
                    if (!isdigit(peek()))
                        abort("Illegal character in number.");
                    while(isdigit(peek()))
                        nextChar();
                }
                curString = source.substr(startPos, (unsigned)curPos - startPos +1);
                token = Token(curString, TokenType::NUMBER);
            }
            else if (isalpha(curChar)){
                // Leading character is a letter, so this must be an identifier or a keyword.
                // Get all consecutive alpha numeric characters.
                unsigned long startPos = (unsigned) curPos;
                while (isalnum(peek()))
                    nextChar();
                
                curString = source.substr(startPos, (unsigned)curPos - startPos +1);
                if (Token::keywordMap.find(curString) != Token::keywordMap.end())
                    token = Token(curString, Token::keywordMap[curString]);
                else
                    token = Token(curString, TokenType::IDENT);
            }
            else
                abort("Unknown token: " + curString);
        }
        break;
    }

    nextChar();
    return token;
}