#pragma once

#include "../ILexer.h"
#include "../LexicalRules.h"
#include "RegexPatterns.h"
#include <string>
#include <vector>

class RegexLexer : public ILexer {
public:
    LexerResult tokenize(const std::string& source, ErrorMode errorMode = ErrorMode::Collect) override;
    static LexerResult tokenizeSource(const std::string& source, ErrorMode errorMode = ErrorMode::Collect);
};
