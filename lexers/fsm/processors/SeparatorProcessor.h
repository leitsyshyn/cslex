#pragma once

#include "IProcessor.h"

class SeparatorProcessor : public IProcessor {
public:
    ProcessorResult process(InputBuffer& buffer) override {
        if (const char c = buffer.peek(); !isSeparator(c)) {
            return noMatch();
        }
        
        Position start = buffer.getCurrentPosition();
        return tokenResult(TokenType::SEPARATOR,
                           std::string(1, buffer.advance()),
                           start,
                           buffer.getCurrentPosition());
    }

private:
    static bool isSeparator(char c) {
        switch (c) {
            case '(':
            case ')':
            case '{':
            case '}':
            case '[':
            case ']':
            case ';':
            case ',':
                return true;
            default:
                return false;
        }
    }
};
