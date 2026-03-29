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
    
    // Interpolated strings: $@"...", $"..."
    {regex(R"(^\$@"(?:[^"]|"")*"|^\$"(?:[^"\\]|\\.)*")"), false, TokenType::INTERPOLATED_STRING, DiagnosticCode::UnexpectedCharacter},

    // Unterminated interpolated strings
    {regex(R"(^\$@(?=[^"])|^\$@"(?:[^"]|"")*$|^\$"(?:[^"\\]|\\.)*$)"), true, TokenType::ERROR, DiagnosticCode::UnterminatedInterpolatedStringLiteral},
     
    // Verbatim strings: @"..."
    {regex(R"(^@"(?:[^"]|"")*")"), false, TokenType::VERBATIM_STRING, DiagnosticCode::UnexpectedCharacter},

    // Unterminated verbatim strings
    {regex(R"(^@"(?:[^"]|"")*$)"), true, TokenType::ERROR, DiagnosticCode::UnterminatedVerbatimStringLiteral},
     
    // Character literals: '...'
    {regex(R"(^'(?:[^'\\\n]|\\.)*')"), false, TokenType::CHARACTER_LITERAL, DiagnosticCode::UnexpectedCharacter},

    // Unterminated character literals
    {regex(R"(^'(?:[^'\\\n]|\\.)*)"), true, TokenType::ERROR, DiagnosticCode::UnterminatedCharacterLiteral},
     
    // String literals: "..."
    {regex(R"(^"(?:[^"\\]|\\.)*")"), false, TokenType::STRING_LITERAL, DiagnosticCode::UnexpectedCharacter},

    // Unterminated string literals
    {regex(R"(^"(?:[^"\\]|\\.)*(?:$|\n))"), true, TokenType::ERROR, DiagnosticCode::UnterminatedStringLiteral},
     
    // Integer literals: binary (0b...) with optional suffixes (U, L, UL, LU)
    {regex(R"(^0[bB][01_]+([Uu]([Ll])?|[Ll]([Uu])?)?(?![a-zA-Z0-9_]))"), false, TokenType::INTEGER_LITERAL, DiagnosticCode::UnexpectedCharacter},

    // Invalid binary literals
    {regex(R"(^0[bB][01_]+[Uu][Ll][a-zA-Z_][a-zA-Z0-9_]*|^0[bB][01_]+[Ll][Uu][a-zA-Z_][a-zA-Z0-9_]*|^0[bB][01_]+[Uu][a-km-zA-KM-Z_][a-zA-Z0-9_]*|^0[bB][01_]+[Ll][a-tv-zA-TV-Z_][a-zA-Z0-9_]*|^0[bB][01_]+[a-tv-zA-TV-Z_][a-zA-Z0-9_]*)"), true, TokenType::ERROR, DiagnosticCode::InvalidNumericLiteral},

    // Integer literals: hexadecimal (0x...) with optional suffixes (U, L, UL, LU)
    {regex(R"(^0[xX][0-9a-fA-F_]+([Uu]([Ll])?|[Ll]([Uu])?)?(?![a-zA-Z0-9_]))"), false, TokenType::INTEGER_LITERAL, DiagnosticCode::UnexpectedCharacter},

    // Invalid hexadecimal literals
    {regex(R"(^0[xX][0-9a-fA-F_]+[Uu][Ll][a-zA-Z_][a-zA-Z0-9_]*|^0[xX][0-9a-fA-F_]+[Ll][Uu][a-zA-Z_][a-zA-Z0-9_]*|^0[xX][0-9a-fA-F_]+[Uu][a-km-zA-KM-Z_][a-zA-Z0-9_]*|^0[xX][0-9a-fA-F_]+[Ll][a-tv-zA-TV-Z_][a-zA-Z0-9_]*|^0[xX][0-9a-fA-F_]+[g-km-rtv-zG-KM-RTV-Z_][a-zA-Z0-9_]*)"), true, TokenType::ERROR, DiagnosticCode::InvalidNumericLiteral},

    // Real literals: decimal with decimal point and optional exponent, with optional suffixes (F, D, M)
    {regex(R"(^(?:\d[0-9_]*\.[0-9_]*(?:[eE][+-]?\d+)?)[FfDdMm]?(?![a-zA-Z0-9_]))"), false, TokenType::REAL_LITERAL, DiagnosticCode::UnexpectedCharacter},

    // Real literals: decimal with exponent, with optional suffixes
    {regex(R"(^(?:\d[0-9_]*(?:[eE][+-]?\d+))[FfDdMm]?(?![a-zA-Z0-9_]))"), false, TokenType::REAL_LITERAL, DiagnosticCode::UnexpectedCharacter},

    // Real literals: decimal with mandatory real suffix
    {regex(R"(^(?:\d[0-9_]*)[FfDdMm](?![a-zA-Z0-9_]))"), false, TokenType::REAL_LITERAL, DiagnosticCode::UnexpectedCharacter},

    // Invalid real literals
    {regex(R"(^((\d[0-9_]*\.[0-9_]*([eE][+-]?\d+)?|\d[0-9_]*([eE][+-]?\d+)))[FfDdMm][a-zA-Z_][a-zA-Z0-9_]*|^((\d[0-9_]*\.[0-9_]*([eE][+-]?\d+)?|\d[0-9_]*([eE][+-]?\d+)))[a-cg-lno-zA-CG-LNO-Z_][a-zA-Z0-9_]*|^\d[0-9_]*[FfDdMm][a-zA-Z_][a-zA-Z0-9_]*)"), true, TokenType::ERROR, DiagnosticCode::InvalidNumericLiteral},

    // Integer literals: decimal without real indicators, with optional integer suffixes (U, L, UL, LU)
    {regex(R"(^\d[0-9_]*([Uu]([Ll])?|[Ll]([Uu])?)?(?![a-zA-Z0-9_]))"), false, TokenType::INTEGER_LITERAL, DiagnosticCode::UnexpectedCharacter},

    // Invalid decimal integer literals
    {regex(R"(^\d[0-9_]*[Uu][Ll][a-zA-Z_][a-zA-Z0-9_]*|^\d[0-9_]*[Ll][Uu][a-zA-Z_][a-zA-Z0-9_]*|^\d[0-9_]*[Uu][a-km-zA-KM-Z_][a-zA-Z0-9_]*|^\d[0-9_]*[Ll][a-tv-zA-TV-Z_][a-zA-Z0-9_]*|^\d[0-9_]*[a-cek-tv-zA-CEK-TV-Z_][a-zA-Z0-9_]*)"), true, TokenType::ERROR, DiagnosticCode::InvalidNumericLiteral},
     
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
