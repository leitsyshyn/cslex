#include "test_utils.h"

using namespace testing;

namespace {
struct TokenStringCase {
    TokenType type;
    const char* expected;
};

constexpr TokenStringCase kTokenStringCases[] = {
    {TokenType::INTEGER_LITERAL, "INTEGER_LITERAL"},
    {TokenType::REAL_LITERAL, "REAL_LITERAL"},
    {TokenType::CHARACTER_LITERAL, "CHARACTER_LITERAL"},
    {TokenType::STRING_LITERAL, "STRING_LITERAL"},
    {TokenType::VERBATIM_STRING, "VERBATIM_STRING"},
    {TokenType::INTERPOLATED_STRING, "INTERPOLATED_STRING"},
    {TokenType::KEYWORD, "KEYWORD"},
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::OPERATOR, "OPERATOR"},
    {TokenType::SEPARATOR, "SEPARATOR"},
    {TokenType::COMMENT, "COMMENT"},
    {TokenType::PREPROCESSOR, "PREPROCESSOR"},
    {TokenType::WHITESPACE, "WHITESPACE"},
    {TokenType::ERROR, "ERROR"},
    {TokenType::END_OF_FILE, "END_OF_FILE"},
};
} // namespace

class TokenStringTests : public TestWithParam<TokenStringCase> {
};

TEST_P(TokenStringTests, ConvertsTokenTypeToStableString) {
    const TokenStringCase testCase = GetParam();

    EXPECT_EQ(Token(testCase.type, "", {}, {}).tokenTypeToString(), testCase.expected);
}

INSTANTIATE_TEST_SUITE_P(AllTokenTypes,
                         TokenStringTests,
                         ValuesIn(kTokenStringCases),
                         [](const TestParamInfo<TokenStringCase>& info) {
                             return info.param.expected;
                         });

TEST(TokenHelpersTest, FormatsTokenStringWithTypeAndLexeme) {
    const Token token(TokenType::IDENTIFIER, "value", Position(0, 1, 1), Position(5, 1, 6));

    EXPECT_THAT(token.toString(), AllOf(
        HasSubstr("IDENTIFIER"),
        HasSubstr("value")));
}

TEST(PositionHelpersTest, UsesExpectedDefaultCoordinates) {
    const Position position;

    EXPECT_EQ(position.index, 0u);
    EXPECT_EQ(position.line, 1);
    EXPECT_EQ(position.col, 1);
}

TEST(PositionHelpersTest, FormatsPositionAsLineAndColumnString) {
    const Position position(7, 3, 9);

    EXPECT_EQ(position.toString(), "Line    3 Col   9");
}
