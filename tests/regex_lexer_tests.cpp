#include "test_utils.h"

using namespace testing;

class RegexLexerTest : public Test {
protected:
    RegexLexer lexer;
};

TEST_F(RegexLexerTest, StaticHelperMatchesInstanceTokenization) {
    const std::string source = "#define DEBUG\nvalue";

    const LexerResult fromInstance = lexer.tokenizeTolerant(source);
    const LexerResult fromStatic = RegexLexer::tokenizeSource(source);

    EXPECT_EQ(snapshotTokens(fromInstance), snapshotTokens(fromStatic));
    EXPECT_EQ(snapshotDiagnostics(fromInstance), snapshotDiagnostics(fromStatic));
}

TEST_F(RegexLexerTest, TokenizesMultilineCommentsAsSingleToken) {
    const LexerResult result = lexer.tokenizeTolerant("/* first\nsecond */");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_TRUE(result.diagnostics.empty());
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type, TokenType::COMMENT);
    EXPECT_EQ(tokens[0].lexeme, "/* first\nsecond */");
}

TEST_F(RegexLexerTest, ReportsStandaloneAtSymbolAsUnexpectedCharacter) {
    const LexerResult result = lexer.tokenizeTolerant("@");
    const auto tokens = nonEofTokens(result.tokens);

    EXPECT_TRUE(tokens.empty());
    ASSERT_EQ(result.diagnostics.size(), 1u);
    EXPECT_EQ(result.diagnostics[0].code, DiagnosticCode::UnexpectedCharacter);
    EXPECT_EQ(result.diagnostics[0].lexeme, "@");
    EXPECT_EQ(result.diagnostics[0].end.col, 2);
}
