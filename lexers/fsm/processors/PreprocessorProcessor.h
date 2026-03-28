#pragma once

#include "IProcessor.h"

class PreprocessorProcessor : public IProcessor {
public:
    ProcessorResult process(InputBuffer& buffer) override {
        char c = buffer.peek();
        
        if (c != '#') {
            return noMatch();
        }
        
        Position start = buffer.getCurrentPosition();
        string lexeme;
        
        while (buffer.peek() != '\n' && buffer.peek() != '\0') {
            lexeme += buffer.advance();
        }
        
        return tokenResult(TokenType::PREPROCESSOR, lexeme, start, buffer.getCurrentPosition());
    }
};
