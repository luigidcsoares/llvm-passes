//===-- InstCount.h ---------------------------------------------*- C++ -*-===//
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
/// This file contains the declaration of the InstCount analysis pass.
///
//===----------------------------------------------------------------------===//
#ifndef LLVM_INSTCOUNT_H
#define LLVM_INSTCOUNT_H

#include <llvm/IR/PassManager.h>

namespace instcount {
/// A pass that visit all functions inside a module to count the number of
/// instructions it contains. This analysis also compute the total number
/// of instructions for the entire module.
struct Pass : public llvm::AnalysisInfoMixin<Pass> {
    /// Map between a function and its number of instructions.
    using CountMap = llvm::DenseMap<const llvm::Function *, unsigned>;
    /// Result of the analysis: a pair containin the count for each function
    /// (CountMap) as well as the count for the entire module.
    using Result = std::pair<CountMap, unsigned>;

    /// Traverses the module \p M, computing the number of instructions for each
    /// function.
    ///
    /// \returns a map between a function and its number of instructions, plus
    /// the total for the entire module.
    Result run(llvm::Module &M, llvm::ModuleAnalysisManager &MAM);

    // A special type used by analysis passes to provide an address that
    // identifies that particular analysis pass type.
    static llvm::AnalysisKey Key;
};

/// Count the number of instructions of a given function \p F.
///
/// \returns the total number of instructions of \F.
unsigned countInst(const llvm::Function &F);
} // namespace instcount

#endif
