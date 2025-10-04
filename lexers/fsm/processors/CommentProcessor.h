#pragma once

#include "IProcessor.h"

class CommentProcessor : public IProcessor {
public:
    bool process(InputBuffer& buffer, vector<Token>& tokens) override {
        char c = buffer.peek();
        
        if (c != '/') {
            return false;
        }
        
        if (buffer.peek_next() == '/') {
            Position start = buffer.getCurrentPosition();
            string lexeme;
            while (buffer.peek() != '\n' && buffer.peek() != '\0') {
                lexeme += buffer.advance();
            }
            emitToken(TokenType::COMMENT, lexeme, start, buffer.getCurrentPosition(), tokens);
            return true;
        }
        
        if (buffer.peek_next() == '*') {
            Position start = buffer.getCurrentPosition();
            string lexeme;
            lexeme += buffer.advance(); 
            lexeme += buffer.advance(); 
            
            while (!(buffer.peek() == '*' && buffer.peek_next() == '/') && buffer.peek() != '\0') {
                lexeme += buffer.advance();
            }
            
            if (buffer.peek() == '*' && buffer.peek_next() == '/') {
                lexeme += buffer.advance(); 
                lexeme += buffer.advance(); 
                emitToken(TokenType::COMMENT, lexeme, start, buffer.getCurrentPosition(), tokens);
            } else {
                emitToken(TokenType::ERROR, lexeme, start, buffer.getCurrentPosition(), tokens);
            }
            
            return true;
        }
        
        return false;
    }
};
