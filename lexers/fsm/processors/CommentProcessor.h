#pragma once

#include "IProcessor.h"

class CommentProcessor : public IProcessor {
public:
    ProcessorResult process(InputBuffer& buffer) override {
        if (const char c = buffer.peek(); c != '/') {
            return noMatch();
        }
        
        if (buffer.peek_next() == '/') {
            Position start = buffer.getCurrentPosition();
            std::string lexeme;
            while (buffer.peek() != '\n' && buffer.peek() != '\0') {
                lexeme += buffer.advance();
            }
            return tokenResult(TokenType::COMMENT, lexeme, start, buffer.getCurrentPosition());
        }
        
        if (buffer.peek_next() == '*') {
            Position start = buffer.getCurrentPosition();
            std::string lexeme;
            lexeme += buffer.advance(); 
            lexeme += buffer.advance(); 
            
            while (!(buffer.peek() == '*' && buffer.peek_next() == '/') && buffer.peek() != '\0') {
                lexeme += buffer.advance();
            }
            
            if (buffer.peek() == '*' && buffer.peek_next() == '/') {
                lexeme += buffer.advance(); 
                lexeme += buffer.advance(); 
                return tokenResult(TokenType::COMMENT, lexeme, start, buffer.getCurrentPosition());
            }

            return diagnosticResult(DiagnosticCode::UnterminatedComment,
                                    lexeme,
                                    start,
                                    buffer.getCurrentPosition());
        }
        
        return noMatch();
    }
};
