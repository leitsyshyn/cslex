#include "test_utils.h"

using namespace testing;

class FSMLexerTest : public Test {
protected:
    FSMLexer lexer;
};

TEST_F(FSMLexerTest, SplitsAdjacentOperatorsGreedily) {
    const LexerResult result = lexer.tokenizeTolerant("a+++++b");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_TRUE(result.diagnostics.empty());
    EXPECT_TRUE(TokensMatch(tokens, {
        {TokenType::IDENTIFIER, "a"},
        {TokenType::OPERATOR, "++"},
        {TokenType::OPERATOR, "++"},
        {TokenType::OPERATOR, "+"},
        {TokenType::IDENTIFIER, "b"},
    }));
}

TEST_F(FSMLexerTest, TokenizesInterpolatedStringsWithEmbeddedExpressions) {
    const LexerResult result = lexer.tokenizeTolerant("$\"value {items[0]}\"");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_TRUE(result.diagnostics.empty());
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type, TokenType::INTERPOLATED_STRING);
    EXPECT_EQ(tokens[0].lexeme, "$\"value {items[0]}\"");
}

TEST_F(FSMLexerTest, ReportsUnterminatedBlockCommentsAsDiagnostics) {
    const LexerResult result = lexer.tokenizeTolerant("/* comment");
    const auto tokens = nonEofTokens(result.tokens);

    EXPECT_TRUE(tokens.empty());
    ASSERT_EQ(result.diagnostics.size(), 1u);
    EXPECT_EQ(result.diagnostics[0].code, DiagnosticCode::UnterminatedComment);
    EXPECT_EQ(result.diagnostics[0].lexeme, "/* comment");
    EXPECT_EQ(result.diagnostics[0].start.line, 1);
    EXPECT_EQ(result.diagnostics[0].start.col, 1);
}
