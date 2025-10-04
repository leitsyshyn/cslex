#pragma once

#include <string>
#include "../../Position.h"

using namespace std;

class InputBuffer {
public:
    explicit InputBuffer(const string& source);
    
    char peek() const;
    char peek_next() const;
    char advance();
    bool eof() const;
    
    Position getCurrentPosition() const;

private:
    string source;
    Position position;
};
