#pragma once

#include "IProcessor.h"

class InterpolatedStringProcessor : public IProcessor {
public:
    bool process(InputBuffer& buffer, vector<Token>& tokens) override {
        char c = buffer.peek();
        
        if (c != '$' || (buffer.peek_next() != '"' && buffer.peek_next() != '@')) {
            return false;
        }
        
        Position start = buffer.getCurrentPosition();
        string lexeme;
        lexeme += buffer.advance();
        
        bool is_verbatim = false;
        if (buffer.peek() == '@') {
            lexeme += buffer.advance();
            is_verbatim = true;
        }
        
        if (buffer.peek() == '"') {
            lexeme += buffer.advance();
            
            int brace_depth = 0;
            while (buffer.peek() != '\0') {
                if (buffer.peek() == '{') {
                    lexeme += buffer.advance();
                    if (buffer.peek() == '{') {
                        lexeme += buffer.advance();
                    } else {
                        brace_depth++;
                    }
                } else if (buffer.peek() == '}') {
                    lexeme += buffer.advance();
                    if (buffer.peek() == '}' && brace_depth == 0) {
                        lexeme += buffer.advance();
                    } else if (brace_depth > 0) {
                        brace_depth--;
                    }
                } else if (buffer.peek() == '"' && brace_depth == 0) {
                    if (is_verbatim && buffer.peek_next() == '"') {
                        lexeme += buffer.advance();
                        lexeme += buffer.advance(); 
                    } else {
                        lexeme += buffer.advance();
                        break;
                    }
                } else if (buffer.peek() == '\\' && !is_verbatim) {
                    lexeme += buffer.advance();
                    if (buffer.peek() != '\0') {
                        lexeme += buffer.advance();
                    }
                } else {
                    lexeme += buffer.advance();
                }
            }
        }
        
        emitToken(TokenType::INTERPOLATED_STRING, lexeme, start, buffer.getCurrentPosition(), tokens);
        return true;
    }
};
