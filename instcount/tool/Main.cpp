//===-- Main.cpp ----------------------------------------------------------===//
// Copyright (C) 2020  Luigi D. C. Soares
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//===----------------------------------------------------------------------===//
///
/// \file
/// A command-line tool to count the number of instructions for a given module
/// (either the total and per function).
///
//===----------------------------------------------------------------------===//

#include "InstCount.h"
#include <llvm/IRReader/IRReader.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

/// A category for the options specified for this tool.
static cl::OptionCategory InstCountCategory("invariant pass options");

/// A required argument that specifies the module that will be transformed.
static cl::opt<std::string> InputModule(cl::Positional,
                                        cl::desc("<Module to be analyzed>"),
                                        cl::value_desc("bitcode filename"),
                                        cl::init(""), cl::Required,
                                        cl::cat(InstCountCategory));

namespace {
/// A wrapper to the InstCount analysis so we can get the result and print
/// to the user.
struct InstCountWrapper : public PassInfoMixin<InstCountWrapper> {
    /// Get the result of the instcounst analysis and print to the user.
    ///
    /// \returns all analysis preserved.
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM) {
        auto [ByFunction, Total] = MAM.getResult<instcount::Pass>(M);
        errs() << "Number of instructions per function: \n\n";

        for (auto [F, Count] : ByFunction)
            errs() << "\t" << F->getName() << " => " << Count << "\n";

        errs() << "\nTotal: " << Total << "\n";
        return PreservedAnalyses::all();
    }
};
} // namespace

int main(int Argc, char **Argv) {
    // Hide all options apart from the ones specific to this tool.
    cl::HideUnrelatedOptions(InstCountCategory);

    // Parse the command-line options that should be passed to the invariant
    // pass.
    cl::ParseCommandLineOptions(
        Argc, Argv, "transforms functions into versions that are invariant.\n");

    // Makes sure llvm_shutdown() is called (which cleans up LLVM objects)
    //  http://llvm.org/docs/ProgrammersManual.html#ending-execution-with-llvm-shutdown
    llvm_shutdown_obj SDO;

    // Parse the IR file passed on the command line.
    SMDiagnostic Err;
    LLVMContext Ctx;
    std::unique_ptr<Module> M = parseIRFile(InputModule.getValue(), Err, Ctx);

    if (!M) {
        errs() << "Error reading bitcode file: " << InputModule << "\n";
        Err.print(Argv[0], errs());
        return -1;
    }

    PassBuilder PB;
    ModuleAnalysisManager MAM;

    MAM.registerPass([&] { return instcount::Pass(); });
    PB.registerModuleAnalyses(MAM);

    ModulePassManager MPM;
    MPM.addPass(InstCountWrapper());
    MPM.run(*M, MAM);

    return 0;
}
