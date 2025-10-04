#pragma once

#include "../ILexer.h"
#include "InputBuffer.h"
#include "processors/IProcessor.h"
#include <vector>
#include <memory>

using namespace std;

class FSMLexer : public ILexer {
public:
    FSMLexer();
    std::vector<Token> tokenize(const std::string& source) override;

private:
    vector<Token> tokenizeBuffer(InputBuffer& buffer);
    vector<unique_ptr<IProcessor>> processors;
};
