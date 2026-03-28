#include "test_utils.h"

using namespace testing;

class LexerContractTest : public TestWithParam<LexerKind> {
protected:
    void SetUp() override {
        lexer = makeLexer(GetParam());
    }

    std::unique_ptr<ILexer> lexer;
};

TEST_P(LexerContractTest, TokenizesVariableDeclarationWithoutDiagnostics) {
    const LexerResult result = lexer->tokenizeTolerant("int count = 42;");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_TRUE(result.diagnostics.empty());
    ASSERT_EQ(tokens.size(), 5u);
    EXPECT_TRUE(TokensMatch(tokens, {
        {TokenType::KEYWORD, "int"},
        {TokenType::IDENTIFIER, "count"},
        {TokenType::OPERATOR, "="},
        {TokenType::INTEGER_LITERAL, "42"},
        {TokenType::SEPARATOR, ";"},
    }));
}

TEST_P(LexerContractTest, TracksTokenPositionsAcrossLines) {
    const LexerResult result = lexer->tokenizeTolerant("int x = 1;\nvar y = 2;");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_TRUE(result.diagnostics.empty());
    ASSERT_EQ(tokens.size(), 10u);

    EXPECT_EQ(tokens[0].start.index, 0u);
    EXPECT_EQ(tokens[0].start.line, 1);
    EXPECT_EQ(tokens[0].start.col, 1);
    EXPECT_EQ(tokens[0].end.index, 3u);
    EXPECT_EQ(tokens[0].end.line, 1);
    EXPECT_EQ(tokens[0].end.col, 4);

    EXPECT_EQ(tokens[5].lexeme, "var");
    EXPECT_EQ(tokens[5].start.line, 2);
    EXPECT_EQ(tokens[5].start.col, 1);
    EXPECT_EQ(tokens[8].lexeme, "2");
    EXPECT_EQ(tokens[8].start.line, 2);
    EXPECT_EQ(tokens[8].start.col, 9);

    ASSERT_FALSE(result.tokens.empty());
    EXPECT_EQ(result.tokens.back().type, TokenType::END_OF_FILE);
    EXPECT_EQ(result.tokens.back().start.line, 2);
    EXPECT_EQ(result.tokens.back().start.col, 11);
}

TEST_P(LexerContractTest, DistinguishesKeywordsFromVerbatimIdentifiers) {
    const LexerResult result = lexer->tokenizeTolerant("class @class");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_TRUE(result.diagnostics.empty());
    EXPECT_TRUE(TokensMatch(tokens, {
        {TokenType::KEYWORD, "class"},
        {TokenType::IDENTIFIER, "@class"},
    }));
}

TEST_P(LexerContractTest, CollectsDiagnosticsAndContinuesInTolerantMode) {
    const LexerResult result = lexer->tokenizeTolerant("123abc + 5 @");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_TRUE(TokensMatch(tokens, {
        {TokenType::OPERATOR, "+"},
        {TokenType::INTEGER_LITERAL, "5"},
    }));

    ASSERT_EQ(result.diagnostics.size(), 2u);
    EXPECT_TRUE(DiagnosticsMatch(result.diagnostics, {
        {DiagnosticCode::InvalidNumericLiteral, "123abc"},
        {DiagnosticCode::UnexpectedCharacter, "@"},
    }));
}

INSTANTIATE_TEST_SUITE_P(AllLexers,
                         LexerContractTest,
                         Values(LexerKind::Fsm, LexerKind::Regex),
                         [](const TestParamInfo<LexerKind>& info) {
                             return lexerKindName(info.param);
                         });
