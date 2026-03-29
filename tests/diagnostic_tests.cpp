#include "test_utils.h"

using namespace testing;

TEST(DiagnosticHelpersTest, ConvertsDiagnosticCodesToStableStrings) {
    EXPECT_EQ(diagnosticCodeToString(DiagnosticCode::UnexpectedCharacter), "UnexpectedCharacter");
    EXPECT_EQ(diagnosticCodeToString(DiagnosticCode::InvalidNumericLiteral), "InvalidNumericLiteral");
    EXPECT_EQ(diagnosticCodeToString(DiagnosticCode::UnterminatedStringLiteral), "UnterminatedStringLiteral");
    EXPECT_EQ(diagnosticCodeToString(DiagnosticCode::UnterminatedCharacterLiteral), "UnterminatedCharacterLiteral");
    EXPECT_EQ(diagnosticCodeToString(DiagnosticCode::UnterminatedComment), "UnterminatedComment");
    EXPECT_EQ(diagnosticCodeToString(DiagnosticCode::UnterminatedVerbatimStringLiteral), "UnterminatedVerbatimStringLiteral");
    EXPECT_EQ(diagnosticCodeToString(DiagnosticCode::UnterminatedInterpolatedStringLiteral), "UnterminatedInterpolatedStringLiteral");
}

TEST(DiagnosticHelpersTest, BuildsDiagnosticMessagesForAllKnownCodes) {
    EXPECT_EQ(buildDiagnosticMessage(DiagnosticCode::UnexpectedCharacter, "@"), "Unexpected character sequence");
    EXPECT_EQ(buildDiagnosticMessage(DiagnosticCode::InvalidNumericLiteral, "123abc"), "Invalid numeric literal");
    EXPECT_EQ(buildDiagnosticMessage(DiagnosticCode::UnterminatedStringLiteral, "\"text"), "Unterminated string literal");
    EXPECT_EQ(buildDiagnosticMessage(DiagnosticCode::UnterminatedCharacterLiteral, "'a"), "Unterminated character literal");
    EXPECT_EQ(buildDiagnosticMessage(DiagnosticCode::UnterminatedComment, "/*"), "Unterminated comment");
    EXPECT_EQ(buildDiagnosticMessage(DiagnosticCode::UnterminatedVerbatimStringLiteral, "@\""), "Unterminated verbatim string literal");
    EXPECT_EQ(buildDiagnosticMessage(DiagnosticCode::UnterminatedInterpolatedStringLiteral, "$\""), "Unterminated interpolated string literal");
}

TEST(DiagnosticHelpersTest, FormatsDiagnosticStringWithLexeme) {
    const Diagnostic diagnostic = makeDiagnostic(
        DiagnosticCode::InvalidNumericLiteral,
        "123abc",
        Position(4, 2, 5),
        Position(10, 2, 11));

    EXPECT_THAT(diagnostic.toString(), AllOf(
        HasSubstr("InvalidNumericLiteral"),
        HasSubstr("Line    2 Col   5"),
        HasSubstr("Invalid numeric literal"),
        HasSubstr("[123abc]")));
}

TEST(DiagnosticHelpersTest, FormatsDiagnosticStringWithoutLexemeBracketsWhenLexemeIsEmpty) {
    const Diagnostic diagnostic(
        DiagnosticCode::UnexpectedCharacter,
        "Unexpected character sequence",
        "",
        Position(0, 1, 1),
        Position(1, 1, 2));

    EXPECT_THAT(diagnostic.toString(), AllOf(
        HasSubstr("UnexpectedCharacter"),
        HasSubstr("Unexpected character sequence"),
        Not(HasSubstr("["))));
}
