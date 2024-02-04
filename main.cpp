#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/CompilationDatabase.h>

#include <llvm/TargetParser/Host.h>

static llvm::cl::OptionCategory ToolCategory("header options");

int main(int argc, const char **argv) {
  auto ExpectedParser =
      clang::tooling::CommonOptionsParser::create(argc, argv, ToolCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  clang::tooling::CommonOptionsParser &op = ExpectedParser.get();

  auto resolvePath = op.getSourcePathList().at(0);
  auto compileCommand =
      op.getCompilations().getCompileCommands(resolvePath).at(0);

  llvm::outs() << "filename: " << compileCommand.Filename << "\n"
               << "output: " << compileCommand.Output << "\n"
               << "heuristic: " << compileCommand.Heuristic << "\n"
               << "command line: ";
  for (const auto &param : compileCommand.CommandLine) {
    llvm::outs() << "\t" << param << "\n";
  }

  return 0;
}
