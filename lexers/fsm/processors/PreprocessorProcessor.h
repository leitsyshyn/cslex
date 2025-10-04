#pragma once

#include "IProcessor.h"

class PreprocessorProcessor : public IProcessor {
public:
    bool process(InputBuffer& buffer, vector<Token>& tokens) override {
        char c = buffer.peek();
        
        if (c != '#') {
            return false;
        }
        
        Position start = buffer.getCurrentPosition();
        string lexeme;
        
        while (buffer.peek() != '\n' && buffer.peek() != '\0') {
            lexeme += buffer.advance();
        }
        
        emitToken(TokenType::PREPROCESSOR, lexeme, start, buffer.getCurrentPosition(), tokens);
        return true;
    }
};
