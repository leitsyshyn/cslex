#pragma once

#include "Diagnostic.h"
#include <sstream>
#include <stdexcept>

class LexicalException : public std::runtime_error {
public:
    explicit LexicalException(const Diagnostic& diagnostic)
        : std::runtime_error(buildMessage(diagnostic)), diagnostic(diagnostic) {}

    const Diagnostic& getDiagnostic() const {
        return diagnostic;
    }

private:
    Diagnostic diagnostic;

    static std::string buildMessage(const Diagnostic& diagnostic) {
        std::ostringstream oss;
        oss << diagnosticCodeToString(diagnostic.code) << " at " << diagnostic.start.toString();
        if (!diagnostic.lexeme.empty()) {
            oss << ": " << diagnostic.lexeme;
        }
        return oss.str();
    }
};
