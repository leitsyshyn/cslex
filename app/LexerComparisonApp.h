#pragma once

#include "io/ISourceRepository.h"
#include "lexers/ILexer.h"

#include <iosfwd>
#include <string>
#include <vector>

class LexerComparisonApp {
public:
    LexerComparisonApp(ISourceRepository& sourceRepository, ILexer& fsmLexer, ILexer& regexLexer);

    int run(const std::string& inputFile,
            ErrorMode errorMode,
            std::ostream& out,
            std::ostream& err);

    int run(const std::vector<std::string>& inputs,
            ErrorMode errorMode,
            std::ostream& out,
            std::ostream& err);

private:
    ISourceRepository& sourceRepository;
    ILexer& fsmLexer;
    ILexer& regexLexer;
};
