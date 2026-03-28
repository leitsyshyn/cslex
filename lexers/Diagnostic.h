#pragma once

#include "../Position.h"
#include <sstream>
#include <string>

enum class DiagnosticCode {
    UnexpectedCharacter,
    InvalidNumericLiteral,
    UnterminatedStringLiteral,
    UnterminatedCharacterLiteral,
    UnterminatedComment,
    UnterminatedVerbatimStringLiteral,
    UnterminatedInterpolatedStringLiteral
};

inline std::string diagnosticCodeToString(DiagnosticCode code) {
    switch (code) {
        case DiagnosticCode::UnexpectedCharacter:
            return "UnexpectedCharacter";
        case DiagnosticCode::InvalidNumericLiteral:
            return "InvalidNumericLiteral";
        case DiagnosticCode::UnterminatedStringLiteral:
            return "UnterminatedStringLiteral";
        case DiagnosticCode::UnterminatedCharacterLiteral:
            return "UnterminatedCharacterLiteral";
        case DiagnosticCode::UnterminatedComment:
            return "UnterminatedComment";
        case DiagnosticCode::UnterminatedVerbatimStringLiteral:
            return "UnterminatedVerbatimStringLiteral";
        case DiagnosticCode::UnterminatedInterpolatedStringLiteral:
            return "UnterminatedInterpolatedStringLiteral";
    }

    return "UnknownDiagnostic";
}

struct Diagnostic {
    DiagnosticCode code;
    std::string message;
    std::string lexeme;
    Position start;
    Position end;

    Diagnostic() : code(DiagnosticCode::UnexpectedCharacter) {}

    Diagnostic(DiagnosticCode code,
               std::string message,
               std::string lexeme,
               const Position& start,
               const Position& end)
        : code(code),
          message(std::move(message)),
          lexeme(std::move(lexeme)),
          start(start),
          end(end) {}

    std::string toString() const {
        std::ostringstream oss;
        oss << diagnosticCodeToString(code) << " at " << start.toString() << ": " << message;
        if (!lexeme.empty()) {
            oss << " [" << lexeme << "]";
        }
        return oss.str();
    }
};

inline std::string buildDiagnosticMessage(DiagnosticCode code, const std::string& lexeme) {
    switch (code) {
        case DiagnosticCode::UnexpectedCharacter:
            return "Unexpected character sequence";
        case DiagnosticCode::InvalidNumericLiteral:
            return "Invalid numeric literal";
        case DiagnosticCode::UnterminatedStringLiteral:
            return "Unterminated string literal";
        case DiagnosticCode::UnterminatedCharacterLiteral:
            return "Unterminated character literal";
        case DiagnosticCode::UnterminatedComment:
            return "Unterminated comment";
        case DiagnosticCode::UnterminatedVerbatimStringLiteral:
            return "Unterminated verbatim string literal";
        case DiagnosticCode::UnterminatedInterpolatedStringLiteral:
            return "Unterminated interpolated string literal";
    }

    (void)lexeme;
    return "Unknown lexical diagnostic";
}

inline Diagnostic makeDiagnostic(DiagnosticCode code,
                                 const std::string& lexeme,
                                 const Position& start,
                                 const Position& end) {
    return Diagnostic(code, buildDiagnosticMessage(code, lexeme), lexeme, start, end);
}
