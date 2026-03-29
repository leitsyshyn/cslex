#pragma once

#include "Token.h"
#include "lexers/Diagnostic.h"
#include "lexers/ILexer.h"
#include "lexers/LexerResult.h"
#include "lexers/fsm/FSMLexer.h"
#include "lexers/regex/RegexLexer.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

enum class LexerKind {
    Fsm,
    Regex
};

inline std::string lexerKindName(LexerKind kind) {
    switch (kind) {
        case LexerKind::Fsm:
            return "Fsm";
        case LexerKind::Regex:
            return "Regex";
    }

    return "Unknown";
}

inline std::unique_ptr<ILexer> makeLexer(LexerKind kind) {
    switch (kind) {
        case LexerKind::Fsm:
            return std::make_unique<FSMLexer>();
        case LexerKind::Regex:
            return std::make_unique<RegexLexer>();
    }

    return nullptr;
}

inline std::vector<Token> nonEofTokens(const std::vector<Token>& tokens) {
    std::vector<Token> filtered;
    filtered.reserve(tokens.size());

    for (const auto& token : tokens) {
        if (token.type != TokenType::END_OF_FILE) {
            filtered.push_back(token);
        }
    }

    return filtered;
}

inline auto TokenIs(TokenType type, const std::string& lexeme) {
    return ::testing::AllOf(
        ::testing::Field("type", &Token::type, type),
        ::testing::Field("lexeme", &Token::lexeme, lexeme));
}

inline auto DiagnosticIs(DiagnosticCode code, const std::string& lexeme) {
    return ::testing::AllOf(
        ::testing::Field("code", &Diagnostic::code, code),
        ::testing::Field("lexeme", &Diagnostic::lexeme, lexeme));
}
