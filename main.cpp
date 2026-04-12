#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "app/LexerComparisonApp.h"
#include "io/SourceRepository.h"
#include "lexers/fsm/FSMLexer.h"
#include "lexers/regex/RegexLexer.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " [--strict] <input.cs|directory> [more_inputs...]" << endl;
        return 1;
    }

    ErrorMode error_mode = ErrorMode::Collect;
    int first_input_index = 1;

    if (string(argv[1]) == "--strict") {
        if (argc < 3) {
            cerr << "Usage: " << argv[0] << " [--strict] <input.cs|directory> [more_inputs...]" << endl;
            return 1;
        }

        error_mode = ErrorMode::Throw;
        first_input_index = 2;
    } else if (string(argv[1]).rfind("--", 0) == 0) {
        if (string(argv[1]) != "--strict") {
            cerr << "Usage: " << argv[0] << " [--strict] <input.cs|directory> [more_inputs...]" << endl;
            return 1;
        }
    }

    vector<string> inputs;
    for (int index = first_input_index; index < argc; ++index) {
        inputs.push_back(argv[index]);
    }

    SourceRepository source_repository;
    auto fsm_lexer = make_unique<FSMLexer>();
    auto regex_lexer = make_unique<RegexLexer>();
    LexerComparisonApp app(source_repository, *fsm_lexer, *regex_lexer);
    return app.run(inputs, error_mode, cout, cerr);
}
