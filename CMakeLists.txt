cmake_minimum_required(VERSION 3.10)

project(javar-plugin-lexical-parser LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE LexicalParserSourceFiles "${PROJECT_SOURCE_DIR}/Src/*.c")
file(GLOB_RECURSE LexicalParserHeaderFiles "${PROJECT_SOURCE_DIR}/Src/*.h")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/Lib")

add_library(parser SHARED
    ${LexicalParserSourceFiles}
    ${LexicalParserHeaderFiles}
)
