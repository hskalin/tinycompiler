#include<iostream>
#include<string>
#include<unordered_map>

// TokenType is our enum for all the types of tokens
namespace TokenType {
    enum TokenType {
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
    };
}

// Token contains the original text and the type of token.
class Token {
public:
    // choosing EOF as the default
    std::string text;
    TokenType::TokenType kind;
    static inline std::unordered_map<std::string, TokenType::TokenType> keywordMap {
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

    Token (std::string tokenText, TokenType::TokenType tokenKind)
        : text {tokenText}
        , kind {tokenKind}
    {}
};

class Lexer {
public:
    std::string source {""};
    char curChar {'\0'}; // current character in the string.
    long curPos {-1}; // Current position in the string.

    // see https://www.learncpp.com/cpp-tutorial/constructor-member-initializer-lists/ for member init syntax
    Lexer(const std::string& src)
        : source {src + '\n'}   // Source code to lex as a string. newline to simplify lexing/parsing the last token/statement.
    {
        std::cout << "lexer initialized\n";
        nextChar();
    }

    // process next character
    void nextChar(){
        curPos++;
        if ((unsigned) curPos >= source.size())
            curChar = '\0'; // EOF
        else
            curChar = source[curPos];
    }

    // Return the lookahead character
    char peek(){
        if ((unsigned) curPos+1 >= source.size())
            return '\0';
        return source[curPos+1];
    }

    // Invalid token found, print error message and exit
    void abort(const std::string& message){
        std::cerr << "Lexing Error: " << message << '\n';
        exit(1);
    }

    // Skip whitespace except newlines, which we will use to indicate the end of a statement
    void skipWhitespace(){
        while (curChar==' ' || curChar=='\t' || curChar=='\r')
            nextChar();
    }

    // skip comments in the code
    void skipComment(){
        if (curChar == '#')
            while (curChar != '\n')
                nextChar();
    }

    // Return the next token
    Token getToken(){
        Token token {"\0", TokenType::T_EOF};   // default
        skipWhitespace();
        skipComment();

        std::string curString = std::string(1,curChar);

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
};

int main(){
    std::string source {"PRINT \"How many fibonacci numbers do you want?\"\nINPUT nums\nLET a = 0\nLET b = 1\nWHILE nums > 0 REPEAT\nPRINT a\nLET c = a + b\nLET a = b\nLET b = c\nLET nums = nums - 1\nNDWHILE	"};
    Lexer lex{source};

    Token token = lex.getToken();
    while (token.kind != TokenType::T_EOF) {
        std::cout << token.kind << '\t' << token.text << '\n';
        token = lex.getToken();
    }   
}