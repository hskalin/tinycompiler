#ifndef LEXER_H
#define LEXER_H

#include<string>
#include<unordered_map>

// TokenType is our enum for all the types of tokens
// using enum class instead https://stackoverflow.com/questions/482745/namespaces-for-enum-types-best-practices
// https://www.learncpp.com/cpp-tutorial/scoped-enumerations-enum-classes/
enum class TokenType {
    T_EOF = -1,
    NEWLINE = 0,
    NUMBER = 1,
    IDENT = 2,
    STRING = 3,
    // Keywords
    LABEL = 101,
    GOTO = 102,
    PRINT = 103,
    INPUT = 104,
    LET = 105,
    IF = 106,
    THEN = 107,
    ENDIF = 108,
    WHILE = 109,
    REPEAT = 110,
    ENDWHILE = 111,
    // Operators
    EQ = 201,
    PLUS = 202,
    MINUS = 203,
    ASTERISK = 204,
    SLASH = 205,
    EQEQ = 206,
    NOTEQ = 207,
    LT = 208,
    LTEQ = 209,
    GT = 210,
    GTEQ = 211,
    // None
    NAN = 666,
};

// Token contains the original text and the type of token.
class Token {
public:
    std::string text;
    TokenType kind;
    static inline std::unordered_map<std::string, TokenType> keywordMap {
        {"LABEL", TokenType::LABEL},
        {"GOTO", TokenType::GOTO},
        {"PRINT", TokenType::PRINT},
        {"INPUT", TokenType::INPUT},
        {"LET", TokenType::LET},
        {"IF", TokenType::IF},
        {"THEN", TokenType::THEN},
        {"ENDIF", TokenType::ENDIF},
        {"WHILE", TokenType::WHILE},
        {"REPEAT", TokenType::REPEAT},
        {"ENDWHILE", TokenType::ENDWHILE},
    };

    Token (std::string tokenText, TokenType tokenKind);

    // default constuctor
    Token ();
};

// the main class that will take in the source code and tokenize it
class Lexer {
public:
    std::string source;
    char curChar; // current character in the string.
    long curPos; // Current position in the string.

    Lexer(const std::string& src);

    // process next character
    void nextChar();

    // Return the lookahead character
    char peek();

    // Invalid token found, print error message and exit
    void abort(const std::string& message);

    // Skip whitespace except newlines, which we will use to indicate the end of a statement
    void skipWhitespace();

    // skip comments in the code
    void skipComment();

    // Return the next token
    Token getToken();
};

#endif // LEXER_H