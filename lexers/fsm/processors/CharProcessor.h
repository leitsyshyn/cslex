#pragma once

#include "IProcessor.h"

class CharProcessor : public IProcessor {
public:
    bool process(InputBuffer& buffer, vector<Token>& tokens) override {
        char c = buffer.peek();
        
        if (c != '\'') {
            return false;
        }
        
        Position start = buffer.getCurrentPosition();
        string lexeme;
        lexeme += buffer.advance();
        
        while (buffer.peek() != '\'' && buffer.peek() != '\0' && buffer.peek() != '\n') {
            if (buffer.peek() == '\\') {
                lexeme += buffer.advance();
                if (buffer.peek() != '\0') {
                    lexeme += buffer.advance();
                }
            } else {
                lexeme += buffer.advance();
            }
        }
        
        if (buffer.peek() == '\'') {
            lexeme += buffer.advance();
            emitToken(TokenType::CHARACTER_LITERAL, lexeme, start, buffer.getCurrentPosition(), tokens);
        } else {
            emitToken(TokenType::ERROR, lexeme, start, buffer.getCurrentPosition(), tokens);
        }
        
        return true;
    }
};
