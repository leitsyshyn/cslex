#pragma once

#include "../../Diagnostic.h"
#include "../InputBuffer.h"
#include "../../../Token.h"
#include <optional>
#include <string>

using namespace std;

struct ProcessorResult {
    bool matched = false;
    std::optional<Token> token;
    std::optional<Diagnostic> diagnostic;
};

class IProcessor {
public:
    virtual ~IProcessor() = default;
    virtual ProcessorResult process(InputBuffer& buffer) = 0;

protected:
    static ProcessorResult noMatch() {
        return ProcessorResult{};
    }

    static ProcessorResult tokenResult(TokenType type,
                                       const string& lexeme,
                                       const Position& start,
                                       const Position& end) {
        ProcessorResult result;
        result.matched = true;
        result.token = Token(type, lexeme, start, end);
        return result;
    }

    static ProcessorResult diagnosticResult(DiagnosticCode code,
                                            const string& lexeme,
                                            const Position& start,
                                            const Position& end) {
        ProcessorResult result;
        result.matched = true;
        result.diagnostic = makeDiagnostic(code, lexeme, start, end);
        return result;
    }
};
