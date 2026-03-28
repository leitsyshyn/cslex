#pragma once

#include "../Keywords.h"
#include "../Position.h"
#include "../Token.h"
#include <cctype>
#include <string>

inline bool isDigitChar(char c) {
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
}

inline bool isAlphaChar(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) != 0;
}

inline bool isAlnumChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
}

inline bool isHexDigitChar(char c) {
    return std::isxdigit(static_cast<unsigned char>(c)) != 0;
}

inline bool isIdentifierStart(char c) {
    return isAlphaChar(c) || c == '_';
}

inline bool isIdentifierPart(char c) {
    return isAlnumChar(c) || c == '_';
}

inline TokenType classifyIdentifierLexeme(const std::string& lexeme) {
    if (lexeme.empty() || lexeme[0] == '@') {
        return TokenType::IDENTIFIER;
    }

    return KEYWORDS.find(lexeme) != KEYWORDS.end() ? TokenType::KEYWORD : TokenType::IDENTIFIER;
}

inline Position advancePosition(const Position& start, const std::string& lexeme) {
    Position end = start;

    for (char c : lexeme) {
        ++end.index;
        if (c == '\n') {
            ++end.line;
            end.col = 1;
        } else {
            ++end.col;
        }
    }

    return end;
}
