#include "app/LexerComparisonApp.h"

#include "io/ISourceRepository.h"
#include "lexers/LexicalException.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace testing;

namespace {
class MockSourceRepository : public ISourceRepository {
public:
    MOCK_METHOD(std::vector<std::string>, resolveInputs, (const std::vector<std::string>& inputs), (override));
    MOCK_METHOD(SourceDocument, load, (const std::string& path), (override));
};

class MockLexer : public ILexer {
public:
    MOCK_METHOD(LexerResult, tokenize, (const std::string& source, ErrorMode errorMode), (override));
};

SourceDocument makeDocument(const std::string& path, const std::string& text) {
    return SourceDocument(path, text);
}

Token makeToken(TokenType type,
                const std::string& lexeme,
                std::size_t startIndex,
                std::size_t endIndex,
                int line = 1,
                int startColumn = 1) {
    return Token(type,
                 lexeme,
                 Position(startIndex, line, startColumn),
                 Position(endIndex, line, startColumn + static_cast<int>(endIndex - startIndex)));
}

LexerResult makeLexerResult(const std::vector<Token>& tokens,
                            const std::vector<Diagnostic>& diagnostics = {}) {
    LexerResult result;
    result.tokens = tokens;
    result.diagnostics = diagnostics;

    Position eofPosition;
    if (!tokens.empty()) {
        eofPosition = tokens.back().end;
    }
    result.tokens.push_back(Token(TokenType::END_OF_FILE, "", eofPosition, eofPosition));
    return result;
}
} // namespace

TEST(LexerComparisonAppTests, Run_ResolvesBatchInputsAndInvokesLexersInOrder) {
    StrictMock<MockSourceRepository> repository;
    StrictMock<MockLexer> fsmLexer;
    StrictMock<MockLexer> regexLexer;
    LexerComparisonApp app(repository, fsmLexer, regexLexer);
    std::ostringstream out;
    std::ostringstream err;

    const SourceDocument firstDocument = makeDocument("tests/a.cs", "int a = 1;");
    const SourceDocument secondDocument = makeDocument("tests/b.cs", "int b = 2;");
    const LexerResult firstResult = makeLexerResult({makeToken(TokenType::KEYWORD, "int", 0, 3)});
    const LexerResult secondResult = makeLexerResult({makeToken(TokenType::KEYWORD, "int", 0, 3)});

    {
        InSequence sequence;

        EXPECT_CALL(repository, resolveInputs(ElementsAre("batch")))
            .Times(1)
            .WillOnce(Return(std::vector<std::string>{"tests/a.cs", "tests/b.cs"}));
        EXPECT_CALL(repository, load("tests/a.cs"))
            .Times(1)
            .WillOnce(Return(firstDocument));
        EXPECT_CALL(fsmLexer, tokenize(firstDocument.text, ErrorMode::Collect))
            .Times(1)
            .WillOnce(Return(firstResult));
        EXPECT_CALL(regexLexer, tokenize(firstDocument.text, ErrorMode::Collect))
            .Times(1)
            .WillOnce(Return(firstResult));
        EXPECT_CALL(repository, load("tests/b.cs"))
            .Times(1)
            .WillOnce(Return(secondDocument));
        EXPECT_CALL(fsmLexer, tokenize(secondDocument.text, ErrorMode::Collect))
            .Times(1)
            .WillOnce(Return(secondResult));
        EXPECT_CALL(regexLexer, tokenize(secondDocument.text, ErrorMode::Collect))
            .Times(1)
            .WillOnce(Return(secondResult));
    }

    EXPECT_EQ(app.run(std::vector<std::string>{"batch"}, ErrorMode::Collect, out, err), 0);
    EXPECT_THAT(err.str(), IsEmpty());
    EXPECT_THAT(out.str(), AllOf(
        HasSubstr("Analyzing file: tests/a.cs"),
        HasSubstr("Analyzing file: tests/b.cs"),
        HasSubstr("Batch Summary"),
        HasSubstr("Exact matches: 2"),
        HasSubstr("Files with differences: 0"),
        HasSubstr("Files with failures: 0")));
}

TEST(LexerComparisonAppTests, Run_ContinuesAfterLoadFailureAndReportsBatchSummary) {
    StrictMock<MockSourceRepository> repository;
    StrictMock<MockLexer> fsmLexer;
    StrictMock<MockLexer> regexLexer;
    LexerComparisonApp app(repository, fsmLexer, regexLexer);
    std::ostringstream out;
    std::ostringstream err;

    const SourceDocument validDocument = makeDocument("tests/demo_valid.cs", "int count = 42;");
    const LexerResult validResult = makeLexerResult({makeToken(TokenType::KEYWORD, "int", 0, 3)});

    {
        InSequence sequence;

        EXPECT_CALL(repository, resolveInputs(ElementsAre("batch")))
            .Times(1)
            .WillOnce(Return(std::vector<std::string>{"missing.cs", "tests/demo_valid.cs"}));
        EXPECT_CALL(repository, load("missing.cs"))
            .Times(1)
            .WillOnce(Throw(std::runtime_error("disk failure")));
        EXPECT_CALL(repository, load("tests/demo_valid.cs"))
            .Times(1)
            .WillOnce(Return(validDocument));
        EXPECT_CALL(fsmLexer, tokenize(validDocument.text, ErrorMode::Collect))
            .Times(1)
            .WillOnce(Return(validResult));
        EXPECT_CALL(regexLexer, tokenize(validDocument.text, ErrorMode::Collect))
            .Times(1)
            .WillOnce(Return(validResult));
    }

    EXPECT_EQ(app.run(std::vector<std::string>{"batch"}, ErrorMode::Collect, out, err), 1);
    EXPECT_THAT(err.str(), HasSubstr("Error loading 'missing.cs': disk failure"));
    EXPECT_THAT(out.str(), AllOf(
        HasSubstr("Analyzing file: tests/demo_valid.cs"),
        HasSubstr("Batch Summary"),
        HasSubstr("Exact matches: 1"),
        HasSubstr("Files with failures: 1")));
}

TEST(LexerComparisonAppTests, Run_ReportsTokenMismatchBetweenLexers) {
    StrictMock<MockSourceRepository> repository;
    StrictMock<MockLexer> fsmLexer;
    StrictMock<MockLexer> regexLexer;
    LexerComparisonApp app(repository, fsmLexer, regexLexer);
    std::ostringstream out;
    std::ostringstream err;

    const SourceDocument document = makeDocument("tests/demo_valid.cs", "value");
    const LexerResult fsmResult = makeLexerResult({makeToken(TokenType::IDENTIFIER, "value", 0, 5)});
    const LexerResult regexResult = makeLexerResult({makeToken(TokenType::KEYWORD, "value", 0, 5)});

    EXPECT_CALL(repository, resolveInputs(ElementsAre("tests/demo_valid.cs")))
        .Times(1)
        .WillOnce(Return(std::vector<std::string>{"tests/demo_valid.cs"}));
    EXPECT_CALL(repository, load("tests/demo_valid.cs"))
        .Times(1)
        .WillOnce(Return(document));
    EXPECT_CALL(fsmLexer, tokenize(document.text, ErrorMode::Collect))
        .Times(1)
        .WillOnce(Return(fsmResult));
    EXPECT_CALL(regexLexer, tokenize(document.text, ErrorMode::Collect))
        .Times(1)
        .WillOnce(Return(regexResult));

    EXPECT_EQ(app.run(std::vector<std::string>{"tests/demo_valid.cs"}, ErrorMode::Collect, out, err), 1);
    EXPECT_THAT(err.str(), IsEmpty());
    EXPECT_THAT(out.str(), AllOf(
        HasSubstr("Outputs differ"),
        HasSubstr("First token mismatch at index 0"),
        HasSubstr("FSM: Line    1 Col   1 IDENTIFIER"),
        HasSubstr("Regex: Line    1 Col   1 KEYWORD")));
}

TEST(LexerComparisonAppTests, Run_PrintsDiagnosticSnippetsForReportedDiagnostics) {
    StrictMock<MockSourceRepository> repository;
    StrictMock<MockLexer> fsmLexer;
    StrictMock<MockLexer> regexLexer;
    LexerComparisonApp app(repository, fsmLexer, regexLexer);
    std::ostringstream out;
    std::ostringstream err;

    const SourceDocument document = makeDocument("tests/demo_invalid.cs", "int bad = 123abc;");
    const Diagnostic diagnostic = makeDiagnostic(
        DiagnosticCode::InvalidNumericLiteral,
        "123abc",
        Position(10, 1, 11),
        Position(16, 1, 17));
    const LexerResult result = makeLexerResult({}, {diagnostic});

    EXPECT_CALL(repository, resolveInputs(ElementsAre("tests/demo_invalid.cs")))
        .Times(1)
        .WillOnce(Return(std::vector<std::string>{"tests/demo_invalid.cs"}));
    EXPECT_CALL(repository, load("tests/demo_invalid.cs"))
        .Times(1)
        .WillOnce(Return(document));
    EXPECT_CALL(fsmLexer, tokenize(document.text, ErrorMode::Collect))
        .Times(1)
        .WillOnce(Return(result));
    EXPECT_CALL(regexLexer, tokenize(document.text, ErrorMode::Collect))
        .Times(1)
        .WillOnce(Return(result));

    EXPECT_EQ(app.run(std::vector<std::string>{"tests/demo_invalid.cs"}, ErrorMode::Collect, out, err), 0);
    EXPECT_THAT(err.str(), IsEmpty());
    EXPECT_THAT(out.str(), AllOf(
        HasSubstr("Invalid numeric literal [123abc]"),
        HasSubstr("int bad = 123abc;"),
        HasSubstr("          ^^^^^^")));
}

TEST(LexerComparisonAppTests, Run_ReportsFsmLexerExceptionAndSkipsRegexLexerForThatFile) {
    StrictMock<MockSourceRepository> repository;
    StrictMock<MockLexer> fsmLexer;
    StrictMock<MockLexer> regexLexer;
    LexerComparisonApp app(repository, fsmLexer, regexLexer);
    std::ostringstream out;
    std::ostringstream err;

    const SourceDocument document = makeDocument("tests/demo_invalid.cs", "123abc");
    const LexicalException exception(makeDiagnostic(
        DiagnosticCode::InvalidNumericLiteral,
        "123abc",
        Position(0, 1, 1),
        Position(6, 1, 7)));

    EXPECT_CALL(repository, resolveInputs(ElementsAre("tests/demo_invalid.cs")))
        .Times(1)
        .WillOnce(Return(std::vector<std::string>{"tests/demo_invalid.cs"}));
    EXPECT_CALL(repository, load("tests/demo_invalid.cs"))
        .Times(1)
        .WillOnce(Return(document));
    EXPECT_CALL(fsmLexer, tokenize(document.text, ErrorMode::Throw))
        .Times(1)
        .WillOnce(Throw(exception));
    EXPECT_CALL(regexLexer, tokenize(_, _)).Times(0);

    EXPECT_EQ(app.run(std::vector<std::string>{"tests/demo_invalid.cs"}, ErrorMode::Throw, out, err), 1);
    EXPECT_THAT(out.str(), AllOf(
        HasSubstr("Analyzing file: tests/demo_invalid.cs"),
        HasSubstr("FSM Lexer"),
        Not(HasSubstr("Regex Lexer"))));
    EXPECT_THAT(err.str(), HasSubstr("FSM lexical error in 'tests/demo_invalid.cs': InvalidNumericLiteral"));
}
