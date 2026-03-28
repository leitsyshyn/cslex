cmake_minimum_required(VERSION 3.10)

project(cslex VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(cslex_lib
    Token.h
    Position.h
    Keywords.h
    Operators.h
    lexers/ILexer.h
    lexers/Diagnostic.h
    lexers/LexerResult.h
    lexers/LexicalException.h
    lexers/LexicalRules.h
    lexers/fsm/FSMLexer.cpp
    lexers/fsm/FSMLexer.h
    lexers/fsm/FsmRuleRegistry.h
    lexers/fsm/InputBuffer.cpp
    lexers/fsm/InputBuffer.h
    lexers/fsm/processors/IProcessor.h
    lexers/fsm/processors/IdentifierProcessor.h
    lexers/fsm/processors/NumberProcessor.h
    lexers/fsm/processors/StringProcessor.h
    lexers/fsm/processors/VerbatimStringProcessor.h
    lexers/fsm/processors/InterpolatedStringProcessor.h
    lexers/fsm/processors/CharProcessor.h
    lexers/fsm/processors/PreprocessorProcessor.h
    lexers/fsm/processors/CommentProcessor.h
    lexers/fsm/processors/OperatorProcessor.h
    lexers/fsm/processors/SeparatorProcessor.h
    lexers/regex/RegexLexer.cpp
    lexers/regex/RegexLexer.h
    lexers/regex/RegexPatterns.h
)

add_executable(cslex
    main.cpp
)

target_link_libraries(cslex PRIVATE cslex_lib)

set_target_properties(cslex PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(MSVC)
    target_compile_options(cslex_lib PRIVATE /W4)
    target_compile_options(cslex PRIVATE /W4)
else()
    target_compile_options(cslex_lib PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cslex PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_options(cslex_lib PRIVATE /O2)
        target_compile_options(cslex PRIVATE /O2)
    else()
        target_compile_options(cslex_lib PRIVATE -O3)
        target_compile_options(cslex PRIVATE -O3)
    endif()
endif()

# Test runner executable
add_executable(test_runner
    tests/test_runner.cpp
)

target_link_libraries(test_runner PRIVATE cslex_lib)

set_target_properties(test_runner PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(MSVC)
    target_compile_options(test_runner PRIVATE /W4)
else()
    target_compile_options(test_runner PRIVATE -Wall -Wextra -Wpedantic)
endif()
