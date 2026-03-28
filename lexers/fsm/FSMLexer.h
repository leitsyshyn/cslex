#pragma once

#include "../ILexer.h"
#include "InputBuffer.h"
#include "processors/IProcessor.h"
#include <memory>
#include <vector>

using namespace std;

class FSMLexer : public ILexer {
public:
    FSMLexer();
    explicit FSMLexer(std::vector<std::unique_ptr<IProcessor>> customProcessors);
    LexerResult tokenize(const std::string& source, ErrorMode errorMode = ErrorMode::Collect) override;

private:
    LexerResult tokenizeBuffer(InputBuffer& buffer, ErrorMode errorMode);
    vector<unique_ptr<IProcessor>> processors;
};
