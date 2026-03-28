#include "RegexLexer.h"
#include "../LexicalException.h"
#include <regex>

namespace {
void appendDiagnostic(LexerResult& result, const Diagnostic& diagnostic, ErrorMode errorMode) {
    if (errorMode == ErrorMode::Throw) {
        throw LexicalException(diagnostic);
    }

    result.diagnostics.push_back(diagnostic);
}
} // namespace

LexerResult RegexLexer::tokenize(const std::string& source, ErrorMode errorMode) {
    return tokenizeSource(source, errorMode);
}

LexerResult RegexLexer::tokenizeSource(const string& source, ErrorMode errorMode) {
    LexerResult result;
    Position current_position;
    
    size_t cursor = 0;
    while (cursor < source.length()) {
        bool matched = false;
        
        for (const auto& rule : REGEX_PATTERNS) {
            smatch match;
            if (regex_search(source.cbegin() + cursor, source.cend(), match, rule.pattern,
                            regex_constants::match_continuous)) {
                string lexeme = match.str(0);
                Position start = current_position;
                Position end = advancePosition(start, lexeme);

                if (rule.emits_diagnostic) {
                    appendDiagnostic(result,
                                     makeDiagnostic(rule.diagnostic_code, lexeme, start, end),
                                     errorMode);
                } else if (rule.token_type != TokenType::WHITESPACE) {
                    TokenType token_type = rule.token_type;
                    if (token_type == TokenType::IDENTIFIER) {
                        token_type = classifyIdentifierLexeme(lexeme);
                    }

                    result.tokens.push_back(Token(token_type, lexeme, start, end));
                }

                current_position = end;
                cursor = end.index;
                matched = true;
                break;
            }
        }
        
        if (!matched) {
            string lexeme(1, source[cursor]);
            Position start = current_position;
            Position end = advancePosition(start, lexeme);
            appendDiagnostic(result,
                             makeDiagnostic(DiagnosticCode::UnexpectedCharacter, lexeme, start, end),
                             errorMode);
            current_position = end;
            cursor = end.index;
        }
    }
    
    result.tokens.push_back(Token(TokenType::END_OF_FILE, "", current_position, current_position));
    
    return result;
}
