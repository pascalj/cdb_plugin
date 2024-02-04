cmake_minimum_required(VERSION 3.10)
project(outputcc)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find LLVM and Clang
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

# Add LLVM definitions
add_definitions(${LLVM_DEFINITIONS})

# Create Executable
add_executable(outputcc main.cpp src/Index.cpp src/CustomCompilationDatabase.cpp)
target_include_directories(outputcc PUBLIC ${LLVM_INCLUDE_DIRS})
target_include_directories(outputcc PUBLIC "include/")
target_include_directories(outputcc PUBLIC ${CLANG_INCLUDE_DIRS})
target_include_directories(outputcc PUBLIC ${GRAAF_INCLUDE_DIRS})

target_compile_options(outputcc PRIVATE -Werror -Wall -Wextra -pedantic-errors -Wconversion -Wsign-conversion)


# Link against LLVM and Clang libraries
llvm_map_components_to_libnames(llvm_libs support core irreader)

install(PROGRAMS ${CMAKE_BINARY_DIR}/outputcc DESTINATION bin)

target_link_libraries(outputcc
                      clangTooling
                      clangFrontend
                      clangAST
                      clang-cpp
                      clangASTMatchers)
