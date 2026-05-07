#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

using namespace testing;

namespace {
double readTotalLineCoveragePercent(const std::filesystem::path& summaryPath) {
    std::ifstream input(summaryPath);
    std::string line;

    while (std::getline(input, line)) {
        if (line.rfind("TOTAL", 0) != 0) {
            continue;
        }

        std::istringstream tokens(line);
        std::string token;
        for (int index = 0; index <= 9; ++index) {
            tokens >> token;
        }

        token.pop_back();
        return std::stod(token);
    }

    return -1.0;
}
} // namespace

class CoverageSummaryTests : public Test {
public:
    static void SetUpTestSuite() {
        defaultSummaryPath = "build-coverage/coverage/summary.txt";
    }

    void SetUp() override {
        summaryPath = defaultSummaryPath;
        if (!std::filesystem::exists(summaryPath)) {
            GTEST_SKIP() << "Coverage summary not generated; run the coverage target first";
        }
    }

    static std::filesystem::path defaultSummaryPath;
    std::filesystem::path summaryPath;
};

std::filesystem::path CoverageSummaryTests::defaultSummaryPath;

TEST_F(CoverageSummaryTests, Bonus_ReportsLineCoverageAtOrAboveRequiredThreshold) {
    const double totalLineCoverage = readTotalLineCoveragePercent(summaryPath);

    ASSERT_GE(totalLineCoverage, 0.0);
    EXPECT_GE(totalLineCoverage, 75.0);
}
