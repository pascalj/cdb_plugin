#include <CustomCompilationDatabase.h>
#include <clang/Tooling/CompilationDatabase.h>

static clang::tooling::CompilationDatabasePluginRegistry::Add<
    CustomCompilationDatabasePlugin>
    Y("custom-compilation-database", "Custom database plugin for demo purposes");

namespace clang {
namespace tooling {

// volatile int CustomAnchorSource = 0;

} // namespace tooling
} // namespace clang
