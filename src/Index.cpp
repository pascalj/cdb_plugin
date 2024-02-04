#include "Index.h"

#include "graaflib/algorithm/graph_traversal/breadth_first_search.h"
#include "graaflib/types.h"

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Lex/PPCallbacks.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/DependencyScanning/DependencyScanningTool.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/Error.h>

#include <filesystem>
#include <ranges>

using namespace clang;
using namespace clang::tooling;

llvm::Error
Index::BuildDependencyGraph(const std::filesystem::path &workingDirectory) {
  for (const auto &command : db.getAllCompileCommands()) {
    alreadySeen.clear();
    auto dependencies = scanningTool.getTranslationUnitDependencies(
        command.CommandLine, workingDirectory.string(), alreadySeen,
        [](auto, auto) { return ""; });

    if (auto Err = dependencies.takeError()) {
      llvm::handleAllErrors(std::move(Err), [](const llvm::StringError &SE) {
        llvm::errs() << "Error: " << SE.getMessage() << "\n";
      });
      continue;
    }

    if (!vertexMap.contains(command.Filename)) {
      auto commandFilename = command.Filename;
      vertexMap[command.Filename] = includeGraph.add_vertex(commandFilename);
    }

    auto root = vertexMap[command.Filename];

    for (const auto &dependency : dependencies->FileDeps) {
      if (dependency == command.Filename) {
        continue;
      }

      if (!vertexMap.contains(dependency)) {
        auto dependencyFilename = dependency;
        vertexMap[dependency] = includeGraph.add_vertex(dependencyFilename);
      }
      if (!includeGraph.has_edge(root, vertexMap[dependency])) {
        includeGraph.add_edge(root, vertexMap[dependency], 1);
      }
    }
  }

  llvm::outs() << "The graph contains " << includeGraph.vertex_count()
               << " vertices and " << includeGraph.edge_count() << " edges\n";

  return llvm::Error::success();
}

graaf::vertex_id_t
Index::NearestFileInGraph(const std::filesystem::path &path) const {
  auto filteredVertices =
      includeGraph.get_vertices() |
      std::views::filter([&](auto vertex) { return vertex.second != path; });
  return std::ranges::min_element(filteredVertices,
                                  [&](const auto &lhs, const auto &rhs) {
                                    return Distance(lhs.second, path) <
                                           Distance(rhs.second, path);
                                  })
      ->first;
}

std::string
Index::BestOriginForFile(const std::filesystem::path &path) const {
  auto vertex = NearestFileInGraph(path);
  auto file = includeGraph.get_vertex(vertex);
  return file;
}
int64_t Index::Distance(std::filesystem::path from,
                        std::filesystem::path to) const {
  return std::ranges::distance(from.lexically_relative(to));
}
