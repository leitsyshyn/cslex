#pragma once

#include "IProcessor.h"
#include "../../../Operators.h"

class OperatorProcessor : public IProcessor {
public:
    ProcessorResult process(InputBuffer& buffer) override {
        char c = buffer.peek();
        
        if (SINGLE_CHAR_OPERATORS.find(c) == string::npos) {
            return noMatch();
        }
        
        for (const auto& op : THREE_CHAR_OPERATORS) {
            if (buffer.peek() == op[0] && buffer.peek_next() == op[1]) {
                Position start = buffer.getCurrentPosition();
                string lexeme;
                lexeme += buffer.advance();
                lexeme += buffer.advance();
                char third = buffer.peek();
                if (third == op[2]) {
                    lexeme += buffer.advance();
                    return tokenResult(TokenType::OPERATOR, lexeme, start, buffer.getCurrentPosition());
                }

                return tokenResult(TokenType::OPERATOR, lexeme, start, buffer.getCurrentPosition());
            }
        }
        
        for (const auto& op : TWO_CHAR_OPERATORS) {
            if (buffer.peek() == op[0] && buffer.peek_next() == op[1]) {
                Position start = buffer.getCurrentPosition();
                string lexeme;
                lexeme += buffer.advance();
                lexeme += buffer.advance();
                return tokenResult(TokenType::OPERATOR, lexeme, start, buffer.getCurrentPosition());
            }
        }
        
        Position start = buffer.getCurrentPosition();
        return tokenResult(TokenType::OPERATOR,
                           string(1, buffer.advance()),
                           start,
                           buffer.getCurrentPosition());
    }
};
