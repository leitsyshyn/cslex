#pragma once

#include "../Token.h"
#include <vector>
#include <string>

class ILexer {
public:
    virtual ~ILexer() = default;
    virtual std::vector<Token> tokenize(const std::string& source) = 0;
};
