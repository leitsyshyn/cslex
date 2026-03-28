#pragma once

#include "../../Token.h"
#include "../Diagnostic.h"
#include <regex>
#include <string>
#include <vector>

using namespace std;

struct RegexRule {
    regex pattern;
    bool emits_diagnostic;
    TokenType token_type;
    DiagnosticCode diagnostic_code;
};

static const vector<RegexRule> REGEX_PATTERNS = {
    // Comments: single-line and multi-line
    {regex(R"(^//[^\n]*|^/\*[\s\S]*?\*/)"), false, TokenType::COMMENT, DiagnosticCode::UnexpectedCharacter},

    // Unterminated comments
    {regex(R"(^/\*[\s\S]*$)"), true, TokenType::ERROR, DiagnosticCode::UnterminatedComment},
    
    // Preprocessor directives
    {regex(R"(^#[^\n]*)"), false, TokenType::PREPROCESSOR, DiagnosticCode::UnexpectedCharacter},
    
    // Interpolated strings: $@"...", $"...", @$"..."
    {regex(R"(^\$@"(?:[^"]|"")*"|^\$"(?:[^"\\]|\\.)*"|^@\$"(?:[^"]|"")*")"), false, TokenType::INTERPOLATED_STRING, DiagnosticCode::UnexpectedCharacter},

    // Unterminated interpolated strings
    {regex(R"(^\$@"(?:[^"]|"")*$|^@\$"(?:[^"]|"")*$|^\$"(?:[^"\\]|\\.)*$)"), true, TokenType::ERROR, DiagnosticCode::UnterminatedInterpolatedStringLiteral},
     
    // Verbatim strings: @"..."
    {regex(R"(^@"(?:[^"]|"")*")"), false, TokenType::VERBATIM_STRING, DiagnosticCode::UnexpectedCharacter},

    // Unterminated verbatim strings
    {regex(R"(^@"(?:[^"]|"")*$)"), true, TokenType::ERROR, DiagnosticCode::UnterminatedVerbatimStringLiteral},
     
    // Character literals: '...'
    {regex(R"(^'(?:[^'\\]|\\.)')"), false, TokenType::CHARACTER_LITERAL, DiagnosticCode::UnexpectedCharacter},

    // Unterminated character literals
    {regex(R"(^'(?:[^'\\\n]|\\.)*(?:$|\n))"), true, TokenType::ERROR, DiagnosticCode::UnterminatedCharacterLiteral},
     
    // String literals: "..."
    {regex(R"(^"(?:[^"\\]|\\.)*")"), false, TokenType::STRING_LITERAL, DiagnosticCode::UnexpectedCharacter},

    // Unterminated string literals
    {regex(R"(^"(?:[^"\\]|\\.)*(?:$|\n))"), true, TokenType::ERROR, DiagnosticCode::UnterminatedStringLiteral},
     
    // ERROR: Invalid numeric literals (numbers followed by letters that aren't valid suffixes)
    // These must come BEFORE valid numeric patterns to catch errors like "123abc"
    // Binary with invalid continuation (must have at least one binary digit, then invalid letter)
    {regex(R"(^0[bB][01_]+[a-zA-Z][a-zA-Z0-9_]*)"), true, TokenType::ERROR, DiagnosticCode::InvalidNumericLiteral},
    // Hexadecimal with invalid continuation (hex digits + non-hex letter g-z or G-Z)
    {regex(R"(^0[xX][0-9a-fA-F_]+[g-zG-Z][a-zA-Z0-9_]*)"), true, TokenType::ERROR, DiagnosticCode::InvalidNumericLiteral},
    // Decimal numbers followed by invalid letters (excluding F,f,D,d,M,m,U,u,L,l,E,e for valid suffixes/exponents)
    {regex(R"(^(?:[1-9][0-9_]*(?:\.[0-9_]*)?(?:[eE][+-]?\d+)?|0(?![xXbB])[0-9_]*(?:\.[0-9_]*)?(?:[eE][+-]?\d+)?)[abcghi-kn-oqrstvwyzABCGHI-KN-OQRSTVWYZ][a-zA-Z0-9_]*)"), true, TokenType::ERROR, DiagnosticCode::InvalidNumericLiteral},
     
    // Integer literals: binary (0b...) with optional suffixes (U, L, UL)
    {regex(R"(^0[bB][01_]+[UuLl]{0,2})"), false, TokenType::INTEGER_LITERAL, DiagnosticCode::UnexpectedCharacter},
     
    // Integer literals: hexadecimal (0x...) with optional suffixes
    {regex(R"(^0[xX][0-9a-fA-F_]+[UuLl]{0,2})"), false, TokenType::INTEGER_LITERAL, DiagnosticCode::UnexpectedCharacter},
     
    // Real literals: decimal with decimal point and optional exponent, with optional suffixes (F, D, M)
    {regex(R"(^(?:\d[0-9_]*\.[0-9_]*(?:[eE][+-]?\d+)?)[FfDdMm]?)"), false, TokenType::REAL_LITERAL, DiagnosticCode::UnexpectedCharacter},
     
    // Real literals: decimal with exponent, with optional suffixes
    {regex(R"(^(?:\d[0-9_]*(?:[eE][+-]?\d+))[FfDdMm]?)"), false, TokenType::REAL_LITERAL, DiagnosticCode::UnexpectedCharacter},
     
    // Real literals: decimal with mandatory real suffix
    {regex(R"(^(?:\d[0-9_]*)[FfDdMm])"), false, TokenType::REAL_LITERAL, DiagnosticCode::UnexpectedCharacter},
     
    // Integer literals: decimal without real indicators, with optional integer suffixes
    {regex(R"(^(?:\d[0-9_]*)[UuLl]{0,2})"), false, TokenType::INTEGER_LITERAL, DiagnosticCode::UnexpectedCharacter},
     
    // Identifiers: optional @ prefix, followed by letter/underscore and alphanumeric/underscore
    {regex(R"(^@?[a-zA-Z_][a-zA-Z0-9_]*)"), false, TokenType::IDENTIFIER, DiagnosticCode::UnexpectedCharacter},
     
    // Operators: compound and single character operators
    {regex(R"(^(<<=|>>=|\?\?=|===|!==|==|!=|<=|>=|&&|\|\||\?\?|\?\.|\+\+|--|<<|>>|=>|\+=|-=|\*=|/=|%=|&=|\|=|\^=|\+|-|\*|/|%|=|!|<|>|&|\||\^|~|\?|:|\.))"), false, TokenType::OPERATOR, DiagnosticCode::UnexpectedCharacter},
     
    // Separators: parentheses, brackets, braces, semicolon, comma
    {regex(R"(^[\(\)\[\]\{\};,])"), false, TokenType::SEPARATOR, DiagnosticCode::UnexpectedCharacter},
     
    // Whitespace: spaces, tabs, newlines
    {regex(R"(^\s+)"), false, TokenType::WHITESPACE, DiagnosticCode::UnexpectedCharacter},
     
    // Error: any single character that doesn't match above patterns
    {regex(R"(^.)"), true, TokenType::ERROR, DiagnosticCode::UnexpectedCharacter}
};
