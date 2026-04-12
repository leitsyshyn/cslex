#pragma once

#include "SourceDocument.h"

#include <string>
#include <vector>

class ISourceRepository {
public:
    virtual ~ISourceRepository() = default;
    virtual std::vector<std::string> resolveInputs(const std::vector<std::string>& inputs) = 0;
    virtual SourceDocument load(const std::string& path) = 0;
};
