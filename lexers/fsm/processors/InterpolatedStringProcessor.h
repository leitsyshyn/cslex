#pragma once

#include "IProcessor.h"

class InterpolatedStringProcessor : public IProcessor {
public:
    ProcessorResult process(InputBuffer& buffer) override {
        if (!startsInterpolatedString(buffer)) {
            return noMatch();
        }
        
        Position start = buffer.getCurrentPosition();
        std::string lexeme;
        lexeme += buffer.advance();
        
        bool is_verbatim = false;
        if (buffer.peek() == '@') {
            lexeme += buffer.advance();
            is_verbatim = true;
        }
        
        if (buffer.peek() != '"') {
            return unterminatedResult(buffer, start, lexeme);
        }

        lexeme += buffer.advance();
        if (readStringContent(buffer, lexeme, is_verbatim)) {
            return tokenResult(TokenType::INTERPOLATED_STRING, lexeme, start, buffer.getCurrentPosition());
        }

        return unterminatedResult(buffer, start, lexeme);
    }

private:
    static bool startsInterpolatedString(InputBuffer& buffer) {
        return buffer.peek() == '$' && (buffer.peek_next() == '"' || buffer.peek_next() == '@');
    }

    static ProcessorResult unterminatedResult(InputBuffer& buffer,
                                              const Position& start,
                                              const std::string& lexeme) {
        return diagnosticResult(DiagnosticCode::UnterminatedInterpolatedStringLiteral,
                                lexeme,
                                start,
                                buffer.getCurrentPosition());
    }

    static void readOpenBrace(InputBuffer& buffer, std::string& lexeme, int& brace_depth) {
        lexeme += buffer.advance();
        if (buffer.peek() == '{') {
            lexeme += buffer.advance();
            return;
        }

        ++brace_depth;
    }

    static void readCloseBrace(InputBuffer& buffer, std::string& lexeme, int& brace_depth) {
        lexeme += buffer.advance();
        if (buffer.peek() == '}' && brace_depth == 0) {
            lexeme += buffer.advance();
            return;
        }

        if (brace_depth > 0) {
            --brace_depth;
        }
    }

    static bool readQuote(InputBuffer& buffer, std::string& lexeme, bool is_verbatim) {
        if (is_verbatim && buffer.peek_next() == '"') {
            lexeme += buffer.advance();
            lexeme += buffer.advance();
            return false;
        }

        lexeme += buffer.advance();
        return true;
    }

    static void readEscape(InputBuffer& buffer, std::string& lexeme) {
        lexeme += buffer.advance();
        if (buffer.peek() != '\0') {
            lexeme += buffer.advance();
        }
    }

    static bool readStringContent(InputBuffer& buffer, std::string& lexeme, bool is_verbatim) {
        int brace_depth = 0;
        while (buffer.peek() != '\0') {
            if (buffer.peek() == '{') {
                readOpenBrace(buffer, lexeme, brace_depth);
            } else if (buffer.peek() == '}') {
                readCloseBrace(buffer, lexeme, brace_depth);
            } else if (buffer.peek() == '"' && brace_depth == 0) {
                if (readQuote(buffer, lexeme, is_verbatim)) {
                    return true;
                }
            } else if (buffer.peek() == '\\' && !is_verbatim) {
                readEscape(buffer, lexeme);
            } else {
                lexeme += buffer.advance();
            }
        }

        return false;
    }
};
