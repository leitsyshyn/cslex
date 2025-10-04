#pragma once

#include "IProcessor.h"
#include "../../../Operators.h"

class OperatorProcessor : public IProcessor {
public:
    bool process(InputBuffer& buffer, vector<Token>& tokens) override {
        char c = buffer.peek();
        
        if (SINGLE_CHAR_OPERATORS.find(c) == string::npos) {
            return false;
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
                    emitToken(TokenType::OPERATOR, lexeme, start, buffer.getCurrentPosition(), tokens);
                    return true;
                } else {
                    emitToken(TokenType::OPERATOR, lexeme, start, buffer.getCurrentPosition(), tokens);
                    return true;
                }
            }
        }
        
        for (const auto& op : TWO_CHAR_OPERATORS) {
            if (buffer.peek() == op[0] && buffer.peek_next() == op[1]) {
                Position start = buffer.getCurrentPosition();
                string lexeme;
                lexeme += buffer.advance();
                lexeme += buffer.advance();
                emitToken(TokenType::OPERATOR, lexeme, start, buffer.getCurrentPosition(), tokens);
                return true;
            }
        }
        
        Position start = buffer.getCurrentPosition();
        emitToken(TokenType::OPERATOR, string(1, buffer.advance()), start, buffer.getCurrentPosition(), tokens);
        return true;
    }
};
