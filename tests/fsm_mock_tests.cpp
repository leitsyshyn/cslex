#include "lexers/LexicalException.h"
#include "lexers/fsm/FSMLexer.h"
#include "lexers/fsm/processors/IProcessor.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

using namespace testing;

namespace {
class MockProcessor : public IProcessor {
public:
    MOCK_METHOD(ProcessorResult, process, (InputBuffer& buffer), (override));
};

ProcessorResult noMatch() {
    return ProcessorResult{};
}

ProcessorResult tokenResult(TokenType type,
                            const std::string& lexeme,
                            const Position& start,
                            const Position& end) {
    ProcessorResult result;
    result.matched = true;
    result.token = Token(type, lexeme, start, end);
    return result;
}

ProcessorResult diagnosticResult(DiagnosticCode code,
                                 const std::string& lexeme,
                                 const Position& start,
                                 const Position& end) {
    ProcessorResult result;
    result.matched = true;
    result.diagnostic = makeDiagnostic(code, lexeme, start, end);
    return result;
}
} // namespace

TEST(FSMLexerMockTests, Tokenize_QueriesProcessorsInOrderUntilOneMatches) {
    auto first = std::make_unique<StrictMock<MockProcessor>>();
    auto second = std::make_unique<StrictMock<MockProcessor>>();
    auto third = std::make_unique<StrictMock<MockProcessor>>();
    auto* firstPtr = first.get();
    auto* secondPtr = second.get();
    auto* thirdPtr = third.get();

    std::vector<std::unique_ptr<IProcessor>> processors;
    processors.push_back(std::move(first));
    processors.push_back(std::move(second));
    processors.push_back(std::move(third));
    FSMLexer lexer(std::move(processors));

    {
        InSequence sequence;

        EXPECT_CALL(*firstPtr, process(_))
            .Times(1)
            .WillOnce(Return(noMatch()));
        EXPECT_CALL(*secondPtr, process(Truly([](InputBuffer& buffer) {
                        return buffer.peek() == '+' && buffer.getCurrentPosition().index == 0u;
                    })))
            .Times(1)
            .WillOnce(Invoke([](InputBuffer& buffer) {
                const Position start = buffer.getCurrentPosition();
                const std::string lexeme(1, buffer.advance());
                const Position end = buffer.getCurrentPosition();
                return tokenResult(TokenType::OPERATOR, lexeme, start, end);
            }));
    }
    EXPECT_CALL(*thirdPtr, process(_)).Times(0);

    const LexerResult result = lexer.tokenize("+", ErrorMode::Collect);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(result.tokens, SizeIs(2));
    EXPECT_EQ(result.tokens[0].type, TokenType::OPERATOR);
    EXPECT_EQ(result.tokens[0].lexeme, "+");
    EXPECT_EQ(result.tokens[1].type, TokenType::END_OF_FILE);
}

TEST(FSMLexerMockTests, Tokenize_ThrowsWhenProcessorProducesDiagnosticInStrictMode) {
    auto processor = std::make_unique<StrictMock<MockProcessor>>();
    auto* processorPtr = processor.get();

    std::vector<std::unique_ptr<IProcessor>> processors;
    processors.push_back(std::move(processor));
    FSMLexer lexer(std::move(processors));

    EXPECT_CALL(*processorPtr, process(_))
        .Times(1)
        .WillOnce(Invoke([](InputBuffer& buffer) {
            const Position start = buffer.getCurrentPosition();
            const std::string lexeme(1, buffer.advance());
            const Position end = buffer.getCurrentPosition();
            return diagnosticResult(DiagnosticCode::UnexpectedCharacter, lexeme, start, end);
        }));

    try {
        (void)lexer.tokenize("@", ErrorMode::Throw);
        FAIL() << "Expected LexicalException";
    } catch (const LexicalException& exception) {
        EXPECT_EQ(exception.getDiagnostic().code, DiagnosticCode::UnexpectedCharacter);
        EXPECT_EQ(exception.getDiagnostic().lexeme, "@");
    }
}

TEST(FSMLexerMockTests, Tokenize_UsesDifferentResponsesForSuccessiveCalls) {
    auto processor = std::make_unique<StrictMock<MockProcessor>>();
    auto* processorPtr = processor.get();

    std::vector<std::unique_ptr<IProcessor>> processors;
    processors.push_back(std::move(processor));
    FSMLexer lexer(std::move(processors));

    EXPECT_CALL(*processorPtr, process(_))
        .Times(2)
        .WillOnce(Invoke([](InputBuffer& buffer) {
            const Position start = buffer.getCurrentPosition();
            const std::string lexeme(1, buffer.advance());
            const Position end = buffer.getCurrentPosition();
            return tokenResult(TokenType::IDENTIFIER, lexeme, start, end);
        }))
        .WillOnce(Invoke([](InputBuffer& buffer) {
            const Position start = buffer.getCurrentPosition();
            const std::string lexeme(1, buffer.advance());
            const Position end = buffer.getCurrentPosition();
            return tokenResult(TokenType::OPERATOR, lexeme, start, end);
        }));

    const LexerResult result = lexer.tokenize("a+", ErrorMode::Collect);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(result.tokens, SizeIs(3));
    EXPECT_EQ(result.tokens[0].type, TokenType::IDENTIFIER);
    EXPECT_EQ(result.tokens[0].lexeme, "a");
    EXPECT_EQ(result.tokens[1].type, TokenType::OPERATOR);
    EXPECT_EQ(result.tokens[1].lexeme, "+");
    EXPECT_EQ(result.tokens[2].type, TokenType::END_OF_FILE);
}
