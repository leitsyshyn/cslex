#pragma once

#include "../Token.h"
#include "Diagnostic.h"
#include <vector>

enum class ErrorMode {
    Collect,
    Throw
};

struct LexerResult {
    std::vector<Token> tokens;
    std::vector<Diagnostic> diagnostics;
};
