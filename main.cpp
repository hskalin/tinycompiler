#include "lexer.h"
#include "parse.h"
#include "emit.h"
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

    Lexer lexer{source};
    Emitter emitter{"out.c"};
    Parser parser{lexer, emitter};

    parser.program(); // Start the parser.
    emitter.writeFile();
    std::cout << "Compiling completed.\n";
}