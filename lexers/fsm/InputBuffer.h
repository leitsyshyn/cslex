#pragma once

#include "../../Position.h"
#include <cstddef>
#include <string>

class InputBuffer {
public:
    explicit InputBuffer(const std::string& source);
    
    char peek(std::size_t offset = 0) const;
    char peek_next() const;
    char advance();
    bool eof() const;
    
    Position getCurrentPosition() const;

private:
    std::string source;
    Position position;
};
