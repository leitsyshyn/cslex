#include "test_utils.h"

#include "lexers/LexicalException.h"

using namespace testing;

class LexerStrictModeTest : public TestWithParam<LexerKind> {
protected:
    void SetUp() override {
        lexer = makeLexer(GetParam());
    }

    std::unique_ptr<ILexer> lexer;
};

TEST_P(LexerStrictModeTest, ThrowsLexicalExceptionForInvalidNumericLiteral) {
    EXPECT_THROW(lexer->tokenizeStrict("123abc"), LexicalException);
}

TEST_P(LexerStrictModeTest, PreservesDiagnosticDetailsInStrictMode) {
    try {
        (void)lexer->tokenizeStrict("\"unterminated");
        FAIL() << "Expected LexicalException";
    } catch (const LexicalException& exception) {
        const std::string message = exception.what();
        EXPECT_EQ(exception.getDiagnostic().code, DiagnosticCode::UnterminatedStringLiteral);
        EXPECT_EQ(exception.getDiagnostic().lexeme, "\"unterminated");
        EXPECT_EQ(exception.getDiagnostic().start.line, 1);
        EXPECT_EQ(exception.getDiagnostic().start.col, 1);
        EXPECT_NE(message.find("UnterminatedStringLiteral"), std::string::npos);
        EXPECT_NE(message.find("Line"), std::string::npos);
        EXPECT_NE(message.find("\"unterminated"), std::string::npos);
    }
}

INSTANTIATE_TEST_SUITE_P(AllLexers,
                         LexerStrictModeTest,
                         Values(LexerKind::Fsm, LexerKind::Regex),
                         [](const TestParamInfo<LexerKind>& info) {
                             return lexerKindName(info.param);
                         });
