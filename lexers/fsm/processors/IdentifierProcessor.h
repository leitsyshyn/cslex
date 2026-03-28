#pragma once

#include "IProcessor.h"
#include "../../LexicalRules.h"

class IdentifierProcessor : public IProcessor {
public:
    ProcessorResult process(InputBuffer& buffer) override {
        char c = buffer.peek();
        
        bool is_verbatim = false;
        if (c == '@' && isIdentifierStart(buffer.peek_next())) {
            is_verbatim = true;
        } else if (!isIdentifierStart(c)) {
            return noMatch();
        }
        
        Position start = buffer.getCurrentPosition();
        string lexeme;
        
        if (is_verbatim) {
            lexeme += buffer.advance();
        }

        while (isIdentifierPart(buffer.peek())) {
            lexeme += buffer.advance();
        }

        return tokenResult(classifyIdentifierLexeme(lexeme),
                           lexeme,
                           start,
                           buffer.getCurrentPosition());
    }
};
