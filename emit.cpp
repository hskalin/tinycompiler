#include "emit.h"
#include<string>
#include<fstream>

Emitter::Emitter(const std::string& fullpath)
    : fullpath {fullpath}
    , header {""}
    , code {""}
    {}

void Emitter::emit(const std::string code){
    // this is like self in python
    this->code += code;
}

void Emitter::emitLine(const std::string code){
    this->code += code + '\n';
}
    
void Emitter::headerLine(const std::string code){
    header += code + '\n';
}
    
void Emitter::writeFile(){
    std::ofstream outputFile;
    outputFile.open(fullpath);
    outputFile << header + code;
    outputFile.close();
}