#pragma once

#include "IProcessor.h"
#include "../../../Operators.h"

class OperatorProcessor : public IProcessor {
public:
    ProcessorResult process(InputBuffer& buffer) override {
        if (const char c = buffer.peek(); !isSingleCharOperator(c)) {
            return noMatch();
        }
        
        for (const auto& op : THREE_CHAR_OPERATORS) {
            if (buffer.peek() == op[0] && buffer.peek_next() == op[1]) {
                Position start = buffer.getCurrentPosition();
                std::string lexeme;
                lexeme += buffer.advance();
                lexeme += buffer.advance();
                if (const char third = buffer.peek(); third == op[2]) {
                    lexeme += buffer.advance();
                    return tokenResult(TokenType::OPERATOR, lexeme, start, buffer.getCurrentPosition());
                }

                return tokenResult(TokenType::OPERATOR, lexeme, start, buffer.getCurrentPosition());
            }
        }
        
        for (const auto& op : TWO_CHAR_OPERATORS) {
            if (buffer.peek() == op[0] && buffer.peek_next() == op[1]) {
                Position start = buffer.getCurrentPosition();
                std::string lexeme;
                lexeme += buffer.advance();
                lexeme += buffer.advance();
                return tokenResult(TokenType::OPERATOR, lexeme, start, buffer.getCurrentPosition());
            }
        }
        
        Position start = buffer.getCurrentPosition();
        return tokenResult(TokenType::OPERATOR,
                           std::string(1, buffer.advance()),
                           start,
                           buffer.getCurrentPosition());
    }

private:
    static bool isSingleCharOperator(char c) {
        for (char op : SINGLE_CHAR_OPERATORS) {
            if (op == c) {
                return true;
            }
        }

        return false;
    }
};
