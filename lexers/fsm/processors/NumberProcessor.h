#pragma once

#include "IProcessor.h"
#include "../../LexicalRules.h"

class NumberProcessor : public IProcessor {
public:
    ProcessorResult process(InputBuffer& buffer) override {
        char c = buffer.peek();
        
        if (!isDigitChar(c)) {
            return noMatch();
        }
        
        Position start = buffer.getCurrentPosition();
        string lexeme;
        bool is_real = false;
        
        if (buffer.peek() == '0' && (buffer.peek_next() == 'b' || buffer.peek_next() == 'B')) {
            lexeme += buffer.advance();
            lexeme += buffer.advance();
            
            while (buffer.peek() == '0' || buffer.peek() == '1' || buffer.peek() == '_') {
                lexeme += buffer.advance();
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
            
            if (isAlphaChar(buffer.peek()) || buffer.peek() == '_') {
                while (isAlnumChar(buffer.peek()) || buffer.peek() == '_') {
                    lexeme += buffer.advance();
                }
                return diagnosticResult(DiagnosticCode::InvalidNumericLiteral,
                                        lexeme,
                                        start,
                                        buffer.getCurrentPosition());
            }
            
            return tokenResult(TokenType::INTEGER_LITERAL, lexeme, start, buffer.getCurrentPosition());
        }
        
        if (buffer.peek() == '0' && (buffer.peek_next() == 'x' || buffer.peek_next() == 'X')) {
            lexeme += buffer.advance();
            lexeme += buffer.advance();
            
            while (isHexDigitChar(buffer.peek()) || buffer.peek() == '_') {
                lexeme += buffer.advance();
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
            
            if (isAlphaChar(buffer.peek()) || buffer.peek() == '_') {
                while (isAlnumChar(buffer.peek()) || buffer.peek() == '_') {
                    lexeme += buffer.advance();
                }
                return diagnosticResult(DiagnosticCode::InvalidNumericLiteral,
                                        lexeme,
                                        start,
                                        buffer.getCurrentPosition());
            }
            
            return tokenResult(TokenType::INTEGER_LITERAL, lexeme, start, buffer.getCurrentPosition());
        }
        
        while (isDigitChar(buffer.peek()) || buffer.peek() == '_') {
            lexeme += buffer.advance();
        }
        
        if (buffer.peek() == '.') {
            is_real = true;
            lexeme += buffer.advance();
            
            while (isDigitChar(buffer.peek()) || buffer.peek() == '_') {
                lexeme += buffer.advance();
            }
        }
        
        if (buffer.peek() == 'e' || buffer.peek() == 'E') {
            is_real = true;
            lexeme += buffer.advance();
            
            if (buffer.peek() == '+' || buffer.peek() == '-') {
                lexeme += buffer.advance();
            }
            
            while (isDigitChar(buffer.peek()) || buffer.peek() == '_') {
                lexeme += buffer.advance();
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
        
        if (isAlphaChar(buffer.peek()) || buffer.peek() == '_') {
            while (isAlnumChar(buffer.peek()) || buffer.peek() == '_') {
                lexeme += buffer.advance();
            }
            return diagnosticResult(DiagnosticCode::InvalidNumericLiteral,
                                    lexeme,
                                    start,
                                    buffer.getCurrentPosition());
        }
        
        return tokenResult(is_real ? TokenType::REAL_LITERAL : TokenType::INTEGER_LITERAL,
                           lexeme,
                           start,
                           buffer.getCurrentPosition());
    }
};
