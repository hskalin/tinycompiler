#ifndef PARSE_H
#define PARSE_H

#include "lexer.h"
#include<string>
#include<unordered_set>

// helper function to return token kind as a string
const std::string getTokenName(TokenType kind);

class Parser{
public:
    Lexer lexer;
    Token curToken;
    Token peekToken;

    std::unordered_set<std::string> symbols;        // Variables declared so far
    std::unordered_set<std::string> labelsDeclared; // Lables declared so far
    std::unordered_set<std::string> labelsGotoed;   // Lables goto'ed so far

    Parser(Lexer lex);

    // Return true if the current token matches.
    bool checkToken(TokenType kind);

    // Return true if the next token matches.
    bool checkPeek(TokenType kind);

    // Try to match current token. If not, error. Advances the current token.
    void match(TokenType kind);

    // Advances the current token.
    void nextToken();

    // print error and exit
    void abort(const std::string& message);

    // The grammar for the basic language:

    // program ::= {statement}
    // statement ::= "PRINT" (expression | string) nl
    //     | "IF" comparison "THEN" nl {statement} "ENDIF" nl
    //     | "WHILE" comparison "REPEAT" nl {statement} "ENDWHILE" nl
    //     | "LABEL" ident nl
    //     | "GOTO" ident nl
    //     | "LET" ident "=" expression nl
    //     | "INPUT" ident nl
    // comparison ::= expression (("==" | "!=" | ">" | ">=" | "<" | "<=") expression)+
    // expression ::= term {( "-" | "+" ) term}
    // term ::= unary {( "/" | "*" ) unary}
    // unary ::= ["+" | "-"] primary
    // primary ::= number | ident
    // nl ::= '\n'+

    // {} means zero or more, [] means zero or one, + means one or more of whatever is to 
    // the left, () is just for grouping, and | is a logical or
    // we will implement a function for each rule

    // program rule, this is the root of the parser, i.e. the parent rule
    // program ::= {statement}
    void program();

    // statement ::= has 7 rules
    void statement();

    // primary ::= number | ident
    void primary();    

    // unary ::= ["+" | "-"] primary
    void unary();

    // term ::= unary {( "/" | "*" ) unary}
    void term();

    // expression ::= term {( "-" | "+" ) term}
    void expression();

    // checks if curToken is a comparison operator
    bool isComparisonOperator();

    // comparison ::= expression (("==" | "!=" | ">" | ">=" | "<" | "<=") expression)+
    void comparison();

    // nl ::= '\n'+
    void nl();
};

#endif // PARSE_H