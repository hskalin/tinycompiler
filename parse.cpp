#include "parse.h"
#include "lexer.h"
#include<string>
#include<iostream>
#include<sstream>

Parser::Parser(Lexer lex)
    : lexer {lex}
    {
        nextToken();
        nextToken();  // Call this twice to initialize current and peek.
    }

void Parser::abort(const std::string& message){
    std::cerr << "Error " << message << std::endl;
    exit(1);
}

bool Parser::checkToken(TokenType kind){
    return kind == curToken.kind;
}

bool Parser::checkPeek(TokenType kind){
    return kind == peekToken.kind;
}

void Parser::match(TokenType kind){
    if (!checkToken(kind)) {
        std::stringstream buff;
        buff << "Expected kind " << static_cast<int>(kind) << ", got " << static_cast<int>(curToken.kind);
        abort(buff.str());
    }
    nextToken();
}

void Parser::nextToken(){
    curToken = peekToken;
    peekToken = lexer.getToken();
}

