#pragma once

#include "IProcessor.h"
#include <cctype>

class NumberProcessor : public IProcessor {
public:
    bool process(InputBuffer& buffer, vector<Token>& tokens) override {
        char c = buffer.peek();
        
        if (!isdigit(c)) {
            return false;
        }
        
        Position start = buffer.getCurrentPosition();
        string lexeme;
        bool is_real = false;
        
        if (buffer.peek() == '0' && (buffer.peek_next() == 'b' || buffer.peek_next() == 'B')) {
            lexeme += buffer.advance();
            lexeme += buffer.advance();
            
            while (buffer.peek() == '0' || buffer.peek() == '1' || buffer.peek() == '_') {
                if (buffer.peek() != '_') {
                    lexeme += buffer.advance();
                } else {
                    buffer.advance(); 
                }
            }
            
            if (buffer.peek() == 'u' || buffer.peek() == 'U') {
                lexeme += buffer.advance();
                if (buffer.peek() == 'l' || buffer.peek() == 'L') {
                    lexeme += buffer.advance();
                }
            } else if (buffer.peek() == 'l' || buffer.peek() == 'L') {
                lexeme += buffer.advance();
                if (buffer.peek() == 'u' || buffer.peek() == 'U') {
                    lexeme += buffer.advance();
                }
            }
            
            if (isalpha(buffer.peek()) || buffer.peek() == '_') {
                while (isalnum(buffer.peek()) || buffer.peek() == '_') {
                    lexeme += buffer.advance();
                }
                emitToken(TokenType::ERROR, lexeme, start, buffer.getCurrentPosition(), tokens);
                return true;
            }
            
            emitToken(TokenType::INTEGER_LITERAL, lexeme, start, buffer.getCurrentPosition(), tokens);
            return true;
        }
        
        if (buffer.peek() == '0' && (buffer.peek_next() == 'x' || buffer.peek_next() == 'X')) {
            lexeme += buffer.advance();
            lexeme += buffer.advance();
            
            while (isxdigit(buffer.peek()) || buffer.peek() == '_') {
                if (buffer.peek() != '_') {
                    lexeme += buffer.advance();
                } else {
                    buffer.advance();
                }
            }
            
            if (buffer.peek() == 'u' || buffer.peek() == 'U') {
                lexeme += buffer.advance();
                if (buffer.peek() == 'l' || buffer.peek() == 'L') {
                    lexeme += buffer.advance();
                }
            } else if (buffer.peek() == 'l' || buffer.peek() == 'L') {
                lexeme += buffer.advance();
                if (buffer.peek() == 'u' || buffer.peek() == 'U') {
                    lexeme += buffer.advance();
                }
            }
            
            if (isalpha(buffer.peek()) || buffer.peek() == '_') {
                while (isalnum(buffer.peek()) || buffer.peek() == '_') {
                    lexeme += buffer.advance();
                }
                emitToken(TokenType::ERROR, lexeme, start, buffer.getCurrentPosition(), tokens);
                return true;
            }
            
            emitToken(TokenType::INTEGER_LITERAL, lexeme, start, buffer.getCurrentPosition(), tokens);
            return true;
        }
        
        while (isdigit(buffer.peek()) || buffer.peek() == '_') {
            if (buffer.peek() != '_') {
                lexeme += buffer.advance();
            } else {
                buffer.advance();
            }
        }
        
        if (buffer.peek() == '.') {
            is_real = true;
            lexeme += buffer.advance();
            
            while (isdigit(buffer.peek()) || buffer.peek() == '_') {
                if (buffer.peek() != '_') {
                    lexeme += buffer.advance();
                } else {
                    buffer.advance();
                }
            }
        }
        
        if (buffer.peek() == 'e' || buffer.peek() == 'E') {
            is_real = true;
            lexeme += buffer.advance();
            
            if (buffer.peek() == '+' || buffer.peek() == '-') {
                lexeme += buffer.advance();
            }
            
            while (isdigit(buffer.peek()) || buffer.peek() == '_') {
                if (buffer.peek() != '_') {
                    lexeme += buffer.advance();
                } else {
                    buffer.advance();
                }
            }
        }
        
        if (buffer.peek() == 'f' || buffer.peek() == 'F' || buffer.peek() == 'd' || buffer.peek() == 'D' || 
            buffer.peek() == 'm' || buffer.peek() == 'M') {
            is_real = true;
            lexeme += buffer.advance();
        } else if (buffer.peek() == 'u' || buffer.peek() == 'U') {
            lexeme += buffer.advance();
            if (buffer.peek() == 'l' || buffer.peek() == 'L') {
                lexeme += buffer.advance();
            }
        } else if (buffer.peek() == 'l' || buffer.peek() == 'L') {
            lexeme += buffer.advance();
            if (buffer.peek() == 'u' || buffer.peek() == 'U') {
                lexeme += buffer.advance();
            }
        }
        
        if (isalpha(buffer.peek()) || buffer.peek() == '_') {
            while (isalnum(buffer.peek()) || buffer.peek() == '_') {
                lexeme += buffer.advance();
            }
            emitToken(TokenType::ERROR, lexeme, start, buffer.getCurrentPosition(), tokens);
            return true;
        }
        
        emitToken(is_real ? TokenType::REAL_LITERAL : TokenType::INTEGER_LITERAL, lexeme, start, buffer.getCurrentPosition(), tokens);
        return true;
    }
};
