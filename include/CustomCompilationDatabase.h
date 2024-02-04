#pragma once

#include "Index.h"
#include <algorithm>
#include <clang/Tooling/CompilationDatabasePluginRegistry.h>
#include <clang/Tooling/JSONCompilationDatabase.h>

struct CustomCompilationDatabase
    : public clang::tooling::JSONCompilationDatabase {

  std::vector<clang::tooling::CompileCommand>
  getCompileCommands(llvm::StringRef path) const override {
    auto it = std::find_if(getAllCompileCommands().begin(), getAllCompileCommands().end(),
                 [&](auto command) { return command.Filename == path; });
    if (it != getAllCompileCommands().end()) {
        return {*it};
    }

    auto guess =
        getCompileCommands(graphIndex.BestOriginForFile(std::string(path)));
    return {guess};
  }

  Index graphIndex;
};

struct CustomCompilationDatabasePlugin
    : public clang::tooling::CompilationDatabasePlugin {
  std::unique_ptr<clang::tooling::CompilationDatabase>
  loadFromDirectory(llvm::StringRef Directory,
                    std::string &ErrorMessage) override {
    llvm::SmallString<1024> JSONDatabasePath(Directory);
    llvm::sys::path::append(JSONDatabasePath, "compile_commands.jsons");
    return CustomCompilationDatabase::loadFromFile(
        JSONDatabasePath, ErrorMessage,
        clang::tooling::JSONCommandLineSyntax::AutoDetect);
  }
};
