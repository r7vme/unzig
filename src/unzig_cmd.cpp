#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include "codegen.hpp"
#include "context.hpp"
#include "parser.hpp"
#include "sema.hpp"
#include "source.hpp"
#include "tokenizer.hpp"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/TargetParser/Host.h"

namespace fs = std::filesystem;

std::string readFileIntoString(const std::string &path) {
  std::ifstream input_file(path);
  if (!input_file.is_open()) {
    std::cerr << "Could not open the file - '" << path << "'" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  return std::string((std::istreambuf_iterator<char>(input_file)),
                     std::istreambuf_iterator<char>());
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: unzig <file>.uz" << std::endl;
    std::exit(1);
  }
  fs::path inputFilePath = argv[1];
  if (!fs::exists(inputFilePath)) {
    std::cerr << "File " << inputFilePath << " does not exist" << std::endl;
    std::exit(1);
  }
  std::string inputCode = readFileIntoString(inputFilePath);
  auto source = std::make_shared<SourceObject>(inputCode);

  std::string llFileName = inputFilePath.replace_extension("ll");
  std::string objFileName = inputFilePath.replace_extension("o");
  std::string dotFileName = inputFilePath.replace_extension("dot");
  std::string pngFileName = inputFilePath.replace_extension("png");
  std::string outputFile = inputFilePath.replace_extension();

  // prepare .ll file
  std::error_code OutErrorInfo;
  std::error_code ok;
  llvm::raw_fd_ostream llFile(llFileName, OutErrorInfo, llvm::sys::fs::OF_None);
  if (OutErrorInfo != ok) {
    std::cerr << "Unable to create .ll file" << std::endl;
    std::exit(1);
  }

  auto tokens = tokenize(source);
  auto ast = parse(tokens, source);

  auto cc = createCompilerContext(source);
  SemanticAnalyzer sema(cc);
  ast.sema(&sema);

  // AST graph
  DotGenerator g;
  ast.dotgen(&g);
  std::ofstream ofs(dotFileName);
  ofs << g.getDotOutput();
  ofs.close();
  auto dotCmd = std::string("dot -Tpng ") + dotFileName + " > " + pngFileName;
  if (std::system(dotCmd.c_str()) != 0) {
    std::cerr << "png generation failed" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  Codegen codegen(cc);
  auto code = ast.codegen(&codegen);
  if (!code) {
    std::cerr << "unzig: unable to generate code" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  cc->llvmModule.print(llFile, nullptr);

  auto compileCmd = std::string("clang -o ") + outputFile + " " + llFileName;
  if (std::system(compileCmd.c_str()) != 0) {
    std::cerr << "unzig: compilation failed" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  std::cout << "unzig: compiled " << outputFile << std::endl;
  return EXIT_SUCCESS;
}
