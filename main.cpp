#include "lexer.h"
#include "parse.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

int main(int argc, char* argv[]){
    std::cout << "Tiny Compiler\n";

    if (argc != 2){
        std::cerr << "Error: Compiler needs source file as argument.\n";
        exit(1);
    }

    std::ifstream file(argv[1]);

    if (!file.is_open()){
        std::cerr << "Failed to open file " << argv[1] << std::endl;
        exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf(); // reads entire file
    std::string source = buffer.str();

    // Close the file
    file.close();

    Lexer lex{source};

    // Token token = lex.getToken();
    // while (token.kind != TokenType::T_EOF) {
    //     // why use static_cast https://stackoverflow.com/questions/475824/static-castintfoo-vs-intfoo
    //     // another hack listed in https://www.learncpp.com/cpp-tutorial/scoped-enumerations-enum-classes/ is to use +
    //     std::cout << static_cast<int>(token.kind) << '\t' << token.text << '\n';
    //     token = lex.getToken();
    // }  

    Parser parser{lex};
    parser.program(); // Start the parser.
    std::cout << "Parsing completed.\n";
}