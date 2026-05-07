#include "test_utils.h"

#include "lexers/LexicalException.h"

using namespace testing;

class LexerTests : public TestWithParam<LexerKind> {
public:
    void SetUp() override {
        lexer = makeLexer(GetParam());
    }

    std::unique_ptr<ILexer> lexer;
};

// Basic tokenization and positions.

TEST_P(LexerTests, Basic_TokenizesVariableDeclarationWithoutDiagnostics) {
    const LexerResult result = lexer->tokenizeTolerant("int count = 42;");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(tokens, SizeIs(5));
    EXPECT_THAT(tokens, ElementsAre(
        TokenIs(TokenType::KEYWORD, "int"),
        TokenIs(TokenType::IDENTIFIER, "count"),
        TokenIs(TokenType::OPERATOR, "="),
        TokenIs(TokenType::INTEGER_LITERAL, "42"),
        TokenIs(TokenType::SEPARATOR, ";")));
}

TEST_P(LexerTests, Basic_TracksTokenPositionsAcrossLines) {
    const LexerResult result = lexer->tokenizeTolerant("int x = 1;\nvar y = 2;");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(tokens, SizeIs(10));

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

    ASSERT_THAT(result.tokens, Not(IsEmpty()));
    EXPECT_EQ(result.tokens.back().type, TokenType::END_OF_FILE);
    EXPECT_EQ(result.tokens.back().start.line, 2);
    EXPECT_EQ(result.tokens.back().start.col, 11);
}

TEST_P(LexerTests, Basic_DistinguishesKeywordsFromVerbatimIdentifiers) {
    const LexerResult result = lexer->tokenizeTolerant("class @class");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    EXPECT_THAT(tokens, ElementsAre(
        TokenIs(TokenType::KEYWORD, "class"),
        TokenIs(TokenType::IDENTIFIER, "@class")));
}

// Tolerant-mode diagnostics and recovery.

TEST_P(LexerTests, Diagnostics_CollectsAndContinuesInTolerantMode) {
    const LexerResult result = lexer->tokenizeTolerant("123abc + 5 @");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(tokens, SizeIs(2));
    EXPECT_THAT(tokens, ElementsAre(
        TokenIs(TokenType::OPERATOR, "+"),
        TokenIs(TokenType::INTEGER_LITERAL, "5")));

    ASSERT_THAT(result.diagnostics, SizeIs(2));
    EXPECT_THAT(result.diagnostics, ElementsAre(
        DiagnosticIs(DiagnosticCode::InvalidNumericLiteral, "123abc"),
        DiagnosticIs(DiagnosticCode::UnexpectedCharacter, "@")));
}

TEST_P(LexerTests, Diagnostics_ReportsStandaloneAtSymbolAsUnexpectedCharacter) {
    const LexerResult result = lexer->tokenizeTolerant("@");
    const auto tokens = nonEofTokens(result.tokens);

    EXPECT_THAT(tokens, IsEmpty());
    ASSERT_THAT(result.diagnostics, SizeIs(1));
    EXPECT_THAT(result.diagnostics[0], DiagnosticIs(DiagnosticCode::UnexpectedCharacter, "@"));
    EXPECT_EQ(result.diagnostics[0].end.col, 2);
}

TEST_P(LexerTests, Diagnostics_ReportsUnterminatedBlockComment) {
    const LexerResult result = lexer->tokenizeTolerant("/* comment");
    const auto tokens = nonEofTokens(result.tokens);

    EXPECT_THAT(tokens, IsEmpty());
    ASSERT_THAT(result.diagnostics, SizeIs(1));
    EXPECT_THAT(result.diagnostics[0], DiagnosticIs(DiagnosticCode::UnterminatedComment, "/* comment"));
    EXPECT_EQ(result.diagnostics[0].start.line, 1);
    EXPECT_EQ(result.diagnostics[0].start.col, 1);
}

// Operators and comments.

TEST_P(LexerTests, Operators_SplitsAdjacentOperatorsGreedily) {
    const LexerResult result = lexer->tokenizeTolerant("a+++++b");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    EXPECT_THAT(tokens, ElementsAre(
        TokenIs(TokenType::IDENTIFIER, "a"),
        TokenIs(TokenType::OPERATOR, "++"),
        TokenIs(TokenType::OPERATOR, "++"),
        TokenIs(TokenType::OPERATOR, "+"),
        TokenIs(TokenType::IDENTIFIER, "b")));
}

TEST_P(LexerTests, Comments_TokenizesSingleLineCommentAtEndOfFile) {
    const LexerResult result = lexer->tokenizeTolerant("// comment");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(tokens, SizeIs(1));
    EXPECT_THAT(tokens[0], TokenIs(TokenType::COMMENT, "// comment"));
}

TEST_P(LexerTests, Comments_TokenizesMultilineBlockCommentAsSingleToken) {
    const LexerResult result = lexer->tokenizeTolerant("/* first\nsecond */");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(tokens, SizeIs(1));
    EXPECT_THAT(tokens[0], TokenIs(TokenType::COMMENT, "/* first\nsecond */"));
}

// Numeric literals.

TEST_P(LexerTests, Numbers_TokenizesBinaryIntegerLiteralWithUnsignedLongSuffix) {
    const LexerResult result = lexer->tokenizeTolerant("0b1010UL");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(tokens, SizeIs(1));
    EXPECT_THAT(tokens[0], TokenIs(TokenType::INTEGER_LITERAL, "0b1010UL"));
}

TEST_P(LexerTests, Numbers_RejectsBinaryIntegerLiteralWithTrailingIdentifierCharacters) {
    const LexerResult result = lexer->tokenizeTolerant("0B10LUx");
    const auto tokens = nonEofTokens(result.tokens);

    EXPECT_THAT(tokens, IsEmpty());
    ASSERT_THAT(result.diagnostics, SizeIs(1));
    EXPECT_THAT(result.diagnostics[0], DiagnosticIs(DiagnosticCode::InvalidNumericLiteral, "0B10LUx"));
}

TEST_P(LexerTests, Numbers_TokenizesHexIntegerLiteralWithSeparatorsAndSuffix) {
    const LexerResult result = lexer->tokenizeTolerant("0xDEAD_BEEFul");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(tokens, SizeIs(1));
    EXPECT_THAT(tokens[0], TokenIs(TokenType::INTEGER_LITERAL, "0xDEAD_BEEFul"));
}

TEST_P(LexerTests, Numbers_RejectsHexIntegerLiteralWithAlphabeticTail) {
    const LexerResult result = lexer->tokenizeTolerant("0x1G");
    const auto tokens = nonEofTokens(result.tokens);

    EXPECT_THAT(tokens, IsEmpty());
    ASSERT_THAT(result.diagnostics, SizeIs(1));
    EXPECT_THAT(result.diagnostics[0], DiagnosticIs(DiagnosticCode::InvalidNumericLiteral, "0x1G"));
}

TEST_P(LexerTests, Numbers_TokenizesRealLiteralWithExponentAndSuffix) {
    const LexerResult result = lexer->tokenizeTolerant("12.34e-5F");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(tokens, SizeIs(1));
    EXPECT_THAT(tokens[0], TokenIs(TokenType::REAL_LITERAL, "12.34e-5F"));
}

TEST_P(LexerTests, Numbers_TokenizesIntegerLiteralWithLongUnsignedSuffixOrder) {
    const LexerResult result = lexer->tokenizeTolerant("42LU");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(tokens, SizeIs(1));
    EXPECT_THAT(tokens[0], TokenIs(TokenType::INTEGER_LITERAL, "42LU"));
}

TEST_P(LexerTests, Numbers_RejectsRealLiteralWithTrailingIdentifierCharacters) {
    const LexerResult result = lexer->tokenizeTolerant("1.0mExtra");
    const auto tokens = nonEofTokens(result.tokens);

    EXPECT_THAT(tokens, IsEmpty());
    ASSERT_THAT(result.diagnostics, SizeIs(1));
    EXPECT_THAT(result.diagnostics[0], DiagnosticIs(DiagnosticCode::InvalidNumericLiteral, "1.0mExtra"));
}

// Character literals.

TEST_P(LexerTests, Characters_TokenizesEscapedCharacterLiteral) {
    const LexerResult result = lexer->tokenizeTolerant("'\\''");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(tokens, SizeIs(1));
    EXPECT_THAT(tokens[0], TokenIs(TokenType::CHARACTER_LITERAL, "'\\''"));
}

TEST_P(LexerTests, Characters_ReportsUnterminatedCharacterLiteralAtNewline) {
    const LexerResult result = lexer->tokenizeTolerant("'a\n");
    const auto tokens = nonEofTokens(result.tokens);

    EXPECT_THAT(tokens, IsEmpty());
    ASSERT_THAT(result.diagnostics, SizeIs(1));
    EXPECT_THAT(result.diagnostics[0], DiagnosticIs(DiagnosticCode::UnterminatedCharacterLiteral, "'a"));
}

// String and interpolated-string literals.

TEST_P(LexerTests, Strings_TokenizesVerbatimStringWithEscapedQuotes) {
    const LexerResult result = lexer->tokenizeTolerant(R"(@"a ""quoted"" value")");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(tokens, SizeIs(1));
    EXPECT_THAT(tokens[0], TokenIs(TokenType::VERBATIM_STRING, R"(@"a ""quoted"" value")"));
}

TEST_P(LexerTests, Strings_ReportsUnterminatedVerbatimStringLiteral) {
    const LexerResult result = lexer->tokenizeTolerant("@\"unterminated");
    const auto tokens = nonEofTokens(result.tokens);

    EXPECT_THAT(tokens, IsEmpty());
    ASSERT_THAT(result.diagnostics, SizeIs(1));
    EXPECT_THAT(result.diagnostics[0], DiagnosticIs(DiagnosticCode::UnterminatedVerbatimStringLiteral, "@\"unterminated"));
}

TEST_P(LexerTests, Strings_TokenizesInterpolatedStringWithEmbeddedExpression) {
    const LexerResult result = lexer->tokenizeTolerant("$\"value {items[0]}\"");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(tokens, SizeIs(1));
    EXPECT_THAT(tokens[0], TokenIs(TokenType::INTERPOLATED_STRING, "$\"value {items[0]}\""));
}

TEST_P(LexerTests, Strings_TokenizesInterpolatedStringWithEscapedBracesAndNestedExpressionBraces) {
    const LexerResult result = lexer->tokenizeTolerant("$\"a {{ literal }} {x + {y}}\"");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(tokens, SizeIs(1));
    EXPECT_THAT(tokens[0], TokenIs(TokenType::INTERPOLATED_STRING, "$\"a {{ literal }} {x + {y}}\""));
}

TEST_P(LexerTests, Strings_TokenizesInterpolatedStringWithEscapedCharacters) {
    const LexerResult result = lexer->tokenizeTolerant(R"($"c:\\temp\\{file}")");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(tokens, SizeIs(1));
    EXPECT_THAT(tokens[0], TokenIs(TokenType::INTERPOLATED_STRING, R"($"c:\\temp\\{file}")"));
}

TEST_P(LexerTests, Strings_TokenizesVerbatimInterpolatedStringWithDoubledQuotes) {
    const LexerResult result = lexer->tokenizeTolerant(R"CSLEX($@"say ""{name}""")CSLEX");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(result.diagnostics, IsEmpty());
    ASSERT_THAT(tokens, SizeIs(1));
    EXPECT_THAT(tokens[0], TokenIs(TokenType::INTERPOLATED_STRING, R"CSLEX($@"say ""{name}""")CSLEX"));
}

TEST_P(LexerTests, Strings_ReportsUnterminatedInterpolatedStringWithOpenInterpolation) {
    const LexerResult result = lexer->tokenizeTolerant("$\"{value");
    const auto tokens = nonEofTokens(result.tokens);

    EXPECT_THAT(tokens, IsEmpty());
    ASSERT_THAT(result.diagnostics, SizeIs(1));
    EXPECT_THAT(result.diagnostics[0], DiagnosticIs(DiagnosticCode::UnterminatedInterpolatedStringLiteral, "$\"{value"));
}

TEST_P(LexerTests, Strings_ReportsUnterminatedInterpolatedStringWhenQuoteDoesNotFollowDollarAt) {
    const LexerResult result = lexer->tokenizeTolerant("$@foo");
    const auto tokens = nonEofTokens(result.tokens);

    ASSERT_THAT(tokens, SizeIs(1));
    EXPECT_THAT(tokens[0], TokenIs(TokenType::IDENTIFIER, "foo"));
    ASSERT_THAT(result.diagnostics, SizeIs(1));
    EXPECT_THAT(result.diagnostics[0], DiagnosticIs(DiagnosticCode::UnterminatedInterpolatedStringLiteral, "$@"));
}

// Strict mode.

TEST_P(LexerTests, StrictMode_ThrowsLexicalExceptionForInvalidNumericLiteral) {
    EXPECT_THROW(lexer->tokenizeStrict("123abc"), LexicalException);
}

TEST_P(LexerTests, StrictMode_PreservesDiagnosticDetailsInThrownException) {
    try {
        (void)lexer->tokenizeStrict("\"unterminated");
        FAIL() << "Expected LexicalException";
    } catch (const LexicalException& exception) {
        EXPECT_EQ(exception.getDiagnostic().code, DiagnosticCode::UnterminatedStringLiteral);
        EXPECT_EQ(exception.getDiagnostic().lexeme, "\"unterminated");
        EXPECT_EQ(exception.getDiagnostic().start.line, 1);
        EXPECT_EQ(exception.getDiagnostic().start.col, 1);
        EXPECT_THAT(std::string(exception.what()), AllOf(
            HasSubstr("UnterminatedStringLiteral"),
            HasSubstr("Line"),
            HasSubstr("\"unterminated")));
    }
}

INSTANTIATE_TEST_SUITE_P(AllLexers,
                         LexerTests,
                         Values(LexerKind::Fsm, LexerKind::Regex),
                         [](const TestParamInfo<LexerKind>& info) {
                             return lexerKindName(info.param);
                         });
