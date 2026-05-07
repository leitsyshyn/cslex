
#include "FSMLexer.h"
#include "../LexicalException.h"
#include "FsmRuleRegistry.h"
#include <cctype>

FSMLexer::FSMLexer() {
    processors = createDefaultFSMProcessors();
}

FSMLexer::FSMLexer(std::vector<std::unique_ptr<IProcessor>> customProcessors)
    : processors(std::move(customProcessors)) {
}

namespace {
void appendProcessorResult(LexerResult& result, const ProcessorResult& processorResult, ErrorMode errorMode) {
    if (processorResult.token.has_value()) {
        result.tokens.emplace_back(*processorResult.token);
        return;
    }

    if (processorResult.diagnostic.has_value()) {
        if (errorMode == ErrorMode::Throw) {
            throw LexicalException(*processorResult.diagnostic);
        }

        result.diagnostics.emplace_back(*processorResult.diagnostic);
    }
}
} // namespace

LexerResult FSMLexer::tokenize(const std::string& source, ErrorMode errorMode) {
    InputBuffer buffer(source);
    return tokenizeBuffer(buffer, errorMode);
}

LexerResult FSMLexer::tokenizeBuffer(InputBuffer& buffer, ErrorMode errorMode) const {
    LexerResult result;

    while (!buffer.eof()) {
        const char c = buffer.peek();
        
        if (isspace(c)) {
            buffer.advance();
            continue;
        }
        
        bool processed = false;
        for (auto& processor : processors) {
            const auto processorResult = processor->process(buffer);
            if (processorResult.matched) {
                appendProcessorResult(result, processorResult, errorMode);
                processed = true;
                break;
            }
        }
        
        if (!processed) {
            Position start = buffer.getCurrentPosition();
            std::string lexeme(1, buffer.advance());
            Position end = buffer.getCurrentPosition();

            Diagnostic diagnostic = makeDiagnostic(DiagnosticCode::UnexpectedCharacter, lexeme, start, end);
            if (errorMode == ErrorMode::Throw) {
                throw LexicalException(diagnostic);
            }

            result.diagnostics.emplace_back(diagnostic);
        }
    }
    
    Position end_pos = buffer.getCurrentPosition();
    result.tokens.emplace_back(TokenType::END_OF_FILE, "", end_pos, end_pos);
    
    return result;
}
