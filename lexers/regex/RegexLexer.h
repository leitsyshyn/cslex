#pragma once

#include "../ILexer.h"
#include "../../keywords.h"
#include "RegexPatterns.h"
#include <vector>
#include <string>

using namespace std;

class RegexLexer : public ILexer {
public:
    std::vector<Token> tokenize(const std::string& source) override;
    static vector<Token> tokenizeSource(const string& source);
};