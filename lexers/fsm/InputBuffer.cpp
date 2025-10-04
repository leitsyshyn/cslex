#include "InputBuffer.h"

InputBuffer::InputBuffer(const string& source)
    : source(source), position() {
}

char InputBuffer::peek() const {
    if (position.index >= source.length()) return '\0';
    return source[position.index];
}

char InputBuffer::peek_next() const {
    if (position.index + 1 >= source.length()) return '\0';
    return source[position.index + 1];
}

char InputBuffer::advance() {
    if (position.index >= source.length()) return '\0';
    
    const char current = source[position.index];
    position.index++;
    
    if (current == '\n') {
        position.line++;
        position.col = 1;
    } else {
        position.col++;
    }
    
    return current;
}

bool InputBuffer::eof() const {
    return position.index >= source.length();
}

Position InputBuffer::getCurrentPosition() const {
    return position;
}
