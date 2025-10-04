#include "RegexLexer.h"
#include <regex>

std::vector<Token> RegexLexer::tokenize(const std::string& source) {
    return tokenizeSource(source);
}

vector<Token> RegexLexer::tokenizeSource(const string& source) {
    vector<Token> tokens;
    int line_number = 1;
    int col_number = 1;
    
    size_t cursor = 0;
    while (cursor < source.length()) {
        bool matched = false;
        
        for (const auto& [type, pattern] : REGEX_PATTERNS) {
            smatch match;
            if (regex_search(source.cbegin() + cursor, source.cend(), match, pattern, 
                           regex_constants::match_continuous)) {
                string lexeme = match.str(0);
                TokenType token_type = type;
                
                if (type == TokenType::IDENTIFIER) {
                    string identifier = lexeme;
                    if (identifier[0] == '@') {
                        identifier = identifier.substr(1);
                    } else if (KEYWORDS.find(identifier) != KEYWORDS.end()) {
                        token_type = TokenType::KEYWORD;
                    }
                }
                
                if (type != TokenType::WHITESPACE) {
                    Position start(cursor, line_number, col_number);
                    
                    int end_line = line_number;
                    int end_col = col_number;
                    size_t end_cursor = cursor;
                    for (char c : lexeme) {
                        if (c == '\n') {
                            end_line++;
                            end_col = 1;
                        } else {
                            end_col++;
                        }
                        end_cursor++;
                    }
                    Position end(end_cursor, end_line, end_col);
                    
                    tokens.push_back(Token(token_type, lexeme, start, end));
                }
                
                for (char c : lexeme) {
                    if (c == '\n') {
                        line_number++;
                        col_number = 1;
                    } else {
                        col_number++;
                    }
                }
                
                cursor += lexeme.length();
                matched = true;
                break;
            }
        }
        
        if (!matched) {
            cursor++;
            col_number++;
        }
    }
    
    Position end_pos(cursor, line_number, col_number);
    tokens.push_back(Token(TokenType::END_OF_FILE, "", end_pos, end_pos));
    
    return tokens;
}