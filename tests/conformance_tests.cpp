#include "test_utils.h"

#include <ostream>
#include <string>

using namespace testing;

struct ConformanceCase {
    std::string name;
    std::string source;
};

inline std::ostream& operator<<(std::ostream& os, const ConformanceCase& testCase) {
    os << testCase.name;
    return os;
}

class LexerConformanceTest : public TestWithParam<ConformanceCase> {
};

TEST_P(LexerConformanceTest, FsmAndRegexProduceTheSameOutput) {
    FSMLexer fsmLexer;
    RegexLexer regexLexer;

    const auto& source = GetParam().source;
    const LexerResult fsmResult = fsmLexer.tokenizeTolerant(source);
    const LexerResult regexResult = regexLexer.tokenizeTolerant(source);

    EXPECT_EQ(snapshotTokens(fsmResult), snapshotTokens(regexResult));
    EXPECT_EQ(snapshotDiagnostics(fsmResult), snapshotDiagnostics(regexResult));
}

INSTANTIATE_TEST_SUITE_P(
    RepresentativeInputs,
    LexerConformanceTest,
    Values(
        ConformanceCase{"SimpleDeclaration", "int value = 42;"},
        ConformanceCase{"NullConditional", "x?.ToString() ?? \"null\";"},
        ConformanceCase{"InterpolatedString", "$\"hello {name}\""},
        ConformanceCase{"InvalidNumericLiteral", "123abc + 5"},
        ConformanceCase{"UnterminatedComment", "/* unterminated"},
        ConformanceCase{"PreprocessorDirective", "#define DEBUG\nvalue"}),
    [](const TestParamInfo<ConformanceCase>& info) {
        return info.param.name;
    });
