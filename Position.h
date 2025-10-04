#pragma once

#include <string>
#include <iomanip>
#include <sstream>

struct Position {
    int index; 
    int line;
    int col;

    Position() : index(0), line(1), col(1) {}
    
    Position(int idx, int ln, int cl) 
        : index(idx), line(ln), col(cl) {}

    std::string toString() const {
        std::ostringstream oss;
        oss << "Line " << std::setw(4) << line << " Col " << std::setw(3) << col;
        return oss.str();
    }
};
