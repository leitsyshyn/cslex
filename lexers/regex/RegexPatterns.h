#pragma once

#include "../../Token.h"
#include <regex>
#include <vector>
#include <utility>

using namespace std;

static const vector<pair<TokenType, regex>> REGEX_PATTERNS = {
    // Comments: single-line and multi-line
    {TokenType::COMMENT, regex(R"(^//[^\n]*|^/\*[\s\S]*?\*/)")},
    
    // Preprocessor directives
    {TokenType::PREPROCESSOR, regex(R"(^#[^\n]*)")},
    
    // Interpolated strings: $@"...", $"...", @$"..."
    {TokenType::INTERPOLATED_STRING, regex(R"(^\$@"(?:[^"]|"")*"|^\$"(?:[^"\\]|\\.)*"|^@\$"(?:[^"]|"")*")")},
    
    // Verbatim strings: @"..."
    {TokenType::VERBATIM_STRING, regex(R"(^@"(?:[^"]|"")*")")},
    
    // Character literals: '...'
    {TokenType::CHARACTER_LITERAL, regex(R"(^'(?:[^'\\]|\\.)+')")},
    
    // String literals: "..."
    {TokenType::STRING_LITERAL, regex(R"(^"(?:[^"\\]|\\.)*")")},
    
    // ERROR: Invalid numeric literals (numbers followed by letters that aren't valid suffixes)
    // These must come BEFORE valid numeric patterns to catch errors like "123abc"
    // Binary with invalid continuation (must have at least one binary digit, then invalid letter)
    {TokenType::ERROR, regex(R"(^0[bB][01_]+[a-zA-Z][a-zA-Z0-9_]*)")},
    // Hexadecimal with invalid continuation (hex digits + non-hex letter g-z or G-Z)
    {TokenType::ERROR, regex(R"(^0[xX][0-9a-fA-F_]+[g-zG-Z][a-zA-Z0-9_]*)")},
    // Decimal numbers followed by invalid letters (excluding F,f,D,d,M,m,U,u,L,l,E,e for valid suffixes/exponents)
    {TokenType::ERROR, regex(R"(^(?:[1-9][0-9_]*(?:\.[0-9_]*)?(?:[eE][+-]?\d+)?|0(?![xXbB])[0-9_]*(?:\.[0-9_]*)?(?:[eE][+-]?\d+)?)[abcghi-kn-oqrstvwyzABCGHI-KN-OQRSTVWYZ][a-zA-Z0-9_]*)")},
    
    // Integer literals: binary (0b...) with optional suffixes (U, L, UL)
    {TokenType::INTEGER_LITERAL, regex(R"(^0[bB][01_]+[UuLl]{0,2})")},
    
    // Integer literals: hexadecimal (0x...) with optional suffixes
    {TokenType::INTEGER_LITERAL, regex(R"(^0[xX][0-9a-fA-F_]+[UuLl]{0,2})")},
    
    // Real literals: decimal with decimal point and optional exponent, with optional suffixes (F, D, M)
    {TokenType::REAL_LITERAL, regex(R"(^(?:\d[0-9_]*\.[0-9_]*(?:[eE][+-]?\d+)?)[FfDdMm]?)")},
    
    // Real literals: decimal with exponent, with optional suffixes
    {TokenType::REAL_LITERAL, regex(R"(^(?:\d[0-9_]*(?:[eE][+-]?\d+))[FfDdMm]?)")},
    
    // Real literals: decimal with mandatory real suffix
    {TokenType::REAL_LITERAL, regex(R"(^(?:\d[0-9_]*)[FfDdMm])")},
    
    // Integer literals: decimal without real indicators, with optional integer suffixes
    {TokenType::INTEGER_LITERAL, regex(R"(^(?:\d[0-9_]*)[UuLl]{0,2})")},
    
    // Identifiers: optional @ prefix, followed by letter/underscore and alphanumeric/underscore
    {TokenType::IDENTIFIER, regex(R"(^@?[a-zA-Z_][a-zA-Z0-9_]*)")},
    
    // Operators: compound and single character operators
    {TokenType::OPERATOR, regex(R"(^(<<=|>>=|\?\?=|===|!==|==|!=|<=|>=|&&|\|\||\?\?|\?\.|\+\+|--|<<|>>|=>|\+=|-=|\*=|/=|%=|&=|\|=|\^=|\+|-|\*|/|%|=|!|<|>|&|\||\^|~|\?|:|\.))")},
    
    // Separators: parentheses, brackets, braces, semicolon, comma
    {TokenType::SEPARATOR, regex(R"(^[\(\)\[\]\{\};,])")},
    
    // Whitespace: spaces, tabs, newlines
    {TokenType::WHITESPACE, regex(R"(^\s+)")},
    
    // Error: any single character that doesn't match above patterns
    {TokenType::ERROR, regex(R"(^.)")}
};
