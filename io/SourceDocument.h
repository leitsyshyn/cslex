#pragma once

#include "Position.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

struct SourceDocument {
    std::string path;
    std::string text;
    std::vector<std::string> lines;

    SourceDocument() = default;

    SourceDocument(std::string pathValue, std::string textValue)
        : path(std::move(pathValue)),
          text(std::move(textValue)),
          lines(splitLines(text)) {
    }

    bool hasLine(int lineNumber) const {
        return lineNumber >= 1 && static_cast<std::size_t>(lineNumber) <= lines.size();
    }

    std::string lineText(int lineNumber) const {
        if (!hasLine(lineNumber)) {
            return "";
        }

        return lines[static_cast<std::size_t>(lineNumber - 1)];
    }

    std::string renderSnippet(const Position& start, const Position& end) const {
        const std::string line = lineText(start.line);
        if (!hasLine(start.line)) {
            return "";
        }

        const std::size_t lineLength = line.size();
        const std::size_t zeroBasedColumn = start.col > 1 ? static_cast<std::size_t>(start.col - 1) : 0u;
        const std::size_t caretColumn = std::min(zeroBasedColumn, lineLength);

        std::size_t caretWidth = 1;
        if (start.line == end.line && end.col > start.col) {
            caretWidth = static_cast<std::size_t>(end.col - start.col);
        }

        if (lineLength >= caretColumn) {
            const std::size_t remaining = lineLength > caretColumn ? lineLength - caretColumn : 1u;
            caretWidth = std::min(caretWidth, remaining);
        }

        return line + "\n" + std::string(caretColumn, ' ') + std::string(std::max<std::size_t>(1u, caretWidth), '^');
    }

private:
    static std::vector<std::string> splitLines(const std::string& value) {
        std::vector<std::string> result(1);

        for (char c : value) {
            if (c == '\n') {
                result.emplace_back();
                continue;
            }

            result.back().push_back(c);
        }

        return result;
    }
};
