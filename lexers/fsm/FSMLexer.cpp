
#include "FSMLexer.h"
#include "processors/PreprocessorProcessor.h"
#include "processors/CommentProcessor.h"
#include "processors/VerbatimStringProcessor.h"
#include "processors/InterpolatedStringProcessor.h"
#include "processors/StringProcessor.h"
#include "processors/CharProcessor.h"
#include "processors/NumberProcessor.h"
#include "processors/IdentifierProcessor.h"
#include "processors/OperatorProcessor.h"
#include "processors/SeparatorProcessor.h"
#include <cctype>

FSMLexer::FSMLexer() {
    processors.push_back(make_unique<PreprocessorProcessor>());
    processors.push_back(make_unique<CommentProcessor>());
    processors.push_back(make_unique<VerbatimStringProcessor>());
    processors.push_back(make_unique<InterpolatedStringProcessor>());
    processors.push_back(make_unique<StringProcessor>());
    processors.push_back(make_unique<CharProcessor>());
    processors.push_back(make_unique<NumberProcessor>());
    processors.push_back(make_unique<IdentifierProcessor>());
    processors.push_back(make_unique<OperatorProcessor>());
    processors.push_back(make_unique<SeparatorProcessor>());
}

std::vector<Token> FSMLexer::tokenize(const std::string& source) {
    InputBuffer buffer(source);
    return tokenizeBuffer(buffer);
}

vector<Token> FSMLexer::tokenizeBuffer(InputBuffer& buffer) {
    vector<Token> tokens;

    while (!buffer.eof()) {
        const char c = buffer.peek();
        
        if (isspace(c)) {
            buffer.advance();
            continue;
        }
        
        bool processed = false;
        for (auto& processor : processors) {
            if (processor->process(buffer, tokens)) {
                processed = true;
                break;
            }
        }
        
        if (!processed) {
            Position start = buffer.getCurrentPosition();
            string lexeme(1, buffer.advance());
            Position end = buffer.getCurrentPosition();
            tokens.push_back(Token(TokenType::ERROR, lexeme, start, end));
        }
    }
    
    Position end_pos = buffer.getCurrentPosition();
    tokens.push_back(Token(TokenType::END_OF_FILE, "", end_pos, end_pos));
    
    return tokens;
}
