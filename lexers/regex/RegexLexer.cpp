#include "RegexLexer.h"
#include "../LexicalException.h"
#include <regex>

namespace {
void appendDiagnostic(LexerResult& result, const Diagnostic& diagnostic, ErrorMode errorMode) {
    if (errorMode == ErrorMode::Throw) {
        throw LexicalException(diagnostic);
    }

    result.diagnostics.emplace_back(diagnostic);
}

void appendToken(LexerResult& result,
                 const RegexRule& rule,
                 const std::string& lexeme,
                 const Position& start,
                 const Position& end) {
    if (rule.token_type == TokenType::WHITESPACE) {
        return;
    }

    TokenType token_type = rule.token_type;
    if (token_type == TokenType::IDENTIFIER) {
        token_type = classifyIdentifierLexeme(lexeme);
    }

    result.tokens.emplace_back(token_type, lexeme, start, end);
}

bool appendRuleMatch(const std::string& source,
                     const RegexRule& rule,
                     std::size_t& cursor,
                     Position& current_position,
                     LexerResult& result,
                     ErrorMode errorMode) {
    std::smatch match;
    if (!std::regex_search(source.cbegin() + cursor,
                           source.cend(),
                           match,
                           rule.pattern,
                           std::regex_constants::match_continuous)) {
        return false;
    }

    const std::string lexeme = match.str(0);
    const Position start = current_position;
    const Position end = advancePosition(start, lexeme);

    if (rule.emits_diagnostic) {
        appendDiagnostic(result, makeDiagnostic(rule.diagnostic_code, lexeme, start, end), errorMode);
    } else {
        appendToken(result, rule, lexeme, start, end);
    }

    current_position = end;
    cursor = end.index;
    return true;
}
} // namespace

LexerResult RegexLexer::tokenize(const std::string& source, ErrorMode errorMode) {
    return tokenizeSource(source, errorMode);
}

LexerResult RegexLexer::tokenizeSource(const std::string& source, ErrorMode errorMode) {
    LexerResult result;
    Position current_position;
    
    size_t cursor = 0;
    while (cursor < source.length()) {
        bool matched = false;
        
        for (const auto& rule : REGEX_PATTERNS) {
            if (appendRuleMatch(source, rule, cursor, current_position, result, errorMode)) {
                matched = true;
                break;
            }
        }
        
        if (!matched) {
            std::string lexeme(1, source[cursor]);
            Position start = current_position;
            Position end = advancePosition(start, lexeme);
            appendDiagnostic(result,
                             makeDiagnostic(DiagnosticCode::UnexpectedCharacter, lexeme, start, end),
                             errorMode);
            current_position = end;
            cursor = end.index;
        }
    }
    
    result.tokens.emplace_back(TokenType::END_OF_FILE, "", current_position, current_position);
    
    return result;
}
