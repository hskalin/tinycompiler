#include "emit.h"
#include<string>
#include<fstream>

Emitter::Emitter(const std::string& fullpath)
    : fullpath {fullpath}
    , header {""}
    , code {""}
    {}

void Emitter::emit(std::string& code){
    code += code;
}

void Emitter::emitLine(std::string& code){
    code += code + '\n';
}
    
void Emitter::headerLine(std::string& code){
    header += code + '\n';
}
    
void Emitter::writeFile(){
    std::ofstream outputFile;
    outputFile.open(fullpath);
    outputFile << header + code;
    outputFile.close();
}