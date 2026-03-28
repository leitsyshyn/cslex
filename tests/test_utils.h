#pragma once

#include "Token.h"
#include "lexers/Diagnostic.h"
#include "lexers/ILexer.h"
#include "lexers/LexerResult.h"
#include "lexers/fsm/FSMLexer.h"
#include "lexers/regex/RegexLexer.h"
#include <gtest/gtest.h>
#include <initializer_list>
#include <memory>
#include <ostream>
#include <sstream>
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

struct ExpectedToken {
    TokenType type;
    std::string lexeme;
};

struct ExpectedDiagnostic {
    DiagnosticCode code;
    std::string lexeme;
};

inline ::testing::AssertionResult TokensMatch(const std::vector<Token>& actual,
                                              std::initializer_list<ExpectedToken> expected) {
    if (actual.size() != expected.size()) {
        return ::testing::AssertionFailure()
               << "Expected " << expected.size() << " tokens but got " << actual.size();
    }

    std::size_t index = 0;
    for (const auto& item : expected) {
        if (actual[index].type != item.type || actual[index].lexeme != item.lexeme) {
            return ::testing::AssertionFailure()
                   << "Token mismatch at index " << index
                   << ": expected (" << static_cast<int>(item.type) << ", '" << item.lexeme
                   << "') but got (" << static_cast<int>(actual[index].type) << ", '"
                   << actual[index].lexeme << "')";
        }
        ++index;
    }

    return ::testing::AssertionSuccess();
}

inline ::testing::AssertionResult DiagnosticsMatch(const std::vector<Diagnostic>& actual,
                                                   std::initializer_list<ExpectedDiagnostic> expected) {
    if (actual.size() != expected.size()) {
        return ::testing::AssertionFailure()
               << "Expected " << expected.size() << " diagnostics but got " << actual.size();
    }

    std::size_t index = 0;
    for (const auto& item : expected) {
        if (actual[index].code != item.code || actual[index].lexeme != item.lexeme) {
            return ::testing::AssertionFailure()
                   << "Diagnostic mismatch at index " << index
                   << ": expected (" << static_cast<int>(item.code) << ", '" << item.lexeme
                   << "') but got (" << static_cast<int>(actual[index].code) << ", '"
                   << actual[index].lexeme << "')";
        }
        ++index;
    }

    return ::testing::AssertionSuccess();
}

struct TokenSnapshot {
    TokenType type;
    std::string lexeme;
    std::size_t startIndex;
    int startLine;
    int startCol;
    std::size_t endIndex;
    int endLine;
    int endCol;
};

inline bool operator==(const TokenSnapshot& lhs, const TokenSnapshot& rhs) {
    return lhs.type == rhs.type &&
           lhs.lexeme == rhs.lexeme &&
           lhs.startIndex == rhs.startIndex &&
           lhs.startLine == rhs.startLine &&
           lhs.startCol == rhs.startCol &&
           lhs.endIndex == rhs.endIndex &&
           lhs.endLine == rhs.endLine &&
           lhs.endCol == rhs.endCol;
}

inline std::ostream& operator<<(std::ostream& os, const TokenSnapshot& token) {
    os << token.lexeme << " [" << static_cast<int>(token.type) << "] @ "
       << token.startLine << ':' << token.startCol << "-" << token.endLine << ':' << token.endCol;
    return os;
}

inline TokenSnapshot snapshotToken(const Token& token) {
    return TokenSnapshot{
        token.type,
        token.lexeme,
        token.start.index,
        token.start.line,
        token.start.col,
        token.end.index,
        token.end.line,
        token.end.col,
    };
}

inline std::vector<TokenSnapshot> snapshotTokens(const std::vector<Token>& tokens) {
    std::vector<TokenSnapshot> snapshots;
    snapshots.reserve(tokens.size());

    for (const auto& token : tokens) {
        snapshots.push_back(snapshotToken(token));
    }

    return snapshots;
}

inline std::vector<TokenSnapshot> snapshotTokens(const LexerResult& result) {
    return snapshotTokens(result.tokens);
}

struct DiagnosticSnapshot {
    DiagnosticCode code;
    std::string message;
    std::string lexeme;
    std::size_t startIndex;
    int startLine;
    int startCol;
    std::size_t endIndex;
    int endLine;
    int endCol;
};

inline bool operator==(const DiagnosticSnapshot& lhs, const DiagnosticSnapshot& rhs) {
    return lhs.code == rhs.code &&
           lhs.message == rhs.message &&
           lhs.lexeme == rhs.lexeme &&
           lhs.startIndex == rhs.startIndex &&
           lhs.startLine == rhs.startLine &&
           lhs.startCol == rhs.startCol &&
           lhs.endIndex == rhs.endIndex &&
           lhs.endLine == rhs.endLine &&
           lhs.endCol == rhs.endCol;
}

inline std::ostream& operator<<(std::ostream& os, const DiagnosticSnapshot& diagnostic) {
    os << diagnostic.lexeme << " [" << static_cast<int>(diagnostic.code) << "] @ "
       << diagnostic.startLine << ':' << diagnostic.startCol << "-"
       << diagnostic.endLine << ':' << diagnostic.endCol;
    return os;
}

inline DiagnosticSnapshot snapshotDiagnostic(const Diagnostic& diagnostic) {
    return DiagnosticSnapshot{
        diagnostic.code,
        diagnostic.message,
        diagnostic.lexeme,
        diagnostic.start.index,
        diagnostic.start.line,
        diagnostic.start.col,
        diagnostic.end.index,
        diagnostic.end.line,
        diagnostic.end.col,
    };
}

inline std::vector<DiagnosticSnapshot> snapshotDiagnostics(const std::vector<Diagnostic>& diagnostics) {
    std::vector<DiagnosticSnapshot> snapshots;
    snapshots.reserve(diagnostics.size());

    for (const auto& diagnostic : diagnostics) {
        snapshots.push_back(snapshotDiagnostic(diagnostic));
    }

    return snapshots;
}

inline std::vector<DiagnosticSnapshot> snapshotDiagnostics(const LexerResult& result) {
    return snapshotDiagnostics(result.diagnostics);
}
