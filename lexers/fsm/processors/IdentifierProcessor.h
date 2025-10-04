#pragma once

#include "IProcessor.h"
#include "../../../keywords.h"
#include <cctype>

class IdentifierProcessor : public IProcessor {
public:
    bool process(InputBuffer& buffer, vector<Token>& tokens) override {
        char c = buffer.peek();
        
        bool is_verbatim = false;
        if (c == '@' && (isalpha(buffer.peek_next()) || buffer.peek_next() == '_')) {
            is_verbatim = true;
        } else if (!isalpha(c) && c != '_') {
            return false;
        }
        
        Position start = buffer.getCurrentPosition();
        string lexeme;
        
        if (is_verbatim) {
            lexeme += buffer.advance();
        }

        while (isalnum(buffer.peek()) || buffer.peek() == '_') {
            lexeme += buffer.advance();
        }
        
        if (!is_verbatim && KEYWORDS.find(lexeme) != KEYWORDS.end()) {
            emitToken(TokenType::KEYWORD, lexeme, start, buffer.getCurrentPosition(), tokens);
        } else {
            emitToken(TokenType::IDENTIFIER, lexeme, start, buffer.getCurrentPosition(), tokens);
        }
        
        return true;
    }
};
