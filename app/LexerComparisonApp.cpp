#include "LexerComparisonApp.h"

#include "lexers/LexicalException.h"

#include <algorithm>
#include <chrono>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

namespace {
struct TimedLexerResult {
    LexerResult result;
    microseconds duration{};
};

struct ComparisonSummary {
    bool exactMatch = true;
    string detail = "Outputs match exactly";
};

struct FileAnalysisSummary {
    bool completed = false;
    bool exactMatch = false;
};

TimedLexerResult runLexer(ILexer& lexer, const string& source, ErrorMode errorMode) {
    const auto start = high_resolution_clock::now();
    const LexerResult result = lexer.tokenize(source, errorMode);
    const auto end = high_resolution_clock::now();

    return TimedLexerResult{result, duration_cast<microseconds>(end - start)};
}

string describeToken(const Token& token) {
    return token.start.toString() + " " + token.toString();
}

string describeDiagnostic(const Diagnostic& diagnostic) {
    return diagnostic.toString();
}

bool tokensEqual(const Token& left, const Token& right) {
    return left.type == right.type
        && left.lexeme == right.lexeme
        && left.start.index == right.start.index
        && left.start.line == right.start.line
        && left.start.col == right.start.col
        && left.end.index == right.end.index
        && left.end.line == right.end.line
        && left.end.col == right.end.col;
}

bool diagnosticsEqual(const Diagnostic& left, const Diagnostic& right) {
    return left.code == right.code
        && left.message == right.message
        && left.lexeme == right.lexeme
        && left.start.index == right.start.index
        && left.start.line == right.start.line
        && left.start.col == right.start.col
        && left.end.index == right.end.index
        && left.end.line == right.end.line
        && left.end.col == right.end.col;
}

ComparisonSummary compareResults(const LexerResult& fsmResult, const LexerResult& regexResult) {
    if (fsmResult.tokens.size() != regexResult.tokens.size()) {
        return ComparisonSummary{
            false,
            "Token streams differ in size: FSM=" + to_string(fsmResult.tokens.size())
                + ", Regex=" + to_string(regexResult.tokens.size())};
    }

    for (size_t index = 0; index < fsmResult.tokens.size(); ++index) {
        if (!tokensEqual(fsmResult.tokens[index], regexResult.tokens[index])) {
            return ComparisonSummary{
                false,
                "First token mismatch at index " + to_string(index)
                    + "\n  FSM: " + describeToken(fsmResult.tokens[index])
                    + "\n  Regex: " + describeToken(regexResult.tokens[index])};
        }
    }

    if (fsmResult.diagnostics.size() != regexResult.diagnostics.size()) {
        return ComparisonSummary{
            false,
            "Diagnostic streams differ in size: FSM=" + to_string(fsmResult.diagnostics.size())
                + ", Regex=" + to_string(regexResult.diagnostics.size())};
    }

    for (size_t index = 0; index < fsmResult.diagnostics.size(); ++index) {
        if (!diagnosticsEqual(fsmResult.diagnostics[index], regexResult.diagnostics[index])) {
            return ComparisonSummary{
                false,
                "First diagnostic mismatch at index " + to_string(index)
                    + "\n  FSM: " + describeDiagnostic(fsmResult.diagnostics[index])
                    + "\n  Regex: " + describeDiagnostic(regexResult.diagnostics[index])};
        }
    }

    return ComparisonSummary{};
}

void printLexerResult(ostream& out,
                      const string& label,
                      const TimedLexerResult& timedResult,
                      const SourceDocument& document) {
    out << label << " Tokens:\n";
    for (const auto& token : timedResult.result.tokens) {
        out << token.start.toString() << " " << token.toString() << '\n';
    }

    if (!timedResult.result.diagnostics.empty()) {
        out << label << " Diagnostics:\n";
        for (const auto& diagnostic : timedResult.result.diagnostics) {
            out << diagnostic.toString() << '\n';
            const string snippet = document.renderSnippet(diagnostic.start, diagnostic.end);
            if (!snippet.empty()) {
                out << snippet << '\n';
            }
        }
    }

    out << "\n" << label << " Token count: " << timedResult.result.tokens.size() << '\n';
    out << label << " Diagnostic count: " << timedResult.result.diagnostics.size() << '\n';
    out << label << " Time: " << timedResult.duration.count() << " microseconds\n\n";
}

void printComparison(ostream& out,
                     const TimedLexerResult& fsmResult,
                     const TimedLexerResult& regexResult,
                     const ComparisonSummary& comparisonSummary) {
    out << "Comparison\n";
    out << "Token count difference: "
        << static_cast<int>(fsmResult.result.tokens.size())
           - static_cast<int>(regexResult.result.tokens.size())
        << '\n';

    if (comparisonSummary.exactMatch) {
        out << "Outputs match exactly\n";
    } else {
        out << "Outputs differ\n";
        out << comparisonSummary.detail << '\n';
    }

    if (fsmResult.duration.count() == 0) {
        out << "Speed comparison: unavailable\n";
        return;
    }

    const double speedup = static_cast<double>(regexResult.duration.count())
                         / static_cast<double>(fsmResult.duration.count());
    out << "Speed comparison: FSM is " << speedup << "x ";
    if (speedup > 1.0) {
        out << "faster";
    } else {
        out << "slower";
    }
    out << " than Regex\n";
}

FileAnalysisSummary analyzeDocument(const SourceDocument& document,
                                    ErrorMode errorMode,
                                    ILexer& fsmLexer,
                                    ILexer& regexLexer,
                                    ostream& out,
                                    ostream& err) {
    out << "Analyzing file: " << document.path << "\n\n";
    out << "Mode: " << (errorMode == ErrorMode::Throw ? "strict" : "tolerant") << "\n\n";

    out << "FSM Lexer\n";
    TimedLexerResult fsmResult;
    try {
        fsmResult = runLexer(fsmLexer, document.text, errorMode);
    } catch (const LexicalException& exception) {
        err << "FSM lexical error in '" << document.path << "': " << exception.what() << '\n';
        return FileAnalysisSummary{};
    }
    printLexerResult(out, "FSM", fsmResult, document);

    out << "Regex Lexer\n";
    TimedLexerResult regexResult;
    try {
        regexResult = runLexer(regexLexer, document.text, errorMode);
    } catch (const LexicalException& exception) {
        err << "Regex lexical error in '" << document.path << "': " << exception.what() << '\n';
        return FileAnalysisSummary{};
    }
    printLexerResult(out, "Regex", regexResult, document);

    const ComparisonSummary comparisonSummary = compareResults(fsmResult.result, regexResult.result);
    printComparison(out, fsmResult, regexResult, comparisonSummary);
    return FileAnalysisSummary{true, comparisonSummary.exactMatch};
}
} // namespace

LexerComparisonApp::LexerComparisonApp(ISourceRepository& sourceRepository, ILexer& fsmLexer, ILexer& regexLexer)
    : sourceRepository(sourceRepository),
      fsmLexer(fsmLexer),
      regexLexer(regexLexer) {
}

int LexerComparisonApp::run(const string& inputFile,
                            ErrorMode errorMode,
                            ostream& out,
                            ostream& err) {
    return run(vector<string>{inputFile}, errorMode, out, err);
}

int LexerComparisonApp::run(const vector<string>& inputs,
                            ErrorMode errorMode,
                            ostream& out,
                            ostream& err) {
    vector<string> resolvedInputs;
    try {
        resolvedInputs = sourceRepository.resolveInputs(inputs);
    } catch (const runtime_error& exception) {
        err << "Error: " << exception.what() << '\n';
        return 1;
    }

    if (resolvedInputs.empty()) {
        err << "Error: no input files were found\n";
        return 1;
    }

    size_t exactMatches = 0;
    size_t mismatches = 0;
    size_t failures = 0;

    for (size_t index = 0; index < resolvedInputs.size(); ++index) {
        if (index != 0) {
            out << "\n";
        }

        const string& path = resolvedInputs[index];
        SourceDocument document;
        try {
            document = sourceRepository.load(path);
        } catch (const runtime_error& exception) {
            err << "Error loading '" << path << "': " << exception.what() << '\n';
            ++failures;
            continue;
        }

        const FileAnalysisSummary summary = analyzeDocument(document, errorMode, fsmLexer, regexLexer, out, err);
        if (!summary.completed) {
            ++failures;
            continue;
        }

        if (summary.exactMatch) {
            ++exactMatches;
        } else {
            ++mismatches;
        }
    }

    if (resolvedInputs.size() > 1) {
        out << "\nBatch Summary\n";
        out << "Files analyzed: " << resolvedInputs.size() << '\n';
        out << "Exact matches: " << exactMatches << '\n';
        out << "Files with differences: " << mismatches << '\n';
        out << "Files with failures: " << failures << '\n';
    }

    return (mismatches == 0 && failures == 0) ? 0 : 1;
}
