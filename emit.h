#ifndef EMIT_H
#define EMIT_H

#include<string>

// Emitter object keeps track of the generated code and outputs it.
class Emitter{
public:
    std::string fullpath;
    std::string header;
    std::string code;

    Emitter(const std::string& fullpath);

    void emit(std::string& code);

    void emitLine(std::string& code);

    void headerLine(std::string& code);

    void writeFile();
};

#endif // EMIT_H