#pragma once

#include "graaflib/types.h"
#include <clang/Tooling/CommonOptionsParser.h>

#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/DependencyScanning/DependencyScanningService.h>
#include <clang/Tooling/DependencyScanning/DependencyScanningTool.h>
#include <clang/Tooling/DependencyScanning/DependencyScanningWorker.h>
#include <clang/Tooling/DependencyScanning/ModuleDepCollector.h>
#include <clang/Tooling/Tooling.h>
#include <graaflib/graph.h>

#include <filesystem>

class Index {
public:
  Index(const clang::tooling::CompilationDatabase &db)
      : db(db), commands(db.getAllCompileCommands()),
        scanningService(
            clang::tooling::dependencies::ScanningMode::
                DependencyDirectivesScan,
            clang::tooling::dependencies::ScanningOutputFormat::Full),
        scanningTool(scanningService) {}

  llvm::Error
  BuildDependencyGraph(const std::filesystem::path &workingDirectory);
  graaf::vertex_id_t
  NearestFileInGraph(const std::filesystem::path &path) const;
  std::string
  BestOriginForFile(const std::filesystem::path &path) const;

private:
  const clang::tooling::CompilationDatabase &db;
  std::vector<clang::tooling::CompileCommand> commands;

  graaf::directed_graph<std::string, int> includeGraph;
  std::unordered_map<std::string, graaf::vertex_id_t> vertexMap;

  int64_t Distance(std::filesystem::path from, std::filesystem::path to) const;

  llvm::StringSet<> alreadySeen;
  clang::tooling::dependencies::DependencyScanningService scanningService;
  clang::tooling::dependencies::DependencyScanningTool scanningTool;
};
