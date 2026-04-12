#include "io/SourceDocument.h"
#include "io/SourceRepository.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace testing;
namespace fs = std::filesystem;

namespace {
class TempDirectory {
public:
    TempDirectory()
        : root(fs::temp_directory_path() / ("cslex-tests-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()))) {
        fs::create_directories(root);
    }

    ~TempDirectory() {
        std::error_code error;
        fs::remove_all(root, error);
    }

    const fs::path& path() const {
        return root;
    }

private:
    fs::path root;
};

void writeBinaryFile(const fs::path& path, const std::string& content) {
    fs::create_directories(path.parent_path());
    std::ofstream output(path, std::ios::binary);
    output << content;
}
} // namespace

TEST(SourceRepositoryTests, Load_NormalizesUtf8BomAndNewlines) {
    TempDirectory tempDirectory;
    const fs::path sourcePath = tempDirectory.path() / "normalized.cs";
    writeBinaryFile(sourcePath, std::string("\xEF\xBB\xBF") + "first\r\nsecond\rthird");

    SourceRepository repository;
    const SourceDocument document = repository.load(sourcePath.string());

    EXPECT_EQ(document.text, "first\nsecond\nthird");
    EXPECT_EQ(document.lineText(1), "first");
    EXPECT_EQ(document.lineText(2), "second");
    EXPECT_EQ(document.lineText(3), "third");
}

TEST(SourceDocumentTests, RenderSnippet_ShowsLineAndCaretRange) {
    const SourceDocument document("demo.cs", "int bad = 123abc;");

    EXPECT_EQ(document.renderSnippet(Position(10, 1, 11), Position(16, 1, 17)),
              "int bad = 123abc;\n          ^^^^^^");
}

TEST(SourceRepositoryTests, ResolveInputs_ExpandsDirectoriesRecursivelyAndDeduplicates) {
    TempDirectory tempDirectory;
    const fs::path root = tempDirectory.path();
    writeBinaryFile(root / "a.cs", "int a = 1;");
    writeBinaryFile(root / "nested" / "b.cs", "int b = 2;");
    writeBinaryFile(root / "nested" / "ignore.txt", "skip");

    SourceRepository repository;
    const std::vector<std::string> resolved = repository.resolveInputs({root.string(), (root / "a.cs").string()});

    ASSERT_THAT(resolved, SizeIs(2));
    EXPECT_THAT(resolved, ElementsAre((root / "a.cs").string(), (root / "nested" / "b.cs").string()));
}

TEST(SourceRepositoryTests, Load_ThrowsForMissingFile) {
    SourceRepository repository;

    try {
        (void)repository.load("tests/does_not_exist.cs");
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error& exception) {
        EXPECT_THAT(std::string(exception.what()), HasSubstr("Could not open file 'tests/does_not_exist.cs'"));
    }
}

TEST(SourceRepositoryTests, ResolveInputs_ThrowsForMissingPath) {
    SourceRepository repository;

    try {
        (void)repository.resolveInputs({"tests/does_not_exist_directory"});
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error& exception) {
        EXPECT_THAT(std::string(exception.what()), HasSubstr("Input path 'tests/does_not_exist_directory' does not exist"));
    }
}
