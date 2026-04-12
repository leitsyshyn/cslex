#include "SourceRepository.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>

using namespace std;
namespace fs = std::filesystem;

namespace {
string normalizeSourceText(string raw) {
    if (raw.size() >= 3
        && static_cast<unsigned char>(raw[0]) == 0xEF
        && static_cast<unsigned char>(raw[1]) == 0xBB
        && static_cast<unsigned char>(raw[2]) == 0xBF) {
        raw.erase(0, 3);
    }

    string normalized;
    normalized.reserve(raw.size());
    for (size_t index = 0; index < raw.size(); ++index) {
        const char c = raw[index];
        if (c == '\r') {
            normalized.push_back('\n');
            if (index + 1 < raw.size() && raw[index + 1] == '\n') {
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
} // namespace

vector<string> SourceRepository::resolveInputs(const vector<string>& inputs) {
    vector<string> resolved;

    for (const auto& input : inputs) {
        const fs::path path(input);
        if (!fs::exists(path)) {
            throw runtime_error("Input path '" + input + "' does not exist");
        }

        if (fs::is_directory(path)) {
            for (const auto& entry : fs::recursive_directory_iterator(path)) {
                if (!entry.is_regular_file()) {
                    continue;
                }

                if (isCSharpSource(entry.path())) {
                    resolved.push_back(entry.path().string());
                }
            }
            continue;
        }

        resolved.push_back(path.string());
    }

    sort(resolved.begin(), resolved.end());
    resolved.erase(unique(resolved.begin(), resolved.end()), resolved.end());
    return resolved;
}

SourceDocument SourceRepository::load(const string& path) {
    ifstream input(path, ios::binary);
    if (!input.is_open()) {
        throw runtime_error("Could not open file '" + path + "'");
    }

    const string raw((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());
    return SourceDocument(path, normalizeSourceText(raw));
}
