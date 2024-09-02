#include "parse.h"
#include "lexer.h"
#include "emit.h"
#include<string>
#include<iostream>
#include<sstream>


const std::string getTokenName(TokenType kind){
    // TODO return actual names
    // why use static_cast https://stackoverflow.com/questions/475824/static-castintfoo-vs-intfoo
    // another hack listed in https://www.learncpp.com/cpp-tutorial/scoped-enumerations-enum-classes/ is to use +
    return std::to_string(static_cast<int>(kind));
}

Parser::Parser(Lexer& lex, Emitter& emit)
    : lexer {lex}
    , emitter {emit}
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
        abort("Expected kind " + getTokenName(kind) + ", got " + getTokenName(curToken.kind));
    }
    nextToken();
}

void Parser::nextToken(){
    curToken = peekToken;
    peekToken = lexer.getToken();
}

void Parser::program(){
    emitter.headerLine("#include <stdio.h>");
    emitter.headerLine("int main(void){");

    // Since some newlines are required in our grammar, need to skip the excess.
    while (checkToken(TokenType::NEWLINE))
        nextToken();

    // parses all the statements in the program
    while (!checkToken(TokenType::T_EOF)){
        statement();
    }

    // Wrap things up.
    emitter.emitLine("return 0;");
    emitter.emitLine("}");

    // check that all labels referenced by a GOTO is declared
    for (const auto& label : labelsGotoed){
        if (labelsDeclared.find(label) == labelsDeclared.end())
            abort("Attempting to GOTO to undeclared label: " + label);
    }
}

void Parser::nl(){
    // Require at least one newline.
    match(TokenType::NEWLINE);

    // But we will allow extra newlines too, of course.
    while (checkToken(TokenType::NEWLINE))
        nextToken();
}

void Parser::primary(){   
    if (checkToken(TokenType::NUMBER)){
        emitter.emit(curToken.text);
        nextToken();
    }
    else if (checkToken(TokenType::IDENT)){
        // Ensure the variable already exists
        if (symbols.find(curToken.text) == symbols.end())
            abort("Referencing variable before assignment: " + curToken.text);
        emitter.emit(curToken.text);
        nextToken();
    }
    else
        abort("Unexpected token at " + curToken.text);
}

void Parser::unary(){
    // may or may not have a +/- in front
    if (checkToken(TokenType::PLUS) || checkToken(TokenType::MINUS)){
        emitter.emit(curToken.text);
        nextToken();
    }
    primary();
}

void Parser::term(){
    unary();

    // can have zero or more *// unary
    while (checkToken(TokenType::ASTERISK) || checkToken(TokenType::SLASH)){
        emitter.emit(curToken.text);
        nextToken();
        unary();
    }
}

void Parser::expression(){
    term();

    // can have zero or more +/- term
    while (checkToken(TokenType::PLUS) || checkToken(TokenType::MINUS)){
        emitter.emit(curToken.text);
        nextToken();
        term();
    }
}

bool Parser::isComparisonOperator(){
    return (
        checkToken(TokenType::GT) || 
        checkToken(TokenType::LT) ||
        checkToken(TokenType::GTEQ) ||
        checkToken(TokenType::LTEQ) ||
        checkToken(TokenType::EQEQ) ||
        checkToken(TokenType::NOTEQ)
        );
}

void Parser::comparison(){
    expression();

    // there must be at lest one comparison
    if (isComparisonOperator()){
        emitter.emit(curToken.text);
        nextToken();
        expression();
    }
    else{
        abort("Expected comparison operator at: " + curToken.text);
    }

    // there can be more comparison - expression pairs
    while (isComparisonOperator()){
        emitter.emit(curToken.text);
        nextToken();
        expression();
    }
}

void Parser::statement(){
    // check the first token to find what kind of statement we have to parse

    // "PRINT" (expression | string)
    if (checkToken(TokenType::PRINT)){
        nextToken();
        if (checkToken(TokenType::STRING)){
            // found simple string
            emitter.emitLine("printf(\"" + curToken.text + "\\n\");");
            nextToken();
        }
        else{
            // else expecting an expression, print as float
            emitter.emit("printf(\"\%.2f\\n\", (float)(");
            expression();
            emitter.emitLine("));");
        }
    }
    // "IF" comparison "THEN" nl {statement} "ENDIF"
    else if (checkToken(TokenType::IF)){
        nextToken();
        emitter.emit("if(");
        comparison();

        //expect THEN
        match(TokenType::THEN);
        nl();
        emitter.emitLine("){");

        // zero or more statements
        while (!checkToken(TokenType::ENDIF))
            statement();

        // it is already a given that curToken will be ENDIF but this is just formal
        // and moves to the next token
        match(TokenType::ENDIF);
        emitter.emitLine("}");
    }
    // "WHILE" comparison "REPEAT" nl {statement} "ENDWHILE"
    else if (checkToken(TokenType::WHILE)){
        nextToken();
        emitter.emit("while(");
        comparison();

        match(TokenType::REPEAT);
        nl();
        emitter.emitLine("){");

        while (!checkToken(TokenType::ENDWHILE))
            statement();

        match(TokenType::ENDWHILE);
        emitter.emitLine("}");
    }
    // "LABEL" ident 
    else if (checkToken(TokenType::LABEL)){
        nextToken();

        // make sure its not already declared
        if (labelsDeclared.find(curToken.text) != labelsDeclared.end())
            abort("Label already exists: " + curToken.text);
        labelsDeclared.insert(curToken.text);

        emitter.emitLine(curToken.text + ":");
        match(TokenType::IDENT);
    }
    // "GOTO" ident
    else if (checkToken(TokenType::GOTO)){
        nextToken();
        labelsGotoed.insert(curToken.text);
        emitter.emitLine("goto " + curToken.text + ";");
        match(TokenType::IDENT);
    }    
    // "LET" ident "=" expression
    else if (checkToken(TokenType::LET)){
        nextToken();

        // Check if ident exists in symbol table. If not, declare it.
        if (symbols.find(curToken.text) == symbols.end()){
            symbols.insert(curToken.text);
            emitter.headerLine("float " + curToken.text + ";");
        }

        emitter.emit(curToken.text + " = ");
        match(TokenType::IDENT);
        match(TokenType::EQ);
        expression();
        emitter.emitLine(";");
    }
    // "INPUT" ident
    else if (checkToken(TokenType::INPUT)){
        nextToken();

        // If variable doesn't already exist, declare it
        if (symbols.find(curToken.text) == symbols.end()){
            symbols.insert(curToken.text);
            emitter.headerLine("float " + curToken.text + ";");
        }

        // Emit scanf but also validate the input. If invalid, set the variable to 0 and clear the input.
        emitter.emitLine("if(0 == scanf(\"\%f\", &" + curToken.text + ")) {");
        emitter.emitLine(curToken.text + " = 0;");
        emitter.emitLine("scanf(\"\%*s\");");
        emitter.emitLine("}");
        match(TokenType::IDENT);
    }  
    else{
        abort("Invalid statement at " + curToken.text + " (" + getTokenName(curToken.kind) + ")");
    }

    // newline is common for all
    nl();
}