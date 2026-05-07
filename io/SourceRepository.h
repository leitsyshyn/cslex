#pragma once

#include "ISourceRepository.h"

#include <stdexcept>

class SourceRepositoryException : public std::runtime_error {
public:
    explicit SourceRepositoryException(const std::string& message)
        : std::runtime_error(message) {}
};

class SourceRepository : public ISourceRepository {
public:
    std::vector<std::string> resolveInputs(const std::vector<std::string>& inputs) override;
    SourceDocument load(const std::string& path) override;
};
