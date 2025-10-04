#pragma once

#include "IProcessor.h"

class SeparatorProcessor : public IProcessor {
public:
    bool process(InputBuffer& buffer, vector<Token>& tokens) override {
        char c = buffer.peek();
        
        const string separators = "(){}[];,";
        
        if (separators.find(c) == string::npos) {
            return false;
        }
        
        Position start = buffer.getCurrentPosition();
        emitToken(TokenType::SEPARATOR, string(1, buffer.advance()), start, buffer.getCurrentPosition(), tokens);
        return true;
    }
};
