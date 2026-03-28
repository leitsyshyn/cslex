#include "InputBuffer.h"

InputBuffer::InputBuffer(const std::string& source)
    : source(source), position() {
}

char InputBuffer::peek(std::size_t offset) const {
    const std::size_t index = position.index + offset;
    if (index >= source.length()) {
        return '\0';
    }

    return source[index];
}

char InputBuffer::peek_next() const {
    return peek(1);
}

char InputBuffer::advance() {
    if (position.index >= source.length()) {
        return '\0';
    }
    
    const char current = source[position.index];
    ++position.index;
    
    if (current == '\n') {
        ++position.line;
        position.col = 1;
    } else {
        ++position.col;
    }
    
    return current;
}

bool InputBuffer::eof() const {
    return position.index >= source.length();
}

Position InputBuffer::getCurrentPosition() const {
    return position;
}
