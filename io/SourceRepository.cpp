#include "SourceRepository.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>

namespace fs = std::filesystem;

namespace {
std::string normalizeSourceText(std::string raw) {
    if (raw.size() >= 3
        && static_cast<unsigned char>(raw[0]) == 0xEF
        && static_cast<unsigned char>(raw[1]) == 0xBB
        && static_cast<unsigned char>(raw[2]) == 0xBF) {
        raw.erase(0, 3);
    }

    std::string normalized;
    normalized.reserve(raw.size());
    std::size_t index = 0;
    while (index < raw.size()) {
        const char c = raw[index++];
        if (c == '\r') {
            normalized.push_back('\n');
            if (index < raw.size() && raw[index] == '\n') {
                ++index;
            }
            continue;
        }

        normalized.push_back(c);
    }

    return normalized;
}

bool isCSharpSource(const fs::path& path) {
    return path.has_extension() && path.extension() == ".cs";
}

void appendCSharpSources(const fs::path& directory, std::vector<std::string>& resolved) {
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && isCSharpSource(entry.path())) {
            resolved.emplace_back(entry.path().string());
        }
    }
}
} // namespace

std::vector<std::string> SourceRepository::resolveInputs(const std::vector<std::string>& inputs) {
    std::vector<std::string> resolved;

    for (const auto& input : inputs) {
        const fs::path path(input);
        if (!fs::exists(path)) {
            throw SourceRepositoryException("Input path '" + input + "' does not exist");
        }

        if (fs::is_directory(path)) {
            appendCSharpSources(path, resolved);
            continue;
        }

        resolved.emplace_back(path.string());
    }

    std::sort(resolved.begin(), resolved.end());
    resolved.erase(std::unique(resolved.begin(), resolved.end()), resolved.end());
    return resolved;
}

SourceDocument SourceRepository::load(const std::string& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input.is_open()) {
        throw SourceRepositoryException("Could not open file '" + path + "'");
    }

    const std::string raw((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    return SourceDocument(path, normalizeSourceText(raw));
}
