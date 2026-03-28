#pragma once

#include "IProcessor.h"

class VerbatimStringProcessor : public IProcessor {
public:
    ProcessorResult process(InputBuffer& buffer) override {
        char c = buffer.peek();
        
        if (c != '@' || buffer.peek_next() != '"') {
            return noMatch();
        }
        
        Position start = buffer.getCurrentPosition();
        string lexeme;
        bool terminated = false;
        lexeme += buffer.advance();
        lexeme += buffer.advance(); 
        
        while (buffer.peek() != '\0') {
            if (buffer.peek() == '"') {
                lexeme += buffer.advance();
                if (buffer.peek() == '"') {
                    lexeme += buffer.advance();
                } else {
                    terminated = true;
                    break;
                }
            } else {
                lexeme += buffer.advance();
            }
        }
        
        if (terminated) {
            return tokenResult(TokenType::VERBATIM_STRING, lexeme, start, buffer.getCurrentPosition());
        }

        return diagnosticResult(DiagnosticCode::UnterminatedVerbatimStringLiteral,
                                lexeme,
                                start,
                                buffer.getCurrentPosition());
    }
};
