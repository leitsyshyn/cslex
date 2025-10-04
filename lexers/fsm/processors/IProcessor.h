#pragma once

#include "../InputBuffer.h"
#include "../../../Token.h"
#include "../../../Position.h"
#include <vector>
#include <string>

using namespace std;

class IProcessor {
public:
    virtual ~IProcessor() = default;
    virtual bool process(InputBuffer& buffer, vector<Token>& tokens) = 0;

protected:
    void emitToken(TokenType type, const string& lexeme, const Position& start, 
                   const Position& end, vector<Token>& tokens) {
        tokens.push_back(Token(type, lexeme, start, end));
    }
};
