#pragma once

#include <array>
#include <string_view>

constexpr std::array<std::string_view, 3> THREE_CHAR_OPERATORS = {
    "<<=",
    ">>=",
    "\?\?="
};

constexpr std::array<std::string_view, 21> TWO_CHAR_OPERATORS = {
    "==",
    "!=",
    "<=",
    ">=",
    "&&",
    "||",
    "++",
    "--",
    "+=",
    "-=",
    "*=",
    "/=",
    "%=",
    "&=",
    "|=",
    "^=",
    "<<",
    ">>",
    "=>",
    "\?\?",
    "?."
};

constexpr std::string_view SINGLE_CHAR_OPERATORS = "+-*/%=<>!&|^~?:.";
