#pragma once

#include "IProcessor.h"

class SeparatorProcessor : public IProcessor {
public:
    ProcessorResult process(InputBuffer& buffer) override {
        char c = buffer.peek();
        
        const string separators = "(){}[];,";
        
        if (separators.find(c) == string::npos) {
            return noMatch();
        }
        
        Position start = buffer.getCurrentPosition();
        return tokenResult(TokenType::SEPARATOR,
                           string(1, buffer.advance()),
                           start,
                           buffer.getCurrentPosition());
    }
};
