#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <string>
#include <chrono>
#include "lexers/ILexer.h"
#include "lexers/fsm/FSMLexer.h"
#include "lexers/regex/RegexLexer.h"

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file.cs>" << endl;
        return 1;
    }
    
    string input_file = argv[1];
    
    ifstream file(input_file);
    if (!file.is_open()) {
        cerr << "Error: Could not open file '" << input_file << "'" << endl;
        return 1;
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    string file_content = buffer.str();
    file.close();
    
    cout << "Analyzing file: " << input_file << "\n\n";
    
    cout << "FSM Lexer\n";
    auto fsm_lexer = make_unique<FSMLexer>();
    auto fsm_start = high_resolution_clock::now();
    vector<Token> fsm_tokens = fsm_lexer->tokenize(file_content);
    auto fsm_end = high_resolution_clock::now();
    auto fsm_duration = duration_cast<microseconds>(fsm_end - fsm_start);
    
    cout << "FSM Tokens:\n";
    for (const auto& token : fsm_tokens) {
        cout << token.start.toString() << " " << token.toString() << endl;
    }
    cout << "\nFSM Token count: " << fsm_tokens.size() << endl;
    cout << "FSM Time: " << fsm_duration.count() << " microseconds\n\n";
    
    cout << "Regex Lexer\n";
    auto regex_lexer = make_unique<RegexLexer>();
    auto regex_start = high_resolution_clock::now();
    vector<Token> regex_tokens = regex_lexer->tokenize(file_content);
    auto regex_end = high_resolution_clock::now();
    auto regex_duration = duration_cast<microseconds>(regex_end - regex_start);
    
    cout << "Regex Tokens:\n";
    for (const auto& token : regex_tokens) {
        cout << token.start.toString() << " " << token.toString() << endl;
    }
    cout << "\nRegex Token count: " << regex_tokens.size() << endl;
    cout << "Regex Time: " << regex_duration.count() << " microseconds\n\n";
    
    cout << "Comparison\n";
    cout << "Token count difference: " << static_cast<int>(fsm_tokens.size()) - static_cast<int>(regex_tokens.size()) << endl;
    if (fsm_tokens.size() == regex_tokens.size()) {
        cout << "Both lexers produced the same number of tokens" << endl;
    } else {
        cout << "Token counts differ" << endl;
    }
    
    double speedup = static_cast<double>(regex_duration.count()) / static_cast<double>(fsm_duration.count());
    cout << "Speed comparison: FSM is " << speedup << "x ";
    if (speedup > 1.0) {
        cout << "faster";
    } else {
        cout << "slower";
    }
    cout << " than Regex" << endl;
    
    return 0;
}