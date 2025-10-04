#pragma once

#include "IProcessor.h"

class VerbatimStringProcessor : public IProcessor {
public:
    bool process(InputBuffer& buffer, vector<Token>& tokens) override {
        char c = buffer.peek();
        
        if (c != '@' || buffer.peek_next() != '"') {
            return false;
        }
        
        Position start = buffer.getCurrentPosition();
        string lexeme;
        lexeme += buffer.advance();
        lexeme += buffer.advance(); 
        
        while (buffer.peek() != '\0') {
            if (buffer.peek() == '"') {
                lexeme += buffer.advance();
                if (buffer.peek() == '"') {
                    lexeme += buffer.advance();
                } else {
                    break; 
                }
            } else {
                lexeme += buffer.advance();
            }
        }
        
        emitToken(TokenType::VERBATIM_STRING, lexeme, start, buffer.getCurrentPosition(), tokens);
        return true;
    }
};
