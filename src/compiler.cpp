#include <codecvt>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <memory>
#include <vector>

#include "ast.hpp"
#include "codegen.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"

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

  std::string llFileName = inputFilePath.replace_extension("ll");
  std::string objFileName = inputFilePath.replace_extension("o");
  std::string outputFile = inputFilePath.replace_extension();

  // prepare .ll file
  std::error_code OutErrorInfo;
  std::error_code ok;
  llvm::raw_fd_ostream llFile(llFileName, OutErrorInfo, llvm::sys::fs::OF_None);
  if (OutErrorInfo != ok) {
    std::cerr << "Unable to create .ll file" << std::endl;
    std::exit(1);
  }

  // stub main
  // llvm::LLVMContext llvmCtxt;
  // llvm::Module llvmModule("foo", llvmCtxt);
  // llvm::IRBuilder<> llvmIRBuilder(llvmCtxt);
  // auto *funcType = llvm::FunctionType::get(llvmIRBuilder.getInt32Ty(),
  // false); auto *mainFunc = llvm::Function::Create(
  //    funcType, llvm::Function::ExternalLinkage, "main", llvmModule);
  // auto *entry = llvm::BasicBlock::Create(llvmCtxt, "entrypoint", mainFunc);
  // llvmIRBuilder.SetInsertPoint(entry);
  // llvmIRBuilder.CreateRet(llvmIRBuilder.getInt32(0));
  // llvmModule.print(llFile, nullptr);

  std::vector<AstNodePtr> declarations;
  declarations.push_back(
      std::make_shared<FnDefNode>("main", UzType{UzTypeId::Void}, nullptr));
  auto root = std::make_shared<RootNode>(declarations);
  CodeGenerator c;
  if (!root->codegen(&c)) {
    std::exit(1);
  }
  c.getLLVMModule().print(llFile, nullptr);

  //  auto tokens = tokenize(inputCode);
  //  auto ast = parse(std::move(tokens));
  //  if (ast) {
  //    CodeGenerator generator;
  //    if (auto *code = ast->codegen(&generator)) {
  //      code->print(llFile);
  //    }
  //  }
  // compile to object file
  auto llcCmd = std::string("llc -filetype=obj ") + llFileName;
  (void)std::system(llcCmd.c_str());

  // linker
  std::string linkerCmd =
      std::string("ld -o ") + outputFile +
      " -dynamic-linker /lib64/ld-linux-x86-64.so.2 /usr/lib/crt1.o -lc " +
      objFileName;
  return std::system(linkerCmd.c_str());
}
