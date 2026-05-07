#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "app/LexerComparisonApp.h"
#include "io/SourceRepository.h"
#include "lexers/fsm/FSMLexer.h"
#include "lexers/regex/RegexLexer.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [--strict] <input.cs|directory> [more_inputs...]" << std::endl;
        return 1;
    }

    ErrorMode error_mode = ErrorMode::Collect;
    int first_input_index = 1;

    const std::string firstArgument = argv[1];
    if (firstArgument == "--strict") {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " [--strict] <input.cs|directory> [more_inputs...]" << std::endl;
            return 1;
        }

        error_mode = ErrorMode::Throw;
        first_input_index = 2;
    } else if (firstArgument.rfind("--", 0) == 0) {
        std::cerr << "Usage: " << argv[0] << " [--strict] <input.cs|directory> [more_inputs...]" << std::endl;
        return 1;
    }

    std::vector<std::string> inputs;
    for (int index = first_input_index; index < argc; ++index) {
        inputs.emplace_back(argv[index]);
    }

    SourceRepository source_repository;
    auto fsm_lexer = std::make_unique<FSMLexer>();
    auto regex_lexer = std::make_unique<RegexLexer>();
    LexerComparisonApp app(source_repository, *fsm_lexer, *regex_lexer);
    return app.run(inputs, error_mode, std::cout, std::cerr);
}
