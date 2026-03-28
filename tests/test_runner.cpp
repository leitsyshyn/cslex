#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <memory>
#include "../lexers/ILexer.h"
#include "../lexers/fsm/FSMLexer.h"
#include "../lexers/regex/RegexLexer.h"

using namespace std;

void test_lexer(ILexer* lexer, const string& input, const vector<pair<TokenType, string>>& expected, const string& test_name) {
    auto result = lexer->tokenize(input);
    const auto& tokens = result.tokens;
    
    vector<Token> filtered;
    for (const auto& token : tokens) {
        if (token.type != TokenType::WHITESPACE && token.type != TokenType::END_OF_FILE) {
            filtered.push_back(token);
        }
    }
    
    if (filtered.size() != expected.size()) {
        cerr << "FAIL: " << test_name << endl;
        cerr << "  Expected " << expected.size() << " tokens, got " << filtered.size() << endl;
        for (size_t i = 0; i < filtered.size(); i++) {
            cerr << "  " << i << ": " << filtered[i].tokenTypeToString() << " '" << filtered[i].lexeme << "'" << endl;
        }
        assert(false);
    }
    
    for (size_t i = 0; i < expected.size(); i++) {
        if (filtered[i].type != expected[i].first || filtered[i].lexeme != expected[i].second) {
            cerr << "FAIL: " << test_name << " at token " << i << endl;
            cerr << "  Expected: " << Token(expected[i].first, expected[i].second, Position(), Position()).tokenTypeToString() 
                 << " '" << expected[i].second << "'" << endl;
            cerr << "  Got: " << filtered[i].tokenTypeToString() << " '" << filtered[i].lexeme << "'" << endl;
            assert(false);
        }
    }
    
    cout << "PASS: " << test_name << endl;
}

void test_integers(ILexer* lexer, const string& lexer_name) {
    test_lexer(lexer, "123", {{TokenType::INTEGER_LITERAL, "123"}}, lexer_name + " - decimal integer");
    test_lexer(lexer, "0x1A2B", {{TokenType::INTEGER_LITERAL, "0x1A2B"}}, lexer_name + " - hex integer");
    test_lexer(lexer, "0b1010", {{TokenType::INTEGER_LITERAL, "0b1010"}}, lexer_name + " - binary integer");
    test_lexer(lexer, "123U", {{TokenType::INTEGER_LITERAL, "123U"}}, lexer_name + " - unsigned integer");
    test_lexer(lexer, "123L", {{TokenType::INTEGER_LITERAL, "123L"}}, lexer_name + " - long integer");
    test_lexer(lexer, "123UL", {{TokenType::INTEGER_LITERAL, "123UL"}}, lexer_name + " - unsigned long");
    if (lexer_name.find("FSM") != string::npos) {
        test_lexer(lexer, "1_000_000", {{TokenType::INTEGER_LITERAL, "1_000_000"}}, lexer_name + " - integer with underscores");
    } else {
        test_lexer(lexer, "1_000_000", {{TokenType::INTEGER_LITERAL, "1_000_000"}}, lexer_name + " - integer with underscores");
    }
}

void test_reals(ILexer* lexer, const string& lexer_name) {
    test_lexer(lexer, "3.14", {{TokenType::REAL_LITERAL, "3.14"}}, lexer_name + " - simple decimal");
    test_lexer(lexer, "3.14F", {{TokenType::REAL_LITERAL, "3.14F"}}, lexer_name + " - float suffix");
    test_lexer(lexer, "3.14D", {{TokenType::REAL_LITERAL, "3.14D"}}, lexer_name + " - double suffix");
    test_lexer(lexer, "3.14M", {{TokenType::REAL_LITERAL, "3.14M"}}, lexer_name + " - decimal suffix");
    test_lexer(lexer, "1.5e10", {{TokenType::REAL_LITERAL, "1.5e10"}}, lexer_name + " - exponential notation");
    test_lexer(lexer, "1.5E-10", {{TokenType::REAL_LITERAL, "1.5E-10"}}, lexer_name + " - negative exponent");
    test_lexer(lexer, "123F", {{TokenType::REAL_LITERAL, "123F"}}, lexer_name + " - integer with float suffix");
}

void test_strings(ILexer* lexer, const string& lexer_name) {
    test_lexer(lexer, R"("hello")", {{TokenType::STRING_LITERAL, R"("hello")"}}, lexer_name + " - simple string");
    test_lexer(lexer, R"("hello\nworld")", {{TokenType::STRING_LITERAL, R"("hello\nworld")"}}, lexer_name + " - string with escape");
    test_lexer(lexer, R"(@"hello")", {{TokenType::VERBATIM_STRING, R"(@"hello")"}}, lexer_name + " - verbatim string");
    test_lexer(lexer, R"(@"hello""world")", {{TokenType::VERBATIM_STRING, R"(@"hello""world")"}}, lexer_name + " - verbatim with escaped quote");
    test_lexer(lexer, R"($"hello")", {{TokenType::INTERPOLATED_STRING, R"($"hello")"}}, lexer_name + " - interpolated string");
    test_lexer(lexer, R"($@"hello")", {{TokenType::INTERPOLATED_STRING, R"($@"hello")"}}, lexer_name + " - interpolated verbatim");
}

void test_characters(ILexer* lexer, const string& lexer_name) {
    test_lexer(lexer, "'a'", {{TokenType::CHARACTER_LITERAL, "'a'"}}, lexer_name + " - simple char");
    test_lexer(lexer, R"('\n')", {{TokenType::CHARACTER_LITERAL, R"('\n')"}}, lexer_name + " - escaped char");
    test_lexer(lexer, R"('\'')", {{TokenType::CHARACTER_LITERAL, R"('\'')"}}, lexer_name + " - escaped quote");
}

void test_identifiers(ILexer* lexer, const string& lexer_name) {
    test_lexer(lexer, "class", {{TokenType::KEYWORD, "class"}}, lexer_name + " - keyword");
    test_lexer(lexer, "myVar", {{TokenType::IDENTIFIER, "myVar"}}, lexer_name + " - identifier");
    test_lexer(lexer, "_myVar123", {{TokenType::IDENTIFIER, "_myVar123"}}, lexer_name + " - identifier with underscore");
    test_lexer(lexer, "@class", {{TokenType::IDENTIFIER, "@class"}}, lexer_name + " - verbatim identifier");
    test_lexer(lexer, "if", {{TokenType::KEYWORD, "if"}}, lexer_name + " - if keyword");
    test_lexer(lexer, "foreach", {{TokenType::KEYWORD, "foreach"}}, lexer_name + " - foreach keyword");
}

void test_operators(ILexer* lexer, const string& lexer_name) {
    test_lexer(lexer, "+", {{TokenType::OPERATOR, "+"}}, lexer_name + " - plus");
    test_lexer(lexer, "++", {{TokenType::OPERATOR, "++"}}, lexer_name + " - increment");
    test_lexer(lexer, "+=", {{TokenType::OPERATOR, "+="}}, lexer_name + " - plus equals");
    test_lexer(lexer, "==", {{TokenType::OPERATOR, "=="}}, lexer_name + " - equals");
    test_lexer(lexer, "!=", {{TokenType::OPERATOR, "!="}}, lexer_name + " - not equals");
    test_lexer(lexer, "&&", {{TokenType::OPERATOR, "&&"}}, lexer_name + " - logical AND");
    test_lexer(lexer, "||", {{TokenType::OPERATOR, "||"}}, lexer_name + " - logical OR");
    test_lexer(lexer, "?" "?", {{TokenType::OPERATOR, "?" "?"}}, lexer_name + " - null coalescing");
    test_lexer(lexer, "?" "?=", {{TokenType::OPERATOR, "?" "?="}}, lexer_name + " - null coalescing assignment");
    test_lexer(lexer, "?.", {{TokenType::OPERATOR, "?."}}, lexer_name + " - null conditional");
    test_lexer(lexer, "=>", {{TokenType::OPERATOR, "=>"}}, lexer_name + " - lambda arrow");
    test_lexer(lexer, "<<=", {{TokenType::OPERATOR, "<<="}}, lexer_name + " - left shift assignment");
    test_lexer(lexer, ">>=", {{TokenType::OPERATOR, ">>="}}, lexer_name + " - right shift assignment");
}

void test_separators(ILexer* lexer, const string& lexer_name) {
    test_lexer(lexer, "(", {{TokenType::SEPARATOR, "("}}, lexer_name + " - left paren");
    test_lexer(lexer, ")", {{TokenType::SEPARATOR, ")"}}, lexer_name + " - right paren");
    test_lexer(lexer, "{", {{TokenType::SEPARATOR, "{"}}, lexer_name + " - left brace");
    test_lexer(lexer, "}", {{TokenType::SEPARATOR, "}"}}, lexer_name + " - right brace");
    test_lexer(lexer, "[", {{TokenType::SEPARATOR, "["}}, lexer_name + " - left bracket");
    test_lexer(lexer, "]", {{TokenType::SEPARATOR, "]"}}, lexer_name + " - right bracket");
    test_lexer(lexer, ";", {{TokenType::SEPARATOR, ";"}}, lexer_name + " - semicolon");
    test_lexer(lexer, ",", {{TokenType::SEPARATOR, ","}}, lexer_name + " - comma");
}

void test_comments(ILexer* lexer, const string& lexer_name) {
    test_lexer(lexer, "// comment", {{TokenType::COMMENT, "// comment"}}, lexer_name + " - single-line comment");
    test_lexer(lexer, "/* comment */", {{TokenType::COMMENT, "/* comment */"}}, lexer_name + " - multi-line comment");
    test_lexer(lexer, "/* multi\nline\ncomment */", {{TokenType::COMMENT, "/* multi\nline\ncomment */"}}, lexer_name + " - actual multi-line comment");
}

void test_preprocessor(ILexer* lexer, const string& lexer_name) {
    test_lexer(lexer, "#define DEBUG", {{TokenType::PREPROCESSOR, "#define DEBUG"}}, lexer_name + " - define directive");
    test_lexer(lexer, "#if DEBUG", {{TokenType::PREPROCESSOR, "#if DEBUG"}}, lexer_name + " - if directive");
    test_lexer(lexer, "#endif", {{TokenType::PREPROCESSOR, "#endif"}}, lexer_name + " - endif directive");
}

void test_edge_cases(ILexer* lexer, const string& lexer_name) {
    test_lexer(lexer, "a+++++b", {
        {TokenType::IDENTIFIER, "a"},
        {TokenType::OPERATOR, "++"},
        {TokenType::OPERATOR, "++"},
        {TokenType::OPERATOR, "+"},
        {TokenType::IDENTIFIER, "b"}
    }, lexer_name + " - adjacent operators");
    
    test_lexer(lexer, "x+123", {
        {TokenType::IDENTIFIER, "x"},
        {TokenType::OPERATOR, "+"},
        {TokenType::INTEGER_LITERAL, "123"}
    }, lexer_name + " - operator and number");
    
    test_lexer(lexer, R"("")", {{TokenType::STRING_LITERAL, R"("")"}}, lexer_name + " - empty string");
    
    test_lexer(lexer, "int x = 42;", {
        {TokenType::KEYWORD, "int"},
        {TokenType::IDENTIFIER, "x"},
        {TokenType::OPERATOR, "="},
        {TokenType::INTEGER_LITERAL, "42"},
        {TokenType::SEPARATOR, ";"}
    }, lexer_name + " - variable declaration");
    
    test_lexer(lexer, "x?.ToString() ?? \"null\"", {
        {TokenType::IDENTIFIER, "x"},
        {TokenType::OPERATOR, "?."},
        {TokenType::IDENTIFIER, "ToString"},
        {TokenType::SEPARATOR, "("},
        {TokenType::SEPARATOR, ")"},
        {TokenType::OPERATOR, "??"},
        {TokenType::STRING_LITERAL, R"("null")"}
    }, lexer_name + " - null conditional with coalescing");
    
    test_lexer(lexer, "123abc", {{TokenType::ERROR, "123abc"}}, lexer_name + " - invalid number with letters");
    test_lexer(lexer, "0x1G", {{TokenType::ERROR, "0x1G"}}, lexer_name + " - invalid hex digit");
    test_lexer(lexer, "3.14abc", {{TokenType::ERROR, "3.14abc"}}, lexer_name + " - invalid float with letters");
}

int main() {
    cout << "Running Lexer Tests\n" << endl;
    
    auto fsm_lexer = make_unique<FSMLexer>();
    auto regex_lexer = make_unique<RegexLexer>();
    
    cout << "FSM Lexer Tests" << endl;
    test_integers(fsm_lexer.get(), "FSM");
    test_reals(fsm_lexer.get(), "FSM");
    test_strings(fsm_lexer.get(), "FSM");
    test_characters(fsm_lexer.get(), "FSM");
    test_identifiers(fsm_lexer.get(), "FSM");
    test_operators(fsm_lexer.get(), "FSM");
    test_separators(fsm_lexer.get(), "FSM");
    test_comments(fsm_lexer.get(), "FSM");
    test_preprocessor(fsm_lexer.get(), "FSM");
    test_edge_cases(fsm_lexer.get(), "FSM");
    
    cout << "\nRegex Lexer Tests" << endl;
    test_integers(regex_lexer.get(), "Regex");
    test_reals(regex_lexer.get(), "Regex");
    test_strings(regex_lexer.get(), "Regex");
    test_characters(regex_lexer.get(), "Regex");
    test_identifiers(regex_lexer.get(), "Regex");
    test_operators(regex_lexer.get(), "Regex");
    test_separators(regex_lexer.get(), "Regex");
    test_comments(regex_lexer.get(), "Regex");
    test_preprocessor(regex_lexer.get(), "Regex");
    test_edge_cases(regex_lexer.get(), "Regex");
    
    cout << "\nAll tests passed" << endl;
    return 0;
}
