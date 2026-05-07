#pragma once

#include "IProcessor.h"

class CharProcessor : public IProcessor {
public:
    ProcessorResult process(InputBuffer& buffer) override {
        if (const char c = buffer.peek(); c != '\'') {
            return noMatch();
        }
        
        Position start = buffer.getCurrentPosition();
        std::string lexeme;
        lexeme += buffer.advance();
        
        while (buffer.peek() != '\'' && buffer.peek() != '\0' && buffer.peek() != '\n') {
            if (buffer.peek() == '\\') {
                lexeme += buffer.advance();
                if (buffer.peek() != '\0') {
                    lexeme += buffer.advance();
                }
            } else {
                lexeme += buffer.advance();
            }
        }
        
        if (buffer.peek() == '\'') {
            lexeme += buffer.advance();
            return tokenResult(TokenType::CHARACTER_LITERAL, lexeme, start, buffer.getCurrentPosition());
        }

        return diagnosticResult(DiagnosticCode::UnterminatedCharacterLiteral,
                                lexeme,
                                start,
                                buffer.getCurrentPosition());
    }
};
