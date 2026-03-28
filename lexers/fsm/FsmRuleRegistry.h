#pragma once

#include "processors/CharProcessor.h"
#include "processors/CommentProcessor.h"
#include "processors/IdentifierProcessor.h"
#include "processors/InterpolatedStringProcessor.h"
#include "processors/IProcessor.h"
#include "processors/NumberProcessor.h"
#include "processors/OperatorProcessor.h"
#include "processors/PreprocessorProcessor.h"
#include "processors/SeparatorProcessor.h"
#include "processors/StringProcessor.h"
#include "processors/VerbatimStringProcessor.h"
#include <memory>
#include <vector>

inline std::vector<std::unique_ptr<IProcessor>> createDefaultFsmProcessors() {
    std::vector<std::unique_ptr<IProcessor>> processors;
    processors.push_back(std::make_unique<PreprocessorProcessor>());
    processors.push_back(std::make_unique<CommentProcessor>());
    processors.push_back(std::make_unique<VerbatimStringProcessor>());
    processors.push_back(std::make_unique<InterpolatedStringProcessor>());
    processors.push_back(std::make_unique<StringProcessor>());
    processors.push_back(std::make_unique<CharProcessor>());
    processors.push_back(std::make_unique<NumberProcessor>());
    processors.push_back(std::make_unique<IdentifierProcessor>());
    processors.push_back(std::make_unique<OperatorProcessor>());
    processors.push_back(std::make_unique<SeparatorProcessor>());
    return processors;
}
