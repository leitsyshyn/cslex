#pragma once

#include "Position.h"
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

enum class TokenType {
    INTEGER_LITERAL,      // Decimal, hex, or binary integer
    REAL_LITERAL,         // Float, double, or decimal
    CHARACTER_LITERAL,    // 'c'
    STRING_LITERAL,       // "string"
    VERBATIM_STRING,      // @"verbatim"
    INTERPOLATED_STRING,  // $"interpolated"
    KEYWORD,              // Reserved keywords
    IDENTIFIER,           // Names
    OPERATOR,             // +, -, *, /, etc.
    SEPARATOR,            // { } ( ) [ ] , ; :
    COMMENT,              // // or /* */
    PREPROCESSOR,         // #define, #if, etc.
    WHITESPACE,           // Spaces, tabs, newlines
    ERROR,                // Lexical errors
    END_OF_FILE           // EOF marker
};

struct Token {
    TokenType type;      
    string lexeme;      
    Position start;       
    Position end;  

    Token() : type(TokenType::ERROR) {}

    Token(TokenType t, const string& lex, const Position& s, const Position& e)
        : type(t), lexeme(lex), start(s), end(e) {}

    string tokenTypeToString() const {
        switch (type) {
            case TokenType::INTEGER_LITERAL: return "INTEGER_LITERAL";
            case TokenType::REAL_LITERAL: return "REAL_LITERAL";
            case TokenType::CHARACTER_LITERAL: return "CHARACTER_LITERAL";
            case TokenType::STRING_LITERAL: return "STRING_LITERAL";
            case TokenType::VERBATIM_STRING: return "VERBATIM_STRING";
            case TokenType::INTERPOLATED_STRING: return "INTERPOLATED_STRING";
            case TokenType::KEYWORD: return "KEYWORD";
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::OPERATOR: return "OPERATOR";
            case TokenType::SEPARATOR: return "SEPARATOR";
            case TokenType::COMMENT: return "COMMENT";
            case TokenType::PREPROCESSOR: return "PREPROCESSOR";
            case TokenType::WHITESPACE: return "WHITESPACE";
            case TokenType::ERROR: return "ERROR";
            case TokenType::END_OF_FILE: return "END_OF_FILE";
            default: return "UNKNOWN";
        }
    }

    string toString() const {
        ostringstream oss;
        oss << left << setw(20) << tokenTypeToString() << " " << lexeme;
        return oss.str();
    }

};
