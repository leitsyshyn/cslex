#pragma once

#include "ISourceRepository.h"

class SourceRepository : public ISourceRepository {
public:
    std::vector<std::string> resolveInputs(const std::vector<std::string>& inputs) override;
    SourceDocument load(const std::string& path) override;
};
