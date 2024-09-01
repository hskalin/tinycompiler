#include "lexer.h"
#include<iostream>
#include<string>

int main(){
    std::string source {"PRINT \"How many fibonacci numbers do you want?\"\nINPUT nums\nLET a = 0\nLET b = 1\nWHILE nums > 0 REPEAT\nPRINT a\nLET c = a + b\nLET a = b\nLET b = c\nLET nums = nums - 1\nNDWHILE	\n"};
    std::cout << source;

    Lexer lex{source};

    Token token = lex.getToken();
    while (token.kind != TokenType::T_EOF) {
        // why use static_cast https://stackoverflow.com/questions/475824/static-castintfoo-vs-intfoo
        // another hack listed in https://www.learncpp.com/cpp-tutorial/scoped-enumerations-enum-classes/ is to use +
        std::cout << static_cast<int>(token.kind) << '\t' << token.text << '\n';
        token = lex.getToken();
    }   
}