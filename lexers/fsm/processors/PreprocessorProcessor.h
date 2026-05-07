#pragma once

#include "IProcessor.h"

class PreprocessorProcessor : public IProcessor {
public:
    ProcessorResult process(InputBuffer& buffer) override {
        if (const char c = buffer.peek(); c != '#') {
            return noMatch();
        }
        
        Position start = buffer.getCurrentPosition();
        std::string lexeme;
        
        while (buffer.peek() != '\n' && buffer.peek() != '\0') {
            lexeme += buffer.advance();
        }
        
        return tokenResult(TokenType::PREPROCESSOR, lexeme, start, buffer.getCurrentPosition());
    }
};
