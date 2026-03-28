#pragma once

#include "LexerResult.h"
#include <string>

class ILexer {
public:
    virtual ~ILexer() = default;
    virtual LexerResult tokenize(const std::string& source, ErrorMode errorMode = ErrorMode::Collect) = 0;

    LexerResult tokenizeStrict(const std::string& source) {
        return tokenize(source, ErrorMode::Throw);
    }

    LexerResult tokenizeTolerant(const std::string& source) {
        return tokenize(source, ErrorMode::Collect);
    }
};
