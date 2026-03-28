#pragma once

#include "../ILexer.h"
#include "../LexicalRules.h"
#include "RegexPatterns.h"
#include <string>
#include <vector>

using namespace std;

class RegexLexer : public ILexer {
public:
    LexerResult tokenize(const std::string& source, ErrorMode errorMode = ErrorMode::Collect) override;
    static LexerResult tokenizeSource(const string& source, ErrorMode errorMode = ErrorMode::Collect);
};
