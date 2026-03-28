#pragma once

#include "IProcessor.h"

class StringProcessor : public IProcessor {
public:
    ProcessorResult process(InputBuffer& buffer) override {
        char c = buffer.peek();
        
        if (c != '"') {
            return noMatch();
        }
        
        Position start = buffer.getCurrentPosition();
        string lexeme;
        char quote_type = c;
        lexeme += buffer.advance();
        
        while (buffer.peek() != quote_type && buffer.peek() != '\0' && buffer.peek() != '\n') {
            if (buffer.peek() == '\\') {
                lexeme += buffer.advance();
                if (buffer.peek() != '\0') {
                    lexeme += buffer.advance();
                }
            } else {
                lexeme += buffer.advance();
            }
        }
        
        if (buffer.peek() == quote_type) {
            lexeme += buffer.advance();
            return tokenResult(TokenType::STRING_LITERAL, lexeme, start, buffer.getCurrentPosition());
        }

        return diagnosticResult(DiagnosticCode::UnterminatedStringLiteral,
                                lexeme,
                                start,
                                buffer.getCurrentPosition());
    }
};
