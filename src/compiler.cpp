#include <filesystem>
#include <fstream>
#include <iostream>

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: unzig <file>.uz" << std::endl;
    std::exit(1);
  }
  std::filesystem::path inputFile = argv[1];

  // TODO: read input file

  llvm::LLVMContext llvmCtxt;
  llvm::Module llvmModule("foo", llvmCtxt);
  llvm::IRBuilder<> llvmIRBuilder(llvmCtxt);

  // empty main
  auto *funcType = llvm::FunctionType::get(llvmIRBuilder.getInt32Ty(), false);
  auto *mainFunc = llvm::Function::Create(
      funcType, llvm::Function::ExternalLinkage, "main", llvmModule);
  auto *entry = llvm::BasicBlock::Create(llvmCtxt, "entrypoint", mainFunc);
  llvmIRBuilder.SetInsertPoint(entry);
  llvmIRBuilder.CreateRet(llvmIRBuilder.getInt32(0));

  // print LLVM IR
  // llvmModule.print(llvm::outs(), nullptr);

  // Initialize the target registry etc.
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();
  auto targetTriple = llvm::sys::getDefaultTargetTriple();
  llvmModule.setTargetTriple(targetTriple);
  std::string error;
  auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
  if (!target) {
    llvm::errs() << error;
    return 1;
  }
  auto targetMachine = target->createTargetMachine(
      targetTriple, "generic", "", llvm::TargetOptions(),
      llvm::Optional<llvm::Reloc::Model>());
  llvmModule.setDataLayout(targetMachine->createDataLayout());

  // prepare object file
  // TODO: temporaty dir
  auto objFilename = "/tmp/unzig_object_file";
  std::error_code EC;
  llvm::raw_fd_ostream dest("/tmp/unzig_object_file", EC,
                            llvm::sys::fs::OF_None);
  if (EC) {
    llvm::errs() << "Could not open file: " << EC.message();
    return 1;
  }

  llvm::legacy::PassManager pass;
  auto fileType = llvm::CGFT_ObjectFile;
  if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
    llvm::errs() << "targetMachine can't emit a file of this type";
    return 1;
  }

  pass.run(llvmModule);
  dest.flush();

  // linker
  std::string outputFile = inputFile.filename().replace_extension();
  std::string linkerCmd =
      std::string("ld -o ") + outputFile +
      " -dynamic-linker /lib64/ld-linux-x86-64.so.2 /usr/lib/crt1.o -lc " +
      objFilename;
  return std::system(linkerCmd.c_str());
}
