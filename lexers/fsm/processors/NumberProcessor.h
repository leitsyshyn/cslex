#pragma once

#include "IProcessor.h"
#include "../../LexicalRules.h"

class NumberProcessor : public IProcessor {
public:
    ProcessorResult process(InputBuffer& buffer) override {
        if (const char c = buffer.peek(); !isDigitChar(c)) {
            return noMatch();
        }
        
        Position start = buffer.getCurrentPosition();
        std::string lexeme;

        if (startsWithPrefix(buffer, 'b', 'B')) {
            return processPrefixedInteger(buffer, start, lexeme, false);
        }

        if (startsWithPrefix(buffer, 'x', 'X')) {
            return processPrefixedInteger(buffer, start, lexeme, true);
        }

        return processDecimal(buffer, start, lexeme);
    }

private:
    static bool startsWithPrefix(InputBuffer& buffer, char lowerPrefix, char upperPrefix) {
        return buffer.peek() == '0' && (buffer.peek_next() == lowerPrefix || buffer.peek_next() == upperPrefix);
    }

    static bool isUnsignedSuffix(char c) {
        return c == 'u' || c == 'U';
    }

    static bool isLongSuffix(char c) {
        return c == 'l' || c == 'L';
    }

    static bool isRealSuffix(char c) {
        return c == 'f' || c == 'F' || c == 'd' || c == 'D' || c == 'm' || c == 'M';
    }

    static bool isInvalidTailStart(char c) {
        return isAlphaChar(c) || c == '_';
    }

    static bool isPrefixedDigit(char c, bool hexadecimal) {
        return hexadecimal ? isHexDigitChar(c) : c == '0' || c == '1';
    }

    static void readIntegerSuffix(InputBuffer& buffer, std::string& lexeme) {
        if (isUnsignedSuffix(buffer.peek())) {
            lexeme += buffer.advance();
            if (isLongSuffix(buffer.peek())) {
                lexeme += buffer.advance();
            }
            return;
        }

        if (isLongSuffix(buffer.peek())) {
            lexeme += buffer.advance();
            if (isUnsignedSuffix(buffer.peek())) {
                lexeme += buffer.advance();
            }
        }
    }

    static bool readInvalidTail(InputBuffer& buffer, std::string& lexeme) {
        if (!isInvalidTailStart(buffer.peek())) {
            return false;
        }

        while (isAlnumChar(buffer.peek()) || buffer.peek() == '_') {
            lexeme += buffer.advance();
        }
        return true;
    }

    static ProcessorResult invalidNumericResult(InputBuffer& buffer,
                                                const Position& start,
                                                const std::string& lexeme) {
        return diagnosticResult(DiagnosticCode::InvalidNumericLiteral,
                                lexeme,
                                start,
                                buffer.getCurrentPosition());
    }

    static ProcessorResult processPrefixedInteger(InputBuffer& buffer,
                                                  const Position& start,
                                                  std::string& lexeme,
                                                  bool hexadecimal) {
        lexeme += buffer.advance();
        lexeme += buffer.advance();

        while (isPrefixedDigit(buffer.peek(), hexadecimal) || buffer.peek() == '_') {
            lexeme += buffer.advance();
        }

        readIntegerSuffix(buffer, lexeme);
        if (readInvalidTail(buffer, lexeme)) {
            return invalidNumericResult(buffer, start, lexeme);
        }

        return tokenResult(TokenType::INTEGER_LITERAL, lexeme, start, buffer.getCurrentPosition());
    }

    static void readDigits(InputBuffer& buffer, std::string& lexeme) {
        while (isDigitChar(buffer.peek()) || buffer.peek() == '_') {
            lexeme += buffer.advance();
        }
    }

    static bool readFraction(InputBuffer& buffer, std::string& lexeme) {
        if (buffer.peek() == '.') {
            lexeme += buffer.advance();
            readDigits(buffer, lexeme);
            return true;
        }

        return false;
    }

    static bool readExponent(InputBuffer& buffer, std::string& lexeme) {
        if (buffer.peek() == 'e' || buffer.peek() == 'E') {
            lexeme += buffer.advance();

            if (buffer.peek() == '+' || buffer.peek() == '-') {
                lexeme += buffer.advance();
            }

            readDigits(buffer, lexeme);
            return true;
        }

        return false;
    }

    static bool readRealSuffix(InputBuffer& buffer, std::string& lexeme) {
        if (isRealSuffix(buffer.peek())) {
            lexeme += buffer.advance();
            return true;
        }

        return false;
    }

    static ProcessorResult processDecimal(InputBuffer& buffer,
                                          const Position& start,
                                          std::string& lexeme) {
        readDigits(buffer, lexeme);
        bool isReal = readFraction(buffer, lexeme);
        isReal = readExponent(buffer, lexeme) || isReal;

        if (readRealSuffix(buffer, lexeme)) {
            isReal = true;
        } else {
            readIntegerSuffix(buffer, lexeme);
        }

        if (readInvalidTail(buffer, lexeme)) {
            return invalidNumericResult(buffer, start, lexeme);
        }

        return tokenResult(isReal ? TokenType::REAL_LITERAL : TokenType::INTEGER_LITERAL,
                           lexeme,
                           start,
                           buffer.getCurrentPosition());
    }
};
